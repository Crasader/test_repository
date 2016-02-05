#include "DailyMissionJsonParser.h"


DailyMissionJsonParser* DailyMissionJsonParser::create()
{
    DailyMissionJsonParser *dailyMissionJsonParser = new DailyMissionJsonParser() ;
    dailyMissionJsonParser->autorelease() ;
    return dailyMissionJsonParser ;
}

DailyMissionJsonParser::DailyMissionJsonParser()
{
}

DailyMissionJsonParser::~DailyMissionJsonParser()
{
    HMSLOG("DailyMissionJsonParser::~DailyMissionJsonParser()") ;
}

HmsVector<DailyMissionGroupData *> DailyMissionJsonParser::parseDailyMissionGroupDataList(Json json)
{
    HmsVector<DailyMissionGroupData *> dailyMissionGroupDataList ;
    
    int jsonCount = countJsonData(json) ;
    for(int i=0; i<jsonCount; i++){
        string i_str = toString<int>(i);
        Json json_group = json[i_str].object_items(); //daily_mission_group_data のjson
        
        DailyMissionGroupData *dailyMissionGroupData = parseDailyMissionGroupData(json_group) ;
        dailyMissionGroupDataList.push_back(dailyMissionGroupData) ;
    }
    
    return dailyMissionGroupDataList ;
}

DailyMissionGroupData* DailyMissionJsonParser::parseDailyMissionGroupData(Json json)
{
    DailyMissionGroupData *dailyMissionGroupData = DailyMissionGroupData::create() ;
    dailyMissionGroupData->setDailyMissionGroupId(json["daily_mission_group_id"].string_value()) ;
    dailyMissionGroupData->setTitleImage(json["title_image"].string_value()) ;
    dailyMissionGroupData->setCountImage(json["count_image"].string_value()) ;
    dailyMissionGroupData->setChangePageName(json["change_page_name"].string_value()) ;
    
    HmsVector<AchievementData *> achievement_data_list = parseAchievementDataList(json["achievement_data_list"].object_items());
    dailyMissionGroupData->setAchievementDataList(achievement_data_list) ;
    
    //DailyMissionGroupDataの要素であるAchievementDataをDailyMissionGroupDataにaddChild
    for(vector<AchievementData *>::iterator iterator = achievement_data_list.begin(); iterator != achievement_data_list.end(); iterator++){
        AchievementData* achievement_data = *iterator ;
        dailyMissionGroupData->addChild(achievement_data) ;
    }
    
    return dailyMissionGroupData ;
}

HmsVector<AchievementData *> DailyMissionJsonParser::parseAchievementDataList(Json json)
{
    int jsonCount = countJsonData(json) ;
    HmsVector<AchievementData *> achievement_data_list ;
    for(int i=0; i<jsonCount; i++){
        string i_str = toString<int>(i);
        Json json_achieve = json[i_str].object_items(); //daily_mission_group_data のjson
        AchievementData* achievement_data = parseAchievementData(json_achieve) ;
        achievement_data_list.push_back(achievement_data) ;
    }
    
    return achievement_data_list ;
}

AchievementData* DailyMissionJsonParser::parseAchievementData(Json json)
{
    AchievementData *achievement_data = AchievementData::create() ;
    
    Json clear_info = json["clear_info"].object_items() ;
    achievement_data->setClearConditionValue(clear_info["clear_condition_value"].string_value()) ;
    achievement_data->setIsOldClear(clear_info["is_old_clear"].bool_value()) ;
    achievement_data->setIsNewClear(clear_info["is_new_clear"].bool_value()) ;
    achievement_data->setClearComment(clear_info["clear_comment"].string_value()) ;
    
    Json reward_info = json["reward_info"].object_items() ;
    achievement_data->setItemType(reward_info["item_type"].string_value()) ;
    achievement_data->setRelatedPrimaryId(reward_info["related_primary_id"].string_value()) ;
    achievement_data->setCount(reward_info["count"].string_value()) ;
    achievement_data->setCharaName(reward_info["chara_name"].string_value()) ;
    
    return achievement_data ;
}