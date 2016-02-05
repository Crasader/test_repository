#include "BazaarSelectRarelityButton.h"
#include "BazaarSearchListLayer.h"
#include "PopupSelectBoxLayer.h"


BazaarSelectRarelityButton* BazaarSelectRarelityButton::Create(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer){
    
    BazaarSelectRarelityButton* menuItem = new BazaarSelectRarelityButton();
    
    if (menuItem && menuItem->init(imagePath1, imagePath2, layer))
    {
        menuItem->autorelease();
        return menuItem;
    }
    CC_SAFE_DELETE(menuItem);
    return nullptr;
    
}

bool BazaarSelectRarelityButton::init(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer){
    HMSLOG("BazaarSelectRarelityButton::init()");
    
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
    pathList.push_back("init/Home/Bazaar/txt_all.png");
    pathList.push_back("init/Home/Bazaar/txt_n.png");
    pathList.push_back("init/Home/Bazaar/txt_r.png");
    pathList.push_back("init/Home/Bazaar/txt_sr.png");
    pathList.push_back("init/Home/Bazaar/txt_ur.png");
    pathList.push_back("init/Home/Bazaar/txt_ur_plus.png");
    pathList.push_back("init/Home/Bazaar/txt_pt.png");

    
    
    return initWithNormalSprite(normalSprite, selectSprite, NULL, [layer, this](Ref* sender){
        HmsVector<string> imagePaths;
        
        for (int i = 0; i < pathList.size(); i++) {
            imagePaths.push_back(pathList[i]);
        }
        HmsVector<int> number;
        number.push_back(CharaSprite::Rarity::NoneRarity);
        number.push_back(CharaSprite::Rarity::Normal);
        number.push_back(CharaSprite::Rarity::Rare);
        number.push_back(CharaSprite::Rarity::SuperRare);
        number.push_back(CharaSprite::Rarity::UltraRare);
        number.push_back(CharaSprite::Rarity::UltraRare_Plus);
        number.push_back(CharaSprite::Rarity::LegendRare);
        this->selectBox = PopupSelectBoxLayer::create(imagePaths, number, layer->getSortRarelity());
        auto selectFunc = [this](){
            this->baseLayer->setSortRarelity(selectBox->getSelectNumber());
            textSprite->setTexture(pathList[baseLayer->getSortRarelity()]);
        };
        this->selectBox->setSelectFunc(selectFunc);
        layer->addChild(selectBox, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
    });
}

void BazaarSelectRarelityButton::onEnter(){
    MenuItemSprite::onEnter();
    
    createTextSprite();
    
    initEventListener();
}

//イベントリスナ初期化
void BazaarSelectRarelityButton::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarSelectRarelityButton::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarSelectRarelityButton::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarSelectRarelityButton::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarSelectRarelityButton::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BazaarSelectRarelityButton::createTextSprite(){
    textSprite = Sprite::create(pathList[baseLayer->getSortRarelity()]);
//    textSprite->setScale(ratio);
    textSprite->setPosition(Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
    this->addChild(textSprite);
}

bool BazaarSelectRarelityButton::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void BazaarSelectRarelityButton::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarSelectRarelityButton::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarSelectRarelityButton::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}