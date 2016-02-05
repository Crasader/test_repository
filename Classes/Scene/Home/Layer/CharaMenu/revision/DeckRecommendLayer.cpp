#include "DeckRecommendLayer.h"

DeckRecommendLayer::DeckRecommendLayer()
{
}

DeckRecommendLayer* DeckRecommendLayer::create()
{
    DeckRecommendLayer *sprite = new DeckRecommendLayer();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckRecommendLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharacterListLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void DeckRecommendLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //タイトルの作成
    createTitle();
    
    //おすすめデッキボタンの作成
    createButton();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void DeckRecommendLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(DeckRecommendLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckRecommendLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckRecommendLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckRecommendLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckRecommendLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_600.png");
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    _bg->setScale(0);
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
}

void DeckRecommendLayer::createTitle()
{
    //タイトルの作成
    auto title = Sprite::create("init/Home/Popup/popup_index.png");
    title->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 70));
    _bg->addChild(title);
    
    //タイトル文字
    auto titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("おすすめデッキ選択", FONT_NAME_SYSTEM, 33);
    titleLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    titleLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
    titleLabel->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 70));
    _bg->addChild(titleLabel, Z_Title, T_Title);
    
    //閉じるボタン
    auto close_btn = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    auto menu = Menu::create(close_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize()));
    _bg->addChild(menu, Z_Close_Menu, T_Close_Menu);
}

void DeckRecommendLayer::createButton()
{
    HmsVector<MenuItemSprite *> menuItem;
    
    for (int i = 0; i < 3; i++)
    {
        //ボタン画像
        auto sp_off = Sprite::create("init/CharaMenu/revision/bt_recommended" + toString<int>(i + 1) + ".png");
        auto sp_on  = Sprite::create("init/CharaMenu/revision/bt_recommended" + toString<int>(i + 1) + ".png");
        sp_on->setColor(Color3B(150, 150, 150));
        sp_on->setCascadeColorEnabled(true);
        
        menuItem.push_back(MenuItemSprite::create(sp_off, sp_on, CC_CALLBACK_1(DeckRecommendLayer::selectedDeckType, this)));
        menuItem[i]->setTag(T_Offense_Deck + i);
    }
    
//    string text_array[] = {"攻撃おすすめデッキ", "防御おすすめデッキ", "軍師おすすめデッキ"};
//    string icon_array[] = {"icon_offense2.png", "icon_defense2.png", "icon_strategist2.png"};
//    
//    HmsVector<MenuItemSprite *> menuItem;
//    
//    for (int i = 0; i < sizeof(text_array) / sizeof(text_array[0]); i++)
//    {
//        //ボタン画像
//        auto sp_off = Sprite::create("init/Home/Common/bt3.png");
//        auto sp_on  = Sprite::create("init/Home/Common/bt3.png");
//        sp_on->setColor(Color3B(150, 150, 150));
//        sp_on->setCascadeColorEnabled(true);
//        
//        //ボタン文字
//        auto label_off = HMSFontUtil::createWithTTFBySystemFontArgument(text_array[i], FONT_NAME_SYSTEM, 30);
//        label_off->setPosition(Point(sp_off->getContentSize().width / 2 + 10, sp_off->getContentSize().height / 2));
//        label_off->setTextColor(Color4B(255, 230, 140, 255));
//        label_off->enableOutline(Color4B::BLACK, 2);
//        sp_off->addChild(label_off);
//        
//        auto label_on = HMSFontUtil::createWithTTFBySystemFontArgument(text_array[i], FONT_NAME_SYSTEM, 30);
//        label_on->setPosition(Point(sp_on->getContentSize().width / 2 + 10, sp_on->getContentSize().height / 2));
//        label_on->setTextColor(Color4B(255, 230, 140, 255));
//        label_on->enableOutline(Color4B::BLACK, 2);
//        sp_on->addChild(label_on);
//        
//        //アイコン
//        auto icon_off = Sprite::create("init/CharaMenu/" + icon_array[i]);
//        icon_off->setPosition(Point(50, sp_off->getContentSize().height / 2));
//        sp_off->addChild(icon_off);
//        
//        auto icon_on = Sprite::create("init/CharaMenu/" + icon_array[i]);
//        icon_on->setPosition(Point(50, sp_on->getContentSize().height / 2));
//        sp_on->addChild(icon_on);
//        
//        menuItem.push_back(MenuItemSprite::create(sp_off, sp_on, CC_CALLBACK_1(DeckRecommendLayer::selectedDeckType, this)));
//        menuItem[i]->setTag(T_Offense_Deck + i);
//    }
    
    auto menu = Menu::create(menuItem[0], menuItem[1], menuItem[2], nullptr);
    menu->alignItemsVerticallyWithPadding(30);
    menu->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 - 20));
    _bg->addChild(menu, Z_Deck_Menu, T_Deck_Menu);
}

void DeckRecommendLayer::selectedDeckType(cocos2d::Ref *sender)
{
    auto menuItem = (MenuItemSprite *)sender;
    
    auto formation_layer = (DeckFormationLayer *)this->getParent();
    
    switch (menuItem->getTag()) {
        case T_Offense_Deck:    formation_layer->setRecommendData(10, 7, 3); break;
        case T_Defense_Deck:    formation_layer->setRecommendData(8, 8, 4);  break;
        case T_Strategist_Deck: formation_layer->setRecommendData(7, 7, 6);  break;
        default: break;
    }
    
    //閉じる
    Utility::closePopupAction(this);
}

bool DeckRecommendLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckRecommendLayer!");
    return true;
}

void DeckRecommendLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckRecommendLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckRecommendLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
