#include "MessageSprite.h"

MessageSprite* MessageSprite::create(string message, TextHAlignment alignment)
{
    MessageSprite *sprite = new MessageSprite();
    
    if (sprite && sprite->init(message, alignment))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool MessageSprite::init(string message, TextHAlignment alignment)
{
    if ( !Sprite::initWithFile("init/Tutorial/navi1.png"))
    {
        return false;
    }
    HMSLOG("MessageSprite::init()");
    
    //メッセージの表示
    showMessage(message, alignment);
    
    return true;
}

void MessageSprite::showMessage(string message, TextHAlignment alignment)
{
    //表示するメッセージの作成
    _message_label = HMSFontUtil::createWithTTFBySystemFontArgument(message, FONT_NAME_SYSTEM, 25);
    _message_label->setHorizontalAlignment(alignment);
    _message_label->setPosition(Point(this->getContentSize().width / 2 + 80, this->getContentSize().height / 2 - 16));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(_message_label, 2);
}

void MessageSprite::onEnter()
{
    Sprite::onEnter();
    
    
}