#include "ChallengeEventExplainSprite.h"

ChallengeEventExplainSprite::ChallengeEventExplainSprite()
:_event_id()
,_event_name()
,_event_comment()
{
}

ChallengeEventExplainSprite* ChallengeEventExplainSprite::create(json11::Json json, string image_path){
    
    ChallengeEventExplainSprite *sprite = new ChallengeEventExplainSprite();
    
    if (sprite && sprite->init(json, image_path))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool ChallengeEventExplainSprite::init(json11::Json json, string image_path)
{
    if ( !Sprite::initWithFile(image_path) )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json);
    
    return true;
}

void ChallengeEventExplainSprite::onEnter()
{
    Sprite::onEnter();
    
    //クエストコメントの表示
    showChallengeEventComment();
}

void ChallengeEventExplainSprite::setupData(json11::Json json)
{
    //データの設定
    setChallengeEventId(json["challenge_event_id"].string_value());
    setChallengeEventName(json["name"].string_value());
    setChallengeEventCommnet(json["comment"].string_value());
}

void ChallengeEventExplainSprite::showChallengeEventComment()
{
    //説明
    auto comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(_event_comment.c_str(), FONT_NAME_SYSTEM, 21);
    comment_label->setDimensions(450, 75);
    comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
    comment_label->setAnchorPoint(Point(0, 0));
    comment_label->setPosition(Point(150, 35));
    this->addChild(comment_label, 3);
}

