#include "ScorePopupSprite.h"
#include "ResultLayer.h"
#include "DungeonScene.h"
#include "RankingEventResultLayer.h"
#include "HmsCrypto.h"

#define SCORE_LAYER_TAG 1
#define TOTAL_SCORE_LAYER_TAG 2
#define REWARD_LAYER_TAG 11

ScorePopupSprite::ScorePopupSprite()
:_created_next_dungeon(false)
,_now_move_layer(false)
{
}

ScorePopupSprite* ScorePopupSprite::create(Json json)
{
    ScorePopupSprite *pRet = new ScorePopupSprite();
    
    if (pRet && pRet->init(json)) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool ScorePopupSprite::init(Json json)
{
    if(!Layer::init())
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    ratio = 1 ;
    
    this->json = json;
    
    return true;
}

void ScorePopupSprite::onEnter()
{
    Layer::onEnter();
    
    Layer *score_layer = Layer::create();
    this->addChild(score_layer, 1, SCORE_LAYER_TAG);
    
    //背景
    Sprite *img = Sprite::create("init/Dungeon/result/bk_score.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, 400), 0.5, 0.5));
    score_layer->addChild(img, 1,1);
    
    //jsonからデータ取得
    _score = json["score"].int_value();
    ranking_event_stage_id = json["ranking_event_stage_id"].string_value();
    choice_friend_id = json["choice_friend_id"].string_value();

    string label_list_str = json["label_list"].string_value();
    string value_list_str = json["value_list"].string_value();
    HmsVector<string> label_list = split(label_list_str, ":");
    HmsVector<string> value_list = split(value_list_str, ":");
    
    for(int i=0; i<label_list.size(); i++) {
        int y = img->getContentSize().height -103 - i*47;
        
        auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument(label_list.at(i), FONT_NAME_SYSTEM, 24*ratio);
        label1->setPosition(Point(85, y));
        label1->setAnchorPoint(Point(0, 0.5));
        label1->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label1, 1);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(value_list.at(i), FONT_NAME_SYSTEM, 24*ratio);
        label2->setPosition(img->getContentSize().width-85, y);
        label2->setAnchorPoint(Point(1, 0.5));
        label2->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label2, 1);
    }

    
    //白線
    Sprite *line = Sprite::create("init/Dungeon/result/line.png");
    line->setScale(ratio);
    line->setPosition(Point(img->getContentSize().width/2, 104));
    img->addChild(line, 1);
    
    //スコア
    auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument("対戦スコア", FONT_NAME_SYSTEM, 35*ratio);
    label1->setPosition(Point(85, 65));
    label1->setAnchorPoint(Point(0, 0.5));
    label1->enableOutline(Color4B(0, 0, 0, 255), 2);
    img->addChild(label1, 1);
    
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(toString(_score), FONT_NAME_SYSTEM, 35*ratio);
    label2->setPosition(Point(img->getContentSize().width-85, 65));
    label2->setAnchorPoint(Point(1, 0.5));
    label2->enableOutline(Color4B(0, 0, 0, 255), 2);
    img->addChild(label2, 1);
    
    //ボタン
    Sprite *button = Sprite::create("init/Dungeon/result/bt_empty.png");
    button->setScale(ratio);
    button->setAnchorPoint(Point(0.5, 1)) ;
    button->setPosition(Point(img->getContentSize().width/2, -10));
    img->addChild(button, 1, 1);
    
    string text = "トータルスコアへ";
    if(ranking_event_stage_id!="") text = "次のバトルへ";
    label2 = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30*ratio);
    label2->setPosition(button->getContentSize()/2);
    label2->enableOutline(Color4B(0, 0, 0, 255), 2);
    button->addChild(label2, 1);
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ScorePopupSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ScorePopupSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ScorePopupSprite::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ScorePopupSprite::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    HMSLOG("ScorePopupSprite onEnter()");
}

void ScorePopupSprite::getTotalScore()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "battle/get_total_score.php";
    url += "?user_master_id=" + user_master_id;
    
    auto selector = httpresponse_selector(ScorePopupSprite::getTotalScoreCallback);
    HttpConnect::sendHttpRequest((Layer *)this, url, selector);
}

