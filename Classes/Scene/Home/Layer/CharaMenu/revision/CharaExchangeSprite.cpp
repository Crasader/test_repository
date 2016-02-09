#include "CharaExchangeSprite.h"
#include "CharaExchangeLayer.h"
#include "CharaExchangeConfirmLayer.h"

CharaExchangeSprite::CharaExchangeSprite()
:_exchange_chara_id()
,_chara_id()
,_second_name()
,_name()
,_name_kana()
,_rare()
,_chara_type(CharaSprite::ALL)
,_hp()
,_power()
,_need_cost()
,_add_cost()
,_turn_count()
,_active_skill_id()
,_passive_skill_id()
,_start_date()
,_end_date()
,_active_skill_name()
,_active_skill_need_cost()
,_passive_skill_name()
,_need_exchange_ticket()
,_exchange_flg(false)
,_show_date_flg(false)
,_moving_flg(false)
,_shortage_exchange_ticket_flg(false)
{
}

CharaExchangeSprite* CharaExchangeSprite::create(json11::Json json, Node* node)
{
    CharaExchangeSprite *sprite = new CharaExchangeSprite();
    if (sprite && sprite->init(json, node))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaExchangeSprite::init(json11::Json json, Node* node)
{
    if ( !Sprite::initWithFile("init/CharaMenu/revision/chara_exchange/bk_card_exchange.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //データの設定
    setupData(json);
    _chara_data = json;
    
    _node = node;
    
    return true;
}

void CharaExchangeSprite::setupData(json11::Json json)
{
    //データの設定
    setExchangeCharaId(json["exchange_chara_id"].string_value());
    setCharaId(json["chara_id"].string_value());
    setSecondName(json["second_name"].string_value());
    setName(json["name"].string_value());
    setNameKana(json["name_kana"].string_value());
    setRare(json["rare"].string_value());
    setCharaType((CharaSprite::CharaType)fromString<int>(json["chara_type"].string_value()));
    
    setHp(json["hp"].string_value());
    setPower(json["power"].string_value());
    setNeedCost(json["need_cost"].string_value());
    setAddCost(json["add_cost"].string_value());
    setTurnCount(json["turn_count"].string_value());
    
    setStartDate(json["start_date"].string_value());
    setEndDate(json["end_date"].string_value());
    
    setActiveSkillName(json["active_skill_data"]["name"].string_value());
    setActiveSkillNeedCost(json["active_skill_data"]["need_cost"].string_value());
    setActiveSkillName(json["active_skill_data"]["name"].string_value());
    
    setNeedExchngeTicket(json["need_exchange_ticket"].string_value());
    setExchangeFlg(json["exchange_flg"].bool_value());
    setShowDateFlg(json["show_date_flg"].bool_value());
}

void CharaExchangeSprite::onEnter()
{
    Sprite::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //キャラ画像の生成
    generateCharaSprite();
    
    //キャラ名の作成
    createNameLabel();
    
    //必要交換チケット枚数の表示
    showNeedExchangeTicketLabel();
    
    //キャラ詳細、交換ボタンの作成
    createButton();
    
    //掲載終了時間の表示
    if (_show_date_flg) showExchangeEndDate();
    
    //色全てのオブジェクトに適用させる処理（ノード上は適用させないので先にこれを記述）
    Utility::_setCascadeColorEnabled(this, true);
    
    //ノードを作成
    createDisableCascadeNode();
    
    //透過を全ての子に適用させる処理
    Utility::_setCascadeOpacityEnabled(this, true);
}

void CharaExchangeSprite::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CharaExchangeSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaExchangeSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaExchangeSprite::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaExchangeSprite::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaExchangeSprite::generateCharaSprite()
{
    //キャラ画像の生成
    CharaSprite* chara = CharaSprite::create(_chara_data, false, CharaSprite::CardSize::Thumb_100);
    chara->setPosition(Point(72, this->getContentSize().height / 2));
    this->addChild(chara, Z_Chara_Sprite, T_Chara_Sprite);
}

void CharaExchangeSprite::createNameLabel()
{
    //キャラ名の表示
    Label* name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_name, 34, 180));
    name_label->setHorizontalAlignment(TextHAlignment::LEFT);
    name_label->setAnchorPoint(Point(0, 0.5));
    name_label->setPosition(Point(140, 100));
    name_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(name_label, Z_Name, T_Name);
}

void CharaExchangeSprite::createCharaTypeIcon()
{
    //タイプアイコンの表示
    string path = "";
    switch (_chara_type) {
        case CharaSprite::CharaType::OFFENSE: path = "init/CharaMenu/icon_offense3.png"; break;
        case CharaSprite::CharaType::DEFENSE: path = "init/CharaMenu/icon_defense3.png"; break;
        case CharaSprite::CharaType::STRATEGIST: path = "init/CharaMenu/icon_strategist3.png"; break;
        case CharaSprite::CharaType::GENERAL: path = "init/CharaMenu/icon_general3.png"; break;
        default: break;
    }
    
    if (path != "")
    {
        Sprite* chara_type_sp = Sprite::create(path);
        chara_type_sp->setPosition(Point(215, 195));
        this->addChild(chara_type_sp, Z_Icon_Sprite, T_Icon_Sprite);
    }
}

void CharaExchangeSprite::showPower()
{
    //戦闘力
    Label* label = HMSFontUtil::createWithTTFBySystemFontArgument("戦闘力", FONT_NAME_SYSTEM, 22);
    label->setHorizontalAlignment(TextHAlignment::LEFT);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(240, 195));
    this->addChild(label);
    
    Label* power_label = HMSFontUtil::createWithTTFBySystemFontArgument(_power, FONT_NAME_SYSTEM, 35);
    power_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    power_label->setAnchorPoint(Point(1.0, 0.5));
    power_label->setPosition(Point(390, 195));
    this->addChild(power_label, Z_Power, T_Power);
}

