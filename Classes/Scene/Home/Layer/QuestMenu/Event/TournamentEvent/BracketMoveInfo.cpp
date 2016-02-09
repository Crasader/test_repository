#include "BracketMoveInfo.h"

BracketMoveInfo* BracketMoveInfo::create()
{
    BracketMoveInfo *bracketMoveInfo = new BracketMoveInfo() ;
    bracketMoveInfo->autorelease() ;
    return bracketMoveInfo ;
}

BracketMoveInfo::BracketMoveInfo()
{
}

BracketMoveInfo::~BracketMoveInfo()
{
    HMSLOG("BracketMoveInfo::~BracketMoveInfo()") ;
}

void BracketMoveInfo::echoData() {
}

bool BracketMoveInfo::hasValidDestination(Distination distination)
{
    if(distination==Right){
        return _right_node_find ;
    }else if(distination==Left){
        return _left_node_find ;
    }else if(distination==LowerLeft){
        return _lower_left_node_find;
    }else if(distination==LowerRight){
        return _lower_right_node_find ;
    }else if(distination==Up){
        return _up_node_find ;
    }
    return false ;
}

Point BracketMoveInfo::getValidDistination(Distination distination)
{
    if(distination==Right){
        return _near_right ;
    }else if(distination==Left){
        return _near_left;
    }else if(distination==LowerLeft){
        return _near_lower_left;
    }else if(distination==LowerRight){
        return _near_lower_right;
    }else if(distination==Up){
        return _near_up;
    }
    
    HMSLOG("存在しない列挙型が渡された") ;
    return Point(0,0) ;
}