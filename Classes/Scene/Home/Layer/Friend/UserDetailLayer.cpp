#include "UserDetailLayer.h"

UserDetailLayer* UserDetailLayer::create(string page_name)
{
    UserDetailLayer *pRet = new UserDetailLayer();
    
    if (pRet && pRet->init(page_name))
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

bool UserDetailLayer::init(string page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("UserDetailLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの設定
    _page_name = page_name;
    
    return true;
}

void UserDetailLayer::getUserDataRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "friend/get_user_info.php";
    auto selector = httpresponse_selector(UserDetailLayer::UserDataCallback);
    string body = "user_master_id=" + user_master_id + "&friend_id=" + SaveData::loadString("search_friend_id");
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void UserDetailLayer::UserDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    _user_data = json["0"].object_items();
    
    //ページタイトルの作成
    createTitleBar();
    
    //ユーザ情報画像の作成
    createUserDetailSprite();
    
    //スプライトの表示
    showSprite();
    
    //ボタンの作成
    createButton();
}

void UserDetailLayer::createTitleBar()
{
    auto title_bar_layer = (Layer *)this->getParent()->getChildByTag(TITLE_LAYER_TAG);
    
    //ページタイトル
    string page_title = "ユーザー情報";
    if (!_user_data["follow_flg"].bool_value()) page_title = "フォロー申請";
    
    string return_page_name = "FollowList";
    if (_page_name == "UserDetailFromFollowerList") return_page_name = "FollowerList";
    else if (_page_name == "UserDetailFromFriendSearch") return_page_name = "FriendSearch";
    
    //タイトルバーの作成
    auto title_bar = Sprite::create("init/Header/title_base.png");
    title_bar->setScale(ratio);
    title_bar->setPosition(convertFromDisplaySize(Point(236, 789), 0.5, 1));
    title_bar_layer->addChild(title_bar, TitleBarLayer::Z_Title_Bar, TitleBarLayer::T_Title_Bar);
    
    //戻るボタン
    auto back_off = Sprite::create("init/Header/bt_return.png");
    auto back_on = Sprite::create("init/Header/bt_return.png");
    back_on->setColor(Color3B(180, 180, 180));
    
    auto back_btn = MenuItemSprite::create(back_off, back_on, [this, return_page_name](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage(return_page_name);
    });
    back_btn->setScale(ratio);
    auto Menu = Menu::create(back_btn, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(58, 788), 0.5, 1));
    title_bar_layer->addChild(Menu, ZORDER_BACK_BUTTON, BACK_BUTTON_TAG);
    
    //ページ名
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("ユーザー情報", FONT_NAME_SYSTEM, 32);
    title_label->setScale(ratio);
    title_label->setAnchorPoint(Point(0, 0.5));
    title_label->setPosition(convertFromDisplaySize(Point(111, 788), 0.5, 1));
    title_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    title_bar_layer->addChild(title_label, TitleBarLayer::Z_Title_Label, TitleBarLayer::T_Title_Label);
}

void UserDetailLayer::createUserDetailSprite()
{
    //ユーザー情報画面の作成
    userDetail = UserDetailSprite::create();
    userDetail->setupUserData(_user_data);
    userDetail->showUserData();
    userDetail->setScale(ratio);
}

void UserDetailLayer::showSprite()
{
    //ユーザー情報の表示
    userDetail->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(userDetail, 2);
}

