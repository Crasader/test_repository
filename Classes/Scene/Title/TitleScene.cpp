#include "TitleScene.h"
#include "RankingEventResultLayer.h"
#include "ChallengeEventResultLayer.h"
#include "BossEventResultLayer.h"
#include "TournamentEventResultLayer.h"
#include "MapScene.h"

TitleScene::TitleScene()
:_needs_download(true)
{
}

Scene* TitleScene::createScene()
{
    auto scene = Scene::create();
    auto layer = TitleScene::create();
    scene->addChild(layer);
    return scene;
}

bool TitleScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TitleScene::init()");
    
    //シーン名の保存
    this->setName("TitleScene");
    
    //画面の比率取得
    ratio = getVisibleRatio();
        
    //BGMのプリロード
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("init/BGM/02_title.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("init/SE/dora.mp3");
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //ユーザー情報の取得
    _user_master_id = SaveData::loadString("user_master_id");
    _password       = SaveData::loadString("password");
    HMSLOG("user_master_id [%s] , password [%s]", _user_master_id.c_str(), _password.c_str());
    
    return true;
}


void TitleScene::createBg()
{
    //タイトル背景画像の作成
    auto bg = Sprite::create("init/Title/title640_1136.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, Z_Bg, T_Bg);
}

void TitleScene::createVersionCode(){
    Sprite *version_back = Sprite::create("init/Title/title_version_back.png") ;
    version_back->setScale(ratio) ;
    version_back->setAnchorPoint(Point(1, 0)) ;
    version_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH-10, 10), 0.5, 0.0));
    this->addChild(version_back, 2) ;
    
    string version_code = "" ;
    if(IF_ANDROID){
        version_code = APPLICATION_VIRSION_FOR_ANDROID ;
    }else if(IF_IOS){
        version_code = APPLICATION_VIRSION_FOR_IOS ;
    }
    auto version_label = HMSFontUtil::createWithTTFBySystemFontArgument("ver " + version_code , FONT_NAME_SYSTEM, 30);
    version_label->setAlignment(TextHAlignment::CENTER);
    version_label->setTextColor(Color4B::WHITE);
    version_label->setPosition(version_back->getContentSize()/2) ;
    version_label->enableOutline(Color4B::BLACK, 2);
    version_back->addChild(version_label);
    
    if(Utility::isTest())
    {
        //テストを向いているときタイトルに表示
        Sprite *test_back = Sprite::create("init/Title/title_version_back.png") ;
        test_back->setPosition(version_back->getContentSize().width/2, version_back->getContentSize().height) ;
        test_back->setAnchorPoint(Point(0.5, 0));
        version_back->addChild(test_back) ;
        
        auto test_label = HMSFontUtil::createWithTTFBySystemFontArgument("テスト",FONT_NAME_SYSTEM,34);
        test_label->setAlignment(TextHAlignment::CENTER);
        test_label->setTextColor(Color4B::WHITE);
        test_label->setPosition(test_back->getContentSize()/2) ;
        test_label->enableOutline(Color4B::BLACK, 2);
        test_back->addChild(test_label) ;
        
        //楽天かGooglePlayかの表示に使用
        if(IF_ANDROID){
            string store_name = "" ;
            if(RAKUTEN_FLG==1){
                store_name = "楽天App市場" ;
            }else if(RAKUTEN_FLG==0){
                store_name = "GooglePlay" ;
            }
            
            //テストを向いているときタイトルに表示
            Sprite *store_back = Sprite::create("init/Title/title_version_back.png") ;
            store_back->setPosition(0, version_back->getContentSize().height/2) ;
            store_back->setAnchorPoint(Point(1, 0.5));
            version_back->addChild(store_back) ;
            
            auto store_label = HMSFontUtil::createWithTTFBySystemFontArgument(store_name, FONT_NAME_SYSTEM, 25);
            store_label->setAlignment(TextHAlignment::CENTER);
            store_label->setTextColor(Color4B::WHITE);
            store_label->setPosition(store_back->getContentSize()/2) ;
            store_label->enableOutline(Color4B::BLACK, 2);
            store_back->addChild(store_label) ;
        }
    }
    
}

