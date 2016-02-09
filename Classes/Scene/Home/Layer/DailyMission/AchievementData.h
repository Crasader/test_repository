#ifndef __war__AchievementData__
#define __war__AchievementData__

#include "HMSConsoleUtil.h"

USING_NS_CC;
using namespace std ;

/*
 デイリーミッション情報
 */
class AchievementData : public Node
{
public:
    static AchievementData* create() ; //インスタンス生成
    AchievementData();
    ~AchievementData();
 
    //達成状況、達成条件など
    CC_SYNTHESIZE(string, _clear_condition_value, ClearConditionValue);
    CC_SYNTHESIZE(bool, _is_old_clear, IsOldClear);
    CC_SYNTHESIZE(bool, _is_new_clear, IsNewClear);
    CC_SYNTHESIZE(string, _clear_comment, ClearComment);
    
    //達成時のアイテム情報
    CC_SYNTHESIZE(string, _item_type, ItemType);
    CC_SYNTHESIZE(string, _related_primary_id, RelatedPrimaryId);
    CC_SYNTHESIZE(string, _count, Count);
    CC_SYNTHESIZE(string, _chara_name, CharaName);
    
    //スタンプアニメ表示済み
    CC_SYNTHESIZE(bool, _is_already_displayed_stamp , IsAlreadyDisplayedStamp);
    
    void echoData() ;

};

#endif /* defined(__war__AchievementData__) */
