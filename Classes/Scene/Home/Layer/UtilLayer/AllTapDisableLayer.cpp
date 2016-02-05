#include "AllTapDisableLayer.h"

bool AllTapDisableLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("AllTapDisableLayer::init()");
    
    initEventListener() ;
    
    return true;
}

void AllTapDisableLayer::initEventListener(){
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}