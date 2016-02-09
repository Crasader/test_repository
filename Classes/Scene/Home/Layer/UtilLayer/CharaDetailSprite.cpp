#include "CharaDetailSprite.h"

CharaDetailSprite* CharaDetailSprite::create(int chara_list_id, bool hide_lock_button){

    CharaDetailSprite *sprite = new CharaDetailSprite();

    if (sprite && sprite->init(chara_list_id, hide_lock_button))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaDetailSprite::init(int chara_list_id, bool hide_lock_button)
{
    if ( !Sprite::initWithFile("init/CharaMenu/chara_detail_base.png") )//画像サイズ600x350
    {
        return false;
    }
    _chara_list_id = chara_list_id;
    _clipping_masc_width = 520 ;
    _hide_lock_button = hide_lock_button ;
    
    /*
    listener = EventListenerTouchOneByOne::create();
    
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    
    //タップイベントの設定
    listener->onTouchBegan = CC_CALLBACK_2(CharaDetailSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaDetailSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaDetailSprite::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
     */
    
    return true;
}

void CharaDetailSprite::setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel)
{
    _parent = parent;
    _sel = sel;
}

void CharaDetailSprite::setLeftRightFlg(bool left_flg, bool right_flg, float delay_time)
{
    _left_flg = left_flg;
    _right_flg = right_flg;
    _delay_time = delay_time;
}

void CharaDetailSprite::setSkillInfo(json11::Json json_chara, string skill_name, string skill_detail, string skill_icon_detail, string need_cost)
{
    _json_chara = json_chara;
    this->skill_name = skill_name;
    this->skill_detail = skill_detail;
    this->skill_icon_detail = skill_icon_detail;
    this->active_skill_need_cost = need_cost;
    
    _skill_set_flg = true;
}

void CharaDetailSprite::setData()
{
    if(_skill_set_flg==false) {
        json_text_as = SaveData::loadString("user_active_skill_list");
        json_text_ps = SaveData::loadString("user_passive_skill_list");
        
        ////////キャラクター一覧情報の読み込み////////
        string json_text = SaveData::loadString("user_chara_list");
        string err2;
        string err3;
        string err4;
        _json_chara = json11::Json::parse(json_text, err2);
        _json_active_skill = json11::Json::parse(json_text_as, err3);
        _json_passive_skill = json11::Json::parse(json_text_ps, err4);
    }

    int max_chara_count = fromString<int>(SaveData::loadString("max_chara_count"));
    string c_l_id = toString(_chara_list_id);
    for (int i=0; i< max_chara_count; i++) {
        ind = toString(i);
        if(_json_chara[ind]["chara_list_id"].string_value() == c_l_id){
            break;
        }
    }
    
    chara_id = _json_chara[ind]["chara_id"].string_value();                 //該当キャラのID
    chara_no = _json_chara[ind]["chara_no"].string_value();                 //キャラNo
    second_name = _json_chara[ind]["second_name"].string_value();           //二つ名
    chara_name = _json_chara[ind]["name"].string_value();                   //キャラ名
    name_kana = _json_chara[ind]["name_kana"].string_value();               //キャラ名のフリガナ
    rare = _json_chara[ind]["rare"].string_value();                         //レア度
    chara_power = _json_chara[ind]["power"].string_value();                 //該当キャラの戦闘力
    _chara_type = _json_chara[ind]["chara_type"].string_value();            //キャラタイプ
    need_cost = _json_chara[ind]["need_cost"].string_value();               //配置コスト
    hp = _json_chara[ind]["hp"].string_value();                             //体力
    add_cost = _json_chara[ind]["add_cost"].string_value();                 //上昇士気
    turn_count = _json_chara[ind]["turn_count"].string_value();             //継続ターン
    chara_comment = _json_chara[ind]["chara_comment"].string_value();       //キャラコメント
    active_skill_id = _json_chara[ind]["active_skill_id"].string_value();   //アクティブスキルID
    passive_skill_id = _json_chara[ind]["passive_skill_id"].string_value(); //パッシブスキルID
    
    
    if(_skill_set_flg==true) return;
    
    HMSLOG("active_skill_id=%s",active_skill_id.c_str());
    HMSLOG("passive_skill_id=%s",passive_skill_id.c_str());
    if(active_skill_id!="0" && passive_skill_id!="0") {
        _switch_skill_button_flg = true;
    }
    for (int i=0; i< max_chara_count; i++) {
        ind = toString(i);
        if(_json_active_skill[ind]["active_skill_id"].string_value() == active_skill_id){
            break;
        }
    }
    active_skill_name = _json_active_skill[ind]["name"].string_value();              //アクティブスキル名
    active_skill_detail = _json_active_skill[ind]["detail"].string_value();          //アクティブスキル詳細
    active_skill_icon_detail = _json_active_skill[ind]["icon_detail"].string_value();//アクティブスキルアイコン
    active_skill_need_cost = _json_active_skill[ind]["need_cost"].string_value();    //アクティブスキル必要コスト
    HMSLOG("active_skill_name=%s",active_skill_name.c_str());
    HMSLOG("active_skill_detail=%s",active_skill_detail.c_str());
    if(active_skill_id != "0" && _skill_type=="active_skill")//アクティブスキルIDが0ではない場合
    {
        skill_name = active_skill_name;
        skill_detail = active_skill_detail;
        skill_icon_detail = active_skill_icon_detail;
    }else{
        for (int i=0; i< max_chara_count; i++) {
            ind = toString(i);
            if(_json_passive_skill[ind]["passive_skill_id"].string_value() == passive_skill_id){
                break;
            }
        }
        skill_name = _json_passive_skill[ind]["name"].string_value();               //パッシブスキル名
        skill_detail = _json_passive_skill[ind]["detail"].string_value();           //パッシブスキル詳細
        skill_icon_detail = _json_passive_skill[ind]["icon_detail"].string_value(); //パッシブスキルアイコン
    }
}

