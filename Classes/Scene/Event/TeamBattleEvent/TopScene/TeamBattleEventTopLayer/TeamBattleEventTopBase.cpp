#include "TeamBattleEventTopBase.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HttpConnect.h"
#include "HeaderLayer.h"

bool TeamBattleEventTopBase::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamBattleEventTopBase::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamBattleEventTopBase::onEnter()
{
    Layer::onEnter();
}

void TeamBattleEventTopBase::createDisplay(json11::Json json_data)
{
    team_battle_data = json_data;
    
    //データ保存
    saveJsonData();
    
    //画面作成
    createPage();
    
    createButton();
    
    createHelpButton();
    
    createEventItem();
}

void TeamBattleEventTopBase::saveJsonData()
{
    team_id = fromString<int>(team_battle_data["team_id"].string_value());
    event_item_count = team_battle_data["event_item_count"].int_value();
    is_leader = team_battle_data["is_leader"].bool_value();
    is_entry_time = team_battle_data["is_entry_time"].bool_value();
    team_battle_list_data = team_battle_data["team_battle_event_list_data"];
    team_battle_event_list_id = fromString<int>(team_battle_data["team_battle_event_list_data"]["team_battle_event_list_id"].string_value());
    SaveData::saveString("team_battle_event_list_id", toString(team_battle_event_list_id));
    SaveData::saveInt("team_battle_event_item", event_item_count);
}

void TeamBattleEventTopBase::createEventItem()
{
    //所持イベントアイテム数表示
    auto bg = Sprite::create("init/CharaMenu/revision/chara_exchange/ticket_bk.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 1);
    
    string team_battle_event_item = SaveData::loadString("team_battle_event_item");
    auto team_battle_event_item_count = HMSFontUtil::createWithTTFBySystemFontArgument(team_battle_event_item, FONT_NAME_SYSTEM , Utility::calcFlexibleFontSize(team_battle_event_item, 36, 100));
    team_battle_event_item_count->setAnchorPoint(Point(1.0, 0.5));
    team_battle_event_item_count->setPosition(Point(bg->getContentSize().width - 20, bg->getContentSize().height / 2));
    bg->addChild(team_battle_event_item_count, 1);
}

void TeamBattleEventTopBase::createHelpButton()
{
    //ヘルプ
    auto help_btn_on = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent()->getParent();
        
        string url = HOST;
        url += "user/index.php?&p=page__team_battle_event_help&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "チーム対抗戦ヘルプ");
        normal_scene->addChild(WebViewLoadLayer, 9);
        
    });
    help_btn->setScale(ratio);
    auto help_menu = Menu::create(help_btn, nullptr);
    help_menu->setPosition(convertFromDisplaySize(Point(570, 789), 0.5, 1));
    this->addChild(help_menu, Z_Button);
}

void TeamBattleEventTopBase::createPage()
{
}

void TeamBattleEventTopBase::createButton()
{
}