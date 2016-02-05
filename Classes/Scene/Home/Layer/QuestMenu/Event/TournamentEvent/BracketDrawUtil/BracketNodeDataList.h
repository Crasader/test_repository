#ifndef __war__BracketNodeDataList__
#define __war__BracketNodeDataList__

#include "BracketNodeData.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"

using namespace std ;
using namespace json11 ;

class BracketNodeDataList : public Node
{
private:
    map<int, BracketNodeData*> node_data_list;
    
public:
    
    enum WinDirection
    {
        NONE,
        RIGHT,
        LEFT
    };
    
    static BracketNodeDataList* create(Json tournament_event_bracket_node_list) ; //インスタンス生成
    BracketNodeDataList(Json tournament_event_bracket_node_list);
    ~BracketNodeDataList();
//    void setDrawCenterPlace(int node_id, int draw_center_place) ;
 //   void setDrawWidth(int node_id, int draw_width) ;
    void echoDataList() ;
    
    HmsVector<HmsVector<int>> _bracket_draw_data ;
    BracketNodeData* getNodeData(int node_id) ;
    int getNextNodeId(int node_id) ;
    int getLeftNodeId(int node_id) ;
    int getRightNodeId(int node_id) ;
    int getTopNodeId();
    bool hasLeftNode(int node_id) ;
    bool hasRightNode(int node_id) ;
    bool isTopNode(int node_id) ;
    WinDirection getWinDirection(int node_id) ;
    bool getDirectionAtParent(int node_id) ; //親から見た時の自分の方向 0:left 1:right
    
};

#endif /* defined(__war__BracketNodeDataList__) */