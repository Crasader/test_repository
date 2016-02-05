#ifndef __War__EventPowerSprite__
#define __War__EventPowerSprite__

#include "cocos2d.h"
#include "common.h"
#include "HomeCommon.h"
#include "HmsConsoleUtil.h"
#include "SaveData.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "HMSFontUtil.h"


USING_NS_CC;
using namespace std;

class EventPowerSprite : public cocos2d::Sprite
{
public:
    
    enum Tag{
        T_RECOVERY_LABEL,
        T_RECOVERY_BACK,
    };
    
    static EventPowerSprite* create();
    virtual bool init();
    
    void onEnter(); //初期実行関数
    void loadEventPower() ;//体力情報を読みこむ
    
    int _now_event_power ;
    int _max_event_power ;
    time_t _check_time; //時間の経過をチェックする変数
    time_t _event_power_use_time; //最後に体力を使った時間(更新あり)
    time_t _server_diff_time; //サーバーと端末の時間差
    
    void createBattlePointRecoveryTime();//「パワーの最終更新時間」と「最終更新時間時点におけるパワー」に基づき、現在のイベント専用パワーの現在の回復量を求める
    void adjustTime();
    void updateTime(float dt) ;
    void setRecoveryTime(float dt);
    void recoveryAction();
//    int _event_power ;
    
    //のスプライトを表示
    void createEventPowerMark() ;
    void updateEventPowerMark(bool recovery_action) ;
    HmsVector<Sprite *> empty_power_marks;
    HmsVector<Sprite *> power_marks;
    
    
    //unixタイムスタンプを日時と時間の形に整形して保存
    void savePowerUseTime(time_t save_time) ;
    
};

#endif /* defined(__War__EventPowerSprite__) */