void TitleScene::createInfoButton()
{
    //問い合わせボタンの生成
    auto info_off = Sprite::create("init/Title/bt_notice.png");
    auto info_on  = Sprite::create("init/Title/bt_notice.png");
    
    auto info_btn = MenuItemSprite::create(info_off, info_on, [this](Ref* sender){
        
        //お知らせの表示
        string url = HOST;
        url += "user/index.php?p=page__info&user_master_id=" + SaveData::loadString("user_master_id");
        auto webview_layer = WebViewLoadLayer::create(url, "お知らせ");
        this->addChild(webview_layer, Z_WebView_Layer, T_WebView_Layer);
        
    });
    info_btn->setTag(T_Info_MenuItem);
    info_btn->setScale(ratio);
    info_btn->setAnchorPoint(Point(0.5 , 1.0));
    
    //お知らせボタンの生成
    auto inquiry_off = Sprite::create("init/Title/bt_contact.png");
    auto inquiry_on  = Sprite::create("init/Title/bt_contact.png");
    
    auto inquiry_btn = MenuItemSprite::create(inquiry_off, inquiry_on, [this](Ref* sender){
        
        //問い合わせの表示
        string url = HOST;
        url += "user/index.php?&p=page__inquiry&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "問い合わせ");
        this->addChild(WebViewLoadLayer, Z_WebView_Layer, T_WebView_Layer);
    });
    inquiry_btn->setTag(T_Inquiry_MenuItem);
    inquiry_btn->setScale(ratio);
    inquiry_btn->setAnchorPoint(Point(0.5 , 1.0));
    
    //引き継ぎボタンの生成
    auto takeover_off = Sprite::create("init/Title/bt_takeover.png");
    auto takeover_on  = Sprite::create("init/Title/bt_takeover.png");
    
    auto takeover_btn = MenuItemSprite::create(takeover_off, takeover_on, [this](Ref* sender){
        PopupModelChangeLayer *popup = PopupModelChangeLayer::create() ;
        this->addChild(popup, Z_Takeover_Popup) ;
    });
    takeover_btn->setEnabled(false) ;
    takeover_btn->setTag(T_ModelChange_MenuItem);
    takeover_btn->setScale(ratio);
    takeover_btn->setAnchorPoint(Point(0.5 , 1.0));
    
    //タップ有効になるまで無効
    Menu *menu = Menu::create(info_btn, inquiry_btn, takeover_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 960), 0.5, 1.0));
    menu->alignItemsHorizontallyWithPadding(4);
    this->addChild(menu, Z_Info_Menu, T_Info_Menu);
}

void TitleScene::setupBgm()
{
    //BGMと効果音(エフェクト)の音量設定
    float bgm_volume = 0.5f;
    float effect_volume = 0.5f;
    
    if (_user_master_id != "" && _password != "") {
        bgm_volume    = SaveData::loadFloat("bgm_volume");
        effect_volume = SaveData::loadFloat("effect_volume");
    }
    
    //BGMと効果音の音量設定
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
    
    //BGM再生
    SimpleAudioEngine::getInstance()->playBackgroundMusic("init/BGM/02_title.mp3",true);
}

void TitleScene::checkUserData()
{
    //ユーザー情報が端末に登録されているかチェック
    if (_user_master_id == "" && _password == "") {
        //新規ユーザ登録
        userRegistRequest();
    }
    else {
        //不正アクセスチェック
        userCheckRequest();
    }
}

