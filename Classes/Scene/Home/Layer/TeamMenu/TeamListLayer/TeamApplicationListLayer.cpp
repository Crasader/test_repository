#include "TeamApplicationListLayer.h"
#include "HeaderLayer.h"
#include "TeamSearchConditionPopupLayer.h"
#include "TeamSortConditionPopupLayer.h"
#include "TeamDetailApplicationLayer.h"

TeamApplicationListLayer* TeamApplicationListLayer::create(){
    TeamApplicationListLayer *layer = new TeamApplicationListLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void TeamApplicationListLayer::setHeaderSetting(){
    HeaderLayer::setupTitleBar("参加申請中チーム一覧", "TeamNoBelong", this);
}

void TeamApplicationListLayer::setSpriteTouchAction(){
    setListTouchAction([this](Json teamData){
        TeamDetailApplicationLayer* team_detail = TeamDetailApplicationLayer::create(teamData);
        this->addChild(team_detail, 9999);
        HMSLOG("ここでPopupを出すのです");
    });
}

void TeamApplicationListLayer::GetTeamListData(){
    string url = HOST;
    url += "team/team_search.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&search_type=petition";
    auto selector = httpresponse_selector(TeamListLayerBase::httpCallBack);
    HttpConnect::sendHttpRequest(this, url, selector);
}