void CharaDetailSprite::setupImage()
{
//    auto scale = ScaleTo::create(0.2f, 1.0);
//    this->runAction(scale);    
    
    //閉じるボタン
    auto back = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", CC_CALLBACK_1(CharaDetailSprite::deleteAction, this));
    auto close_menu = Menu::create(back, nullptr);
    close_menu->setPosition(Point(getContentSize().width-20, getContentSize().height-75));
    this->addChild(close_menu, 25, 10);
    
    //カード画像
    HMSLOG("chara_id=%s",chara_id.c_str());
    auto ch_id = StringUtils::format("download/Chara/all400/%s.png", chara_id.c_str());
    auto card = Sprite::create(ch_id.c_str());
    card->setScale(0.6);//400x0.7=280
    card->setPosition(Vec2(300,340+75));
    card->setAnchorPoint(Vec2(0.5,0));
    this->addChild(card, 4);

    if(!_hide_lock_button){
        //lock画像
        string fileName = "";
        if (getLockData(_chara_list_id)) {
            _default_lock_flg = true ;
            _lock_flg = true ;
            fileName = "init/CharaMenu/bt_favorite.png";
        }else{
            _default_lock_flg = false ;
            _lock_flg = false ;
            fileName = "init/CharaMenu/bt_favorite_gray.png";
        }
        auto lock_s = Sprite::create(fileName);
        lock_s->setScale(1.0);
        lock_s->setAnchorPoint(Point(0,1)) ;
        lock_s->setTag(LOCK_BUTTUN_TAG);
        lock_s->setPosition(Vec2(0,getContentSize().height-75));
        this->addChild(lock_s, 5);
        auto lock = MenuItemImage::create(fileName, fileName, CC_CALLBACK_1(CharaDetailSprite::touchButtun, this));
        lock->setScale(1.0);
        lock->setTag(LOCK_BUTTUN_TAG);
        lock->setAnchorPoint(Point(0,1)) ;
        auto m_lock = Menu::create(lock, nullptr);
        m_lock->setPosition(Vec2(0,getContentSize().height-75));
        this->addChild(m_lock,4);
    }
    
    //スキル切り替えボタン
    if(_switch_skill_button_flg==true) {
        string fileName = "init/CharaMenu/chara_detail_p_a.png";
        
        auto switch_button = MenuItemImage::create(fileName, fileName, CC_CALLBACK_1(CharaDetailSprite::touchButtun, this));
        switch_button->setScale(1.0);
        switch_button->setTag(SWITCH_SKILL_BUTTON_TAG);
        switch_button->setAnchorPoint(Point(0.5, 0.5)) ;
        auto m_switch_button = Menu::create(switch_button, nullptr);
        m_switch_button->setPosition(Vec2(502,554-105));
        this->addChild(m_switch_button, 4);
    }
    
    //スキル枠画像
    string filename1 = "";
    string filename2 = "";
    int x;
    if(_switch_skill_button_flg==true) {
        if(_skill_type=="active_skill") {
            filename1 = "init/CharaMenu/tab_a.png";
            filename2 = "init/CharaMenu/tab_s_p.png";
            x = 183 - 20;
        } else {
            filename1 = "init/CharaMenu/tab_p.png";
            filename2 = "init/CharaMenu/tab_s_a.png";
            x = 458 - 20;
        }
    } else {
        if(active_skill_id != "0") {
            filename1 = "init/CharaMenu/tab_a2.png";
        } else {
            filename1 = "init/CharaMenu/tab_p.png";
        }
    }
    if(_switch_skill_button_flg==true) {
        auto menu_img = MenuItemImage::create(filename1, filename1, CC_CALLBACK_1(CharaDetailSprite::touchButtun, this));
        menu_img->setTag(SWITCH_SKILL_BUTTON_TAG);
        menu_img->setAnchorPoint(Point(0.5, 0.5)) ;
        auto menu = Menu::create(menu_img, nullptr);
        menu->setPosition(Point(this->getContentSize().width / 2, 318-105));
        this->addChild(menu, 4);
    } else {
        auto img = Sprite::create(filename1.c_str());
        img->setAnchorPoint(Point(0.5, 0.5));
        img->setPosition(Point(this->getContentSize().width / 2, 318-105));
        this->addChild(img, 4);
    }
    
    if(filename2!="") {
        auto img = Sprite::create(filename2.c_str());
        img->setAnchorPoint(Point(0.5, 0.5));
        img->setPosition(Point(x, 354-105));
        this->addChild(img, 3);
    }

    //画像配置(タイプ → レア → 二つ名 → 名前の順)
    //タイプ画像準備
    string i_type[]={"","offense","defense","strategist","general"};
    auto ic_type = StringUtils::format("init/CharaMenu/icon_%s2.png", i_type[fromString<int>(_chara_type)].c_str());
    auto icon_type = Sprite::create(ic_type.c_str());
    icon_type->setScale(1.0);
    icon_type->setPosition(Vec2(42,217+75));
    icon_type->setAnchorPoint(Vec2(0,0));
    this->addChild(icon_type, 5);

    /*
    //レアリティ画像準備
    string rar[]={"","n","r","sr","ur","le"};
    auto rarity = StringUtils::format("init/CharaMenu/icon_%s2.png", rar[fromString<int>(rare)].c_str());
    auto rare_icon = Sprite::create(rarity.c_str());
    rare_icon->setScale(1.0);
    rare_icon->setPosition(Vec2(40,280));
    rare_icon->setAnchorPoint(Vec2(0.0,0.0));
    this->addChild(rare_icon, 5);
     */
    
    //名前画像
    string name_img = StringUtils::format("download/Chara/name/%s.png", chara_id.c_str());
    auto name = Sprite::create(name_img.c_str());
    name->setAnchorPoint(Point(0.5, 0));
    name->setPosition(Point(this->getContentSize().width / 2, 280 + 75));
    this->addChild(name, 3);
    
    if(second_name != ""){
        //キャラの二つ名がある場合
        name->setPosition(Point(name->getPosition().x - 70, name->getPosition().y));
        
        float second_x = name->getPosition().x + (name->getContentSize().width / 2 + 8);
        
        auto second_label = HMSFontUtil::createWithTTFBySystemFontArgument(second_name.c_str(), FONT_NAME_SYSTEM, 40);
        second_label->enableOutline(Color4B(0, 0, 0, 255), 3);
        second_label->setPosition(Point(second_x, name->getPosition().y));
        second_label->setAnchorPoint(Vec2(0, 0));
        this->addChild(second_label, 4, CharaDetailSprite::Tag::T_Second_name);
    }
    
    //名前のフリガナ（ノーマル以上のみ表示）
    if (1 < fromString<int>(rare) && name_kana != "")
    {
        //通常文字画像、二つ名を小さくする
        name->setScale(0.82f);
        auto second_label = (Label *)this->getChildByTag(CharaDetailSprite::Tag::T_Second_name);
        if (second_label) {
            second_label->setScale(0.82f);
        }
        
        //フリガナ
        auto kana_label = HMSFontUtil::createWithTTFBySystemFontArgument(name_kana.c_str(), FONT_NAME_SYSTEM, 18);
        kana_label->setPosition(Point(this->getContentSize().width / 2, (280 + 120)));
        this->addChild(kana_label, 5);
    }
}