void TitleScene::userRegistRequest()
{
    //ユーザ作成リクエスト
    string url = HOST;
    url += "user/user_regist.php?a=a";
    url += "&resource_version=" + SaveData::loadString("resource_version");
    auto selector = httpresponse_selector(TitleScene::userRegistCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TitleScene::userRegistCallback(HttpClient *sender, HttpResponse *response)
{
    //ユーザ作成レスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s",result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["user_master_id"].string_value()!="") {
        //メンバ変数の更新
        _user_master_id = json["user_master_id"].string_value();
        _password       = json["password"].string_value();
        
        //ユーザ情報登録
        SaveData::saveString("user_master_id", _user_master_id);
        SaveData::saveString("password", _password);
        SaveData::saveString("party_no", "1");
        SaveData::saveInt("sort_status", 1);
        SaveData::saveFloat("bgm_volume", 0.5);
        SaveData::saveFloat("effect_volume", 0.5);
        SaveData::saveBool("effect",true);

        //チュートリアルステータスの設定
        SaveData::saveInt("tutorialStatus", 0);
		
		//利用規約同意の有無
		SaveData::saveInt("useTermsStatus", 0);
		
		//資金決済法の課金額の年齢別制限の初期設定
		SaveData::saveInt("ageBuyStatus", 0);
        
        //リソースのダウンロードが必要か サーバーのリソースのバージョンID
        _needs_download = json["download_info"]["needs_download"].bool_value() ;
        _resource_version = json["download_info"]["resource_version"].string_value();
        
        //ダウンロード素材チェック
        setupResource();
    }
    else {
        //ユーザ登録失敗処理
        string text = "";
        if(json["check_error_message"].string_value()!="") {
            text = json["check_error_message"].string_value();
        } else {
            text = "ネットワーク通信を行いましたが、\n";
            text += "データを取得することができませんでした\n";
            text += "\n";
            text += "しばらく時間を置いた後、\n";
            text += "再度アクセスしてください。\n";
        }
        auto popup_layer = PopupStayLayer::create(text, "");
        addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
    }
}

void TitleScene::userCheckRequest()
{
    //不正アクセスチェックリクエスト
    string url = HOST;
    url += "user/user_check.php?";
    url += "resource_version=" + SaveData::loadString("resource_version");
    auto selector = httpresponse_selector(TitleScene::userCheckCallback);
    string body = "user_master_id="+_user_master_id+"&password="+_password;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TitleScene::userCheckCallback(HttpClient *sender, HttpResponse *response)
{
    //不正アクセスチェックレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //管理ユーザーだったらログを表示する
    if(json["admin_debug_flg"].bool_value()){
        HmsConsoleUtil::setVisibleLog(true) ;
    }else{
        if(Utility::isTest()){
            //テスト環境の時ログを表示
            HmsConsoleUtil::setVisibleLog(true) ;
        }
        
    }
    
    HMSLOG("%s",result.c_str());
	HMSLOG("app_status:%s",json["app_status"].string_value().c_str());

	//ユーザ認証失敗処理 メンテナンス　バージョンアップ　ポップアップ
	string text = "";
	string url = "";
    string type = "";
	if (json["app_maintenance_check"].bool_value()) {
		text = json["app_status_detail"].string_value();
		text = strReplace(text, "¥n", "\n");
	} else if (json["app_version_check"].bool_value()){
		text = "最新のバージョンが公開されました。\n";
		if (IF_IOS == 1) {
			text += "お手数ですがAppStoreから\nアップデートを行ってください。";
			url = "https://itunes.apple.com/jp/app/san-guo-zhibatoruu-ozu/id939935835?mt=8";
		} else {
            if(RAKUTEN_FLG == 0){
    			text += "お手数ですがGooglePlayから\nアップデートを行ってください。";
                url = "https://play.google.com/store/apps/details?id=jp.pkga.war&hl=ja";
            }else if(RAKUTEN_FLG == 1){
    			text += "お手数ですが楽天App市場から\nアップデートを行ってください。";
                url = "https://apps.rakuten.co.jp/detail/?appId=93";
            }
		}
    } else if (json["ban_check"].bool_value()){
        text = json["ban_text"].string_value();
        type = "ban";
	} else {
		if (json["check_flg"].bool_value()) {
            
        #if(IF_ANDROID)
            //シリアルコード付きshemeでアプリが起動された時、プレゼントを付与する処理
            bool is_pro = ( Utility::isTest() == false )  ;
            NativeBridge::checkSerialRewardAndroid(_user_master_id.c_str(), _password.c_str(), is_pro) ;
        #endif
            
            //リソースのダウンロードが必要か サーバーのリソースのバージョンID
            _needs_download = json["download_info"]["needs_download"].bool_value() ;
            _resource_version = json["download_info"]["resource_version"].string_value();
    
			//ダウンロード素材チェック
			setupResource();
		} else {
            if(json["check_error_message"].string_value()!="") {
                text = json["check_error_message"].string_value();
                //text = "認証に失敗したため、\n";
                //text += "このゲームデータをプレイすることはできません\n";
                //text += "尚、機種変更による引き継ぎを行った場合、\n";
                //text += "旧端末側でこの画面が表示されます。\n";
                //text += "新たなゲームデータにてプレイを開始するには\n";
                //text += "アプリの再インストールを行ってください。";
            } else {
                text = "ネットワーク通信を行いましたが、\n";
                text += "データを取得することができませんでした\n";
                text += "\n";
                text += "しばらく時間を置いた後、\n";
                text += "再度アクセスしてください。\n";
            }
		}
	}
	if(text != ""){
		if(url != "") {
			auto func = [this, url, text](){
				NativeBridge::openURL(url.c_str());
				auto popup_layer = PopupStayLayer::create(text, "");
				addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
			};
			auto popup_ok_layer = PopupOkButtonLayer::create(func, text);
			addChild(popup_ok_layer, Z_Popup_Layer, T_Popup_Layer);
		} else {
			auto popup_layer = PopupStayLayer::create(text, type);
			addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
		}
	}
}

void TitleScene::setupResource()
{
    //ストレージのパスを省略して使用できるようにする。
    Utility::setSearchPath(Utility::getDownloadSavePath()) ;
    
    //足りない素材のダウンロード
    if(DOWNLOAD_RESOURCE_FLG == 1 && _needs_download) {
        DownloadScene *download_scene = DownloadScene::create();
        
        //ダウンロード終了時の処理
        download_scene->setOnCompleteDownload([this](){
            HMSLOG("onCompleteDownload") ;
            //ダウンロードが完了し次第リソースバージョンを更新
            SaveData::saveString("resource_version", _resource_version) ;
            touchStart();
        }) ;
        
        this->addChild(download_scene, ZORDER_DOWNLOAD_LAYER);
        download_scene->setup();
    } else {
        touchStart();
    }
}

void TitleScene::touchStart()
{
    //タップ有効にする
    Menu *menu = (Menu*)getChildByTag(T_Info_Menu);
    MenuItem *takeover_btn = (MenuItemSprite *)menu->getChildByTag(T_ModelChange_MenuItem);
    takeover_btn->setEnabled(true);
    
    //タッチ画像
    _touch_sp = TouchScreenSprite::create();
    _touch_sp->setScale(ratio);
    _touch_sp->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0));
    this->addChild(_touch_sp, ZORDER_TOUCH_SCREAN, TOUCH_SCREAN_TAG);
    
    //イベントリスナーの設定
    touchON();
}

