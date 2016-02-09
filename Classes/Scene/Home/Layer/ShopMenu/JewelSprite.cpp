#include "JewelSprite.h"

JewelSprite* JewelSprite::create(string name, string price, string id){
    
    JewelSprite *sprite = new JewelSprite();
    
    
    if (sprite && sprite->init(name, price, id))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool JewelSprite::init(string name, string price, string id)
{
    string image_path = switchImageById(id) ;
    if ( !Sprite::initWithFile(image_path))
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //商品名と価格の取得
    _name  = name;
    _price = price;
    _id    = id;
    
    return true;
}


string JewelSprite::switchImageById(string product_id)
{
    //初回限定の購入商品のみ、デザインを豪華なやつにする
    if(product_id.find("once", 0 ) != string::npos ){
        return "init/Home/Shop/popup_shop2.png" ;
    }else{
        return "init/Home/Shop/popup_shop.png" ;
    }
}

void JewelSprite::createJewelImg()
{
    //宝玉画像の作成
    _jewel_sp = Sprite::create("init/Home/Shop/item.png");
    _jewel_sp->setScale(0.95f);
    _jewel_sp->setAnchorPoint(Point(0, 0.5));
    _jewel_sp->setPosition(Point(20, this->getContentSize().height /2));
    addChild(_jewel_sp, 1);
}

void JewelSprite::createButton()
{
    //購入ボタン
    auto btn_off = Sprite::create("init/Home/Shop/bt_buy.png");
    auto btn_on  = Sprite::create("init/Home/Shop/bt_buy.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto purchase_btn = MenuItemSprite::create(btn_off, btn_on, [this](Ref *obj)
    {
		int use_age_buy_status = SaveData::loadInt("ageBuyStatus"); //ユーザーが選んだ年齢制限
		HMSLOG("price = %s", _price.c_str());
		int new_pkga_sum_coin = SaveData::loadInt("uvec_pkga_sum_coin") + stoi(_price); //今月の課金額 + 新しい購入額
		bool popup_type_flg = false; //表示するレイヤーの判別用
		
		switch (use_age_buy_status) {
			case 1: //16歳未満:5000円まで
				if(new_pkga_sum_coin > AGE_BUY_UNDER16) { //制限額を超過している場合
					popup_type_flg = true;
				}
				break;
			case 2: //16以上20歳未満:10000円まで
				if(new_pkga_sum_coin > AGE_BUY_OVER16) { //制限額を超過している場合
					popup_type_flg = true;
				}
				break;
			case 3: //20歳以上:制限無し
				popup_type_flg = false;
				break;
		}
		
		if(popup_type_flg) {
			string message = "お客様のご購入金額が\n資金決済法に基づく支払い\n可能金額の上限に達しているため\nご購入は出来ません";
			auto popup_layer = StoneShopAge2ndConfirmLayer::create();
			popup_layer->setupImage(message);
			auto normal_scene = (NormalScene *)this->getParent()->getParent()->getParent()->getParent();
			normal_scene->addChild(popup_layer, ORDER_RECEIVE_LAYER);
		} else {
			auto yesFunc = [&]()
			{
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
				auto stone_shop_layer = (StoneShopMenuLayer *)this->getParent()->getParent()->getParent();
				stone_shop_layer->setDelegate(_name, _price, _id, fromString<int>(SaveData::loadString("user_master_id")));
			};
			
			auto noFunc = [](){
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_no.mp3");
			};
			
			string message = _name + "(¥" + _price + ")" ;
			message += "を購入しますか？" ;
			HMSLOG("stone message = %s", message.c_str());
			
			auto popup_layer = StoneShopBuyConfirmLayer::create(StoneShopBuyConfirmLayer::ButtonType::YES_AND_NO,message, yesFunc, noFunc) ;
			auto normal_scene = (NormalScene *)this->getParent()->getParent()->getParent()->getParent();
			normal_scene->addChild(popup_layer, 10);
		}
    });
    purchase_btn->setAnchorPoint(Point(1.0, 0.5));
    auto menu = Menu::create(purchase_btn, nullptr);
    menu->setPosition(Point(this->getContentSize().width, this->getContentSize().height / 2));
    this->addChild(menu, 1);
}

void JewelSprite::showJewelNum()
{
    //宝玉の数を表示
    auto jewel_num = HMSFontUtil::createWithTTFBySystemFontArgument(_name, FONT_NAME_SYSTEM, 30);
    jewel_num->setAlignment(TextHAlignment::LEFT);
    jewel_num->setAnchorPoint(Point(0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    jewel_num->setPosition(Point(130, 56));
    this->addChild(jewel_num, 1);
}

void JewelSprite::showJewelPrice()
{
    //宝玉の価格表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("価格 :", FONT_NAME_SYSTEM, 24);
    label->setAlignment(TextHAlignment::LEFT);
    label->setAnchorPoint(Point(0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    label->setPosition(Point(140, 14));
    this->addChild(label, 1);
    
    auto price_label = HMSFontUtil::createWithTTFBySystemFontArgument("¥" + _price, FONT_NAME_SYSTEM, 31);
    price_label->setAlignment(TextHAlignment::RIGHT);
    price_label->setAnchorPoint(Point(1.0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    price_label->setPosition(Point(444, 14));
    this->addChild(price_label, 1);
}
void JewelSprite::onEnter()
{
    Sprite::onEnter();
    
    //宝玉画像の作成
    createJewelImg();
    
    //購入ボタンの作成
    createButton();
    
    //宝玉の数を表示
    showJewelNum();
    
    //価格文字の作成
    showJewelPrice();
}