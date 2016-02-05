#include "BazaarExhibitTopLayer.h"

bool BazaarExhibitTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarExhibitTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    //初期データ設定
    _cell_size = 156 * ratio;
    cansel_bazaar_log_id = "0";
    
    initData();
    
    return true;
}

void BazaarExhibitTopLayer::initData()
{
    exhibit_data_text = "";
    exhibits.clear();
    _vec_position_index.clear();
}

void BazaarExhibitTopLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("出品中のキャラ", "BazaarTop", this);
   
    //バザーのリスト以外の画面を生成
    createButton();
    
    //自分の出品情報リストを生成
    createExhibitList();
    
    //通信して自分の出品情報を取得
//    getExhibitData();
}

//自分の出品一覧を取得
void BazaarExhibitTopLayer::getExhibitData()
{
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "bazaar/get_bazaar_exchange_list.php";
    auto selector = httpresponse_selector(BazaarExhibitTopLayer::getExhibitDataCallBack);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

//出品一覧を取得完了
void BazaarExhibitTopLayer::getExhibitDataCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if (result != "not connect" && result != "response NG"){
		//出品データを保存
        SaveData::saveString("bazaar_my_list", result);
        
        //出品キャラの情報の取得
        string err;
        Json json = Json::parse(result, err);
        
        _my_bazaar_data_list = json["exhibit_log"].object_items();
    
        if (countJsonData(_my_bazaar_data_list)) {
            //ポジションインデックス
            createPositionIndex();
            
            //出品キャラリスト作成
            createScrollView();
            
            setScrollViewData();
            
            //交換済みのチェック
            //checkExchangedCharaAndShortage();
        }
        else {
            //出品キャラがいない場合
            string path = "init/Home/Popup/popup_400.png";
            auto _bg = Sprite::create(path);
            _bg->setScale(ratio);
            _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.0, 0.5));
            this->addChild(_bg, Z_NoExhibit, T_NoExhibit);
            
            auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("出品中のキャラはいません。", FONT_NAME_SYSTEM, 29);
            label2->setScale(ratio);
            label2->setAnchorPoint(Point(0.5, 0.5));
            label2->setPosition(convertFromDisplaySize(Point(320, 480), 0.0, 0.5));
            label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
            this->addChild(label2, Z_Message, T_Message);
        }
    }
}

void BazaarExhibitTopLayer::createExhibitList()
{
    //出品データ取得
    string result = SaveData::loadString("bazaar_my_list");
    
    //出品キャラの情報の取得
    string err;
    Json json = Json::parse(result, err);
    
    _my_bazaar_data_list = json["exhibit_log"].object_items();
    
    if (countJsonData(_my_bazaar_data_list)) {
        //ポジションインデックス
        createPositionIndex();
        
        //出品キャラリスト作成
        createScrollView();
        
        setScrollViewData();
        
        //交換済みのチェック
        //checkExchangedCharaAndShortage();
    }
    else {
        //出品キャラがいない場合
        string path = "init/Home/Popup/popup_400.png";
        auto _bg = Sprite::create(path);
        _bg->setScale(ratio);
        _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.0, 0.5));
        this->addChild(_bg, Z_NoExhibit, T_NoExhibit);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("出品中のキャラはいません。", FONT_NAME_SYSTEM, 29);
        label2->setScale(ratio);
        label2->setAnchorPoint(Point(0.5, 0.5));
        label2->setPosition(convertFromDisplaySize(Point(320, 480), 0.0, 0.5));
        label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        this->addChild(label2, Z_Message, T_Message);
    }
    
}

//出品リスト以外の画面生成
void BazaarExhibitTopLayer::createButton()
{
    //バザーチケット枚数
    auto bazaar_ticket = Sprite::create("init/CharaMenu/revision/chara_exchange/ticket_bk.png");
    bazaar_ticket->setScale(ratio);
    bazaar_ticket->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(bazaar_ticket, Z_BazaarTicketBg, T_BazaarTicketBg);
    
    string exchange_ticket_str = SaveData::loadString("exchange_ticket");
    auto bazaar_ticket_label = HMSFontUtil::createWithTTFBySystemFontArgument(exchange_ticket_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(exchange_ticket_str, 36, 100));
//    bazaar_ticket_label->setScale(ratio);
    bazaar_ticket_label->setAnchorPoint(Point(1.0, 0.5));
    bazaar_ticket_label->setPosition(Point(bazaar_ticket->getContentSize().width - 20, bazaar_ticket->getContentSize().height / 2));
    bazaar_ticket->addChild(bazaar_ticket_label, Z_BazaarTicket, T_BazaarTicket);
}

void BazaarExhibitTopLayer::createPositionIndex()
{
    //ポジションインデックスの作成
    int data_count = countJsonData(_my_bazaar_data_list);
    for (int i = 0; i < data_count; i++) {
        _vec_position_index.push_back(Point(320 * ratio, (_cell_size / 2) + (_cell_size * ((data_count - 1) - i))));
    }
}

