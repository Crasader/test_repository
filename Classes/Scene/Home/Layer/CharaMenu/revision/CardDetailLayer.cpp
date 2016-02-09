#include "CardDetailLayer.h"

CardDetailLayer::CardDetailLayer()
:_skill_status(SkillStatus::None)
,_close_func(nullptr)
{
}

CardDetailLayer* CardDetailLayer::create(string chara_id, string chara_list_id)
{
    CardDetailLayer *pRet = new CardDetailLayer();
    
    if (pRet && pRet->init(chara_id, chara_list_id))
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

bool CardDetailLayer::init(string chara_id, string chara_list_id)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CardDetailLayer::init()");
    
    //初期データの設定
    _chara_id = chara_id ;
    _chara_list_id = chara_list_id;
    
    ratio = getVisibleRatio() ;
    
    return true;
}

void CardDetailLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    //キャラ情報取得通信処理
    getCharaData();
}

void CardDetailLayer::getCharaData(){
    string charaDataStr = SaveData::loadString("chara_data_list");
    string err;
    Json charaData = Json::parse(charaDataStr, err);
    
    if (!charaData.is_object()){
        //キャラ情報取得通信処理
        getCharaDataRequest();
        return;
    }
    Json result = nullptr;
    int jsonCount = countJsonData(charaData);
    for (int i = 0; i < jsonCount; i++) {
        string iStr = to_string(i);
        if(charaData[iStr]["chara_id"] == _chara_id){
            result = charaData[iStr].object_items();
            break;
        }
    }
    
    if(!result.is_object()){
        //キャラ情報取得通信処理
        getCharaDataRequest();
        return;
    }
    
    
    //キャラデータの設定
    HMSLOG(result.dump().c_str());
    setCharaData(result);
    createLayer();
    
}

void CardDetailLayer::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardDetailLayer::getCharaDataRequest()
{
    //売却通信処理
    string url = HOST;
    url += "user/get_chara_data.php";
    auto selector = httpresponse_selector(CardDetailLayer::getCharaDataCallback);
    
    string body = "chara_id=" + _chara_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void CardDetailLayer::getCharaDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (!json.is_object()){
        //キャラデータ取得失敗
        auto func = [](){};
        string message = "キャラデータの取得に失敗しました。\n通信環境を確認の上、時間を置き\n再度お試し下さい。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        this->addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
        
        return;
    }
    
    //キャラデータの設定
    setCharaData(json);
    createLayer();
}

void CardDetailLayer::setCharaData(json11::Json json)
{
    //キャラデータの設定
    if(json["chara_data"].is_null()) _chara_data = json.object_items();
    else _chara_data = json["chara_data"].object_items();
    _active_skill_data  = json["active_skill_data"].object_items();
    _passive_skill_data = json["passive_skill_data"].object_items();
    
    setCharaNo(_chara_data["chara_no"].string_value());
    setSecondName(_chara_data["second_name"].string_value());
    setName(_chara_data["name"].string_value());
    setNameKana(_chara_data["name_kana"].string_value());
    setRare(_chara_data["rare"].string_value());
    setCharaType((CharaSprite::CharaType)fromString<int>(_chara_data["chara_type"].string_value()));
    setLeftJoint((CharaSprite::Combo)fromString<int>(_chara_data["left_joint"].string_value()));
    setRightJoint((CharaSprite::Combo)fromString<int>(_chara_data["right_joint"].string_value()));
    setHp(_chara_data["hp"].string_value());
    setPower(_chara_data["power"].string_value());
    setNeedCost(_chara_data["need_cost"].string_value());
    setAddCost(_chara_data["add_cost"].string_value());
    setTurnCount(_chara_data["turn_count"].string_value());
    setLimitCount(_chara_data["limit_count"].string_value());
    
    setActiveSkillId(_chara_data["active_skill_id"].string_value());
    setPassiveSkillId(_chara_data["passive_skill_id"].string_value());
    setCharaComment(_chara_data["chara_comment"].string_value());
    setAddTiming(_chara_data["add_timing"].string_value());
    
    //アクティブスキルデータの設定
    if (fromString<int>(_active_skill_id) > 0)
    {
        setActiveSkillName(_active_skill_data["name"].string_value());
        setActiveSkillDetail(_active_skill_data["detail"].string_value());
        setActiveSkillTurnCount(_active_skill_data["turn_count"].string_value());
        setActiveSkillNeedCost(_active_skill_data["need_cost"].string_value());
    }
    
    //パッシブスキルデータの設定
    if (fromString<int>(_passive_skill_id) > 0)
    {
        setPassiveSkillName(_passive_skill_data["name"].string_value());
        setPassiveSkillDetail(_passive_skill_data["detail"].string_value());
        setPassiveSkillTurnCount(_passive_skill_data["turn_count"].string_value());
        setPassiveSkillNeedCost(_passive_skill_data["need_cost"].string_value());
    }
}

