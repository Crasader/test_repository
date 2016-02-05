#include "TeamBattleEventLayer.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "json11.hpp"
#include "HeaderLayer.h"
#include "TeamBattleEventTopLayer.h"
#include "TeamBattleEventEntryTopLayer.h"

bool TeamBattleEventLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamBattleEventLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamBattleEventLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チーム対抗戦", "TeamTop", this);
    
    eventCheck();
}

void TeamBattleEventLayer::eventCheck()
{
    //イベント開催＆エントリー有無チェック
    string url = HOST;
    url += "event/team_battle_event/top_scene/get_user_entry_team_battle_info.php";
    string body = "user_master_id=" + SaveData::loadString("user_master_id");
    auto selector = httpresponse_selector(TeamBattleEventLayer::eventCheckCallBack);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TeamBattleEventLayer::eventCheckCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //イベント未開催
    if(json["error_status"] == "not_event"){
        auto layer = popupChangePageLayer::create("イベントは未開催です。\nチーム画面に戻ります。", "TeamTop");
        addChild(layer, 999999);
        
        return;
    }
    
    //エントリー時画面生成
    TeamBattleEventTopBase* layer_base = nullptr;
    if(json["is_entry"].bool_value()){
        layer_base = TeamBattleEventEntryTopLayer::create();
    }
    //未エントリー時画面生成
    else{
        layer_base = TeamBattleEventTopLayer::create();
    }
    this->addChild(layer_base, Z_LayerBase, T_LayerBase);
    layer_base->createDisplay(json);
}