void CharaDetailSprite::setupLabel()
{
    string lapower_array[]={"","戦闘力:","戦闘力:","継続ターン:","体力:"};//戦闘力ラベル
    string lacost_array[]={"","配置士気:","配置士気:","配置士気:","上昇士気:"};//コストラベル
    string power_array[]={"",chara_power,chara_power,turn_count,hp};//戦闘力
    string cost_array[]={"",need_cost,need_cost,need_cost,add_cost};//コスト
    
    float power_label_right_x = 187 ;
    if(fromString<int>(_chara_type)==3)power_label_right_x=247;
    else if(fromString<int>(_chara_type)==4)power_label_right_x=167;
    
    
    //戦闘力ラベル
    auto laPowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lapower_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,26);
    laPowerLabel->setScale(1.0);
//    laPowerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    laPowerLabel->setAnchorPoint(Point(1,0.5));
    laPowerLabel->enableOutline(Color4B::BLACK,1);
    laPowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laPowerLabel->setPosition(Vec2(power_label_right_x, 242+75));
    this->addChild(laPowerLabel,4);
    //戦闘力
    auto powerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(power_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,40);
    powerLabel->setScale(1.0);
    powerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    powerLabel->setAnchorPoint(Point(0,0.5));
    powerLabel->enableOutline(Color4B::BLACK,1);
    powerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    powerLabel->setPosition(Vec2(power_label_right_x + 5, 242+75));
    this->addChild(powerLabel,4);
    //コストラベル
    auto laCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lacost_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,26);
    laCostLabel->setScale(1.0);
