#ifndef __war__TournamentRemainTimeSprite__
#define __war__TournamentRemainTimeSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "json11.hpp"
#include "TournamentEventTopSprite.h"

USING_NS_CC;

class TournamentRemainTimeSprite : public Sprite
{
public:
    
    enum Tag{
        T_Round_Label = 1,
        T_Remain_Label,
    };
    
    enum ZOrder{
        Z_Round_Label = 1,
        Z_Remain_Label,
    };
    
    time_t _remain_time;
    
    int _total_battle_number;
    int _now_battle_number;
    Node* _node;
    
    TournamentRemainTimeSprite(); //コンストラクタ
    
    static TournamentRemainTimeSprite* create(int total_battle_number, int now_battle_number, Node* node);
    virtual bool init(int total_battle_number, int now_battle_number, Node* node);
    float ratio;
    
    void setTournamentData();
    void createNextRoundLabel();
    void createRemainTimeLabel();
    
    string createRemainStr(time_t remain_time);
    void updateTournamentNextBattleRemainTimeLabel(float dt);
    
    void showTournamentStartLayer(); //トーナメント開始レイヤーの表示
    void closeAction(); //閉じるアクション
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentRemainTimeSprite__) */
