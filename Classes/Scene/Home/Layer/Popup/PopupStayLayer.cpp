#include "PopupStayLayer.h"

PopupStayLayer* PopupStayLayer::create(string message, string type)
{
    PopupStayLayer *node = new PopupStayLayer();
    if (node && node->init(message, type))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool PopupStayLayer::init(string message, string type)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupStayLayer::init()");
    
    ratio = getVisibleRatio();
    
    //表示テキストの取得
    _message = message;
    _type = type;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(PopupStayLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupStayLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupStayLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupStayLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void PopupStayLayer::setupImage()
{
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

    int font_size = 24;
    if(_type=="ban") font_size = 18;
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, font_size);
    label->setScale(0);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(label, 3, POPUP_ACTION_TAG);
    
    //ポップアップアクション
    popupAction();
}

void PopupStayLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void PopupStayLayer::onEnter()
{
    Layer::onEnter();
    
    //画像の表示
    setupImage();
}

bool PopupStayLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch PopupStayLayer!");
    return true;
}
void PopupStayLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupStayLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupStayLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}