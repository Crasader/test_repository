#ifndef __war__TournamentEventTopSprite__
#define __war__TournamentEventTopSprite__

#include "cocos2d.h"
#include "NormalScene.h"
#include "HMSFontUtil.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"
#include "Utility.h"

USING_NS_CC;

class TournamentEventTopSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _tournament_event_list_id, TournamentEventListId); //トーナメントID
    CC_SYNTHESIZE(string, _tournament_name, TournamentName); //トーナメント名
    CC_SYNTHESIZE(string, _enemy_name, EnemyName); //対戦相手の名前
    CC_SYNTHESIZE(string, _wait_start_date, WaitStartDate); //対戦開始日時
    
    CC_SYNTHESIZE(string, _total_battle_number, TotalBattleNumber); //合計対戦数
    CC_SYNTHESIZE(string, _now_battle_number, NowBattleNumber); //現在の対戦回数
    
public:
    
    //タグ
    enum Tag{
        T_Tournament_Name = 1,
        T_Event_Open_Label,
        T_Enemy_Name,
        T_Round_Label,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Tournament_Name = 1,
        Z_Event_Open_Label,
        Z_Enemy_Name,
        Z_Round_Label,
    };
    
    enum RoundStatus{
        None = -1,
        Final,
        SemiFinal,
        QuarterFinals,
        Other,
    };
        
    TournamentEventTopSprite(); //コンストラクタ
    
    static TournamentEventTopSprite* create(json11::Json json, string &image_path);
    virtual bool init(json11::Json json, string &image_path);
    
    float ratio;
    void setupData(json11::Json json); //データの設定
    
    void createTournamentName(); //トーナメント名の作成
    void showRoundName(); //第何回戦か表示
    string createEventOpenDate(); //トーナメント開催日時の文字列の作成
    void showEventOpenDate(); //対戦日時の表示
    void createEnemyName(); //対戦相手の名前ラベルの作成
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentEventTopSprite__) */