void ScorePopupSprite::getTotalScoreCallback(HttpClient *sender, HttpResponse *response)
{
    //リザルトデータレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s",result.c_str());
    
    string err;
    json = Json::parse(result, err);
    
    Layer *score_layer = (Layer *)this->getChildByTag(SCORE_LAYER_TAG);
    layer_list.insert(score_layer);
    
    //プレゼント数の更新
    updatePresentCount() ;
    
    //*********************************************************************
    //トータルスコアレイヤー
    //*********************************************************************
    Layer *total_score_layer = Layer::create();
    this->addChild(total_score_layer, 1, TOTAL_SCORE_LAYER_TAG);
    
    //背景
    Sprite *img = Sprite::create("init/Dungeon/result/bk_score.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, 400), 0.5, 0.5));
    total_score_layer->addChild(img, 1);
    
    //データ取得
    string score_list_str = json["score_list"].string_value();
    HmsVector<string> score_list = split(score_list_str, ":");
    
    int total_score = 0;
    for(int i=0; i<score_list.size(); i++) {
        int y = img->getContentSize().height - 103 - i*47;
        total_score += fromString<int>(score_list.at(i));
        
        auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument(toString(i+1) + "戦目", FONT_NAME_SYSTEM, 30*ratio);
        label1->setPosition(Point(85, y));
        label1->setAnchorPoint(Point(0, 0.5));
        label1->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label1, 1);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(score_list.at(i), FONT_NAME_SYSTEM, 30*ratio);
        label2->setPosition(Point(img->getContentSize().width-85, y));
        label2->setAnchorPoint(Point(1, 0.5));
        label2->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label2, 1);
    }
    
    //白線
    Sprite *line = Sprite::create("init/Dungeon/result/line.png");
    line->setScale(ratio);
    line->setPosition(Point(img->getContentSize().width/2, 104));
    img->addChild(line, 1);
    
    //合計スコア
    auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument("合計スコア", FONT_NAME_SYSTEM, 35*ratio);
    label1->setPosition(Point(85, 65));
    label1->setAnchorPoint(Point(0, 0.5));
    label1->enableOutline(Color4B(0, 0, 0, 255), 2);
    img->addChild(label1, 1);
    
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(toString(total_score), FONT_NAME_SYSTEM, 35*ratio);
    label2->setPosition(Point(img->getContentSize().width-85, 65));
    label2->setAnchorPoint(Point(1, 0.5));
    label2->enableOutline(Color4B(0, 0, 0, 255), 2);
    img->addChild(label2, 1);
    
    total_score_layer->setPositionX(640*ratio);
    layer_list.insert(total_score_layer);
    
    //*********************************************************************
    //トータルスコアレイヤー終わり
    //*********************************************************************

    //*********************************************************************
    //報酬レイヤー
    //*********************************************************************
    for(int i=0; i<100; i++) {
        string reward_str = json["reward" + toString(i+1)].string_value();
        if(reward_str=="") break;
        
        HmsVector<string> reward_list = split(reward_str, ":");
        string type = reward_list.at(0);
        string id = reward_list.at(1);
        string reward = reward_list.at(2);
        string count = reward_list.at(3);
        string text = reward_list.at(4);
        
        Layer *reward_layer = Layer::create();
        this->addChild(reward_layer, 1, REWARD_LAYER_TAG+i);
        
        //背景
        Sprite *img = Sprite::create("init/Dungeon/result/bk_reward.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 407), 0.5, 0.5));
        reward_layer->addChild(img, 1);
        
        //ラベル
        auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30*ratio);
        label1->setPosition(Point(img->getContentSize().width/2, 207));
        label1->setAnchorPoint(Point(0.5, 0.5));
        label1->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label1, 1);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(reward, FONT_NAME_SYSTEM, 28*ratio);
        label2->setPosition(Point(330, 143));
        label2->setAnchorPoint(Point(0, 0.5));
        label2->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label2, 1);
        
        auto label3 = HMSFontUtil::createWithTTFBySystemFontArgument("×" + count, FONT_NAME_SYSTEM, 28*ratio);
        label3->setPosition(Point(330, 112));
        label3->setAnchorPoint(Point(0, 0.5));
        label3->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label3, 1);
        
        //報酬画像
        string filename = "";
        if(type=="chara") filename = "download/Chara/all400/" + id + ".png";
        else if(type=="gold") filename = "init/Home/Beginner/Clear/reward_l_1.png";
        else if(type=="jewel") filename = "init/Home/Beginner/Clear/reward_l_2.png";
        Sprite *booty = Sprite::create(filename);
        booty->setScale(ratio*0.2);
        booty->setPosition(Point(267, 125));
        img->addChild(booty, 1);
        
        reward_layer->setPositionX(640*ratio);
        layer_list.insert(reward_layer);
    }
    //*********************************************************************
    //報酬レイヤー終わり
    //*********************************************************************

    moveLayer();
}

