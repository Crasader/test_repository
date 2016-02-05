#include "CreateRoomLayer.h"

bool CreateRoomLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CreateRoomLayer::init()");
    
    ratio = getVisibleRatio();
    
    //ページ名の設定
    setName("createRoom");
    
    return true;
}

void CreateRoomLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    //HeaderLayer::setupTitleBar("ルーム作成", "BattleRoomTop", this);
    
    GpsManager::setDelegate(this);
    GpsManager::requestLocation();
}

void CreateRoomLayer::onSuccess(string gps_data)
{
    _gps_data = gps_data;
    HMSLOG("gps_data=%s", gps_data.c_str());
    
    
    //ルーム番号作成通信処理
    string url = HOST;
    url += "battle/create_battle_room.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&deck_no=" + SaveData::loadString("party_no");//デッキナンバー追加
    url += "&gps_data=" + _gps_data;
    
    string check_sum = HmsCrypto::makeCheckSum(_gps_data, "z38BSemzJ");
    url += "&cs=" + check_sum;
    
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        createRoomCallback(sender, response);
    });
}

void CreateRoomLayer::createRoomCallback(HttpClient *sender, HttpResponse *response)
{
    if(response->isSucceed()==false){//通信失敗したら
        CCLOG("通信失敗");
        runAction(Sequence::create(DelayTime::create(1.5f),CallFunc::create([this](){//少し時間をおき
            
            auto popup = PopupOk::create();
            popup->setPopup("タイムアウトしました。再送信します。",[this](){//ポップアップ出して再送信
                onSuccess(_gps_data);//再送信
            });
            addChild(popup,10000001);
            
        }), NULL));
        
        return;
    }
    
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("result = %s", result.c_str());
    
    string err;
    _battle_room_list = Json::parse(result, err);
    
    if (_battle_room_list["create_flg"].bool_value()) {
        //ルーム作成完了、又は既に作成されているルームがあった場合
        _room_key = _battle_room_list["room_key"].string_value();
        
        //背景の作成
        createBg();
        
        //ボタンの作成
        createButton();
        
        //マッチング中の通信を開始
        waittingBattleRequest();

    }
    else {
        //対戦中の履歴がある場合
    }
}

void CreateRoomLayer::waittingBattleRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "battle/waitting_battle.php";
    
    string body = "user_master_id=" + user_master_id + "&room_key=" + _room_key;
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        waittingBattleCallback(sender, response);
    }, body);
}

void CreateRoomLayer::waittingBattleCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    json = Json::parse(result, err);
    
    if (json["status"].string_value() == "wait") {
        //対戦相手待ち中 ※再帰
        HMSLOG("waitting...");
        CallFunc *expire1 = CallFunc::create([this]()
        {
            waittingBattleRequest();
        });
        this->runAction(Sequence::create(DelayTime::create(1.0f), expire1, NULL));
    }
    else if (json["status"].string_value() == "start"){
        //対戦相手が見つかったとき
        
        //ダンジョンシーンのデータを設定
        int battle_list_id =  fromString<int>(json["battle_list_id"].string_value());
        string enemy_user_master_id = json["enemy_user_list"]["user_master_id"].string_value();
        Utility::setOnlineBattleData(battle_list_id, enemy_user_master_id);
        
        //オンラインバトルタイプ（初心の手引きのフラグで使用）
        SaveData::saveString("online_battle_type", "create_room");
        
        //ダンジョンシーンへ（変更予定）
        auto scene = DungeonScene::createScene();
        auto transition = TransitionFade::create(1.0f, scene);
        Director::getInstance()->replaceScene(transition);
        
        //        //対戦相手(ルーム)が見つかったとき最終確認ページに飛ばす
        //        auto popup_layer = popupChangePageLayer::create(json["comment"].string_value(), "BattleDeckChoiceForOnline");
        //        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        
    }
    else {
        //作成した部屋がなくなったとき
        HMSLOG("lost room...");
    }
}

void CreateRoomLayer::createBg()
{
    //背景の作成
    _bg = Sprite::create("init/Home/Quest/Online/popup_room_create.png");
    _bg->setScale(ratio);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, 1);
    
    //表示番号
    _display_num = HMSFontUtil::createWithTTFBySystemFontArgument(_battle_room_list["room_key"].string_value(), FONT_NAME_SYSTEM, 55);
    _display_num->setPosition(Point(_bg->getContentSize().width / 2, 235));
    _bg->addChild(_display_num, 2);
    
    //GPS成功かどうか
    string text = "位置情報取得:-";
    if(_gps_data!="") text = "位置情報取得:○";
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 38);
    label->setPosition(Point(_bg->getContentSize().width / 2, 135));
    _bg->addChild(label, 2);
}

void CreateRoomLayer::createButton()
{
    //キャンセルボタン
    auto cancel_off = Sprite::create("init/Home/Common/bt_cancel.png");
    auto cancel_on  = Sprite::create("init/Home/Common/bt_cancel.png");
    cancel_on->setColor(Color3B(180, 180, 180));
    
    _cancel_btn = MenuItemSprite::create(cancel_off, cancel_on, [&](Ref* sender){
        
        waittingBattleRequest();
        
        //キャンセルしたらバトルリストを消去
        string user_master_id = SaveData::loadString("user_master_id");
        string url = HOST;
        url += "battle/cancel_wait_battle.php?user_master_id=" + user_master_id;
        
        HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
            cancelRoomCallback(sender,  response);
        });

    });
    _cancel_btn->setAnchorPoint(Point(0.5, 0));
    
    
    auto menu = Menu::create(_cancel_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 14));
    _bg->addChild(menu, 1);

}

void CreateRoomLayer::cancelRoomCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    string str = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("response:%s", str.c_str());
    //通信エラー以外
    if(str != "not connect" && str != "response NG") {
        
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("BattleRoomTop");
    }
}

