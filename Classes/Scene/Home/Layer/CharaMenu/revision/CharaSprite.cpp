#include "CharaSprite.h"

CharaSprite::CharaSprite()
:_chara_list_id()
,_chara_id()
,_chara_no()
,_second_name()
,_name()
,_name_kana()
,_rare()
,_chara_type(CharaType::ALL)
,_left_joint(Combo::None)
,_right_joint(Combo::None)
,_hp()
,_power()
,_need_cost()
,_add_cost()
,_turn_count()
,_active_skill_id()
,_passive_skill_id()
,_chara_comment()
,_add_timing()
,_limit_count()
,_exchange_ticket()
,_next_chara_id()
,_prev_chara_id()
,_insert_date()
,_insert_date_timestamp(0)
,_deck_used_flg(false)
,_new_flg(true)
,_evolution_flg(false)
{
}

CharaSprite* CharaSprite::create(json11::Json json, bool new_flg, CardSize card_size)
{
    CharaSprite *sprite = new CharaSprite();
    
    if (sprite && sprite->init(json, new_flg, card_size))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaSprite::init(json11::Json json, bool new_flg, CardSize card_size)
{
    if ( !Sprite::initWithFile("download/Chara/parts100/card_bk1_reverse.png"))
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json);
    
    //表示するカードサイズの取得
    _card_size = card_size;
    
    //newマーク表示フラグの取得
    _new_flg = new_flg;
    
    return true;
}

void CharaSprite::onEnter()
{
    Sprite::onEnter();
    
    //キャラ画像の表示
    showCharaSprite();
    
    //戦闘力の表示
    showPower();
    
    //配置コストの表示
    showNeedCost();
    
    //キャラタイプの表示
    showCharaType();
    
    //コンボマークの表示
    showCombo();
    
    //new画像の表示
    if (_new_flg) showNewSprite();
    
    //総大将ラベルアクション
    actionGeneralLabel();
    
    //色全てのオブジェクトに適用させる処理（ノード上は適用させないので先にこれを記述）
    Utility::_setCascadeColorEnabled(this, true);
    
    //ノードの作成
    createDisableCascadeNode();
    
    //透過を全てのオブジェクトに適用させる処理
    Utility::_setCascadeOpacityEnabled(this, true);
}

