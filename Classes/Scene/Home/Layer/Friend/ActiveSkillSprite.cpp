#include "ActiveSkillSprite.h"

ActiveSkillSprite::ActiveSkillSprite()
:_active_skill_id()
,_active_skill_name()
,_active_skill_need_cost()
,_active_skill_detail()
{
}

ActiveSkillSprite* ActiveSkillSprite::create(){
    
    ActiveSkillSprite *sprite = new ActiveSkillSprite();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool ActiveSkillSprite::init()
{
    if ( !Sprite::initWithFile("init/Home/Friend/popup_skill_a.png") )
    {
        return false;
    }
    ratio = getVisibleRatio();
    
    return true;
}

void ActiveSkillSprite::setupSkillData(Json json)
{
    //データの保存
    setActiveSkillId(json["active_skill_id"].string_value());
    setActiveSkillName(json["active_skill_name"].string_value());
    setActiveSkillNeedCost(json["active_skill_need_cost"].string_value());
    setActiveSkillDetail(json["active_skill_detail"].string_value());
}

void ActiveSkillSprite::showSkillData()
{
    //スキル名
    auto skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(_active_skill_name, FONT_NAME_SYSTEM, 24);
    skill_name->setHorizontalAlignment(TextHAlignment::LEFT);
    skill_name->setPosition(Point(200, 96));
    skill_name->setColor(Color3B(255, 153, 0));
    skill_name->setAnchorPoint(Point(0, 0.5));
//    skill_name->enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(skill_name, 2);
    
    //発動コスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("発動士気", FONT_NAME_SYSTEM, 18);
    label->setPosition(Point(466, 92));
    label->setColor(Color3B(255, 153, 0));
//    label->enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(label, 2);
    
    _active_skill_need_cost = "18";
    auto need_cost = HMSFontUtil::createWithTTFBySystemFontArgument(_active_skill_need_cost, FONT_NAME_SYSTEM, 40);
    need_cost->setPosition(Point(534, 94));
    need_cost->setColor(Color3B(255, 153, 0));
//    need_cost->enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(need_cost, 2);
    
    //説明
    auto detail_label = HMSFontUtil::createWithTTFBySystemFontArgument(_active_skill_detail, FONT_NAME_SYSTEM, 24);
    detail_label->setHorizontalAlignment(TextHAlignment::LEFT);
//    detail_label->setDimensions(540, 80);
    detail_label->setDimensions(540, 80);
    detail_label->setAnchorPoint(Point(0, 1.0f));
    detail_label->setPosition(Point(14, 80));
//    detail_label->enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(detail_label, 2);
}

void ActiveSkillSprite::onEnter()
{
    Sprite::onEnter();
}
