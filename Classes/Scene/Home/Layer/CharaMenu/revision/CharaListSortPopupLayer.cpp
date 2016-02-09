#include "CharaListSortPopupLayer.h"

CharaListSortPopupLayer::CharaListSortPopupLayer()
{
}

CharaListSortPopupLayer* CharaListSortPopupLayer::create(bool* chara_type, string from_page_name)
{
    CharaListSortPopupLayer *sprite = new CharaListSortPopupLayer();
    
    if (sprite && sprite->init(chara_type, from_page_name))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}


bool CharaListSortPopupLayer::init(bool* chara_type, string from_page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("CharaListSortPopupLayer::init()");
    
    ratio = getVisibleRatio();
    
    //パラメータの初期化
    _sort_status = (SortStatus)SaveData::loadInt("sort_status");
    _from_page_name = from_page_name;
    
    //配列の代入
    for(int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        _chara_type[i] = chara_type[i];
    }
    
    return true;
}

void CharaListSortPopupLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //タイトルの表示
    showTItle();
    
    //絞り込み属性ボタンの作成
    createCharaTypeButton();
    
    //表示順選択ボタンの作成
    createSortButton();
    
    //変更するボタンの作成
    createChangeButton();
    
    //選択条件のボタンの設定
    setSelectButtonData();
    
    //ポップアップアクション
    Utility::openPopupAction(this);
}

void CharaListSortPopupLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CharaListSortPopupLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaListSortPopupLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaListSortPopupLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaListSortPopupLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaListSortPopupLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_500.png");
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    _bg->setScale(0);
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
}

void CharaListSortPopupLayer::showTItle()
{
    //タイトル枠の表示
    auto title = Sprite::create("init/Home/Popup/popup_index.png");
    title->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 70));
    _bg->addChild(title);
    
    auto titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("キャラの並びを変更します", FONT_NAME_SYSTEM, 33);
    titleLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    titleLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
    titleLabel->setPosition(Point(title->getContentSize() / 2));
    title->addChild(titleLabel);
    
    //閉じるボタン
    auto close_btn = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    auto menu = Menu::create(close_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize()));
    _bg->addChild(menu, Z_Close_Menu, T_Close_Menu);
}

void CharaListSortPopupLayer::createCharaTypeButton()
{
    //絞り込みボタンの作成（onになると明るくなる）
    
    //絞り込み属性テキストの作成
    auto attributeLabel = HMSFontUtil::createWithTTFBySystemFontArgument("絞り込み属性", FONT_NAME_SYSTEM, 26);
    attributeLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    attributeLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
    attributeLabel->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 130));
    _bg->addChild(attributeLabel);

    //攻撃選択
    auto offense_off  = MenuItemImage::create("init/CharaMenu/bt_order_offense_gray.png", "init/CharaMenu/bt_order_offense_gray.png");
    auto offense_on = MenuItemImage::create("init/CharaMenu/bt_order_offense.png", "init/CharaMenu/bt_order_offense.png");
    auto offense_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setCharaTypeData, this), offense_off, offense_on, nullptr);
    offense_btn->setTag(CharaSprite::CharaType::OFFENSE);
    
    //防御選択
    auto defense_off  = MenuItemImage::create("init/CharaMenu/bt_order_defense_gray.png", "init/CharaMenu/bt_order_defense_gray.png");
    auto defense_on = MenuItemImage::create("init/CharaMenu/bt_order_defense.png", "init/CharaMenu/bt_order_defense.png");
    auto defense_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setCharaTypeData, this), defense_off, defense_on, nullptr);
    defense_btn->setTag(CharaSprite::CharaType::DEFENSE);
    
    //軍師選択
    auto strategist_off  = MenuItemImage::create("init/CharaMenu/bt_order_strategist_gray.png", "init/CharaMenu/bt_order_strategist_gray.png");
    auto strategist_on = MenuItemImage::create("init/CharaMenu/bt_order_strategist.png", "init/CharaMenu/bt_order_strategist.png");
    auto strategist_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setCharaTypeData, this), strategist_off, strategist_on, nullptr);
    strategist_btn->setTag(CharaSprite::CharaType::STRATEGIST);
    
    //総大将選択
    auto general_off  = MenuItemImage::create("init/CharaMenu/bt_order_general_gray.png", "init/CharaMenu/bt_order_general_gray.png");
    auto general_on = MenuItemImage::create("init/CharaMenu/bt_order_general.png", "init/CharaMenu/bt_order_general.png");
    auto general_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setCharaTypeData, this), general_off, general_on, nullptr);
    general_btn->setTag(CharaSprite::CharaType::GENERAL);

    auto menu = Menu::create(offense_btn, defense_btn, strategist_btn, general_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height - 195));
    _bg->addChild(menu, Z_Type_Menu, T_Type_Menu);
}