void TitleScene::touchON()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TitleScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TitleScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TitleScene::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TitleScene::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TitleScene::touchOFF()
{
    //タッチイベントOFF
    _eventDispatcher->removeEventListener(listener);
    
    //タッチ画像の削除
    _touch_sp->removeFromParent();
}

void TitleScene::getStartingDataRequest()
{
    //スターティングデータリクエスト
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "user/get_starting_data.php";
    auto selector = httpresponse_selector(TitleScene::startingDataCallback);
    string body = "user_master_id="+_user_master_id+"&password="+_password;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TitleScene::startingDataCallback(HttpClient *sender, HttpResponse *response)
{
    //スターティングデータレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    Json user_data_list = json["user_data_list"]["0"].object_items();
    
    //初期ユーザデータ保存
    SaveData::saveString("rank_name", user_data_list["rank_name"].string_value());
    SaveData::saveString("user_name", user_data_list["user_name"].string_value());
    SaveData::saveString("gold", user_data_list["gold"].string_value());
    SaveData::saveString("jewel", user_data_list["jewel"].string_value());
    SaveData::saveString("level", user_data_list["level"].string_value());
    SaveData::saveString("power", user_data_list["power"].string_value());
    SaveData::saveString("max_power", user_data_list["max_power"].string_value());
    SaveData::saveString("rank_point", user_data_list["rank_point"].string_value());
    SaveData::saveString("total_rank_point", user_data_list["total_rank_point"].string_value());
    SaveData::saveString("need_rank_point", user_data_list["need_rank_point"].string_value());
    SaveData::saveString("reward_point", user_data_list["reward_point"].string_value());
    SaveData::saveString("total_reward_point", user_data_list["total_reward_point"].string_value());
    SaveData::saveString("need_reward_point", user_data_list["need_reward_point"].string_value());
    SaveData::saveString("friend_id", user_data_list["friend_id"].string_value());
    SaveData::saveString("power_use_time", user_data_list["power_use_time"].string_value());
    SaveData::saveString("max_chara_count", user_data_list["max_chara_count"].string_value());
    SaveData::saveString("party_no", user_data_list["deck_no"].string_value());
    SaveData::saveString("max_chara_count", user_data_list["max_chara_count"].string_value());
    SaveData::saveString("comment", user_data_list["comment"].string_value());
    SaveData::saveString("dungeon_id", user_data_list["dungeon_id"].string_value());
    SaveData::saveString("dungeon_stage_id", user_data_list["dungeon_stage_id"].string_value());
    SaveData::saveString("beginner_guide_complete_flg", user_data_list["beginner_guide_complete_flg"].string_value());
    SaveData::saveString("total_rank", user_data_list["total_rank"].string_value());
    SaveData::saveString("weekly_rank", user_data_list["weekly_rank"].string_value());
    SaveData::saveString("weekly_rank_point", user_data_list["weekly_rank_point"].string_value());
    SaveData::saveString("timestamp_power_use_time", user_data_list["timestamp_power_use_time"].string_value());
    SaveData::saveString("exchange_ticket", user_data_list["exchange_ticket"].string_value());
    
    //function_control系
    SaveData::saveBool("review_demand", json["review_demand"].bool_value());
    SaveData::saveBool("chara_exchange_flg", json["chara_exchange_flg"].bool_value());
    SaveData::saveBool("chara_evolution_flg", json["chara_evolution_flg"].bool_value());
    SaveData::saveBool("chara_bazaar_flg", json["chara_bazaar_flg"].bool_value());
    SaveData::saveBool("chara_book_flg", json["chara_book_flg"].bool_value());
    
    //リスト保存
    SaveData::saveString("user_data_list", json["user_data_list"].dump());
    SaveData::saveString("user_chara_list", json["user_chara_list"].dump());
    SaveData::saveString("user_present_list", json["user_present_list"].dump());
    SaveData::saveString("user_party_list", json["user_party_list"].dump());
    SaveData::saveString("tips_list", json["tips_list"].dump());
    SaveData::saveString("event_list", json["event_list"].dump());
    SaveData::saveString("multi_event_list", json["multi_event_list"].dump());
    SaveData::saveString("online_battle_list", json["online_battle_list"].dump());
    //SaveData::saveString("gacha_banner_list", json["gacha_banner_list"].dump());
    SaveData::saveString("user_active_skill_list", json["user_active_skill_list"].dump());
    SaveData::saveString("user_passive_skill_list", json["user_passive_skill_list"].dump());
    SaveData::saveString("chara_sell_price", json["chara_sell_price"].dump());
    SaveData::saveString("shop_item_ios_list", json["shop_item_ios_list"].dump());
    SaveData::saveString("restart_data_list", json["restart_data_list"].dump());
    SaveData::saveString("chara_data_list", json["chara_data_list"].dump());
    SaveData::saveString("tournament_data_list", json["tournament_data_list"].dump());
    
    SaveData::saveInt("present_count", countJsonData(json["user_present_list"]));
	SaveData::saveInt("uvec_pkga_sum_coin", json["uvec_pkga_sum_coin"].int_value());
    
    //トーナメントデータ関連
    SaveData::saveString("tournament_entry_interval", json["tournament_data_list"]["tournament_entry_interval"].string_value());
    
    //獲得合計星数
    SaveData::saveInt("sum_reward_flg", json["sum_reward_flg"].int_value());
	
	//ゲーム設定保存
	SaveData::saveInt("max_same_card_count", fromString<int>(json["max_same_card_count"].string_value())); //デッキ内同一カード最大枚数
	SaveData::saveInt("active_skill_limit", fromString<int>(json["active_skill_limit"].string_value())); //アクティブスキル使用回数
    SaveData::saveString("beginner_guide_list", json["beginner_guide_list"].dump());
    
    //初心者クエストクリア状況確認
    Json beginnerGuideClearLog = json["beginner_guide_clear_log"].object_items();
    HMSLOG("#####################BEGIINERHOGEHOGEHOGE = %s", beginnerGuideClearLog.dump().c_str());
    for (int i = 0; i < countJsonData(beginnerGuideClearLog); i++) {
        string iStr = to_string(i);
        string keyName = "beginner_guide_clear_flg_" + beginnerGuideClearLog[iStr]["beginner_guide_id"].string_value();
        SaveData::saveBool(keyName.c_str(), beginnerGuideClearLog[iStr]["flg"].bool_value() );
    }
    
    //サーバー時間の取得
    SaveData::saveInt("server_time", json["server_time"].int_value());
    
    //端末の現在時刻を取得
    time_t now_time;
    time(&now_time);
    
    //サーバー時間と端末時間の差分を保持しておく
    int server_diff_time = (int)now_time - json["server_time"].int_value();
    SaveData::saveInt("server_diff_time", server_diff_time);
    HMSLOG("サーバーとの差分時間 = %d seconds", server_diff_time);
    
    //進化必要資金
    SaveData::saveInt("evolution_price", json["evolution_price"].int_value());
    HMSLOG("evolution_price=%d", SaveData::loadInt("evolution_price"));
    
    //チームID
    SaveData::saveInt("team_id", json["team_id"].int_value());
    HMSLOG("team_id=%d", json["team_id"].int_value());

    //既読チャットIDのリセット
    bool reset_chat_id_flg = json["reset_chat_id_flg"].bool_value();
    if(reset_chat_id_flg==true) {
        SaveData::saveString("team_chat_id_list_all", "");
        SaveData::saveString("team_chat_id_list_team", "");
    }
    
    HMSLOG("user_data_list = %s", SaveData::loadString("user_data_list").c_str());
    HMSLOG("user_chara_list = %s", SaveData::loadString("user_chara_list").c_str());
    HMSLOG("user_present_list = %s", SaveData::loadString("user_present_list").c_str());
    HMSLOG("user_party_list = %s", SaveData::loadString("user_party_list").c_str());
    HMSLOG("event_list = %s", SaveData::loadString("event_list").c_str());
    HMSLOG("multi_event_list = %s", SaveData::loadString("multi_event_list").c_str());
    HMSLOG("online_battle_list = %s", SaveData::loadString("online_battle_list").c_str());
    HMSLOG("gacha_banner_list = %s", SaveData::loadString("gacha_banner_list").c_str());
    HMSLOG("user_active_skill_list = %s", SaveData::loadString("user_active_skill_list").c_str());
    HMSLOG("user_passive_skill_list = %s", SaveData::loadString("user_passive_skill_list").c_str());
    HMSLOG("chara_sell_price = %s", SaveData::loadString("chara_sell_price").c_str());
	HMSLOG("uvec_pkga_sum_coin = %d", SaveData::loadInt("uvec_pkga_sum_coin"));
    HMSLOG("beginner_guide_list = %s", SaveData::loadString("beginner_guide_list").c_str());
    HMSLOG("restart_data_list = %s", SaveData::loadString("restart_data_list").c_str());
    HMSLOG("review_demand = %d ", SaveData::loadBool("review_demand"));
    HMSLOG("chara_data_list = %s ", SaveData::loadString("chara_data_list").c_str());
	
	HMSLOG("max_same_card_count = %d", SaveData::loadInt("max_same_card_count"));
	HMSLOG("active_skill_limit = %d", SaveData::loadInt("active_skill_limit"));

	/* リストの使い方(例) */
//    string json_text = SaveData::loadString("user_exp_data");
//    string err2;
//    Json json2 = Json::parse(json_text, err2);
//    
//    string exp = json2["1"]["need_experience"].string_value();
//    HMSLOG("exp = %s", exp.c_str());
    
    //シーン切り替え
    changeScene();
}

void TitleScene::changeScene()
{
    //オンラインバトルデータの取得
    string json_text = SaveData::loadString("online_battle_list");
    string err;
    _online_battle_list = Json::parse(json_text, err);
    
    //クエスト、イベント再開データの取得
    string json_text2 = SaveData::loadString("restart_data_list");
    string err2;
    _restart_data_list = Json::parse(json_text2, err2);
    
    //トーナメントデータの取得
    string json_text3 = SaveData::loadString("tournament_data_list");
    string err3;
    _tournament_data_list = Json::parse(json_text3, err3);
    
    //次回マイページ遷移時にデイリーミッションの判定を行うフラグ
    SaveData::saveBool("daily_mission_check_triger", true) ;
    
    //チャットの位置を初期化
    SaveData::saveInt("chat_position_y", 0);
    
    if (_online_battle_list["battle_list_id"].int_value() > 0 || _restart_data_list["event_battle_result_log_id"].string_value() != "" || _restart_data_list["quest_battle_result_log_id"].string_value() != "" ||
        _restart_data_list["ranking_event_stage_log_id"].string_value() != "" ||
        _restart_data_list["boss_event_stage_log_id"].string_value() != "" ||
        _restart_data_list["challenge_event_stage_log_id"].string_value() != "") {
        
        //データの初期化
        SaveData::saveString("online_battle_list", "");
        SaveData::saveString("restart_data_list", "");
        
        //ポップアップコメントの作成
        string popup_comment = "";
        
        //ボスイベント再開かどうか
        bool is_boss_event_restart_flg = false;
        
        if (_online_battle_list["battle_list_id"].int_value() > 0)
        {
            //オンラインバトル再開データの設定
            int battle_list_id = _online_battle_list["battle_list_id"].int_value();
            string enemy_user_master_id = _online_battle_list["enemy_user_master_id"].string_value();
            Utility::setOnlineBattleData(battle_list_id, enemy_user_master_id, true);
            popup_comment = "中断されているバトルがあります。\nバトルを再開しますか？\n\n※再開しない場合は、自動的に敗北となります。";
            
            //トーナメントバトル時の再開データのの設定
            if (_tournament_data_list["tournament_restart_flg"].bool_value()) {
                SaveData::saveString("tournament_event_list_id", _tournament_data_list["tournament_event_list_id"].string_value());
                popup_comment = "トーナメントイベント対戦中の\nデータがあります。再開しますか？\n\n※再開しない場合は、自動的に敗北となります。";
            }
            
        }
        else if (_restart_data_list["event_battle_result_log_id"].string_value() != "")
        {
            //イベントバトル再開データの設定
            SaveData::saveString("event_battle_result_log_id", _restart_data_list["event_battle_result_log_id"].string_value());
            SaveData::saveString("result_log_id", _restart_data_list["event_battle_result_log_id"].string_value()); //リザルトで送るデータを設定
            SaveData::saveString("choice_friend_id", _restart_data_list["choice_friend_id"].string_value());
            SaveData::saveString("npc_battle_event_rank_list_id", _restart_data_list["npc_battle_event_rank_list_id"].string_value());
            
            popup_comment = "中断されたイベントがあります。\nイベントを再開しますか？";
        }
        else if (_restart_data_list["quest_battle_result_log_id"].string_value() != "")
        {
            //クエストバトル再開データの設定
            SaveData::saveString("quest_battle_result_log_id", _restart_data_list["quest_battle_result_log_id"].string_value());
            SaveData::saveString("result_log_id", _restart_data_list["quest_battle_result_log_id"].string_value()); //リザルトで送るデータを設定
            SaveData::saveString("choice_friend_id", _restart_data_list["choice_friend_id"].string_value());
            SaveData::saveString("quest_stage_id", _restart_data_list["quest_stage_id"].string_value());
            
            popup_comment = "中断された中華統一戦記があります。\n中華統一戦記を再開しますか？";
        }
        else if (_restart_data_list["ranking_event_stage_log_id"].string_value() != "")
        {
            //ランキングバトル再開データの設定
            SaveData::saveString("ranking_event_stage_log_id", _restart_data_list["ranking_event_stage_log_id"].string_value());
            SaveData::saveString("choice_friend_id", _restart_data_list["choice_friend_id"].string_value());
            SaveData::saveString("ranking_event_stage_id", _restart_data_list["ranking_event_stage_id"].string_value());
            
            popup_comment = "中断された国士無双イベントがあります。\nイベントを再開しますか？";
        }
        else if (_restart_data_list["boss_event_stage_log_id"].string_value() != "")
        {
            //ボスイベント再開データの設定
            SaveData::saveString("boss_event_stage_log_id", _restart_data_list["boss_event_stage_log_id"].string_value());
            
            popup_comment = "参加中の匈奴討伐イベントがあります。\nイベントを再開しますか？";
            
            is_boss_event_restart_flg = true;
        }
        else if (_restart_data_list["challenge_event_stage_log_id"].string_value() != "")
        {
            //ランキングバトル再開データの設定
            SaveData::saveString("challenge_event_stage_log_id", _restart_data_list["challenge_event_stage_log_id"].string_value());
            SaveData::saveString("choice_friend_id", _restart_data_list["choice_friend_id"].string_value());
            SaveData::saveString("challenge_event_stage_id", _restart_data_list["challenge_event_stage_id"].string_value());
            
            popup_comment = "中断された試練イベントがあります。\nイベントを再開しますか？";
        }

        
        auto ok_func = [this, is_boss_event_restart_flg](){
            
            if (is_boss_event_restart_flg==true) {
                //Mapシーンへの遷移
                auto scene = MapScene::createScene();
                auto transition = TransitionFade::create(3.0f, scene);
                Director::getInstance()->replaceScene(transition);
            } else {
                //ダンジョンシーンへの遷移
                auto scene = DungeonScene::createScene();
                auto transition = TransitionFade::create(3.0f, scene);
                Director::getInstance()->replaceScene(transition);
            }
        };
        
        auto ng_func = [this]()
        {
            //再開データの初期化
            SaveData::saveString("battle_data_for_restart", "");
            
            if (_restart_data_list["ranking_event_stage_log_id"].string_value() != "") {
                SaveData::saveString("result_log_id", _restart_data_list["ranking_event_stage_log_id"].string_value());

                RankingEventResultLayer *result_layer = RankingEventResultLayer::create(false, "TitleScene");
                this->addChild(result_layer, 20);
            } else if (_restart_data_list["boss_event_stage_log_id"].string_value() != "") {
                //ページ遷移の処理
                auto scene = NormalScene::createScene("");
                auto transition = TransitionFade::create(3.0f, scene);
                Director::getInstance()->replaceScene(transition);
            } else if (_restart_data_list["challenge_event_stage_log_id"].string_value() != "") {
                SaveData::saveString("result_log_id", _restart_data_list["challenge_event_stage_log_id"].string_value());
                    
                ChallengeEventResultLayer *result_layer = ChallengeEventResultLayer::create(false, "TitleScene");
                this->addChild(result_layer, 20);
            } else if (_tournament_data_list["tournament_restart_flg"].bool_value()) {
                SaveData::saveString("tournament_event_list_id", _tournament_data_list["tournament_event_list_id"].string_value());
                TournamentEventResultLayer *result_layer = TournamentEventResultLayer::create(false, "TitleScene");
                this->addChild(result_layer, 20);
            } else {
                //リザルト（レイヤーの中身は表示させていない）
                ResultLayer *result_layer = ResultLayer::create(false, "TitleScene");
                this->addChild(result_layer, 20);
            }
        };
        
        //確認レイヤーの作成
        popupConfirmLayer *layer = popupConfirmLayer::create(popup_comment);
        layer->setFunction(ok_func, ng_func);
        this->addChild(layer, 20);
    }
    else
    {
        //再開データの初期化
        SaveData::saveString("battle_data_for_restart", "");
        
        //SE（ドラの音)
        float effect_volume = SimpleAudioEngine::getInstance()->getEffectsVolume();
        if (effect_volume != 0) SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume + 0.3f);
        SimpleAudioEngine::getInstance()->playEffect("init/SE/dora.mp3");
        
        //BGMの消音
        this->runAction(FadeToVolumeOfBGM::create(0.5f, 0.0f));
        
        auto func = CallFunc::create([](){
            
            //ノーマルシーンへの遷移
            auto scene = NormalScene::createScene("");
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        });
        
        auto func2 = CallFunc::create([](){
            //音量を元に戻す
            SimpleAudioEngine::getInstance()->setEffectsVolume(SaveData::loadFloat("effect_volume"));
        });
        
        runAction(Sequence::create(func, DelayTime::create(3.0f), func2, nullptr));
    }
}

