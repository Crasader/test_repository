#include "TeamBattleEventTypeSelectLayer.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "HeaderLayer.h"

bool TeamBattleEventTypeSelectLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamBattleEventTypeSelectLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamBattleEventTypeSelectLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チーム対抗戦", "TeamTop", this);
}