#include "AchievementData.h"


AchievementData* AchievementData::create()
{
    AchievementData *achievementData = new AchievementData() ;
    achievementData->autorelease() ;
    return achievementData ;
}

AchievementData::AchievementData()
:_is_already_displayed_stamp(false)
{
}

AchievementData::~AchievementData()
{
    HMSLOG("AchievementData::~AchievementData()") ;
}

void AchievementData::echoData() {
    HMSLOG("_clear_condition_value: %s(string)", getClearConditionValue().c_str()) ;
    HMSLOG("_is_old_clear: %d(bool)", getIsOldClear()) ;
    HMSLOG("_is_new_clear: %d(bool)", getIsNewClear()) ;
    
    HMSLOG("_clear_comment: %s(string)", getClearComment().c_str()) ;
    
    HMSLOG("_item_type: %s(string)", getItemType().c_str()) ;
    HMSLOG("_related_primary_id: %s(string)", getRelatedPrimaryId().c_str()) ;
    HMSLOG("_count: %s(string)", getCount().c_str()) ;
    HMSLOG("_chara_name: %s(string)", getCharaName().c_str()) ;
}

