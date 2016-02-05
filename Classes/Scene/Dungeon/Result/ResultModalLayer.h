#ifndef __War__ResultModalLayer__
#define __War__ResultModalLayer__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"


USING_NS_CC;

class ResultModalLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    virtual bool init();
    CREATE_FUNC(ResultModalLayer);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__ResultModalLayer__) */
