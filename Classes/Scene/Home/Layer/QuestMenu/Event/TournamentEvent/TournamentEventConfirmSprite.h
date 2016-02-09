#ifndef __war__TournamentEventConfirmSprite__
#define __war__TournamentEventConfirmSprite__

#include "cocos2d.h"
#include "NormalScene.h"
#include "HMSFontUtil.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"
#include "Utility.h"

USING_NS_CC;

class TournamentEventConfirmSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _tournament_event_list_id, TournamentEventListId); //トーナメントID
    CC_SYNTHESIZE(string, _tournament_name, TournamentName); //トーナメント名
    CC_SYNTHESIZE(string, _condition_name, ConditionName); //トーナメント参加条件の名前
    CC_SYNTHESIZE(string, _tournament_start_date, TournamentStartDate); //トーナメント開始時間
    CC_SYNTHESIZE(string, _deck_name, DeckName); //選択デッキ名

    
public:
    
    //タグ
    enum Tag{
        T_Tournament_Name = 1,
        T_Condition_Label,
        T_Event_Open_Label,
        T_Use_Deck_Name,
        T_Caution,
        T_Entry_MenuItem,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Tournament_Name = 1,
        Z_Condition_Label,
        Z_Event_Open_Label,
        Z_Use_Deck_Name,
        Z_Caution,
        Z_Entry_MenuItem,
    };
    
    enum Status{
        Entry = 1,
        Cancel,
        Top_info,
    };
    
    Status _status; //スプライトの状態
    
    TournamentEventConfirmSprite(); //コンストラクタ
    
    static TournamentEventConfirmSprite* create(json11::Json json, Status status);
    virtual bool init(json11::Json json, Status status);
    
    float ratio;
    void setupData(json11::Json json); //データの設定
    
    void createTournamentName(); //トーナメント名の作成
    void createConditionLabel(); //参加条件
    
    void showEventOpenDate(); //対戦日時の表示
    void showUseDeckName(); //使用デッキ名
    void showCautionLabel(); //注意文の表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentEventConfirmSprite__) */
