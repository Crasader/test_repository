#ifndef __war__Synchronizer__
#define __war__Synchronizer__

#include <map>
#include <SocketIO.h>
#include "cocos2d.h"
#include "HmsVector.h"
#include "json11.hpp"

USING_NS_CC;
using namespace network;

class SynchronizedObject;
class SynchronizedObjectCreater;

class Synchronizer : public Node, public SocketIO::SIODelegate{
 public:
    enum ConnectionState{
        kConnecting = 0,
        kOpen,
        kClosing,
        kClosed
    };
    
    enum CommandType{
        kUpdate = 0,
        kAddObject,
        kDeleteObject,
        kHeartBeat,
    };
    
    static Synchronizer* create(Node* parent, std::string url, std::string port,
                                float syncFps,
                                const function<void(Synchronizer*)>& onConnectionFunc,
                                const function<void(Synchronizer*)>& onErrorFunc,
                                const function<void(Synchronizer*)>& onDisconnectionFunc);
    virtual bool init(Node* parent, std::string url, std::string port,
                      float syncFps,
                      const function<void(Synchronizer*)>& onConnectionFunc,
                      const function<void(Synchronizer*)>& onErrorFunc,
                      const function<void(Synchronizer*)>& onDisconnectionFunc);
    
    void connectionStart();
    void send(SynchronizedObject* synchronizedObject,
              std::string value, std::string type, CommandType);
    void add(SynchronizedObject* synchronizedObject);
    void addSynchronizedObjectCreater(SynchronizedObject* synchronizedObject);
    void deleteSynchronizedObject(SynchronizedObject* synchronizedObject);
    float syncCycle() const;
    int getConnectionsCount() const;
    ConnectionState getConnectionState() const;
 private:
    void update(float delta);
    void emit();
    void processCachedCommands(int maxCommand);
    void addNetworkObject(json11::Json commandJson);
    void deleteNetworkObject(json11::Json commandJson);
    void updateNetworkObject(json11::Json commandJson);
    std::string convertToJsonCommand(std::string commandType, std::string id,
                                     std::string name, std::string value,
                                     std::string type, std::string addNetworkAargs);
    
    //socket.IO eventlistener
    void onRecieveEvent(SIOClient* client, const std::string& data);
    //SIODelegate
    virtual void onConnect(SIOClient* client);
    virtual void onMessage(SIOClient* client, const std::string& data);
    virtual void onClose(SIOClient* client);
    virtual void onError(SIOClient* client, const std::string& data);
    
    SIOClient* _client;
    Node* _parent;
    SynchronizedObjectCreater* _synchronized_object_creater;
    function<void(Synchronizer*)> _on_connection_func;
    function<void(Synchronizer*)> _on_error_func;
    function<void(Synchronizer*)> _on_disconnection_func;
    
    std::string _url;
    std::string _port;
    float _sync_fps;
    //IDをKeyにObjectを特定するテーブル
    std::map<std::string, SynchronizedObject*> _synchronized_object_lookup;
    std::map<std::string, std::string> _update_object_commands;
    std::map<std::string, std::string> _add_object_commands;
    std::vector<std::string> _delete_object_commands;
    //大量のコマンドが送られてきた際にキャッシュし、順次実行する
    std::vector<json11::Json> _cached_commands;
    int _connections_count;//サーバに接続してるクライアント数
    ConnectionState _connection_state;
    float _elapsed_time_from_last_sync;
};

#endif /* defined(__war__Synchronizer__) */
