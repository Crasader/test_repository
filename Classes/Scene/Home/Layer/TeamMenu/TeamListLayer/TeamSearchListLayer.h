//
//  TeamSearchListLayer.h
//  参加したいチーム一覧
//


#ifndef __war__TeamSearchListLayer__
#define __war__TeamSearchListLayer__

#include "TeamListLayerBase.h"

class TeamSearchListLayer : public TeamListLayerBase{
public:
    //検索条件
    string searchTeamName;
    string searchPlayType;
    string searchTeamType;
    string sortType;
    
    enum sortTeamMenberType{
        DESCENDING_ORDER = 1,
        ASCENDING_ORDER
    };
    
    static TeamSearchListLayer* create();
    
    void createButton();
    
    //overRide
    void setHeaderSetting();
    void setSpriteTouchAction();
    void GetTeamListData();
    string getSortConditionButtonText();
    void updateButtonText();
};


#endif /* defined(__war__TeamSearchListLayer__) */
