#include "SynchronizedObject.h"
#include <uuid/uuid.h>
#include "Synchronizer.h"
#include "MultiValue.h"
#include "PrimitiveParser.h"

using namespace std;

bool SynchronizedObject::init(bool isLocal, Synchronizer* synchronizer,
                              string objectName, string id){
    if(!Node::init())return false;
    _no_message_elapsed_time = 0.0f;
    _elapsed_time_from_last_send = 0.0f;
    _is_local = isLocal;
    _object_name = objectName;
    _synchronizer = synchronizer;
    _add_network_args = MultiValue::create();
    this->addChild(_add_network_args);
    if(isLocal) _id = generateId();
    else        _id = id;
    this->schedule(schedule_selector(SynchronizedObject::heartBeat), kHeartBeatDuration);
    return true;
}

string SynchronizedObject::generateId(){
    uuid_t uuidValue;
    uuid_generate(uuidValue);
    uuid_string_t uuidString;
    uuid_unparse_upper(uuidValue, uuidString);
    return uuidString;
}

void SynchronizedObject::update(float delta){
    if(_is_local){
        onLocalUpdate(delta);
        _elapsed_time_from_last_send += delta;
        if(_elapsed_time_from_last_send + 0.005f > sendCycle()){
            onSend();
            _elapsed_time_from_last_send = 0.0f;
        }
    }else{
        _no_message_elapsed_time += delta;
        if(_no_message_elapsed_time > kDeadTime){
            _synchronizer->deleteSynchronizedObject(this);
            return;
        }
        onRemoteUpdate(delta);
    }
}

MultiValue* SynchronizedObject::getAddNetworkArgs(){
    if(_add_network_args == nullptr){
        CCLOG("error!! plz set _add_network_args");
        return nullptr;
    }
    
    return _add_network_args;
}

void SynchronizedObject::heartBeat(float delta){
    if(isLocal() && !_pre_value.empty() && !_pre_type.empty()){
        _synchronizer->send(this, _pre_value, _pre_type, Synchronizer::kHeartBeat);
    }
}

void SynchronizedObject::send(int value){
    send(PrimitiveParser::convertToCommand(value), "int");
}
void SynchronizedObject::send(uint value){
    send(PrimitiveParser::convertToCommand(value), "uint");
}
void SynchronizedObject::send(long value){
    send(PrimitiveParser::convertToCommand(value), "long");
}
void SynchronizedObject::send(float value){
    send(PrimitiveParser::convertToCommand(value), "float");
}
void SynchronizedObject::send(Vec2 value){
    send(PrimitiveParser::convertToCommand(value), "Vec2");
}
void SynchronizedObject::send(MultiValue* value){
    send(PrimitiveParser::convertToCommand(value), "MultiValue");
}

void SynchronizedObject::send(string value, string type){
    if(value.length() < 1 || type.length() < 1 || _pre_value == value){
        return;
    }
    _pre_value = value;
    _pre_type = type;
    _synchronizer->send(this, value, type, Synchronizer::CommandType::kUpdate);
}

void SynchronizedObject::onReceive(int value){
    HMSLOG("receive int : %d", value);
}
void SynchronizedObject::onReceive(uint value){
    HMSLOG("receive uint : %d", value);
}
void SynchronizedObject::onReceive(long value){
     HMSLOG("receive long : %l", value);
}
void SynchronizedObject::onReceive(float value){
     HMSLOG("receive float : %f", value);
}
void SynchronizedObject::onReceive(std::string value){
     HMSLOG("receive string : %s", value.c_str());
}
void SynchronizedObject::onReceive(cocos2d::Vec2 value){
     HMSLOG("receive Vec2 : x=%f, y%f", value.x, value.y);
}
void SynchronizedObject::onReceive(MultiValue* value){
    HMSLOG("receive MultiValue : %s", PrimitiveParser::convertToCommand(value).c_str());
}

void SynchronizedObject::receive(string type, string value){
    notifyAlive();
    
    if(type == "int") onReceive(PrimitiveParser::asInt(value));
    else if(type == "uint") onReceive(PrimitiveParser::asUint(value));
    else if(type == "long") onReceive(PrimitiveParser::asLong(value));
    else if(type == "float") onReceive(PrimitiveParser::asFloat(value));
    else if(type == "Vec2") onReceive(PrimitiveParser::asVec2(value));
    else if(type == "MultiValue") onReceive(PrimitiveParser::asMultiValue(value));
    else HMSLOG("invalid type of %s", type.c_str());
}


SynchronizedObject* SynchronizedObject::clone(json11::Json json,
                                              Synchronizer* synchronizer){
    MultiValue* value = nullptr;
    if (json["add_network_args"].string_value().length() != 0) {
        value = PrimitiveParser::asMultiValue(json["add_network_args"].string_value());
    }
    CCLOG("%s Clone!!", json["object_name"].string_value().c_str());
    return onClone(value, json["id"].string_value(), synchronizer);
}

float SynchronizedObject::sendCycle() const{
    return 1 / kSendFps;
}

string SynchronizedObject::getId() const{
    return _id;
}

bool SynchronizedObject::isLocal() const{
    return _is_local;
}

string SynchronizedObject::getObjectName() const{
    return _object_name;
}

void SynchronizedObject::notifyAlive(){
    _no_message_elapsed_time = 0.0f;
}