#include "TeamMenuLayer.h"

bool TeamMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamMenuLayer::init()");
    
    return true;
}

void TeamMenuLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TeamMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamMenuLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


bool TeamMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TeamMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TeamMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{

}


void TeamMenuLayer::onEnter()
{
    Layer::onEnter();
}