void CharaListSortPopupLayer::createSortButton()
{
    //並び替えボタンの作成
    auto showTypeLabel = HMSFontUtil::createWithTTFBySystemFontArgument("表示順選択", FONT_NAME_SYSTEM,26);
    showTypeLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    showTypeLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
    showTypeLabel->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 270));
    _bg->addChild(showTypeLabel);
    
    //戦闘力
    auto power_off = MenuItemImage::create("init/CharaMenu/bt_order_gray1.png", "init/CharaMenu/bt_order_gray1.png");
    auto power_on = MenuItemImage::create("init/CharaMenu/bt_order1.png", "init/CharaMenu/bt_order1.png");
    auto power_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setSortStatusData, this), power_off, power_on, nullptr);
    power_btn->setTag(SortStatus::Power);
    
    //レア度
    auto rare_off  = MenuItemImage::create("init/CharaMenu/bt_order_gray2.png", "init/CharaMenu/bt_order_gray2.png");
    auto rare_on = MenuItemImage::create("init/CharaMenu/bt_order2.png", "init/CharaMenu/bt_order2.png");
    auto rare_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setSortStatusData, this), rare_off, rare_on, nullptr);
    rare_btn->setTag(SortStatus::Rare);
    
    //配置士気
    auto need_cost_off  = MenuItemImage::create("init/CharaMenu/bt_order_gray3.png", "init/CharaMenu/bt_order_gray3.png");
    auto need_cost_on = MenuItemImage::create("init/CharaMenu/bt_order3.png", "init/CharaMenu/bt_order3.png");
    auto need_cost_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setSortStatusData, this), need_cost_off, need_cost_on, nullptr);
    need_cost_btn->setTag(SortStatus::NeedCost);
    
    //入手順
    auto insert_date_off  = MenuItemImage::create("init/CharaMenu/bt_order_gray4.png", "init/CharaMenu/bt_order_gray4.png");
    auto insert_date_on = MenuItemImage::create("init/CharaMenu/bt_order4.png", "init/CharaMenu/bt_order4.png");
    auto insert_date_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaListSortPopupLayer::setSortStatusData, this), insert_date_off, insert_date_on, nullptr);
    insert_date_btn->setTag(SortStatus::InsertDate);
    
    auto menu2 = Menu::create(power_btn, rare_btn, need_cost_btn, insert_date_btn, nullptr);
    menu2->alignItemsHorizontallyWithPadding(0);
    menu2->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height - 330));
    _bg->addChild(menu2, Z_Sort_Menu, T_Sort_Menu);

}

void CharaListSortPopupLayer::createChangeButton()
{
    //変更するボタンの作成
    auto change_off = Sprite::create("init/Home/Common/bt1_gold.png");
    auto change_on = Sprite::create("init/Home/Common/bt1_gold.png");
    change_on->setColor(Color3B(180, 180, 180));
    
    auto change_off_text = HMSFontUtil::createWithTTFBySystemFontArgument("変更する", FONT_NAME_SYSTEM, 28);
    change_off_text->enableOutline(Color4B(0, 0, 0, 255), 1);
    change_off_text->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    change_off_text->setPosition(Point(change_off->getContentSize() / 2));
    change_off->addChild(change_off_text);
    
    auto change_on_text = HMSFontUtil::createWithTTFBySystemFontArgument("変更する", FONT_NAME_SYSTEM, 28);
    change_on_text->enableOutline(Color4B(0, 0, 0, 255), 1);
    change_on_text->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    change_on_text->setPosition(Point(change_on->getContentSize() / 2));
    change_on->addChild(change_on_text, 11);
    
    auto change_btn = MenuItemSprite::create(change_off, change_on, CC_CALLBACK_0(CharaListSortPopupLayer::changeButtonAction, this));
    auto menu = Menu::create(change_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu);
}

void CharaListSortPopupLayer::setSelectButtonData()
{
    //選択ボタンの設定
    
    //絞り込みボタンの設定
    auto type_menu = (MenuItemToggle *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_Menu);
    for (int i = 0; i < type_menu->getChildrenCount(); i++)
    {
        //絞り込み条件に当てはまるものだけ選択状態にする
        if (_chara_type[i])
        {
            auto toggle = (MenuItemToggle *)type_menu->getChildByTag(i + 1);
            toggle->setSelectedIndex(1);
        }
    }
    
    //表示順選択ボタンの設定
    auto sort_menu = (MenuItemToggle *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Sort_Menu)->getChildByTag(_sort_status);
    if (sort_menu) {
        //ソート状態のボタンを選択状態にする
        sort_menu->setSelectedIndex(1);
    }
    else {
        //もしソート状態が設定されていなかった場合、戦闘力順を選択状態にする
        auto sort_menu = (MenuItemToggle *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Sort_Menu)->getChildByTag(SortStatus::Power);
        sort_menu->setSelectedIndex(1);
    }
}

