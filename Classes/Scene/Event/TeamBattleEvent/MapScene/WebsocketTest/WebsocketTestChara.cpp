#include "WebsocketTestChara.h"
#include "MultiValue.h"
#include "PrimitiveParser.h"
#include "HmsConsoleUtil.h"

using namespace std;

WebsocketTestChara* WebsocketTestChara::create(Synchronizer* synchronizer){
    WebsocketTestChara *sprite = new WebsocketTestChara();
    if (sprite && sprite->init(synchronizer))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}
bool WebsocketTestChara::init(Synchronizer* synchronizer){
    if (!SynchronizedObject::init(false, synchronizer, "WebSocketTestChara") ) {
        return false;
    }    
    return true;
}

WebsocketTestChara* WebsocketTestChara::create(string path, Synchronizer* synchronizer,
                                               bool isLocal, string id){
    WebsocketTestChara *sprite = new WebsocketTestChara();
    if (sprite && sprite->init(path, synchronizer, isLocal, id))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool WebsocketTestChara::init(string path, Synchronizer* synchronizer,
                              bool isLocal, string id){
    if (!SynchronizedObject::init(isLocal, synchronizer, "WebSocketTestChara", id) ) {
     return false;
    }
    _path = path;
    _add_network_args->push(path);
    
    Sprite* sprite = Sprite::create(_path);
    this->addChild(sprite);
    return true;
}

void WebsocketTestChara::onEnter(){
    SynchronizedObject::onEnter();
    initEventListener();
}

void WebsocketTestChara::onSend(){
    
    
    MultiValue* multiValue = MultiValue::create();
    multiValue->push(getPosition());
    multiValue->push(1.5f);
    multiValue->push(getScale());
    
    send(multiValue);
}

void WebsocketTestChara::onReceive(MultiValue* value){
    Vec2 vec = PrimitiveParser::asVec2(value->pop().value);
    HMSLOG("multiValueTest!!!  Vec2 : x = %f, y = %f", vec.x, vec.y);
    setPosition(vec);
    
    float f = PrimitiveParser::asFloat(value->pop().value);
    HMSLOG("multiValueTest!!!  float : %f", f);
    
    float scale = PrimitiveParser::asFloat(value->pop().value);
    HMSLOG("multiValueTest!!!  Scale : %f", scale);
}

SynchronizedObject* WebsocketTestChara::onClone(MultiValue* value, std::string id,
                                                Synchronizer* synchronizer){
    string path = value->pop().value;
    WebsocketTestChara* clone = create(path, synchronizer, false, id);
    return clone;
}

void WebsocketTestChara::onLocalUpdate(float delta){
    
}

void WebsocketTestChara::onRemoteUpdate(float delta){
    
}

void WebsocketTestChara::initEventListener(){
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(WebsocketTestChara::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(WebsocketTestChara::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(WebsocketTestChara::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(WebsocketTestChara::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool WebsocketTestChara::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){

    return true;
}
void WebsocketTestChara::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){}

void WebsocketTestChara::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event){}
void WebsocketTestChara::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){}