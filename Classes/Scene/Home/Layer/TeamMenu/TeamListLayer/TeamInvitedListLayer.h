//
//  TeamInvitedListLayer.h
//  参加したいチーム一覧
//


#ifndef __war__TeamInvitedListLayer__
#define __war__TeamInvitedListLayer__

#include "TeamListLayerBase.h"

class TeamInvitedListLayer : public TeamListLayerBase{
public:
    
    static TeamInvitedListLayer* create();
    
    //overRide
    void setHeaderSetting();
    void setSpriteTouchAction();
    void GetTeamListData();
};


#endif /* defined(__war__TeamInvitedListLayer__) */
