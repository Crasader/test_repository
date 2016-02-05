#include "QuestStageSprite.h"

QuestStageSprite::QuestStageSprite()
:_stage_id()
,_stage_level()
,_stage_rank_name()
,_stage_name()
,_stage_chara_id()
,_stage_chara_rare()
,_stage_npc_name()
,_gain_rank_point()
,_gain_reward_point()
,_gain_gold()
,_new_flg(false)
,_clear_flg(false)
,_need_power()
,_reward_text1()
,_reward_text2()
,_reward_text3()
,_reward_flg1()
,_reward_flg2()
,_reward_flg3()
{
}

QuestStageSprite* QuestStageSprite::create(json11::Json json, string name_or_reward)
{
    QuestStageSprite *sprite = new QuestStageSprite();
    
    sprite->_name_or_reward = name_or_reward;
    
    sprite->filename = "init/Home/Quest/story_title_s.png";
    if(name_or_reward=="reward") sprite->filename = "init/Home/Quest/story_title_s2.png";
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool QuestStageSprite::init(json11::Json json)
{
    if ( !Sprite::initWithFile(filename.c_str()) )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json);
    
    return true;
}

void QuestStageSprite::onEnter()
{
    Sprite::onEnter();
    
    if(_name_or_reward=="name") {
        //クエスト名の表示
        showQuestStageName();
        
        //必要体力の表示
        showNeedPower();
     
        //New画像の表示
        showNewSprite();
        
        //New画像のアクション
        actionNewSprite();
        
        //クリア済み画像の表示
        showClearSprite();
        
        //星マークの表示
        showStarSprite();
    } else {
        //報酬獲得条件の表示
        showReward();
    }
    
    //矢印の表示
    showArrow();
    
    //矢印のアクション
    actionArrow();
}

void QuestStageSprite::setupData(json11::Json json)
{
    //データの設定
    setStageId(json["quest_stage_id"].string_value());
    setStageLevel(json["quest_stage_level"].string_value());
    setStageRankName(json["rank_name"].string_value());
    setStageName(json["stage_name"].string_value());
    setStageCharaId(json["chara_id"].string_value());
    setStageCharaRare(json["rare"].string_value());
    setStageNpcName(json["npc_name"].string_value());
    setGainRankPoint(json["gain_rank_point"].string_value());
    setGainRewardPoint(json["gain_reward_point"].string_value());
    setGainGold(json["gain_gold"].string_value());
    
    setQuestStageClearFlg(json["clear_flg"].bool_value());
    setQuestStageNewFlg(json["new_flg"].bool_value());
    setNeedPower(json["need_power"].string_value());
    
    setRewardText1(json["reward_text1"].string_value());
    setRewardText2(json["reward_text2"].string_value());
    setRewardText3(json["reward_text3"].string_value());
    setRewardFlg1(json["reward_flg1"].int_value());
    setRewardFlg2(json["reward_flg2"].int_value());
    setRewardFlg3(json["reward_flg3"].int_value());
    
    HMSLOG("ステージ stage_id = %s, stage_level = %s, rank_name = %s, stage_name = %s, chara_id = %s, npc_name = %s", _stage_id.c_str(), _stage_level.c_str(), _stage_rank_name.c_str(), _stage_name.c_str(), _stage_chara_id.c_str(), _stage_npc_name.c_str());
}

void QuestStageSprite::showQuestStageName()
{
    //ステージ名
    auto stage_name = HMSFontUtil::createWithTTFBySystemFontArgument(_stage_name.c_str(), FONT_NAME_SYSTEM, 38);
    stage_name->enableOutline(Color4B(0, 0, 0, 255), 3);
    stage_name->setAnchorPoint(Point(0, 0.5));
    stage_name->setPosition(Point(80, this->getContentSize().height / 2));
    this->addChild(stage_name, 1);
}

void QuestStageSprite::showNeedPower()
{
    //必要体力
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("必要行動力 :", FONT_NAME_SYSTEM, 22);
    label->enableOutline(Color4B(0, 0, 0, 255), 3);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(330, 42));
    this->addChild(label, 1);
    
    //消費体力
    auto need_power = HMSFontUtil::createWithTTFBySystemFontArgument(_need_power, FONT_NAME_SYSTEM, 33);
    need_power->enableOutline(Color4B(0, 0, 0, 255), 3);
    need_power->setAnchorPoint(Point(1.0, 0.5));
    need_power->setPosition(Point(550, 40));
    this->addChild(need_power, 1);
    
    if (fromString<int>(SaveData::loadString("max_power")) < fromString<int>(_need_power))
    {
        //最大体力が必要行動力に足りていない場合、赤く点滅する処理
        auto tint1 = TintTo::create(1.0f, 220, 0, 0);
        auto tint2 = TintTo::create(0.5f, 255, 255, 255);
        auto repeat = RepeatForever::create(Sequence::create(tint1, tint2, DelayTime::create(0.4f), nullptr));
        
        label->runAction(repeat);
        need_power->runAction(repeat->clone());
    }
}

