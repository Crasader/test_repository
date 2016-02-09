#include "TeamSearchConditionPopupLayer.h"
#include "TeamSearchListLayer.h"

TeamSearchConditionPopupLayer::TeamSearchConditionPopupLayer()
:_close_func(nullptr)
{
}

TeamSearchConditionPopupLayer* TeamSearchConditionPopupLayer::create(TeamSearchListLayer* layer)
{
    TeamSearchConditionPopupLayer *pRet = new TeamSearchConditionPopupLayer();
    
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

bool TeamSearchConditionPopupLayer::init(TeamSearchListLayer* layer)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamSearchConditionPopupLayer::init()");
    
    //初期データの設定
    teamSearchListLayer = layer;
    ratio = getVisibleRatio() ;
    
    return true;
}

void TeamSearchConditionPopupLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    createBg();
    
    createInputName();
    createTeamTypeButton();
    createPlayTypeButton();
    setSelectButtonData();
    createSearchButton();
}

void TeamSearchConditionPopupLayer::createBg()
{
    //背景画像
    background = Sprite::create("init/Team/management/search/popup_team_type.png");
    background->setScale(ratio);
    background->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(background, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
    //ページのタイトル画像
    Sprite* _title_bg = Sprite::create("init/Home/Popup/popup_index_s.png");
    _title_bg->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height + 18));
    background->addChild(_title_bg, Utility::Z_Popup_Action+1);
    
    //ページタイトル
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("条件検索", FONT_NAME_SYSTEM, 30);
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

void TeamSearchConditionPopupLayer::createInputName(){
    //入力フォームの作成
    Size editBoxSize = Size(background->getContentSize().width-100, 70);
    EditBox* pEditbox = EditBox::create(editBoxSize, Scale9Sprite::create("init/Home/Friend/entry_column.png"));
    pEditbox->setPosition(Point(background->getContentSize().width /2 , background->getContentSize().height - 100));
    pEditbox->setInputMode(EditBox::InputMode::SINGLE_LINE);
    pEditbox->setFontColor(Color3B(255, 255, 255));
    pEditbox->setFont(FONT_NAME_SYSTEM, 36);
    pEditbox->setDelegate(this);
    pEditbox->setReturnType(EditBox::KeyboardReturnType::DONE);
    pEditbox->setTag(T_Search_name_box);
    if(teamSearchListLayer->searchTeamName != ""){
        string name = teamSearchListLayer->searchTeamName;
        pEditbox->setText(name.c_str());
    }
    background->addChild(pEditbox, 1);
}

void TeamSearchConditionPopupLayer::createTeamTypeButton(){
    
    //初心者歓迎
    MenuItemImage* teamTypeAOff = MenuItemImage::create("init/Team/management/info/bt_guild_type_a_off.png", "init/Team/management/info/bt_guild_type_a_off.png");
    MenuItemImage* teamTypeAOn = MenuItemImage::create("init/Team/management/info/bt_guild_type_a_on.png", "init/Team/management/info/bt_guild_type_a_on.png");
    MenuItemToggle* teamTypeAButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSearchConditionPopupLayer::setToggleTeamTypeCallBack, this), teamTypeAOff, teamTypeAOn, nullptr);
    teamTypeAButton->setTag(1);
    
    //中級者限定
    MenuItemImage* teamTypeBOff = MenuItemImage::create("init/Team/management/info/bt_guild_type_b_off.png", "init/Team/management/info/bt_guild_type_b_off.png");
    MenuItemImage* teamTypeBOn = MenuItemImage::create("init/Team/management/info/bt_guild_type_b_on.png", "init/Team/management/info/bt_guild_type_b_on.png");
    MenuItemToggle* teamTypeBButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSearchConditionPopupLayer::setToggleTeamTypeCallBack, this), teamTypeBOff, teamTypeBOn, nullptr);
    teamTypeBButton->setTag(2);
    
    //上級者求む
    MenuItemImage* teamTypeCOff = MenuItemImage::create("init/Team/management/info/bt_guild_type_c_off.png", "init/Team/management/info/bt_guild_type_c_off.png");
    MenuItemImage* teamTypeCOn = MenuItemImage::create("init/Team/management/info/bt_guild_type_c_on.png", "init/Team/management/info/bt_guild_type_c_on.png");
    MenuItemToggle* teamTypeCButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSearchConditionPopupLayer::setToggleTeamTypeCallBack, this), teamTypeCOff, teamTypeCOn, nullptr);
    teamTypeCButton->setTag(3);
    
    Menu* menu = Menu::create(teamTypeAButton, teamTypeBButton, teamTypeCButton, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height - 335));
    background->addChild(menu, Z_TEAM_TYPE, T_TEAM_TYPE);
    

    
}


