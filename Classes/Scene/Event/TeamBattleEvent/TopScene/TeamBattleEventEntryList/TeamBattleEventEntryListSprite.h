#ifndef __war__TeamBattleEventEntryListSprite__
#define __war__TeamBattleEventEntryListSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "Utility.h"
#include "json11.hpp"
#include "ScrollSprite.h"

USING_NS_CC;

class TeamBattleEventEntryListSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _team_battle_event_list_id, TeamBattleEventListId); //チーム対抗戦リストID
    CC_SYNTHESIZE(string, _team_battle_name, TeamBattleName); //チーム対抗戦名
    CC_SYNTHESIZE(string, _reception_time, ReceptionTime); //受付期間
    CC_SYNTHESIZE(string, _start_date_timestamp, StartDateTimestamp); //チーム対抗戦開始タイムスタンプ
    CC_SYNTHESIZE(bool, _entry_time_flg, EntryTimeFlg); //エントリー時間内かのフラグ
    CC_SYNTHESIZE(json11::Json, _reward_data_list, RewardDataList); //報酬のデータのjson
public:
    
    //タグ
    enum Tag{
        T_Team_Battle_Name = 1,
        T_Condition_Label,
        T_Event_Open_Label,
        T_Reception_Time,
        T_Join_Number,
        T_Reward_MenuItem,
        T_Reward_Menu,
        T_Entry_MenuItem,
        T_Entry_Menu,
        T_Entry_White,
        T_Need_Sprite,
        T_Alert_Label,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Team_Battle_Name = 1,
        Z_Condition_Label,
        Z_Event_Open_Label,
        Z_Reception_Time,
        Z_Join_Number,
        Z_Reward_MenuItem,
        Z_Reward_Menu,
        Z_Entry_MenuItem,
        Z_Entry_Menu,
        Z_Entry_White,
        Z_Need_Sprite,
        Z_Alert_Label,
    };
    
    Node* node; //親シーンのポインタを保持しておく
    
    TeamBattleEventEntryListSprite(); //コンストラクタ
    
    static TeamBattleEventEntryListSprite* create(json11::Json json, Node* node_);
    virtual bool init(json11::Json json, Node* node_);
    
    float ratio;
    void setupData(json11::Json json); //データの設定
    
    void createTeamBattleName(); //チーム対抗戦名の作成
    void showReceptionTime(); //受付期間の表示
    
    void createRewardButton(); //報酬ボタンの作成
    void createEntryButton(); //参加するボタンの作成
    
    void setConditionOver(); //チーム対抗戦条件外時の設定
    void checkEntryTime(); //エントリー時間かチェック
    void blinkingAction(); //エントリー時間になったら光らせるアクション
    void blinkingStopAction(); //エントリー時間になったら光らせるアクション
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TeamBattleEventEntryListSprite__) */
