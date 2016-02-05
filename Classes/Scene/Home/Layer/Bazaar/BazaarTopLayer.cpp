#include "BazaarTopLayer.h"

bool BazaarTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarTopLayer::init()");
    
    ratio = getVisibleRatio();
    new_flg = 0;
    exchange_day_limit_flg = "0";
    can_exchange_count = "";
    can_exhibit_count = "";
    
    return true;
}

void BazaarTopLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("バザー", "CharaMenu", this);
   
    //画面作成
//    createButton();
    
    //出品情報取得
    getExhibitData();
}

//最新の出品一覧を取得
void BazaarTopLayer::getExhibitData()
{
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "bazaar/get_bazaar_exchange_list.php";
    auto selector = httpresponse_selector(BazaarTopLayer::getExhibitDataCallBack);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
    
//    string user_master_id = SaveData::loadString("user_master_id");
//    string url = HOST;
//    url += "bazaar/get_bazaar_search_list.php";
//    auto selector = httpresponse_selector(BazaarTopLayer::getExhibitDataCallBack);
//    string body = "user_master_id="+user_master_id;
//    HttpConnect::sendHttpRequest(this, url, selector, body);
}

//最新の出品一覧を取得完了
void BazaarTopLayer::getExhibitDataCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if (result != "not connect" && result != "response NG"){
        
        string err;
        Json json = Json::parse(result, err);
		
		//出品データを保存
        SaveData::saveString("bazaar_my_list", result);
        SaveData::saveString("ban_exhibit_chara_list", json["ban_exhibit_chara_list"].dump());
        
        new_flg = json["new_flg"].int_value();
        exchange_day_limit_flg = json["exchange_day_limit_flg"].string_value();
        can_exchange_count = json["exchange_day_limit_count"].string_value();
        can_exhibit_count = toString(json["can_exhibit_chara_count"].int_value());
        
        //背景を作成
        createBG();
        
        //ボタンを作成
        createButton();
    }
}

//背景の作成
void BazaarTopLayer::createBG()
{
    auto exhibit_bg = Sprite::create("init/Home/Bazaar/bk_trading.png");
    exhibit_bg->setScale(ratio);
    exhibit_bg->setPosition(convertFromDisplaySize(Point(320, 618), 0.5, 1));
    this->addChild(exhibit_bg, Z_BG, T_BG);
    
    auto search_bg = Sprite::create("init/Home/Bazaar/bk_search.png");
    search_bg->setScale(ratio);
    search_bg->setPosition(convertFromDisplaySize(Point(320, 400), 0.5, 1));
    this->addChild(search_bg, Z_BG, T_BG);
}

//ボタンの生成
void BazaarTopLayer::createButton()
{
    //出品
    auto exhibit_btn_on = Sprite::create("init/Home/Bazaar/button/bt_chara_exhibit.png");
    auto exhibit_btn_off = Sprite::create("init/Home/Bazaar/button/bt_chara_exhibit.png");
    exhibit_btn_on->setColor(Color3B(180, 180, 180));
    auto exhibit_btn = MenuItemSprite::create(exhibit_btn_off, exhibit_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("BazaarExhibitCharaList");
    });
    exhibit_btn->setScale(ratio);
    auto exhibit_menu = Menu::create(exhibit_btn, nullptr);
    exhibit_menu->setPosition(convertFromDisplaySize(Point(284, 604), 0.5, 1));
    this->addChild(exhibit_menu, Z_Button);
    
    //出品可能数をテキストで出す
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("あと　体出品可能", FONT_NAME_SYSTEM, 26);
    label->setScale(ratio);
    label->setAnchorPoint(Point(0.5, 0.5));
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    label->setPosition(convertFromDisplaySize(Point(387, 548), 0.5, 1));
    this->addChild(label, Z_Text);
    
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(can_exhibit_count, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(can_exhibit_count, 26, 38));
    label2->setColor(Color3B(234, 245, 67));
    label2->setScale(ratio);
    label2->setAnchorPoint(Point(0.5, 0.5));
	label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    label2->setPosition(convertFromDisplaySize(Point(347, 548), 0.5, 1));
    this->addChild(label2, Z_Text2);
    
    //出品状況
    auto my_exhibit_btn_on = Sprite::create("init/Home/Bazaar/button/bt_situation.png");
    auto my_exhibit_btn_off = Sprite::create("init/Home/Bazaar/button/bt_situation.png");
    my_exhibit_btn_on->setColor(Color3B(180, 180, 180));
    auto my_exhibit_btn = MenuItemSprite::create(my_exhibit_btn_off, my_exhibit_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("BazaarExhibitTop");
    });
    my_exhibit_btn->setScale(ratio);
    auto my_exhibit_menu = Menu::create(my_exhibit_btn, nullptr);
    my_exhibit_menu->setPosition(convertFromDisplaySize(Point(490, 604), 0.5, 1));
    this->addChild(my_exhibit_menu, Z_Button);
    
    //出品物で取引成立したものがあったら、ボタンに数字重ねる
    if (new_flg > 0){
        auto bg_red = Sprite::create("init/Home/Mypage/bt_mypage_red_circle.png");
        bg_red->setScale(ratio);
        bg_red->setPosition(convertFromDisplaySize(Point(573, 636), 0.5, 1));
        this->addChild(bg_red, Z_Button);
        
        if (new_flg > 99) {
            new_flg = 99;
        }
        auto num_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(new_flg), FONT_NAME_SYSTEM, 24);
        num_label->setPosition(Point(bg_red->getContentSize() / 2));
        bg_red->addChild(num_label, ZORDER_BUTTON);
    }
    
    //検索
    auto search_btn_on = Sprite::create("init/Home/Bazaar/button/bt_search.png");
    auto search_btn_off = Sprite::create("init/Home/Bazaar/button/bt_search.png");
    search_btn_on->setColor(Color3B(180, 180, 180));
    auto search_btn = MenuItemSprite::create(search_btn_off, search_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("BazaarSearchLayer");
    });
    search_btn->setScale(ratio);
    auto search_menu = Menu::create(search_btn, nullptr);
    search_menu->setPosition(convertFromDisplaySize(Point(386, 384), 0.5, 1));
    this->addChild(search_menu, Z_Button);
    
    //検索可能数をテキストで出す
    if (exchange_day_limit_flg == "1"){
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument("あと　回交換可能", FONT_NAME_SYSTEM, 26);
        label->setScale(ratio);
        label->setAnchorPoint(Point(0.5, 0.5));
        label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        label->setPosition(convertFromDisplaySize(Point(387, 328), 0.5, 1));
        this->addChild(label, Z_Text);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(can_exchange_count, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(can_exchange_count, 26, 38));
        label2->setColor(Color3B(234, 245, 67));
        label2->setScale(ratio);
        label2->setAnchorPoint(Point(0.5, 0.5));
        label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        label2->setPosition(convertFromDisplaySize(Point(347, 328), 0.5, 1));
        this->addChild(label2, Z_Text2);
    }
    
    //ヘルプ
    auto help_btn_on = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent();
        
        string url = HOST;
        url += "user/index.php?&p=page__chara_bazaar_help&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "バザーヘルプ");
        normal_scene->addChild(WebViewLoadLayer, 9);
        
    });
    help_btn->setScale(ratio);
    auto help_menu = Menu::create(help_btn, nullptr);
    help_menu->setPosition(convertFromDisplaySize(Point(570, 789), 0.5, 1));
    this->addChild(help_menu, Z_Button);
}

bool BazaarTopLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void BazaarTopLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{

}
void BazaarTopLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarTopLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{

}
