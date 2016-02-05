#ifndef __War__TeamBattleEventBattleWaitLayer__
#define __War__TeamBattleEventBattleWaitLayer__

#include "cocos2d.h"
#include "HttpConnect.h"

USING_NS_CC;
using namespace std;

class TeamBattleEventBattleWaitLayer : public Layer
{
private:
    Label* waiting_label;
    Label* remain_label;
    time_t _wait_remain_time; //待機残り時間
    int _update_count; //更新カウント
    string _waiting_text; //待機文字列
    enum WaitingStatus{ //チーム対抗戦状態
        No_Data = 0,  //チーム対抗戦データ未作成(マッチング待ち状態)
        Battle_Start, //対戦開始状態
        Next_Waiting, //チーム対抗戦開催直前の待ち状態
        Exception,    //チーム対抗戦データ作成異常終了状態（最低チーム数に満たなかった場合）
    };
    WaitingStatus _waiting_status; //待ち状態
    
    enum Tag {
        T_Inquiry = 1,
        T_BG,
        T_Glass,
        T_Waiting_Label,
        T_Waiting_Time_Label,
    };
    
    enum Zorder {
        Z_BG = 1,
        Z_Glass,
        Z_Button,
        Z_Waiting_Label,
        Z_Waiting_Time_Label,
    };
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TeamBattleEventBattleWaitLayer);
    
    void onEnter(); //初期実行関数
    void createSandglass(); //砂時計の作成
    void createWaitingLabel(); //待機文字列の作成
    
    //更新系
    void waitBattle(float dt);
    void waitBattleCallback(HttpClient *sender, HttpResponse *response);
    void updateWaitingLabel(float dt); //待機文字列の更新
};
#endif /* defined(__War__TeamBattleEventBattleWaitLayer__) */
