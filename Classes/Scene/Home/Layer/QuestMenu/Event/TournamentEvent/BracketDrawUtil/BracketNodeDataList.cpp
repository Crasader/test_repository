#include "BracketNodeDataList.h"

BracketNodeDataList* BracketNodeDataList::create(Json tournament_event_bracket_node_list)
{
    BracketNodeDataList *bracketNodeDataList = new BracketNodeDataList(tournament_event_bracket_node_list) ;
    bracketNodeDataList->autorelease() ;
    return bracketNodeDataList ;
}

BracketNodeDataList::BracketNodeDataList(Json tournament_event_bracket_node_list)
{

    int jsonCount = countJsonData(tournament_event_bracket_node_list) ;
    for(int i=0; i<jsonCount; i++){
        Json node = tournament_event_bracket_node_list[toString<int>(i)].object_items() ;
        
        BracketNodeData *nodedata = BracketNodeData::create();
        
        nodedata->setTournamentEventBracketNodeId(fromString<int>(node["tournament_event_bracket_node_id"].string_value())) ;
        nodedata->setTournamentEventListId(fromString<int>(node["tournament_event_list_id"].string_value())) ;
        nodedata->setNextTournamentEventBracketNodeId(fromString<int>(node["next_tournament_event_bracket_node_id"].string_value())) ;
        nodedata->setLeftTournamentEventBracketNodeId(fromString<int>(node["left_tournament_event_bracket_node_id"].string_value())) ;
        nodedata->setRightTournamentEventBracketNodeId(fromString<int>(node["right_tournament_event_bracket_node_id"].string_value())) ;
        nodedata->setLeftUserMasterId(fromString<int>(node["left_user_master_id"].string_value())) ;
        nodedata->setRightUserMasterId(fromString<int>(node["right_user_master_id"].string_value())) ;
        nodedata->setBattleListId(fromString<int>(node["battle_list_id"].string_value())) ;
        nodedata->setRoundNo(fromString<int>(node["round_no"].string_value())) ;
        nodedata->setWaitStartDate(node["wait_start_date"].string_value()) ;
        
        nodedata->setWinDirection(node["win_direction"].string_value()) ;
        nodedata->setIsNowBattleNode(node["is_now_battle_node"].bool_value()) ;
        
        node_data_list[fromString<int>(node["tournament_event_bracket_node_id"].string_value())] = nodedata ;
    }
}

BracketNodeDataList::~BracketNodeDataList()
{
    HMSLOG("BracketNodeDataList::~BracketNodeDataList()") ;
}

void BracketNodeDataList::echoDataList() {
    for(map<int, BracketNodeData*>::iterator iterator = node_data_list.begin(); iterator != node_data_list.end(); iterator++){
        HMSLOG("------------------------------") ;
        HMSLOG("node_id: %d についての情報", iterator->first) ;
        
        BracketNodeData *nodedata = iterator->second ;
        nodedata->echoData() ;
    }
}


BracketNodeData *BracketNodeDataList::getNodeData(int node_id)
{
    return node_data_list[node_id] ;
}

int BracketNodeDataList::getNextNodeId(int node_id)
{
    return node_data_list[node_id]->getNextTournamentEventBracketNodeId() ;
}

int BracketNodeDataList::getLeftNodeId(int node_id)
{
    return node_data_list[node_id]->getLeftTournamentEventBracketNodeId() ;
}

int BracketNodeDataList::getRightNodeId(int node_id)
{
    return node_data_list[node_id]->getRightTournamentEventBracketNodeId() ;
}

bool BracketNodeDataList::hasLeftNode(int node_id)
{
    if(getLeftNodeId(node_id) == 0){
        return false ;
    }else{
        return true ;
    }
}

bool BracketNodeDataList::hasRightNode(int node_id)
{
    if(getRightNodeId(node_id) == 0){
        return false ;
    }else{
        return true ;
    }
}

bool BracketNodeDataList::getDirectionAtParent(int node_id)
{
    BracketNodeData* node_data = getNodeData(node_id) ;
    int parent_node_id = node_data->getNextTournamentEventBracketNodeId() ;
    BracketNodeData* parent_node_data = getNodeData(parent_node_id) ;
    
    if(node_id == parent_node_data->getLeftTournamentEventBracketNodeId()){
        return 0;
    }else if(node_id == parent_node_data->getRightTournamentEventBracketNodeId()){
        return 1;
    }else{
        return -1 ;
    }
}

bool BracketNodeDataList::isTopNode(int node_id)
{
    if(getNextNodeId(node_id)==0){
        return true ;
    }else{
        return false ;
    }
}

BracketNodeDataList::WinDirection BracketNodeDataList::getWinDirection(int node_id)
{
    BracketNodeData* node_data = getNodeData(node_id) ;
    string win_direction = node_data->getWinDirection() ;
    if(win_direction=="right"){
        return WinDirection::RIGHT;
    }else if(win_direction=="left"){
        return WinDirection::LEFT ;
    }else{
        return WinDirection::NONE ;
    }
}

int BracketNodeDataList::getTopNodeId(){
    BracketNodeData *nodedata ;
    for(map<int, BracketNodeData*>::iterator iterator = node_data_list.begin(); iterator != node_data_list.end(); iterator++){
        nodedata = iterator->second ;
        int next_id = nodedata->getNextTournamentEventBracketNodeId() ;
        if(next_id==0){
            return nodedata->getTournamentEventBracketNodeId() ;
        }
    }
    
    HMSLOG("異常") ;
    return -1;
}


