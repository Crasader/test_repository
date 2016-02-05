#include "StoneShopAgeConfirmLayer.h"

bool StoneShopAgeConfirmLayer::init()
{
    if ( !Layer::init() ) {
        return false;
    }
    HMSLOG("StoneShopAgeConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    initEventListener() ;
    
    setupImage();
    return true;
}

void StoneShopAgeConfirmLayer::onEnter()
{
    Layer::onEnter();

    //0.1数秒後ポップアップアクション
    auto func = CallFunc::create([this](){
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3",false);
        popupAction();
    });
    runAction(Sequence::create(DelayTime::create(0.1f), func, nullptr));
    
}

void StoneShopAgeConfirmLayer::setupImage()
{
    //画面の半分のサイズ
    int helfWitdh = DISPLAY_WIDTH / 2 ;
    int helfHeight = DISPLAY_HEIGHT / 2 ;
    
    //二十歳を超えているか確認するためのメッセージ
    string confirmMessage = "あなたの年齢(ねんれい)によって\n" ;
    confirmMessage += "宝玉を買える条件(じょうけん)が\n" ;
    confirmMessage += "きまっています。\n" ;
    confirmMessage += "あなたの年齢を教えてください。" ;
    
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(helfWitdh, helfHeight), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //窓
    auto bg = Sprite::create("init/Home/Popup/popup_600.png");
    bg->setPosition(convertFromDisplaySize(Point(helfWitdh, helfHeight), 0.5, 0.5));
    bg->setScale(0);
    this->addChild(bg, 2, POPUP_ACTION_TAG);
    
    Size bgsize = bg->getContentSize();
    HMSLOG("bgsize: height %f  width %f", bgsize.height, bgsize.width) ;
    
    //モーダルレイヤーを消すボタン
    auto back = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close_L.png", CC_CALLBACK_0(StoneShopAgeConfirmLayer::deleteAction, this));
    auto Menu1 = Menu::create(back, nullptr);
    Menu1->setPosition(Point(bgsize.width - 10, bgsize.height - 10));
    bg->addChild(Menu1, 25, BACK_TAG);
    
    //注意書きメッセージ
    int confirmTextHeight = 150 ;
    auto confirmTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument(confirmMessage, FONT_NAME_SYSTEM, 30);
    confirmTextLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    confirmTextLabel->setPosition(Point(bgsize.width/2, bgsize.height/2+confirmTextHeight));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	confirmTextLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(confirmTextLabel, 3);
    
    ////////////// Menu関係 start /////////////
    
    //// 20歳以上のボタン start ////
    int heightOver20 = 0 ; //中心からの下方向の距離
    //ボタン背景画像
    auto over20btn = MenuItemImage::create("init/Home/Common/bt3.png", "init/Home/Common/bt3.png", CC_CALLBACK_1(StoneShopAgeConfirmLayer::onOver20Button, this));
    auto over20Menu= Menu::create(over20btn, nullptr);
    over20Menu->setPosition(Point(bgsize.width/2, bgsize.height/2-heightOver20));
    bg->addChild(over20Menu, 3);
    
    //ボタン文字
    auto over20Label = HMSFontUtil::createWithTTFBySystemFontArgument("20歳以上", FONT_NAME_SYSTEM, 33);
	over20Label->setHorizontalAlignment(TextHAlignment::LEFT);
	over20Label->setPosition(bgsize.width/2, bgsize.height/2-heightOver20);
//enableOutline(Color4B(0, 0, 0, 255), 1);
	over20Label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(over20Label, 4);
    //// 20歳以上のボタン end ////
    
    //// 16歳〜19歳のボタン start ////
    int heightOver16= 110 ; //中心からの下方向の距離
    //ボタン背景画像
    auto over16btn = MenuItemImage::create("init/Home/Common/bt3.png", "init/Home/Common/bt3.png", CC_CALLBACK_1(StoneShopAgeConfirmLayer::onOver16Button, this));
    auto over16Menu= Menu::create(over16btn, nullptr);
    over16Menu->setPosition(Point(bgsize.width/2, bgsize.height/2-heightOver16));
	bg->addChild(over16Menu, 3);
    
    //ボタン文字
    auto over16Label = HMSFontUtil::createWithTTFBySystemFontArgument("16〜19歳", FONT_NAME_SYSTEM, 33);
	over16Label->setHorizontalAlignment(TextHAlignment::LEFT);
	over16Label->setAnchorPoint(Point(0.5,0.5));
    over16Label->setPosition(Point(bgsize.width/2, bgsize.height/2-heightOver16 ));
//enableOutline(Color4B(0, 0, 0, 255), 1);
	over16Label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(over16Label, 4);
    //// 16歳以上20未満のボタン end ////
    
    //// 16未満のボタン start ////
    int heightUnder16= 220; //中心からの下方向の距離
    //ボタン背景画像
    auto under16btn = MenuItemImage::create("init/Home/Common/bt3.png", "init/Home/Common/bt3.png", CC_CALLBACK_1(StoneShopAgeConfirmLayer::onUnder16Button, this));
    auto under16Menu= Menu::create(under16btn, nullptr);
    under16Menu->setPosition(Point(bgsize.width/2, bgsize.height/2-heightUnder16));
    bg->addChild(under16Menu, 3);
    
    //ボタン文字
    auto under16Label = HMSFontUtil::createWithTTFBySystemFontArgument("16歳未満", FONT_NAME_SYSTEM, 33);
	under16Label->setHorizontalAlignment(TextHAlignment::LEFT);
    under16Label->setAnchorPoint(Point(0.5,0.5));
    under16Label->setPosition(Point(bgsize.width/2, bgsize.height/2-heightUnder16));
//enableOutline(Color4B(0, 0, 0, 255), 1);
	under16Label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(under16Label, 4);
    //// 16未満のボタン end ////
    
}

// 20歳以上のボタンstart
void StoneShopAgeConfirmLayer::onOver20Button(Ref *obj)
{
	SaveData::saveInt("ageBuyStatus", AGE_BUY_STATUS_OVER20);
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->changePage("StoneShopMenu");
    this->removeFromParent();
}

// 16歳〜19未満のボタンタップ時のコールバック
void StoneShopAgeConfirmLayer::onOver16Button(Ref *obj)
{
    deleteAction() ;
    auto shop_menu_layer = (ShopMenuLayer *)getParent();
    auto func = CallFunc::create([this,shop_menu_layer](){
        auto popup_layer = StoneShopAge2ndConfirmLayer::create();
        string message = "宝玉を買うには本物のお金が必要です。\n";
        message += "必ず購入前にご両親の許可を\n" ;
        message += "もらってから購入して下さい。\n\n" ;
        message += "許可はもらいましたか？" ;
        popup_layer->setupImage(message, AGE_BUY_STATUS_OVER16);
        shop_menu_layer->addChild(popup_layer, ORDER_RECEIVE_LAYER);
    });
    shop_menu_layer->runAction(Sequence::create(DelayTime::create(0.3f), func, nullptr));
}

// 16未満のボタンタップ時のコールバック
void StoneShopAgeConfirmLayer::onUnder16Button(Ref *obj)
{
    deleteAction() ;
    auto shop_menu_layer = (ShopMenuLayer *)getParent();
    auto func = CallFunc::create([this,shop_menu_layer](){
        auto popup_layer = StoneShopAge2ndConfirmLayer::create();
        string message = "宝玉(ほうぎょく)を買うには\n" ;
        message += "本物のお金がひつようです。\n" ;
        message += "ご両親にスマートフォンを渡して\n" ;
        message += "購入(こうにゅう)の手続きを\n" ;
        message += "してもらってください。\n\n" ;
        message += "ご両親にスマホを渡しましたか？" ;
        
        popup_layer->setupImage(message, AGE_BUY_STATUS_UNDER16);
        shop_menu_layer->addChild(popup_layer, ORDER_RECEIVE_LAYER);
    });
    shop_menu_layer->runAction(Sequence::create(DelayTime::create(0.3f), func, nullptr));
}

void StoneShopAgeConfirmLayer::initEventListener()
{
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
            return true ;
    };
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

}

//窓を出現させる
void StoneShopAgeConfirmLayer::popupAction()
{
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer();
    
    //自身の子からPOPUP_ELEMENTの要素をすべて拡大する
    auto child = getChildByTag(POPUP_ACTION_TAG) ;
    auto scale    = ScaleTo::create(0.1f, ratio);
    child->runAction(scale);
    
    //指定秒数待ってからタップを可能にする。
    auto func = CallFunc::create([normal_scene](){
        normal_scene->removeTapDisableLayer() ;
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

//窓をひっこめ削除する
void StoneShopAgeConfirmLayer::deleteAction()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer() ;
    
    //自身の子からPOPUP_ELEMENTの要素をすべて縮小する
    auto child = getChildByTag(POPUP_ACTION_TAG) ;
    auto scale = ScaleTo::create(0.1f, 0);
    child->runAction(scale);
    
    auto func = CallFunc::create([this,normal_scene](){
        this->removeFromParent();
        //タップ禁止レイヤー削除
        normal_scene->removeTapDisableLayer() ;
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}