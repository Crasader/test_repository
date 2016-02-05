
#include "ContinueStoreScroll.h"
#include "ContinueStoneShop.h"
#include "LoadingLayer.h"
#include "LoadingManager.h"
#include "PopupNoYes.h"

void ContinueStoreScroll::onEnter(){
    ScrollView::onEnter();
    
    ratio = getVisibleRatio();
    _cell_size = 130 * ratio;
    
}

void ContinueStoreScroll::setJson(Json json_txt){
    int count = countJsonData(json_txt) ;
    
    setAnchorPoint(Size(0.0f,0.0f));
    //スクロール枠の下限
    setPosition(convertFromDisplaySize(Point(getVisibleSpaceWidth()/ratio, getVisibleSpaceHeight()/ratio+50),0.0f,0.0f));
    
    //スクロール枠の下限から上限までの距離
    setViewSize(Size(DISPLAY_WIDTH*ratio, 770*ratio));
    
    //スクロール内のコンテンツのサイズ
    setContentSize(Size(DISPLAY_WIDTH*ratio, _cell_size * count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    setContentOffset(Point(0, -_cell_size * count + 770*ratio));


    //縦方向にスクロール設定
    setDirection(ScrollView::Direction::VERTICAL);
    
    setBounceable(true);

    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        CCLOG("ローケーション：%f：%f",touch->getLocation().x,touch->getLocation().y);
        //if(touch->getLocation().x<0 || touch->getLocation().y<0 || touch->getLocation().x<0 || touch->getLocation().y<0)
        return true ;
    };
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    
    
    vector<string> shopItemNameArray;
    vector<string> shopItemPriceArray;
    vector<string> shopItemNameIdArray;
    
    
    for (int i=0; i < count ; i++) {
        string name = json_txt[to_string(i)]["name"].string_value();
        string price = json_txt[to_string(i)]["price"].string_value();
        string product_id = json_txt[to_string(i)]["product_id"].string_value();
        
        shopItemNameArray.push_back(name) ;
        shopItemPriceArray.push_back(price);
        shopItemNameIdArray.push_back(product_id);
    }
    
    //int display_item_count = (int)shopItemNameIdArray.size() ;
    
    
    
    
    //プレゼント画像の配置
    int num = 0;
    for (int i = (int)shopItemNameArray.size()-1; 0 <= i; i--)
    {
        makeJewelSprite(shopItemNameArray[num], shopItemPriceArray[num], shopItemNameIdArray[num], i);
        //jewel_sp->setPosition(Point(320 * ratio, (_cell_size / 2) + (_cell_size * i)));
        num++;
    }
}

void ContinueStoreScroll::makeJewelSprite(string name,string price,string name_id, int line){
    
    float line_height = _cell_size * line;
    
    
    //初回限定の購入商品のみ、デザインを豪華なやつにする
    string image_path;
    if(name_id.find("once", 0 ) != string::npos ){
        image_path = "init/Home/Shop/popup_shop2.png" ;
    }else{
        image_path = "init/Home/Shop/popup_shop.png" ;
    }
    
    auto board = Sprite::create(image_path);
    board->setAnchorPoint(Point(0.5f, 1));
    board->setScale(ratio);
    board->setPosition(Point(DISPLAY_WIDTH/2*ratio, 130*ratio)+Point(0,line_height));
    addChild(board, 1);
    
    
    //宝玉画像の作成
    auto jewel_sp = Sprite::create("init/Home/Shop/item.png");
    jewel_sp->setScale(0.95f*ratio);
    jewel_sp->setAnchorPoint(Point(0.5f, 0));
    jewel_sp->setPosition(Point(70*ratio, 23*ratio)+Point(0,line_height));
    addChild(jewel_sp, 1);
    
    
    //宝玉の数を表示
    auto jewel_num = HMSFontUtil::createWithTTFBySystemFontArgument(name, FONT_NAME_SYSTEM, 30);
    jewel_num->setAlignment(TextHAlignment::LEFT);
    jewel_num->setAnchorPoint(Point(0, 0));
    jewel_num->setScale(ratio);
    jewel_num->setPosition(Point(132*ratio,70*ratio)+Point(0,line_height));
    addChild(jewel_num, 2);
    
    
    //宝玉の価格表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("価格 :", FONT_NAME_SYSTEM, 24);
    label->setAlignment(TextHAlignment::LEFT);
    label->setAnchorPoint(Point(0.5f, 0));
    label->setScale(ratio);
    label->setPosition(Point(182*ratio, 27*ratio)+Point(0,line_height));
    addChild(label, 3);
    
    auto price_label = HMSFontUtil::createWithTTFBySystemFontArgument("¥" + price, FONT_NAME_SYSTEM, 31);
    price_label->setAlignment(TextHAlignment::RIGHT);
    price_label->setAnchorPoint(Point(1.0f, 0));
    price_label->setScale(ratio);
    price_label->setPosition(Point(440*ratio, 23*ratio)+Point(0,line_height));
    addChild(price_label, 4);

    
    
    
    //購入ボタンの作成
    auto btn_off = Sprite::create("init/Home/Shop/bt_buy.png");
    auto btn_on  = Sprite::create("init/Home/Shop/bt_buy.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto purchase_btn = MenuItemSprite::create(btn_off, btn_on, [this, name, price, name_id](Ref *obj){
        
        stopAllActions();
        
        int use_age_buy_status = SaveData::loadInt("ageBuyStatus"); //ユーザーが選んだ年齢制限
        //HMSLOG("price = %s", _price.c_str());
        int new_pkga_sum_coin = SaveData::loadInt("uvec_pkga_sum_coin") + stoi(price); //今月の課金額 + 新しい購入額
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
            
            addChild(popup_layer, 100000);
        } else {
            auto yesFunc = [&]()//はいを押した時
            {
                //ローディング作成
                LoadingManager::setLoading((Layer *)getParent());
                //getParent()->addChild(loading,100000);
                //loading->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5, 0.5) - loading->convertToWorldSpaceAR(Size::ZERO));
                
                //SE
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
                
                //購入処理へ
                auto stone_shop_layer = (ContinueStoneShop *)this->getParent();
                stone_shop_layer->setDelegate(name, price, name_id, fromString<int>(SaveData::loadString("user_master_id")));
            };
            
            auto noFunc = [](){
                //SE
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_no.mp3");
            };
            
            string message = name + "(¥" + price + ")" ;
            message += "を購入しますか？" ;
            
            auto popup_layer = PopupNoYes::create() ;
            popup_layer->setPopup(message, noFunc, yesFunc);
            popup_layer->setAnchorPoint(Size(0.5f,0.5f));
            getParent()->addChild(popup_layer,10000001);
            popup_layer->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2),0.5f,0.5f) - popup_layer->convertToWorldSpaceAR(Size::ZERO));
            
        }
    });
    purchase_btn->setAnchorPoint(Point(0.5f, 0.5f));
    purchase_btn->setPosition(Point::ZERO);
    purchase_btn->setScale(ratio);//スケールはこっちに
    auto menu = Menu::create(purchase_btn, nullptr);
    //menu->setScale(ratio);
    menu->setAnchorPoint(Point(0.5f, 0));
    menu->setPosition(Point(540*ratio, 70*ratio)+Point(0,line_height));
    addChild(menu, 6);
    
}

