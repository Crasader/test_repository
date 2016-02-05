#ifndef __War__TapEffectLayer__
#define __War__TapEffectLayer__

#include "cocos2d.h"
#include "common.h"
#include "NormalScene.h"
#include <time.h>
#include "HmsVector.h"
#include "TitleBarLayer.h"


USING_NS_CC;
using namespace std;

class TapEffectLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    TapEffectLayer() ;
    
    enum Tag{
        T_TAP_CENTER,
        T_TAP_WAVEFORM,
        T_TAP_TAIL,
        T_MOTION_STREAK,
    };
    
    enum ZOrder{
        Z_TAP_CENTER,
        Z_TAP_WAVEFORM,
        Z_TAP_TAIL,
    };

    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TapEffectLayer);
    
    void initEventListener() ;
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

    int count_touch_move ; //noTouchMoveが呼ばれた回数を記録
    void resetMotionStreak() ;
    void addMotionStreakPoint(Point point) ;
    
    void showTapEffect(Point point) ; //タップエフェクト
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__TapEffectLayer__) */
