#include "MapStampScroll.h"
#include "MapStampLayer.h"
#include "MapStampMenu.h"
#include "MapScene.h"
#include "SaveData.h"

bool MapStampScroll::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    CCLOG("スタンプテスト");
    ratio = getVisibleRatio();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(MapStampScroll::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MapStampScroll::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MapStampScroll::onTouchEnded, this);
    
    listener->setSwallowTouches(false);
    
    //タップイベントの登録
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    _eventDispatcher->addEventListenerWithFixedPriority(listener, -1);
    
    return true;
}

void MapStampScroll::makeScroll(){
    
    
    int wide_interval = 120;//ボタン同士の左右の間隔
    int height_interval = 135;//ボタン同士の上下の間隔
    int page_interval = 60;//ページの間に追加する間隔
 
    //スタンプボタン
    for(int i=1; i<=16; i++){
        Sprite *temp = Sprite::create("download/Map/bt_stamp" + to_string(i) +".png");
        Sprite *temp2 = Sprite::create("download/Map/bt_stamp" + to_string(i) +".png");
    
        temp2->setColor(Color3B(180, 180, 180));
        
        auto item_temp = MenuItemSprite::create(temp, temp2, CC_CALLBACK_1(MapStampScroll::onStampButton, this));
        item_temp->setTag(i);
        
        items.pushBack(item_temp);
        
        if(SaveData::loadInt("map_stamp_mute") == 1){//ミュート状態の時
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
    stamp_menu = MapStampMenu::createWithArray(items);
    stamp_menu->setPosition(Vec2(0,0));
    addChild(stamp_menu);
}

void MapStampScroll::onStampButton(Ref *menu){
    //20151008
    /*
    DungeonScene *dung = static_cast<DungeonScene *>(getParent()->getParent()->getParent());
    
    MenuItemSprite *test = static_cast<MenuItemSprite *>(menu);
    
    dung->uiLayer->showStamp(test->getTag(), TurnManage::getPlayerIndex());
    
    map<string , Json> send_map;
    send_map.insert(make_pair("player_no", Json(TurnManage::getPlayerNo())));
    send_map.insert(make_pair("type", Json("stamp")));
    send_map.insert(make_pair("extra_data", Json(test->getTag()-1)));
    dung->sendMessage(Json(send_map).dump());
    
    
    
    //dung->sendMessage("stamp:" + toString(TurnManage::getPlayerNo()) + ":" + toString(test->getTag()-1));
    static_cast<MapStampLayer *>(getParent()->getParent())->close();
    
    dung->uiLayer->stamp_cant_time = 2.0f;//X秒間はスタンプ不可
     */
    
    MapScene *map_scene = static_cast<MapScene *>(getParent()->getParent()->getParent());
    
    MenuItemSprite *menu_item = static_cast<MenuItemSprite *>(menu);
    int tag = menu_item->getTag();
    HMSLOG("stamp tag=%d", tag);
    
    map_scene->insertSendCommandList("stamp_" + SaveData::loadString("user_master_id") + "_" + to_string(tag));
    
    static_cast<MapStampLayer *>(getParent()->getParent())->close();
}

bool MapStampScroll::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
    if(SaveData::loadInt("map_stamp_mute") == 0){//ミュート状態でない時
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
    
    
    HMSLOG("タッチ位置%f",items.at(0)->getPosition().x);
    
    
    return ScrollView::onTouchBegan(touch, event);
}

void MapStampScroll::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //ScrollView::onTouchMoved(touch, event);
}

void MapStampScroll::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("タッチ距離%f",swipe_distance);
    
    if(Vec2(touch->getLocationInView()).distance(touch->getStartLocationInView()) < 30.0f){//タッチした場所から動いた距離が短い
        //ScrollView::onTouchEnded(touch, event);
        
    }else{//スワイプした時
        for(MenuItem *item : items){
            item->setEnabled(false);
        }
        runAction(Sequence::create(DelayTime::create(0.3f), CallFunc::create([this](){
            if(SaveData::loadInt("map_stamp_mute") == 0){//ミュート状態でない時
                for(MenuItem *item : this->items){
                    item->setEnabled(true);
                }
            }
        }), NULL));
        
        stopAllActions();
        getContainer()->stopAllActions();
        
        if((touch->getLocationInView().x - touch->getStartLocationInView().x) > 0){//左にスワイプ
            setContentOffsetInDuration(Vec2(0,0), 0.3f);
            static_cast<MapStampLayer *>(getParent()->getParent())->left_item->setVisible(false);
            static_cast<MapStampLayer *>(getParent()->getParent())->right_item->setVisible(true);
            now_page = 1;
            CCLOG("ページ移動１");
        }else if((touch->getLocationInView().x - touch->getStartLocationInView().x) < 0){//右にスワイプ
            setContentOffsetInDuration(Vec2(-533,0), 0.3f);
            static_cast<MapStampLayer *>(getParent()->getParent())->left_item->setVisible(true);
            static_cast<MapStampLayer *>(getParent()->getParent())->right_item->setVisible(false);
            now_page = 2;
            
            CCLOG("ページ移動２");
        }
        
        //ScrollView::onTouchCancelled(touch, event);
    }
    ScrollView::onTouchEnded(touch, event);
    
    //swipe_distance = 0.0f;
}

