
#include "ContinueStoneShop.h"
#include "Cocos2dxFox.h"
#include "ContinueLayer.h"
#include "ContinueStoreScroll.h"
#include "LoadingManager.h"

bool ContinueStoneShop::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("StoneShopMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    _cell_size = 130 * ratio;
    
    return true;
}

void ContinueStoneShop::onEnter()
{
    Layer::onEnter();
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    
    //イベントリスナーの作成
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    shadow->setOpacity(0);
    this->addChild(shadow, 1);
    shadow->runAction(FadeTo::create(0.15f, 255));

    
    //タイトルバーの場所を
    float header_pos_y = 870/*+ getVisibleSpaceHeight()/ratio*/;
    //CCLOG("ヘッダーY：%f,%f,%f",header_pos_y,getVisibleSpaceHeight(),ratio);
    
    //ページタイトルバーの作成
    auto title_bar = Sprite::create("init/Header/title_base.png");
    title_bar->setScale(ratio);
    title_bar->setPosition(convertFromDisplaySize(Point(236, header_pos_y), 0.5, 0.5));
    addChild(title_bar, 12);
    
    //戻るボタン
    auto back_off = Sprite::create("init/Header/bt_return.png");
    auto back_on = Sprite::create("init/Header/bt_return.png");
    back_on->setColor(Color3B(180, 180, 180));
    
    auto back_btn = MenuItemSprite::create(back_off, back_on, [&, dung](Ref* sender){
        //ページ遷移
        auto continue_layer = ContinueLayer::create();//コンティニューレイヤー再び作成
        dung->addChild(continue_layer,100);
        continue_layer->showDetail();
        
        this->removeFromParent();//このレイヤー削除
        
    });
    back_btn->setScale(ratio);
    auto Menu = Menu::create(back_btn, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(58, header_pos_y), 0.5, 0.5));
    addChild(Menu, 12);
    
    //ページ名
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("宝玉購入メニュー", FONT_NAME_SYSTEM, 32);
    title_label->setScale(ratio);
    title_label->setAnchorPoint(Point(0, 0.5));
    title_label->setPosition(convertFromDisplaySize(Point(111, header_pos_y), 0.5, 0.5));
    title_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    addChild(title_label, 13);
    
    
    //ヘルプボタン
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [&](Ref* sender){
        
        auto WebViewLoadLayer = WebViewLoadLayer::create(TOKUTEI_HELP_URL, "特定商取引法表記");
        addChild(WebViewLoadLayer,80000);
    });
    
    help_btn->setScale(ratio);
    help_btn->setAnchorPoint(Point(1, 0.5));
    help_btn->setPosition(convertFromDisplaySize(Point(615,header_pos_y), 0.5, 0.5));
    auto menu = Menu::create(help_btn, nullptr);
    menu->setPosition(Point(0,0));
    this->addChild(menu, 5);
    
    
    //購入確認通信
    payCheckRequest();
}

void ContinueStoneShop::setDelegate(string item_name, string price, string itemId, int userId)
{
    //購入する宝玉数を置換して取得
    string find_str  = "宝玉 ";	// 検索対象の文字列
    string find_str2 = "個"; // 検索対象の文字列
    item_name.replace(item_name.find(find_str), find_str.length(), "");
    item_name.replace(item_name.find(find_str2), find_str2.length(), "");
    _purchase = fromString<int>(item_name);
    HMSLOG("purchase = %d", _purchase);
    
    HMSLOG("price : %s", price.c_str());
    _price = 0;
    if(price!="") _price = stoi(price);
    
    InAppBillingManager::setDelegate(this) ;
    InAppBillingManager::purchaseForItemId(itemId, userId) ;
}

