#include "TeamBattleEventTopLayer.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "HeaderLayer.h"


TeamBattleEventTopLayer* TeamBattleEventTopLayer::create(){
    TeamBattleEventTopLayer *layer = new TeamBattleEventTopLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void TeamBattleEventTopLayer::createButton()
{
    //チーム対抗戦一覧へのリンク
    auto entry_list_btn_on = Sprite::create("init/Home/Event/TournamentEvent/bt_tournament_list.png");
    auto entry_list_btn_off = Sprite::create("init/Home/Event/TournamentEvent/bt_tournament_list.png");
    entry_list_btn_on->setColor(Color3B(180, 180, 180));
    auto entry_list_btn = MenuItemSprite::create(entry_list_btn_off, entry_list_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent()->getParent();
        normal_scene->changePage("TeamBattleEventEntryList");
    });
    entry_list_btn->setScale(ratio);
    auto entry_list_menu = Menu::create(entry_list_btn, nullptr);
    entry_list_menu->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(entry_list_menu, Z_Button);
}

void TeamBattleEventTopLayer::createPage()
{
    
}