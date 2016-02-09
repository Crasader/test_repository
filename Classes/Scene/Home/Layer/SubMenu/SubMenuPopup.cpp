#include "SubMenuPopup.h"

SubMenuPopup* SubMenuPopup::create(string return_page_name)
{
    SubMenuPopup *node = new SubMenuPopup();
    if (node && node->init(return_page_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool SubMenuPopup::init(string return_page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("SubMenuPopup::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //メンバ変数に保存
    _return_page_name = return_page_name;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(SubMenuPopup::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(SubMenuPopup::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(SubMenuPopup::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(SubMenuPopup::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void SubMenuPopup::setupImage(string text)
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_200_s.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 2 ,ACTION_SPRITE_TAG);
    
    //テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24);
    label->setScale(0);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(label, 3, ACTION_SPRITE_TAG);
    
    //ポップアップアクション
    popupAction();
}

void SubMenuPopup::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

bool SubMenuPopup::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void SubMenuPopup::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void SubMenuPopup::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void SubMenuPopup::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
