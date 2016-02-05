#include "RankingEventExplainSprite.h"

RankingEventExplainSprite::RankingEventExplainSprite()
:_event_comment()
{
}

RankingEventExplainSprite* RankingEventExplainSprite::create(string comment, string image_path){
    
    RankingEventExplainSprite *sprite = new RankingEventExplainSprite();
    
    if (sprite && sprite->init(comment, image_path))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool RankingEventExplainSprite::init(string comment, string image_path)
{
    if ( !Sprite::initWithFile(image_path) )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setEventComment(comment);
    return true;
}

void RankingEventExplainSprite::onEnter()
{
    Sprite::onEnter();
    
    //クエストコメントの表示
    showEventComment();
}

void RankingEventExplainSprite::showEventComment()
{
    //説明
    auto comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(_event_comment.c_str(), FONT_NAME_SYSTEM, 21);
    comment_label->setDimensions(450, 75);
    comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
    comment_label->setAnchorPoint(Point(0, 0));
    comment_label->setPosition(Point(150, 35));
    this->addChild(comment_label, 1);
}
