#include "PassiveSkillSprite.h"

PassiveSkillSprite::PassiveSkillSprite()
:_passive_skill_id()
,_passive_skill_name()
,_passive_skill_detail()
{
}

PassiveSkillSprite* PassiveSkillSprite::create(){
    
    PassiveSkillSprite *sprite = new PassiveSkillSprite();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool PassiveSkillSprite::init()
{
    if ( !Sprite::initWithFile("init/Home/Friend/popup_skill_p.png") )
    {
        return false;
    }
    ratio = getVisibleRatio();
    
    return true;
}

void PassiveSkillSprite::setupSkillData(Json json)
{
    //データの保存
    setPassiveSkillId(json["passive_skill_id"].string_value());
    setPassiveSkillName(json["passive_skill_name"].string_value());
    setPassiveSkillDetail(json["passive_skill_detail"].string_value());
}

void PassiveSkillSprite::showSkillData()
{
    //スキル名
    auto skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(_passive_skill_name, FONT_NAME_SYSTEM, 24);
    skill_name->setHorizontalAlignment(TextHAlignment::LEFT);
    skill_name->setPosition(Point(200, 96));
    skill_name->setColor(Color3B(255, 204, 0));
    skill_name->setAnchorPoint(Point(0, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(skill_name, 2);
    
    //説明
    auto detail_label = HMSFontUtil::createWithTTFBySystemFontArgument(_passive_skill_detail, FONT_NAME_SYSTEM, 24);
    detail_label->setHorizontalAlignment(TextHAlignment::LEFT);
    detail_label->setDimensions(540, 80);
    detail_label->setAnchorPoint(Point(0, 1.0f));
    detail_label->setPosition(Point(14, 80));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(detail_label, 2);
}

void PassiveSkillSprite::onEnter()
{
    Sprite::onEnter();
}
