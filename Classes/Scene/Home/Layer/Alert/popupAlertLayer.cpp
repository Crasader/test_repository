#include "popupAlertLayer.h"

popupAlertLayer* popupAlertLayer::create(string return_page_name)
{
    popupAlertLayer *node = new popupAlertLayer();
    if (node && node->init(return_page_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool popupAlertLayer::init(string return_page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("popupAlertLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(popupAlertLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(popupAlertLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(popupAlertLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(popupAlertLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //リターンページ名の取得
    _return_page_name = return_page_name;
    
    //3秒たったら消す
    this->scheduleOnce(schedule_selector(popupAlertLayer::deleteAction), 3.0f);
    
    return true;
}

void popupAlertLayer::setupData(string text)
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
    this->addChild(bg, 2, ACTION_SPRITE_TAG);
    
    //テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 24);
    label->setScale(0);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(label, 3, ACTION_SPRITE_TAG);

    //ポップアップアクション
    popupAction();
}

void popupAlertLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.2f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void popupAlertLayer::deleteAction(float dt)
{
    auto normal_scene = (NormalScene *)this->getParent()->getParent();
    auto return_page_name = _return_page_name;
    
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == ACTION_SPRITE_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    
    auto func2 = CallFunc::create([normal_scene, return_page_name](){
        normal_scene->changePage(return_page_name);
    });
    runAction(Sequence::create(DelayTime::create(0.2f), func, func2, NULL));
    
    
//    auto children = this->getChildren();
//    
//    for (auto child : children)
//    {
//        auto scale    = ScaleTo::create(0.2f, 0);
//        auto callback = CallFunc::create([this]()
//                                         {
//                                             this->removeFromParent();
//                                         });
//        child->runAction(Sequence::create(scale, callback, NULL));
//    }
}

bool popupAlertLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void popupAlertLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void popupAlertLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->unschedule(schedule_selector(popupAlertLayer::deleteAction));
    
    this->scheduleOnce(schedule_selector(popupAlertLayer::deleteAction), 0);
}
void popupAlertLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}