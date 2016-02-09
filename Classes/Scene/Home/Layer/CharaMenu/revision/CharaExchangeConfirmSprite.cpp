#include "CharaExchangeConfirmSprite.h"

CharaExchangeConfirmSprite::CharaExchangeConfirmSprite()
:_skill_status(SkillStatus::None)
,_close_func(nullptr)
{
}

CharaExchangeConfirmSprite* CharaExchangeConfirmSprite::create(Json json)
{
    CharaExchangeConfirmSprite *pRet = new CharaExchangeConfirmSprite();
    
    if (pRet && pRet->init(json))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool CharaExchangeConfirmSprite::init(Json json)
{
    if ( !Sprite::initWithFile("init/CharaMenu/revision/chara_exchange/exchange_chara_detail.png") )
    {
        return false;
    }
    HMSLOG("CharaExchangeConfirmSprite::init()");
    
    //キャラデータの設定
    setCharaData(json);
    
    ratio = getVisibleRatio() ;
    
    return true;
}

void CharaExchangeConfirmSprite::setCharaData(json11::Json json)
{
    //キャラデータの設定
    setCharaId(json["chara_id"].string_value());
    setCharaNo(json["chara_no"].string_value());
    setSecondName(json["second_name"].string_value());
    setName(json["name"].string_value());
    setNameKana(json["name_kana"].string_value());
    setRare(json["rare"].string_value());
    setCharaType((CharaSprite::CharaType)fromString<int>(json["chara_type"].string_value()));
    setLeftJoint((CharaSprite::Combo)fromString<int>(json["left_joint"].string_value()));
    setRightJoint((CharaSprite::Combo)fromString<int>(json["right_joint"].string_value()));
    setHp(json["hp"].string_value());
    setPower(json["power"].string_value());
    setNeedCost(json["need_cost"].string_value());
    setAddCost(json["add_cost"].string_value());
    setTurnCount(json["turn_count"].string_value());
    setLimitCount(json["limit_count"].string_value());
    
    setActiveSkillId(json["active_skill_id"].string_value());
    setPassiveSkillId(json["passive_skill_id"].string_value());
    setCharaComment(json["chara_comment"].string_value());
    setAddTiming(json["add_timing"].string_value());
    
    //アクティブスキルデータの設定
    if (fromString<int>(_active_skill_id) > 0)
    {
        setActiveSkillName(json["active_skill_data"]["name"].string_value());
        setActiveSkillDetail(json["active_skill_data"]["detail"].string_value());
        setActiveSkillTurnCount(json["active_skill_data"]["turn_count"].string_value());
        setActiveSkillNeedCost(json["active_skill_data"]["need_cost"].string_value());
    }
    
    //パッシブスキルデータの設定
    if (fromString<int>(_passive_skill_id) > 0)
    {
        setPassiveSkillName(json["passive_skill_data"]["name"].string_value());
        setPassiveSkillDetail(json["passive_skill_data"]["detail"].string_value());
        setPassiveSkillTurnCount(json["passive_skill_data"]["turn_count"].string_value());
        setPassiveSkillNeedCost(json["passive_skill_data"]["need_cost"].string_value());
    }
}

void CharaExchangeConfirmSprite::onEnter()
{
    Sprite::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    //キャラ画像の表示
    createCharaSprite();
    
    //名前の作成
    createNameLabel();
    
    //戦闘力などの表示
    createCharaStatus();
    
    //スキルの表示
    showSkill();
    
    //表示アクション
    Utility::openPopupAction(this);
    
}

void CharaExchangeConfirmSprite::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaExchangeConfirmSprite::createCharaSprite()
{
    //キャラ画像の表示
    string path = StringUtils::format("download/Chara/all400/%s.png", _chara_id.c_str());
    auto card = Sprite::create(path);
    card->setScale(0.54f);
    card->setPosition(Point(this->getContentSize().width / 2, 370));
    this->addChild(card, Z_Chara_Sprite, T_Chara_Sprite);
}

void CharaExchangeConfirmSprite::createNameLabel()
{
    //名前画像
    string path2 = StringUtils::format("download/Chara/name/%s.png", _chara_id.c_str());
    
    auto name = Sprite::create(path2);
    name->setAnchorPoint(Point(0.5, 0));
    name->setPosition(Point(this->getContentSize().width / 2, 210));
    name->setScale(0.9f);
    this->addChild(name, Z_Name, T_Name);
    
    //二つ名
    string second_name = _second_name;
    if(second_name != "")
    {
        //キャラの二つ名がある場合
        name->setPosition(Point(name->getPosition().x - 70, name->getPosition().y));
        
        float second_x = name->getPosition().x + (name->getContentSize().width / 2 + 8);
        
        auto second_label = HMSFontUtil::createWithTTFBySystemFontArgument(second_name, FONT_NAME_SYSTEM, 36);
        second_label->enableOutline(Color4B(0, 0, 0, 255), 3);
        second_label->setPosition(Point(second_x, name->getPosition().y));
        second_label->setAnchorPoint(Vec2(0, 0));
        this->addChild(second_label, T_Second_Name, Z_Second_Name);
    }
    
    //名前のフリガナ（ノーマル以上のみ表示）
    if (1 < fromString<int>(_rare) && _name_kana != "")
    {
        //通常文字画像、二つ名を小さくする
        name->setScale(0.82f);
        auto second_label = (Label *)this->getChildByTag(T_Second_Name);
        if (second_label) second_label->setScale(0.82f);
        
        //フリガナ
        auto kana_label = HMSFontUtil::createWithTTFBySystemFontArgument(_name_kana.c_str(), FONT_NAME_SYSTEM, 16);
        kana_label->setPosition(Point(this->getContentSize().width / 2, 250));
        this->addChild(kana_label, Z_Name_Kana, T_Name_Kana);
    }
}

void CharaExchangeConfirmSprite::createCharaStatus()
{
    //タイプアイコン
    string i_type[]={"offense", "defense", "strategist", "general"};
    string path = StringUtils::format("init/CharaMenu/icon_%s2.png", i_type[_chara_type - 1].c_str());
    auto icon_type = Sprite::create(path);
    icon_type->setPosition(Point(55, 172));
    this->addChild(icon_type, Z_Type_Icon, T_Type_Icon);
    
    //戦闘力
    string lapower_array[]={"戦闘力:", "戦闘力:", "継続ターン:", "体力:"};
    string lacost_array[]={"配置士気:", "配置士気:", "配置士気:", "上昇士気:"};
    
    float power_label_right_x = 168 ;
    if(_chara_type == CharaSprite::CharaType::STRATEGIST) power_label_right_x=220;
    else if(_chara_type == CharaSprite::CharaType::GENERAL) power_label_right_x=142;
    
    //戦闘力ラベル
    auto laPowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lapower_array[_chara_type - 1].c_str(), FONT_NAME_SYSTEM, 24);
    laPowerLabel->setAnchorPoint(Point(1, 0.5));
    laPowerLabel->enableOutline(Color4B::BLACK,1);
    laPowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laPowerLabel->setPosition(Vec2(power_label_right_x, 172));
    this->addChild(laPowerLabel);
    
    //戦闘力
    string text1 = _power;
    if (_chara_type == CharaSprite::CharaType::STRATEGIST) text1 = _turn_count;
    else if (_chara_type == CharaSprite::CharaType::GENERAL) text1 = _hp;
    
    auto powerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(text1.c_str(), FONT_NAME_SYSTEM, 35);
    powerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    powerLabel->setAnchorPoint(Point(0, 0.5));
    powerLabel->enableOutline(Color4B::BLACK,1);
    powerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    powerLabel->setPosition(Vec2(power_label_right_x + 2, 172));
    this->addChild(powerLabel, Z_Power, T_Power);
    
    //配置士気
    auto laCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lacost_array[_chara_type - 1], FONT_NAME_SYSTEM, 24);
    laCostLabel->setAnchorPoint(Point(1, 0.5));
    laCostLabel->enableOutline(Color4B::BLACK,1);
    laCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laCostLabel->setPosition(Vec2(420, 172));
    this->addChild(laCostLabel);
    
    //配置士気必要コスト
    string text2 = _need_cost;
    if (_chara_type == CharaSprite::CharaType::GENERAL) text2 = _add_cost;
    
    auto costLabel = HMSFontUtil::createWithTTFBySystemFontArgument(text2, FONT_NAME_SYSTEM, 35);
    costLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    costLabel->setAnchorPoint(Point(0, 0.5));
    costLabel->enableOutline(Color4B::BLACK,1);
    costLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    costLabel->setPosition(Vec2(426, 172));
    this->addChild(costLabel, Z_Cost, T_Cost);
}