void TeamSearchConditionPopupLayer::setToggleTeamTypeCallBack(Ref* sender){
    
    //絞り込みボタンを押した時のデータの設定
    MenuItemToggle* toggle = (MenuItemToggle *)sender;
    
    int tag = toggle->getTag();
    bool isSelected = false;
    if(tag == 1 && teamSearchListLayer->searchTeamType == "a")isSelected = true;
    else if(tag == 2 && teamSearchListLayer->searchTeamType == "b")isSelected = true;
    else if(tag == 3 && teamSearchListLayer->searchTeamType == "c")isSelected = true;
    
    //選択ボタン以外をOFFにする
    Menu* menu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_TEAM_TYPE);
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
    
    if(isSelected)teamSearchListLayer->searchTeamType = "";
    else if(tag == 1)teamSearchListLayer->searchTeamType = "a";
    else if(tag == 2)teamSearchListLayer->searchTeamType = "b";
    else if(tag == 3)teamSearchListLayer->searchTeamType = "c";
    
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
}

void TeamSearchConditionPopupLayer::createPlayTypeButton(){
    
    //まったり
    MenuItemImage* playTypeAOff = MenuItemImage::create("init/Team/management/info/bt_play_type_a_off.png", "init/Team/management/info/bt_play_type_a_off.png");
    MenuItemImage* playTypeAOn = MenuItemImage::create("init/Team/management/info/bt_play_type_a_on.png", "init/Team/management/info/bt_play_type_a_on.png");
    MenuItemToggle* playTypeAButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSearchConditionPopupLayer::setTogglePlayTypeCallBack, this), playTypeAOff, playTypeAOn, nullptr);
    playTypeAButton->setTag(1);
    
    //ほどほど
    MenuItemImage* playTypeBOff = MenuItemImage::create("init/Team/management/info/bt_play_type_b_off.png", "init/Team/management/info/bt_play_type_b_off.png");
    MenuItemImage* playTypeBOn = MenuItemImage::create("init/Team/management/info/bt_play_type_b_on.png", "init/Team/management/info/bt_play_type_b_on.png");
    MenuItemToggle* playTypeBButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSearchConditionPopupLayer::setTogglePlayTypeCallBack, this), playTypeBOff, playTypeBOn, nullptr);
    playTypeBButton->setTag(2);
    
    //がっつり
    MenuItemImage* playTypeCOff = MenuItemImage::create("init/Team/management/info/bt_play_type_c_off.png", "init/Team/management/info/bt_play_type_c_off.png");
    MenuItemImage* playTypeCOn = MenuItemImage::create("init/Team/management/info/bt_play_type_c_on.png", "init/Team/management/info/bt_play_type_c_on.png");
    MenuItemToggle* playTypeCButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(TeamSearchConditionPopupLayer::setTogglePlayTypeCallBack, this), playTypeCOff, playTypeCOn, nullptr);
    playTypeCButton->setTag(3);
    
    Menu* menu = Menu::create(playTypeAButton, playTypeBButton, playTypeCButton, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(Point(background->getContentSize().width / 2, background->getContentSize().height - 220));
    background->addChild(menu, Z_PLAY_TYPE, T_PLAY_TYPE);
}


