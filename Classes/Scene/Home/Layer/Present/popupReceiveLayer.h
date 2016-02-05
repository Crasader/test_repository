#ifndef __War__popupReceiveLayer__
#define __War__popupReceiveLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class popupReceiveLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    Size visibleSize;

    virtual bool init();
    CREATE_FUNC(popupReceiveLayer);
    
    void setupImage(string text);

    void popupAction();
    void deleteAction(float dt);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif /* defined(__War__popupReceiveLayer__) */
