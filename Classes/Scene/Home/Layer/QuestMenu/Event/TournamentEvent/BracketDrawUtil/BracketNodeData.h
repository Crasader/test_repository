#ifndef __war__BracketNodeData__
#define __war__BracketNodeData__

#include "HMSConsoleUtil.h"

USING_NS_CC;
using namespace std ;

/*
    データベースのtournament_event_bracket
 */
class BracketNodeData : public Node
{
public:
    static BracketNodeData* create() ; //インスタンス生成
    BracketNodeData();
    ~BracketNodeData();
 
    //テーブルの情報をそのまま
    CC_SYNTHESIZE(int, _tournament_event_bracket_node_id, TournamentEventBracketNodeId);
    CC_SYNTHESIZE(int, _tournament_event_list_id, TournamentEventListId);
    CC_SYNTHESIZE(int, _next_tournament_event_bracket_node_id, NextTournamentEventBracketNodeId);
    CC_SYNTHESIZE(int, _left_tournament_event_bracket_node_id, LeftTournamentEventBracketNodeId);
    CC_SYNTHESIZE(int, _right_tournament_event_bracket_node_id, RightTournamentEventBracketNodeId);
    CC_SYNTHESIZE(int, _left_user_master_id, LeftUserMasterId);
    CC_SYNTHESIZE(int, _right_user_master_id, RightUserMasterId);
    CC_SYNTHESIZE(int, _battle_list_id, BattleListId);
    CC_SYNTHESIZE(int, _round_no, RoundNo);
    CC_SYNTHESIZE(string, _wait_start_date, WaitStartDate);
    
    //phpでテーブル情報に補足的な情報を付加したもの
    CC_SYNTHESIZE(string, _win_direction, WinDirection);
    CC_SYNTHESIZE(bool, _is_now_battle_node, IsNowBattleNode);
    
    //c++側で計算し描画に必要な情報を格納
    CC_SYNTHESIZE(int, _draw_x, DrawX);
    CC_SYNTHESIZE(int, _draw_y, DrawY);
    CC_SYNTHESIZE(int, _draw_width, DrawWidth);
    void echoData() ;
    
    

};

#endif /* defined(__war__BracketNodeData__) */
