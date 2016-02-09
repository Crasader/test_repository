#include "DailyMissionGroupData.h"


DailyMissionGroupData* DailyMissionGroupData::create()
{
    DailyMissionGroupData *dailyMissionGroupData = new DailyMissionGroupData() ;
    dailyMissionGroupData->autorelease() ;
    return dailyMissionGroupData ;
}

DailyMissionGroupData::DailyMissionGroupData()
{
}

DailyMissionGroupData::~DailyMissionGroupData()
{
    HMSLOG("DailyMissionGroupData::~DailyMissionGroupData()") ;
}


void DailyMissionGroupData::setAchievementDataList(HmsVector<AchievementData*> achievement_data_list)
{
    _achievement_data_list = achievement_data_list ;
}

HmsVector<AchievementData *> DailyMissionGroupData::getAchievementDataList()
{
    return _achievement_data_list ;
}

void DailyMissionGroupData::echoData() {
    for(vector<AchievementData *>::iterator iterator = _achievement_data_list.begin(); iterator != _achievement_data_list.end(); iterator++){
        AchievementData* data = *iterator ;
        data->echoData() ;
    }
    
    HMSLOG("__id: %s(string)", getDailyMissionGroupId().c_str()) ;
    HMSLOG("_change_page_name: %s(string)", getChangePageName().c_str()) ;
    HMSLOG("_title_image: %s(string)", getTitleImage().c_str()) ;
    HMSLOG("_count_image: %s(string)", getCountImage().c_str()) ;
}
