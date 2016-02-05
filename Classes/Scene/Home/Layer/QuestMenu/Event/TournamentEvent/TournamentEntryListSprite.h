#ifndef __war__TournamentEntryListSprite__
#define __war__TournamentEntryListSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "json11.hpp"
#include "ScrollSprite.h"
#include "TournamentEventRewardSprite.h"

USING_NS_CC;

class TournamentEntryListSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _tournament_event_list_id, TournamentEventListId); //トーナメントID
    CC_SYNTHESIZE(string, _tournament_name, TournamentName); //トーナメント名
    CC_SYNTHESIZE(string, _item_type, ItemType); //参加必要アイテムタイプ
    CC_SYNTHESIZE(string, _count, Count); //参加必要アイテム数
    CC_SYNTHESIZE(string, _reception_time, ReceptionTime); //受付期間
    CC_SYNTHESIZE(string, _condition_name, ConditionName); //トーナメント参加条件の名前
    CC_SYNTHESIZE(string, _join_number, JoinNumber); //トーナメント参加人数
    CC_SYNTHESIZE(string, _result_text, ResultText); //トーナメント参加人数
    CC_SYNTHESIZE(string, _start_date_timestamp, StartDateTimestamp); //トーナメント開始タイムスタンプ
    
    CC_SYNTHESIZE(bool, _condition_match_flg, ConditionMatchFlg); //条件マッチフラグ
    CC_SYNTHESIZE(bool, _entry_time_flg, EntryTimeFlg); //エントリー時間内かのフラグ
    
    CC_SYNTHESIZE(json11::Json, _reward_data_list, RewardDataList); //報酬のデータのjson
public:
    
    //タグ
    enum Tag{
        T_Tournament_Name = 1,
        T_Condition_Label,
        T_Event_Open_Label,
        T_Reception_Time,
        T_Join_Number,
        T_Reward_MenuItem,
        T_Bracket,
        T_Reward_Menu,
        T_Entry_MenuItem,
        T_Entry_Menu,
        T_Entry_White,
        T_Need_Sprite,
        T_Alert_Label,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Tournament_Name = 1,
        Z_Condition_Label,
        Z_Event_Open_Label,
        Z_Reception_Time,
        Z_Join_Number,
        Z_Reward_MenuItem,
        Z_Bracket,
        Z_Reward_Menu,
        Z_Entry_MenuItem,
        Z_Entry_Menu,
        Z_Entry_White,
        Z_Need_Sprite,
        Z_Alert_Label,
    };
    
    Node* _normal_scene; //ノーマルシーンのポインタを保持しておく
    
    TournamentEntryListSprite(); //コンストラクタ
    
    static TournamentEntryListSprite* create(json11::Json json, Node* normal_scene);
    virtual bool init(json11::Json json, Node* normal_scene);
    
    float ratio;
    void setupData(json11::Json json); //データの設定
    
    void createTournamentName(); //トーナメント名の作成
    void createConditionLabel(); //参加条件
    void showReceptionTime(); //受付期間の表示
    void showJoinNumber(); //参加人数の表示
    void showResultText(); //結果の表示
    
    void createRewardButton(); //報酬ボタンの作成
    void createEntryButton(); //参加するボタンの作成
    void createEntryNeedItem(); //エントリー必要アイテム
    
    void setConditionOver(); //トーナメント条件外時の設定
    void checkEntryTime(); //エントリー時間かチェック
    void blinkingAction(); //エントリー時間になったら光らせるアクション
    void blinkingStopAction(); //エントリー時間になったら光らせるアクション
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentEntryListSprite__) */
