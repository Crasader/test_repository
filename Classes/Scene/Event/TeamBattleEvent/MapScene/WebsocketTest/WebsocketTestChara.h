#ifndef __war__WebsocketTestChara__
#define __war__WebsocketTestChara__

#include "cocos2d.h"
#include "SynchronizedObject.h"

USING_NS_CC;

class WebsocketTestChara : public SynchronizedObject{
 public:
    //creater登録用
    static WebsocketTestChara* create(Synchronizer* synchronizer);
    virtual bool init(Synchronizer* synchronizer);
    static WebsocketTestChara* create(std::string path,Synchronizer* synchronizer,
                                      bool isLocal, std::string id = "");
    virtual bool init(std::string path, Synchronizer* synchronizer,
                      bool isLocal, std::string id = "");
    void addNetwork();
 private:
    virtual void onSend();
    virtual void onReceive(MultiValue* value);
    virtual SynchronizedObject* onClone(MultiValue* value, std::string id,
                                        Synchronizer* synchronizer);
    //ローカル側の更新処理
    virtual void onLocalUpdate(float delta);
    //リモート側の更新処理
    virtual void onRemoteUpdate(float delta);
    void setObjectName();
    void initEventListener(); //イベントリスナーの設定
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
    EventListenerTouchOneByOne *listener;
    Vec2 _sync_start_position;
    Vec2 _sync_end_position;
    std::string _path;
};

#endif /* defined(__war__WebsocketTestChara__) */
