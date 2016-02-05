#include "BossEventListSprite.h"
#include "HmsUtilityFunction.h"

#define BOSS_EVENT_LIST_SPRITE_LABEL_TAG 11
#define BOSS_EVENT_LIST_SPRITE_BACK_TAG 12

BossEventListSprite::BossEventListSprite()
:_boss_event_id()
,_boss_event_stage_log_id()
,_boss_name()
,_comment_line1()
,_comment_line2()
,_boss_event_stage_level()
,_hp()
,_rest_time()
,_need_power()
,_card_image_id()
,_user_name()
,_comment()
,_gold()
,_reward_point()
,_rank_point()
,_new_flg(false)
,_clear_flg(false)
,_open_flg(false)
{
}

BossEventListSprite* BossEventListSprite::create(Json json, string name_or_reward)
{
    BossEventListSprite *sprite = new BossEventListSprite();
    
    sprite->_name_or_reward = name_or_reward;
    
    sprite->filename = "init/Home/Event/BossEvent/event_title_s.png";
    if(name_or_reward=="reward") sprite->filename = "init/Home/Event/NpcBattleEvent/event_title_s2.png";
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool BossEventListSprite::init(Json json)
{
    if ( !Sprite::initWithFile(filename.c_str()))
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //メンバ変数に保存
    _boss_event_stage_data = json;
    HMSLOG("boss_event_stage_data=%s", _boss_event_stage_data.dump().c_str());
    
    //データの設定
    setData();
    
    return true;
}

void BossEventListSprite::onEnter()
{
    Sprite::onEnter();
    
    if(_name_or_reward=="name") {
        //テキストの表示
        showText();
        
        //報酬カードの作成、表示
        createRewardSprite();
        
        //New画像の表示
        showNewSprite();
        
        //New画像のアクション
        actionNewSprite();
        
        //クリア済みの画像の表示
        showClearSprite();
        
        //鍵画像の作成
        createKey();
        
    } else {
        //報酬獲得条件の表示
    }
    
    //矢印の表示
    showArrow();
    
    //矢印のアクション
    actionArrow();
}


void BossEventListSprite::setData()
{
    //データの設定
    setBossEventId(_boss_event_stage_data["boss_event_id"].string_value());
    setBossEventStageLogId(_boss_event_stage_data["boss_event_stage_log_id"].string_value());
    setBossName(_boss_event_stage_data["boss_name"].string_value());
    setCommentLine1(_boss_event_stage_data["comment_line1"].string_value());
    setCommentLine2(_boss_event_stage_data["comment_line2"].string_value());
    setPlayerCount(_boss_event_stage_data["player_count"].int_value());
    setBossEventStageLevel(_boss_event_stage_data["boss_event_stage_level"].string_value());
    setFindUserName(_boss_event_stage_data["user_name"].string_value());
    setHp(_boss_event_stage_data["hp"].int_value());
    setMaxHp(_boss_event_stage_data["max_hp"].int_value());
    setEndTime(fromString<long>(_boss_event_stage_data["end_time"].string_value()));
    setNeedPower(_boss_event_stage_data["need_power"].string_value());
    setCardImageId(_boss_event_stage_data["chara_id"].string_value());
    setUserName(_boss_event_stage_data["npc_name"].string_value());
    setComment(_boss_event_stage_data["comment"].string_value());
    setGold(_boss_event_stage_data["gold"].string_value());
    setRewardPoint(_boss_event_stage_data["reward_point"].string_value());
    setRankPoint(_boss_event_stage_data["rank_point"].string_value());
    setNewFlg(_boss_event_stage_data["new_flg"].bool_value());
    setClearFlg(_boss_event_stage_data["clear_flg"].bool_value());
    setOpenFlg(_boss_event_stage_data["open_flg"].bool_value());
    
//    HMSLOG("event_id = %s, event_name = %s, rank_name = %s, need_power = %s",_boss_event_stage_id.c_str(), _event_name.c_str(), _event_rank_name.c_str(), _need_power.c_str());
//    HMSLOG("chara_id = %s, npc_name = %s, comment = %s, gold = %s, reward_pont = %s, rank_point = %s", _card_image_id.c_str(), _user_name.c_str(), _comment.c_str(), _gold.c_str(), _reward_point.c_str(), _rank_point.c_str());

    /*
    HMSLOG("-------BossEventListSprite 保有情報----------") ;
    HMSLOG("getBossEventStageId:%s",getBossEventStageId().c_str());
    HMSLOG("getEventRankLevel:%s",getEventRankLevel().c_str());
    HMSLOG("getFloorCount:%s",getFloorCount().c_str());
    HMSLOG("getEventRankName:%s",getEventRankName().c_str());
    HMSLOG("getNeedPower:%s",getNeedPower().c_str());
    HMSLOG("getCardImageId:%s",getCardImageId().c_str());
    HMSLOG("getUserName:%s",getUserName().c_str());
    HMSLOG("getComment:%s",getComment().c_str());
    HMSLOG("getGold:%s",getGold().c_str());
    HMSLOG("getRewardPoint:%s",getRewardPoint().c_str());
    HMSLOG("getRankPoint:%s",getRankPoint().c_str());
    HMSLOG("getNewFlg:%d",getNewFlg());
    HMSLOG("getClearFlg:%d",getClearFlg());
    HMSLOG("getFloorSumInSameLevel:%s",getFloorSumInSameLevel().c_str());
    HMSLOG("------------------------------------------------") ;
     */
    
}

void BossEventListSprite::showText()
{
    //コメント1
    auto comment_line1 = HMSFontUtil::createWithTTFBySystemFontArgument(_comment_line1, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_comment_line1, 24, 240));
    comment_line1->enableOutline(Color4B(0, 0, 0, 255), 3);
    comment_line1->setAnchorPoint(Point(0, 0.5));
    comment_line1->setPosition(Point(50, this->getContentSize().height/2+20));
    this->addChild(comment_line1, 0);
    
    //コメント2
    auto comment_line2 = HMSFontUtil::createWithTTFBySystemFontArgument(_comment_line2, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_comment_line2, 24, 240));
    comment_line2->enableOutline(Color4B(0, 0, 0, 255), 3);
    comment_line2->setAnchorPoint(Point(0, 0.5));
    comment_line2->setPosition(Point(50, this->getContentSize().height/2-20));
    this->addChild(comment_line2, 0);
    
    //参加人数の表示
    if(_player_count>0) {
        string filename = "init/Home/Event/BossEvent/bt_num.png";
        Sprite *img = Sprite::create(filename.c_str());
        img->setPosition(Point(320, this->getContentSize().height/2+58));
        this->addChild(img, 0);
        
        string text = toString(_player_count) + "人参加中";
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 24, 240));
        label->enableOutline(Color4B(0, 0, 0, 255), 3);
        label->setAnchorPoint(Point(0.5, 0.5));
        label->setPosition(Point(320, this->getContentSize().height/2+58));
        this->addChild(label, 0);
    }
    
