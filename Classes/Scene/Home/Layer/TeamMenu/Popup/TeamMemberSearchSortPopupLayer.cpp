#include "TeamMemberSearchSortPopupLayer.h"
#include "TeamMemberListLayer.h"
#include "EditBoxUtil.h"

TeamMemberSearchSortPopupLayer::TeamMemberSearchSortPopupLayer()
:_close_func(nullptr)
{
}

TeamMemberSearchSortPopupLayer* TeamMemberSearchSortPopupLayer::create(TeamMemberListLayer* layer)
{
    TeamMemberSearchSortPopupLayer *pRet = new TeamMemberSearchSortPopupLayer();
    
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

bool TeamMemberSearchSortPopupLayer::init(TeamMemberListLayer* layer)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamMemberSearchSortPopupLayer::init()");
    
    //初期データの設定
    MemberListLayer = layer;
    ratio = getVisibleRatio();
    
    return true;
}

void TeamMemberSearchSortPopupLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    createBg();
    createSearchTypeButton();
    
    createSearchButton();
    setSelectButtonData();
}

void TeamMemberSearchSortPopupLayer::createBg()
{
    //背景画像
    background = Sprite::create("init/Team/management/search/popup_sort.png");
    background->setScale(ratio);
    background->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(background, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    //ページのタイトル画像
    Sprite* _title_bg = Sprite::create("init/Home/Popup/popup_index_s.png");
    _title_bg->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height + 18));
    background->addChild(_title_bg, Utility::Z_Popup_Action+1);
    
    //ページタイトル
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("表示順変更", FONT_NAME_SYSTEM, 30);
    title_label->setPosition(_title_bg->getContentSize() / 2);
    _title_bg->addChild(title_label, Utility::Z_Popup_Action+2);
    
    //閉じるボタン
    MenuItemImage* closeButton = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close.png", [this](Ref* sender){
        
        if (_close_func != nullptr) _close_func();
        
        //閉じるアクション
        Utility::closePopupAction(this);
    });
    Menu* menu = Menu::create(closeButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width - 96, background->getContentSize().height + 18));
    background->addChild(menu, Utility::Z_Popup_Action+3);
}

void TeamMemberSearchSortPopupLayer::createSearchButton(){
    Sprite* searchOff = Sprite::create("init/Team/management/search/bt_sort.png");
    Sprite* searchOn  = Sprite::create("init/Team/management/search/bt_sort.png");
    searchOn->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite* searchButton = MenuItemSprite::create(searchOff, searchOn,[this](Ref* sender){
        MemberListLayer->refreshList();
        
        Utility::closePopupAction(this);
    });
    
    Menu* menu = Menu::create(searchButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width/2, background->getContentSize().height - 250));
    background->addChild(menu, Z_Refresh_Button, T_Refresh_Button);
}

void TeamMemberSearchSortPopupLayer::createSearchTypeButton(){
    //並び替えボタンの作成
    
    //入会日時順
    MenuItemImage* offenseOff = MenuItemImage::create("init/Team/management/search/bt_sort_gray4.png", "init/Team/management/search/bt_sort_gray4.png");
    MenuItemImage* offenseOn = MenuItemImage::create("init/Team/management/search/bt_sort4.png", "init/Team/management/search/bt_sort4.png");
    MenuItemToggle* offenceButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamMemberSearchSortPopupLayer::setToggleCallBack, this), offenseOff, offenseOn, nullptr);
    offenceButton->setTag(TeamMemberListLayer::SearchTag::NewList);
    
    //最終ログイン順
    MenuItemImage* defenseOff = MenuItemImage::create("init/Team/management/search/bt_sort_gray5.png","init/Team/management/search/bt_sort_gray5.png");
    MenuItemImage* defenseOn = MenuItemImage::create("init/Team/management/search/bt_sort5.png", "init/Team/management/search/bt_sort5.png");
    MenuItemToggle* defenseButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamMemberSearchSortPopupLayer::setToggleCallBack, this), defenseOff, defenseOn, nullptr);
    defenseButton->setTag(TeamMemberListLayer::SearchTag::LoginList);
    
    //Lv順
    MenuItemImage* strategistOff  = MenuItemImage::create("init/Team/management/search/bt_sort_gray6.png", "init/Team/management/search/bt_sort_gray6.png");
    MenuItemImage* strategistOn = MenuItemImage::create("init/Team/management/search/bt_sort6.png", "init/Team/management/search/bt_sort6.png");
    MenuItemToggle* strategistButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamMemberSearchSortPopupLayer::setToggleCallBack, this), strategistOff, strategistOn, nullptr);
    strategistButton->setTag(TeamMemberListLayer::SearchTag::LevelList);
    
    //Rank順
    MenuItemImage* generalOff  = MenuItemImage::create("init/Team/management/search/bt_sort_gray7.png", "init/Team/management/search/bt_sort_gray7.png");
    MenuItemImage* generalOn = MenuItemImage::create("init/Team/management/search/bt_sort7.png", "init/Team/management/search/bt_sort7.png");
    MenuItemToggle* generalButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamMemberSearchSortPopupLayer::setToggleCallBack, this), generalOff, generalOn, nullptr);
    generalButton->setTag(TeamMemberListLayer::SearchTag::RankList);
    
    Menu* menu = Menu::create(offenceButton, defenseButton, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height - 70));
    background->addChild(menu, Z_Type_button, T_Type_button);
    
    Menu* menu2 = Menu::create(strategistButton, generalButton, nullptr);
    menu2->alignItemsHorizontallyWithPadding(0);
    menu2->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height - 162));
    background->addChild(menu2, Z_Type_button2, T_Type_button2);
}

void TeamMemberSearchSortPopupLayer::setToggleCallBack(Ref* sender){
    
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
    Menu* menu2 = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_button2);
    if (menu2)
    {
        for (auto child : menu2->getChildren())
        {
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            
        }
    }
    
    //選択ボタンを押した状態にする
    toggle->setSelectedIndex(1);
    
    MemberListLayer->setSortName(tag);
    
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    //ログ出力
    CCLOG("getSortCharaType = %s", toString(MemberListLayer->getSortName()).c_str());
    
}

void TeamMemberSearchSortPopupLayer::setSelectButtonData()
{
    
    //表示順選択ボタンの設定 TODO 保存した値から初期値を設定
    
    //選択ボタン以外をOFFにする
    Menu* menu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_button);
    if (menu){
        for (auto child : menu->getChildren()){
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            if(menuItemToggle->getTag() == MemberListLayer->getSortName())menuItemToggle->setSelectedIndex(1);
        }
    }
    Menu* menu2 = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_Type_button2);
    if (menu2){
        for (auto child : menu2->getChildren()){
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            if(menuItemToggle->getTag() == MemberListLayer->getSortName())menuItemToggle->setSelectedIndex(1);
        }
    }
    
    
}



void TeamMemberSearchSortPopupLayer::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
