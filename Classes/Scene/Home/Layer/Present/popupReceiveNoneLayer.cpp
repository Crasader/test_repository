#include "popupReceiveNoneLayer.h"

popupReceiveNoneLayer* popupReceiveNoneLayer::create(string return_page_name)
{
    popupReceiveNoneLayer *node = new popupReceiveNoneLayer();
    if (node && node->init(return_page_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool popupReceiveNoneLayer::init(string return_page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("popupReceiveNoneLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //メンバ変数に保存
    _return_page_name = return_page_name;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(popupReceiveNoneLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(popupReceiveNoneLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(popupReceiveNoneLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(popupReceiveNoneLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //3秒たったら消す
    this->scheduleOnce(schedule_selector(popupReceiveNoneLayer::deleteAction), 3.0f);
    
    return true;
}

void popupReceiveNoneLayer::setupImage(string text)
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

void popupReceiveNoneLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void popupReceiveNoneLayer::deleteAction(float dt)
{
    auto normal_scene = (NormalScene *)this->getParent()->getParent();
    
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
    
    auto func2 = CallFunc::create([normal_scene](){
        normal_scene->changePage("Mypage");
    });
    runAction(Sequence::create(DelayTime::create(0.2f), func, func2, NULL));
    
}

bool popupReceiveNoneLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void popupReceiveNoneLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void popupReceiveNoneLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->unschedule(schedule_selector(popupReceiveNoneLayer::deleteAction));
    
    this->scheduleOnce(schedule_selector(popupReceiveNoneLayer::deleteAction), 0);
}
void popupReceiveNoneLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
