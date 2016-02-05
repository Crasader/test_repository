#include "TeamBattleEventDeckSelectLayer.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "HeaderLayer.h"

bool TeamBattleEventDeckSelectLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamBattleEventDeckSelectLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamBattleEventDeckSelectLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("デッキ・タイプ選択", "TeamBattleEventTop", this);
    
    //画面生成
    createButton();
    
    //タイプ生成
    createType();
    
    //デッキ生成
    createDeck();
}

void TeamBattleEventDeckSelectLayer::createButton()
{
    //決定ボタン
    auto ok_btn_on = Sprite::create("init/CharaMenu/revision/bt_decision2.png");
    auto ok_btn_off = Sprite::create("init/CharaMenu/revision/bt_decision2.png");
    ok_btn_on->setColor(Color3B(180, 180, 180));
    auto ok_btn = MenuItemSprite::create(ok_btn_off, ok_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("TeamBattleEventTop");
    });
    ok_btn->setScale(ratio);
    auto ok_menu = Menu::create(ok_btn, nullptr);
    ok_menu->setPosition(convertFromDisplaySize(Point(320, 200), 0.5, 0.5));
    this->addChild(ok_menu, Z_Button);
}

void TeamBattleEventDeckSelectLayer::createType()
{
    
}

void TeamBattleEventDeckSelectLayer::createDeck()
{
    //デッキレイヤー生成
    deck_layer = DeckChoiceBaseLayer::create(480.0f, 1.0f, true);
    this->addChild(deck_layer, Z_Deck, T_Deck);
}