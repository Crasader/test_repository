#include "TeamInvitedListLayer.h"
#include "HeaderLayer.h"
#include "TeamSearchConditionPopupLayer.h"
#include "TeamSortConditionPopupLayer.h"
#include "TeamDetailSolicitationLayer.h"

TeamInvitedListLayer* TeamInvitedListLayer::create(){
    TeamInvitedListLayer *layer = new TeamInvitedListLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void TeamInvitedListLayer::setHeaderSetting(){
    HeaderLayer::setupTitleBar("勧誘中チーム一覧", "TeamNoBelong", this);
}

void TeamInvitedListLayer::setSpriteTouchAction(){
    setListTouchAction([this](Json teamData){
        TeamDetailSolicitationLayer* team_detail = TeamDetailSolicitationLayer::create(teamData);
        this->addChild(team_detail, 9999);
        HMSLOG("ここでPopupを出すのです");
    });
}

void TeamInvitedListLayer::GetTeamListData(){
    string url = HOST;
    url += "team/team_search.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&search_type=invitation";
    auto selector = httpresponse_selector(TeamListLayerBase::httpCallBack);
    HttpConnect::sendHttpRequest(this, url, selector);
}