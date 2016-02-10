#ifndef __war__TeamBattleMapLayer__
#define __war__TeamBattleMapLayer__

#include "cocos2d.h"

USING_NS_CC;

class Synchronizer;

class TeamBattleMapLayer : public Layer{
private:   
    void onEnter(); //初期実行関数
    
    void createBackground();
    
    void initEventListener(); //イベントリスナーの設定
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    //synchronizerCallback
    void onConnectionCallback(Synchronizer* synchronizer);
    void onErrorCallback(Synchronizer* synchronizer);
    void onDisconnectionCallback(Synchronizer* synchronizer);
    void createClone();
    float _ratio;
    Synchronizer* _synchronizer;
    EventListenerTouchOneByOne* _listener;
    Sprite* _background;
public:
    virtual bool init();
    static TeamBattleMapLayer* create();
 
    
    
    
    
};

#endif /* defined(__war__TeamBattleMapLayer__) */
