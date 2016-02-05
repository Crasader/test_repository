#include "MapStampLayer.h"
#include "MapStampScroll.h"
#include "MapStampMenu.h"
#include "MapScene.h"
#include "SaveData.h"

bool MapStampLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(MapStampLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MapStampLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MapStampLayer::onTouchEnded, this);
    
    listener->setSwallowTouches(true);
    
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //背景
    bg = Sprite::create("download/Map/bk_stamp.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 280), 0.5, 0.5));
    this->addChild(bg, 2, STAMP_ACTION_TAG);
    
    
    //スクロールの作成
    stamp_scroll = MapStampScroll::create();
    stamp_scroll->initWithViewSize(Size(533, 700));//右端の決定
    stamp_scroll->makeScroll();
    
    
    bg->addChild(stamp_scroll);
    stamp_scroll->setDirection(ScrollView::Direction::HORIZONTAL);
    stamp_scroll->setPosition(Vec2(38,0));//左端の決定
    stamp_scroll->setContentSize(Size(1072,100));//移動できるサイズの決定


    //ミュートボタン
    Sprite *mute_bt2;
    if(SaveData::loadInt("map_stamp_mute") == 1){//ミュート状態の時
        mute_bt = Sprite::create("download/Dungeon/bt_mute02.png");
        mute_bt2 = Sprite::create("download/Dungeon/bt_mute02.png");
    }else{
        mute_bt = Sprite::create("download/Dungeon/bt_mute01.png");
        mute_bt2 = Sprite::create("download/Dungeon/bt_mute01.png");
    }
    
    auto item_mute = MenuItemSprite::create(mute_bt, mute_bt2, [this](Ref *sender){
        
        if(SaveData::loadInt("map_stamp_mute") == 1){//ミュート状態の時
            SaveData::saveInt("map_stamp_mute", 0);
            static_cast<MenuItemSprite *>(sender)->setNormalImage(Sprite::create("download/Dungeon/bt_mute01.png"));
            static_cast<MenuItemSprite *>(sender)->setSelectedImage(Sprite::create("download/Dungeon/bt_mute01.png"));
            /*for(Menu *menu : stamp_menu){
                menu->setColor(Color3B(255,255,255));
                menu->setEnabled(true);
            }*/
            for(MenuItem *item : stamp_scroll->items){
                item->setEnabled(true);
                item->setColor(Color3B(255,255,255));
            }
            
            //20151008
            //static_cast<DungeonScene *>(getParent())->uiLayer->stamp_img->setTexture("download/Dungeon/bt_emotion_on.png");
            //static_cast<DungeonScene *>(getParent())->uiLayer->stamp_img->setOpacity(255);
        }else{
            SaveData::saveInt("map_stamp_mute", 1);
            static_cast<MenuItemSprite *>(sender)->setNormalImage(Sprite::create("download/Dungeon/bt_mute02.png"));
            static_cast<MenuItemSprite *>(sender)->setSelectedImage(Sprite::create("download/Dungeon/bt_mute02.png"));
            /*for(Menu *menu : stamp_menu){
                menu->setColor(Color3B(127,127,127));
                menu->setEnabled(false);
            }*/
            this->close();
            //20151008
            //static_cast<DungeonScene *>(getParent())->uiLayer->stamp_img->setTexture("download/Dungeon/bt_emotion_off.png");
            //static_cast<DungeonScene *>(getParent())->uiLayer->stamp_img->setOpacity(160);
        }
        
    });
    auto mute_menu = Menu::create(item_mute, NULL);
    bg->addChild(mute_menu);
    mute_menu->setPosition(Point(305,55));
    
    
    //クローズボタン
    close_bt = Sprite::create("download/Dungeon/bt_close.png");
    Sprite *close_bt2 = Sprite::create("download/Dungeon/bt_close.png");
    auto item_close = MenuItemSprite::create(close_bt, close_bt2, [this](Ref *sender){
        this->close();
    });
    auto menu = Menu::create(item_close, NULL);
    bg->addChild(menu);
    menu->setPosition(Point(570,410));
    
    //ポップアップアクション
    bg->setScale(0.0f);
    bg->runAction(ScaleTo::create(0.1f, ratio));
    
    //ページ移動ボタン
    auto right_spr = Sprite::create("init/CharaMenu/arrow_right.png");
    auto right_spr2 = Sprite::create("init/CharaMenu/arrow_right.png");
    right_spr2->setColor(Color3B(127,127,127));
    auto left_spr = Sprite::create("init/CharaMenu/arrow_left.png");
    auto left_spr2 = Sprite::create("init/CharaMenu/arrow_left.png");
    left_spr2->setColor(Color3B(127,127,127));
    right_item = MenuItemSprite::create(right_spr, right_spr2, [this](Ref *sender){//右側の三角を押した時
        this->stamp_scroll->setContentOffsetInDuration(Vec2(-533,0), 0.3f);
        this->left_item->setVisible(true);
        this->right_item->setVisible(false);
        this->stamp_scroll->now_page = 2;
    });
    right_item->setPosition(Point(540,0));
    auto scaler = MoveTo::create(0.5f, Vec2(10+540,0));
    auto ease1 = EaseSineIn::create(scaler);
    auto scaler2 = MoveTo::create(0.5f, Vec2(540,0));
    auto ease2 = EaseSineOut::create(scaler2);
    auto forever = RepeatForever::create(Sequence::create(ease1,ease2, NULL));
    right_item->runAction(forever);
    
    left_item = MenuItemSprite::create(left_spr, left_spr2, [this](Ref *sender){//左側の三角を押した時
        this->stamp_scroll->setContentOffsetInDuration(Vec2(0,0), 0.3f);
        this->left_item->setVisible(false);
        this->right_item->setVisible(true);
        this->stamp_scroll->now_page = 1;
    });
    auto menu2 = Menu::create(right_item,left_item, NULL);
    bg->addChild(menu2);
    menu2->setPosition(Point(40,235));
    
    left_item->setPosition(Point(540,0));
    auto scalerr = MoveTo::create(0.5f, Vec2(-10+2,0));
    auto ease1r = EaseSineIn::create(scalerr);
    auto scaler2r = MoveTo::create(0.5f, Vec2(2,0));
    auto ease2r = EaseSineOut::create(scaler2r);
    auto foreverr = RepeatForever::create(Sequence::create(ease1r,ease2r, NULL));
    left_item->runAction(foreverr);
    
    left_item->setVisible(false);
    
    return true;
}


void MapStampLayer::close()
{
    //Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(stamp_scroll);
    //stamp_scroll->removeAllChildren();
    //CCARRAY_FOREACH_REVERse
    
    //タッチを停止しないとなぜか次開く時に落ちる・・・
    listener->setEnabled(false);
    stamp_scroll->listener->setEnabled(false);
    
    
    runAction(Sequence::create(FadeTo::create(0.1f, 0), RemoveSelf::create(), NULL));

}

bool MapStampLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
     if(bg->boundingBox().containsPoint(touch->getLocation())) {
     //スタンプ背景内クリック
         listener->setSwallowTouches(true);//これをすることで下にある武将の名前などが表示されないようにする
         
        
         return true;
    }
    
    //close();
    
    return true;
}
void MapStampLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}


void MapStampLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    CCLOG("ロケーションy:%f",touch->getLocation().y/ratio);
    if(bg->boundingBox().containsPoint(touch->getLocation()) || touch->getLocation().y/ratio < 480){//スタンプ背景内クリック
        //stamp_scroll->setContentOffsetInDuration(Vec2(300,0), 0.3f);
    }else{//背景以外クリックしたらとじる
        close();
    }
    
}

