#include "Synchronizer.h"
#include "HmsConsoleUtil.h"
#include "HmsUtilityFunction.h"
#include "json11.hpp"
#include "PrimitiveParser.h"
#include "SynchronizedObject.h"
#include "SynchronizedObjectCreater.h"

using namespace std;
using namespace json11;

Synchronizer* Synchronizer::create(Node* parent, std::string url, std::string port,
                                   float syncFps,
                                   function<void()> onConnectionFunc,
                                   function<void()> onErrorFunc,
                                   function<void()> onDisconnectionFunc){
    Synchronizer *node = new Synchronizer();
    
    if (node && node->init(parent, url, port, syncFps, onConnectionFunc,
                           onErrorFunc, onDisconnectionFunc)){
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool Synchronizer::init(Node* parent, std::string url, std::string port,
                        float syncFps,
                        function<void()> onConnectionFunc,
                        function<void()> onErrorFunc,
                        function<void()> onDisconnectionFunc){
    if(!Node::init()) return false;
    HMSLOG("Synchronizer::init()");
    _parent = parent;
    _on_connection_func = onConnectionFunc;
    _on_error_func = onErrorFunc;
    _on_disconnection_func = onDisconnectionFunc;
    _url = url;
    _port = port;
    _sync_fps = syncFps;
    _connections_count = 0;
    _elapsed_time_from_last_sync = 0.0f;
    _synchronized_object_creater = SynchronizedObjectCreater::create();
    this->addChild(_synchronized_object_creater);
    this->scheduleUpdate();
    return true;
}

void Synchronizer::connectionStart(){
    _connection_state = kConnecting;
    string url = _url + ":" + _port;
    HMSLOG("connectingURL : %s", url.c_str());
    _client = SocketIO::connect(url, *this);
    _client->on("syncCommand", CC_CALLBACK_2(Synchronizer::onRecieveEvent, this));
}

void Synchronizer::update(float delta){
    for_each(_synchronized_object_lookup.begin(), _synchronized_object_lookup.end(),
             [delta](pair<string, SynchronizedObject*> elm){
                 SynchronizedObject* synchronizedObject = elm.second;
                 synchronizedObject->update(delta);
             });
    
    
    if(_connection_state != ConnectionState::kOpen) return;
    
    processCachedCommands(10);
    if(_cached_commands.size() > 5000) processCachedCommands(_cached_commands.size());
    
    _elapsed_time_from_last_sync += delta;
    if(_elapsed_time_from_last_sync + 0.001f > syncCycle()){
        _elapsed_time_from_last_sync = 0.0f;
        emit();
    }
}

void Synchronizer::emit(){
    if(_connection_state != ConnectionState::kOpen) return;
    
    string message = "[";
    int index = 0;
    //新規追加コマンドをまとめる
    for_each(_add_object_commands.begin(), _add_object_commands.end(),
             [&message, &index, this](pair<string, string> commands){
                 string id = commands.first;
                 string command = commands.second;
                 message += command;
                 if(_add_object_commands.size()-1 > index){
                     message += ", ";
                 }
                 index++;
             });
    
    if(_add_object_commands.size() > 0 &&
       _update_object_commands.size() > 0) message += ", ";
    _add_object_commands.clear();
    index = 0;
    for_each(_update_object_commands.begin(), _update_object_commands.end(),
             [&message, &index, this](pair<string, string> commands){
                 string id = commands.first;
                 string command = commands.second;
                 message += command;
                 if(_update_object_commands.size()-1 > index){
                     message += ", ";
                 }
                 index++;
             });
    if((_add_object_commands.size() > 0 ||
       _update_object_commands.size() > 0) &&
       _delete_object_commands.size() > 0) message += ", ";
    _update_object_commands.clear();
    
    index = 0;
    for_each(_delete_object_commands.begin(), _delete_object_commands.end(),
             [&message, &index, this](string command){
                 message += command;
                 if(_delete_object_commands.size()-1 > index){
                     message += ", ";
                 }
                 index++;
             });
    _delete_object_commands.clear();
    message += "]";
    if(message != "[]"){
        message = "{\"commands\": " + message + "}";
        _client->emit("syncCommand", message);
    }
}

void Synchronizer::processCachedCommands(int maxCommand){
    if(_cached_commands.size() < 1 ) return;
    int eraseCount = 0;
    for (int i = 0; i < maxCommand && i < _cached_commands.size(); i++, eraseCount++) {
        Json command = _cached_commands[i];
        if(command["command"].string_value().length() < 1) continue;
        int commandType = stoi(command["command"].string_value());
        
        switch (commandType) {
            case kUpdate:
                updateNetworkObject(command);
                break;
            case kAddObject:
                addNetworkObject(command);
                break;
            case kDeleteObject:
                deleteNetworkObject(command);
                break;
            case kHeartBeat:
                updateNetworkObject(command);
                break;
            default:
                break;
        }
        
    }
    for (int i = 0; i < eraseCount; i++) {
        _cached_commands.erase(_cached_commands.begin());

    }
}

void Synchronizer::updateNetworkObject(Json commandJson){
    string id = commandJson["id"].string_value();
    string type = commandJson["type"].string_value();
    string value = commandJson["value"].string_value();
    if(_synchronized_object_lookup.find(id) == _synchronized_object_lookup.end()){
        addNetworkObject(commandJson);
    }
    
    SynchronizedObject* synchronizedObject = _synchronized_object_lookup[id];
    if(type == "") return;
    synchronizedObject->receive(type, value);
    
}

void Synchronizer::addNetworkObject(Json commandJson){
    string id = commandJson["id"].string_value();
//    if(_synchronized_object_lookup.find(id) != _synchronized_object_lookup.end()){
//        return;
//    }
    
    SynchronizedObject* synchronizedObject = _synchronized_object_creater->
      getSynchronizedObject(commandJson, this);
    _synchronized_object_lookup[synchronizedObject->getId()] = synchronizedObject;
    _parent->addChild(synchronizedObject);
    
}

void Synchronizer::deleteNetworkObject(Json commandJson){
    string id = commandJson["id"].string_value();
    if(_synchronized_object_lookup.find(id) == _synchronized_object_lookup.end()){
        return;
    }
    SynchronizedObject* object = _synchronized_object_lookup[id];
    deleteSynchronizedObject(object);
}

void Synchronizer::deleteSynchronizedObject(SynchronizedObject *synchronizedObject){
    string id = synchronizedObject->getId();
    if(_synchronized_object_lookup.find(id) == _synchronized_object_lookup.end()){
        return;
    }
    
    if(synchronizedObject->isLocal()){
        send(synchronizedObject, "1", "int", kDeleteObject);
    }
    synchronizedObject->removeFromParentAndCleanup(true);
    _synchronized_object_lookup.erase(_synchronized_object_lookup.find(id));
}

void Synchronizer::send(SynchronizedObject *synchronizedObject,
                        string value, string type, CommandType commandType){
    if(type.length() < 1){
        HMSLOG("type is not specified : %s", type.c_str());
        return;
    }
    
    string id = synchronizedObject->getId();
    string addNetworkArgs = PrimitiveParser::convertToCommand(synchronizedObject->getAddNetworkArgs());
    string syncCommand = convertToJsonCommand(to_string(commandType), id,
                                              synchronizedObject->getObjectName(),
                                              value, type, addNetworkArgs);
    
    if(commandType == kUpdate ||
       commandType == kHeartBeat)         _update_object_commands[id] = syncCommand;
    else if(commandType == kAddObject)    _add_object_commands[id] = syncCommand;
    else if(commandType == kDeleteObject) _delete_object_commands.push_back(syncCommand);
}

void Synchronizer::onRecieveEvent(SIOClient* client, const string& data){
    string error;
    Json recieveDataJson = Json::parse(data, error);
    Json args = recieveDataJson["args"][0];
    Json values = args["value"];
    Json commands = values["commands"];
    HMSLOG("####values : %s", commands.dump().c_str());
    int index = 0;
    while(true){
        if(commands[index].is_null()) break;
        _cached_commands.push_back(commands[index]);
        index++;
    }
    
}

string Synchronizer::convertToJsonCommand(string commandType, string id,
                                         string name, string value, string type, string addNetworkAargs){
    string jsonString = "{\"id\" : \"" + id +
                        "\", \"object_name\" : \"" + name +
                        "\", \"command\" : \"" + commandType +
                        "\",  \"type\" : \"" +type +
                        "\",  \"value\" : \"" +value +
                        "\", \"add_network_args\" : \"" + addNetworkAargs + "\"}  ";
    return jsonString;
}

void Synchronizer::add(SynchronizedObject *synchronizedObject){
    _synchronized_object_lookup[synchronizedObject->getId()] = synchronizedObject;
    send(synchronizedObject, "1" , "int", kAddObject);
    HMSLOG("add ID : %s", synchronizedObject->getId().c_str());
}

void Synchronizer::addSynchronizedObjectCreater(SynchronizedObject *synchronizedObject){
    _synchronized_object_creater->add(synchronizedObject);
}

float Synchronizer::syncCycle() const{
    return 1.0f / _sync_fps;
}

int Synchronizer::getConnectionsCount() const{
    return _connections_count;
}

Synchronizer::ConnectionState Synchronizer::getConnectionState() const{
    return _connection_state;
}

void Synchronizer::onConnect(SIOClient *client){
    HMSLOG("#####socketIO::connection success!!");
    _connection_state = kOpen;
    _on_connection_func();
}

void Synchronizer::onMessage(SIOClient* client, const std::string& data){
    HMSLOG("#####socketIO::send receive!! : %s", data.c_str());
}

void Synchronizer::onClose(SIOClient* client){
    HMSLOG("#####socketIO::disconnection success!!");
    _connection_state = kClosed;
    _on_disconnection_func();
}

void Synchronizer::onError(SIOClient* client, const std::string& data){
    HMSLOG("#####socketIO::failed... :  %s", data.c_str());
    _on_error_func();
}
