#include "EventExplainSprite.h"

EventExplainSprite::EventExplainSprite()
:_event_id()
,_event_name()
,_event_comment()
,_achieve_count(0)
{
}

EventExplainSprite* EventExplainSprite::create(json11::Json json, string image_path){
    
    EventExplainSprite *sprite = new EventExplainSprite();
    
    if (sprite && sprite->init(json, image_path))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool EventExplainSprite::init(json11::Json json, string image_path)
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

void EventExplainSprite::onEnter()
{
    Sprite::onEnter();
    
    //クエストコメントの表示
    showEventComment();
    
    //達成画像の表示
    showAchieveStar();
}

void EventExplainSprite::setupData(json11::Json json)
{
    //データの設定
    setEventId(json["npc_battle_event_id"].string_value());
    setEventName(json["name"].string_value());
    setEventCommnet(json["comment"].string_value());
    setEventAchieveCount(fromString<int>(json["achieve_count"].string_value()));
}

void EventExplainSprite::showEventComment()
{
    //説明
    auto comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(_event_comment.c_str(), FONT_NAME_SYSTEM, 21);
    comment_label->setDimensions(450, 75);
    comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
    comment_label->setAnchorPoint(Point(0, 0));
    comment_label->setPosition(Point(150, 35));
    this->addChild(comment_label, 3);
}

void EventExplainSprite::showAchieveStar()
{
    if (_achieve_count > 0 && _achieve_count <= 3)
    {
        //達成画像の表示
        auto achieve_sp = Sprite::create("init/Home/Quest/star" + toString<int>(_achieve_count) + ".png");
        achieve_sp->setAnchorPoint(Point(0.5, 0));
        achieve_sp->setPosition(Point(560, 120));
        this->addChild(achieve_sp, 10);
    }
}