#include "DeckCopyLayer.h"

DeckCopyLayer::DeckCopyLayer()
:_deck_no()
{
}

DeckCopyLayer* DeckCopyLayer::create()
{
    DeckCopyLayer *sprite = new DeckCopyLayer();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckCopyLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharacterListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //現在選択中のデッキ番号を取得
    _deck_no = SaveData::loadString("party_no");
    
    return true;
}

void DeckCopyLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //タイトルの作成
    createTitle();
    
    //ボタンの作成
    createButton();
    
    //テキストの表示
    showMessageLabel();
    
    //ポップアップ表示アクション
    Utility::openPopupAction(this);
}

void DeckCopyLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(DeckCopyLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckCopyLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckCopyLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckCopyLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckCopyLayer::createBg()
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

void DeckCopyLayer::createTitle()
{
    //タイトルの作成
    auto title = Sprite::create("init/Home/Popup/popup_index.png");
    title->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 50));
    _bg->addChild(title);
    
    //タイトル文字
    auto titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("デッキコピー", FONT_NAME_SYSTEM, 33);
    titleLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    titleLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
    titleLabel->setPosition(Point(title->getContentSize() / 2));
    title->addChild(titleLabel, Z_Title, T_Title);
    
    //閉じるボタン
    auto close_btn = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        
        //矢印の表示
        auto layer = (DeckFormationTopLayer *)this->getParent();
        layer->_left_arrow->setVisible(true);
        layer->_right_arrow->setVisible(true);
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    auto menu = Menu::create(close_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize()));
    _bg->addChild(menu, Z_Close_Menu, T_Close_Menu);
}

void DeckCopyLayer::createButton()
{
    //デッキ番号ボタンの作成
    HmsVector<MenuItemSprite *> menuItem;
    
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        //ボタン画像
        auto sp_off = Sprite::create("init/CharaMenu/revision/deck_copy/deck_copy" + toString<int>(i + 1) + ".png");
        auto sp_on  = Sprite::create("init/CharaMenu/revision/deck_copy/deck_copy" + toString<int>(i + 1) + ".png");
        sp_on->setColor(Color3B(150, 150, 150));
        sp_on->setCascadeColorEnabled(true);
        
        menuItem.push_back(MenuItemSprite::create(sp_off, sp_on, CC_CALLBACK_1(DeckCopyLayer::selectedDeckNo, this)));
        menuItem[i]->setTag(Deck_1 + i);
    }
    
    //TODO デッキ10にする際は下記の条件分けを消す！
    if (menuItem.size() == 5)
    {
        auto menu1 = Menu::create(menuItem[0], menuItem[1], nullptr);
        menu1->alignItemsHorizontallyWithPadding(20);
        menu1->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 + 103));
        _bg->addChild(menu1, Z_Deck_Menu_1, T_Deck_Menu_1);
        
        auto menu2 = Menu::create(menuItem[2], menuItem[3], menuItem[4], nullptr);
        menu2->alignItemsHorizontallyWithPadding(20);
        menu2->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 - 103));
        _bg->addChild(menu2, Z_Deck_Menu_2, T_Deck_Menu_2);
        
        //現在選択中のデッキ番号のボタンを押せなくする
        int array_no = fromString<int>(_deck_no) - 1;
        menuItem[array_no]->setSelectedImage(Sprite::create("init/CharaMenu/revision/deck_copy/deck_copy_gray" + _deck_no + ".png"));
        menuItem[array_no]->setEnabled(false);
        menuItem[array_no]->selected();
    }
    else if (menuItem.size() == 10)
    {
        auto menu1 = Menu::create(menuItem[0], menuItem[1], menuItem[2], nullptr);
        menu1->alignItemsHorizontallyWithPadding(10);
        menu1->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 + 145));
        _bg->addChild(menu1, Z_Deck_Menu_1, T_Deck_Menu_1);
        
        auto menu2 = Menu::create(menuItem[3], menuItem[4], menuItem[5], menuItem[6], nullptr);
        menu2->alignItemsHorizontallyWithPadding(10);
        menu2->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2));
        _bg->addChild(menu2, Z_Deck_Menu_2, T_Deck_Menu_2);
        
        auto menu3 = Menu::create(menuItem[7], menuItem[8], menuItem[9], nullptr);
        menu3->alignItemsHorizontallyWithPadding(10);
        menu3->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height / 2 - 145));
        _bg->addChild(menu3, Z_Deck_Menu_3, T_Deck_Menu_3);
        
        //現在選択中のデッキ番号のボタンを押せなくする
        int array_no = fromString<int>(_deck_no) - 1;
        menuItem[array_no]->setSelectedImage(Sprite::create("init/CharaMenu/revision/deck_copy/deck_copy_gray" + _deck_no + ".png"));
        menuItem[array_no]->setEnabled(false);
        menuItem[array_no]->selected();
    }
}

void DeckCopyLayer::showMessageLabel()
{
    //テキストの表示
    auto message_label = HMSFontUtil::createWithTTFBySystemFontArgument("コピー先のデッキ番号を選択してください", FONT_NAME_SYSTEM, 28);
    message_label->setPosition(Point(_bg->getContentSize().width / 2, 50));
    message_label->enableOutline(Color4B::BLACK, 2);
    _bg->addChild(message_label);
}

void DeckCopyLayer::selectedDeckNo(cocos2d::Ref *sender)
{
    auto menuItem = (MenuItemSprite *)sender;
    
    int selected_number = 0;
    switch (menuItem->getTag())
    {
        case Deck_1: selected_number = 1; break;
        case Deck_2: selected_number = 2; break;
        case Deck_3: selected_number = 3; break;
        case Deck_4: selected_number = 4; break;
        case Deck_5: selected_number = 5; break;
        case Deck_6: selected_number = 6; break;
        case Deck_7: selected_number = 7; break;
        case Deck_8: selected_number = 8; break;
        case Deck_9: selected_number = 9; break;
        case Deck_10: selected_number = 10; break;
        default: break;
    }
    
    CCLOG("選択番号 = %d", selected_number);
    
    //確認レイヤーの表示
    auto top_layer = (DeckFormationTopLayer *)this->getParent();
    
    auto confirm_layer = DeckCopyConfirmLayer::create(selected_number);
    top_layer->addChild(confirm_layer, DeckFormationTopLayer::Z_Copy_Confirm_Layer, DeckFormationTopLayer::T_Copy_Confirm_Layer);
    
    //自身を消す
    Utility::closePopupAction(this);
}

bool DeckCopyLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckCopyLayer!");
    return true;
}

void DeckCopyLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckCopyLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckCopyLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
