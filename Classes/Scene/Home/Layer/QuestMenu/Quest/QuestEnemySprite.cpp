#include "QuestEnemySprite.h"

QuestEnemySprite::QuestEnemySprite()
:_chara_id ()
,_rank_name ()
,_rare ()
,_npc_name ()
,_battle_point_rate()
,_offense()
,_defense()
,_strategist()
,_general()
{
}

QuestEnemySprite* QuestEnemySprite::create(json11::Json json){
    
    QuestEnemySprite *sprite = new QuestEnemySprite();
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool QuestEnemySprite::init(json11::Json json)
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

void QuestEnemySprite::setupData(json11::Json json)
{
    //スプライト情報の設定
    setCharaId(json["chara_id"].string_value());
    setRankName(json["rank_name"].string_value());
    setRare(json["rare"].string_value());
    setNpcName(json["npc_name"].string_value());
    setBattlePointRate(json["battle_point_rate"].string_value());
    setOffense(json["offense"].string_value());
    setDefense(json["defense"].string_value());
    setStrategist(json["strategist"].string_value());
    setGeneral(json["general"].string_value());
}

void QuestEnemySprite::showCardImage()
{
    if (_chara_id == "" || _rare == "")
    {
        //カード情報が取れなかった場合
        auto card = Sprite::create("download/Chara/parts100/card_bk1_reverse.png");
        card->setPosition(Point(70, this->getContentSize().height /2));
        addChild(card, 1);
    }
    else {
        //キャラ背景
        auto chara_bg = Sprite::create("download/Chara/parts100/card_bk" + _rare + ".png");
        chara_bg->setPosition(Point(70, this->getContentSize().height /2));
        addChild(chara_bg, 1);
        
        //キャラ画像
        auto chara = Sprite::create("download/Chara/chara100/" + _chara_id+ ".png");
        chara->setPosition(Point(chara_bg->getContentSize() / 2));
        chara_bg->addChild(chara, 1);
        
        //レア度
        auto chara_rare = Sprite::create("download/Chara/parts100/card_rare" + _rare + ".png");
        chara_rare->setPosition(Point(chara_bg->getContentSize() / 2));
        chara_bg->addChild(chara_rare, 2);
    }
}

void QuestEnemySprite::showData()
{
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
    
    if(_npc_name != ""){
        //ユーザー名
        auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_npc_name.c_str(), FONT_NAME_SYSTEM, 30);
        name_label->setHorizontalAlignment(TextHAlignment::LEFT);
        name_label->setAnchorPoint(Point(0, 0.5));
        name_label->setPosition(Point(230, 88));
        name_label->enableOutline(Color4B(0, 0, 0, 255), 2);
        this->addChild(name_label, 2);
    }
}

void QuestEnemySprite::showDeckParam()
{
    //攻撃
    auto offense_label = HMSFontUtil::createWithTTFBySystemFontArgument(_offense, FONT_NAME_SYSTEM, 30);
    offense_label->setPosition(Point(231, 18));
    offense_label->setAnchorPoint(Point(1.0, 0));
    offense_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(offense_label, 2);
    
    //防御
    auto defense_label = HMSFontUtil::createWithTTFBySystemFontArgument(_defense, FONT_NAME_SYSTEM, 30);
    defense_label->setPosition(Point(345, 18));
    defense_label->setAnchorPoint(Point(1.0, 0));
    defense_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(defense_label, 2);
    
    //軍師力
    auto strategist_label = HMSFontUtil::createWithTTFBySystemFontArgument(_strategist, FONT_NAME_SYSTEM, 30);
    strategist_label->setPosition(Point(459, 18));
    strategist_label->setAnchorPoint(Point(1.0, 0));
    strategist_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(strategist_label, 2);
}

void QuestEnemySprite::showExperience()
{
    //獲得経験値
    string reward_point = SaveData::loadString("quest_reward_point");
    auto reward_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(reward_point, FONT_NAME_SYSTEM, 26);
    reward_point_label->setPosition(Point(555, 20));
    reward_point_label->setAnchorPoint(Point(0.5, 0));
    reward_point_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(reward_point_label, 2);
}

void QuestEnemySprite::onEnter()
{
    Sprite::onEnter();
    
    //カードの表示
    showCardImage();
    
    //データの表示
    showData();
    
    //デッキパラメータの表示
    showDeckParam();
    
    //獲得倍率の表示
    showExperience();
}
