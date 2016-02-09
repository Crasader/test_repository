#include "EventListSprite.h"

EventListSprite::EventListSprite()
:_npc_battle_event_rank_list_id()
,_event_rank_name()
,_event_rank_level()
,_need_power()
,_card_image_id()
,_user_name()
,_comment()
,_gold()
,_reward_point()
,_rank_point()
,_new_flg(false)
,_clear_flg(false)
,_reward_text1()
,_reward_text2()
,_reward_text3()
,_reward_flg1()
,_reward_flg2()
,_reward_flg3()
{
}

EventListSprite* EventListSprite::create(Json json, string name_or_reward)
{
    EventListSprite *sprite = new EventListSprite();
    
    sprite->_name_or_reward = name_or_reward;
    
    sprite->filename = "init/Home/Event/NpcBattleEvent/event_title_s.png";
    if(name_or_reward=="reward") sprite->filename = "init/Home/Event/NpcBattleEvent/event_title_s2.png";
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool EventListSprite::init(Json json)
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

void EventListSprite::onEnter()
{
    Sprite::onEnter();
    
    if(_name_or_reward=="name") {
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
        
        //星の表示
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


void EventListSprite::setData()
{
    //データの設定
    setNpcBattleEventRankListId(_event_rank_data["npc_battle_event_rank_list_id"].string_value());
    setEventRankName(_event_rank_data["rank_name"].string_value());
    setEventRankLevel(_event_rank_data["rank_level"].string_value());
    setNeedPower(_event_rank_data["need_power"].string_value());
    setCardImageId(_event_rank_data["chara_id"].string_value());
    setUserName(_event_rank_data["npc_name"].string_value());
    setComment(_event_rank_data["comment"].string_value());
    setGold(_event_rank_data["gold"].string_value());
    setRewardPoint(_event_rank_data["reward_point"].string_value());
    setRankPoint(_event_rank_data["rank_point"].string_value());
    setNewFlg(_event_rank_data["new_flg"].bool_value());
    setClearFlg(_event_rank_data["clear_flg"].bool_value());
    
    setRewardText1(_event_rank_data["reward_text1"].string_value());
    setRewardText2(_event_rank_data["reward_text2"].string_value());
    setRewardText3(_event_rank_data["reward_text3"].string_value());
    setRewardFlg1(_event_rank_data["reward_flg1"].int_value());
    setRewardFlg2(_event_rank_data["reward_flg2"].int_value());
    setRewardFlg3(_event_rank_data["reward_flg3"].int_value());
    
//    HMSLOG("event_id = %s, event_name = %s, rank_name = %s, need_power = %s",_npc_battle_event_rank_list_id.c_str(), _event_name.c_str(), _event_rank_name.c_str(), _need_power.c_str());
//    HMSLOG("chara_id = %s, npc_name = %s, comment = %s, gold = %s, reward_pont = %s, rank_point = %s", _card_image_id.c_str(), _user_name.c_str(), _comment.c_str(), _gold.c_str(), _reward_point.c_str(), _rank_point.c_str());
    
}

void EventListSprite::showRankName()
{
    //ランク名の表示
    if (fromString<int>(_event_rank_level) >= 4) {
        _event_rank_level = "4";
    }
    string image_path = "init/Home/Event/level" + _event_rank_level + ".png";
    auto rank_name = Sprite::create(image_path);
    rank_name->setPosition(Point(130, this->getContentSize().height / 2));
    this->addChild(rank_name, 1);
    
//    auto rank_name = HMSFontUtil::createWithTTFBySystemFontArgument(_event_rank_name.c_str(), FONT_NAME_SYSTEM, 38);
//    rank_name->enableOutline(Color4B(0, 0, 0, 255), 3);
//    rank_name->setAnchorPoint(Point(0, 0.5));
//    rank_name->setPosition(Point(80, this->getContentSize().height / 2));
//    this->addChild(rank_name, 1);
}

void EventListSprite::showNeedPower()
{
    //必要体力
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("必要行動力 :", FONT_NAME_SYSTEM, 22);
    label->enableOutline(Color4B(0, 0, 0, 255), 3);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(330, 42));
    this->addChild(label, 1, T_NEED_LABEL);
    
    //消費体力
    auto need_power = HMSFontUtil::createWithTTFBySystemFontArgument(_need_power, FONT_NAME_SYSTEM, 33);
    need_power->enableOutline(Color4B(0, 0, 0, 255), 3);
    need_power->setAnchorPoint(Point(1.0, 0.5));
    need_power->setPosition(Point(550, 40));
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

void EventListSprite::createRewardList()
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

string EventListSprite::getImagePass(Json reward_data)
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

void EventListSprite::createRewardSprite()
{
    for (int i = 0; i < _reward_data_list.size(); i++) {
        _reward_sp.push_back(Sprite::create(getImagePass(_reward_data_list[i])));
        _reward_sp[i]->setScale(0.8);
        _reward_sp[i]->setPosition(Point(FIRST_CARD_POS_X + (94 * i), FIRST_CARD_POS_Y));
        addChild(_reward_sp[i], 1);
    }
}

void EventListSprite::showNewSprite()
{
    if (_new_flg)
    {
        //Newだった場合画像の表示
        auto new_sp = Sprite::create("init/Home/Event/new.png");
        new_sp->setAnchorPoint(Point(0, 0.5));
        new_sp->setPosition(Point(70, 114));
        this->addChild(new_sp, 10, EventListSprite::TAG::NEW_TAG);
    }
}

void EventListSprite::actionNewSprite()
{
    auto new_sp = (Sprite *)this->getChildByTag(EventListSprite::TAG::NEW_TAG);
    
    if (new_sp) {
        auto fadeOut = FadeOut::create(0.8f);
        auto fadeIn  = FadeIn::create(0.4f);
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, fadeIn, DelayTime::create(1.0f), NULL));
        new_sp->runAction(repeat);
    }
}
void EventListSprite::showArrow()
{
    //矢印の表示
    auto arrow_sp = Sprite::create("init/Home/Event/arrow.png");
    arrow_sp->setPosition(Point(20, this->getContentSize().height / 2));
    this->addChild(arrow_sp, 2, EventListSprite::TAG::ARROW_TAG);
}

void EventListSprite::actionArrow()
{
    auto arrow_sp = (Sprite *)this->getChildByTag(EventListSprite::TAG::ARROW_TAG);
    
    if (arrow_sp) {
        auto scale   = ScaleTo::create(1.0f, 0.8);
        auto scale2  = ScaleTo::create(1.0f, 1.0);
        auto repeat  = RepeatForever::create(Sequence::create(scale, scale2, NULL));
        arrow_sp->runAction(repeat);
    }
}

void EventListSprite::changeVisibleSprite(bool visible_flg)
{
    //newと矢印の表示切り替え
//    auto new_sp = (Sprite *)this->getChildByTag(EventListSprite::TAG::NEW_TAG);
    auto arrow_sp = (Sprite *)this->getChildByTag(EventListSprite::TAG::ARROW_TAG);
    
    //必要行動力の文字と必要行動力を赤く点滅させる
    auto label = (Label *)this->getChildByTag(EventListSprite::TAG::T_NEED_LABEL);
    auto need_power = (Label *)this->getChildByTag(EventListSprite::TAG::T_NEED_POWER);
    
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
void EventListSprite::showReward()
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

void EventListSprite::showStarSprite()
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

void EventListSprite::showClearSprite()
{
    if (_clear_flg)
    {
        //クリア済みだった場合画像の表示
        auto clear_sp = Sprite::create("init/Home/Quest/clear2.png");
        clear_sp->setPosition(Point(130, 122));
        this->addChild(clear_sp, 10);
    }
}