void BazaarExhibitTopLayer::createScrollView()
{
    //スクロールビューの作成
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, Z_ScrollView, T_ScrollView);
    
    //スクロールの中身を作成
    pScrollView->setContainer(createScrollList());
}

void BazaarExhibitTopLayer::refreshScrollView()
{
    //いろいろ削除
    removeChildByTag(T_ScrollView);
    initData();
    
    //出品リストがある場合、リストを表示
    if (countJsonData(_my_bazaar_data_list)) {
        //ポジションインデックス
        createPositionIndex();
        
        //出品キャラリスト作成
        createScrollView();
        
        setScrollViewData();
    }
    //出品リストがない場合
    else {
        //出品キャラがいない場合
        string path = "init/Home/Popup/popup_400.png";
        auto _bg = Sprite::create(path);
        _bg->setScale(ratio);
        _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(_bg, Z_NoExhibit, T_NoExhibit);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("出品中のキャラはいません。", FONT_NAME_SYSTEM, 29);
        label2->setScale(ratio);
        label2->setAnchorPoint(Point(0.5, 0.5));
        label2->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        this->addChild(label2, Z_Message, T_Message);
    }
}

void BazaarExhibitTopLayer::sortExhibitChara()
{
    //交換済みが下にくるように並び替え
    sort(exhibits.begin(), exhibits.end(), [](const BazaarCharaSprite* chara1, const BazaarCharaSprite* chara2){
        return (chara2->getExhibitStatus() == "e" ? true : false);
    });
}

void BazaarExhibitTopLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, _cell_size * exhibits.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - ((_cell_size * exhibits.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* BazaarExhibitTopLayer::createScrollList()
{
    //スクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    layer->setTag(T_ScrollLayer);
    
    //出品キャラリストの一覧の作成
    int data_count = countJsonData(_my_bazaar_data_list);
    for (int i = 0; i < data_count; i++)
    {
        string iStr = toString<int>(i);
        
        //出品キャラ画像の作成
        exhibits.push_back(BazaarCharaSprite::create(_my_bazaar_data_list[iStr], this, true));
        exhibits[i]->setScale(ratio);
        layer->addChild(exhibits[i]);
    }
    
    //並び替え
    sortExhibitChara();
    
    //表示
    for (int i = 0; i < data_count; i++) {
        HMSLOG("position[%d]:%f  %f",i ,_vec_position_index[i].x, _vec_position_index[i].y);
        exhibits[i]->setPosition(_vec_position_index[i]);
    }
    
    return layer;
}

void BazaarExhibitTopLayer::exhibitComPopup(string bazaar_log_id, string status)
{
    //ログID保持
    cansel_bazaar_log_id = bazaar_log_id;
    
    //出品取り消し
    if (status == "e"){
        //取り消し確認ポップアップの表示
        auto ok_func = [this](){
            //出品するリクエスト
            string user_master_id = SaveData::loadString("user_master_id");
            
            string url = HOST;
            url += "bazaar/bazaar_exhibit_cansel.php";
            auto selector = httpresponse_selector(BazaarExhibitTopLayer::exhibitCanselCallback);
            string body = "user_master_id=" + user_master_id;
            body += "&bazaar_log_id=" + cansel_bazaar_log_id;
            HttpConnect::sendHttpRequest(this, url, selector, body);
        };
        auto ng_func = [](){}; //何もしない
        auto confirm_layer = popupConfirmLayer::create ("このキャラの出品を取り下げますか？");
        confirm_layer->setFunction(ok_func, ng_func);
        this->addChild(confirm_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
    //取り消し物受け取り
    else if (status == "c"){
        //取り消し物受け取り
        string user_master_id = SaveData::loadString("user_master_id");
        
        string url = HOST;
        url += "bazaar/get_bazaar_result.php";
        auto selector = httpresponse_selector(BazaarExhibitTopLayer::exhibitGetCharaCallback);
        string body = "user_master_id=" + user_master_id;
        body += "&bazaar_log_id=" + cansel_bazaar_log_id;
        HttpConnect::sendHttpRequest(this, url, selector, body);
    }
    //取引成立物受け取り
    else if (status == "f"){
        //取引成立物受け取り
        string user_master_id = SaveData::loadString("user_master_id");
        
        string url = HOST;
        url += "bazaar/get_bazaar_result.php";
        auto selector = httpresponse_selector(BazaarExhibitTopLayer::exhibitGetExchangeTicketCallback);
        string body = "user_master_id=" + user_master_id;
        body += "&bazaar_log_id=" + cansel_bazaar_log_id;
        HttpConnect::sendHttpRequest(this, url, selector, body);
    }
}

void BazaarExhibitTopLayer::exhibitCanselCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
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
        //既に落札済みかキャンセル済み
        else if (json["result"].string_value() == "finish"){
            //カードデータのリスト保存
            SaveData::saveString("chara_data_list", json["chara_data_list"].dump());
            
            //自分の出品リストを更新
            SaveData::saveString("bazaar_my_list", json["bazaar_my_list"].dump());
            _my_bazaar_data_list = json["bazaar_my_list"]["exhibit_log"].object_items();
            
            string error_message = "既に交換されたか、\nキャンセルされています。";
            if (json["other_txt"].string_value() != ""){
                error_message = json["other_txt"].string_value();
            }
            auto popup_layer = popupNoChangePageLayer::create(error_message);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
            
            //リスト表示を更新
            refreshScrollView();
        }
        //成功
        else if (json["result"].string_value() == "success"){
            //カードデータのリスト保存
            SaveData::saveString("chara_data_list", json["chara_data_list"].dump());
            
            //自分の出品リストを更新
            SaveData::saveString("bazaar_my_list", json["bazaar_my_list"].dump());
            _my_bazaar_data_list = json["bazaar_my_list"]["exhibit_log"].object_items();
            
            auto layer = popupNoChangePageLayer::create("出品を取り下げました。");
            addChild(layer, 999999);
            
            //リスト表示を更新
            refreshScrollView();
        }
    }
}

void BazaarExhibitTopLayer::actionUpdatePage()
{
    //ページ更新の際のアクション
    for (int i = 0; i < exhibits.size(); i++)
    {
        auto move = MoveTo::create(0.3f, _vec_position_index[i]);
        exhibits[i]->runAction(move);
    }
}

void BazaarExhibitTopLayer::exhibitGetCharaCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
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
        //キャラ数がいっぱい
        else if (json["result"].string_value() == "chara_max"){
            auto yes_func = [this](){
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("ShopMenu");
                this->removeFromParent();
                normal_scene->removeChildByTag(SHADOW_TAG);
            };
            auto no_func = [](){};
            PopupStayButtonLayer *popup = PopupStayButtonLayer::create(yes_func, no_func, "キャラの所持上限を超えてしまいます。\n最大キャラ所持数を追加しますか？") ;
            addChild(popup,999999) ;
            HMSLOG("所持上限を超えてしまいます！");
        }
        //成功
        else if (json["result"].string_value() == "success"){
            //カードデータのリスト保存
            SaveData::saveString("chara_data_list", json["chara_data_list"].dump());
            
            //自分の出品リストを更新
            SaveData::saveString("bazaar_my_list", json["bazaar_my_list"].dump());
            _my_bazaar_data_list = json["bazaar_my_list"]["exhibit_log"].object_items();
            
            auto layer = popupNoChangePageLayer::create("キャラを受け取りました。");
            addChild(layer, 999999);
            
            //リスト表示を更新
            refreshScrollView();
        }
    }
}