void CharaSprite::setupData(json11::Json json)
{
    //データの設定
    setCharaListId(json["chara_list_id"].string_value());
    setCharaId(json["chara_id"].string_value());
    setCharaNo(json["chara_no"].string_value());
    setSecondName(json["second_name"].string_value());
    setName(json["name"].string_value());
    setNameKana(json["name_kana"].string_value());
    setRare(json["rare"].string_value());
    setCharaType((CharaType)fromString<int>(json["chara_type"].string_value()));
    setLeftJoint((Combo)fromString<int>(json["left_joint"].string_value()));
    setRightJoint((Combo)fromString<int>(json["right_joint"].string_value()));
    setHp(json["hp"].string_value());
    setPower(json["power"].string_value());
    setNeedCost(json["need_cost"].string_value());
    setAddCost(json["add_cost"].string_value());
    setTurnCount(json["turn_count"].string_value());
    setLimitCount(json["limit_count"].string_value());
    setExchangeTicket(json["exchange_ticket"].string_value());
    setNextCharaId(json["next_chara_id"].string_value());
    setPrevCharaId(json["prev_chara_id"].string_value());
    
    setActiveSkillId(json["active_skill_id"].string_value());
    setPassiveSkillId(json["passive_skill_id"].string_value());
    setCharaComment(json["chara_comment"].string_value());
    setAddTiming(json["add_timing"].string_value());
    setInsertDate(json["insert_date"].string_value());
    
    
    //ソート用の数値を保存
    _chara_id_int = fromString<int>(_chara_id);
    _rare_int = fromString<int>(_rare);
    _power_int = fromString<int>(_power);
    _need_cost_int = fromString<int>(_need_cost);
    struct tm tm;
    strptime(_insert_date.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
    _insert_date_int = mktime(&tm);
    
    //タイムスタンプを計算
    setInsertDateTimestamp(_insert_date_int);
}

time_t CharaSprite::calcTimeStamp()
{
    //取得日時からタイムスタンプを計算
    struct tm tm;
    strptime(getInsertDate().c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
    return mktime(&tm);
}

void CharaSprite::showCharaSprite()
{
    //キャラ画像の表示
    string path = "download/Chara/thumb100/" + getCharaId() + ".png";
    if (_card_size == ALL_100) path = "download/Chara/all100/" + getCharaId() + ".png";
    else if (_card_size == ALL_400) path = "download/Chara/all400/" + getCharaId() + ".png";
    
    if (fromString<int>(getCharaId()) > 0)
    {
        setTexture(path);
    }
}

void CharaSprite::showPower()
{
    if (getCharaType() == CharaType::OFFENSE || getCharaType() == CharaType::DEFENSE)
    {
        //戦闘力の表示（攻撃か防御タイプのみ表示）
        auto power_label = HMSFontUtil::createWithTTFBySystemFontArgument(getPower(), FONT_NAME_SYSTEM, 30);
        power_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        power_label->setAnchorPoint(Point(0.5, 0));
        power_label->setPosition(Point(this->getContentSize().width / 2 , 0));
        this->addChild(power_label, Z_Power, T_Power);
    }
    else if (getCharaType() == CharaType::STRATEGIST)
    {
//        //継続ターン数
//        auto turn_count_label = HMSFontUtil::createWithTTFBySystemFontArgument(getTurnCount() + "ターン", FONT_NAME_SYSTEM, 24);
//        turn_count_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//        turn_count_label->setAnchorPoint(Point(0.5, 0));
//        turn_count_label->setPosition(Point(this->getContentSize().width / 2 , 0));
//        this->addChild(turn_count_label, Z_Turn_Count, T_Turn_Count);
    }
    else if (getCharaType() == CharaType::GENERAL)
    {
        //士気
        int font_size = 24;
        if (_card_size == ALL_400) font_size = 74;
        auto add_cost_label = HMSFontUtil::createWithTTFBySystemFontArgument("士気" + getAddCost(), FONT_NAME_SYSTEM, font_size);
        add_cost_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        add_cost_label->setAnchorPoint(Point(0.5, 0));
        add_cost_label->setPosition(Point(this->getContentSize().width / 2 , 0));
        this->addChild(add_cost_label, Z_Add_Cost, T_Add_Cost);
    }
}

void CharaSprite::showNeedCost()
{
    if (getCharaType() != CharaType::GENERAL)
    {
        //配置コスト背景画像
        auto need_cost_sp = Sprite::create("init/CharaMenu/card_cost_bk2.png");
        //    auto need_cost_sp = Sprite::create("download/Chara/parts100/card_cost_bk.png");
        need_cost_sp->setAnchorPoint(Point(1.0, 1.0));
        need_cost_sp->setPosition(Point(this->getContentSize()));
        this->addChild(need_cost_sp, Z_NeedCost_Sp, T_NeedCost_Sp);
        
        //配置コストの表示
        auto need_cost_label = HMSFontUtil::createWithTTFBySystemFontArgument(getNeedCost(), FONT_NAME_SYSTEM, 20);
        need_cost_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        need_cost_label->setAnchorPoint(Point(0.5, 0.5));
        need_cost_label->setPosition(Point(need_cost_sp->getContentSize().width / 2 + 4, need_cost_sp->getContentSize().height / 2 + 4));
        need_cost_sp->addChild(need_cost_label, Z_NeedCost_Label, T_NeedCost_Label);
    }
}

void CharaSprite::showCharaType()
{
    //キャラ属性の表示
    string path = "";
    switch (getCharaType()) {
        case CharaType::OFFENSE:    path = "init/CharaMenu/icon_offense3.png"; break;
        case CharaType::DEFENSE:    path = "init/CharaMenu/icon_defense3.png"; break;
        case CharaType::STRATEGIST: path = "init/CharaMenu/icon_strategist3.png"; break;
        case CharaType::GENERAL:    path = "init/CharaMenu/icon_general3.png"; break;
        default: break;
    }
    
    if (path != "")
    {
        auto chara_type_sp = Sprite::create(path);
        chara_type_sp->setAnchorPoint(Point(0, 0));
        chara_type_sp->setPosition(Point(0, 0));
        this->addChild(chara_type_sp, Z_Chara_Type, T_Chara_Type);
    }
}

void CharaSprite::showCombo()
{
    //コンボマークの表示（コンボの無い軍師、総大将は表示しない）
    if (getLeftJoint() > 0 && getRightJoint() > 0)
    {
        //左
        auto left_rect = Rect(20, 0, 20, 40);
        auto left_combo_sp = Sprite::create("download/Chara/parts100/card_combo" + toString<int>(getLeftJoint()) + "_c.png", left_rect);
        left_combo_sp->setAnchorPoint(Point(0, 0.5));
        left_combo_sp->setPosition(Point(0, this->getContentSize().height / 2));
        this->addChild(left_combo_sp, Z_Left_Joint, T_Left_Joint);
        
        //右
        auto right_rect = Rect(0, 0, 20, 40);
        auto right_combo_sp = Sprite::create("download/Chara/parts100/card_combo" + toString<int>(getRightJoint()) + "_c.png", right_rect);
        right_combo_sp->setAnchorPoint(Point(1.0, 0.5));
        right_combo_sp->setPosition(Point(this->getContentSize().width, this->getContentSize().height / 2));
        this->addChild(right_combo_sp, Z_Right_Joint, T_Right_Joint);
    }
}

void CharaSprite::showNewSprite()
{
    //現在時刻の取得
    time_t now_time;
    time(&now_time);
    
    //入手日時から１時間経過以内であればNEW表示を出す
    if (now_time - getInsertDateTimestamp() < CHARA_NEW_TIME)
    {
        //NEW表示
        auto new_sp = Sprite::create("init/CharaMenu/new2.png");
        new_sp->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height));
        this->addChild(new_sp, Z_NEW, T_NEW);
        
        //アクション
        auto fadeOut = FadeOut::create(0.8f);
        auto fadeIn  = FadeIn::create(0.4f);
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, fadeIn, DelayTime::create(1.0f), nullptr));
        new_sp->runAction(repeat);
    }
}