void CardDetailLayer::createLayer(){
    
    //背景の作成
    createBg();
    
    //キャラ画像の表示
    createCharaSprite();
    
    //お気に入りボタンの作成
    createFavoriteButton();
    
    //名前の作成
    createNameLabel();
    
    //戦闘力などの表示
    createCharaStatus();
    
    //スキルの表示
    showSkill();
    
    //キャラの説明
    showCharaComment();
    
    //表示アクション
    Utility::openPopupAction(this);
    
}

void CardDetailLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5)) ;
    this->addChild(shadow, Z_Shadow, T_Shadow);
    
    //背景画像
    _bg = Sprite::create("init/CharaMenu/chara_detail_base.png");
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    //閉じるボタン
    auto close_btn = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        
        if (_close_func != nullptr) _close_func();
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    auto menu = Menu::create(close_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width - 20, _bg->getContentSize().height - 90));
    _bg->addChild(menu, Z_Close_Menu, T_Close_Menu);
}

void CardDetailLayer::createCharaSprite()
{
    //キャラ画像の表示
    string path = StringUtils::format("download/Chara/all400/%s.png", _chara_id.c_str());
    auto card = Sprite::create(path);
    card->setScale(0.6f);
    card->setPosition(Point(300, 340+75));
    card->setAnchorPoint(Point(0.5, 0));
    _bg->addChild(card, Z_Chara_Sprite, T_Chara_Sprite);
}

void CardDetailLayer::createFavoriteButton()
{
    //ロック（お気に入り）ボタンの作成
    if(_chara_list_id != "")
    {
        auto lock_btn = MenuItemImage::create("init/CharaMenu/bt_favorite.png", "init/CharaMenu/bt_favorite_gray.png", [this](Ref* sender){
            
            auto menuItem = (MenuItemImage *)sender;
            
            //お気に入り変更処理
            if (!Utility::checkFavorite(_chara_list_id)){
                
                //お気に入り登録
                Utility::setFavorite(_chara_list_id);
                menuItem->unselected();
            }
            else {
    
                //お気に入り解除
                Utility::removeFavorite(_chara_list_id);
                menuItem->selected();
            }
        });
        
        lock_btn->setTag(T_Lock_MenuItem);
        lock_btn->setAnchorPoint(Point(0, 1)) ;
        
        auto menu = Menu::create(lock_btn, nullptr);
        menu->setPosition(Point(0, _bg->getContentSize().height-75));
        _bg->addChild(menu, Z_Lock_Menu, T_Lock_Menu);
        
        //お気に入りボタンの状態チェックし登録していない場合off
        if (!Utility::checkFavorite(_chara_list_id)) lock_btn->selected();
    }
    
}

void CardDetailLayer::createNameLabel()
{
    //名前画像
    string path2 = StringUtils::format("download/Chara/name/%s.png", _chara_id.c_str());
    
    auto name = Sprite::create(path2);
    name->setAnchorPoint(Point(0.5, 0));
    name->setPosition(Point(_bg->getContentSize().width / 2, 280 + 75));
    _bg->addChild(name, Z_Name, T_Name);
    
    //二つ名
    string second_name = _second_name;
    if(second_name != "")
    {
        //キャラの二つ名がある場合
        name->setPosition(Point(name->getPosition().x - 70, name->getPosition().y));
        
        float second_x = name->getPosition().x + (name->getContentSize().width / 2 + 8);
        
        auto second_label = HMSFontUtil::createWithTTFBySystemFontArgument(second_name, FONT_NAME_SYSTEM, 40);
        second_label->enableOutline(Color4B(0, 0, 0, 255), 3);
        second_label->setPosition(Point(second_x, name->getPosition().y));
        second_label->setAnchorPoint(Vec2(0, 0));
        _bg->addChild(second_label, T_Second_Name, Z_Second_Name);
    }
    
    //名前のフリガナ（ノーマル以上のみ表示）
    if (1 < fromString<int>(_rare) && _name_kana != "")
    {
        //通常文字画像、二つ名を小さくする
        name->setScale(0.82f);
        auto second_label = (Label *)_bg->getChildByTag(T_Second_Name);
        if (second_label) second_label->setScale(0.82f);
        
        //フリガナ
        auto kana_label = HMSFontUtil::createWithTTFBySystemFontArgument(_name_kana.c_str(), FONT_NAME_SYSTEM, 18);
        kana_label->setPosition(Point(_bg->getContentSize().width / 2, (280 + 120)));
        _bg->addChild(kana_label, 5);
    }
}

