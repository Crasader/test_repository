#include "BazaarSearchConditionPopupLayer.h"
#include "BazaarSearchListLayer.h"
#include "EditBoxUtil.h"
#include "BazaarSelectRarelityButton.h"
#include "BazaarSelectCostButton.h"

BazaarSearchConditionPopupLayer::BazaarSearchConditionPopupLayer()
:_close_func(nullptr)
{
}

BazaarSearchConditionPopupLayer* BazaarSearchConditionPopupLayer::create(BazaarSearchListLayer* layer)
{
    BazaarSearchConditionPopupLayer *pRet = new BazaarSearchConditionPopupLayer();
    
    if (pRet && pRet->init(layer))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool BazaarSearchConditionPopupLayer::init(BazaarSearchListLayer* layer)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarSearchConditionPopupLayer::init()");
    
    //初期データの設定
    SearchListLayer = layer;
    ratio = getVisibleRatio() ;
    
    return true;
}

void BazaarSearchConditionPopupLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    createBg();
    
    createInputName();
    
    createCharaTypeButton();
    createRarelitySelectBoxButton();
    createCostSelectBoxButton();
    setSelectButtonData();
    createSearchButton();
}


void BazaarSearchConditionPopupLayer::createBg()
{
    //背景画像
    background = Sprite::create("init/Home/Bazaar/bk_advanced_search.png");
    background->setScale(ratio);
    background->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(background, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    
    //閉じるボタン
    MenuItemImage* closeButton = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this](Ref* sender){
        
        if (_close_func != nullptr) _close_func();
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    Menu* menu = Menu::create(closeButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width - 20, background->getContentSize().height - 20));
    background->addChild(menu, ZOrder::Z_Close_Button, Tag::T_Close_Button);
}

void BazaarSearchConditionPopupLayer::createInputName(){
        //入力フォームの作成
        Size editBoxSize = Size(background->getContentSize().width-100, 70);
        ui::EditBox* pEditbox = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create("init/Home/Friend/entry_column.png"));
        pEditbox->setPosition(Point(background->getContentSize().width /2 , background->getContentSize().height - 100));
        pEditbox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
        pEditbox->setFontColor(Color3B(255, 255, 255));
        pEditbox->setFont(FONT_NAME_SYSTEM, 36);
        pEditbox->setDelegate(this);
        pEditbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        pEditbox->setTag(EDITBOX_TAG);
        if(SearchListLayer->getSortName() != ""){
            string name = SearchListLayer->getSortName();
            pEditbox->setText(name.c_str());
        }
        background->addChild(pEditbox, 1);

}

void BazaarSearchConditionPopupLayer::createCharaTypeButton(){
    //並び替えボタンの作成
    
    //攻撃カード
    MenuItemImage* offenseOff = MenuItemImage::create("init/CharaMenu/bt_order_offense_gray.png", "init/CharaMenu/bt_order_offense_gray.png");
    MenuItemImage* offenseOn = MenuItemImage::create("init/CharaMenu/bt_order_offense.png", "init/CharaMenu/bt_order_offense.png");
    MenuItemToggle* offenceButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(BazaarSearchConditionPopupLayer::setToggleCallBack, this), offenseOff, offenseOn, nullptr);
    offenceButton->setTag(CharaSprite::CharaType::OFFENSE);
    
    //防御カード
    MenuItemImage* defenseOff = MenuItemImage::create("init/CharaMenu/bt_order_defense_gray.png","init/CharaMenu/bt_order_defense_gray.png");
    MenuItemImage* defenseOn = MenuItemImage::create("init/CharaMenu/bt_order_defense.png", "init/CharaMenu/bt_order_defense.png");
    MenuItemToggle* defenseButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(BazaarSearchConditionPopupLayer::setToggleCallBack, this), defenseOff, defenseOn, nullptr);
    defenseButton->setTag(CharaSprite::CharaType::DEFENSE);
    
    //軍師カード
    MenuItemImage* strategistOff  = MenuItemImage::create("init/CharaMenu/bt_order_strategist_gray.png", "init/CharaMenu/bt_order_strategist_gray.png");
    MenuItemImage* strategistOn = MenuItemImage::create("init/CharaMenu/bt_order_strategist.png", "init/CharaMenu/bt_order_strategist.png");
    MenuItemToggle* strategistButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(BazaarSearchConditionPopupLayer::setToggleCallBack, this), strategistOff, strategistOn, nullptr);
    strategistButton->setTag(CharaSprite::CharaType::STRATEGIST);
    
    //将軍カード
    MenuItemImage* generalOff  = MenuItemImage::create("init/CharaMenu/bt_order_general_gray.png", "init/CharaMenu/bt_order_general_gray.png");
    MenuItemImage* generalOn = MenuItemImage::create("init/CharaMenu/bt_order_general.png", "init/CharaMenu/bt_order_general.png");
    MenuItemToggle* generalButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(BazaarSearchConditionPopupLayer::setToggleCallBack, this), generalOff, generalOn, nullptr);
    generalButton->setTag(CharaSprite::CharaType::GENERAL);
    
    //全部
    MenuItemImage* allOff  = MenuItemImage::create("init/CharaMenu/bt_order_all_gray2.png", "init/CharaMenu/bt_order_all_gray2.png");
    MenuItemImage* allOn = MenuItemImage::create("init/CharaMenu/bt_order_all.png", "init/CharaMenu/bt_order_all.png");
    MenuItemToggle* allButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(BazaarSearchConditionPopupLayer::setToggleCallBack, this), allOff, allOn, nullptr);
    allButton->setTag(CharaSprite::CharaType::ALL);
    
    Menu* menu = Menu::create(offenceButton, defenseButton, strategistButton, generalButton, allButton, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height - 365));
    background->addChild(menu, Z_Type_button, T_Type_button);

}






