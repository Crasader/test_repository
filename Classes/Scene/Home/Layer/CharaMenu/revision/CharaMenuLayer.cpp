#include "CharaMenuLayer.h"
#include "CharaCollectionGetRewardPopup.h"

bool CharaMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void CharaMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //キャラデータの取得
    getCharaDataRequest();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("キャラメニュー", "Mypage", this, HeaderLayer::TitleBarType::WIDTH_348);
    
    BazaarExhibitCharaListLayer::initRarityAndType();
    
    //所持交換チケット枚数の表示
    if(SaveData::loadBool("chara_exchange_flg")==true) showExchangeTicketSprite();
    
    //図鑑ボタンの表示
    if(SaveData::loadBool("chara_book_flg")==true) showBookButton();
    
    //ボタンの作成
    createButton();
}

void CharaMenuLayer::getCharaDataRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "chara/chara_revision/get_chara_list_info.php";
    string body = "user_master_id=" + user_master_id;
    body += "&resource_version=" + SaveData::loadString("resource_version") ;
    
    HttpConnectHms::sendHttpRequest(url,[this](HttpClient *client, HttpResponse *response){
        charaDataCallback(client, response);
    },body);
    
    LoadingManager::setLoading((Layer *)getParent(),"clear");//ローディング作成
}

void CharaMenuLayer::charaDataCallback(HttpClient *sender, HttpResponse *response)
{
    if(response->isSucceed()==false){//通信失敗したら
        CCLOG("通信失敗");
        runAction(Sequence::create(DelayTime::create(1.5f),CallFunc::create([this](){//少し時間をおき
            
            auto popup = PopupOk::create();
            popup->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5, 0.5) - popup->convertToWorldSpaceAR(Size::ZERO));
            popup->setPopup("タイムアウトしました。再送信します。",[this](){//ポップアップ出して再送信
                getCharaDataRequest();//再送信
            });
            
            getParent()->addChild(popup,10000001);
            
        }), NULL));
        
        return;
    }
    LoadingManager::removeLoading();//ローディング消去
    
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //ダウンロードする必要なデータがあった時タイトルに戻す処理
    if(json["needs_download"].bool_value()){
        auto normal_scene = (NormalScene *)this->getParent();
        auto func = [](){
            //タイトルへ
            auto scene = TitleScene::createScene();
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        };
        //ボップアップ表示
        string message = json["message"].string_value() ;
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        normal_scene->addChild(popup_layer, 999999);
        return ;
    }
    
    //キャラデータの保存
    SaveData::saveString("chara_data_list", json["chara_data_list"].dump());
    
    //イベントリスナーの登録
    initEventListener();
    
    //図鑑報酬があればPopup
    Json charaCollectionRewardData = json["chara_collection_reward"].object_items();
    if(!charaCollectionRewardData["item_type"].is_null()){
        string rewardComment = "やったわね♪\nキャラを集めてどんどん\n報酬を獲得しましょう♪";
        string rewardCharaName = "";
        if(charaCollectionRewardData["item_type"].string_value() == "chara"){
            rewardCharaName = charaCollectionRewardData["chara_name"].string_value();
        }
        CharaCollectionGetRewardPopup* popup = CharaCollectionGetRewardPopup::create(charaCollectionRewardData["item_type"].string_value(), charaCollectionRewardData["item_id"].string_value(), rewardComment, std::to_string(charaCollectionRewardData["count"].int_value()), "");
        //footerの上に出したい
        this->getParent()->addChild(popup,10000, 10000);
        //プレゼント数の更新
        int present_count = SaveData::loadInt("present_count") + charaCollectionRewardData["present_count"].int_value();
        SaveData::saveInt("present_count", present_count);
    }

    //トーナメントデータ取得
    Json _tournament_data_list = json["tournament_data_list"].object_items();
    if (fromString<int>(_tournament_data_list["wait_start_date_timestamp"].string_value()) > 0){
        SaveData::saveString("tournament_event_entry", "1");
        SaveData::saveString("tournament_event_deck_no", _tournament_data_list["tournament_event_deck_no"].string_value());
    }
    //不参加状態だったらエントリーフラグをリセット
    else {
        SaveData::saveString("tournament_event_entry", "0");
        SaveData::saveString("tournament_event_list_id", "0");
        SaveData::saveString("tournament_event_deck_no", "0");
    }

}

void CharaMenuLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CharaMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaMenuLayer::showExchangeTicketSprite()
{
    //所持チケット枚数
    auto bg = Sprite::create("init/CharaMenu/revision/chara_exchange/ticket_bk.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(bg, 1);
    
    string exchange_ticket = SaveData::loadString("exchange_ticket");
    auto exchange_ticket_label = HMSFontUtil::createWithTTFBySystemFontArgument(exchange_ticket, FONT_NAME_SYSTEM , Utility::calcFlexibleFontSize(exchange_ticket, 36, 100));
    exchange_ticket_label->setAnchorPoint(Point(1.0, 0.5));
    exchange_ticket_label->setPosition(Point(bg->getContentSize().width - 20, bg->getContentSize().height / 2));
    bg->addChild(exchange_ticket_label, 1);
    
}

void CharaMenuLayer::showBookButton()
{
    //図鑑ボタン
    auto book_btn_off = Sprite::create("init/CharaMenu/book/bt_book.png");
    auto book_btn_on  = Sprite::create("init/CharaMenu/book/bt_book.png");
    book_btn_on->setColor(Color3B(180, 180, 180));
    
    auto book_btn = MenuItemSprite::create(book_btn_off, book_btn_on,[this](Ref* sender){
        
        auto normal_scene = (NormalScene *)this->getParent();
        
        //WebView表示
        string url = HOST;
        url += "webview/book.php?user_master_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "キャラ図鑑");
        normal_scene->addChild(WebViewLoadLayer, 9);
    });
    book_btn->setScale(ratio);
    
    auto menu = Menu::create(book_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(412, 789), 0.5, 1));
    this->addChild(menu, 1);
}

void CharaMenuLayer::createButton()
{
    //string path_array[] = {"bt_formation.png", "bt_sell.png", "bt_charalist.png", "bt_up.png", "bt_up.png"};
    HmsVector<string> path_list;
    HmsVector<int> tag_list;
    path_list.insert("bt_formation.png");
    tag_list.insert(1);
    path_list.insert("bt_sell.png");
    tag_list.insert(2);
    path_list.insert("bt_charalist.png");
    tag_list.insert(3);

    if(SaveData::loadBool("chara_exchange_flg")==true) {
        path_list.insert("bt_chara_exchange.png");
        tag_list.insert(4);
    }
    if(SaveData::loadBool("chara_evolution_flg")==true) {
        path_list.insert("bt_evolution.png");
        tag_list.insert(5);
    }
    
    if(SaveData::loadBool("chara_bazaar_flg")) {
        path_list.insert("bt_basser.png");
        tag_list.insert(6);
    }
    
    for (int i = 0; i < path_list.size(); i++)
    {
        //ボタン画像
        _button_list.push_back(Sprite::create("init/Home/MenuButton/" + path_list.at(i)));
        _button_list[i]->setTag(tag_list.at(i));
    }
    
    auto func = [this](int tag){
        changePage(tag);
    };
    
    //スクロールレイヤーの作成
    int height = 160;
    if(SaveData::loadBool("chara_exchange_flg")==true) height -= 10;
    if(SaveData::loadBool("chara_evolution_flg")==true) height -= 10;
    if(SaveData::loadBool("chara_bazaar_flg")==true) height -= 10;
    auto scroll_layer = HmsScrollLayer::create(height * ratio, _button_list, func);
    this->addChild(scroll_layer, 1, SCROLLLAYER_TAG);
}

void CharaMenuLayer::changePage(int tag)
{
    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == T_Formation) {
        //デッキ編成へ
        normal_scene->changePage("DeckFormationTop");
    }
    else if (tag == T_Sell) {
        //キャラ売却へ
        normal_scene->changePage("CharaSellList");
    }
    else if (tag == T_CharaList) {
        //キャラ一覧へ
        normal_scene->changePage("CharaList");
    }
    else if (tag == T_Exchange) {
        //キャラ交換へ
        normal_scene->changePage("CharaExchange");
    }
    else if (tag == T_Evolution) {
        //キャラ進化へ
        normal_scene->changePage("CharaEvolution");
    }else if (tag == T_Basser) {
        //
        normal_scene->changePage("BazaarTop");
    }
}

bool CharaMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void CharaMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void CharaMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void CharaMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