//    //ボスの名前の表示
//    auto label_boss = HMSFontUtil::createWithTTFBySystemFontArgument(_boss_name, FONT_NAME_SYSTEM, 24);
//    label_boss->enableOutline(Color4B(0, 0, 0, 255), 3);
//    label_boss->setAnchorPoint(Point(0, 0.5));
//    label_boss->setPosition(Point(50, this->getContentSize().height/2+20));
//    this->addChild(label_boss, 0);
//
    
//    //発見ユーザの表示
//    auto label_user_name = HMSFontUtil::createWithTTFBySystemFontArgument(_find_user_name, FONT_NAME_SYSTEM, 24);
//    label_user_name->enableOutline(Color4B(0, 0, 0, 255), 3);
//    label_user_name->setAnchorPoint(Point(0, 0.5));
//    label_user_name->setPosition(Point(50, this->getContentSize().height/2-20));
//    this->addChild(label_user_name, 0);
    
    //残り時間
    updateRestTime(0.0f);
    this->schedule(schedule_selector(BossEventListSprite::updateRestTime), 1.0f);
    
    
//    //ランク名の表示
//    if (fromString<int>(_boss_event_stage_level) >= 5) {
//        _boss_event_stage_level = "5";
//    }
//    string image_path = "init/Home/Event/level" + _boss_event_stage_level + ".png";
//    auto rank_name = Sprite::create(image_path);
//    rank_name->setPosition(Point(106, this->getContentSize().height/2));
//    this->addChild(rank_name, 1);

