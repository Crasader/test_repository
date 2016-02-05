#include "Utility.h"
#include "WebsocketTestChatSprite.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"

WebsocketTestChatSprite* WebsocketTestChatSprite::create(string text, bool isMine){
    WebsocketTestChatSprite *sprite = new WebsocketTestChatSprite();
    if (sprite && sprite->init(text, isMine))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool WebsocketTestChatSprite::init(string text, bool isMine){
    string path ="init/Team/chat/comment_box_myself.png";
    if(!isMine) path ="init/Team/chat/comment_box_others.png";
    if ( !Sprite::initWithFile(path) )    return false;
    
    ratio = getVisibleRatio();
    messageStr = text;
    _isMine = isMine;
    return true;
}

void WebsocketTestChatSprite::onEnter(){
    Sprite::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    createMessageLabel();
}

void WebsocketTestChatSprite::createMessageLabel(){
    messageLabel = HMSFontUtil::createWithTTFBySystemFontArgument(messageStr, FONT_NAME_SYSTEM, 23);
    messageLabel->setPosition(Point(this->getContentSize().width/2, this->getContentSize().height/2));
    messageLabel->setHorizontalAlignment(TextHAlignment::LEFT) ;
    messageLabel->setVerticalAlignment(TextVAlignment::TOP);
    messageLabel->setAnchorPoint(Point(0.5, 0.5));
    this->addChild(messageLabel, ZOrder::Z_MESSAGE_LABEL, Tag::T_MESSAGE_LABEL);
}

void WebsocketTestChatSprite::initEventListener(){
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(WebsocketTestChatSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(WebsocketTestChatSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(WebsocketTestChatSprite::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(WebsocketTestChatSprite::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool WebsocketTestChatSprite::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){
    HMSLOG("onTouchBegan");
    return true;
}
void WebsocketTestChatSprite::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){}

void WebsocketTestChatSprite::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event){}
void WebsocketTestChatSprite::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){}