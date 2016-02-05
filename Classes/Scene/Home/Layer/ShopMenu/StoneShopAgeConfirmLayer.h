#ifndef __War__StoneShopAgeConfirmLayer__
#define __War__StoneShopAgeConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "StoneShopAge2ndConfirmLayer.h"

#define AGE_BUY_STATUS_UNDER16 1 //16歳未満
#define AGE_BUY_STATUS_OVER16  2 //16~19歳まで
#define AGE_BUY_STATUS_OVER20  3 //20歳以上
USING_NS_CC;

class StoneShopAgeConfirmLayer : public cocos2d::Layer
{
    
private:
    enum Order{
        ORDER_RECEIVE_LAYER=20,
    };
    
    EventListenerTouchOneByOne *listener;
    void initEventListener();
    
    
    void onOver20Button(Ref* obj); //20歳以上のボタンコールバック
    void onOver16Button(Ref* obj); // 16歳〜19未満のボタンタップ時のコールバック
    void onUnder16Button(Ref* obj); // 16未満のボタンタップ時のコールバック
public:
    float ratio;
    Size visibleSize;
    
    virtual bool init();
    CREATE_FUNC(StoneShopAgeConfirmLayer);
    void setupImage();
    
    void popupAction();
    void deleteAction();
    
    //初期実行関数
    void onEnter();
    
};

#endif /* defined(__War__StoneShopAgeConfirmLayer__) */
