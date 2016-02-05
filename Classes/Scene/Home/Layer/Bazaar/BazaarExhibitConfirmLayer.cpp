#include "BazaarExhibitConfirmLayer.h"
#include "BazaarCardDetailLayer.h"

BazaarExhibitConfirmLayer::BazaarExhibitConfirmLayer()
{
}

BazaarExhibitConfirmLayer* BazaarExhibitConfirmLayer::create()
{
    BazaarExhibitConfirmLayer *pRet = new BazaarExhibitConfirmLayer();
    
    if (pRet && pRet->init())
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

bool BazaarExhibitConfirmLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarExhibitConfirmLayer::init()");
    
    //初期データの設定
    
    _chara_id = SaveData::loadString("bazaar_exhibit_chara_id");
    _chara_list_id = SaveData::loadString("bazaar_exhibit_chara_list_id");
    under_ticket_count = "";
    open_flg = false;
    
    ratio = getVisibleRatio() ;
    
    return true;
}

void BazaarExhibitConfirmLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("出品確認", "BazaarExhibitCharaList", this);
    
    //キャラデータ表示
    createCharaData();
    
    //その他のレイアウト生成
    createBg();
}

void BazaarExhibitConfirmLayer::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
//    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BazaarExhibitConfirmLayer::createBg()
{
    //ボタン背景
    auto button_bg = Sprite::create("init/Home/Bazaar/bk_exhibit_jewel.png");
    button_bg->setScale(ratio);
    button_bg->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
    this->addChild(button_bg, Z_ButtonBg, T_ButtonBg);
    
    //オーブ数入力フォーム
//    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("希望する交換チケット数を設定してください", FONT_NAME_SYSTEM, 29);
//    label2->setScale(ratio);
//    label2->setAnchorPoint(Point(0, 0.5));
//    label2->setPosition(convertFromDisplaySize(Point(320, 130), 0.5, 0.5));
//  enableOutline(Color4B(0, 0, 0, 255), 3);
//	label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//    addChild(label2, 2);
    
    auto edit_box_size = Size(240 * ratio, 60 * ratio);
    auto pEditbox = ui::EditBox::create(edit_box_size, ui::Scale9Sprite::create("init/Home/Friend/entry_column.png"));
    pEditbox->setAnchorPoint(Point(0, 0.5));
    pEditbox->setPosition(convertFromDisplaySize(Point(150, 180), 0.5, 0.5));
    pEditbox->setInputMode(ui::EditBox::InputMode::DECIMAL);
    pEditbox->setFontColor(Color3B(255, 255, 255));
    pEditbox->setFont(FONT_NAME_SYSTEM, 40 * ratio);
    pEditbox->setDelegate(this);
    pEditbox->setMaxLength(8);
    pEditbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    
    int shortageTicketCount = getShortageTicketCount();
    string str = "出品枚数を入力。";
    if(shortageTicketCount != 0) str = toString(shortageTicketCount) + "枚以上で出品可能";
    pEditbox->setPlaceHolder(str.c_str());
    pEditbox->setPlaceholderFont(FONT_NAME_SYSTEM, 22 * ratio);
    //出品最低個数を記載するので、あれば↓この感じで書く
//    under_ticket_count = under_ticket_count + "枚以上で出品可能";
//    pEditbox->setPlaceHolder(under_ticket_count.c_str());
//    pEditbox->setPlaceholderFont(FONT_NAME_SYSTEM, 22 * ratio);
    this->addChild(pEditbox, Z_EditBox, T_EditBox);
    
    
    //出品ボタン
    auto exhibit_btn_on = Sprite::create("init/Home/Bazaar/button/bt_exhibit.png");
    auto exhibit_btn_off = Sprite::create("init/Home/Bazaar/button/bt_exhibit.png");
    exhibit_btn_on->setColor(Color3B(180,180,180));
    auto exhibit_btn = MenuItemSprite::create(exhibit_btn_off, exhibit_btn_on, [this](Ref*){
        if (exchange_ticket_count == "" || exchange_ticket_count == "0"){
            string text = "希望交換チケット数を\n設定してください。";
            auto popup_layer = popupNoChangePageLayer::create(text);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        } else {
            //出品する処理
            auto ok_func = [this](){
                //通信する
                exhibitStart();
            };
            auto ng_func = [](){}; //何もしない
    
            string message = "交換チケット"+exchange_ticket_count+"枚で出品します。\nよろしいですか？\n※交換希望者がいた場合、自動で交換されます。";
            auto popup_layer = popupConfirmLayer::create(message);
            popup_layer->setFunction(ok_func, ng_func);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
    });
    exhibit_btn->setScale(ratio);
    auto exhibit_menu = Menu::create(exhibit_btn, nullptr);
    exhibit_menu->setPosition(convertFromDisplaySize(Point(494, 180), 0.5, 0.5));
    this->addChild(exhibit_menu, Z_Button);
    
    //取引成立一覧
    auto history_btn_on = Sprite::create("init/Home/Bazaar/button/bt_trading_list.png");
    auto history_btn_off = Sprite::create("init/Home/Bazaar/button/bt_trading_list.png");
    history_btn_on->setColor(Color3B(180,180,180));
    auto history_btn = MenuItemSprite::create(history_btn_off, history_btn_on, [this](Ref*){
        this->historyLayerOpenClose();
    });
    history_btn->setTag(T_HistoryMenuItem);
    history_btn->setScale(ratio);
    auto history_menu = Menu::create(history_btn, nullptr);
    history_menu->setPosition(convertFromDisplaySize(Point(564, 787), 0.5, 1));
    this->addChild(history_menu, Z_HistoryMenu, T_HistoryMenu);
}

void BazaarExhibitConfirmLayer::createCharaData()
{
    //キャラ詳細の表示
    BazaarCardDetailLayer* bazaarCardDetailLayer = BazaarCardDetailLayer::create(_chara_id, "");
    bazaarCardDetailLayer->setPosition(convertFromDisplaySize(Point(320, 490), 0.5, 0.5));
    this->addChild(bazaarCardDetailLayer, Z_Chara_Sprite, T_Chara_Sprite);
}

void BazaarExhibitConfirmLayer::exhibitStart()
{
    //出品するリクエスト
    string user_master_id    = SaveData::loadString("user_master_id");
    string chara_list_id = SaveData::loadString("bazaar_exhibit_chara_list_id");
    
    string url = HOST;
    url += "bazaar/bazaar_exhibit.php";
    auto selector = httpresponse_selector(BazaarExhibitConfirmLayer::exhibitStartCallBack);
    string body = "user_master_id=" + user_master_id;
    body += "&chara_list_id=" + chara_list_id;
    body += "&exchange_ticket=" + exchange_ticket_count;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BazaarExhibitConfirmLayer::exhibitStartCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json.is_object())
    {
        //失敗
        if (json["result"].string_value() == "error"){
            string error_message = "想定外のエラーが発生しました。";
            if (json["other_txt"].string_value() != ""){
                error_message = json["other_txt"].string_value();
            }
            auto popup_layer = popupNoChangePageLayer::create(error_message);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
        //成功
        else if (json["result"].string_value() == "success"){
            //カードデータのリスト保存
            SaveData::saveString("chara_data_list", json["chara_data_list"].dump());
            
            //自分の出品リストを更新
            SaveData::saveString("bazaar_my_list", json["bazaar_my_list"].dump());
            
            auto layer = popupChangePageLayer::create("出品完了しました。", "BazaarExhibitTop");
            addChild(layer, 999999);
        }
    }
}

void BazaarExhibitConfirmLayer::getHistoryData()
{
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "bazaar/get_bazaar_history_list.php";
    auto selector = httpresponse_selector(BazaarExhibitConfirmLayer::createHistoryLayer);
    string body = "user_master_id="+user_master_id;
    body += "&chara_id="+_chara_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BazaarExhibitConfirmLayer::createHistoryLayer(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json.is_object())
    {
        //失敗
        if (json["result"].string_value() == "error"){
            string error_message = "想定外のエラーが発生しました。";
            if (json["other_txt"].string_value() != ""){
                error_message = json["other_txt"].string_value();
            }
            auto popup_layer = popupNoChangePageLayer::create(error_message);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
        else {
            //履歴レイヤー作成
            _history_data = json["bazaar_history"].object_items();
            _history_layer = BazaarExhibitHistoryLayer::create(_history_data);
            
            this->addChild(_history_layer, Z_History_BG, T_Hisitory_BG);
            
            //始めは小さく非表示にしておく
            _history_layer->setScale(0);
            
            //生成が終わったら大きくする
            historyLayerOpenClose();
        }
    }
}

void BazaarExhibitConfirmLayer::historyLayerOpenClose()
{
    //初めてレイヤーを開く場合は通信して、レイヤーを生成
    if (!_history_data.is_object()){
        getHistoryData();
    }
    //開いていたら閉じる、閉じていたら開く
    else {
        auto menuItem = (MenuItemSprite *)this->getChildByTag(T_HistoryMenu)->getChildByTag(T_HistoryMenuItem);
        //閉じていたら開く
        if (!open_flg){
            _history_layer->inPopupAction();
            
            //画像の切り替え
            menuItem->setNormalImage(Sprite::create("init/Home/Bazaar/button/bt_exhibit_detail.png"));
            menuItem->setSelectedImage(Sprite::create("init/Home/Bazaar/button/bt_exhibit_detail.png"));
        }
        //開いていたら閉じる
        else {
            _history_layer->outPopupAction();
            
            //画像の切り替え
            menuItem->setNormalImage(Sprite::create("init/Home/Bazaar/button/bt_trading_list.png"));
            menuItem->setSelectedImage(Sprite::create("init/Home/Bazaar/button/bt_trading_list.png"));
        }
        open_flg = !open_flg;
    }
}

int BazaarExhibitConfirmLayer::getShortageTicketCount(){
    int result = 0;
    string err;
    string  jsonText = SaveData::loadString("chara_data_list");
    HMSLOG(jsonText.c_str());
    Json json = Json::parse(SaveData::loadString("chara_data_list"), err);
    int countJson = countJsonData(json);
    for (int i = 0; i < countJson; i++) {
        if(json[toString(i)]["chara_id"].string_value() != _chara_id){
            continue;
        }
        
        result = stoi(json[toString(i)]["exhibit_exchange_ticket"].string_value());
    }
    
    return result;
}

void BazaarExhibitConfirmLayer::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    HMSLOG("editBegin!");
}
void BazaarExhibitConfirmLayer::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    HMSLOG("editEnd!");
}
void BazaarExhibitConfirmLayer::editBoxTextChanged(ui::EditBox *editBox, const std::string& text)
{
    //入力文字列を取得
    exchange_ticket_count = editBox->getText();
    HMSLOG("editChanged! text = %s", editBox->getText());
}
void BazaarExhibitConfirmLayer::editBoxReturn(ui::EditBox *editBox)
{
    HMSLOG("editReturn!");
}
