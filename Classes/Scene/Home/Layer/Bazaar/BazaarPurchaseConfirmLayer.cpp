#include "BazaarPurchaseConfirmLayer.h"
#include "BazaarCardDetailLayer.h"

BazaarPurchaseConfirmLayer::BazaarPurchaseConfirmLayer()
{
}

BazaarPurchaseConfirmLayer* BazaarPurchaseConfirmLayer::create(Json charaData, Node* node)
{
    BazaarPurchaseConfirmLayer *sprite = new BazaarPurchaseConfirmLayer();
    
    if (sprite && sprite->init(charaData, node))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool BazaarPurchaseConfirmLayer::init(Json charaData, Node* node)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarPurchaseConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    this->node = node;
    
    //データの設定
    this->charaData = charaData;
    
    return true;
}

void BazaarPurchaseConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //カード詳細生成
    createCardDetailLayer();
    
    //確認文字の作成、表示
    createConfirmLabel();
    
    //はい、いいえボタンの作成
    createButton();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void BazaarPurchaseConfirmLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BazaarPurchaseConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarPurchaseConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarPurchaseConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarPurchaseConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BazaarPurchaseConfirmLayer::createBg()
{
    //影
    Sprite* shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, Z_Shadow, T_Shadow);
    
    //背景
    //    _chara_confirm_bg = CharaExchangeConfirmSprite::create(_charaData);
    //    _chara_confirm_bg->setPosition(convertFromDisplaySize(Point(320, 560), 0.5, 0.5));
    //    _chara_confirm_bg->setScale(0);
    //    this->addChild(_chara_confirm_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    background = Sprite::create("init/CharaMenu/revision/chara_exchange/exchange_confirm_bk.png");
    background->setPosition(convertFromDisplaySize(Point(320, 204), 0.5, 0.5));
    background->setScale(0);
    this->addChild(background, Utility::Z_Popup_Action, Utility::T_Popup_Action);
}
void BazaarPurchaseConfirmLayer::createCardDetailLayer()
{
    BazaarCardDetailLayer* bazaarCardDetailLayer = BazaarCardDetailLayer::create(charaData["chara_id"].string_value());
    bazaarCardDetailLayer->setPosition(convertFromDisplaySize(Point(320, 552), 0.5, 0.5));
    this->addChild(bazaarCardDetailLayer, Z_Shadow, T_Shadow);
}

void BazaarPurchaseConfirmLayer::createConfirmLabel()
{
    //確認ラベル
    string text = "×" + this->charaData["exchange_ticket"].string_value() + "で";
    Label* priceTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30);
    priceTextLabel->setHorizontalAlignment(TextHAlignment::LEFT);
    priceTextLabel->setAnchorPoint(Point(0, 0.5));
    priceTextLabel->setPosition(Point(280, 165));
    background->addChild(priceTextLabel);
    
    Label* confirmTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument("このキャラを手に入れますか?", FONT_NAME_SYSTEM, 30);
    confirmTextLabel->setPosition(Point(background->getContentSize().width / 2, 120));
    background->addChild(confirmTextLabel);
}

void BazaarPurchaseConfirmLayer::createButton()
{
    //はいボタン
    Sprite* yesInactive = Sprite::create("init/Home/Common/bt_yes.png");
    Sprite* yesActive  = Sprite::create("init/Home/Common/bt_yes.png");
    yesActive->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite* yesButton = MenuItemSprite::create(yesInactive, yesActive, [this](Ref* sender) {
        
        BazaarSearchListLayer* bazaarSreachLayer = (BazaarSearchListLayer *)node;
        bazaarSreachLayer->exchangeCharaDataRequest(charaData["bazaar_log_id"].string_value());

        //自身を閉じる処理
        Utility::closePopupAction(this);
    } );
    
    //いいえボタン
    Sprite* noInactive = Sprite::create("init/Home/Common/bt_no.png");
    Sprite* noActive  = Sprite::create("init/Home/Common/bt_no.png");
    noActive->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite* noButton = MenuItemSprite::create(noInactive, noActive, [this](Ref* sender) {
        
        //閉じる処理
        Utility::closePopupAction(this);
    });
    
    Menu* menu = Menu::create(yesButton, noButton, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(Point(background->getContentSize().width / 2, 60));
    background->addChild(menu, Z_Menu, T_Menu);
}



bool BazaarPurchaseConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan BazaarPurchaseConfirmLayer!");
    
    return true;
}
void BazaarPurchaseConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BazaarPurchaseConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void BazaarPurchaseConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
