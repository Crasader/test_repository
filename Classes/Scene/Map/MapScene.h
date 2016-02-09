#ifndef __War__MapScene__
#define __War__MapScene__

#include "cocos2d.h"
#include "HmsVector.h"
#include "XTLayer.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "Player.h"
#include "SimpleAudioEngine.h"
#include "TGestureRecognizer.h"
#include "MapLayer.h"
#include "MapNaviLayer.h"
#include "ChatBaseUpperLayer.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;
using namespace CocosDenshion;

#define MAP_LAYER_Z 100
#define MAP_NAVI_LAYER_Z 1000
#define MAP_UI_LAYER_Z 1000
#define MAP_STAMP_Z 1000
#define MAP_ESCAPE_Z 1000
#define MAP_SUPPORT_Z 1000
#define MAP_CHAT_LAYER_Z 2000
#define MAP_POPUP_Z 3000

class MapScene : public Layer, public SuperAnim::SuperAnimNodeListener, public TGestureHandler
{
private:
    EventListenerTouchOneByOne *listener;
    float ratio;
    
    //イベント関連データ
    string _boss_event_id;
    string _boss_event_stage_log_id;
    
    //プレイヤーデータ
    int _user_master_id = 0;
    
    //通信関連データ
    bool _connection_flg = false;
    
    //各種レイヤー
    MapLayer *_map_layer = nullptr;
    MapNaviLayer *_map_navi_layer = nullptr;
    ChatBaseUpperLayer *_chat_base_upper_layer = nullptr;
    
    //マップ関連データ
    Json _json_map;
    CC_SYNTHESIZE(string, _map_filename, MapFilename);//マップファイル名
    CC_SYNTHESIZE(int, _map_width, MapWidth);//マップの横幅
    CC_SYNTHESIZE(int, _map_height, MapHeight);//マップの高さ
    float _max_map_scale = 2.0f;
    float _min_map_scale = 0.6f;
    float _movable_distance;
    float _move_speed;
    float _contact_distance;
    float _camp_distance;
    
    //キャラ関連データ
    Json _json_chara;
    
    //チーム関連データ
    string _player_team_name;
    string _enemy_team_name;
    
    //タップ関連データ
    TGestureRecognizer* _gestureRecognizer;
    bool _layer_animation_flg;
    float _limit_distance = 100.0f;
    
    //ターン＆状態管理データ
    bool _initialize_flg = false;
    int _turn_no = MAP_CHARA_TYPE_ENEMY;
    int _rest_time = 0;
    time_t _rest_time_update_time;
    int _time_up = 0;
    bool _time_up_flg = false;
    int _delay_time;
    bool _delay_flg = false;
    bool _finish_flg = false;
    bool _finish_effect_flg = false;
    Point _move_start_point;
    
    //コマンド管理データ
    Json _json_command;
    int _command_history_id = 0;
    HmsVector<string> _receive_command_list;
    bool _receive_command_execute_flg = false;
    HmsVector<string> _send_command_list;
    bool _battle_scene_flg = false;
    
    //イベントPt管理データ
    int _personal_point;
    int _team_point;
    
    //応援管理データ
    int _support_user_master_id = 0;
    string _support_user_name = "";
    
    //ボタンデータ
    Sprite *_stamp_img;
    Sprite *_escape_img;
    Sprite *_support_img;
    Sprite *_support_bk_img;
    Label *_support_label;
public:
    //基本メソッド
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(MapScene);
    
    //******************************初期化関連******************************
    void cutLeftAndRightVisibleSpace(); //両サイドのはみ出ている部分を見えなくする
    void onEnter();
    void getMapData();
    void getMapDataCallback(HttpClient *sender, HttpResponse *response);
    void initialize();
    void getCharaData();
    void getCharaDataCallback(HttpClient *sender, HttpResponse *response);
    
    //******************************その他******************************
    void setSupportUserMasterId(int support_user_master_id, string support_user_name);
    void resetSupportUserMasterId();
    void setSupportAnimation();
    bool isPlayerTurn();
    
    //******************************タップ処理******************************
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual bool onGesturePinch(TGesturePinch* gesture);
    virtual bool onGestureSwipe(TGestureSwipe* gesture);
    bool checkBoundary();
    
    //******************************スケジュラー******************************
    void timeup();
    void finish();
    void finishCallback(HttpClient *sender, HttpResponse *response);
    void showEffect(Layer* layer, string filename, float scale, Point pos);
    void insertSendCommandList(string command);
    void sendCommandData();
    void sendCommandDataCallback(HttpClient *sender, HttpResponse *response);
    void getCommandData();
    void getCommandDataCallback(HttpClient *sender, HttpResponse *response);
    void executeCommand(float dt);
    void checkBattle(float dt);
    void checkBattleCallback(HttpClient *sender, HttpResponse *response);
    void goToBattle(int boss_event_battle_log_id);
    void updateData(float dt);
};

#endif /* defined(__War__MapScene__) */
