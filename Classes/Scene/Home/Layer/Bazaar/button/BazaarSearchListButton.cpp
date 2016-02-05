#include "BazaarSearchListButton.h"
#include "BazaarSearchConditionPopupLayer.h"
#include "BazaarSearchListLayer.h"

BazaarSearchListButton* BazaarSearchListButton::Create(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer){
    
    BazaarSearchListButton* menuItem = new BazaarSearchListButton();
    
    if (menuItem && menuItem->init(imagePath1, imagePath2, layer))
    {
        menuItem->autorelease();
        return menuItem;
    }
    CC_SAFE_DELETE(menuItem);
    return nullptr;
    
}

bool BazaarSearchListButton::init(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer){
    HMSLOG("BazaarSearchListButton::init()");
    
    ratio = getVisibleRatio();
    
    Sprite* normalSprite = Sprite::create(imagePath1);
    Sprite* selectSprite = NULL;
    if(imagePath2.length() != 0){
        selectSprite = Sprite::create(imagePath2);
    }else{
        selectSprite = Sprite::create(imagePath1);
        selectSprite->setColor(Color3B(105, 105, 105));
    }
    
    return initWithNormalSprite(normalSprite, selectSprite, NULL, [layer](Ref* sender){
        BazaarSearchConditionPopupLayer* popupLayer = BazaarSearchConditionPopupLayer::create(layer);
        layer->addChild(popupLayer, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
    });
}

void BazaarSearchListButton::onEnter(){
    MenuItemSprite::onEnter();
    
    initEventListener();
}



//イベントリスナ初期化
void BazaarSearchListButton::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarSearchListButton::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarSearchListButton::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarSearchListButton::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarSearchListButton::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


bool BazaarSearchListButton::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void BazaarSearchListButton::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarSearchListButton::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarSearchListButton::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}