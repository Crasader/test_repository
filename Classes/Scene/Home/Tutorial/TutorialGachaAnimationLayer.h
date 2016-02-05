#ifndef __War__TutorialGachaAnimationLayer__
#define __War__TutorialGachaAnimationLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;

class TutorialGachaAnimationLayer : public cocos2d::Layer
{
private:
    float ratio;
    EventListenerTouchOneByOne *listener;
    
public:
    virtual bool init();
    CREATE_FUNC(TutorialGachaAnimationLayer);
    
    void createEventLitener(); //イベントリスナーの作成
    void showEffect(float dt); //エフェクトの表示
    void showTouchStart(); //タッチスタート画像の表示
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__TutorialGachaAnimationLayer__) */