//    laCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    laCostLabel->setAnchorPoint(Point(1,0.5));
    laCostLabel->enableOutline(Color4B::BLACK,1);
    laCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laCostLabel->setPosition(Vec2(471, 242+75));
    this->addChild(laCostLabel,4);
    //コスト
    auto costLabel = HMSFontUtil::createWithTTFBySystemFontArgument(cost_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,40);
    costLabel->setScale(1.0);
    costLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    costLabel->setAnchorPoint(Point(0,0.5));
    costLabel->enableOutline(Color4B::BLACK,1);
    costLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    costLabel->setPosition(Vec2(476, 242+75));
    this->addChild(costLabel,4);
    
    
    //スキル名
    auto la_skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(skill_name, FONT_NAME_SYSTEM, 24);
    la_skill_name->setScale(1.0);
    la_skill_name->setColor(Color3B(255,153,0));
    la_skill_name->setHorizontalAlignment(TextHAlignment::LEFT);
    la_skill_name->setAnchorPoint(Point(0, 0.5));
    la_skill_name->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    la_skill_name->enableOutline(Color4B(0, 0, 0, 255), 1);
    la_skill_name->setPosition(Vec2(68-20, 330-105));
    this->addChild(la_skill_name,5);
    
    if(active_skill_id != "0" && _skill_type=="active_skill")//アクティブスキルIDが0ではない場合
    {
        //発動士気ラベル
        auto laSkillCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument("発動士気", FONT_NAME_SYSTEM, 24);
        laSkillCostLabel->setScale(1.0);
        laSkillCostLabel->setColor(Color3B(255,153,0));
        laSkillCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
        laSkillCostLabel->setAnchorPoint(Point(1, 0.5));
        laSkillCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        laSkillCostLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
        laSkillCostLabel->setPosition(Vec2(568-60,330-105));
        this->addChild(laSkillCostLabel,5);
        //発動士気
        auto SkillCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(active_skill_need_cost, FONT_NAME_SYSTEM, 28);
        SkillCostLabel->setScale(1.0);
        SkillCostLabel->setColor(Color3B(255,153,0));
        SkillCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
        SkillCostLabel->setAnchorPoint(Point(1, 0.5));
        SkillCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        SkillCostLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
        SkillCostLabel->setPosition(Vec2(568-20,330-105));
        this->addChild(SkillCostLabel,5);
    }
    //キャラコメント
    auto charaCommentLabel = HMSFontUtil::createWithTTFBySystemFontArgument(chara_comment,FONT_NAME_SYSTEM,24);
    charaCommentLabel->setScale(1.0);
    charaCommentLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    charaCommentLabel->setVerticalAlignment(TextVAlignment::TOP);
    charaCommentLabel->setDimensions(510, 180);
    charaCommentLabel->setAnchorPoint(Point(0.5,1));
    charaCommentLabel->enableOutline(Color4B::BLACK,1);
    charaCommentLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    charaCommentLabel->setPosition(Vec2(300,106));
    this->addChild(charaCommentLabel,4);
    
    //アクティブスキル詳細
	//Label* scroll_label = nullptr;
	//if(skill_detail != "") scroll_label = setupScrollLabel(scroll_label, skill_detail, Point(40, 130));
    if(skill_detail!="") {
        auto skillDetailLabel = HMSFontUtil::createWithTTFBySystemFontArgument(skill_detail,FONT_NAME_SYSTEM,24);
        skillDetailLabel->setScale(1.0);
        skillDetailLabel->setHorizontalAlignment(TextHAlignment::LEFT);
        skillDetailLabel->setVerticalAlignment(TextVAlignment::TOP);
        skillDetailLabel->setDimensions(510, 180);
        skillDetailLabel->setAnchorPoint(Point(0.5,1));
        skillDetailLabel->enableOutline(Color4B::BLACK,1);
        skillDetailLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        skillDetailLabel->setPosition(Vec2(300,310-105));
        this->addChild(skillDetailLabel,4);
    }
    
    if(_left_flg==true) {
        auto arrowleft = Sprite::create("init/CharaMenu/arrow_left.png");
        arrowleft->setScale(1.0);
        arrowleft->setPosition(Point(10,300+75));
        this->addChild(arrowleft,5);
    }
    
    if(_right_flg==true) {
        auto arrowright = Sprite::create("init/CharaMenu/arrow_right.png");
        arrowright->setScale(1.0);
        arrowright->setPosition(Point(610,300+75));
        this->addChild(arrowright,5);
    }
    
    /*
    //スキル詳細
    auto skill_text = HMSFontUtil::createWithTTFBySystemFontArgument(skill_detail, FONT_NAME_SYSTEM, 22);
    skill_text->setScale(1.0);
//    skill_text->setDimensions(540, 65);
    skill_text->setHorizontalAlignment(TextHAlignment::LEFT);
    skill_text->setVerticalAlignment(TextVAlignment::TOP);
    skill_text->setAnchorPoint(Point(0.5, 1));
    skill_text->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
     skill_text->enableOutline(Color4B(0, 0, 0, 255), 1);
    skill_text->setPosition(Vec2(300,155));
    this->addChild(skill_text,5);
     */
}

