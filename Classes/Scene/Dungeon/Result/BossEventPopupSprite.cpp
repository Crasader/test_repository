#include "BossEventPopupSprite.h"
#include "ResultLayer.h"
#include "DungeonScene.h"
#include "RankingEventResultLayer.h"
#include "HmsCrypto.h"
#include "MapScene.h"

#define RESULT_LAYER_TAG 1
#define REWARD_LAYER_TAG 11

BossEventPopupSprite::BossEventPopupSprite()
:_created_next_dungeon(false)
,_now_move_layer(false)
{
}

BossEventPopupSprite* BossEventPopupSprite::create(Json json)
{
    BossEventPopupSprite *pRet = new BossEventPopupSprite();
    
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

bool BossEventPopupSprite::init(Json json)
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

void BossEventPopupSprite::onEnter()
{
    Layer::onEnter();
    
    Layer *result_layer = Layer::create();
    this->addChild(result_layer, 1, RESULT_LAYER_TAG);
    
    //背景
    Sprite *img = Sprite::create("init/Dungeon/result/bk_score_alpha.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, 400), 0.5, 0.5));
    result_layer->addChild(img, 1,1);
    
    //jsonからデータ取得
    boss_event_stage_log_id = json["boss_event_stage_log_id"].string_value();

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
    button->setScale(ratio);
    button->setAnchorPoint(Point(0.5, 1)) ;
    button->setPosition(Point(img->getContentSize().width/2, -10));
    img->addChild(button, 1, 1);
    
    string text = "戦場に戻る";
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30*ratio);
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
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BossEventPopupSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BossEventPopupSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BossEventPopupSprite::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BossEventPopupSprite::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    HMSLOG("BossEventPopupSprite onEnter()");
}

void BossEventPopupSprite::moveLayer()
{
    if(_now_move_layer == false){
        _now_move_layer = true ;
        
        if(layer_index>=layer_list.size()-1) {
            //Mapシーンへの遷移
            auto scene = MapScene::createScene();
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

bool BossEventPopupSprite::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch BossEventPopupSprite!");
    return true;
}

void BossEventPopupSprite::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BossEventPopupSprite::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_created_next_dungeon) return ;
    
    Layer *score_layer = (Layer *)this->getChildByTag(RESULT_LAYER_TAG);
    Sprite *img = (Sprite *)score_layer->getChildByTag(1)->getChildByTag(1);
    //Rect rect = img->getBoundingBox();
    //if(rect.containsPoint(touch->getLocation())) {
   if(Utility::containsPoint(img, touch->getLocation()))
   {
        HMSLOG("next. boss_event_stage_id=%s", boss_event_stage_log_id.c_str());
        if(boss_event_stage_log_id!="") {
            SaveData::saveString("boss_event_stage_log_id", boss_event_stage_log_id);
            SaveData::saveString("choice_friend_id", choice_friend_id);
            
            RankingEventResultLayer *result_layer = (RankingEventResultLayer *)this->getParent();
            auto dungeon_scene = (DungeonScene *)result_layer->getParent();
            SaveData::saveInt("ranking_event_power", dungeon_scene->player[0]->getNowPower());
            SaveData::saveInt("ranking_event_hp", dungeon_scene->player[0]->getHp());
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

void BossEventPopupSprite::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void BossEventPopupSprite::updatePresentCount()
{
    //プレゼントカウントの更新
    int total_present_item_count = json["present_count_data"].int_value();
    if(total_present_item_count!=0){
        SaveData::saveInt("present_count", SaveData::loadInt("present_count") + total_present_item_count);
    }
}
