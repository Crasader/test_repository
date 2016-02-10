#include "WebSocketTestLayer.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "WebsocketTestChara.h"
#include "Synchronizer.h"


#include "MultiValue.h"
#include "PrimitiveParser.h"

using namespace std;

WebSocketTestLayer* WebSocketTestLayer::create(){
    WebSocketTestLayer *layer = new WebSocketTestLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool WebSocketTestLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("WebSocketTestLayer::init()");
    function<void()> onConnectionFunc = [](){
        HMSLOG("CallBack!!!!! ConnectionFinish!!!!");
    };
    function<void()> onErrorFunc = [](){
        HMSLOG("CallBack!!!!! error!!!!");
    };
    function<void()> onDisconnectionFunc = [](){
        HMSLOG("CallBack!!!!! Disconnection!!!!");
    };
    
    
/*    _synchronizer = Synchronizer::create(this, "http://192.168.100.112", "3000", 60.0f,
                                         onConnectionFunc, onErrorFunc,
                                         onDisconnectionFunc);*/
    this->addChild(_synchronizer);
    //clone用オブジェクトの登録
    _synchronizer->
        addSynchronizedObjectCreater(WebsocketTestChara::create(_synchronizer));
    ratio = getVisibleRatio();
    
    _synchronizer->connectionStart();
    return true;
}

void WebSocketTestLayer::onEnter()
{
    Layer::onEnter();
    
    initEventListener();
    
    Sprite* bg = Sprite::create("init/Home/Mypage/back.png");
    bg->setPosition(convertFromDisplaySize(Vec2(320, 480), 0.5f, 0.5f));
    
    float myDisplaySizeY = Director::getInstance()->getVisibleSize().height;
    float y = (myDisplaySizeY / 960);
    bg->setScale(1, y);
    this->addChild(bg);
    
    _test_chara = WebsocketTestChara::create("init/CharaMenu/icon_offense3.png", _synchronizer, true);

    this->addChild(_test_chara);
    _synchronizer->add(_test_chara);
    
    _test_chara->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height / 2));
}


void WebSocketTestLayer::initEventListener(){
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(WebSocketTestLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(WebSocketTestLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(WebSocketTestLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(WebSocketTestLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool WebSocketTestLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){
    _test_chara->setPosition(touch->getLocation());
    
    return true;
}
void WebSocketTestLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){}

void WebSocketTestLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event){}
void WebSocketTestLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){}