void TitleScene::cancelBattleCallback(HttpClient *sender, HttpResponse *response)
{
    //スターティングデータレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s", result.c_str());

}

void TitleScene::changeTutorialScene()
{
    //チュートリアルシーンへの遷移
    Scene* scene = nullptr;
    
    if (SaveData::loadInt("tutorialStatus") == 1)
    {
        //バトルチュートリアルの表示
        scene = DungeonScene::createScene();
        auto transition = TransitionFade::create(1.0f, scene);
        Director::getInstance()->replaceScene(transition);
    } else {
        scene = TutorialScene::createScene();
        auto transition = TransitionFade::create(3.0f, scene);
        Director::getInstance()->replaceScene(transition);
    }

    //BGMの消音
    this->runAction(Sequence::create(FadeToVolumeOfBGM::create(0.3f, 0.0f),
                    CallFunc::create([](){HMSLOG("volume:%f", SimpleAudioEngine::getInstance()->getBackgroundMusicVolume());}),nullptr)
                    );

    //SE
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("init/SE/dora.mp3");
}

bool TitleScene::onTouchBegan(Touch *touch, Event *event)
{
    HMSLOG("TitleScene::touchBegan!");
    return true;
}

void TitleScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TitleScene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タッチイベントの削除
    this->touchOFF();
	
	if (SaveData::loadInt("useTermsStatus") == 0) {
		ModalLayer* layer = ModalLayer::create(MODAL_CONTENTS_TERMS);
		this->addChild(layer, 3000, 100);
	} else if (SaveData::loadInt("tutorialStatus") < 3) {
        //チュートリアルへの遷移
        changeTutorialScene();
    } else {
        //スターティングデータ通信処理
        getStartingDataRequest();
    }
}

