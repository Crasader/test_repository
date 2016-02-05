#include "BazaarSelectCostButton.h"
#include "BazaarSearchListLayer.h"
#include "PopupSelectBoxLayer.h"
#include <iostream>


BazaarSelectCostButton* BazaarSelectCostButton::Create(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer){
    
    BazaarSelectCostButton* menuItem = new BazaarSelectCostButton();
    
    if (menuItem && menuItem->init(imagePath1, imagePath2, layer))
    {
        menuItem->autorelease();
        return menuItem;
    }
    CC_SAFE_DELETE(menuItem);
    return nullptr;
    
}

bool BazaarSelectCostButton::init(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer){
    HMSLOG("BazaarSelectCostButton::init()");
    
    ratio = getVisibleRatio();
    baseLayer = layer;
    
    Sprite* normalSprite = Sprite::create(imagePath1);
    Sprite* selectSprite = NULL;
    if(imagePath2.length() != 0){
        selectSprite = Sprite::create(imagePath2);
    }else{
        selectSprite = Sprite::create(imagePath1);
        selectSprite->setColor(Color3B(105, 105, 105));
    }
    for (int i = 0; i < 9; i++) {
        if(i == 0){
            pathList.push_back("init/Home/Bazaar/txt_all.png");
            continue;
        }
        pathList.push_back( "init/Home/Bazaar/txt_" + toString(i) + ".png");
    }
    
    
    
    return initWithNormalSprite(normalSprite, selectSprite, NULL, [layer, this](Ref* sender){
        HmsVector<string> imagePaths;
        HmsVector<int> number;
        
        for (int i = 0; i < pathList.size(); i++) {
            imagePaths.push_back(pathList[i]);
            number.push_back(i);
        }
        this->selectBox = PopupSelectBoxLayer::create(imagePaths, number, layer->getSortCost());
        auto selectFunc = [this](){
            this->baseLayer->setSortCost(selectBox->getSelectNumber());
            textSprite->setTexture(pathList[baseLayer->getSortCost()]);
        };
        this->selectBox->setSelectFunc(selectFunc);
        layer->addChild(selectBox, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
    });
}

void BazaarSelectCostButton::onEnter(){
    MenuItemSprite::onEnter();
    
    createTextSprite();
    
    initEventListener();
}

//イベントリスナ初期化
void BazaarSelectCostButton::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarSelectCostButton::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarSelectCostButton::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarSelectCostButton::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarSelectCostButton::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BazaarSelectCostButton::createTextSprite(){
    textSprite = Sprite::create(pathList[baseLayer->getSortCost()]);
//    textSprite->setScale(ratio);
    textSprite->setPosition(Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
    this->addChild(textSprite);
}

bool BazaarSelectCostButton::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void BazaarSelectCostButton::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarSelectCostButton::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarSelectCostButton::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}