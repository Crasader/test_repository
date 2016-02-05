#include "ChallengeEventPopupSprite.h"
#include "ResultLayer.h"
#include "DungeonScene.h"
#include "RankingEventResultLayer.h"
#include "HmsCrypto.h"

#define RESULT_LAYER_TAG 1
#define REWARD_LAYER_TAG 11

ChallengeEventPopupSprite::ChallengeEventPopupSprite()
:_created_next_dungeon(false)
,_now_move_layer(false)
{
}

ChallengeEventPopupSprite* ChallengeEventPopupSprite::create(Json json)
{
    ChallengeEventPopupSprite *pRet = new ChallengeEventPopupSprite();
    
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

bool ChallengeEventPopupSprite::init(Json json)
{
    if(!Layer::init())
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    this->json = json;
    
    return true;
}

void ChallengeEventPopupSprite::onEnter()
{
    Layer::onEnter();
    
    //jsonからデータ取得
    challenge_event_stage_id = json["challenge_event_stage_id"].string_value();
    HmsVector<string> reward_text_list = split(json["reward_text"].string_value(), ":");
    string result_type = json["result_type"].string_value();
    
    Layer *result_layer = Layer::create();
    this->addChild(result_layer, 1, RESULT_LAYER_TAG);
    
    //背景
    string filename = "init/Dungeon/result/bk_match_result.png";
    if(challenge_event_stage_id!="" || result_type=="lose") {
        filename = "init/Dungeon/result/bk_score_alpha.png";
    }
    Sprite *img = Sprite::create(filename.c_str());
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, 400), 0.5, 0.5));
    result_layer->addChild(img, 1,1);
    
    //報酬獲得条件テキスト
    for(int i=0; i<reward_text_list.size(); i++) {
        int y = 420 - i*36;
        string text = reward_text_list.at(i);
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24*ratio);
        label->setAnchorPoint(Point(0, 0.5));
        label->setPosition(convertFromDisplaySize(Point(152, y), 0.5, 0.5));
        label->enableOutline(Color4B(0, 0, 0, 255), 2);
        result_layer->addChild(label, 10);
    }

    /*
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
     */
    
    //ボタン
    Sprite *button = Sprite::create("init/Dungeon/result/bt_empty.png");
    button->setAnchorPoint(Point(0.5, 1)) ;
    button->setPosition(Point(img->getContentSize().width/2, -10));
    img->addChild(button, 1, 1);
    
    string text = "次へ";
    if(challenge_event_stage_id!="") text = "次のバトルへ";
    else if(json["reward" + toString(1)].string_value()=="") text = "ホームへ";
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30);
    label2->setPosition(button->getContentSize()/2);
    label2->enableOutline(Color4B(0, 0, 0, 255), 2);
    button->addChild(label2, 1);
    
    //次のレイヤーの準備
    layer_list.insert(result_layer);
    
    //プレゼント数の更新
    updatePresentCount() ;
    
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
        auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30);
        label1->setPosition(Point(img->getContentSize().width/2, 207));
        label1->setAnchorPoint(Point(0.5, 0.5));
        label1->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label1, 1);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(reward, FONT_NAME_SYSTEM, 28);
        label2->setPosition(Point(330, 143));
        label2->setAnchorPoint(Point(0, 0.5));
        label2->enableOutline(Color4B(0, 0, 0, 255), 2);
        img->addChild(label2, 1);
        
        auto label3 = HMSFontUtil::createWithTTFBySystemFontArgument("×" + count, FONT_NAME_SYSTEM, 28);
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
        booty->setScale(0.2);
        booty->setPosition(Point(267, 125));
        img->addChild(booty, 1);
        
        reward_layer->setPositionX(640*ratio);
        layer_list.insert(reward_layer);
    }
    //*********************************************************************
    //報酬レイヤー終わり
    //*********************************************************************
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ChallengeEventPopupSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ChallengeEventPopupSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ChallengeEventPopupSprite::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ChallengeEventPopupSprite::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    HMSLOG("ChallengeEventPopupSprite onEnter()");
}

void ChallengeEventPopupSprite::moveLayer()
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

bool ChallengeEventPopupSprite::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch ChallengeEventPopupSprite!");
    return true;
}

void ChallengeEventPopupSprite::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ChallengeEventPopupSprite::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_created_next_dungeon) return ;
    
    Layer *score_layer = (Layer *)this->getChildByTag(RESULT_LAYER_TAG);
    Sprite *img = (Sprite *)score_layer->getChildByTag(1)->getChildByTag(1);
    //Rect rect = img->getBoundingBox();
    //if(rect.containsPoint(touch->getLocation())) {
   if(Utility::containsPoint(img, touch->getLocation()))
   {
        HMSLOG("next. challenge_event_stage_id=%s", challenge_event_stage_id.c_str());
        if(challenge_event_stage_id!="") {
            SaveData::saveString("challenge_event_stage_id", challenge_event_stage_id);
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
            moveLayer();
            return;
        }
    }
    
    if(layer_index>0) moveLayer();
}

void ChallengeEventPopupSprite::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void ChallengeEventPopupSprite::updatePresentCount()
{
    //プレゼントカウントの更新
    int total_present_item_count = json["present_count_data"].int_value();
    if(total_present_item_count!=0){
        SaveData::saveInt("present_count", SaveData::loadInt("present_count") + total_present_item_count);
    }
}

void ChallengeEventPopupSprite::showRewardFlg()
{
    Layer *result_layer = (Layer *)this->getChildByTag(RESULT_LAYER_TAG);
    HmsVector<string> reward_flg_list = split(json["reward_flg"].string_value(), ":");
    
    //報酬獲得アニメーション
    for(int i=0; i<reward_flg_list.size(); i++) {
        int x = 101;
        int y = 420 - i*36;
        string reward_flg = reward_flg_list.at(i);
        //獲得済の場合
        if(reward_flg=="1") {
            auto clear_sp = Sprite::create("init/Home/Event/ChallengeEvent/clear.png");
            clear_sp->setPosition(convertFromDisplaySize(Point(x, y), 0.5, 0.5));
            clear_sp->setScale(ratio);
            result_layer->addChild(clear_sp, 10);
        }
        //新規獲得の場合
        if(reward_flg=="2") {
            auto clear_sp = Sprite::create("init/Home/Event/ChallengeEvent/clear.png");
            clear_sp->setScale(3.0*ratio);
            clear_sp->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
            result_layer->addChild(clear_sp, 10);
            
            Point target_point = convertFromDisplaySize(Point(x, y), 0.5, 0.5);
            DelayTime *delay = DelayTime::create(1.0f + 0.3f*i);
            MoveTo *move = MoveTo::create(0.3f, target_point);
            ScaleTo *scale = ScaleTo::create(0.3f, ratio);
            RotateTo *rotate = RotateTo::create(0.3f, 360*5);
            Action *action = Spawn::create(move, scale, rotate, NULL);
            CallFunc *expire1 = CallFunc::create([result_layer, target_point, i, clear_sp]()
                                                 {
                                                     //SE
                                                     CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/lvup.mp3",false);
                                                     
                                                     //パーティクル
                                                     ParticleSystemQuad* particle = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                                                     particle->setAutoRemoveOnFinish(true);
                                                     particle->setPosition(target_point);
                                                     particle->setScale(getVisibleRatio());
                                                     result_layer->addChild(particle, 1);
                                                 });//ラムダ式
            clear_sp->runAction(Sequence::create(delay, action, expire1, NULL));
        }
    }
}