void CharaSprite::actionGeneralLabel()
{
    //総大将のラベルアクション
    auto label = (Label *)this->getChildByTag(T_Add_Cost);
    
    if (label && getCharaType() == CharaType::GENERAL)
    {
        //アクション
        auto fadeOut = FadeOut::create(0.5f);
        auto fadeIn  = FadeIn::create(0.5f);
        
        auto func = CallFunc::create([this, label](){
            if (label->getTag() == T_Add_Cost) {
                //士気から体力に変更
                label->setString(getHp());
                label->setTag(T_Hp);
            }
            else {
                //体力から士気に変更
                label->setString("士気" + getAddCost());
                label->setTag(T_Add_Cost);
            }
        });
        auto repeat  = RepeatForever::create(Sequence::create(fadeOut, func, fadeIn, DelayTime::create(2.5f), nullptr));
        label->runAction(repeat);
    }
}

void CharaSprite::createDisableCascadeNode()
{
    //_setCascadeの後に作っているのでこれの上におけば適用されなくなる
    auto node = Node::create();
    node->setPosition(Point(getContentSize() / 2));
    this->addChild(node, Z_Node, T_Node);
}

void CharaSprite::setFavoriteSprite()
{
    //お気に入り画像を設定する
    auto sprite = this->getChildByTag(CharaSprite::T_Favorite);
    if (!sprite)
    {
        auto favorite_sp = Sprite::create("init/CharaMenu/revision/card_favorite.png");
        favorite_sp->setAnchorPoint(Point(1.0, 0));
        favorite_sp->setPosition(Point(this->getContentSize().width, 0));
        this->addChild(favorite_sp, Z_Favorite, T_Favorite);
    }
}

void CharaSprite::setExchangeTicketSprite()
{
    //交換チケット画像の設定
    auto exchange_ticket_sp = Sprite::create("init/CharaMenu/revision/chara_exchange/icon_ticket.png");
    exchange_ticket_sp->setAnchorPoint(Point(1.0, 0));
    exchange_ticket_sp->setPosition(Point(this->getContentSize().width + 20, -4));
    this->addChild(exchange_ticket_sp, Z_Exchange_Ticket, T_Exchange_Ticket);
}

string _sort_param;

bool compareChara(CharaSprite *chara1, CharaSprite *chara2)
{
    if(_sort_param=="rare") {
        if(chara1->_rare_int < chara2->_rare_int) return false;
        if(chara1->_rare_int > chara2->_rare_int) return true;
    } else if(_sort_param=="power") {
        if(chara1->_power_int < chara2->_power_int) return false;
        if(chara1->_power_int > chara2->_power_int) return true;
    } else if(_sort_param=="cost") {
        if(chara1->_need_cost_int < chara2->_need_cost_int) return false;
        if(chara1->_need_cost_int > chara2->_need_cost_int) return true;
    } else if(_sort_param=="insert_date") {
        if(chara1->_insert_date_int < chara2->_insert_date_int) return false;
        if(chara1->_insert_date_int> chara2->_insert_date_int) return true;
    }
    // 同じ場合は元の並び順で決める
    return chara1->_chara_id_int < chara2->_chara_id_int;
}

HmsVector<CharaSprite *> CharaSprite::sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param)
{
    _sort_param = sort_param;
    
    vector<CharaSprite *> vec;
    for(int i=0; i<chara_list.size(); i++) {
        vec.push_back(chara_list.at(i));
    }
    sort(vec.begin(), vec.end(), &compareChara);
    HmsVector<CharaSprite *> chara_list2;
    for(int i=0; i<vec.size(); i++) {
        chara_list2.push_back(vec.at(i));
    }
    return chara_list2;
}