void CharaExchangeConfirmSprite::showSkill()
{
    //スキルの表示
    auto skill_sp_off = Sprite::create("init/CharaMenu/revision/chara_exchange/tab_p.png");
    auto skill_sp_on  = Sprite::create("init/CharaMenu/revision/chara_exchange/tab_p.png");
    
    auto menuItem = MenuItemSprite::create(skill_sp_off, skill_sp_on, CC_CALLBACK_1(CharaExchangeConfirmSprite::changeSkillDetail, this));
    menuItem->setTag(T_Skill_MenuItem);
    
    auto menu = Menu::create(menuItem, nullptr);
    menu->setPosition(Point(this->getContentSize().width / 2, 70));
    this->addChild(menu, Z_Skill_Menu, T_Skill_Menu);
    
    
    //初期化
    string skill_name   = "";
    string skill_detail = "";
    string skill_need_cost = "";
    string text = "";

    if (fromString<int>(_passive_skill_id) > 0) {
        
        _skill_status = SkillStatus::Passive_Skill;
        
        //パッシブ持ちの場合、スキル名と詳細
        skill_name   = _passive_skill_name;
        skill_detail = _passive_skill_detail;
    }
    else if (fromString<int>(_active_skill_id) > 0) {
        
        _skill_status = SkillStatus::Active_Skill;
        
        //アクティブ持ちの場合、スキル名と詳細
        skill_name   = _active_skill_name;
        skill_detail = _active_skill_detail;
        
        //アクティブのみ発動士気という文字を表示
        text = "発動士気";
        skill_need_cost = _active_skill_need_cost;
        
        //画像の切り替え
        menuItem->setNormalImage(Sprite::create("init/CharaMenu/revision/chara_exchange/tab_a2.png"));
        menuItem->setSelectedImage(Sprite::create("init/CharaMenu/revision/chara_exchange/tab_a2.png"));
    }
    
    if (fromString<int>(_passive_skill_id) > 0 && fromString<int>(_active_skill_id) > 0) {
        
        //両方持ちの場合、切り替えタグ画像の生成
        auto skill_tab = Sprite::create("init/CharaMenu/revision/chara_exchange/tab_s_a.png");
        skill_tab->setPosition(Point(384, 104));
        this->addChild(skill_tab, Z_Skill_Tab, T_Skill_Tab);
        
        //切り替えボタンの作成
        auto switch_btn = MenuItemImage::create("init/CharaMenu/chara_detail_p_a.png", "init/CharaMenu/chara_detail_p_a.png", CC_CALLBACK_1(CharaExchangeConfirmSprite::changeSkillDetail, this));
        switch_btn->setScale(0.8f);
        auto menu = Menu::create(switch_btn, nullptr);
        menu->setPosition(Point(444, 300));
        this->addChild(menu, Z_Skill_Switch_Button, T_Skill_Switch_Button);
    }
    
    //スキル名
    _skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(skill_name, FONT_NAME_SYSTEM, 22);
    _skill_name->setAnchorPoint(Point(0, 0.5));
    _skill_name->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_name->enableOutline(Color4B(0, 0, 0, 255), 1);
    _skill_name->setColor(Color3B(255, 153, 0));
    _skill_name->setPosition(Point(36, 80));
    this->addChild(_skill_name, Z_Skill_Name, T_Skill_Name);
    
    //スキル詳細
    _skill_detail = HMSFontUtil::createWithTTFBySystemFontArgument(skill_detail, FONT_NAME_SYSTEM, 22);
    _skill_detail->setHorizontalAlignment(TextHAlignment::LEFT);
    _skill_detail->setVerticalAlignment(TextVAlignment::TOP);
    _skill_detail->setDimensions(470, 50);
    _skill_detail->enableOutline(Color4B::BLACK,1);
    _skill_detail->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_detail->setPosition(Point(this->getContentSize().width / 2, 38));
    this->addChild(_skill_detail, Z_Skill_Detail, T_Skill_Detail);
    
    //発動士気文字
    _skill_text = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 22);
    _skill_text->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_text->enableOutline(Color4B(0, 0, 0, 255), 1);
    _skill_text->setColor(Color3B(255, 153, 0));
    _skill_text->setPosition(Point(400, 80));
    this->addChild(_skill_text, Z_Skill_Text, T_Skill_Text);

    //発動士気必要コスト
    _skill_need_cost = HMSFontUtil::createWithTTFBySystemFontArgument(skill_need_cost, FONT_NAME_SYSTEM, 24);
    _skill_need_cost->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_need_cost->enableOutline(Color4B(0, 0, 0, 255), 1);
    _skill_need_cost->setColor(Color3B(255, 153, 0));
    _skill_need_cost->setPosition(Point(470, 80));
    this->addChild(_skill_need_cost, Z_Skill_Need_Cost, T_Skill_Need_Cost);
}

