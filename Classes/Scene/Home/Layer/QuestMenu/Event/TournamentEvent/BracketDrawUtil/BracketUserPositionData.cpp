#include "BracketUserPositionData.h"

BracketUserPositionData* BracketUserPositionData::create(int user_master_id, int round_no, bool is_now_battle, int chara_id, Point user_position, Point depend_node_position)
{
    BracketUserPositionData *bracketUserPositionData = new BracketUserPositionData(user_master_id, round_no, is_now_battle, chara_id, user_position, depend_node_position) ;
    bracketUserPositionData->autorelease() ;
    return bracketUserPositionData ;
}

BracketUserPositionData::BracketUserPositionData(int user_master_id, int round_no, bool is_now_battle, int chara_id, Point user_position, Point depend_node_position)
:_user_master_id(user_master_id)
,_round_no(round_no)
,_is_now_battle(is_now_battle)
,_chara_id(chara_id)
,_user_position(user_position)
,_depend_node_position(depend_node_position)
{
}

BracketUserPositionData::~BracketUserPositionData()
{
    HMSLOG("BracketUserPositionData::~BracketUserPositionData()") ;
}


void BracketUserPositionData::echoData()
{
    HMSLOG("_round_no: %d(int)", getRoundNo()) ;
    HMSLOG("_is_now_battle: %d(int)", getIsNowBattle()) ;
    HMSLOG("_user_master_id: %d(int)", getUserMasterId()) ;
    HMSLOG("_chara_id: %d(int)", getCharaId()) ;
    HMSLOG("_user_position: %Point(%f, %f)", getUserPosition().x, getUserPosition().y) ;
    HMSLOG("_depend_node_position: %Point(%f, %f)", getDependNodePosition().x, getDependNodePosition().y) ;
    
    if(BottomDirection::LEFT == getBottomDirection()){
        HMSLOG("_bottom_direction: LEFT") ;
    }else if(BottomDirection::RIGHT == getBottomDirection()){
        HMSLOG("_bottom_direction: RIGHT") ;
    }
    
    HMSLOG("_default_user_position: %Point(%f, %f)", getDefaultUserPosition().x, getDefaultUserPosition().y) ;
}


bool BracketUserPositionData::isDefaultPosition()
{
    if(_default_user_position==_user_position){
        return true ;
    }else{
        return false ;
    }
}