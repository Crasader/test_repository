#include "BazaarListRefreshButton.h"

BazaarListRefreshButton* BazaarListRefreshButton::Create(string imagePath1, string imagePath2, BazaarListLayer* bazaarListLayer){
    
    BazaarListRefreshButton* menuItem = new BazaarListRefreshButton();
    
    if (menuItem && menuItem->init(imagePath1, imagePath2, bazaarListLayer))
    {
        menuItem->autorelease();
        return menuItem;
    }
    CC_SAFE_DELETE(menuItem);
    return nullptr;
    
}

bool BazaarListRefreshButton::init(string imagePath1, string imagePath2, BazaarListLayer* bazaarListLayer){
    HMSLOG("BazaarListRefreshButton::init()");
    this->bazaarListLayer = bazaarListLayer;
    
    ratio = getVisibleRatio();
    
    Sprite* normalSprite = Sprite::create(imagePath1);
    Sprite* selectSprite = NULL;
    if(imagePath2.length() != 0){
        selectSprite = Sprite::create(imagePath2);
    }else{
        selectSprite = Sprite::create(imagePath1);
        selectSprite->setColor(Color3B(105, 105, 105));
    }
    
    return initWithNormalSprite(normalSprite, selectSprite, NULL, [](Ref* sender){
        HMSLOG("BazaarListRefreshButton::StartRefresh!!");
    });
}

void BazaarListRefreshButton::onEnter(){
    MenuItemSprite::onEnter();
    
    initEventListener();
}



//イベントリスナ初期化
void BazaarListRefreshButton::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarListRefreshButton::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarListRefreshButton::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarListRefreshButton::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarListRefreshButton::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


bool BazaarListRefreshButton::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void BazaarListRefreshButton::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarListRefreshButton::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarListRefreshButton::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}