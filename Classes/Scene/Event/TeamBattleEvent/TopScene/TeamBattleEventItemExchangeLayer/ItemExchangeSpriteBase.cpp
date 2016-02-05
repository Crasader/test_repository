#include "ItemExchangeSpriteBase.h"
#include "PopupOkButtonLayer.h"

bool ItemExchangeSpriteBase::init(json11::Json json, Node* node)
{
    if ( !Sprite::initWithFile("init/CharaMenu/revision/chara_exchange/bk_card_exchange.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //データの設定
    setupData(json);
    _json_data = json;
    
    _node = node;
    
    return true;
}

void ItemExchangeSpriteBase::onEnter()
{
    Sprite::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //交換アイテムのデータセット
    setExchangeItemData();
    
    //報酬画像の生成
    generateItemSprite();
    
    //キャラ名の作成
    createNameLabel();
    
    //必要交換アイテム枚数の表示
    showNeedExchangeItemLabel();
    
    //キャラ詳細、交換ボタンの作成
    createButton();
    
    //掲載終了時間の表示
    if (_show_date_flg) showExchangeEndDate();
    
    //残り交換回数の表示
    if (is_exchange_count_remain) showExchangeCountRemain();
    
    //色全てのオブジェクトに適用させる処理（ノード上は適用させないので先にこれを記述）
    Utility::_setCascadeColorEnabled(this, true);
    
    //ノードを作成
    createDisableCascadeNode();
    
    //透過を全ての子に適用させる処理
    Utility::_setCascadeOpacityEnabled(this, true);
}

void ItemExchangeSpriteBase::setExchangeItemData()
{
    //交換に使用するアイテムのデータをセット(ひとまず交換チケットで初期化)
    exchange_item_sprite_path = "init/CharaMenu/revision/chara_exchange/icon_ticket.png";
    exchange_item_name = "交換チケット";
    exchange_item = fromString<int>(SaveData::loadString("exchange_ticket"));
}

void ItemExchangeSpriteBase::tapExchangeButtonAction()
{
}

void ItemExchangeSpriteBase::setupData(json11::Json json)
{
    //キャラかどうかフラグセット
    item_type = json["item_type"].string_value();
    is_chara = (item_type == "chara");
    
    //キャラ
    if(is_chara){
        _chara_data = json["reward_data"];
    }
    
    //共通データの保存
    setPrimaryId(json["team_battle_event_reward_exchange_id"].string_value());
    related_primary_id = json["related_primary_id"].string_value();
    is_exchange_count_remain = (json["exchange_count_remain_flg"].string_value()=="N");
    exchange_count_remain = fromString<int>(json["exchange_count_remain"].string_value());
    setName(json["reward_data"]["name"].string_value());
    setNeedExchangeItem(json["need_event_item"].string_value());
    setExchangeFlg(json["exchange_flg"].bool_value());
    setShowDateFlg(json["show_date_flg"].bool_value());
    setStartDate(json["start_date"].string_value());
    setEndDate(json["end_date"].string_value());
}

string ItemExchangeSpriteBase::getRewardItemPath(string item_type)
{
    //画像の切り替え
    if(item_type == "chara"){
        return "download/Chara/all400/" + related_primary_id + ".png";
    }
    if(item_type == "exchange_ticket"){
        return "init/Present/item7.png";
    }
    else if(item_type == "gold"){
        return "init/Present/item2.png";
    }
    else if(item_type == "jewel"){
        return "init/Present/item.png";
    }
    //TODO ガチャチケットや、一定時間獲得資金２倍アイテムとかも対応にしたい
    
    //何にもマッチしなかったら何もない画像返す
    return "download/Chara/parts100/card_bk1_reverse.png";
}

void ItemExchangeSpriteBase::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(ItemExchangeSpriteBase::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ItemExchangeSpriteBase::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ItemExchangeSpriteBase::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ItemExchangeSpriteBase::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void ItemExchangeSpriteBase::generateItemSprite()
{
    //キャラ画像の生成
    if(is_chara){
        item_sprite = CharaSprite::create(_chara_data, false, CharaSprite::CardSize::Thumb_100);
    }
    //それ以外の画像
    else {
        item_sprite = Sprite::create(getRewardItemPath(item_type));
    }
    item_sprite->setPosition(Point(72, this->getContentSize().height / 2));
    this->addChild(item_sprite, Z_Chara_Sprite, T_Chara_Sprite);
}

void ItemExchangeSpriteBase::createNameLabel()
{
    //報酬名の表示
    Label* name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_name, 34, 180));
    name_label->setHorizontalAlignment(TextHAlignment::LEFT);
    name_label->setAnchorPoint(Point(0, 0.5));
    name_label->setPosition(Point(140, 100));
    name_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(name_label, Z_Name, T_Name);
}

void ItemExchangeSpriteBase::showNeedExchangeItemLabel()
{
    //必要交換アイテム画像
    Sprite* exchange_item_sprite = Sprite::create(exchange_item_sprite_path);
    exchange_item_sprite->setPosition(Point(169, 48));
    this->addChild(exchange_item_sprite, Z_Exchange_Ticket);
    
    //必要交換アイテム枚数
    Label* label = HMSFontUtil::createWithTTFBySystemFontArgument("×", FONT_NAME_SYSTEM, 30);
    label->setPosition(Point(216, 48));
    label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(label);
    
    Label* exchange_ticket_label = HMSFontUtil::createWithTTFBySystemFontArgument(_need_exchange_item, FONT_NAME_SYSTEM, 36);
    exchange_ticket_label->setHorizontalAlignment(TextHAlignment::LEFT);
    exchange_ticket_label->setAnchorPoint(Point(0, 0.5));
    exchange_ticket_label->setPosition(Point(234, 48));
    exchange_ticket_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(exchange_ticket_label, Z_Exchange_Ticket, T_Exchange_Ticket);
}

void ItemExchangeSpriteBase::createButton()
{
    //交換ボタン
    Sprite* exchange_off = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
    Sprite* exchange_on  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
    exchange_on->setColor(Color3B(180, 180, 180));
    
    exchange_btn = MenuItemSprite::create(exchange_off, exchange_on,[this](Ref* sender){
        
        if (_shortage_exchange_ticket_flg) {
            //所持している交換アイテムが足りない場合、ポップアップの表示
            auto func = [](){};
            string message = "所持している"+exchange_item_name+"数が\n交換に必要な数に足りません。";
            PopupOkButtonLayer* popup_layer = PopupOkButtonLayer::create(func, message);
            _node->addChild(popup_layer, ZORDER_POPUP_LAYER);
        }
        else {
            //交換ボタン時の動き
            this->tapExchangeButtonAction();
        }
    });
    exchange_btn->setTag(T_Exchange_MenuItem);
    
    Menu* menu2 = Menu::create(exchange_btn, nullptr);
    menu2->setPosition(Point(520, 50));
    this->addChild(menu2, Z_Exchange_Menu, T_Exchange_Menu);
}

void ItemExchangeSpriteBase::showExchangeEndDate()
{
    //交換終了期間の文字
    Label* end_date_label = HMSFontUtil::createWithTTFBySystemFontArgument(createExchageEndDateText(), FONT_NAME_SYSTEM, 22);
    end_date_label->setPosition(Point(450, 100));
    end_date_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(end_date_label);
}

void ItemExchangeSpriteBase::showExchangeCountRemain()
{
    //残り交換回数の文字
    Label* count_remain_label = HMSFontUtil::createWithTTFBySystemFontArgument("残り"+toString(exchange_count_remain)+"回", FONT_NAME_SYSTEM, 22);
    count_remain_label->setPosition(Point(400, 50));
    count_remain_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(count_remain_label, 1, T_Exchange_Count_Remain);
}

int ItemExchangeSpriteBase::exchangeCountRemainDecrement()
{
    //交換回数制限のあるものであれば残り交換回数を１減らす
    if(is_exchange_count_remain){
        exchange_count_remain--;
        Label* count_remain_label = (Label*)this->getChildByTag(T_Exchange_Count_Remain);
        count_remain_label->setString(toString(exchange_count_remain));
    }
    return exchange_count_remain;
}

string ItemExchangeSpriteBase::createExchageEndDateText()
{
    string end_date = _end_date;
    
    end_date.replace(10, 9, "日まで");
    end_date.replace(7, 1, "月");
    end_date.replace(4, 1, "年");
    
    return end_date;
}

void ItemExchangeSpriteBase::createDisableCascadeNode()
{
    //_setCascadeの後に作っているのでこれの上におけば適用されなくなる
    Node* node = Node::create();
    node->setPosition(Point(getContentSize() / 2));
    this->addChild(node, Z_Node, T_Node);
}

void ItemExchangeSpriteBase::setUnselectedExchangeButton()
{
    //交換ボタンを押せなくする（通常ボタンの画像を暗い画像にする）
    MenuItemSprite* exchange_button_sprite = (MenuItemSprite *)exchange_btn;
    
    Sprite* exchange_sp  = Sprite::create("init/CharaMenu/revision/chara_exchange/bt_exchange.png");
    exchange_sp->setColor(Color3B(180, 180, 180));
    exchange_button_sprite->setNormalImage(exchange_sp);
    
    _shortage_exchange_ticket_flg = true;
}

void ItemExchangeSpriteBase::setExchangedItem()
{
    //交換済みの状態にする
    MenuItemSprite* exchange_btn_sprite = (MenuItemSprite *)exchange_btn;
    
    if (exchange_btn_sprite) {
    
        //ボタンを押せなくする
        exchange_btn_sprite->setEnabled(false);
        
        //画像全てを暗くする
        this->setColor(Color3B(100, 100, 100));
        
        //交換済み画像を載せる
        Node* node = (Node *)this->getChildByTag(ItemExchangeSpriteBase::Tag::T_Node);
        Sprite* sp = Sprite::create("init/CharaMenu/revision/chara_exchange/exchange_already.png");
        sp->setPosition(Point(node->getContentSize() / 2));
        node->addChild(sp, 1);
    }
}

bool ItemExchangeSpriteBase::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    _moving_flg = false;
    return true;
}
void ItemExchangeSpriteBase::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (abs(touch->getStartLocation().x - touch->getLocation().x) > 20
        || abs(touch->getStartLocation().y- touch->getLocation().y) > 20) {
        _moving_flg = true;
    }
}
void ItemExchangeSpriteBase::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //キャラ以外はreturn
    if(!is_chara) return;
    
    Node* chara = this->getChildByTag(T_Chara_Sprite);
    
    //charaの絶対座標の値を取得
    Point spWorldPoint = chara->convertToWorldSpace(Point::ZERO);
    Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, chara->getContentSize().width * ratio, chara->getContentSize().height * ratio);
    
    if(rect.containsPoint(touch->getLocation()) && _moving_flg == false && getExchangeFlg() == false) {
        //キャラ詳細の表示
        CardDetailLayer* detail_layer = CardDetailLayer::create(_chara_data["chara_id"].string_value(), "");
        _node->addChild(detail_layer, ZORDER_POPUP_LAYER);
    }
}