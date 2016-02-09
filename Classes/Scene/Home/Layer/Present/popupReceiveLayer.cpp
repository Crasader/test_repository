#include "popupReceiveLayer.h"

bool popupReceiveLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("popupReceiveLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(popupReceiveLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(popupReceiveLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(popupReceiveLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(popupReceiveLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //3秒たったら消す
    this->scheduleOnce(schedule_selector(popupReceiveLayer::deleteAction), 3.0f);
    
    return true;
}

void popupReceiveLayer::setupImage(string text)
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
    this->addChild(bg, 2);
    
    //テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24);
    label->setScale(0);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(label, 3);
    
    //ポップアップアクション
    popupAction();
}

void popupReceiveLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void popupReceiveLayer::deleteAction(float dt)
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 0);
        auto callback = CallFunc::create([this]()
        {
            this->removeFromParent();
        });
        child->runAction(Sequence::create(scale, callback, NULL));
    }
    
    auto present_layer = (PresentMenuLayer *)this->getParent();
    present_layer->removeChildByTag(1); //スクロールビューを削除
    present_layer->getPresentDataRequest();
}

bool popupReceiveLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void popupReceiveLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void popupReceiveLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->unschedule(schedule_selector(popupReceiveLayer::deleteAction));
    
    this->scheduleOnce(schedule_selector(popupReceiveLayer::deleteAction), 0);
}
void popupReceiveLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
