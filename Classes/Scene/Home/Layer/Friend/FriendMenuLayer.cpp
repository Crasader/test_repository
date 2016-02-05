#include "FriendMenuLayer.h"

bool FriendMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("FriendMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void FriendMenuLayer::setupButton()
{
    
    Vector<MenuItem*> buttons;
    
	//宝玉購入ボタン
	Sprite* follow_btn_off = Sprite::create("init/Home/MenuButton/bt_follow.png");			//押されていない時
	Sprite* follow_btn_on = Sprite::create("init/Home/MenuButton/bt_follow.png");			//押された時
	follow_btn_on->setColor(Color3B(180,180,180));											//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* follow_btn = MenuItemSprite::create(follow_btn_off, follow_btn_on, CC_CALLBACK_1(FriendMenuLayer::changePage, this));
	follow_btn->setScale(ratio);
	follow_btn->setTag(FOLLOW_LIST_TAG);
    buttons.pushBack(follow_btn) ;
	
	//行動力回復ボタン
	Sprite* follower_btn_off = Sprite::create("init/Home/MenuButton/bt_follower.png");		//押されていない時
	Sprite* follower_btn_on = Sprite::create("init/Home/MenuButton/bt_follower.png");		//押された時
	follower_btn_on->setColor(Color3B(180,180,180));										//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* follower_btn = MenuItemSprite::create(follower_btn_off, follower_btn_on, CC_CALLBACK_1(FriendMenuLayer::changePage, this));
	follower_btn->setScale(ratio);
	follower_btn->setTag(FOLLOWER_LIST_TAG);
    buttons.pushBack(follower_btn) ;
	
	//キャラ最大所持数追加ボタン
	Sprite* search_btn_off = Sprite::create("init/Home/MenuButton/bt_search.png");			//押されていない時
	Sprite* search_btn_on = Sprite::create("init/Home/MenuButton/bt_search.png");			//押された時
	search_btn_on->setColor(Color3B(180,180,180));											//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* search_btn = MenuItemSprite::create(search_btn_off, search_btn_on, CC_CALLBACK_1(FriendMenuLayer::changePage, this));
	search_btn->setScale(ratio);
	search_btn->setTag(FRIEND_SEARCH_TAG);
    buttons.pushBack(search_btn) ;
	
#if (IF_ANDROID)
	//友達招待ボタン(楽天の.apkのみに含める処理)
	Sprite* introduction_btn_off = Sprite::create("init/Home/MenuButton/bt_invitation.png");//押されていない時
	Sprite* introduction_btn_on = Sprite::create("init/Home/MenuButton/bt_invitation.png");	//押された時
	introduction_btn_on->setColor(Color3B(180,180,180));									//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* introduction_btn = MenuItemSprite::create(introduction_btn_off, introduction_btn_on, CC_CALLBACK_1(FriendMenuLayer::changePage, this));
	introduction_btn->setScale(ratio);
	introduction_btn->setTag(FRIEND_INTRODUCTION_TAG);
    buttons.pushBack(introduction_btn) ;
#endif
	
    Menu* menu = Menu::createWithArray(buttons);
    if (IF_ANDROID) menu->setPosition(convertFromDisplaySize(Point(320, 430), 0.5, 1));
    else menu->setPosition(convertFromDisplaySize(Point(320, 500), 0.5, 1));
	menu->alignItemsVertically();
	menu->alignItemsVerticallyWithPadding(10.0f);
	menu->setAnchorPoint(Point(0.5,1));
	this->addChild(menu, 1);
    
}

void FriendMenuLayer::initTwitterLine()
{
    //ツイッター、ラインボタンの画像作成
    auto twitter = Sprite::create("init/Home/Friend/icon_twitter.png");
    twitter->setScale(ratio);
    twitter->setPosition(convertFromDisplaySize(Point(470, 620), 0.5, 1));
    addChild(twitter, 3);
    
    auto line = Sprite::create("init/Home/Friend/icon_line.png");
    line->setScale(ratio);
    line->setPosition(convertFromDisplaySize(Point(530, 620), 0.5, 1));
    addChild(line, 3);
}

void FriendMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //ページボタンの作成
    setupButton();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("フレンドメニュー", "Mypage", this);
    
    //通信してメンテ状態を確認する
    shopMenuRequest();
}

void FriendMenuLayer::changePage(Ref* sender){
    
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();
    
    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == FOLLOW_LIST_TAG)
    {
        normal_scene->changePage("FollowList");
    }
    else if (tag == FOLLOWER_LIST_TAG)
    {
        normal_scene->changePage("FollowerList");
    }
    else if (tag == FRIEND_SEARCH_TAG)
    {
        normal_scene->changePage("FriendSearch");
    }
#if (IF_ANDROID)
    else if (tag == FRIEND_INTRODUCTION_TAG)
    {
        auto normal_scene = (NormalScene *)this->getParent();
        string url = FRIEND_INTRODUCTION_URL"&user_id="+user_master_id;//url+ユーザーID
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "フレンド招待");
        normal_scene->addChild(WebViewLoadLayer,9);
    }
#endif
}

void FriendMenuLayer::shopMenuRequest()
{
    //課金可能確認
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "user/pay_check.php";
    auto selector = httpresponse_selector(ShopMenuLayer::shopMenuCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void FriendMenuLayer::shopMenuCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    
}

Sprite* FriendMenuLayer::menuButtonCreateSprite(string img_name, int count){
	Sprite* base = Sprite::create("init/Home/MenuButton/bt_base.png");
	
	for(int i=count; i>0; i--) {
		Sprite* img = Sprite::create("init/Home/MenuButton/" + img_name + to_string(i) + ".png");
		img->setAnchorPoint(Point(0.5,0.5));//中心にアンカーを配置
		img->setPosition(Point(base->getContentSize() / 2));//ベース画像(親)の中心座標に配置
		base->addChild(img, MENU_BUTTON_Z, MENU_BUTTON_T);
	}
	
	return base;
}