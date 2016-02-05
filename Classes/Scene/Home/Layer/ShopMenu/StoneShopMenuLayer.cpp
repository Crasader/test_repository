#include "StoneShopMenuLayer.h"
#include "Cocos2dxFox.h"

bool StoneShopMenuLayer::init()
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

void StoneShopMenuLayer::createScrollView()
{
    this->removeChildByTag(SCROLLVIEW_TAG) ;
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, ORDER_SCROLLVIEW, SCROLLVIEW_TAG);
    
    //PresentMenuのレイヤーを戻り値に指定して使う
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _display_item_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _display_item_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* StoneShopMenuLayer::createList()
{
    HmsVector<string> shopItemNameArray;
    HmsVector<string> shopItemPriceArray;
    HmsVector<string> shopItemNameIdArray;
    
    initShopItemArrayByOsType(shopItemNameArray, shopItemPriceArray, shopItemNameIdArray);
    
    //初期セルサイズとスクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    
    //プレゼント画像の配置
    int num = 0;
    for (int i = shopItemNameArray.size()-1; 0 <= i; i--)
    {
        auto jewel_sp = JewelSprite::create(shopItemNameArray[num], shopItemPriceArray[num], shopItemNameIdArray[num]);
        jewel_sp->setScale(ratio);
        jewel_sp->setTag(num);
        jewel_sp->setPosition(Point(320 * ratio, (_cell_size / 2) + (_cell_size * i)));
        layer->addChild(jewel_sp, 1);
        num++;
    }
	
    return layer;
}

void StoneShopMenuLayer::setDelegate(string item_name, string price, string itemId, int userId)
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

    //タップ禁止レイヤー生成
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer() ;
    
    InAppBillingManager::setDelegate(this) ;
    InAppBillingManager::purchaseForItemId(itemId, userId) ;
}

void StoneShopMenuLayer::onSuccess(string itemId)
{
    //タップ禁止レイヤー削除
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->removeTapDisableLayer();
    HMSLOG("StoneShopMenuLayer#onSuccess");
    
    //ヘッダー情報更新処理
    auto header_layer = (HeaderLayer *)this->getParent()->getChildByTag(HEADER_LAYER_TAG);
    
    //宝玉数更新処理
    auto jewel_label = (Label *)header_layer->getChildByTag(JEWEL_LABEL_TAG);
    int new_jewel = fromString<int>(SaveData::loadString("jewel")) + _purchase;
    jewel_label->setString(toString<int>(new_jewel));
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
    normal_scene->addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    
    //購入アイテムの表示フロー開始 リスト再生成
    startListDisplayFlow();
}

void StoneShopMenuLayer::onCancel(){
    //タップ禁止レイヤー削除
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->removeTapDisableLayer() ;
    
    //購入を中止したポップアップを表示
    HMSLOG("StoneShopMenuLayer#onCancel");
    auto yesFunc = [](){
		//SE
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
	};
    auto noFunc = [](){} ;
    string message = "購入を中止しました。";
    
    auto popup_layer = StoneShopBuyConfirmLayer::create(StoneShopBuyConfirmLayer::ButtonType::OK, message, yesFunc, noFunc) ;
    normal_scene->addChild(popup_layer, ORDER_RECEIVE_LAYER);
}

void StoneShopMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    
}

void StoneShopMenuLayer::payCheckRequest()
{
    //課金可能確認
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "user/pay_check.php";
    auto selector = httpresponse_selector(StoneShopMenuLayer::payCheckCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
    
}

void StoneShopMenuLayer::payCheckCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if (result != "not connect" && result != "response NG"){
        
        string err;
        Json json = Json::parse(result, err);
        
        bool payment_flg = json["check_flg"].bool_value();
        
        if (payment_flg) {
            getShopItemListRequest() ;
        } else {
            auto popup = popupChangePageLayer::create("現在宝玉を購入することはできません。\nショップメニューに戻ります。", "ShopMenu");
            addChild(popup, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
        }
        
    }
    
}

void StoneShopMenuLayer::getShopItemListRequest()
{
    //課金可能確認
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "user/get_shop_item_list.php";
    auto selector = httpresponse_selector(StoneShopMenuLayer::getShopItemListCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
    
}

void StoneShopMenuLayer::getShopItemListCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    _buyable_shop_item_list = json["buyable_shop_item_list"].object_items() ;
    
    //スクロールビューの作成
    createScrollView();
}

void StoneShopMenuLayer::setupImage()
{
	//ヘルプボタン
	auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
	auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
	help_btn_on->setColor(Color3B(180, 180, 180));
	
	auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [&](Ref* sender){
		auto normal_scene = (NormalScene *)this->getParent();
		string url = TOKUTEI_HELP_URL;
		auto WebViewLoadLayer = WebViewLoadLayer::create(url, "特定商取引法表記");
		normal_scene->addChild(WebViewLoadLayer,9);
		//SaveData::saveInt("ageBuyStatus", 0);//制限リセット※デバッグ用
	});
	
	help_btn->setScale(ratio);
	help_btn->setAnchorPoint(Point(1, 0.5));
	help_btn->setPosition(convertFromDisplaySize(Point(615,788), 0.5, 1));
	auto menu = Menu::create(help_btn, nullptr);
	menu->setPosition(Point(0,0));
	this->addChild(menu, 5);
}

//void StoneShopMenuLayer::createEventListener()
//{
//	//タップイベントの設定
//	listener = EventListenerTouchOneByOne::create();
//	listener->setSwallowTouches(true);
//	listener->onTouchBegan = [this](Touch* touch, Event* event){
//		return true ;
//	};
//
//	//タップイベントの登録
//	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
//}

void StoneShopMenuLayer::initShopItemArrayByOsType(HmsVector<string> &shopItemNameArray, HmsVector<string> &shopItemPriceArray, HmsVector<string> &shopItemNameIdArray)
{
    int count = countJsonData(_buyable_shop_item_list) ;
	for (int i=0; i < count ; i++) {
		string name = _buyable_shop_item_list[to_string(i)]["name"].string_value();
		string price = _buyable_shop_item_list[to_string(i)]["price"].string_value();
		string product_id = _buyable_shop_item_list[to_string(i)]["product_id"].string_value();
        
        shopItemNameArray.push_back(name) ;
        shopItemPriceArray.push_back(price);
        shopItemNameIdArray.push_back(product_id);
    }
    
    _display_item_count = shopItemNameIdArray.size() ;
}

void StoneShopMenuLayer::onEnter()
{
    Layer::onEnter();
    
	//createEventListener();
	
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("宝玉購入メニュー", "ShopMenu", this);
	
	//ヘルプボタン
	setupImage();
    
    //購入アイテムの表示フロー開始
    startListDisplayFlow() ;
}

void StoneShopMenuLayer::startListDisplayFlow()
{
    //購入確認通信
    payCheckRequest();
}