void CharaExchangeSprite::showNeedCost()
{
    //配置士気
    Label* label = HMSFontUtil::createWithTTFBySystemFontArgument("配置士気", FONT_NAME_SYSTEM, 22);
    label->setHorizontalAlignment(TextHAlignment::LEFT);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(420, 195));
    this->addChild(label);
    
    Label* need_cost = HMSFontUtil::createWithTTFBySystemFontArgument(_need_cost, FONT_NAME_SYSTEM, 35);
    need_cost->setHorizontalAlignment(TextHAlignment::RIGHT);
    need_cost->setAnchorPoint(Point(1.0, 0.5));
    need_cost->setPosition(Point(565, 195));
    this->addChild(need_cost, Z_Need_Cost, T_Need_Cost);
}

void CharaExchangeSprite::showNeedExchangeTicketLabel()
{
    //必要交換チケット枚数
    Label* label = HMSFontUtil::createWithTTFBySystemFontArgument("×", FONT_NAME_SYSTEM, 30);
    label->setPosition(Point(216, 48));
    label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(label);
    
    Label* exchange_ticket_label = HMSFontUtil::createWithTTFBySystemFontArgument(_need_exchange_ticket, FONT_NAME_SYSTEM, 36);
    exchange_ticket_label->setHorizontalAlignment(TextHAlignment::LEFT);
    exchange_ticket_label->setAnchorPoint(Point(0, 0.5));
    exchange_ticket_label->setPosition(Point(234, 48));
    exchange_ticket_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(exchange_ticket_label, Z_Exchange_Ticket, T_Exchange_Ticket);
}

void CharaExchangeSprite::createButton()
{
    //キャラ詳細
    Sprite* detail_off = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_detail.png");
    Sprite* detail_on  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_detail.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    detail_btn = MenuItemSprite::create(detail_off, detail_on,[this](Ref* sender){
        
        //キャラ詳細の表示
        CardDetailLayer* detail_layer = CardDetailLayer::create(_chara_data["chara_id"].string_value(), _chara_data["chara_list_id"].string_value());
        _node->addChild(detail_layer, CharaExchangeLayer::ZOrder::Z_Popup_Layer, CharaExchangeLayer::Tag::T_Popup_Layer);
        
    });
    detail_btn->setTag(T_Chara_Detail_MenuItem);
    
    Menu* menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(Point(388, 50));
    this->addChild(menu, Z_Chara_Detail_Menu, T_Chara_Detail_Menu);
    
    //交換ボタン
    Sprite* exchange_off = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
    Sprite* exchange_on  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
    exchange_on->setColor(Color3B(180, 180, 180));
    
    exchange_btn = MenuItemSprite::create(exchange_off, exchange_on,[this](Ref* sender){
        
        if (_shortage_exchange_ticket_flg) {
            //所持している交換チケットが足りない場合、ポップアップの表示
            auto func = [](){};
            string message = "所持している交換チケット枚数が\n交換に必要な枚数に足りません。";
            PopupOkButtonLayer* popup_layer = PopupOkButtonLayer::create(func, message);
            _node->addChild(popup_layer, CharaExchangeLayer::ZOrder::Z_Popup_Layer, CharaExchangeLayer::Tag::T_Popup_Layer);
        }
        else {
            //交換確認ポップアップの表示
            CharaExchangeConfirmLayer* confirm_layer = CharaExchangeConfirmLayer::create(_chara_data);
            _node->addChild(confirm_layer, CharaExchangeLayer::ZOrder::Z_Popup_Layer, CharaExchangeLayer::Tag::T_Popup_Layer);
        }
    });
    exchange_btn->setTag(T_Exchange_MenuItem);
    
    Menu* menu2 = Menu::create(exchange_btn, nullptr);
    menu2->setPosition(Point(520, 50));
    this->addChild(menu2, Z_Exchange_Menu, T_Exchange_Menu);
}

