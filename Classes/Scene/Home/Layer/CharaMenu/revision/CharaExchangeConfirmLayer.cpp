#include "CharaExchangeConfirmLayer.h"

CharaExchangeConfirmLayer::CharaExchangeConfirmLayer()
{
}

CharaExchangeConfirmLayer* CharaExchangeConfirmLayer::create(Json chara_data)
{
    CharaExchangeConfirmLayer *sprite = new CharaExchangeConfirmLayer();
    
    if (sprite && sprite->init(chara_data))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaExchangeConfirmLayer::init(Json chara_data)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharacterListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの設定
    _chara_data = chara_data;
    
    return true;
}

void CharaExchangeConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //確認文字の作成、表示
    createConfirmLabel();
    
    //はい、いいえボタンの作成
    createButton();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void CharaExchangeConfirmLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CharaExchangeConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaExchangeConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaExchangeConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaExchangeConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaExchangeConfirmLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, Z_Shadow, T_Shadow);
    
    //背景
    _chara_confirm_bg = CharaExchangeConfirmSprite::create(_chara_data);
    _chara_confirm_bg->setPosition(convertFromDisplaySize(Point(320, 560), 0.5, 0.5));
    _chara_confirm_bg->setScale(0);
    this->addChild(_chara_confirm_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    _bg = Sprite::create("init/CharaMenu/revision/chara_exchange/exchange_confirm_bk.png");
    _bg->setPosition(convertFromDisplaySize(Point(320, 204), 0.5, 0.5));
    _bg->setScale(0);
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
}

void CharaExchangeConfirmLayer::createConfirmLabel()
{
    //確認ラベル
    string text = "×" + _chara_data["need_exchange_ticket"].string_value() + "で";
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 30);
    label->setHorizontalAlignment(TextHAlignment::LEFT);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(280, 165));
    _bg->addChild(label);
    
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("このキャラを手に入れますか?", FONT_NAME_SYSTEM, 30);
    label2->setPosition(Point(_bg->getContentSize().width / 2, 120));
    _bg->addChild(label2);
}

void CharaExchangeConfirmLayer::createButton()
{
    //はいボタン
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender) {
        
        //もう一度確認ポップアップ表示
        auto exchange_layer = (CharaExchangeLayer *)this->getParent();
        string exchange_chara_id = _chara_data["exchange_chara_id"].string_value();
        
        auto ok_func = [exchange_layer, exchange_chara_id](){
            
            //はい、交換通信処理
            exchange_layer->exchangeCharaDataRequest(exchange_chara_id);
        };
        
        auto ng_func = [](){};
        
        string message = "交換チケットを" + _chara_data["need_exchange_ticket"].string_value() + "枚消費します\nよろしいですか？";
        auto popup_layer = popupConfirmLayer::create(message);
        popup_layer->setFunction(ok_func, ng_func);
        exchange_layer->addChild(popup_layer, CharaExchangeLayer::ZOrder::Z_Popup_Layer, CharaExchangeLayer::Tag::T_Popup_Layer);
        
        //自身を閉じる処理
        Utility::closePopupAction(this);
    } );
    
    //いいえボタン
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender) {
        
        //閉じる処理
        Utility::closePopupAction(this);
    });
    
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu, Z_Menu, T_Menu);
}

bool CharaExchangeConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan CharaExchangeConfirmLayer!");
    
    return true;
}
void CharaExchangeConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void CharaExchangeConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void CharaExchangeConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