void ContinueStoneShop::onSuccess(string itemId)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    //宝玉数更新処理
    int new_jewel = fromString<int>(SaveData::loadString("jewel")) + _purchase;
    SaveData::saveString("jewel", toString<int>(new_jewel));
    HMSLOG("new_jewel = %d", new_jewel);
    
    //今月の課金額に購入額を追加
    if(_price > 0) SaveData::saveInt("uvec_pkga_sum_coin", SaveData::loadInt("uvec_pkga_sum_coin") + _price);
    HMSLOG("uvec_pkga_sum_coin : %d", SaveData::loadInt("uvec_pkga_sum_coin"));
    
    //FOX LTV計測地点（宝玉購入）
    char fox_price[5];
    sprintf(fox_price, "%d", _price);
    //商品名
    string fox_purchase_name = "宝玉 " + toString<int>(_purchase)+"個" ;
    
    if (RAKUTEN_FLG){
        //楽天の場合計測無し
        
    } else if (IF_IOS) {
        FoxPlugin::sendEvent("課金iOS", "", "", "", "", fox_purchase_name.c_str(), _price, 1, "JPY"); //数量は必ず1つ 例を挙げると「宝玉 185個」を1つ購入するという意味合い
        FoxPlugin::addParameter(CC_LTV_PARAM_PRICE, fox_price);
        FoxPlugin::sendLtv(6424);
    } else if (IF_ANDROID) {
        FoxPlugin::sendEvent("課金Android", "", "", "", "", fox_purchase_name.c_str(), _price, 1, "JPY"); //数量は必ず1つ 例を挙げると「宝玉 185個」を1つ購入するという意味合い
        FoxPlugin::addParameter(CC_LTV_PARAM_PRICE, fox_price);
        FoxPlugin::sendLtv(6426);
    }
    
    //購入確認後のポップアップ表示
    string message = "宝玉を" + toString<int>(_purchase) + "個購入しました。";
    auto func = [](){};
    auto popup_layer = PopupOkButtonLayer::create(func, message);
    addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    
  
    
    auto continue_layer = ContinueLayer::create();//コンティニューレイヤー再び作成
    dung->addChild(continue_layer,100);
    continue_layer->showDetail();
    setVisible(false);
    this->removeFromParent();//このレイヤー削除
}

void ContinueStoneShop::onCancel(){
    
    //購入を中止したポップアップを表示
    HMSLOG("ContinueStoneShop#onCancel");
    auto yesFunc = [](){
        LoadingManager::removeLoading();
        
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
    };
    
    string message = "購入を中止しました。";
    
    auto popup_layer = PopupOk::create() ;
    popup_layer->setPopup(message, yesFunc);
    addChild(popup_layer, 10000001);
}

void ContinueStoneShop::payCheckRequest()
{
    //課金可能確認
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "user/pay_check.php";
    auto selector = httpresponse_selector(ContinueStoneShop::payCheckCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
    
}

void ContinueStoneShop::payCheckCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if (result != "not connect" && result != "response NG"){
        
        string err;
        Json json = Json::parse(result, err);
        
        bool payment_flg = json["check_flg"].bool_value();
        
        if (payment_flg) {
            
            //課金可能確認
            string user_master_id = SaveData::loadString("user_master_id");
            
            string url = HOST;
            url += "user/get_shop_item_list.php";
            auto selector = httpresponse_selector(ContinueStoneShop::getShopItemListCallback);
            string body = "user_master_id="+user_master_id;
            HttpConnect::sendHttpRequest(this, url, selector, body);

        } else {
            auto popup = PopupOk::create();
            popup->setPopup("現在宝玉を購入することはできません。", [this,dung](){
                auto continue_layer = ContinueLayer::create();//コンティニューレイヤー再び作成
                dung->addChild(continue_layer,100);
                continue_layer->showDetail();
                setVisible(false);
                this->removeFromParent();//このレイヤー削除
            });
            addChild(popup, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
    }
}


void ContinueStoneShop::getShopItemListCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    _buyable_shop_item_list = json["buyable_shop_item_list"].object_items() ;
    
    //スクロールビューの作成
    auto pScrollView = ContinueStoreScroll::create();
    this->addChild(pScrollView, ORDER_SCROLL_VIEW, SCROLLVIEW_TAG);
    pScrollView->setJson(_buyable_shop_item_list);
    
}
