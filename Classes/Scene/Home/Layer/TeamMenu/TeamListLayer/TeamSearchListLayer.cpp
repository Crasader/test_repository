#include "TeamSearchListLayer.h"
#include "HeaderLayer.h"
#include "TeamSearchConditionPopupLayer.h"
#include "TeamSortConditionPopupLayer.h"
#include "TeamDetailSearchLayer.h"

TeamSearchListLayer* TeamSearchListLayer::create(){
    TeamSearchListLayer *layer = new TeamSearchListLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void TeamSearchListLayer::setHeaderSetting(){
    HeaderLayer::setupTitleBar("検索結果", "TeamNoBelong", this, HeaderLayer::TitleBarType::WIDTH_324);
    createButton();
}

void TeamSearchListLayer::setSpriteTouchAction(){
    setListTouchAction([this](Json teamData){
        TeamDetailSearchLayer* team_detail = TeamDetailSearchLayer::create(teamData);
        this->addChild(team_detail, 9999);
        HMSLOG("ここでPopupを出すのです");
    });
}

void TeamSearchListLayer::GetTeamListData(){
    string url = HOST;
    url += "team/team_search.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    //チーム名
    url += "&team_name=";
    url += URLEncode(searchTeamName);
    //チームタイプ
    url += "&team_type=";
    url += searchTeamType;
    //プレイタイプ
    url += "&play_type=";
    url += searchPlayType;
    //ソートタイプ
    url += "&sort_type=";
    url += sortType;
    auto selector = httpresponse_selector(TeamListLayerBase::httpCallBack);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamSearchListLayer::createButton(){
    Sprite* searchOn = Sprite::create("init/Team/management/search/bt_condition_search.png");
    Sprite* searchOff = Sprite::create("init/Team/management/search/bt_condition_search.png");
    searchOn->setColor(Color3B(180, 180, 180));
    MenuItemSprite* searchButton = MenuItemSprite::create(searchOff, searchOn, [this](Ref* sender){
        TeamSearchConditionPopupLayer* popup = TeamSearchConditionPopupLayer::create(this);
        popup->setPositionY(popup->getPositionY() - 28 * this->ratio);
        this->addChild(popup, Utility::Z_Popup_Action, Utility::T_Popup_Action);
        
    });
    searchButton->setScale(ratio);
    Menu* SearchMenu = Menu::create(searchButton, nullptr);
    SearchMenu->setPosition(convertFromDisplaySize(Point(570, 785), 0.5, 1));
    this->addChild(SearchMenu, Z_Button, T_Button);
    
    
    Sprite* sortOn = Sprite::create("init/Team/management/search/bt_display_change.png");
    Sprite* sortOff = Sprite::create("init/Team/management/search/bt_display_change.png");
    sortOn->setColor(Color3B(180, 180, 180));
    MenuItemSprite* sortButton = MenuItemSprite::create(sortOff, sortOn, [this](Ref* sender){
        TeamSortConditionPopupLayer* popup = TeamSortConditionPopupLayer::create(this);
        this->addChild(popup, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    });
    sortButton->setScale(ratio);
    Menu* sortMenu = Menu::create(sortButton, nullptr);
    sortMenu->setPosition(convertFromDisplaySize(Point(426, 785), 0.5, 1));
    this->addChild(sortMenu,Z_Button, T_Button);
    
    //表示順テキスト
    Label* titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("新着順", FONT_NAME_SYSTEM, 19);
    titleLabel->setScale(ratio);
    titleLabel->setPosition(convertFromDisplaySize(Point(426, 774), 0.5, 1));
    this->addChild(titleLabel, Z_Button_Text, T_Button_Text);
    
}

string TeamSearchListLayer::getSortConditionButtonText(){
    if(sortType == "a") return "新着順";
    else if(sortType == "b") return "人数多い順";
    else if(sortType == "c") return "人数少ない順";
    return "なし";
    
}

void TeamSearchListLayer::updateButtonText(){
    Label* buttonTextLabel = (Label*)this->getChildByTag(T_Button_Text);
    buttonTextLabel->setString(getSortConditionButtonText());
}