//    //HP
//    auto label_hp = HMSFontUtil::createWithTTFBySystemFontArgument("HP:" + toString(_hp) + "/" + toString(_max_hp), FONT_NAME_SYSTEM, 24);
//    label_hp->enableOutline(Color4B(0, 0, 0, 255), 3);
//    label_hp->setAnchorPoint(Point(1, 0.5));
//    label_hp->setPosition(Point(540, this->getContentSize().height/2-20));
//    this->addChild(label_hp, 0);
    
}

void BossEventListSprite::createKey()
{
    _key = Sprite::create("init/Home/Quest/bt_battle_sellect_key.png") ;
    _key->setVisible(false) ;
    _key->setPosition(this->getContentSize()/2) ;
    this->addChild(_key) ;
}
void BossEventListSprite::showKey()
{
    _key->setVisible(true);
}

/*
void BossEventListSprite::showNeedPower()
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
    
    if (fromString<int>(SaveData::loadString("max_event_power")) < fromString<int>(_need_power))
    {
        //最大体力が必要行動力に足りていない場合、赤く点滅する処理
        auto tint1 = TintTo::create(1.0f, 220, 0, 0);
        auto tint2 = TintTo::create(0.5f, 255, 255, 255);
        auto repeat = RepeatForever::create(Sequence::create(tint1, tint2, DelayTime::create(0.4f), nullptr));
        
        label->runAction(repeat);
        need_power->runAction(repeat->clone());
    }
}
 */

/*
void BossEventListSprite::showFloorSumInSameLevel() {
    //「ステージ数」と表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("ステージ数 :", FONT_NAME_SYSTEM, 22);
    label->enableOutline(Color4B(0, 0, 0, 255), 3);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(330, this->getContentSize().height/2));
    this->addChild(label, 1);
    

    //ステージ数の表示
    auto floor_sum = HMSFontUtil::createWithTTFBySystemFontArgument("1", FONT_NAME_SYSTEM, 33);
    floor_sum->enableOutline(Color4B(0, 0, 0, 255), 3);
    floor_sum->setAnchorPoint(Point(1.0, 0.5));
    floor_sum->setPosition(Point(550, this->getContentSize().height/2));
    this->addChild(floor_sum, 1);
}
 */

void BossEventListSprite::createRewardList()
{
    //報酬一覧の設定
    int event_reward_list_count = countJsonData(_boss_event_stage_data["event_reward_list"].object_items()) ;
    for (int i = 0; i < event_reward_list_count; i++) {
        string num = toString<int>(i);
        _reward_data_list.push_back(_boss_event_stage_data["event_reward_list"][num].object_items());
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

string BossEventListSprite::getImagePass(Json reward_data)
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

void BossEventListSprite::createRewardSprite()
{
    for (int i = 0; i < _reward_data_list.size(); i++) {
        _reward_sp.push_back(Sprite::create(getImagePass(_reward_data_list[i])));
        _reward_sp[i]->setScale(0.8);
        _reward_sp[i]->setPosition(Point(FIRST_CARD_POS_X + (94 * i), FIRST_CARD_POS_Y));
        addChild(_reward_sp[i], 1);
    }
}

void BossEventListSprite::showNewSprite()
{
    if (_new_flg)
    {
        //Newだった場合画像の表示
        auto new_sp = Sprite::create("init/Home/Event/new.png");
        new_sp->setAnchorPoint(Point(0, 0.5));
        new_sp->setPosition(Point(70, 114));
        this->addChild(new_sp, 10, BossEventListSprite::TAG::NEW_TAG);
    }
}

void BossEventListSprite::actionNewSprite()
{
    auto new_sp = (Sprite *)this->getChildByTag(BossEventListSprite::TAG::NEW_TAG);
    
    if (new_sp) {
        auto fadeOut = FadeOut::create(0.8f);
        auto fadeIn  = FadeIn::create(0.4f);
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, fadeIn, DelayTime::create(1.0f), NULL));
        new_sp->runAction(repeat);
    }
}
void BossEventListSprite::showArrow()
{
    //矢印の表示
    auto arrow_sp = Sprite::create("init/Home/Event/arrow.png");
    arrow_sp->setPosition(Point(20, this->getContentSize().height / 2));
    this->addChild(arrow_sp, 2, BossEventListSprite::TAG::ARROW_TAG);
}