//ロックしているかどうか
bool CharaDetailSprite::getLockFlg(){
    return _lock_flg ;
}

//ロック情報がウィンドウを開いたときから変更されているか
bool CharaDetailSprite::isLockStateChanged() {
    return (_lock_flg!=_default_lock_flg);
}

Label* CharaDetailSprite::setupScrollLabel(Label* scroll_label, string text, Point pos){
	//スクロールする文章構築
	Rect rect = Rect(0, 0, _clipping_masc_width , 28);
	Sprite* mask = Sprite::create();
	mask->setTextureRect(rect);
	mask->setAnchorPoint(Point(0,0));
	
	scroll_label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24);
	scroll_label->setHorizontalAlignment(TextHAlignment::LEFT);
	scroll_label->setAnchorPoint(Point(0, 0));
	scroll_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    scroll_label->enableOutline(Color4B(0, 0, 0, 255), 1);
	
	auto clipper = ClippingNode::create();
	clipper->setStencil(mask);
	clipper->setAlphaThreshold(0.01f);
	clipper->addChild(scroll_label);
	clipper->setPosition(pos);
	this->addChild(clipper);
	
	//スクロールアクション
	setScrollLavelAction(scroll_label);
	return scroll_label;
}

void CharaDetailSprite::setScrollLavelAction(Label* text_label)
{
	//テキストの無限スクロール※一定アクションの繰り返し
	if(text_label->getContentSize().width > _clipping_masc_width) {
		text_label->setPosition(Point(0, 0));
		
		float move_speed_time = round(text_label->getContentSize().width / 120);
		DelayTime* delay1 = DelayTime::create(1.5f); //スタート位置からの待機時間
		DelayTime* delay2 = DelayTime::create(1.0f); //画面外(右側)からリスタートするまでの待機時間
		MoveTo* move1 = MoveTo::create(move_speed_time, Point(-text_label->getContentSize().width, 0)); //画面外(左側)に移動
		MoveTo* move2 = MoveTo::create(0.01, Point(_clipping_masc_width, 0)); //画面外(右側)に配置
		MoveTo* move3 = MoveTo::create(move_speed_time, Point(0, 0)); //画面外(右側)からスタート位置に移動
		FadeIn* fadein = FadeIn::create(0.01f);//非表示 → 表示
		FadeOut* fadeout = FadeOut::create(0.01f);//表示 → 非表示
		
		Sequence* seq = Sequence::create(delay1, move1, fadeout, move2, fadein, delay2, move3, nullptr);
		RepeatForever* forever = RepeatForever::create(seq);
		text_label->runAction(forever);
	}
}

