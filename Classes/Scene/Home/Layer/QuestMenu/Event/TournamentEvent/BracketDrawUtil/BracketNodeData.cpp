#include "BracketNodeData.h"


BracketNodeData* BracketNodeData::create()
{
    BracketNodeData *bracketNodeData = new BracketNodeData() ;
    bracketNodeData->autorelease() ;
    return bracketNodeData ;
}

BracketNodeData::BracketNodeData()
{
}

BracketNodeData::~BracketNodeData()
{
    HMSLOG("BracketNodeData::~BracketNodeData()") ;
}

void BracketNodeData::echoData() {
    HMSLOG("_tournament_event_bracket_node_id: %d(int)", getTournamentEventBracketNodeId()) ;
    HMSLOG("_tournament_event_list_id: %d(int)", getTournamentEventListId()) ;
    HMSLOG("_next_tournament_event_bracket_node_id: %d(int)", getNextTournamentEventBracketNodeId()) ;
    HMSLOG("_left_tournament_event_bracket_node_id: %d(int)", getLeftTournamentEventBracketNodeId()) ;
    HMSLOG("_right_tournament_event_bracket_node_id: %d(int)", getRightTournamentEventBracketNodeId()) ;
    HMSLOG("_left_user_master_id: %d(int)", getLeftUserMasterId()) ;
    HMSLOG("_right_user_master_id: %d(int)", getRightUserMasterId()) ;
    HMSLOG("_battle_list_id: %d(int)", getBattleListId()) ;
    HMSLOG("_round_no: %d(int)", getRoundNo()) ;
    HMSLOG("_wait_start_date: %s(string)", getWaitStartDate().c_str()) ;
    
    HMSLOG("_win_direction: %s(string)", getWinDirection().c_str()) ;
    HMSLOG("_is_now_battle_node: %d(bool)", getIsNowBattleNode()) ;
    
    HMSLOG("_draw_x: %d(int)", getDrawX()) ;
    HMSLOG("_draw_y: %d(int)", getDrawY()) ;
    HMSLOG("_draw_width: %d(int)", getDrawWidth()) ;
}