void UserDetailLayer::createButton()
{
    if (_user_data["follow_flg"].bool_value())
    {
        //既にフォローしていた場合解除のボタン表示
        auto remove_off = Sprite::create("init/Home/Common/bt2.png");
        auto remove_on  = Sprite::create("init/Home/Common/bt2.png");
        remove_on->setColor(Color3B(180, 180, 180));
        
        auto text_off = HMSFontUtil::createWithTTFBySystemFontArgument("フォロー解除", FONT_NAME_SYSTEM, 30);
        text_off->setPosition(Point(remove_off->getContentSize() / 2));
        remove_off->addChild(text_off, 1);
        
        auto text_on = HMSFontUtil::createWithTTFBySystemFontArgument("フォロー解除", FONT_NAME_SYSTEM, 30);
        text_on->setPosition(Point(remove_on->getContentSize() / 2));
        remove_on->addChild(text_on, 1);
        
        auto remove_btn = MenuItemSprite::create(remove_off, remove_on, [&](Ref* sender)
        {
            //解除の確認レイヤーを表示
            auto ok_func = [this](){
                //フォロー解除リクエスト
                removeFollowRequest();
            };
            
            auto ng_func = [](){}; //何もしない
            
            auto normal_scene = (NormalScene *)this->getParent();
            
            string message = userDetail->getUserName() + "さんとの\nフォローを解除してもよろしいですか。";
            auto popup_layer = popupConfirmLayer::create(message);
            popup_layer->setFunction(ok_func, ng_func);
            normal_scene->addChild(popup_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);

        });
        remove_btn->setScale(ratio);
        
        auto menu = Menu::create(remove_btn, nullptr);
        menu->alignItemsHorizontallyWithPadding(30);
        menu->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
        this->addChild(menu, 1);
    }
    else
    {
        //フォローしていない場合申請ボタンの表示
        auto follow_off = Sprite::create("init/Home/Common/bt2.png");
        auto follow_on  = Sprite::create("init/Home/Common/bt2.png");
        follow_on->setColor(Color3B(180, 180, 180));
        
        auto text_off = HMSFontUtil::createWithTTFBySystemFontArgument("フォロー申請", FONT_NAME_SYSTEM, 30);
        text_off->setPosition(Point(follow_off->getContentSize() / 2));
        follow_off->addChild(text_off, 1);
        
        auto text_on = HMSFontUtil::createWithTTFBySystemFontArgument("フォロー申請", FONT_NAME_SYSTEM, 30);
        text_on->setPosition(Point(follow_on->getContentSize() / 2));
        follow_on->addChild(text_on, 1);
        
        auto follow_btn = MenuItemSprite::create(follow_off, follow_on, [&](Ref* sender)
        {
            //フォロー申請通信処理
            demandFollowRequest();
        });
        follow_btn->setScale(ratio);
        
        auto menu = Menu::create(follow_btn, nullptr);
//        auto menu = Menu::create(follow_btn, cancel_btn, nullptr);
        menu->alignItemsHorizontallyWithPadding(30);
        menu->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
        this->addChild(menu, 1);
    }
}

void UserDetailLayer::demandFollowRequest()
{
    string user_master_id    = SaveData::loadString("user_master_id");
    string to_user_master_id = _user_data["user_master_id"].string_value();
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "friend/follow_demand.php";
    auto selector = httpresponse_selector(UserDetailLayer::demandFollowCallback);
    string body = "user_master_id=" + user_master_id + "&to_user_master_id=" + to_user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void UserDetailLayer::demandFollowCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["success_flg"].bool_value())
    {
        //初心の手引きクリアーフラグ2 フレンドをフォローする(フラグが立つ処理は他の場所にもあるので注意)
        if (SaveData::loadBool("beginner_guide_clear_flg_2") == false) {
            SaveData::saveBool("beginner_guide_clear_flg_2", true);
            SaveData::saveBool("beginner_layer_open_flg", true);
        }
        
        //申請成功時
        string message = userDetail->getUserName() + "さんをフォローしました。\nフォロー一覧に移動します。";
        auto popup_layer = popupChangePageLayer::create(message, "FollowList");
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
    else
    {
        //申請失敗時
        auto popup_layer = popupNoChangePageLayer::create(json["comment"].string_value());
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}

void UserDetailLayer::removeFollowRequest()
{
    //フォロー解除通信処理
    string user_master_id    = SaveData::loadString("user_master_id");
    string to_user_master_id = SaveData::loadString("to_user_master_id");
    
    string url = HOST;
    url += "friend/follow_remove.php";
    auto selector = httpresponse_selector(UserDetailLayer::removeFollowCallback);
    string body = "user_master_id=" + user_master_id + "&to_user_master_id=" + to_user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void UserDetailLayer::removeFollowCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    
    auto normal_scene = (NormalScene *)this->getParent();
    if (json["successflg"].bool_value())
    {
        //解除成功
        string message = "フォローを解除しました。\nフレンドメニューに戻ります。";
        auto popup_layer = popupChangePageLayer::create(message, "FriendMenu");
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
    else
    {
        //解除失敗時
        string message = "フォローの解除に失敗しました。\n時間を置いてから再度試してください。";
        auto popup_layer = popupNoChangePageLayer::create(message);
        normal_scene->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}

void UserDetailLayer::onEnter()
{
    Layer::onEnter();
    
    //ユーザ情報取得通信
    getUserDataRequest();
}