void CharaListSortPopupLayer::setCharaTypeData(cocos2d::Ref *sender)
{
    //絞り込みボタンを押した時のデータの設定
    auto toggle = (MenuItemToggle *)sender;
    
    //トグルボタンの状態インデックスとタグを取得
    int index = toggle->getSelectedIndex();
    int tag   = toggle->getTag();
    
    //総大将選択ページの場合絞り込みを行わせないようにする
    if (_from_page_name == "GeneralFormation")
    {
        if (index != 0)
            toggle->setSelectedIndex(0);
        else
            toggle->setSelectedIndex(1);
        return;
    }
    
    //デッキ編成ページの場合、総大将ボタンは押せないようにする
    if (_from_page_name == "DeckFormation" && tag == CharaSprite::CharaType::GENERAL)
    {
        if (index != 0)
            toggle->setSelectedIndex(0);
        else
            toggle->setSelectedIndex(1);
        return;
    }
    
    switch (tag)
    {
        case CharaSprite::CharaType::OFFENSE:    _chara_type[0] = changeBoolean(_chara_type[0]); break;
        case CharaSprite::CharaType::DEFENSE:    _chara_type[1] = changeBoolean(_chara_type[1]); break;
        case CharaSprite::CharaType::STRATEGIST: _chara_type[2] = changeBoolean(_chara_type[2]); break;
        case CharaSprite::CharaType::GENERAL:    _chara_type[3] = changeBoolean(_chara_type[3]); break;
        default: break;
    }
    
    //ボタンが押されているかチェック
    checkSelected(tag);
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    //ログ出力
    for (int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        CCLOG("chara_type[%d] = %s", i, _chara_type[i] ? "true" : "false");
    }
}

bool CharaListSortPopupLayer::changeBoolean(bool status)
{
    //引数のboolと反対のboolを返す
    if (status) return false;
    else return true;
}

void CharaListSortPopupLayer::initCharaTypeData()
{
    //ソートに使うキャラタイプデータの初期化（全部選択状態にする）
    for (int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        _chara_type[i] = true;
    }
    
    //ボタンの初期化
    auto menu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_Menu);
    if (menu)
    {
        for (auto child : menu->getChildren())
        {
            auto toggle = (MenuItemToggle *)child;
            
            toggle->setSelectedIndex(1);
            
        }
    }
}

void CharaListSortPopupLayer::checkSelected(int tag)
{
    //全てのボタンが押されていない状態かチェック
    for (int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        if (_chara_type[i] == true) return;
    }
    
    //もし、押されていない場合選択したボタンを選択状態のままにしておく
    _chara_type[tag - 1] = true;
    auto btn = (MenuItemToggle *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_Menu)->getChildByTag(tag);
    btn->setSelectedIndex(1);
}

void CharaListSortPopupLayer::setSortStatusData(Ref* sender)
{
    //ソート条件のデータの設定
    auto toggle = (MenuItemToggle *)sender;
    int tag = toggle->getTag();
    
    //選択ボタン以外をOFFにする
    auto menu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Sort_Menu);
    if (menu)
    {
        for (auto child : menu->getChildren())
        {
            auto menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            
        }
    }
    
    //選択ボタンを押した状態にする
    toggle->setSelectedIndex(1);
    
    //データ設定
    switch (tag)
    {
        case Power:      SaveData::saveInt("sort_status", SortStatus::Power);      break;
        case Rare:       SaveData::saveInt("sort_status", SortStatus::Rare);       break;
        case NeedCost:   SaveData::saveInt("sort_status", SortStatus::NeedCost);   break;
        case InsertDate: SaveData::saveInt("sort_status", SortStatus::InsertDate); break;
        default: break;
    }
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    //ログ出力
    CCLOG("sort_status = %d", SaveData::loadInt("sort_status"));
}

void CharaListSortPopupLayer::changeButtonAction()
{
    //変更するボタンの処理
    if (_from_page_name == "CharaList")
    {
        auto layer = (CharaListLayer *)this->getParent();
        
        //リフレッシュ
        layer->refreshPage(_chara_type);
    }
    else if (_from_page_name == "CharaSellList")
    {
        auto layer = (CharaSellListLayer *)this->getParent();
        
        //リフレッシュ
        layer->refreshSortPage(_chara_type);
    }
    else if (_from_page_name == "GeneralFormation")
    {
        auto layer = (GeneralFormationLayer *)this->getParent();
        
        //リフレッシュ
        layer->refreshSortPage(_chara_type);
    }
    else if (_from_page_name == "DeckFormation")
    {
        auto layer = (DeckFormationLayer *)this->getParent();
        
        //リフレッシュ
        layer->refreshSortPage(_chara_type);
    }
    else if (_from_page_name == "BazaarExhibitCharaList")
    {
        auto layer = (BazaarExhibitCharaListLayer *)this->getParent();
        
        //リフレッシュ
        layer->refreshSortPage(_chara_type);
    }
    
    //レイヤーを閉じる
    Utility::closePopupAction(this);
}

bool CharaListSortPopupLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    HMSLOG("touchBegan CharaListSortPopupLayer!");
    
    return true;
}
void CharaListSortPopupLayer::onTouchMoved(Touch *touch, Event *unused_event)
{

}
void CharaListSortPopupLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
}

void CharaListSortPopupLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    
}

