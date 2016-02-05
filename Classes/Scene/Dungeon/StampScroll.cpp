#include "StampScroll.h"
#include "StampLayer.h"
#include "DungeonScene.h"
#include "TurnManage.h"
#include "StampMenu.h"

bool StampScroll::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    CCLOG("スタンプテスト");
    ratio = getVisibleRatio();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(StampScroll::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(StampScroll::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(StampScroll::onTouchEnded, this);
    
    listener->setSwallowTouches(false);
    
    //タップイベントの登録
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    _eventDispatcher->addEventListenerWithFixedPriority(listener, -1);
    
    return true;
}

void StampScroll::makeScroll(){
    
    //スタンプラベル
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("喜ぶ", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("疑問", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("悩む", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("焦る", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("泣く", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("驚く", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("ナイス！", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("らぶりー", FONT_NAME_SYSTEM, 23));
    
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("怒る", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("ベー", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("ひらめく", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("よろしく", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("攻撃！", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("防御！", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("失敗した", FONT_NAME_SYSTEM, 23));
    stamp_label.push_back(HMSFontUtil::createWithTTFBySystemFontArgument("またね", FONT_NAME_SYSTEM, 23));
    
    
    int wide_interval = 120;//ボタン同士の左右の間隔
    int height_interval = 135;//ボタン同士の上下の間隔
    int page_interval = 60;//ページの間に追加する間隔
    
    for(int i=1; i<stamp_label.size()+1; i++){
        if(i <= 8){
            if(i<=4){
                stamp_label.at(i-1)->setPosition(Point(-32 + wide_interval*i, 305-60));
            }else{
                stamp_label.at(i-1)->setPosition(Point(-32 + wide_interval*(i-4), 305-height_interval-60));
                //stamp_label.at(i-1)->setPosition(Point(-32 + wide_interval*i + page_interval, 305-60));
            }
        }else{
            if(i<=12){
                stamp_label.at(i-1)->setPosition(Point(-32 + wide_interval*(i-4) + page_interval, 305-60));
            }else{
                stamp_label.at(i-1)->setPosition(Point(-32 + wide_interval*(i-8) + page_interval, 305-height_interval-60));
            }
        }
        stamp_label.at(i-1)->enableGlow(Color4B::BLACK);
        this->addChild(stamp_label.at(i-1),5);
    }
 
    //スタンプボタン
    for(int i=1; i<=16; i++){
        Sprite *temp = Sprite::create("download/Dungeon/bt_emotion" + to_string(i) +".png");
        Sprite *temp2 = Sprite::create("download/Dungeon/bt_emotion" + to_string(i) +".png");
        
        temp2->setColor(Color3B(180, 180, 180));
        
        auto item_temp = MenuItemSprite::create(temp, temp2, CC_CALLBACK_1(StampScroll::onStampButton, this));
        item_temp->setTag(i);
        
        items.pushBack(item_temp);
        
        if(SaveData::loadInt("stamp_mute") == 1){//ミュート状態の時
            item_temp->setColor(Color3B(127,127,127));
            item_temp->setEnabled(false);
        }else{
            item_temp->setColor(Color3B(255,255,255));
            item_temp->setEnabled(true);
        }
        
        if(i <= 8){
            if(i<=4){
                item_temp->setPosition(Point(-32 + wide_interval*i, 305));
            }else{
                item_temp->setPosition(Point(-32 + wide_interval*(i-4), 305-height_interval));
            }
        }else{
            if(i<=12){
                item_temp->setPosition(Point(-32 + wide_interval*(i-4) + page_interval, 305));
            }else{
                item_temp->setPosition(Point(-32 + wide_interval*(i-8) + page_interval, 305-height_interval));
            }
        }
    }
    stamp_menu = StampMenu::createWithArray(items);
    stamp_menu->setPosition(Vec2(0,0));
    addChild(stamp_menu);
}

void StampScroll::onStampButton(Ref *menu){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent()->getParent());
    
    MenuItemSprite *test = static_cast<MenuItemSprite *>(menu);
    
    dung->uiLayer->showStamp(test->getTag(), TurnManage::getPlayerIndex());
    
    map<string , Json> send_map;
    send_map.insert(make_pair("player_no", Json(TurnManage::getPlayerNo())));
    send_map.insert(make_pair("type", Json("stamp")));
    send_map.insert(make_pair("extra_data", Json(test->getTag()-1)));
    dung->send_message_vec.push_back(send_map);//送信待ちベクターに追加
    
    static_cast<StampLayer *>(getParent()->getParent())->close();
    
    dung->uiLayer->stamp_cant_time = 2.0f;//X秒間はスタンプ不可
}

bool StampScroll::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //touch_in_point = touch->getLocation();
    
    if(SaveData::loadInt("stamp_mute") == 0){//ミュート状態でない時
        for(int i=0; i<16; i++){
            if(now_page == 1){
                if(i<8) items.at(i)->setEnabled(true);
                if(i>=8) items.at(i)->setEnabled(false);
            }
            if(now_page == 2){
                if(i<8) items.at(i)->setEnabled(false);
                if(i>=8) items.at(i)->setEnabled(true);
            }
        }
    }
    
    
    
    
    CCLOG("タッチ位置%f",items.at(0)->getPosition().x);
    
    
    return ScrollView::onTouchBegan(touch, event);
}

void StampScroll::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //setContentOffset(getPosition().x + touch->getLocation().x-touch->getPreviousLocation().x, getPosition().y);    ここの動きをどうするか
    //ScrollView::onTouchMoved(touch, event);
}

void StampScroll::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
    if(Vec2(touch->getLocationInView()).distance(touch->getStartLocationInView()) < 30.0f){//タッチした場所から動いた距離が短い
        //ScrollView::onTouchEnded(touch, event);
        CCLOG("距離が短い");
    }else{//スワイプした時
        //CCLOG("ロケーション：%d,%d", (int)(touch->getLocation().x/ratio),(int)(touch->getLocation().y/ratio));
        //CCLOG("タッチインポイント：%d,%d", (int)(touch_in_point.x/ratio),(int)(touch_in_point.y/ratio));
        //CCLOG("コンテントオフセット：%d", (int)(getContentOffset().x/ratio));
        
        for(MenuItem *item : items){
            item->setEnabled(false);
        }
        runAction(Sequence::create(DelayTime::create(0.3f), CallFunc::create([this](){
            if(SaveData::loadInt("stamp_mute") == 0){//ミュート状態でない時
                for(MenuItem *item : this->items){
                    item->setEnabled(true);
                }
            }
        }), NULL));
        
        stopAllActions();
        getContainer()->stopAllActions();
        
        
        if((touch->getLocationInView().x - touch->getStartLocationInView().x) > 0){//左にスワイプ
            setContentOffsetInDuration(Vec2(0,0), 0.3f);
            static_cast<StampLayer *>(getParent()->getParent())->left_item->setVisible(false);
            static_cast<StampLayer *>(getParent()->getParent())->right_item->setVisible(true);
            now_page = 1;
            CCLOG("ページ移動１");
        }else if((touch->getLocationInView().x - touch->getStartLocationInView().x) < 0){//右にスワイプ
            setContentOffsetInDuration(Vec2(-533,0), 0.3f);
            static_cast<StampLayer *>(getParent()->getParent())->left_item->setVisible(true);
            static_cast<StampLayer *>(getParent()->getParent())->right_item->setVisible(false);
            now_page = 2;
            
            CCLOG("ページ移動２");
        }else{
            CCLOG("わからん");
        }
        
        //ScrollView::onTouchCancelled(touch, event);
    }
    ScrollView::onTouchEnded(touch, event);
}

