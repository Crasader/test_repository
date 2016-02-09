#include "PopupChangePageLayer.h"

popupChangePageLayer* popupChangePageLayer::create(string message, string change_page_name, PopupSize popup_size)
{
    popupChangePageLayer *node = new popupChangePageLayer();
    if (node && node->init(message, change_page_name, popup_size))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool popupChangePageLayer::init(string message, string change_page_name, PopupSize popup_size)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("popupChangePageLayer::init()");
    
    ratio = getVisibleRatio();
    
    //表示テキストの取得
    _message = message;
    
    //変わるページ名の取得
    _change_page_name = change_page_name;
    
    //背景サイズの取得
    _popup_size = popup_size;
    
    //3秒たったら消す
    this->scheduleOnce(schedule_selector(popupChangePageLayer::closeAndchangePage), 3.0f);
    
    return true;
}

void popupChangePageLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //画像の表示
    setupImage();
    
    //ポップアップアクション
    Utility::openPopupAction(this);
}


void popupChangePageLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(popupChangePageLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(popupChangePageLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(popupChangePageLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(popupChangePageLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void popupChangePageLayer::setupImage()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, Z_Shadow, T_Shadow);
    
    //背景
    string path = "init/Home/Popup/popup_300.png";;
    switch (_popup_size)
    {
        case Popup_400: path = "init/Home/Popup/popup_400.png"; break;
        case Popup_500: path = "init/Home/Popup/popup_500.png"; break;
        default: break;
    }
    
    _bg = Sprite::create(path);
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_message, 28, _bg->getContentSize().width - 20));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize() / 2));
    _bg->addChild(label, Z_Message, T_Message);
}

void popupChangePageLayer::closeAndchangePage(float dt)
{
    auto _func = [this]()
    {
        auto normal_scene = (NormalScene *)this->getParent()->getParent();
        normal_scene->changePage(_change_page_name);
    };
    
    Utility::closePopupAction(this, _func);
}

bool popupChangePageLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch popupChangePageLayer!");
    return true;
}

void popupChangePageLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void popupChangePageLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->unschedule(schedule_selector(popupChangePageLayer::closeAndchangePage));
    
    this->scheduleOnce(schedule_selector(popupChangePageLayer::closeAndchangePage), 0);
    
}

void popupChangePageLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}