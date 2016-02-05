#include "CharaDetailSpriteForResultAnimation.h"

CharaDetailSpriteForResultAnimation* CharaDetailSpriteForResultAnimation::create(int chara_list_id){

    CharaDetailSpriteForResultAnimation *sprite = new CharaDetailSpriteForResultAnimation();

    if (sprite && sprite->init(chara_list_id))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaDetailSpriteForResultAnimation::init(int chara_list_id)
{
    if ( !Sprite::initWithFile("init/CharaMenu/chara_detail_base2.png") )//画像サイズ600x350
    {
        return false;
    }
    _chara_list_id = chara_list_id;
    
    _clipping_masc_width = 520 ;

    return true;
}

void CharaDetailSpriteForResultAnimation::setData()
{
    
    auto normal_scene = (NormalScene *)this->getParent()->getParent();
    
//    map<int, map<string, string> > character_list = normal_scene->character_list;
//    map<int, map<string, string> > active_skill_list = normal_scene->active_skill_list;
//    map<int, map<string, string> > passive_skill_list = normal_scene->passive_skill_list;
    
    int target_index = 0 ;
    
    int max_chara_count = fromString<int>(SaveData::loadString("max_chara_count"));
    for (int i=0; i< max_chara_count; i++) {
        if(normal_scene->character_list[i]["chara_list_id"] == toString<int>(_chara_list_id)){
            target_index = i;
            break;
        }
    }
    
    chara_id = normal_scene->character_list[target_index]["chara_id"];                 //該当キャラのID
    chara_no = normal_scene->character_list[target_index]["chara_no"];                 //キャラNo
    second_name = normal_scene->character_list[target_index]["second_name"];           //二つ名
    chara_name = normal_scene->character_list[target_index]["name"];                   //キャラ名
    name_kana = normal_scene->character_list[target_index]["name_kana"];               //キャラ名のフリガナ
    rare = normal_scene->character_list[target_index]["rare"];                         //レア度
    chara_power = normal_scene->character_list[target_index]["power"];                 //該当キャラの戦闘力
    _chara_type = normal_scene->character_list[target_index]["chara_type"];            //キャラタイプ
    need_cost = normal_scene->character_list[target_index]["need_cost"];               //配置コスト
    hp = normal_scene->character_list[target_index]["hp"];                             //体力
    add_cost = normal_scene->character_list[target_index]["add_cost"];                 //上昇士気
    turn_count = normal_scene->character_list[target_index]["turn_count"];             //継続ターン
    chara_comment = normal_scene->character_list[target_index]["chara_comment"];       //キャラコメント
    active_skill_id = normal_scene->character_list[target_index]["active_skill_id"];   //アクティブスキルID
    passive_skill_id = normal_scene->character_list[target_index]["passive_skill_id"]; //パッシブスキルID
    
    HMSLOG("active_skill_id=%s",active_skill_id.c_str());
    HMSLOG("passive_skill_id=%s",passive_skill_id.c_str());
    if(active_skill_id!="0" && passive_skill_id!="0") {
        _switch_skill_button_flg = true;
    }
    for (int i=0; i< max_chara_count; i++) {
        target_index = i ;
        if(normal_scene->active_skill_list[i]["active_skill_id"] == active_skill_id){
            break;
        }
    }
    active_skill_name = normal_scene->active_skill_list[target_index]["name"];              //アクティブスキル名
    active_skill_detail = normal_scene->active_skill_list[target_index]["detail"];          //アクティブスキル詳細
    active_skill_icon_detail = normal_scene->active_skill_list[target_index]["icon_detail"];//アクティブスキルアイコン
    active_skill_need_cost = normal_scene->active_skill_list[target_index]["need_cost"];    //アクティブスキル必要コスト
    HMSLOG("active_skill_name=%s",active_skill_name.c_str());
    HMSLOG("active_skill_detail=%s",active_skill_detail.c_str());
    if(active_skill_id != "0" && _skill_type=="active_skill")//アクティブスキルIDが0ではない場合
    {
        skill_name = active_skill_name;
        skill_detail = active_skill_detail;
        skill_icon_detail = active_skill_icon_detail;
    }else{
        for (int i=0; i< max_chara_count; i++) {
            if(normal_scene->passive_skill_list[i]["passive_skill_id"] == passive_skill_id){
                target_index = i ;
                break;
            }
        }
        skill_name = normal_scene->passive_skill_list[target_index]["name"];               //パッシブスキル名
        skill_detail = normal_scene->passive_skill_list[target_index]["detail"];           //パッシブスキル詳細
        skill_icon_detail = normal_scene->passive_skill_list[target_index]["icon_detail"]; //パッシブスキルアイコン
    }
}

void CharaDetailSpriteForResultAnimation::setupImage()
{
    
    //名前画像
    string name_img = StringUtils::format("download/Chara/name/%s.png", chara_id.c_str());
    auto name = Sprite::create(name_img.c_str());
    name->setAnchorPoint(Point(0.5, 0));
    name->setPosition(Point(this->getContentSize().width / 2, 276+80));
    this->addChild(name, 5);
    
    /*
    //スキル切り替えボタン
    if(_switch_skill_button_flg==true) {
        string fileName = "init/CharaMenu/chara_detail_p_a.png";
        
        auto switch_button = MenuItemImage::create(fileName, fileName, CC_CALLBACK_1(CharaDetailSpriteForResultAnimation::touchButtun, this));
        switch_button->setScale(1.0);
        switch_button->setTag(SWITCH_SKILL_BUTTON_TAG);
        switch_button->setAnchorPoint(Point(0.5, 0.5)) ;
        auto m_switch_button = Menu::create(switch_button, nullptr);
        m_switch_button->setPosition(Vec2(502,554-105));
        this->addChild(m_switch_button, 4);
    }
     */
    
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
        auto menu_img = MenuItemImage::create(filename1, filename1, CC_CALLBACK_1(CharaDetailSpriteForResultAnimation::touchButtun, this));
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
    
    if(second_name != ""){
        //キャラの二つ名がある場合
        name->setPosition(Point(name->getPosition().x - 70, name->getPosition().y));
        
        float second_x = name->getPosition().x + (name->getContentSize().width / 2 + 8);
        
        auto second_label = HMSFontUtil::createWithTTFBySystemFontArgument(second_name.c_str(), FONT_NAME_SYSTEM, 40);
        second_label->enableOutline(Color4B(0, 0, 0, 255), 3);
        second_label->setPosition(Point(second_x, name->getPosition().y));
        second_label->setAnchorPoint(Vec2(0, 0));
        this->addChild(second_label, 4, CharaDetailSpriteForResultAnimation::Tag::T_Second_name);
    }
    
    //名前のフリガナ（ノーマル以上のみ表示）
    if (1 < fromString<int>(rare) && name_kana != "")
    {
        //通常文字画像、二つ名を小さくする
        name->setScale(0.82f);
        auto second_label = (Label *)this->getChildByTag(CharaDetailSpriteForResultAnimation::Tag::T_Second_name);
        if (second_label) {
            second_label->setScale(0.82f);
        }
        
        //フリガナ
        auto kana_label = HMSFontUtil::createWithTTFBySystemFontArgument(name_kana.c_str(), FONT_NAME_SYSTEM, 18);
        kana_label->setPosition(Point(this->getContentSize().width / 2, 322+80));
        this->addChild(kana_label, 5);
    }
    
    //タイプ画像
    string i_type[]={"","offense","defense","strategist","general"};
    auto ic_type = StringUtils::format("init/CharaMenu/icon_%s2.png", i_type[fromString<int>(_chara_type)].c_str());
    auto icon_type = Sprite::create(ic_type.c_str());
    icon_type->setScale(1.0);
    icon_type->setPosition(Point(37,215+80));
    icon_type->setAnchorPoint(Vec2(0,0));
    this->addChild(icon_type, 5);
    
    /*
    //レアリティ画像
    string rar[]={"","n","r","sr","ur","le"};
    auto rarity = StringUtils::format("init/CharaMenu/icon_%s2.png", rar[fromString<int>(rare)].c_str());
    auto rare_icon = Sprite::create(rarity.c_str());
    rare_icon->setScale(1.0);
    rare_icon->setPosition(Point(40,280));
    rare_icon->setAnchorPoint(Vec2(0,0));
    this->addChild(rare_icon, 5);
     */
    
    /*
    //lock画像
    string fileName = "";
    if (getLockData(_chara_list_id)) {
        fileName = "init/CharaMenu/bt_favorite.png";
    }else{
        fileName = "init/CharaMenu/bt_favorite_gray.png";
    }
    auto lock_s = Sprite::create(fileName);
    lock_s->setScale(1.0);
    lock_s->setTag(LOCK_BUTTUN_TAG);
    lock_s->setPosition(Point(600,350));
    this->addChild(lock_s, 5);
    auto lock = MenuItemImage::create(fileName, fileName, CC_CALLBACK_1(CharaDetailSpriteForResultAnimation::touchButtun, this));
    lock->setScale(1.0);
    lock->setTag(LOCK_BUTTUN_TAG);
    auto m_lock = Menu::create(lock, nullptr);
    m_lock->setPosition(Point(660,410));
    this->addChild(m_lock,4);
     */
}

void CharaDetailSpriteForResultAnimation::setupLabel()
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
    laPowerLabel->setPosition(Point(power_label_right_x, 237+80));
    this->addChild(laPowerLabel,4);
    //戦闘力
    auto PowerLabel = HMSFontUtil::createWithTTFBySystemFontArgument(power_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,40);
    PowerLabel->setScale(1.0);
    PowerLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    PowerLabel->setAnchorPoint(Point(0,0.5));
    PowerLabel->enableOutline(Color4B::BLACK,1);
    PowerLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    PowerLabel->setPosition(Point(power_label_right_x + 5, 237+80));
    this->addChild(PowerLabel,4);
    //コストラベル
    auto laCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(lacost_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,26);
    laCostLabel->setScale(1.0);
//    laCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    laCostLabel->setAnchorPoint(Point(1,0.5));
    laCostLabel->enableOutline(Color4B::BLACK,1);
    laCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    laCostLabel->setPosition(Point(471, 237+80));
    this->addChild(laCostLabel,4);
    //コスト
    auto CostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(cost_array[fromString<int>(_chara_type)].c_str(),FONT_NAME_SYSTEM,40);
    CostLabel->setScale(1.0);
    CostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    CostLabel->setAnchorPoint(Point(0,0.5));
    CostLabel->enableOutline(Color4B::BLACK,1);
    CostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    CostLabel->setPosition(Point(476, 237+80));
    this->addChild(CostLabel,4);
    
    //スキル名
    auto la_skill_name = HMSFontUtil::createWithTTFBySystemFontArgument(skill_name, FONT_NAME_SYSTEM, 24);
    la_skill_name->setScale(1.0);
    la_skill_name->setColor(Color3B(255,153,0));
    la_skill_name->setHorizontalAlignment(TextHAlignment::LEFT);
    la_skill_name->setAnchorPoint(Point(0, 0.5));
    la_skill_name->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    la_skill_name->enableOutline(Color4B(0, 0, 0, 255), 1);
    la_skill_name->setPosition(Point(68-20,330-105));
    this->addChild(la_skill_name,5);
    
    if(active_skill_id != "0" && _skill_type=="active_skill")//アクティブスキルIDが0ではない場合
    {
        //発動士気ラベル
        auto laSkillCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument("発動士気", FONT_NAME_SYSTEM, 26);
        laSkillCostLabel->setScale(1.0);
        laSkillCostLabel->setColor(Color3B(255,153,0));
        laSkillCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
        laSkillCostLabel->setAnchorPoint(Point(1, 0.5));
        laSkillCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        laSkillCostLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
        laSkillCostLabel->setPosition(Point(568-60,330-105));
        this->addChild(laSkillCostLabel,5);
        //発動士気
        auto SkillCostLabel = HMSFontUtil::createWithTTFBySystemFontArgument(active_skill_need_cost, FONT_NAME_SYSTEM, 33);
        SkillCostLabel->setScale(1.0);
        SkillCostLabel->setColor(Color3B(255,153,0));
        SkillCostLabel->setHorizontalAlignment(TextHAlignment::LEFT);
        SkillCostLabel->setAnchorPoint(Point(1, 0.5));
        SkillCostLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        SkillCostLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
        SkillCostLabel->setPosition(Point(568-20,330-105));
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
    charaCommentLabel->setPosition(Point(300,111));
    this->addChild(charaCommentLabel,4);
    
    
    //アクティブスキル詳細
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
    
    /*
    //スキル詳細
    auto skill_text = HMSFontUtil::createWithTTFBySystemFontArgument(skill_detail, FONT_NAME_SYSTEM, 22);
    skill_text->setScale(1.0);
//    skill_text->setDimensions(1080, 65);
    skill_text->setHorizontalAlignment(TextHAlignment::LEFT);
    skill_text->setVerticalAlignment(TextVAlignment::TOP);
    skill_text->setAnchorPoint(Point(0.5, 1));
    skill_text->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
     skill_text->enableOutline(Color4B(0, 0, 0, 255), 1);
    skill_text->setPosition(Point(300,155));
    this->addChild(skill_text,5);
     */
}

Label* CharaDetailSpriteForResultAnimation::setupScrollLabel(Label* scroll_label, string text, Point pos){
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

void CharaDetailSpriteForResultAnimation::setScrollLavelAction(Label* text_label)
{
	//テキストの無限スクロール※一定アクションの繰り返し
	if(text_label->getContentSize().width > _clipping_masc_width) {
		text_label->setPosition(Point(0, 0));
		
		float move_speed_time = round(text_label->getContentSize().width / 100);
		DelayTime* delay1 = DelayTime::create(2.0f); //スタート位置からの待機時間
		DelayTime* delay2 = DelayTime::create(1.0f); //スタート位置からの待機時間
		MoveTo* move1 = MoveTo::create(move_speed_time, Point(-text_label->getContentSize().width, 0)); //画面外(左側)に移動
		MoveTo* move2 = MoveTo::create(0.01, Point(_clipping_masc_width, 0)); //画面外(右側)に配置
		MoveTo* move3 = MoveTo::create(move_speed_time, Point(0, 0)); //画面外(右側)からスタート位置に移動
		
        Sequence* seq = Sequence::create(delay1, move1, Hide::create(), move2, Show::create(), delay2, move3, nullptr);
		RepeatForever* forever = RepeatForever::create(seq);
		text_label->runAction(forever);
	}
}

void CharaDetailSpriteForResultAnimation::touchButtun(Ref *sender)
{
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();

    if(tag==SWITCH_SKILL_BUTTON_TAG) {
        HMSLOG("switch skill");
        switchSkill();
    }
}

void CharaDetailSpriteForResultAnimation::switchSkill()
{
    if(_skill_type=="active_skill") _skill_type = "passive_skill";
    else _skill_type = "active_skill";
    
    removeAllChildren();
    onEnter();
}

void CharaDetailSpriteForResultAnimation::setSwitchSkillButtonFlg(bool switch_skill_button_flg)
{
    _switch_skill_button_flg = switch_skill_button_flg;
}

bool CharaDetailSpriteForResultAnimation::onTouchBegan(Touch *touch, Event *unused_event)
{
    return true;
}

void CharaDetailSpriteForResultAnimation::onTouchMoved(Touch *touch, Event *unused_event)
{
}

void CharaDetailSpriteForResultAnimation::onTouchEnded(Touch *touch, Event *unused_event)
{
}

void CharaDetailSpriteForResultAnimation::onEnter()
{
    Sprite::onEnter();

    setData();//情報の取得

    setupImage();//画像の設置

    setupLabel();//文字の設置
}