void BossEventListSprite::actionArrow()
{
    auto arrow_sp = (Sprite *)this->getChildByTag(BossEventListSprite::TAG::ARROW_TAG);
    
    if (arrow_sp) {
        auto scale   = ScaleTo::create(1.0f, 0.8);
        auto scale2  = ScaleTo::create(1.0f, 1.0);
        auto repeat  = RepeatForever::create(Sequence::create(scale, scale2, NULL));
        arrow_sp->runAction(repeat);
    }
}

void BossEventListSprite::changeVisibleSprite(bool visible_flg)
{
    //newと矢印の表示切り替え
//    auto new_sp = (Sprite *)this->getChildByTag(BossEventListSprite::TAG::NEW_TAG);
    auto arrow_sp = (Sprite *)this->getChildByTag(BossEventListSprite::TAG::ARROW_TAG);
    
    //必要行動力の文字と必要行動力を赤く点滅させる
    auto label = (Label *)this->getChildByTag(BossEventListSprite::TAG::T_NEED_LABEL);
    auto need_power = (Label *)this->getChildByTag(BossEventListSprite::TAG::T_NEED_POWER);
    
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

void BossEventListSprite::showClearSprite()
{
    if (_clear_flg)
    {
        //クリア済みだった場合画像の表示
        auto clear_sp = Sprite::create("init/Home/Quest/clear2.png");
        clear_sp->setPosition(Point(130, 122));
        this->addChild(clear_sp, 10);
    }
}

string BossEventListSprite::convertRestTime(long rest_time)
{
    //残り時間が1日以上の時秒数を表示しない
    bool second_display_flg = true ;
    if(rest_time > 86400 ) second_display_flg = false ;
    
    HmsVector<int> unit_list;
    HmsVector<string> reps_list;
    unit_list.insert(86400);
    unit_list.insert(3600);
    unit_list.insert(60);
    reps_list.insert("日");
    reps_list.insert("時間");
    reps_list.insert("分");
    
    string text = "";
    for(int i=0; i<3; i++) {
        int unit = unit_list.at(i);
        string reps = reps_list.at(i);
        if(rest_time>unit) {
            int count = rest_time / unit;
            text += toString(count) + reps;
            rest_time -= unit * count;
        }
    }
    
    if(second_display_flg)text += toString(rest_time) + "秒";
    return text;
}

void BossEventListSprite::updateRestTime(float dt)
{
    long now_time = (long)getNowTime();
    _rest_time = _end_time - now_time;
    //HMSLOG("now_time=%d, end_time=%d, rest_time=%d", now_time, _end_time, _rest_time);
    string rest_time_str = convertRestTime(_rest_time);
    
    //残り時間
    this->removeChildByTag(BOSS_EVENT_LIST_SPRITE_BACK_TAG);
    Sprite *back = Sprite::create("init/Home/Event/BossEvent/time_left.png") ;
    back->setAnchorPoint(Point(1, 0.5));
    back->setPosition(Point(540, this->getContentSize().height/2));
    this->addChild(back, 0, BOSS_EVENT_LIST_SPRITE_BACK_TAG);
    
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(rest_time_str, FONT_NAME_SYSTEM, 23);
    label->setPosition(Point(back->getContentSize().width/2, 29)) ;
    label->enableOutline(Color4B(0, 0, 0, 255), 3);
    back->addChild(label, 0, BOSS_EVENT_LIST_SPRITE_LABEL_TAG);
}