void CharaExchangeConfirmSprite::changeSkillDetail(Ref* sender)
{
    //両方持ちだったら切り替える処理
    if (fromString<int>(_active_skill_id) > 0 && fromString<int>(_passive_skill_id) > 0)
    {
        auto menuItem = (MenuItemSprite *)this->getChildByTag(T_Skill_Menu)->getChildByTag(T_Skill_MenuItem);
        
        switch (_skill_status) {
            case SkillStatus::Active_Skill:
            {
                //アクティブ→パッシブ
                _skill_status = SkillStatus::Passive_Skill;
                
                //画像の切り替え
                menuItem->setNormalImage(Sprite::create("init/CharaMenu/revision/chara_exchange/tab_p.png"));
                menuItem->setSelectedImage(Sprite::create("init/CharaMenu/revision/chara_exchange/tab_p.png"));
                
                //スキル詳細の切り替え
                _skill_name->setString(_passive_skill_name);
                _skill_detail->setString(_passive_skill_detail);
                _skill_need_cost->setString("");
                _skill_text->setString("");
                
                //タブの切り替え
                auto skill_tab = (Sprite *)this->getChildByTag(T_Skill_Tab);
                if (skill_tab) {
                    skill_tab->setTexture("init/CharaMenu/revision/chara_exchange/tab_s_a.png");
                    skill_tab->setPosition(Point(384, 104));
                }
            }break;
            case SkillStatus::Passive_Skill:
            {
                //パッシブ→アクティブ
                _skill_status = SkillStatus::Active_Skill;
                
                //画像の切り替え
                menuItem->setNormalImage(Sprite::create("init/CharaMenu/revision/chara_exchange/tab_a.png"));
                menuItem->setSelectedImage(Sprite::create("init/CharaMenu/revision/chara_exchange/tab_a.png"));
                
                //スキル詳細の切り替え
                _skill_name->setString(_active_skill_name);
                _skill_detail->setString(_active_skill_detail);
                _skill_need_cost->setString(_active_skill_need_cost);
                _skill_text->setString("発動士気");
                
                //タブの切り替え
                auto skill_tab = (Sprite *)this->getChildByTag(T_Skill_Tab);
                if (skill_tab) {
                    skill_tab->setTexture("init/CharaMenu/revision/chara_exchange/tab_s_p.png");
                    skill_tab->setPosition(Point(148, 104));
                }
                
            }break;
            default: break;
        }
    }
}
