#include "SearchRoomLayer.h"

bool SearchRoomLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("SearchRoomLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void SearchRoomLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("ルーム検索", "BattleRoomTop", this);
    
    //背景画像の作成
    createBg();
    
    //数字ボタンの作成
    createNumButton();
    
    //クリア、OKボタンの作成
    createButton();
    
    //ルーム番号作成通信処理
}

void SearchRoomLayer::createBg()
{
    //背景の作成
    _bg = Sprite::create("init/Home/Quest/Online/popup_room_search.png");
    _bg->setScale(ratio);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, 1);
    
    //表示番号
    _display_num = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 55);
    _display_num->setPosition(Point(_bg->getContentSize().width / 2, 386));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    _bg->addChild(_display_num, 2);
}

void SearchRoomLayer::createNumButton()
{
    //数字ボタンの作成
    for (int i = 0; i < BUTTON_NUM; i++) {
        auto num_off = Sprite::create("init/Home/Quest/Online/room_num"+toString<int>(i)+".png");
        auto num_on  = Sprite::create("init/Home/Quest/Online/room_num"+toString<int>(i)+".png");
        num_on->setColor(Color3B(180, 180, 180));
        
        _menu_item.insert(MenuItemSprite::create(num_off, num_on, [this](Ref *sender){
            MenuItem* menuItem = (MenuItem *)sender;
            int tag = menuItem->getTag();
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/tap.mp3",false);
            
            //数字更新処理
            if (_display_num->getStringLength() < MAX_INSERT_NUM) {
                string new_num = _display_num->getString() + toString<int>(tag);
                _display_num->setString(new_num);
            }
        }));
        _menu_item[i]->setTag(i);
    }
    
    auto menu1 = Menu::create(_menu_item[1], _menu_item[2], _menu_item[3], _menu_item[4], _menu_item[5], nullptr);
    menu1->setPosition(Point(_bg->getContentSize().width / 2, 274));
    menu1->alignItemsHorizontallyWithPadding(0);
    _bg->addChild(menu1, 1);
    
    auto menu2 = Menu::create(_menu_item[6], _menu_item[7], _menu_item[8], _menu_item[9], _menu_item[0], nullptr);
    menu2->setPosition(Point(_bg->getContentSize().width / 2, 174));
    menu2->alignItemsHorizontallyWithPadding(0);
    _bg->addChild(menu2, 1);
}

void SearchRoomLayer::createButton()
{
    //クリア、OKボタンの作成
    auto clear_btn_off = Sprite::create("init/Home/Quest/Online/bt_clear.png");
    auto clear_btn_on  = Sprite::create("init/Home/Quest/Online/bt_clear.png");
    clear_btn_on->setColor(Color3B(180, 180, 180));
    
    auto clear_btn = MenuItemSprite::create(clear_btn_off, clear_btn_on, [&](Ref* sender){
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
        //入力数字をリセット
        _display_num->setString("");
    });
    clear_btn->setAnchorPoint(Point(0.5, 0));
    
    auto ok_btn_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto ok_btn_on  = Sprite::create("init/Home/Common/bt_ok.png");
    ok_btn_on->setColor(Color3B(180, 180, 180));
    
    ok_button = MenuItemSprite::create(ok_btn_off, ok_btn_on, [&](Ref* sender){
        ok_button->setEnabled(false);
        //ルーム検索
        GpsManager::setDelegate(this);
        GpsManager::requestLocation();
    });
    ok_button->setAnchorPoint(Point(0.5, 0));
    
    auto menu = Menu::create(clear_btn, ok_button, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 14));
    menu->alignItemsHorizontallyWithPadding(30 * ratio);
    _bg->addChild(menu, 1);
}

void SearchRoomLayer::onSuccess(string gps_data)
{
    _gps_data = gps_data;
    HMSLOG("gps_data=%s", gps_data.c_str());
    
    searchRoomRequest();
}

void SearchRoomLayer::searchRoomRequest()
{
    string url = HOST;
    url += "battle/search_room.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&room_key=" + _display_num->getString();
    url += "&deck_no=" + SaveData::loadString("party_no");//デッキナンバー追加
    url += "&gps_data=" + _gps_data;
    string check_sum = HmsCrypto::makeCheckSum(_gps_data, "z38BSemzJ");
    url += "&cs=" + check_sum;
    
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        searchRoomCallback(sender, response);
    });
}

void SearchRoomLayer::searchRoomCallback(HttpClient *sender, HttpResponse *response)
{
    if(response->isSucceed()==false){//通信失敗したら
        CCLOG("通信失敗");
        runAction(Sequence::create(DelayTime::create(1.5f),CallFunc::create([this](){//少し時間をおき
            
            auto popup = PopupOk::create();
            popup->setPopup("タイムアウトしました。再送信します。",[this](){//ポップアップ出して再送信
                searchRoomRequest();//再送信
            });
            addChild(popup,10000001);
            
        }), NULL));
        
        return;
    }
    
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("result = %s", result.c_str());
    
    string err;
    json = Json::parse(result, err);
    
    if (json["status"].bool_value()) {

        //ダンジョンシーンのデータを設定
        int battle_list_id =  fromString<int>(json["battle_list_id"].string_value());
        string enemy_user_master_id = json["enemy_user_list"]["user_master_id"].string_value();
        Utility::setOnlineBattleData(battle_list_id, enemy_user_master_id);
        
        //オンラインバトルタイプ（初心の手引きのフラグで使用）
        SaveData::saveString("online_battle_type", "search_room");
        
        
        
        //ダンジョンシーンへ（変更予定）
        auto scene = DungeonScene::createScene();
        auto transition = TransitionFade::create(1.0f, scene);
        Director::getInstance()->replaceScene(transition);
        
    }else {
        ok_button->setEnabled(true);
        //対戦相手(ルーム)が見つからなかったときの処理
        auto popup_layer = popupNoChangePageLayer::create(json["comment"].string_value(),2.0f);
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}
