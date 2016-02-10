#include "TeamBattleMapLayer.h"

#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "Synchronizer.h"

#include "TeamBattleCharacter.h"

TeamBattleMapLayer* TeamBattleMapLayer::create(){
    TeamBattleMapLayer *layer = new TeamBattleMapLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool TeamBattleMapLayer::init(){
    if ( !Layer::init() ) return false;
    HMSLOG("TeamBattleMapLayer::init()");
    
    _ratio = getVisibleRatio();
    _synchronizer = Synchronizer::create(this, "http://192.168.100.112", "3000", 60.0f,
                    CC_CALLBACK_1(TeamBattleMapLayer::onConnectionCallback, this),
                    CC_CALLBACK_1(TeamBattleMapLayer::onErrorCallback, this),
                    CC_CALLBACK_1(TeamBattleMapLayer::onDisconnectionCallback, this));
    this->addChild(_synchronizer);
    
    createClone();
    
    _synchronizer->connectionStart();
    return true;
}

void TeamBattleMapLayer::onEnter()
{
    Layer::onEnter();
    
    initEventListener();
    
    createBackground();
    
    TeamBattleCharacter* chara = TeamBattleCharacter::createLocal(_synchronizer);
    this->addChild(chara);
    this->setPosition(Vec2::ZERO);
    _synchronizer->add(chara);
}

void TeamBattleMapLayer::createBackground(){
    _background = Sprite::create("init/Map/back.png");
    _background->setPosition(convertFromDisplaySize(Vec2(320, 480), 0.5f, 0.5f));
    
    //4S基準の大きさに変更
    float myDisplaySizeY = Director::getInstance()->getVisibleSize().height;
    float y = (myDisplaySizeY / 960);
    _background->setScale(1, y);
    this->addChild(_background);
}

void TeamBattleMapLayer::createClone(){
    _synchronizer->addSynchronizedObjectCreater(TeamBattleCharacter::createPrototype(_synchronizer));
}

void TeamBattleMapLayer::onConnectionCallback(Synchronizer *synchronizer){
    CCLOG("onConnectionCallback!!");
}
void TeamBattleMapLayer::onErrorCallback(Synchronizer *synchronizer){
    CCLOG("onErrorCallback!!");
}
void TeamBattleMapLayer::onDisconnectionCallback(Synchronizer *synchronizer){
    CCLOG("onDisconnectionCallback!!");
}

void TeamBattleMapLayer::initEventListener(){
    //タップイベントの設定
    _listener = EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = CC_CALLBACK_2(TeamBattleMapLayer::onTouchBegan, this);
    _listener->onTouchMoved = CC_CALLBACK_2(TeamBattleMapLayer::onTouchMoved, this);
    _listener->onTouchEnded = CC_CALLBACK_2(TeamBattleMapLayer::onTouchEnded, this);
    _listener->onTouchCancelled = CC_CALLBACK_2(TeamBattleMapLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
}

bool TeamBattleMapLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){
    
    return true;
}
void TeamBattleMapLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){}

void TeamBattleMapLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event){}
void TeamBattleMapLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){}