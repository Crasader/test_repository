#include "TouchScreenSprite.h"

TouchScreenSprite* TouchScreenSprite::create()
{
    TouchScreenSprite *sprite = new TouchScreenSprite();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TouchScreenSprite::init()
{
    if ( !Sprite::initWithFile("init/Title/tap.png"))
    {
        return false;
    }
    HMSLOG("TouchScreenSprite::init()");
    
    return true;
}

void TouchScreenSprite::blinkingAction()
{
    //点滅させる処理
    auto fadeIn  = FadeIn::create(BLINKING_TIME);
    auto fadeOut = FadeOut::create(BLINKING_TIME);
    auto seq     = Sequence::create(fadeIn, fadeOut, NULL);
    auto repeat  = RepeatForever::create(seq);
    this->runAction(repeat);
}

void TouchScreenSprite::onEnter()
{
    Sprite::onEnter();
    
    //点滅
    blinkingAction();
}