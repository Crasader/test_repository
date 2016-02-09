#include "ShopMenuLayer.h"

bool ShopMenuLayer::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	HMSLOG("ShopMenuLayer::init()");
	
	ratio = getVisibleRatio();
	
	return true;
}

void ShopMenuLayer::onEnter()
{
	Layer::onEnter();
	
	//ページボタンの作成
	setupButton();
    
    //通信してメンテ状態を確認する
    shopMenuRequest();
	
	//ページタイトルの作成
	HeaderLayer::setupTitleBar("ショップメニュー", "Mypage", this);
}

void ShopMenuLayer::setupButton(){
	//宝玉購入ボタン
//	Sprite* crystal_btn_off = menuButtonCreateSprite("bt_buy", 5);		//押されていない時
//	Sprite* crystal_btn_on = menuButtonCreateSprite("bt_buy", 5);		//押された時
//	crystal_btn_on->setCascadeColorEnabled(true);						//親の色彩を子に受け継がせる
	Sprite* crystal_btn_off = Sprite::create("init/Home/MenuButton/bt_buy.png");		//押されていない時
	Sprite* crystal_btn_on = Sprite::create("init/Home/MenuButton/bt_buy.png");			//押された時
	crystal_btn_on->setColor(Color3B(180,180,180));										//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* crystal_btn = MenuItemSprite::create(crystal_btn_off, crystal_btn_on, CC_CALLBACK_1(ShopMenuLayer::jewelPurchaseConfirm, this));
	crystal_btn->setScale(ratio);
	
	//行動力回復ボタン
//	Sprite* recovery_btn_off = menuButtonCreateSprite("bt_recovery", 4);//押されていない時
//	Sprite* recovery_btn_on = menuButtonCreateSprite("bt_recovery", 4);	//押された時
//	recovery_btn_on->setCascadeColorEnabled(true);						//親の色彩を子に受け継がせる
	Sprite* recovery_btn_off = Sprite::create("init/Home/MenuButton/bt_recovery.png");	//押されていない時
	Sprite* recovery_btn_on = Sprite::create("init/Home/MenuButton/bt_recovery.png");	//押された時
	recovery_btn_on->setColor(Color3B(180,180,180));									//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* recovery_btn = MenuItemSprite::create(recovery_btn_off, recovery_btn_on, CC_CALLBACK_1(ShopMenuLayer::powerRestoration, this));
	recovery_btn->setScale(ratio);
	
	//キャラ最大所持数追加ボタン
//	Sprite* chara_box_up_btn_off = menuButtonCreateSprite("bt_up", 7);	//押されていない時
//	Sprite* chara_box_up_btn_on = menuButtonCreateSprite("bt_up", 7);	//押された時
//	chara_box_up_btn_on->setCascadeColorEnabled(true);					//親の色彩を子に受け継がせる
	Sprite* chara_box_up_btn_off = Sprite::create("init/Home/MenuButton/bt_up.png");	//押されていない時
	Sprite* chara_box_up_btn_on = Sprite::create("init/Home/MenuButton/bt_up.png");		//押された時
	chara_box_up_btn_on->setColor(Color3B(180,180,180));								//色彩変更(※ボタンが押された時なので薄暗い色に変更)
	MenuItemSprite* chara_box_up_btn = MenuItemSprite::create(chara_box_up_btn_off, chara_box_up_btn_on, CC_CALLBACK_1(ShopMenuLayer::charaBoxUpGrow, this));
	chara_box_up_btn->setScale(ratio);
	
	auto menu = Menu::create(crystal_btn, recovery_btn, chara_box_up_btn, nullptr);
	menu->alignItemsVertically();
	menu->alignItemsVerticallyWithPadding(10.0f);
	menu->setAnchorPoint(Point(0.5,1));
	menu->setPosition(convertFromDisplaySize(Point(320, 500), 0.5, 1));
	this->addChild(menu, Z_button);
}
//Shop => MyHome 遷移関数
void ShopMenuLayer::changePage(Ref* sender){
	
	auto normal_scene = (NormalScene *)this->getParent();
	normal_scene->changePage("Mypage");
	HMSLOG("touch back!");
}
//キャラ最大所持数追加コールバック関数
void ShopMenuLayer::charaBoxUpGrow(Ref *sender)
{
	HMSLOG("touch charaBox");
	auto normal_scene = (NormalScene *)this->getParent();
	normal_scene->modalLayerCreate(MODAL_CONTENTS_CHARA);
}
//バトルPT回復コールバック関数
void ShopMenuLayer::powerRestoration(Ref *sender)
{
	HMSLOG("touch powerRestoretion");
	auto normal_scene = (NormalScene *)this->getParent();
	normal_scene->modalLayerCreate(MODAL_CONTENTS_POWER);
}
//宝玉購入コールバック関数
void ShopMenuLayer::jewelPurchaseConfirm(Ref *sender)
{
/*-----------------------初期1回判定の場合の処理------------------------*/
//	//年齢別制限の確認
//	auto normal_scene = (NormalScene *)this->getParent();
//	int use_age_buy_status = SaveData::loadInt("ageBuyStatus");			//ユーザーが選んだ年齢制限
//	int uvec_pkga_sum_coin = SaveData::loadInt("uvec_pkga_sum_coin");	//今月の課金額
//	string message = "お客様の今月ご購入された金額は\n資金決済法に基づく支払い可能\n金額の上限に達しているため\nご購入は出来ません";
//	
//	switch (use_age_buy_status) {
//		case 1: //16歳未満:5000円まで
//			if(uvec_pkga_sum_coin > AGE_BUY_UNDER16) { //制限額を超えていた場合
//				auto popup_layer = StoneShopAge2ndConfirmLayer::create();
//				popup_layer->setupImage(message);
//				this->addChild(popup_layer, ORDER_RECEIVE_LAYER);
//			} else {
//				normal_scene->changePage("StoneShopMenu");
//			}
//			HMSLOG("AGE_BUY_UNDER16 touch");
//			break;
//		case 2: //16以上20歳未満:10000円まで
//			if(uvec_pkga_sum_coin > AGE_BUY_OVER16) { //制限額を超えていた場合
//				auto popup_layer = StoneShopAge2ndConfirmLayer::create();
//				popup_layer->setupImage(message);
//				this->addChild(popup_layer, ORDER_RECEIVE_LAYER);
//			} else {
//				normal_scene->changePage("StoneShopMenu");
//			}
//			HMSLOG("AGE_BUY_OVER16 touch");
//			break;
//		case 3: //20歳以上:制限無し
//			normal_scene->changePage("StoneShopMenu");
//			HMSLOG("AGE_BUY_OVER20 touch");
//			break;
//		default: //年齢制限未設定の場合、年齢別制限の確認窓表示
//			auto popup_layer = StoneShopAgeConfirmLayer::create();
//			normal_scene->addChild(popup_layer, ORDER_RECEIVE_LAYER);
//			break;
//	}
	//毎回年齢確認を表示させる
	auto normal_scene = (NormalScene *)this->getParent();
	auto popup_layer = StoneShopAgeConfirmLayer::create();
	normal_scene->addChild(popup_layer, ORDER_RECEIVE_LAYER);
}

void ShopMenuLayer::shopMenuRequest()
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

void ShopMenuLayer::shopMenuCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    
}

Sprite* ShopMenuLayer::menuButtonCreateSprite(string img_name, int count){
	Sprite* base = Sprite::create("init/Home/MenuButton/bt_base.png");
	
	for(int i=count; i>0; i--) {
		Sprite* img = Sprite::create("init/Home/MenuButton/" + img_name + to_string(i) + ".png");
		img->setAnchorPoint(Point(0.5,0.5));//中心にアンカーを配置
		img->setPosition(Point(base->getContentSize() / 2));//ベース画像(親)の中心座標に配置
		base->addChild(img, MENU_BUTTON_Z, MENU_BUTTON_T);
	}
	
	return base;
}
