#ifndef __war__SynchronizedObject__
#define __war__SynchronizedObject__

#include "cocos2d.h"
#include "json11.hpp"

class Synchronizer;
class MultiValue;

class SynchronizedObject : public cocos2d::Node{
public:
    void update(float delta);
    void receive(std::string type, std::string value);
    SynchronizedObject* clone(json11::Json json, Synchronizer* synchronizer);
    MultiValue* getAddNetworkArgs();
    
    std::string getObjectName() const;
    std::string getId() const;
    bool isLocal() const;
protected:
    const float kDeadTime = 5.0f;
    const float kSendFps = 30.0f;
    const float kHeartBeatDuration = 1.0f;
    
    virtual bool init(bool isLocal, Synchronizer* synchronizer,
                      std::string objectName, std::string id = "");
    
    void send(int value);
    void send(uint value);
    void send(long value);
    void send(float value);
    void send(std::string value);
    void send(cocos2d::Vec2 value);
    void send(MultiValue* value);
    virtual void onSend() = 0;
    
    virtual void onReceive(int value);
    virtual void onReceive(uint value);
    virtual void onReceive(long value);
    virtual void onReceive(float value);
    virtual void onReceive(std::string value);
    virtual void onReceive(cocos2d::Vec2 value);
    virtual void onReceive(MultiValue* value);
    
    virtual SynchronizedObject* onClone(MultiValue* value, std::string id,
                                        Synchronizer* synchronizer) = 0;
    //ローカル側の更新処理
    virtual void onLocalUpdate(float delta) = 0;
    //リモート側の更新処理
    virtual void onRemoteUpdate(float delta) = 0;
    
    float sendCycle() const;
    std::string _id;
    bool _is_local;
    float _no_message_elapsed_time;
    float _elapsed_time_from_last_send;
    MultiValue* _add_network_args;
    std::string _object_name;
private:
    void heartBeat(float delta);
    void send(std::string value, std::string type);
    std::string generateId();
    void notifyAlive();
    
    Synchronizer* _synchronizer;
    std::string _pre_value;
    std::string _pre_type;
};

#endif /* defined(__war__SynchronizedObject__) */