void TitleScene::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void TitleScene::createTutorialButton()
{
    //デバック用
    auto btn_off = Sprite::create("init/CharaMenu/icon_gi.png");
    auto btn_on  = Sprite::create("init/CharaMenu/icon_gi.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto menuItem = MenuItemSprite::create(btn_off, btn_on, [this](Ref* sender)
    {
        //チュートリアルステータスの変更
        SaveData::saveInt("tutorialStatus", 0);
        
        //チュートリアルへの遷移
        changeTutorialScene();
    });
    menuItem->setScale(3.0 * ratio);
    menuItem->setAnchorPoint(Point(0.5, 0));
    auto Menu = Menu::create(menuItem, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(100, 800), 0.5, 1));
    this->addChild(Menu, 1);
}

void TitleScene::onEnter()
{
    Layer::onEnter();
    
    //背景画像の作成
    createBg();
    
    //両サイドの余白に画像を配置する
    cutLeftAndRightVisibleSpace() ;
    
    //バージョンコードの作成
    createVersionCode() ;
    
    //BGMの作成
    setupBgm();
    
    //お知らせ、問い合わせボタンの作成
    createInfoButton();
    
    //ユーザー登録チェック
    checkUserData();
}


void TitleScene::cutLeftAndRightVisibleSpace()
{
    int space_width = getVisibleSpaceWidth();
    
    //左右の余白がある場合、余白部分を黒くする
    if(space_width > 0){
        //左の柄
        Sprite *left_sprite = Sprite::create("init/Home/Common/side.png");
        left_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        left_sprite->setAnchorPoint(Point(1,0));
        left_sprite->setScale(ratio);
        left_sprite->setPosition(convertFromDisplaySize(Point(0, 0), 0.5, 0));
        this->addChild(left_sprite);
        
        //右の柄
        Sprite *right_sprite = Sprite::create("init/Home/Common/side.png");
        right_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        right_sprite->setAnchorPoint(Point(0,0));
        right_sprite->setScale(ratio);
        right_sprite->setPosition(convertFromDisplaySize(Point(640, 0), 0.5, 0));
        this->addChild(right_sprite);
        
    }
}