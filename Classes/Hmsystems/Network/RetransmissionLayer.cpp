#include "RetransmissionLayer.h"

RetransmissionLayer* RetransmissionLayer::create(string message)
{
    RetransmissionLayer *node = new RetransmissionLayer();
    if (node && node->init(message))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool RetransmissionLayer::init(string message)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("RetransmissionLayer::init()");
    
    //表示するテキストの取得
    _message = message;
    
    //モーダルレイヤーにするためのタッチ処理
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(RetransmissionLayer::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    ratio = getVisibleRatio();
    
    return true;
}

void RetransmissionLayer::setupImage(){
    
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_300.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 2 ,POPUP_ACTION_TAG);
    
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, 24);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(bg->getContentSize().width / 2, bg->getContentSize().height / 2 + 40));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(label, 1);
    
    //ボタンの作成
    auto btn_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto btn_on  = Sprite::create("init/Home/Common/bt_ok.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto ok_btn = MenuItemSprite::create(btn_off, btn_on, [this](Ref* sender)
    {
        this->removeFromParent();
        
        //再送信処理
        HttpConnect::HttpRetransmission();
    });
    ok_btn->setAnchorPoint(Point(0.5, 0));
    auto Menu = Menu::create(ok_btn, nullptr);
    Menu->setPosition(Point(bg->getContentSize().width / 2, 10));
    bg->addChild(Menu, 1);
    
    
    //ポップアップアクション
    popupAction();
}

void RetransmissionLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void RetransmissionLayer::onEnter()
{
    Layer::onEnter();
    
    //画像の表示
    setupImage();
}

bool RetransmissionLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch RetransmissionLayer!");
    return true;
}
void RetransmissionLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void RetransmissionLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void RetransmissionLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}