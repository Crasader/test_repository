#ifndef __War__DailyMissionGroupSprite__
#define __War__DailyMissionGroupSprite__

#include "cocos2d.h"
#include "common.h"
#include "DailyMissionGroupData.h"
#include "HMSFontUtil.h"

USING_NS_CC;
using namespace std;


class DailyMissionGroupSprite : public cocos2d::Sprite
{
private:
    DailyMissionGroupData *_daily_mission_group_data ;
    void createTitle() ;
    void createReward() ;
    void createConditionValue() ;
    Sprite *getRewardSprite(string item_type, string related_primary_id) ;
    Sprite* createCountSprite(string number, string unit); //数値+単位 unit=単位の意味で
    
    //スタンプアニメ作成
    ActionInterval* createStampAction();
    float basePositionXByIndex(int index) ;
    
    //点滅する枠を表示
    void createFlasingFrame() ;
    
    enum Tag{
    };
    
public:
    static DailyMissionGroupSprite* create(DailyMissionGroupData *daily_mission_group_data);
    virtual bool init(DailyMissionGroupData *daily_mission_group_data);
    
    //次のスタンプアニメーションがあるか
    bool hasNextStampAnimation() ;
    
    //次のスタンプアニメーションを再生(引数でコールバック関数の指定)
    void nextStampAnimation(Layer* parent, SEL_SCHEDULE s) ;
    void startStampAnimation(Layer* parent, SEL_SCHEDULE s, AchievementData* achievement_data, Point center_point);
    
    
    //すでに押されているスタンプを全て表示(アニメーションなしで)
    void showAllOldClearStamp() ;
    
    //新しく押されるスタンプを全て表示(アニメーションなしで)
    void showAllNewClearStamp() ;
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__DailyMissionGroupSprite__) */