bool CharaDetailSprite::getLockData(int chara_list_id)
{
    //カード保護情報を取得
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    HmsVector<string> favorite_character_array = split(favorite_character_list, "/");
    bool favorite_flg = false;
    for(int i=0; i<favorite_character_array.size(); i++) {
        if(fromString<int>(favorite_character_array.at(i))==chara_list_id) {
            favorite_flg = true;
        }
    }
    
    //HMSLOG("favorite_character_list = %s", SaveData::loadString("favorite_character_list").c_str());
    
    return favorite_flg;
}

void CharaDetailSprite::setLockData(int chara_list_id)
{
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    //追加
    if(favorite_character_list!="") favorite_character_list += "/";
    favorite_character_list += toString(chara_list_id);
    
    SaveData::saveString("favorite_character_list", favorite_character_list);
    
}

void CharaDetailSprite::removeLockData(int chara_list_id)
{
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    //解除
    string new_favorite_character_list = "";
    HmsVector<string> favorite_character_array = split(favorite_character_list, "/");
    for(int i=0; i<favorite_character_array.size(); i++) {
        if(fromString<int>(favorite_character_array.at(i))==chara_list_id) {
            favorite_character_array.at(i) = "";
        } else {
            if(new_favorite_character_list!="") new_favorite_character_list += "/";
            new_favorite_character_list += favorite_character_array.at(i);
        }
    }
    
    SaveData::saveString("favorite_character_list", new_favorite_character_list);
    
    HMSLOG("favorite_character_list = %s", SaveData::loadString("favorite_character_list").c_str());
    
}