void BazaarSearchConditionPopupLayer::setToggleCallBack(Ref* sender){
    
    //絞り込みボタンを押した時のデータの設定
    MenuItemToggle* toggle = (MenuItemToggle *)sender;
    
    int tag = toggle->getTag();
    
    //選択ボタン以外をOFFにする
    Menu* menu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_button);
    if (menu)
    {
        for (auto child : menu->getChildren())
        {
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            
        }
    }
    
    //選択ボタンを押した状態にする
    toggle->setSelectedIndex(1);
    
    SearchListLayer->setSortCharaType(tag);
    
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    //ログ出力
    CCLOG("getSortCharaType = %d", SearchListLayer->getSortCharaType());
    
}



void BazaarSearchConditionPopupLayer::setSelectButtonData()
{
    
    //表示順選択ボタンの設定 TODO 保存した値から初期値を設定
    
    //選択ボタン以外をOFFにする
    Menu* menu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_button);
    if (menu){
        for (auto child : menu->getChildren()){
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            if(menuItemToggle->getTag() == SearchListLayer->getSortCharaType())menuItemToggle->setSelectedIndex(1);
        }
    }
    
}





bool BazaarSearchConditionPopupLayer::changeBoolean(bool status)
{
    //引数のboolと反対のboolを返す
    if (status) return false;
    else return true;
}


void BazaarSearchConditionPopupLayer::checkSelected(int tag)
{
    //全てのボタンが押されていない状態かチェック
    for (int i = 0; i < sizeof(charaType) / sizeof(charaType[0]); i++) {
        if (charaType[i] == true) return;
    }
    
    //もし、押されていない場合選択したボタンを選択状態のままにしておく
    charaType[tag - 1] = true;
    MenuItemToggle* btn = (MenuItemToggle *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_button)->getChildByTag(tag);
    btn->setSelectedIndex(1);
}


void BazaarSearchConditionPopupLayer::createRarelitySelectBoxButton(){
    //レア度選択
    BazaarSelectRarelityButton* selectRarelityButton = BazaarSelectRarelityButton::Create("init/Home/Bazaar/button/bt_advanced_search.png", "init/Home/Bazaar/button/bt_advanced_search.png", SearchListLayer);
    Menu* menu = Menu::create(selectRarelityButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width -450, background->getContentSize().height - 230));
    
    background->addChild(menu, Z_Reality_button, T_Reality_button);
    
}

void BazaarSearchConditionPopupLayer::createCostSelectBoxButton(){
    //更新
    BazaarSelectCostButton* selectCostButton = BazaarSelectCostButton::Create("init/Home/Bazaar/button/bt_advanced_search.png", "init/Home/Bazaar/button/bt_advanced_search.png", SearchListLayer);
    Menu* menu = Menu::create(selectCostButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width-165, background->getContentSize().height - 230));
    
    background->addChild(menu, Z_Reality_button, T_Reality_button);
}

void BazaarSearchConditionPopupLayer::createSearchButton(){
    Sprite* searchOff = Sprite::create("init/Home/Bazaar/button/bt_search_for.png");
    Sprite* searchOn  = Sprite::create("init/Home/Bazaar/button/bt_search_for.png");
    searchOn->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite* searchButton = MenuItemSprite::create(searchOff, searchOn,[this](Ref* sender){
        SearchListLayer->refreshList();
        
        Utility::closePopupAction(this);
    });
    
    Menu* menu = Menu::create(searchButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width/2, background->getContentSize().height -470));
    background->addChild(menu, Z_Refresh_Button, T_Refresh_Button);

    
}


void BazaarSearchConditionPopupLayer::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void BazaarSearchConditionPopupLayer::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    HMSLOG("editBegin!");
}
void BazaarSearchConditionPopupLayer::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    HMSLOG("editEnd!");
}
void BazaarSearchConditionPopupLayer::editBoxTextChanged(ui::EditBox *editBox, const std::string& text)
{
    //入力文字列を取得
    SearchListLayer->setSortName(editBox->getText());
    HMSLOG("editChanged! text = %s", editBox->getText());
}
void BazaarSearchConditionPopupLayer::editBoxReturn(ui::EditBox *editBox)
{
    HMSLOG("editReturn!");
}

