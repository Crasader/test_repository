#ifndef __War__TeamMenuLayer__
#define __War__TeamMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "XTLayer.h"
#include "cocos-ext.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "network/HttpClient.h"
#include "HeaderLayer.h"

USING_NS_CC;
class TeamMenuLayer : public Layer
{
private:
    
public:
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    virtual bool init();
    CREATE_FUNC(TeamMenuLayer);
    
    void initEventListener(); //イベントリスナーの設定
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter();//初期実行関数
};

#endif /* defined(__War__TeamMenuLayer__) */