void BazaarExhibitTopLayer::exhibitGetExchangeTicketCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
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
        //失敗(キャラ数最大)
        else if (json["result"].string_value() == "chara_max"){
            string error_message = "キャラの所持数が上限を超えています。";
            auto popup_layer = popupNoChangePageLayer::create(error_message);
            this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
        //チケット枚数がいっぱい
        else if (json["result"].string_value() == "ticket_over"){
            string error_message = "交換チケット枚数が上限を超えて\nしまうため、受け取れません。";
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
            _my_bazaar_data_list = json["bazaar_my_list"]["exhibit_log"].object_items();
            
            //チケット数を更新
            string get_exchange_ticket = json["get_exchange_ticket"].string_value();
            string new_exchange_ticket = json["exchange_ticket"].string_value();
            SaveData::saveString("exchange_ticket", new_exchange_ticket);
            
            //チケット数表示を処理
            auto exchange_ticket_label = (Label *)this->getChildByTag(T_BazaarTicketBg)->getChildByTag(T_BazaarTicket);
            exchange_ticket_label->setString(new_exchange_ticket);
            exchange_ticket_label->setSystemFontSize(Utility::calcFlexibleFontSize(new_exchange_ticket, 36, 100));
            
            auto layer = popupNoChangePageLayer::create("交換チケットを"+get_exchange_ticket+"枚受け取りました。");
            addChild(layer, 999999);
            
            //リスト表示を更新
            refreshScrollView();
        }
    }
}

//出品物取り下げ確認
void BazaarExhibitTopLayer::confirmExhibitCansel(int tag)
{
    
}

void BazaarExhibitTopLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらボタンタッチをfalseにする
    touchButtonFlg = false;
}

void BazaarExhibitTopLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarExhibitTopLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarExhibitTopLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarExhibitTopLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarExhibitTopLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool BazaarExhibitTopLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (exhibits.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}

void BazaarExhibitTopLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void BazaarExhibitTopLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (Sprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                //ユーザー情報ページ
//                SaveData::saveString("to_user_master_id", exhibits[tag]->getUserMasterId());
//                SaveData::saveString("search_friend_id", exhibits[tag]->getFriendId());
//                changeUserDetail();
                confirmExhibitCansel(tag);
            }
            tag++;
        }
    }
}

void BazaarExhibitTopLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
