#ifndef __War__ResultAchievementLayer__
#define __War__ResultAchievementLayer__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsFontUtil.h"
#include "Utility.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

class ResultAchievementLayer : public cocos2d::Layer
{
public:
    EventListenerTouchOneByOne *listener;
    float ratio;
    
    HmsVector<string> _achivement_txt_list;
    HmsVector<int> _old_achievement_flg_list;
    HmsVector<int> _new_achievement_flg_list;
public:
    virtual bool init();
    CREATE_FUNC(ResultAchievementLayer);
    
    void setup(HmsVector<string> achivement_txt_list,
               HmsVector<int> old_achievement_flg_list,
               HmsVector<int> new_achievement_flg_list);
    void createSprites();
    void animation();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__ResultAchievementLayer__) */
