#include "PopupNoChangePageLayer.h"

popupNoChangePageLayer* popupNoChangePageLayer::create(string message)
{
    return popupNoChangePageLayer::create(message, 3) ;
}

popupNoChangePageLayer* popupNoChangePageLayer::create(string message, float remove_duration)
{
    popupNoChangePageLayer *node = new popupNoChangePageLayer();
    if (node && node->init(message, remove_duration))
    {
    node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool popupNoChangePageLayer::init(string message, float remove_duration)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("popupNoChangePageLayer::init()");
    
    ratio = getVisibleRatio();
    
    //表示テキストの取得
    _message = message;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(popupNoChangePageLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(popupNoChangePageLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(popupNoChangePageLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(popupNoChangePageLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //数秒後削除
    this->scheduleOnce(schedule_selector(popupNoChangePageLayer::deleteAction), remove_duration);
    
    return true;
}

void popupNoChangePageLayer::setupImage()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_200.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 2 ,POPUP_ACTION_TAG);
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    label->setScale(0);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(label, 3, POPUP_ACTION_TAG);
    
    //ポップアップアクション
    popupAction();
}

void popupNoChangePageLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3");
}

void popupNoChangePageLayer::deleteAction(float dt)
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == POPUP_ACTION_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
	
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
}

void popupNoChangePageLayer::onEnter()
{
    Layer::onEnter();
    
    //画像の表示
    setupImage();
}

bool popupNoChangePageLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch popupNoChangePageLayer!");
    return true;
}
void popupNoChangePageLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void popupNoChangePageLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->unschedule(schedule_selector(popupNoChangePageLayer::deleteAction));
    
    this->scheduleOnce(schedule_selector(popupNoChangePageLayer::deleteAction), 0);

}
void popupNoChangePageLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
