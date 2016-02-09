#include "ResultModalLayer.h"

bool ResultModalLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ResultModalLayer::init()");
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ResultModalLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ResultModalLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ResultModalLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ResultModalLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void ResultModalLayer::onEnter()
{
    Layer::onEnter();
}

bool ResultModalLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch ResultModalLayer!");
    return true;
}
void ResultModalLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ResultModalLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ResultModalLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}