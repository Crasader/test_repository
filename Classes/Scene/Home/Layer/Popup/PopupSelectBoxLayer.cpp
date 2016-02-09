#include "PopupSelectBoxLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "Utility.h"
#include "HmsVector.h"
#include <math.h>


PopupSelectBoxLayer::PopupSelectBoxLayer()
:_close_func(nullptr),
selectFunc(nullptr)
{
}

PopupSelectBoxLayer* PopupSelectBoxLayer::create(HmsVector<string> imagePaths,HmsVector<int> number, int defaultNumber, bool isShadow)
{
    PopupSelectBoxLayer *pRet = new PopupSelectBoxLayer();
    
    if (pRet && pRet->init(imagePaths, number, defaultNumber, isShadow))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool PopupSelectBoxLayer::init(HmsVector<string> imagePaths,HmsVector<int> number, int defaultNumber, bool isShadow)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupSelectBoxLayer::init()");
    
    //初期データの設定
    this->isShadow = isShadow;
    this->imagePaths = imagePaths;
    this->numbers = number;
    isMoving = false;
    ratio = getVisibleRatio() ;
    int index = 0;
    for_each(number.begin(), number.end(), [this, &defaultNumber, &index](int &num){
        if(num == defaultNumber){
            nowIndex = index;
        }
        index++;
    });

    return true;
}

void PopupSelectBoxLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    createBg();
    createOkButton();
    initClippingNode();
    createCenterBar();
    createContentsImage();
    
    this->schedule(schedule_selector(PopupSelectBoxLayer::updateContentsPosition), 0.01f);
}

void PopupSelectBoxLayer::createBg()
{
    
    Sprite* shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5)) ;
    this->addChild(shadow, Z_Shadow, T_Shadow);
    
    
    //背景画像
    background = Sprite::create("init/Home/Bazaar/bk_select.png");
    background->setScale(ratio);
    background->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(background, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    
    //閉じるボタン
    MenuItemImage* closeButton = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        
        if (_close_func != nullptr) _close_func();
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    Menu* menu = Menu::create(closeButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width - 20, background->getContentSize().height - 20));
    background->addChild(menu, ZOrder::Z_Close_Button, Tag::T_Close_Button);
    center = Vec2(background->getContentSize().width/2, background->getContentSize().height/2);
}

void PopupSelectBoxLayer::createOkButton(){
    //OKボタンの作成
    Sprite* okOff = Sprite::create("init/Home/Bazaar/button/bt_decide.png");
    Sprite* okOn  = Sprite::create("init/Home/Bazaar/button/bt_decide.png");
    okOn->setColor(Color3B(180, 180, 180));
    
    auto okButton = MenuItemSprite::create(okOff, okOn, [this](Ref* sender){
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
        this->selectFunc();
        //ポップアップ閉じる
        Utility::closePopupAction(this);
    });
    okButton->setAnchorPoint(Point(0.5, 0));
    
    Menu* menu = Menu::create(okButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width / 2, -150));
    background->addChild(menu, ZOrder::Z_Ok_Button, Tag::T_Ok_Button);

    
}

void PopupSelectBoxLayer::createCenterBar(){
    Sprite* centerBar = Sprite::create("init/Home/Bazaar/select.png");
    centerBar->setPosition(Point(background->getContentSize().width / 2 , background->getContentSize().height / 2));
    background->addChild(centerBar, Z_Center_Bar, T_Center_Bar);
}

void PopupSelectBoxLayer::createContentsImage(){
    int index = 0;
    for_each(imagePaths.begin(), imagePaths.end(), [this, &index](string &imagePath){
        HMSLOG(imagePath.c_str());
        Sprite* contents = Sprite::create(imagePath);
//        contents->setScale(ratio) ;
        contentsSprites.push_back(contents);
        imageHeight = contents->getContentSize().height;
        int diff = index - nowIndex;
        contents->setPosition(Vec2(background->getContentSize().width/2, background->getContentSize().height/2 + diff * (imageHeight + MARGIN) ));
        
        clipper->addChild(contents, Z_Contents, T_Contents);
        
        index++;
    });

}

void PopupSelectBoxLayer::updateContentsPosition(float t){
    //移動量をスプライトたちに加算
    for(int i = 0; i < contentsSprites.size(); i++){
        contentsSprites[i]->setPositionY(contentsSprites[i]->getPositionY() + velocity.y);
        
    };
    
    velocity.y = velocity.y * 0.7f;

    //適当なスピード
    if(velocity.y >= 0.1f || isMoving) return;
    
    //一番近いところに移動
    Vec2 approachVelocity;
    int target = culcClosestCenterIndex();
    approachVelocity = (center - contentsSprites[target]->getPosition()) * 0.25f;
    
    for(int i = 0; i < contentsSprites.size(); i++){
        contentsSprites[i]->setPositionY(contentsSprites[i]->getPositionY() + approachVelocity.y);
    };
    
    nowIndex = target;
}

int PopupSelectBoxLayer::culcClosestCenterIndex(){
    float distance = 1000;
    int resultIndex;
    for(int i = 0; i < contentsSprites.size(); i++){
        float diffDistance = center.distance(contentsSprites[i]->getPosition());
        
        if(diffDistance <= distance){
            distance = diffDistance;
            resultIndex = i;
        }
    };
    
    return resultIndex;
}

int PopupSelectBoxLayer::getSelectNumber(){
    return this->numbers[nowIndex];
}

void PopupSelectBoxLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(PopupSelectBoxLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupSelectBoxLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupSelectBoxLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupSelectBoxLayer::onTouchCancelled, this);
    listener->setSwallowTouches(true);
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool PopupSelectBoxLayer::isContainPopupPosition(Vec2 position){
    //有効判定
    Size bgSize = background->getContentSize();
    Point center = convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2) , 0.5, 0.5);
    float bgTop = center.y + bgSize.height/2 * ratio; //上端位置
    float bgBottom = center.y - bgSize.height/2 * ratio; //下端位置
    float bgLeft = center.x + bgSize.width/2 * ratio; //左端位置
    float bgRight = center.x - bgSize.width/2 * ratio; //右端位置
    
    if((bgBottom <= position.y && bgTop >= position.y) && (bgLeft <= position.x && bgRight >= position.x))  {
        return true ;
    }
    
    return false ;

}

void PopupSelectBoxLayer::initClippingNode() {
    Rect rect = Rect(0, 0, background->getContentSize().width-20, background->getContentSize().height-35);
    Sprite* mask = Sprite::create();
    mask->setTextureRect(rect);
    mask->setAnchorPoint(Point(0,0));
    mask->setPositionY(mask->getPositionY()+20);
    
    clipper = ClippingNode::create();
    clipper->setStencil(mask);
    clipper->setAlphaThreshold(0.01f);
    background->addChild(clipper);
}

bool PopupSelectBoxLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){
    HMSLOG("onTouchBegan");
    if(isContainPopupPosition(touch->getLocation())) return false;
    prevTouchPosition = touch->getLocation();
    
    return true;
}
void PopupSelectBoxLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){
    velocity = touch->getLocation() - prevTouchPosition;
    
    prevTouchPosition = touch->getLocation();
    isMoving = true;
    
}
void PopupSelectBoxLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    isMoving = false;
}
void PopupSelectBoxLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}