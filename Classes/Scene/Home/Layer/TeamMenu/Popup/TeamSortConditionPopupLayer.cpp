#include "TeamSortConditionPopupLayer.h"
#include "TeamSearchListLayer.h"

TeamSortConditionPopupLayer::TeamSortConditionPopupLayer()
:_close_func(nullptr)
{
}

TeamSortConditionPopupLayer* TeamSortConditionPopupLayer::create(TeamSearchListLayer* layer)
{
    TeamSortConditionPopupLayer *pRet = new TeamSortConditionPopupLayer();
    
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

bool TeamSortConditionPopupLayer::init(TeamSearchListLayer* layer)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamSortConditionPopupLayer::init()");
    
    //初期データの設定
    teamSearchListLayer = layer;
    ratio = getVisibleRatio() ;
    
    return true;
}

void TeamSortConditionPopupLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    createBg();
    createSortConditionButton();
    setSelectButtonData();
    createSearchButton();
}

void TeamSortConditionPopupLayer::createBg()
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
    background->addChild(menu, Utility::Z_Popup_Action+3, Tag::T_Close_Button);
}

void TeamSortConditionPopupLayer::createSortConditionButton(){
    
    //新着
    MenuItemImage* sortTypeAOff = MenuItemImage::create("init/Team/management/search/bt_sort_gray1.png", "init/Team/management/search/bt_sort_gray1.png");
    MenuItemImage* sortTypeAOn = MenuItemImage::create("init/Team/management/search/bt_sort1.png", "init/Team/management/search/bt_sort1.png");
    MenuItemToggle* sortTypeAButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSortConditionPopupLayer::setToggleSortConditionCallBack, this),
sortTypeAOff, sortTypeAOn, nullptr);
    sortTypeAButton->setTag(1);
    
    //人数多い順
    MenuItemImage* sortTypeBOff = MenuItemImage::create("init/Team/management/search/bt_sort_gray2.png", "init/Team/management/search/bt_sort_gray2.png");
    MenuItemImage* sortTypeBOn = MenuItemImage::create("init/Team/management/search/bt_sort2.png", "init/Team/management/search/bt_sort2.png");
    MenuItemToggle* sortTypeBButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSortConditionPopupLayer::setToggleSortConditionCallBack, this), sortTypeBOff, sortTypeBOn, nullptr);
    sortTypeBButton->setTag(2);
    
    //人数少ない順
    MenuItemImage* sortTypeCOff = MenuItemImage::create("init/Team/management/search/bt_sort_gray3.png", "init/Team/management/search/bt_sort_gray3.png");
    MenuItemImage* sortTypeCOn = MenuItemImage::create("init/Team/management/search/bt_sort3.png", "init/Team/management/search/bt_sort3.png");
    MenuItemToggle* sortTypeCButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSortConditionPopupLayer::setToggleSortConditionCallBack, this), sortTypeCOff, sortTypeCOn, nullptr);
    sortTypeCButton->setTag(3);
    
    Menu* menu = Menu::create(sortTypeAButton, sortTypeBButton, sortTypeCButton, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height - 120));
    background->addChild(menu, Z_PLAY_TYPE, T_PLAY_TYPE);
}


void TeamSortConditionPopupLayer::setToggleSortConditionCallBack(Ref* sender){
    
    //絞り込みボタンを押した時のデータの設定
    MenuItemToggle* toggle = (MenuItemToggle *)sender;
    
    int tag = toggle->getTag();
    bool isSelected = false;
    if(tag == 1 && teamSearchListLayer->sortType == "a")isSelected = true;
    else if(tag == 2 && teamSearchListLayer->sortType == "b")isSelected = true;
    else if(tag == 3 && teamSearchListLayer->sortType == "c")isSelected = true;
    
    //選択ボタン以外をOFFにする
    Menu* menu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_PLAY_TYPE);
    if (menu)
    {
        for (auto child : menu->getChildren())
        {
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            
        }
    }
    
    //選択ボタンを押した状態にする
    if(!isSelected)toggle->setSelectedIndex(1);
    
    if(isSelected)teamSearchListLayer->sortType = "";
    else if(tag == 1)teamSearchListLayer->sortType = "a";
    else if(tag == 2)teamSearchListLayer->sortType = "b";
    else if(tag == 3)teamSearchListLayer->sortType = "c";
    
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
}

void TeamSortConditionPopupLayer::setSelectButtonData()
{
    
    //表示順選択ボタンの設定 TODO 保存した値から初期値を設定
    
    //選択ボタン以外をOFFにする
    Menu* sortTypeMenu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_PLAY_TYPE);
    int selectedTagNumberSortType = 0;
    if(teamSearchListLayer->sortType == "a") selectedTagNumberSortType = 1;
    else if(teamSearchListLayer->sortType == "b") selectedTagNumberSortType = 2;
    else if(teamSearchListLayer->sortType == "c") selectedTagNumberSortType = 3;
    

    
    if (sortTypeMenu){
        for (auto child : sortTypeMenu->getChildren()){
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            if(menuItemToggle->getTag() == selectedTagNumberSortType)menuItemToggle->setSelectedIndex(1);
        }
    }
    
    
}


void TeamSortConditionPopupLayer::createSearchButton(){
    Sprite* searchOff = Sprite::create("init/Team/management/search/bt_sort.png");
    Sprite* searchOn  = Sprite::create("init/Team/management/search/bt_sort.png");
    searchOn->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite* searchButton = MenuItemSprite::create(searchOff, searchOn,[this](Ref* sender){
        teamSearchListLayer->refreshList();
        teamSearchListLayer->updateButtonText();
        Utility::closePopupAction(this);
    });
    
    Menu* menu = Menu::create(searchButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width/2, background->getContentSize().height - 250));
    background->addChild(menu, Z_Refresh_Button, T_Refresh_Button);
}

void TeamSortConditionPopupLayer::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