void QuestStageSprite::showNewSprite()
{
    if (_new_flg)
    {
        //Newだった場合画像の表示
        auto new_sp = Sprite::create("init/Home/Quest/new.png");
        new_sp->setAnchorPoint(Point(0, 0.5));
        new_sp->setPosition(Point(70, 114));
        this->addChild(new_sp, 10, STAGE_NEW_TAG);
    }
}

void QuestStageSprite::showClearSprite()
{
    if (_clear_flg)
    {
        //クリア済みだった場合画像の表示
        auto clear_sp = Sprite::create("init/Home/Quest/clear2.png");
        clear_sp->setPosition(Point(140, 114));
        this->addChild(clear_sp, 10);
    }
}

void QuestStageSprite::showArrow()
{
    //矢印の表示
    auto arrow_sp = Sprite::create("init/Home/Quest/arrow.png");
    arrow_sp->setPosition(Point(20, this->getContentSize().height / 2));
    this->addChild(arrow_sp, 2, STAGE_ARROW_TAG);
}

void QuestStageSprite::actionArrow()
{
    auto arrow_sp = (Sprite *)this->getChildByTag(STAGE_ARROW_TAG);
    
    if (arrow_sp) {
        auto scale   = ScaleTo::create(1.0f, 0.8);
        auto scale2  = ScaleTo::create(1.0f, 1.0);
        auto repeat  = RepeatForever::create(Sequence::create(scale, scale2, NULL));
        arrow_sp->runAction(repeat);
    }
}

void QuestStageSprite::actionNewSprite()
{
    auto new_sp = (Sprite *)this->getChildByTag(STAGE_NEW_TAG);
    
    if (new_sp) {
        auto fadeOut = FadeOut::create(0.8f);
        auto fadeIn  = FadeIn::create(0.4f);
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, fadeIn, DelayTime::create(1.0f), NULL));
        new_sp->runAction(repeat);
    }
}

//報酬一覧の表示
void QuestStageSprite::showReward()
{
    
    Label *label;
    Sprite *sprite;
    string reward_text_list[3];
    reward_text_list[0] = _reward_text1;
    reward_text_list[1] = _reward_text2;
    reward_text_list[2] = _reward_text3;
    int reward_flg_list[3];
    reward_flg_list[0] = _reward_flg1;
    reward_flg_list[1] = _reward_flg2;
    reward_flg_list[2] = _reward_flg3;
    
    float height = this->getContentSize().height;
    float width = this->getContentSize().width;
    bool no_flg = true;
    for(int i=0; i<3; i++) {
        if(reward_text_list[i]!="") {
            no_flg = false;
            //テキスト
            label = HMSFontUtil::createWithTTFBySystemFontArgument(reward_text_list[i], FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(reward_text_list[i], 24, 480));
            label->enableOutline(Color4B(0, 0, 0, 255), 3);
            label->setAnchorPoint(Point(0, 0.5));
            label->setPosition(Point(80, height*(3-i)/4.0f));
            this->addChild(label, 1);
            
            //星マーク
            string file_path = "init/Home/Quest/star.png";
            if(reward_flg_list[i]==0) {
                file_path = "init/Home/Quest/star_gray.png";;
            }
            sprite = Sprite::create(file_path);
            sprite->setPosition(Point(50, height*(3-i)/4.0f));
            this->addChild(sprite, 1);
        }
    }
    
    if(no_flg==true) {
        //テキスト
        label = HMSFontUtil::createWithTTFBySystemFontArgument("条件が設定されていません", FONT_NAME_SYSTEM, 24);
        label->enableOutline(Color4B(0, 0, 0, 255), 3);
        label->setAnchorPoint(Point(0.5, 0.5));
        label->setPosition(Point(width/2.0f, height/2.0f));
        this->addChild(label, 1);
    }
}

void QuestStageSprite::showStarSprite()
{
    //星画像の表示
    int reward_flg_list[3];
    reward_flg_list[0] = _reward_flg1;
    reward_flg_list[1] = _reward_flg2;
    reward_flg_list[2] = _reward_flg3;
    
    for (int i = 0; i < sizeof(reward_flg_list) / sizeof(reward_flg_list[0]); i++) {
        //星マーク
        string file_path = "init/Home/Quest/star_l.png";
        if(reward_flg_list[i]==0) {
            file_path = "init/Home/Quest/star_l_gray.png";;
        }
        auto star_sp = Sprite::create(file_path);
        star_sp->setPosition(Point(380 + (i * 60), 85));
        this->addChild(star_sp, 1);
    }
}

void QuestStageSprite::changeVisibleSprite(bool visible_flg)
{
    //矢印の表示切り替え
    auto arrow_sp = (Sprite *)this->getChildByTag(STAGE_ARROW_TAG);
    
    if (arrow_sp)
    {
        if (visible_flg) {
            arrow_sp->setVisible(true);
        }
        else {
            arrow_sp->setVisible(false);
        }
    }
}