void CardDetailLayer::createCharaStatus()
{
    //タイプアイコン
    string i_type[]={"offense", "defense", "strategist", "general"};
    string path = StringUtils::format("init/CharaMenu/icon_%s2.png", i_type[_chara_type - 1].c_str());
    auto icon_type = Sprite::create(path);
    icon_type->setPosition(Point(42, 217+75));
    icon_type->setAnchorPoint(Vec2(0, 0));
    _bg->addChild(icon_type, Z_Type_Icon, T_Type_Icon);
    
    //戦闘力
    string lapower_array[]={"戦闘力:", "戦闘力:", "継続ターン:", "体力:"};
    string lacost_array[]={"配置士気:", "配置士気:", "配置士気:", "上昇士気:"};
    
    float power_label_right_x = 187 ;
    if(_chara_type == CharaSprite::CharaType::STRATEGIST) power_label_right_x=247;
    else if(_chara_type == CharaSprite::CharaType::GENERAL) power_label_right_x=167;
    
    //戦闘力ラベル
    auto laPowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lapower_array[_chara_type - 1].c_str(), FONT_NAME_SYSTEM, 26);
    laPowerLabel->setAnchorPoint(Point(1, 0.5));
    laPowerLabel->enableOutline(Color4B::BLACK,1);
    laPowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laPowerLabel->setPosition(Vec2(power_label_right_x, 242 + 75));
    _bg->addChild(laPowerLabel);
    
    //戦闘力
    string text1 = _power;
    if (_chara_type == CharaSprite::CharaType::STRATEGIST) text1 = _turn_count;
    else if (_chara_type == CharaSprite::CharaType::GENERAL) text1 = _hp;
    
    auto powerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(text1.c_str(), FONT_NAME_SYSTEM, 40);
    powerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    powerLabel->setAnchorPoint(Point(0, 0.5));
    powerLabel->enableOutline(Color4B::BLACK,1);
    powerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    powerLabel->setPosition(Vec2(power_label_right_x + 5, 242+75));
    _bg->addChild(powerLabel, Z_Power, T_Power);
    
    //コストラベル
    auto laCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lacost_array[_chara_type - 1], FONT_NAME_SYSTEM, 26);
    laCostLabel->setScale(1.0);
    laCostLabel->setAnchorPoint(Point(1, 0.5));
    laCostLabel->enableOutline(Color4B::BLACK,1);
    laCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laCostLabel->setPosition(Vec2(471, 242+75));
    _bg->addChild(laCostLabel);
    
    //コスト
    string text2 = _need_cost;
    if (_chara_type == CharaSprite::CharaType::GENERAL) text2 = _add_cost;
    
    auto costLabel = HMSFontUtil::createWithTTFBySystemFontArgument(text2, FONT_NAME_SYSTEM, 40);
    costLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    costLabel->setAnchorPoint(Point(0, 0.5));
    costLabel->enableOutline(Color4B::BLACK,1);
    costLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    costLabel->setPosition(Vec2(476, 242+75));
    _bg->addChild(costLabel, Z_Cost, T_Cost);
}

void CardDetailLayer::showSkill()
{
    //スキルの表示
    auto skill_sp_off = Sprite::create("init/CharaMenu/tab_p.png");
    auto skill_sp_on  = Sprite::create("init/CharaMenu/tab_p.png");
    
    auto menuItem = MenuItemSprite::create(skill_sp_off, skill_sp_on, CC_CALLBACK_1(CardDetailLayer::changeSkillDetail, this));
    menuItem->setTag(T_Skill_MenuItem);
    
    auto menu = Menu::create(menuItem, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 318-105));
    _bg->addChild(menu, Z_Skill_Menu, T_Skill_Menu);

    
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
        menuItem->setNormalImage(Sprite::create("init/CharaMenu/tab_a2.png"));
        menuItem->setSelectedImage(Sprite::create("init/CharaMenu/tab_a2.png"));
    }
    
    if (fromString<int>(_passive_skill_id) > 0 && fromString<int>(_active_skill_id) > 0) {
        
        //両方持ちの場合、切り替えタグ画像の生成
        auto skill_tab = Sprite::create("init/CharaMenu/tab_s_a.png");
        skill_tab->setPosition(Point(438, 250));
        _bg->addChild(skill_tab, Z_Skill_Tab, T_Skill_Tab);
        
        //切り替えボタンの作成
        auto switch_btn = MenuItemImage::create("init/CharaMenu/chara_detail_p_a.png", "init/CharaMenu/chara_detail_p_a.png", CC_CALLBACK_1(CardDetailLayer::changeSkillDetail, this));
        auto menu = Menu::create(switch_btn, nullptr);
        menu->setPosition(Point(502, 450));
        _bg->addChild(menu, Z_Skill_Switch_Button, T_Skill_Switch_Button);
    }
    
    //スキル名
    _skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(skill_name, FONT_NAME_SYSTEM, 24);
    _skill_name->setAnchorPoint(Point(0, 0.5));
    _skill_name->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_name->enableOutline(Color4B(0, 0, 0, 255), 1);
    _skill_name->setColor(Color3B(255, 153, 0));
    _skill_name->setPosition(Point(48, 225));
    _bg->addChild(_skill_name, Z_Skill_Name, T_Skill_Name);

    //スキル詳細
    _skill_detail = HMSFontUtil::createWithTTFBySystemFontArgument(skill_detail, FONT_NAME_SYSTEM, 24);
    _skill_detail->setHorizontalAlignment(TextHAlignment::LEFT);
    _skill_detail->setVerticalAlignment(TextVAlignment::TOP);
    _skill_detail->setDimensions(510, 60);
    _skill_detail->enableOutline(Color4B::BLACK,1);
    _skill_detail->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_detail->setPosition(Point(_bg->getContentSize().width / 2, 175));
    _bg->addChild(_skill_detail, Z_Skill_Detail, T_Skill_Detail);
    
    //発動士気文字
    _skill_text = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24);
    _skill_text->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_text->enableOutline(Color4B(0, 0, 0, 255), 1);
    _skill_text->setColor(Color3B(255, 153, 0));
    _skill_text->setPosition(Point(450, 225));
    _bg->addChild(_skill_text, Z_Skill_Text, T_Skill_Text);
    
    //発動士気必要コスト
    _skill_need_cost = HMSFontUtil::createWithTTFBySystemFontArgument(skill_need_cost, FONT_NAME_SYSTEM, 28);
    _skill_need_cost->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _skill_need_cost->enableOutline(Color4B(0, 0, 0, 255), 1);
    _skill_need_cost->setColor(Color3B(255, 153, 0));
    _skill_need_cost->setPosition(Point(530, 225));
    _bg->addChild(_skill_need_cost, Z_Skill_Need_Cost, T_Skill_Need_Cost);
}

