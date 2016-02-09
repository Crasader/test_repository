#include "TeamMemberSearchConditionPopupLayer.h"
#include "TeamMemberListLayer.h"
#include "EditBoxUtil.h"

TeamMemberSearchConditionPopupLayer::TeamMemberSearchConditionPopupLayer()
:_close_func(nullptr)
{
}

TeamMemberSearchConditionPopupLayer* TeamMemberSearchConditionPopupLayer::create(TeamMemberListLayer* layer)
{
    TeamMemberSearchConditionPopupLayer *pRet = new TeamMemberSearchConditionPopupLayer();
    
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

bool TeamMemberSearchConditionPopupLayer::init(TeamMemberListLayer* layer)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamMemberSearchConditionPopupLayer::init()");
    
    //初期データの設定
    MemberListLayer = layer;
    ratio = getVisibleRatio() ;
    
    return true;
}

void TeamMemberSearchConditionPopupLayer::onEnter()
{
    Layer::onEnter() ;
    
    //イベントリスナーの設定
    initEventListener() ;
    
    createBg();
    
    createInputName();
    createInputId();
    
    createSearchButton();
}

void TeamMemberSearchConditionPopupLayer::createBg()
{
    //背景画像
    background = Sprite::create("init/Team/management/search/popup_team_search.png");
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

void TeamMemberSearchConditionPopupLayer::createInputName(){
        //入力フォームの作成
        Size editBoxSize = Size(background->getContentSize().width-100, 70);
        EditBox* pEditbox = EditBox::create(editBoxSize, Scale9Sprite::create("init/Home/Friend/entry_column.png"));
        pEditbox->setPosition(Point(background->getContentSize().width /2 , background->getContentSize().height - 242));
        pEditbox->setInputMode(EditBox::InputMode::SINGLE_LINE);
        pEditbox->setFontColor(Color3B(255, 255, 255));
        pEditbox->setFont(FONT_NAME_SYSTEM, 36);
        pEditbox->setDelegate(this);
        pEditbox->setReturnType(EditBox::KeyboardReturnType::DONE);
        pEditbox->setTag(T_Search_name_box);
        if(MemberListLayer->getSearchUserName() != ""){
            string name = MemberListLayer->getSearchUserName();
            pEditbox->setText(name.c_str());
        }
        background->addChild(pEditbox, 1);
}

void TeamMemberSearchConditionPopupLayer::createInputId(){
        //入力フォームの作成
        Size editBoxSize = Size(background->getContentSize().width-100, 70);
        EditBox* pEditbox = EditBox::create(editBoxSize, Scale9Sprite::create("init/Home/Friend/entry_column.png"));
        pEditbox->setPosition(Point(background->getContentSize().width /2 , background->getContentSize().height - 118));
        pEditbox->setInputMode(EditBox::InputMode::SINGLE_LINE);
        pEditbox->setFontColor(Color3B(255, 255, 255));
        pEditbox->setFont(FONT_NAME_SYSTEM, 36);
        pEditbox->setDelegate(this);
        pEditbox->setReturnType(EditBox::KeyboardReturnType::DONE);
        pEditbox->setTag(T_Search_id_box);
        if(MemberListLayer->getSearchId() != ""){
            string name = MemberListLayer->getSearchId();
            pEditbox->setText(name.c_str());
        }
        background->addChild(pEditbox, 1);
}

void TeamMemberSearchConditionPopupLayer::createSearchButton(){
    Sprite* searchOff = Sprite::create("init/Team/management/search/bt_search.png");
    Sprite* searchOn  = Sprite::create("init/Team/management/search/bt_search.png");
    searchOn->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite* searchButton = MenuItemSprite::create(searchOff, searchOn,[this](Ref* sender){
        MemberListLayer->refreshList();
        
        Utility::closePopupAction(this);
    });
    
    Menu* menu = Menu::create(searchButton, nullptr);
    menu->setPosition(Point(background->getContentSize().width/2, background->getContentSize().height - 358));
    background->addChild(menu, Z_Refresh_Button, T_Refresh_Button);
}

void TeamMemberSearchConditionPopupLayer::initEventListener()
{
    //タップイベントの登録
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TeamMemberSearchConditionPopupLayer::editBoxEditingDidBegin(ui::EditBox *editBox)
{
    HMSLOG("editBegin!");
}
void TeamMemberSearchConditionPopupLayer::editBoxEditingDidEnd(ui::EditBox *editBox)
{
    HMSLOG("editEnd!");
}
void TeamMemberSearchConditionPopupLayer::editBoxTextChanged(ui::EditBox *editBox, const std::string& text)
{
    //ID検索
    if (editBox->getTag() == T_Search_id_box){
        //入力文字列を取得
        MemberListLayer->setSearchId(editBox->getText());
        HMSLOG("editChanged! search_id = %s", editBox->getText());
    }
    //名前検索
    else if (editBox->getTag() == T_Search_name_box){
        //入力文字列を取得
        MemberListLayer->setSearchUserName(editBox->getText());
        HMSLOG("editChanged! search_name = %s", editBox->getText());
    }
}
void TeamMemberSearchConditionPopupLayer::editBoxReturn(ui::EditBox *editBox)
{
    HMSLOG("editReturn!");
}
