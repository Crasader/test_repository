#ifndef __War__LoadingLayer__
#define __War__LoadingLayer__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "SuperAnimNodeV2.h"
#include "HomeCommon.h"

USING_NS_CC;

class LoadingLayer : public cocos2d::Layer, public SuperAnim::SuperAnimNodeListener
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(LoadingLayer);
    
    void setupImage(float dt); //ロード中画像の設定
    void popupAction(); //ポップアップアクション処理
    
    void showEffect(string filename, Vec2 position, float scale);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__War__LoadingLayer__) */
