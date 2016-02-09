#ifndef __war__DailyMissionGroupData__
#define __war__DailyMissionGroupData__

#include "HMSConsoleUtil.h"
#include "AchievementData.h"
#include "HmsVector.h"

USING_NS_CC;
using namespace std ;


/*
    データベースのtournament_event_bracket
 */
class DailyMissionGroupData : public Node
{
public:
    static DailyMissionGroupData* create() ; //インスタンス生成
    DailyMissionGroupData();
    ~DailyMissionGroupData();
    
    HmsVector<AchievementData *> _achievement_data_list; //ボタン背景画像
    
    void setAchievementDataList(HmsVector<AchievementData*> achievement_data_list) ;
    HmsVector<AchievementData *> getAchievementDataList() ;
 
    //達成条件のグループのid,遷移先ページ名, タイトル画像, 単位の画像
    CC_SYNTHESIZE(string, _daily_mission_group_id, DailyMissionGroupId);
    CC_SYNTHESIZE(string, _change_page_name, ChangePageName);
    CC_SYNTHESIZE(string, _title_image, TitleImage);
    CC_SYNTHESIZE(string, _count_image, CountImage);
    
    
    void echoData() ;
};

#endif /* defined(__war__DailyMissionGroupData__) */
