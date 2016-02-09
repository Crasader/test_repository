#include "CharaSellConfirmPopupBase.h"
#include "PopupStayButtonLayer.h"

bool CharaSellConfirmPopupBase::init(HmsVector<CharaSprite *> &sell_chara_list, int sell_price)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaSellConfirmPopupBase::init()");
    
    ratio = getVisibleRatio();
    
    //セルリストの初期化
    _sell_chara_list = sell_chara_list;
    _sell_price = sell_price;
    return true;
}

void CharaSellConfirmPopupBase::onEnter()
{
    Layer::onEnter();
    
    setNoAction();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //タイトルの作成
    createTitle();
    
    //はい、いいえボタンの作成
    createButton();
    
    //売却キャラ画像の作成
    createSellChara();
    
    //売却キャラの表示
    showSellChara();
    
    //交換チケット枚数の表示
    showExchangeTicket();
    
    //レア度チェック
    checkRarity();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void CharaSellConfirmPopupBase::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CharaSellConfirmPopupBase::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaSellConfirmPopupBase::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaSellConfirmPopupBase::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaSellConfirmPopupBase::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaSellConfirmPopupBase::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_700.png");
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    _bg->setScale(0);
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
}

void CharaSellConfirmPopupBase::createTitle()
{
    //タイトルの作成
    auto title = Sprite::create("init/Home/Popup/popup_index.png");
    title->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 45));
    _bg->addChild(title);
    
    //タイトル文字
    auto titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("売却キャラ確認", FONT_NAME_SYSTEM,33);
    titleLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    titleLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
    titleLabel->setPosition(Point(title->getContentSize() / 2));
    title->addChild(titleLabel, Z_Title, T_Title);
}

void CharaSellConfirmPopupBase::createButton()
{
    //はいボタン
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender){
        if (checkOverMaxGold()) {
            
            //現在のレイヤーを隠す
            Utility::hidePopupAction(this);
            
            //資金が超えてしまう警告ポップアップ
            auto yes_func = [this](){
                //通信処理
                sellCharaDataRequest();
            };
            
            auto no_func = [this](){
                //閉じて、表示
                Utility::openPopupAction(this);
            };
            
            auto popup_layer = PopupStayButtonLayer::create(yes_func, no_func, "売却の合計額が所持できる\n最大資金額を超えてしまいます。\n超えた場合最大額に設定されます。\nよろしいですか？");
            this->addChild(popup_layer, 999999);
        }
        else {
            //通信処理
            sellCharaDataRequest();
        }
    } );
    
    
    //いいえボタン
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, noAction);
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu);
    
    //売却メッセージ
    auto sell_confirm_label = HMSFontUtil::createWithTTFBySystemFontArgument("売却してもよろしいですか？", FONT_NAME_SYSTEM, 28);
    sell_confirm_label->setPosition(Point(_bg->getContentSize().width / 2, 120));
    _bg->addChild(sell_confirm_label);
}

void CharaSellConfirmPopupBase::createSellChara()
{
    //キャラ画像の作成
    for (int i = 0; i < _sell_chara_list.size(); i++)
    {
        _chara.push_back(CharaSprite::create(createCharaJsonData(i), false));
    }
}

Json CharaSellConfirmPopupBase::createCharaJsonData(int i)
{
    //キャラのJsonデータを作成 (C+11からの初期化子リストでコンテナの初期化を使用している)
    Json json = Json::object {
        {"chara_list_id", _sell_chara_list[i]->getCharaListId()},
        {"chara_id", _sell_chara_list[i]->getCharaId()},
        {"chara_no", _sell_chara_list[i]->getCharaNo()},
        {"second_name", _sell_chara_list[i]->getSecondName()},
        {"name", _sell_chara_list[i]->getName()},
        {"name_kana", _sell_chara_list[i]->getNameKana()},
        {"rare", _sell_chara_list[i]->getRare()},
        {"chara_type", toString<int>((int)_sell_chara_list[i]->getCharaType())},
        {"left_joint", toString<int>((int)_sell_chara_list[i]->getLeftJoint())},
        {"right_joint", toString<int>((int)_sell_chara_list[i]->getRightJoint())},
        {"hp", _sell_chara_list[i]->getHp()},
        {"power", _sell_chara_list[i]->getPower()},
        {"need_cost", _sell_chara_list[i]->getNeedCost()},
        {"add_cost", _sell_chara_list[i]->getAddCost()},
        {"turn_count", _sell_chara_list[i]->getTurnCount()},
        {"active_skill_id", _sell_chara_list[i]->getActiveSkillId()},
        {"passive_skill_id", _sell_chara_list[i]->getPassiveSkillId()},
        {"chara_comment", _sell_chara_list[i]->getCharaComment()},
        {"add_timing", _sell_chara_list[i]->getAddTiming()},
        {"insert_date", _sell_chara_list[i]->getInsertDate()},
        {"exchange_ticket", _sell_chara_list[i]->getExchangeTicket()},
    };
    
    return json;
}

Point CharaSellConfirmPopupBase::calcPositionChara(int i, int j)
{
    int x, y;
    
    x = POS_X_SELL_FIRST + (POS_X_SELL_DISTANCE * j);
    y = POS_Y_SELL_FIRST - (POS_Y_SELL_DISTANCE * i);
    
    return Point(x, y);
}

