#include "ChallengeEventListSprite.h"

ChallengeEventListSprite::ChallengeEventListSprite()
:_challenge_event_stage_id()
,_rank_name()
,_rank_level()
,_need_power()
,_card_image_id()
,_user_name()
,_comment()
,_gold()
,_reward_point()
,_rank_point()
,_reward_text()
,_reward_clear_flg()
,_new_flg(false)
,_clear_flg(false)
{
}

ChallengeEventListSprite* ChallengeEventListSprite::create(Json json)
{
    ChallengeEventListSprite *sprite = new ChallengeEventListSprite();
    
    sprite->filename = "init/Home/Event/ChallengeEvent/event_title_s.png";

    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool ChallengeEventListSprite::init(Json json)
{
    if ( !Sprite::initWithFile(filename.c_str()))
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //メンバ変数に保存
    _event_rank_data = json;
    
    //データの設定
    setData();
    
    return true;
}

void ChallengeEventListSprite::onEnter()
{
    Sprite::onEnter();
    
    //ランク名の表示
    showRankName();
    
    //必要行動力の表示
    showNeedPower();
    
    //報酬カードの作成、表示
    createRewardSprite();
    
    //New画像の表示
    showNewSprite();
    
    //New画像のアクション
    actionNewSprite();
    
    //クリア済みの画像の表示
    showClearSprite();

    //矢印の表示
    showArrow();
    
    //矢印のアクション
    actionArrow();
}


void ChallengeEventListSprite::setData()
{
    //データの設定
    setChallengeEventStageId(_event_rank_data["challenge_event_stage_id"].string_value());
    setRankName(_event_rank_data["rank_name"].string_value());
    setRankLevel(_event_rank_data["challenge_event_stage_level"].string_value());
    setNeedPower(_event_rank_data["need_power"].string_value());
    setCardImageId(_event_rank_data["chara_id"].string_value());
    setUserName(_event_rank_data["npc_name"].string_value());
    setComment(_event_rank_data["comment"].string_value());
    setRewardPoint(_event_rank_data["reward_point"].string_value());
    setRewardText(_event_rank_data["reward_text"].string_value());
    setRewardClearFlg(_event_rank_data["reward_clear_flg"].string_value());
    setNewFlg(_event_rank_data["new_flg"].bool_value());
    setClearFlg(_event_rank_data["clear_flg"].bool_value());
}

void ChallengeEventListSprite::showRankName()
{
    //ランク名の表示
    if (fromString<int>(_rank_level) >= 4) {
        _rank_level = "4";
    }
    string image_path = "init/Home/Event/level" + _rank_level + ".png";
    auto rank_name = Sprite::create(image_path);
    rank_name->setPosition(Point(106, this->getContentSize().height/2+52));
    this->addChild(rank_name, 1);
    
//    auto rank_name = HMSFontUtil::createWithTTFBySystemFontArgument(_event_rank_name.c_str(), FONT_NAME_SYSTEM, 38);
//    rank_name->enableOutline(Color4B(0, 0, 0, 255), 3);
//    rank_name->setAnchorPoint(Point(0, 0.5));
//    rank_name->setPosition(Point(80, this->getContentSize().height / 2));
//    this->addChild(rank_name, 1);
}

void ChallengeEventListSprite::showNeedPower()
{
    //必要体力
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("必要行動力 :", FONT_NAME_SYSTEM, 22);
    label->enableOutline(Color4B(0, 0, 0, 255), 3);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(330, this->getContentSize().height/2+48));
    this->addChild(label, 1, T_NEED_LABEL);
    
    //消費体力
    auto need_power = HMSFontUtil::createWithTTFBySystemFontArgument(_need_power, FONT_NAME_SYSTEM, 33);
    need_power->enableOutline(Color4B(0, 0, 0, 255), 3);
    need_power->setAnchorPoint(Point(1.0, 0.5));
    need_power->setPosition(Point(550, this->getContentSize().height/2+46));
    this->addChild(need_power, 1, T_NEED_POWER);
    
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

void ChallengeEventListSprite::createRewardList()
{
    //報酬一覧の設定
    int event_reward_list_count = countJsonData(_event_rank_data["event_reward_list"].object_items()) ;
    for (int i = 0; i < event_reward_list_count; i++) {
        string num = toString<int>(i);
        _reward_data_list.push_back(_event_rank_data["event_reward_list"][num].object_items());
    }
    
    //報酬一覧の並び替え (報酬がchara且つレアリティの高いものが左にいくように指定)
    sort(_reward_data_list.begin(), _reward_data_list.end(), [](const Json &json1, const Json &json2){
        if (json1["item_type"].string_value() == "chara" && fromString<int>(json1["rare"].string_value()) > fromString<int>(json2["rare"].string_value())) {
            return true;
        } else {
            return false;
        }
    });
}

string ChallengeEventListSprite::getImagePass(Json reward_data)
{
    string image_pass = "download/Chara/parts100/card_bk1_reverse.png";
    if (reward_data["item_type"].string_value() == "chara" && reward_data["related_primary_id"].string_value() != "0")
    {
        image_pass = "download/Chara/thumb100/" + reward_data["related_primary_id"].string_value() + ".png";
    }
    else if (reward_data["item_type"] == "gold")
    {
        image_pass = "init/Present/item2.png";
    }
    else if (reward_data["item_type"] == "jewel")
    {
        image_pass = "init/Present/item.png";
    }
    return image_pass;
}

void ChallengeEventListSprite::createRewardSprite()
{
    for (int i = 0; i < _reward_data_list.size(); i++) {
        _reward_sp.push_back(Sprite::create(getImagePass(_reward_data_list[i])));
        _reward_sp[i]->setScale(0.8);
        _reward_sp[i]->setPosition(Point(FIRST_CARD_POS_X + (94 * i), FIRST_CARD_POS_Y));
        addChild(_reward_sp[i], 1);
    }
    
    HmsVector<string> reward_text_list = split(_reward_text, ":");
    HmsVector<string> reward_clear_flg_list = split(_reward_clear_flg, ":");
    for(int i=0; i<reward_text_list.size()-1; i++) {
        string reward_text = reward_text_list.at(i);
        string reward_clear_flg = reward_clear_flg_list.at(i);
        HMSLOG("reward_text=%s,reward_clear_flg=%s", reward_text.c_str(), reward_clear_flg.c_str());
        
        int y = this->getContentSize().height/2-3-i*32;
        //報酬テキスト
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(reward_text, FONT_NAME_SYSTEM, 24);
        label->enableOutline(Color4B(0, 0, 0, 255), 3);
        label->setAnchorPoint(Point(0, 0.5));
        label->setPosition(Point(132, y));
        this->addChild(label, 1);
        
        if(reward_clear_flg=="1") {
            //クリア済みだった場合画像の表示
            auto clear_sp = Sprite::create("init/Home/Event/ChallengeEvent/clear.png");
            //clear_sp->setScale(0.5);
            clear_sp->setPosition(Point(81, y));
            this->addChild(clear_sp, 10);
        }
    }
}

void ChallengeEventListSprite::showNewSprite()
{
    if (_new_flg)
    {
        //Newだった場合画像の表示
        auto new_sp = Sprite::create("init/Home/Event/new.png");
        new_sp->setAnchorPoint(Point(0, 0.5));
        new_sp->setPosition(Point(45, this->getContentSize().width/2+130));
        this->addChild(new_sp, 10, ChallengeEventListSprite::TAG::NEW_TAG);
    }
}

void ChallengeEventListSprite::actionNewSprite()
{
    auto new_sp = (Sprite *)this->getChildByTag(ChallengeEventListSprite::TAG::NEW_TAG);
    
    if (new_sp) {
        auto fadeOut = FadeOut::create(0.8f);
        auto fadeIn  = FadeIn::create(0.4f);
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, fadeIn, DelayTime::create(1.0f), NULL));
        new_sp->runAction(repeat);
    }
}
void ChallengeEventListSprite::showArrow()
{
    //矢印の表示
    auto arrow_sp = Sprite::create("init/Home/Event/arrow.png");
    arrow_sp->setPosition(Point(20, this->getContentSize().height / 2));
    this->addChild(arrow_sp, 2, ChallengeEventListSprite::TAG::ARROW_TAG);
}

