#ifndef __War__TournamentEventBattleWaitLayer__
#define __War__TournamentEventBattleWaitLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class TournamentEventBattleWaitLayer : public Layer
{
public:
    
    //ZOrder
    enum ZOrder{
        T_Glass = 1,
        T_Waiting_Label,
        T_Waiting_Time_Label,
        T_Cancel_MenuItem,
        T_Cancel_Menu,
    };
    
    //Tag
    enum Tag{
        Z_Glass = 1,
        Z_Waiting_Label,
        Z_Waiting_Time_Label,
        Z_Cancel_MenuItem,
        Z_Cancel_Menu,
    };
    
    //トーナメント状態
    enum WaitingStatus{
        No_Data = 0,  //トーナメントデータ未作成（一回目の開催待ち状態）
        Battle_Start, //対戦開始状態
        Next_Waiting, //次戦の開催時間まで対戦待ち状態
        Open_Waiting, //トーナメント開催中の待ち状態
        Win_Default,  //不戦勝の状態
        Exception,    //トーナメントデータ作成異常終了状態（最低人数に満たなかった場合）
        Already_Finished,    //対戦が決着済み
    };
    
    WaitingStatus _waiting_status; //待ち状態
    string _waiting_text; //待機文字列

    time_t _wait_remain_time; //待機残り時間
    bool _open_flg;
    bool _waiting_flg;
    bool _no_data_flg;
    
    //------- tips必要変数 ---------
    Json _tips_json ;
    Sprite *_tips_frame;
    Sprite *_left_chara;
    Sprite *_right_chara;
    Label *_tips_title;
    Label *_tips_comment;
    Label *_next;
    DrawNode *_next_shape;
    
    
    TournamentEventBattleWaitLayer(); //コンストラクタ
    ~TournamentEventBattleWaitLayer(); //デストラクタ
    
    float ratio;
	EventListenerTouchOneByOne* listener;
    
    static TournamentEventBattleWaitLayer* create();
    virtual bool init();
//    CREATE_FUNC(TournamentEventBattleWaitLayer);
    
    void initEventLitsener();
    void createSandglass(); //砂時計の作成
    void createWaitingLabel(); //待機文字列の作成
    void createCancelButton(); //キャンセルボタンの作成
    void removeCancelButton(); //キャンセルボタンを閉じる
    void createTips(); //ティップスの作成
    void showTipsAction(); //ティップスの表示
    DrawNode* getTriangleShape(const Point& point);
    
    void updateWaitingLabel(float dt); //待機文字列の更新
    int _update_count; //更新カウント
    
    //通信処理
    void waitBattle(float dt);
    void waitBattleCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__TournamentEventBattleWaitLayer__) */
