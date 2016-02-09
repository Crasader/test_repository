#ifndef __War__RetransmissionLayer__
#define __War__RetransmissionLayer__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"

#define POPUP_ACTION_TIME 0.3f

USING_NS_CC;
using namespace std;

class RetransmissionLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示テキスト
    
public:
    float ratio;
    
    static RetransmissionLayer* create(string message);
    virtual bool init(string message);
    
    void setupImage(); //画像の表示
    void popupAction(); //ポップアップアクション
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__RetransmissionLayer__) */
