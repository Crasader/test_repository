#include "EventEnemySprite.h"

EventEnemySprite::EventEnemySprite()
:_user_master_id ()
,_card_image_id ()
,_rank_name ()
,_level()
,_user_name ()
,_battle_point_rate()
,_offense()
,_defense()
,_strategist()
,_general()
,_json()
{
}

EventEnemySprite* EventEnemySprite::create(json11::Json json){
    
    EventEnemySprite *sprite = new EventEnemySprite();
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool EventEnemySprite::init(json11::Json json)
{
    if ( !Sprite::initWithFile("init/Home/Quest/popup_opponent_red.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json);
    
    return true;
}

void EventEnemySprite::setupData(json11::Json json)
{
    //ユーザー情報の設定
    setUserMasterId(json["user_master_id"].string_value());
    setCardImageId(json["card_image_id"].string_value());
    setRankName(json["rank_name"].string_value());
    setLevel(json["level"].string_value());
    setUserName(json["user_name"].string_value());
    setBattlePointRate(json["battle_point_rate"].string_value());
    setOffense(json["offense"].string_value());
    setDefense(json["defense"].string_value());
    setStrategist(json["strategist"].string_value());
    setGeneral(json["general"].string_value());
    
    setJson(json.dump());
}

void EventEnemySprite::showCardImage()
{
    string card_pass = "download/Chara/parts100/card_bk1_reverse.png";
    if(_card_image_id != "") {
        card_pass = "download/Chara/thumb100/"+_card_image_id+".png";
    }
    
    _card_image = Sprite::create(card_pass);
    _card_image->setPosition(Point(70, this->getContentSize().height /2));
    addChild(_card_image, 1);
}

void EventEnemySprite::showData()
{
    //カードの表示
    showCardImage();
    
    if(_rank_name != ""){
        //階級名
        auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_name, FONT_NAME_SYSTEM, 24);
        rank_label->setTextColor(Color4B(255, 204, 0, 255));
        rank_label->setAnchorPoint(Point(0, 0.5));
        rank_label->setPosition(Point(126, 88));
        this->addChild(rank_label, 1);
        
        //5文字以上の場合はスケールを小さくする
        if (calcStrLength(_rank_name) == 5) {
            rank_label->setScale(0.8);
        }
        else if (calcStrLength(_rank_name) >= 6) {
            rank_label->setScale(0.7);
        }
    }
    
    if(_user_name != ""){
        //ユーザー名
        auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_name.c_str(), FONT_NAME_SYSTEM, 30);
        name_label->setHorizontalAlignment(TextHAlignment::LEFT);
        name_label->setAnchorPoint(Point(0, 0.5));
        name_label->setPosition(Point(230, 88));
        name_label->enableOutline(Color4B(0, 0, 0, 255), 2);
        this->addChild(name_label, 2);
    }
}

void EventEnemySprite::showDeckParam()
{
    //攻撃
    auto offense_label = HMSFontUtil::createWithTTFBySystemFontArgument(_offense, FONT_NAME_SYSTEM, 30);
    offense_label->setPosition(Point(232, 14));
    offense_label->setAnchorPoint(Point(1.0, 0));
    offense_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(offense_label, 2);
    
    //防御
    auto defense_label = HMSFontUtil::createWithTTFBySystemFontArgument(_defense, FONT_NAME_SYSTEM, 30);
    defense_label->setPosition(Point(346, 14));
    defense_label->setAnchorPoint(Point(1.0, 0));
    defense_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(defense_label, 2);
    
    //軍師力
    auto strategist_label = HMSFontUtil::createWithTTFBySystemFontArgument(_strategist, FONT_NAME_SYSTEM, 30);
    strategist_label->setPosition(Point(459, 14));
    strategist_label->setAnchorPoint(Point(1.0, 0));
    strategist_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(strategist_label, 2);
}

void EventEnemySprite::showMagnification()
{
    //獲得経験値
    string event_reward_point = SaveData::loadString("event_reward_point");
    auto reward_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(event_reward_point, FONT_NAME_SYSTEM, 26);
    reward_point_label->setPosition(Point(555, 20));
    reward_point_label->setAnchorPoint(Point(0.5, 0));
    reward_point_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(reward_point_label, 2);
}

void EventEnemySprite::onEnter()
{
    Sprite::onEnter();
    
    //データの表示
    showData();
    
    //デッキパラメータの表示
    showDeckParam();
    
    //獲得倍率の表示
    showMagnification();
}