void ScorePopupSprite::moveLayer()
{
    if(_now_move_layer == false){
        _now_move_layer = true ;
        
        if(layer_index>=layer_list.size()-1) {
            //ページ遷移の処理
            auto scene = NormalScene::createScene("");
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);

            return;
        }
        
        Layer *layer1 = layer_list.at(layer_index);
        Layer *layer2 = layer_list.at(layer_index+1);
        layer_index++;
        
        //アニメーション
        MoveTo *move1 = MoveTo::create(0.2f, Point(-640*ratio, 0));
        DelayTime *delay2 = DelayTime::create(0.2f);
        MoveTo *move2 = MoveTo::create(0.2f, Point(0, 0));
        Sequence *action2 = Sequence::create(delay2, move2, CallFunc::create([this](){ _now_move_layer = false;}), nullptr); //アニメーションが終了した時点でnow_move_layerをfalseに変更

        layer1->runAction(move1);
        layer2->runAction(action2);
    }
    
}

bool ScorePopupSprite::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch ScorePopupSprite!");
    return true;
}

void ScorePopupSprite::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ScorePopupSprite::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_created_next_dungeon) return ;
    
    Layer *score_layer = (Layer *)this->getChildByTag(SCORE_LAYER_TAG);
    Sprite *img = (Sprite *)score_layer->getChildByTag(1)->getChildByTag(1);
    //Rect rect = img->getBoundingBox();
    //if(rect.containsPoint(touch->getLocation())) {
   if(Utility::containsPoint(img, touch->getLocation()))
   {
        HMSLOG("next. ranking_event_stage_id=%s", ranking_event_stage_id.c_str());
        if(ranking_event_stage_id!="") {
            SaveData::saveString("ranking_event_stage_id", ranking_event_stage_id);
            SaveData::saveString("choice_friend_id", choice_friend_id);
            
            RankingEventResultLayer *result_layer = (RankingEventResultLayer *)this->getParent();
            auto dungeon_scene = (DungeonScene *)result_layer->getParent();
            SaveData::saveInt("ranking_event_power", dungeon_scene->player[0]->getNowPower());
            if(dungeon_scene->player[0]->getHp()>0){
                SaveData::saveInt("ranking_event_hp", dungeon_scene->player[0]->getHp());
            }else{//相打ちしたらHP1で勝利ってことに
                dungeon_scene->player[0]->setHp(1);
                SaveData::saveInt("ranking_event_hp", 1);
            }
            string key = "key" + toString(dungeon_scene->player[0]->getNowPower()) + ":" + toString(dungeon_scene->player[0]->getHp());
            string checksum = HmsCrypto::makeCheckSum(key);
            SaveData::saveString("ranking_event_cs", checksum);


            //ダンジョンシーンへの遷移
            _created_next_dungeon = true;
            auto scene = DungeonScene::createScene();
            auto transition = TransitionFade::create(2.0f, scene);
            Director::getInstance()->replaceScene(transition);
            
            return;
        } else {
            HMSLOG("TotalScoreLayer");
            if(get_score_flg==false) {
                get_score_flg = true;
                getTotalScore();
            }
        }
    }
    
    if(get_score_flg==true && layer_index>0) moveLayer();
}

void ScorePopupSprite::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void ScorePopupSprite::updatePresentCount()
{
    //プレゼントカウントの更新
    int total_present_item_count = json["present_count_data"].int_value();
    if(total_present_item_count!=0){
        SaveData::saveInt("present_count", SaveData::loadInt("present_count") + total_present_item_count);
    }
}