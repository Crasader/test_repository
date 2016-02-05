#ifndef __war__DailyMissionJsonParser__
#define __war__DailyMissionJsonParser__

#include "HMSConsoleUtil.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "DailyMissionGroupData.h"
#include "AchievementData.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std ;
using namespace json11 ;

/*
 デイリーミッション情報のjsonを解析してクラスを作成
 */
class DailyMissionJsonParser : public Node
{
public:
    static DailyMissionJsonParser* create() ; //インスタンス生成
    DailyMissionJsonParser();
    ~DailyMissionJsonParser();
 
    HmsVector<DailyMissionGroupData *> parseDailyMissionGroupDataList(Json json) ;
    
public:
    DailyMissionGroupData* parseDailyMissionGroupData(Json json) ;
    HmsVector<AchievementData *> parseAchievementDataList(Json json) ;
    AchievementData* parseAchievementData(Json json) ;
};

#endif /* defined(__war__DailyMissionJsonParser__) */