void CharaDetailSprite::touchButtun(Ref *sender)
{
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();
    
    HMSLOG("lock tag %d",tag);
    if (tag == -1) {
        HMSLOG("not");
        //        return;
    }
    
    if (tag == LOCK_BUTTUN_TAG) {
        string fileName = "";
        Sprite * lock = (Sprite*)this->getChildByTag(LOCK_BUTTUN_TAG);
        
        if (getLockData(_chara_list_id)) {
            _lock_flg = false;
            fileName = "init/CharaMenu/bt_favorite_gray.png";
            removeLockData(_chara_list_id);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/key-tighten1.mp3",false);
            HMSLOG("LOCK OFF");
        }else{
            _lock_flg = true ;
            fileName = "init/CharaMenu/bt_favorite.png";
            setLockData(_chara_list_id);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/key-in1.mp3",false);
            HMSLOG("LOCK ON");
        }
        lock->setTexture(fileName);
    }
    
    if(tag==SWITCH_SKILL_BUTTON_TAG) {
        HMSLOG("switch skill");
        switchSkill();
    }
}

void CharaDetailSprite::deleteAction(Ref *sender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
    auto scale = ScaleTo::create(0.1f, 0);
    //窓を閉じる(最後に登録されたコールバックを呼び出す)
    this->runAction(Sequence::create(scale, CallFunc::create(CC_CALLBACK_0(CharaDetailSprite::callback,this)), nullptr));
}

void CharaDetailSprite::callback(){
    if(_parent!=nullptr && _sel!=nullptr) {
        _parent->scheduleOnce(_sel, 0.0f);
        _sel = nullptr;
    }
}

void CharaDetailSprite::switchSkill()
{
    if(_skill_type=="active_skill") _skill_type = "passive_skill";
    else _skill_type = "active_skill";
    
    _delay_time = 0;
    removeAllChildren();
    onEnter();
}

void CharaDetailSprite::setSwitchSkillButtonFlg(bool switch_skill_button_flg)
{
    _switch_skill_button_flg = switch_skill_button_flg;
    _skill_set_flg = false;
}

bool CharaDetailSprite::onTouchBegan(Touch *touch, Event *unused_event)
{
    return true;
}

void CharaDetailSprite::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void CharaDetailSprite::onTouchEnded(Touch *touch, Event *unused_event)
{
    
    //deleteAction(nullptr);
}

void CharaDetailSprite::onEnter()
{
    Sprite::onEnter();

    if(_delay_time==0) {
        setData();//情報の取得
        
        setupImage();//画像の設置
        
        setupLabel();//文字の設置
    } else {
        DelayTime *delay = DelayTime::create(_delay_time);
        CallFunc *expire1 = CallFunc::create([this]()
                                             {
                                                 this->setData();
                                                 this->setupImage();
                                                 this->setupLabel();
                                             });//ラムダ式
        this->runAction(Sequence::create(delay, expire1, NULL));
    }
}