void CardDetailLayer::showCharaComment()
{
    //キャラ説明の表示
    auto charaCommentLabel = HMSFontUtil::createWithTTFBySystemFontArgument(_chara_comment, FONT_NAME_SYSTEM, 24);
    charaCommentLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    charaCommentLabel->setVerticalAlignment(TextVAlignment::TOP);
    charaCommentLabel->setDimensions(510, 180);
    charaCommentLabel->setAnchorPoint(Point(0.5,1));
    charaCommentLabel->enableOutline(Color4B::BLACK, 1);
    charaCommentLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    charaCommentLabel->setPosition(Point(_bg->getContentSize().width / 2, 106));
    _bg->addChild(charaCommentLabel, Z_Comment, T_Comment);
}

void CardDetailLayer::changeSkillDetail(Ref* sender)
{
    //両方持ちだったら切り替える処理
    if (fromString<int>(_active_skill_id) > 0 && fromString<int>(_passive_skill_id) > 0)
    {
        auto menuItem = (MenuItemSprite *)_bg->getChildByTag(T_Skill_Menu)->getChildByTag(T_Skill_MenuItem);
        
        switch (_skill_status) {
            case SkillStatus::Active_Skill:
            {
                //アクティブ→パッシブ
                _skill_status = SkillStatus::Passive_Skill;
                
                //画像の切り替え
                menuItem->setNormalImage(Sprite::create("init/CharaMenu/tab_p.png"));
                menuItem->setSelectedImage(Sprite::create("init/CharaMenu/tab_p.png"));
                
                //スキル詳細の切り替え
                _skill_name->setString(_passive_skill_name);
                _skill_detail->setString(_passive_skill_detail);
                _skill_need_cost->setString("");
                _skill_text->setString("");
                
                //タブの切り替え
                auto skill_tab = (Sprite *)_bg->getChildByTag(T_Skill_Tab);
                if (skill_tab) {
                    skill_tab->setTexture("init/CharaMenu/tab_s_a.png");
                    skill_tab->setPosition(Point(438, 250));
                }
            }break;
            case SkillStatus::Passive_Skill:
            {
                //パッシブ→アクティブ
                _skill_status = SkillStatus::Active_Skill;
                
                //画像の切り替え
                menuItem->setNormalImage(Sprite::create("init/CharaMenu/tab_a.png"));
                menuItem->setSelectedImage(Sprite::create("init/CharaMenu/tab_a.png"));
                
                //スキル詳細の切り替え
                _skill_name->setString(_active_skill_name);
                _skill_detail->setString(_active_skill_detail);
                _skill_need_cost->setString(_active_skill_need_cost);
                _skill_text->setString("発動士気");
                
                //タブの切り替え
                auto skill_tab = (Sprite *)_bg->getChildByTag(T_Skill_Tab);
                if (skill_tab) {
                    skill_tab->setTexture("init/CharaMenu/tab_s_p.png");
                    skill_tab->setPosition(Point(163, 250));
                }
                
            }break;
            default: break;
        }
    }
}