void ChallengeEventListSprite::actionArrow()
{
    auto arrow_sp = (Sprite *)this->getChildByTag(ChallengeEventListSprite::TAG::ARROW_TAG);
    
    if (arrow_sp) {
        auto scale   = ScaleTo::create(1.0f, 0.8);
        auto scale2  = ScaleTo::create(1.0f, 1.0);
        auto repeat  = RepeatForever::create(Sequence::create(scale, scale2, NULL));
        arrow_sp->runAction(repeat);
    }
}

void ChallengeEventListSprite::changeVisibleSprite(bool visible_flg)
{
    //newと矢印の表示切り替え
//    auto new_sp = (Sprite *)this->getChildByTag(ChallengeEventListSprite::TAG::NEW_TAG);
    auto arrow_sp = (Sprite *)this->getChildByTag(ChallengeEventListSprite::TAG::ARROW_TAG);
    
    //必要行動力の文字と必要行動力を赤く点滅させる
    auto label = (Label *)this->getChildByTag(ChallengeEventListSprite::TAG::T_NEED_LABEL);
    auto need_power = (Label *)this->getChildByTag(ChallengeEventListSprite::TAG::T_NEED_POWER);
    
    if (arrow_sp && label && need_power)
    {
        if (visible_flg) {
//            new_sp->setVisible(true);
            arrow_sp->setVisible(true);
            
        }
        else {
//            new_sp->setVisible(false);
            arrow_sp->setVisible(false);
        }
    }
}

//報酬一覧の表示
void ChallengeEventListSprite::showReward()
{
    /*
    Label *label;
    Sprite *sprite;
    
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
     */
}

void ChallengeEventListSprite::showClearSprite()
{
    if (_clear_flg)
    {
        //クリア済みだった場合画像の表示
        //auto clear_sp = Sprite::create("init/Home/Quest/clear2.png");
        //clear_sp->setPosition(Point(110, this->getContentSize().width/2+130));
        //this->addChild(clear_sp, 10);
    }
}
