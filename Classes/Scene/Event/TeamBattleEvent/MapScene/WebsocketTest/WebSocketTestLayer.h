#ifndef __war__WebSocketTestLayer__
#define __war__WebSocketTestLayer__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"


USING_NS_CC;
USING_NS_CC_EXT;

class Synchronizer;
class WebsocketTestChara;

class WebSocketTestLayer : public Layer{
private:
    
    void onEnter(); //初期実行関数
    
    void initEventListener(); //イベントリスナーの設定
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    float ratio;
    Synchronizer* _synchronizer;
    WebsocketTestChara* _test_chara;
    EventListenerTouchOneByOne *listener;
public:
    virtual bool init();
    static WebSocketTestLayer* create();

};

#endif /* defined(__war__WebSocketTestLayer__) */
