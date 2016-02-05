//
//  TeamApplicationListLayer.h
//  参加したいチーム一覧
//


#ifndef __war__TeamApplicationListLayer__
#define __war__TeamApplicationListLayer__

#include "TeamListLayerBase.h"

class TeamApplicationListLayer : public TeamListLayerBase{
public:
    
    static TeamApplicationListLayer* create();
        
    //overRide
    void setHeaderSetting();
    void setSpriteTouchAction();
    void GetTeamListData();
};


#endif /* defined(__war__TeamApplicationListLayer__) */
