#include "BazaarCharaSprite.h"
#include "BazaarSearchListLayer.h"
#include "CardDetailLayer.h"
#include "BazaarPurchaseConfirmLayer.h"
#include "BazaarExhibitTopLayer.h"

BazaarCharaSprite::BazaarCharaSprite()
:bazaarLogId()
,charaId()
,secondName()
,name()
,nameKana()
,rare()
,charaType(CharaSprite::ALL)
,hp()
,power()
,needCost()
,addCost()
,turnCount()
,activeSkillId()
,passiveSkillId()
,activeSkillName()
,activeSkillNeedCost()
,passiveSkillName()
,price()
,movingFlg(false)
,shortagePriceFlg(false)
,exhibitStatus()
{
}

BazaarCharaSprite* BazaarCharaSprite::create(json11::Json json, Node* node, bool sreachFlg)
{
    BazaarCharaSprite *sprite = new BazaarCharaSprite();
    if (sprite && sprite->init(json, node, sreachFlg))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool BazaarCharaSprite::init(json11::Json json, Node* node, bool sreachFlg)
{
    if ( !Sprite::initWithFile("init/CharaMenu/revision/chara_exchange/bk_card_exchange.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //データの設定
    setupData(json);
    charaData = json;
    this->sreachFlg = sreachFlg;
    this->node = node;
    
    return true;
}

void BazaarCharaSprite::setupData(Json json)
{
    //データの設定
    setBazaarLogId(json["bazaar_log_id"].string_value());
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
    
    setActiveSkillName(json["active_skill_data"]["name"].string_value());
    setActiveSkillNeedCost(json["active_skill_data"]["need_cost"].string_value());
    setActiveSkillName(json["active_skill_data"]["name"].string_value());
    
    setPrice(json["exchange_ticket"].string_value());
    
    setExhibitStatus(json["status"].string_value());
}


void BazaarCharaSprite::onEnter()
{
    Sprite::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //キャラ画像の生成
    generateCharaSprite();
    
    //キャラ名の作成
    createNameLabel();
    
    //必要交換チケット枚数の表示
    showPriceLabel();
    
    //キャラ詳細、交換ボタンの作成
    createButton();
    
    //色全てのオブジェクトに適用させる処理（ノード上は適用させないので先にこれを記述）
    Utility::_setCascadeColorEnabled(this, true);
    
    //ノードを作成
    createDisableCascadeNode();
    
    //リストの上に置くボタンの作成
    createOnButton();
    
    //透過を全ての子に適用させる処理
    Utility::_setCascadeOpacityEnabled(this, true);
}


void BazaarCharaSprite::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarCharaSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarCharaSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarCharaSprite::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarCharaSprite::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void BazaarCharaSprite::generateCharaSprite()
{
    //キャラ画像の生成
    CharaSprite* chara = CharaSprite::create(charaData, false, CharaSprite::CardSize::Thumb_100);
    chara->setPosition(Point(72, this->getContentSize().height / 2));
    this->addChild(chara, Z_Chara_Sprite, T_Chara_Sprite);
}

void BazaarCharaSprite::createNameLabel()
{
    //キャラ名の表示
    Label* nameLabel = HMSFontUtil::createWithTTFBySystemFontArgument(name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(name, 34, 180));
    nameLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    nameLabel->setAnchorPoint(Point(0, 0.5));
    nameLabel->setPosition(Point(140, 100));
    nameLabel->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(nameLabel, Z_Name, T_Name);
}

void BazaarCharaSprite::createCharaTypeIcon()
{
    //タイプアイコンの表示
    string path = "";
    switch (charaType) {
        case CharaSprite::CharaType::OFFENSE: path = "init/CharaMenu/icon_offense3.png"; break;
        case CharaSprite::CharaType::DEFENSE: path = "init/CharaMenu/icon_defense3.png"; break;
        case CharaSprite::CharaType::STRATEGIST: path = "init/CharaMenu/icon_strategist3.png"; break;
        case CharaSprite::CharaType::GENERAL: path = "init/CharaMenu/icon_general3.png"; break;
        default: break;
    }
    
    if (path != "")
    {
        Sprite* charaTypeSprite = Sprite::create(path);
        charaTypeSprite->setPosition(Point(215, 195));
        this->addChild(charaTypeSprite, Z_Icon_Sprite, T_Icon_Sprite);
    }
}

void BazaarCharaSprite::showPower()
{
    //戦闘力
    Label* powerTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument("戦闘力", FONT_NAME_SYSTEM, 22);
    powerTextLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    powerTextLabel->setAnchorPoint(Point(0, 0.5));
    powerTextLabel->setPosition(Point(240, 195));
    this->addChild(powerTextLabel);
    
    Label* powerNumberLabel = HMSFontUtil::createWithTTFBySystemFontArgument(power, FONT_NAME_SYSTEM, 35);
    powerNumberLabel->setHorizontalAlignment(TextHAlignment::RIGHT);
    powerNumberLabel->setAnchorPoint(Point(1.0, 0.5));
    powerNumberLabel->setPosition(Point(390, 195));
    this->addChild(powerNumberLabel, Z_Power, T_Power);
}

void BazaarCharaSprite::showNeedCost()
{
    //配置士気
    Label* costTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument("配置士気", FONT_NAME_SYSTEM, 22);
    costTextLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    costTextLabel->setAnchorPoint(Point(0, 0.5));
    costTextLabel->setPosition(Point(420, 195));
    this->addChild(costTextLabel);
    
    Label* costNumberLabel = HMSFontUtil::createWithTTFBySystemFontArgument(needCost, FONT_NAME_SYSTEM, 35);
    costNumberLabel->setHorizontalAlignment(TextHAlignment::RIGHT);
    costNumberLabel->setAnchorPoint(Point(1.0, 0.5));
    costNumberLabel->setPosition(Point(565, 195));
    this->addChild(costNumberLabel, Z_Need_Cost, T_Need_Cost);
}

void BazaarCharaSprite::showPriceLabel()
{
    //必要交換数
    Label* countTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument("×", FONT_NAME_SYSTEM, 30);
    countTextLabel->setPosition(Point(216, 48));
    countTextLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(countTextLabel);
    
    Label* priceNumberLabel = HMSFontUtil::createWithTTFBySystemFontArgument(price, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(price, 30, 100));
    priceNumberLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    priceNumberLabel->setAnchorPoint(Point(0, 0.5));
    priceNumberLabel->setPosition(Point(230, 48));
    priceNumberLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(priceNumberLabel, Z_Price, T_Price);
}


void BazaarCharaSprite::createButton()
{
    //検索リスト
    if (!sreachFlg){
        //キャラ詳細
        Sprite* detailOff = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_detail.png");
        Sprite* detailOn  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_detail.png");
        detailOn->setColor(Color3B(180, 180, 180));
        
        MenuItemSprite* detailButton = MenuItemSprite::create(detailOff, detailOn,[this](Ref* sender){
            
            //キャラ詳細の表示
            CardDetailLayer* detailLayer = CardDetailLayer::create(charaData["chara_id"].string_value());
            node->getParent()->addChild(detailLayer, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
            
        });
        detailButton->setTag(T_Chara_Detail_MenuItem);
        
        auto detailMenu = Menu::create(detailButton, nullptr);
        detailMenu->setPosition(Point(388, 50));
        this->addChild(detailMenu, Z_Chara_Detail_Menu, T_Chara_Detail_Menu);

        
        
        //交換ボタン
        Sprite* purchaseButtonOff = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
        Sprite* purchaseButtonOn  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
        purchaseButtonOn->setColor(Color3B(180, 180, 180));
        
        auto purchaseButton = MenuItemSprite::create(purchaseButtonOff, purchaseButtonOn,[this](Ref* sender){
        
            if (shortagePriceFlg) {
                //所持している石が足りない場合、ポップアップの表示
                auto func = [](){};
                string message = "所持している交換チケット枚数が\n交換に必要な枚数に足りません。";
                Layer* popupLayer = PopupOkButtonLayer::create(func, message);
                node->getParent()->addChild(popupLayer, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
            }
            else {
                //交換確認ポップアップの表示
                BazaarPurchaseConfirmLayer* confirmLayer = BazaarPurchaseConfirmLayer::create(charaData, node->getParent());
                node->getParent()->getParent()->addChild(confirmLayer, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
            }
        });
        purchaseButton->setTag(T_Purchase_MenuItem);
        
        Menu* purchaseMenu = Menu::create(purchaseButton, nullptr);
        purchaseMenu->setPosition(Point(520, 50));
        this->addChild(purchaseMenu, Z_Purchase_Menu, T_Purchase_Menu);
    }
    //出品リスト
    else {
        //キャラ詳細
        Sprite* detailOff = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_detail.png");
        Sprite* detailOn  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_detail.png");
        detailOn->setColor(Color3B(180, 180, 180));
        
        MenuItemSprite* detailButton = MenuItemSprite::create(detailOff, detailOn,[this](Ref* sender){
            
            //キャラ詳細の表示
            CardDetailLayer* detailLayer = CardDetailLayer::create(charaData["chara_id"].string_value());
            node->addChild(detailLayer, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
            
        });
        detailButton->setTag(T_Chara_Detail_MenuItem);
        
        auto detailMenu = Menu::create(detailButton, nullptr);
        detailMenu->setPosition(Point(388, 50));
        this->addChild(detailMenu, Z_Chara_Detail_Menu, T_Chara_Detail_Menu);
        
        //出品中物
        if (getExhibitStatus() == "e"){
            //取り消しボタン
            auto exchange_off = Sprite::create("init/Home/Bazaar/button/bt_cancel.png");
            auto exchange_on  = Sprite::create("init/Home/Bazaar/button/bt_cancel.png");
            exchange_on->setColor(Color3B(180, 180, 180));
            
            auto exchange_btn = MenuItemSprite::create(exchange_off, exchange_on,[this](Ref* sender){
                auto bazaar_exhibit_top_layer = (BazaarExhibitTopLayer*)node;
                bazaar_exhibit_top_layer->exhibitComPopup(getBazaarLogId(), getExhibitStatus());
            });
            
            auto menu2 = Menu::create(exchange_btn, nullptr);
            menu2->setPosition(Point(520, 50));
            this->addChild(menu2, Z_Exhibit_Menu, T_Exhibit_Menu);
        }
        //出品キャンセル物 or 取引成立物
        else if (getExhibitStatus() == "c" || getExhibitStatus() == "f"){
            this->setColor(Color3B(100, 100, 100));
        }
    }
}

void BazaarCharaSprite::createOnButton()
{
    auto parent_node = this->getChildByTag(T_Node);
    
    //出品キャンセル物
    if (getExhibitStatus() == "c"){
        //キャンセル済み
        auto cancel = Sprite::create("init/Home/Bazaar/cancel_already.png");
        cancel->setPosition(Point(0, 0));
        parent_node->addChild(cancel, Z_Exhibit_Menu);
        
        //受け取るボタン(キャラ)
        auto get_chara_off = Sprite::create("init/Present/bt_receive.png");
        auto get_chara_on  = Sprite::create("init/Present/bt_receive.png");
        get_chara_on->setColor(Color3B(180, 180, 180));
        
        auto get_chara_btn = MenuItemSprite::create(get_chara_off, get_chara_on,[this](Ref* sender){
            auto bazaar_exhibit_top_layer = (BazaarExhibitTopLayer*)node;
            bazaar_exhibit_top_layer->exhibitComPopup(getBazaarLogId(), getExhibitStatus());
        });
        
        auto get_chara_menu = Menu::create(get_chara_btn, nullptr);
        get_chara_menu->setPosition(Point(218, 16));
        parent_node->addChild(get_chara_menu, Z_Exhibit_Menu, T_Exhibit_Menu);
    }
    //取引成立物
    else if (getExhibitStatus() == "f"){
        //キャンセル済み
        auto cancel = Sprite::create("init/Home/Bazaar/trading_already.png");
        cancel->setPosition(Point(0, 0));
        parent_node->addChild(cancel, Z_Exhibit_Menu);
        
        //受け取るボタン(交換チケット)
        auto get_ticket_off = Sprite::create("init/Present/bt_receive.png");
        auto get_ticket_on  = Sprite::create("init/Present/bt_receive.png");
        get_ticket_on->setColor(Color3B(180, 180, 180));
        
        auto get_ticket_btn = MenuItemSprite::create(get_ticket_off, get_ticket_on,[this](Ref* sender){
            auto bazaar_exhibit_top_layer = (BazaarExhibitTopLayer*)node;
            bazaar_exhibit_top_layer->exhibitComPopup(getBazaarLogId(), getExhibitStatus());
        });
        
        auto get_ticket_menu = Menu::create(get_ticket_btn, nullptr);
        get_ticket_menu->setPosition(Point(218, 16));
        parent_node->addChild(get_ticket_menu, Z_Exhibit_Menu, T_Exhibit_Menu);
    }
}

void BazaarCharaSprite::createDisableCascadeNode()
{
    //_setCascadeの後に作っているのでこれの上におけば適用されなくなる
    Node* node2 = Node::create();
    node2->setPosition(Point(getContentSize() / 2));
    this->addChild(node2, Z_Node, T_Node);
}

void BazaarCharaSprite::setUnselectedPurchaseButton()
{
    //交換ボタンを押せなくする（通常ボタンの画像を暗い画像にする）
    MenuItemSprite* purchaseButton = (MenuItemSprite *)this->getChildByTag(T_Purchase_Menu)->getChildByTag(T_Purchase_MenuItem);
    
    Sprite* purchaseSprite  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
    purchaseSprite->setColor(Color3B(180, 180, 180));
    purchaseButton->setNormalImage(purchaseSprite);
    
    shortagePriceFlg = true;
}


bool BazaarCharaSprite::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    
    movingFlg = false;
    
    return true;
}
void BazaarCharaSprite::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (abs(touch->getStartLocation().x - touch->getLocation().x) > 20
        || abs(touch->getStartLocation().y- touch->getLocation().y) > 20) {
        movingFlg = true;
    }
}
void BazaarCharaSprite::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    CharaSprite* chara = (CharaSprite*)this->getChildByTag(T_Chara_Sprite);
    
    //charaの絶対座標の値を取得
    Vec2 spWorldPoint = chara->convertToWorldSpace(Point::ZERO);
    Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, chara->getContentSize().width * ratio, chara->getContentSize().height * ratio);
    
    if(rect.containsPoint(touch->getLocation()) && !movingFlg) {
        
        //キャラ詳細の表示
        CardDetailLayer* cardDetailLayer = CardDetailLayer::create(this->charaData["chara_id"].string_value());
        this->node->getParent()->addChild(cardDetailLayer, BazaarSearchListLayer::ZOrder::Z_PopupLayer, BazaarSearchListLayer::Tag::T_PopupLayer);
    }
}
void BazaarCharaSprite::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}