void CharaExchangeSprite::showExchangeEndDate()
{
    //交換終了期間の文字
    Label* end_date_label = HMSFontUtil::createWithTTFBySystemFontArgument(createExchageEndDateText(), FONT_NAME_SYSTEM, 22);
    end_date_label->setPosition(Point(450, 100));
    end_date_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(end_date_label);
}

string CharaExchangeSprite::createExchageEndDateText()
{
    string end_date = _end_date;
    
    end_date.replace(10, 9, "日まで");
    end_date.replace(7, 1, "月");
    end_date.replace(4, 1, "年");
    
    return end_date;
}

void CharaExchangeSprite::createDisableCascadeNode()
{
    //_setCascadeの後に作っているのでこれの上におけば適用されなくなる
    Node* node = Node::create();
    node->setPosition(Point(getContentSize() / 2));
    this->addChild(node, Z_Node, T_Node);
}

void CharaExchangeSprite::setUnselectedExchangeButton()
{
    //交換ボタンを押せなくする（通常ボタンの画像を暗い画像にする）
    MenuItemSprite* exchange_button_sprite = (MenuItemSprite *)exchange_btn;
    
    Sprite* exchange_sp  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
    exchange_sp->setColor(Color3B(180, 180, 180));
    exchange_button_sprite->setNormalImage(exchange_sp);
    
    _shortage_exchange_ticket_flg = true;
}

void CharaExchangeSprite::setExchangedChara()
{
    //交換済みの状態にする
    MenuItemSprite* exchange_btn_sprite = (MenuItemSprite *)exchange_btn;
    MenuItemSprite* detail_btn_sprite = (MenuItemSprite *)detail_btn;
    
    if (exchange_btn_sprite && detail_btn_sprite) {
        
        //ボタンを押せなくする
        exchange_btn_sprite->setEnabled(false);
        detail_btn_sprite->setEnabled(false);
        
        //画像全てを暗くする
        this->setColor(Color3B(100, 100, 100));
        
        //交換済み画像を載せる
        Node* node = (Node *)this->getChildByTag(CharaExchangeSprite::Tag::T_Node);
        Sprite* sp = Sprite::create("init/CharaMenu/revision/chara_exchange/exchange_already.png");
        sp->setPosition(Point(node->getContentSize() / 2));
        node->addChild(sp, 1);
    }
}

bool CharaExchangeSprite::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    
    _moving_flg = false;
    
    return true;
}
void CharaExchangeSprite::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (abs(touch->getStartLocation().x - touch->getLocation().x) > 20
        || abs(touch->getStartLocation().y- touch->getLocation().y) > 20) {
        _moving_flg = true;
    }
}
void CharaExchangeSprite::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    Node* chara = this->getChildByTag(T_Chara_Sprite);
    
    //charaの絶対座標の値を取得
    Point spWorldPoint = chara->convertToWorldSpace(Point::ZERO);
    Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, chara->getContentSize().width * ratio, chara->getContentSize().height * ratio);
    
    if(rect.containsPoint(touch->getLocation()) && _moving_flg == false && getExchangeFlg() == false) {
        
        //キャラ詳細の表示
        CardDetailLayer* detail_layer = CardDetailLayer::create(_chara_data["chara_id"].string_value(), _chara_data["chara_list_id"].string_value());
        _node->addChild(detail_layer, CharaExchangeLayer::ZOrder::Z_Popup_Layer, CharaExchangeLayer::Tag::T_Popup_Layer);
    }
}
void CharaExchangeSprite::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}