void CharaSellConfirmPopupBase::showExchangeTicket()
{
    //交換チケット枚数の表示
    int exchange_ticket_number = 0;
    for (int i = 0; i < _chara.size(); i++) {
        
        exchange_ticket_number += fromString<int>(_chara[i]->getExchangeTicket());
    }
    
    if (exchange_ticket_number > 0) {
        
        //背景画像
        auto exchange_bg = Sprite::create("init/CharaMenu/revision/chara_exchange/card_confirm_bk.png");
        exchange_bg->setPosition(Point(_bg->getContentSize().width / 2, 200));
        _bg->addChild(exchange_bg, Z_Exchange_Bg, T_Exchange_Bg);
        
        //文字
        string text = "×" + toString<int>(exchange_ticket_number) + "枚手に入ります";
        auto exchange_label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 28);
        exchange_label->setAnchorPoint(Point(0, 0.5));
        exchange_label->setPosition(Point(180, exchange_bg->getContentSize().height / 2));
        exchange_label->setColor(Color3B(255, 255, 144));
        exchange_bg->addChild(exchange_label, Z_Exchange_Label, T_Exchange_Label);
    }
}

void CharaSellConfirmPopupBase::checkRarity()
{
    //レア度チェック
    bool check_flg = false;
    for (int i = 0; i < _chara.size(); i++)
    {
        if (fromString<int>(_chara[i]->getRare()) >= CharaSprite::Rarity::SuperRare) {
            check_flg = true;
            
            //キャラを点滅させる
            auto tint1 = TintTo::create(0.5f, 255, 130, 130);
            auto tint2 = TintTo::create(0.5f, 255, 255, 255);
            _chara[i]->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.5f), tint1, tint2, nullptr)));
        }
    }
    
    if (check_flg)
    {
        //高いレア度売却警告文
        auto warning_label = HMSFontUtil::createWithTTFBySystemFontArgument("レア度の高いキャラが含まれています", FONT_NAME_SYSTEM, 28);
        warning_label->setPosition(Point(_bg->getContentSize().width / 2, 150));
        _bg->addChild(warning_label);
        
        auto tint1 = TintTo::create(0.5f, 255, 130, 130);
        auto tint2 = TintTo::create(0.5f, 255, 255, 255);
        warning_label->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.5f), tint1, tint2, nullptr)));
        
    }
}

bool CharaSellConfirmPopupBase::checkOverMaxGold()
{
    //売却合計値が最大値を超えていないかチェック
    if (fromString<int>(SaveData::loadString("gold")) + _sell_price > MAX_GOLD_COUNT) return true;
    else return false;
}

string CharaSellConfirmPopupBase::createRequestCharaSellList()
{
    //リクエストするときのキャラリストの作成
    string sell_list;
    
    for (int i = 0; i < _sell_chara_list.size(); i++) {
        sell_list += _sell_chara_list[i]->getCharaListId();
        if (i != _sell_chara_list.size() - 1) sell_list += ",";
    }
    
    return sell_list;
}

void CharaSellConfirmPopupBase::sellCharaDataRequest()
{
    //売却通信処理
    string url = HOST;
    url += "chara/chara_revision/sell_chara_list.php";
    
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&sell_chara=" + createRequestCharaSellList();
    auto selector = httpresponse_selector(CharaSellConfirmPopupBase::sellCharaDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void CharaSellConfirmPopupBase::sellCharaDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json.is_object())
    {
        //カードデータのリスト保存
        SaveData::saveString("chara_data_list", json["chara_data_list"].dump());
        
        //ヘッダー情報更新処理
        auto header_layer = (HeaderLayer *)this->getParent()->getParent()->getChildByTag(HEADER_LAYER_TAG);
        
        //資金更新処理
        auto gold_label = (Label *)header_layer->getChildByTag(GOLD_LABEL_TAG);
        int new_gold = fromString<int>(SaveData::loadString("gold")) + fromString<int>(json["sell_price"].string_value());
        gold_label->setString(toString<int>(new_gold));
        SaveData::saveString("gold", toString<int>(new_gold));
        
        //交換チケット更新処理
        string exchange_ticket = json["exchange_ticket"].string_value();
        if (exchange_ticket != "") {
            int new_exchange_ticket = fromString<int>(SaveData::loadString("exchange_ticket")) + fromString<int>(json["exchange_ticket"].string_value());
            SaveData::saveString("exchange_ticket", toString<int>(new_exchange_ticket));
        }
        
        auto sell_list_layer = (CharaSellListLayer *)this->getParent();
        
        //確認ポップアップの表示
        Utility::hidePopupAction(this);
        auto func = [this, sell_list_layer](){
            
            //売却完了後、キャラリストページを更新する
            bool chara_type[4] = {true, true, true, true};
            sell_list_layer->refreshNewPage(chara_type);
            
            Utility::closePopupAction(this);
        };
        string message =  toString<int>(_sell_chara_list.size()) + "体のキャラを売却して\n資金" + json["sell_price"].string_value();
        if (exchange_ticket != "" && fromString<int>(exchange_ticket) > 0) message += "と交換チケット" + exchange_ticket + "枚\n";
        message += "を手に入れました";
        
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        this->addChild(popup_layer, 999999);
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/money.mp3");
    }
    else
    {
        //売却失敗
        Utility::hidePopupAction(this);
        auto func = [this](){
            Utility::openPopupAction(this);
        };
        string message = "売却に失敗しました。\n通信環境を確認の上、時間を置き\n再度お試し下さい。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        this->addChild(popup_layer, 999999);
        
        
    }
}

bool CharaSellConfirmPopupBase::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan CharaSellConfirmPopupBase!");
    
    return true;
}
void CharaSellConfirmPopupBase::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void CharaSellConfirmPopupBase::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void CharaSellConfirmPopupBase::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