void TeamSearchConditionPopupLayer::setTogglePlayTypeCallBack(Ref* sender){
    
    //絞り込みボタンを押した時のデータの設定
    MenuItemToggle* toggle = (MenuItemToggle *)sender;
    
    int tag = toggle->getTag();
    bool isSelected = false;
    if(tag == 1 && teamSearchListLayer->searchPlayType == "a")isSelected = true;
    else if(tag == 2 && teamSearchListLayer->searchPlayType == "b")isSelected = true;
    else if(tag == 3 && teamSearchListLayer->searchPlayType == "c")isSelected = true;
    
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
    
    if(isSelected)teamSearchListLayer->searchPlayType = "";
    else if(tag == 1)teamSearchListLayer->searchPlayType = "a";
    else if(tag == 2)teamSearchListLayer->searchPlayType = "b";
    else if(tag == 3)teamSearchListLayer->searchPlayType = "c";
    
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
}

void TeamSearchConditionPopupLayer::setSelectButtonData()
{
    
    //表示順選択ボタンの設定 TODO 保存した値から初期値を設定
    
    //選択ボタン以外をOFFにする
    Menu* teamTypeMenu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_TEAM_TYPE);
    int selectedTagNumberTeamType = 0;
    if(teamSearchListLayer->searchTeamType == "a") selectedTagNumberTeamType = 1;
    else if(teamSearchListLayer->searchTeamType == "b") selectedTagNumberTeamType = 2;
    else if(teamSearchListLayer->searchTeamType == "c") selectedTagNumberTeamType = 3;
    
    Menu* playTypeMenu = (Menu *)this->getChildByTag(Utility::T_Popup_Action)->getChildByTag(T_PLAY_TYPE);
    int selectedTagNumberPlayType = 0;
    if(teamSearchListLayer->searchPlayType == "a") selectedTagNumberPlayType = 1;
    else if(teamSearchListLayer->searchPlayType == "b") selectedTagNumberPlayType = 2;
    else if(teamSearchListLayer->searchPlayType == "c") selectedTagNumberPlayType = 3;
    
    if (teamTypeMenu){
        for (auto child : teamTypeMenu->getChildren()){
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            if(menuItemToggle->getTag() == selectedTagNumberTeamType)menuItemToggle->setSelectedIndex(1);
        }
    }
    
    if (playTypeMenu){
        for (auto child : playTypeMenu->getChildren()){
            MenuItemToggle * menuItemToggle = (MenuItemToggle *)child;
            menuItemToggle->setSelectedIndex(0);
            if(menuItemToggle->getTag() == selectedTagNumberPlayType)menuItemToggle->setSelectedIndex(1);
        }
    }
    
    
}


void TeamSearchConditionPopupLayer::createSearchButton(){
    Sprite* searchOff = Sprite::create("init/Team/management/search/bt_search.png");
    Sprite* searchOn  = Sprite::create("init/Team/management/search/bt_search.png");
    searchOn->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite* searchButton = MenuItemSprite::create(searchOff, searchOn,[this](Ref* sender){
        teamSearchListLayer->refreshList();
        
        Utility::closePopupAction(this);
    });
    
    Menu* menu = Menu::create(searchButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width/2, background->getContentSize().height - 425));
    background->addChild(menu, Z_Refresh_Button, T_Refresh_Button);
}

void TeamSearchConditionPopupLayer::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TeamSearchConditionPopupLayer::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    HMSLOG("editBegin!");
}
void TeamSearchConditionPopupLayer::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    HMSLOG("editEnd!");
}
void TeamSearchConditionPopupLayer::editBoxTextChanged(ui::EditBox *editBox, const std::string& text)
{
    //名前検索
        //入力文字列を取得
    teamSearchListLayer->searchTeamName = editBox->getText();
    HMSLOG("editChanged! search_name = %s", editBox->getText());
    
}
void TeamSearchConditionPopupLayer::editBoxReturn(ui::EditBox *editBox)
{
    HMSLOG("editReturn!");
}
