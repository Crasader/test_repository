#ifndef __war__BracketMoveInfo__
#define __war__BracketMoveInfo__

#include "HMSConsoleUtil.h"

USING_NS_CC;
using namespace std ;

class BracketMoveInfo : public Node
{
public:
    static BracketMoveInfo* create() ; //インスタンス生成
    BracketMoveInfo();
    ~BracketMoveInfo();
 
    CC_SYNTHESIZE(Point, _near_right, NearRight); //右
    CC_SYNTHESIZE(Point, _near_left, NearLeft); //最も近い左の位置
    CC_SYNTHESIZE(Point, _near_lower_left, NearLowerLeft); //左下
    CC_SYNTHESIZE(Point, _near_lower_right, NearLowerRight); //右下
    CC_SYNTHESIZE(Point, _near_up, NearUp); //上
    
    CC_SYNTHESIZE(bool, _right_node_find, RightNodeFind); //右のノードを発見
    CC_SYNTHESIZE(bool, _left_node_find, LeftNodeFind); //左のノードを発見
    CC_SYNTHESIZE(bool, _lower_left_node_find, LowerLeftNodeFind) ; //左下のノードを発見
    CC_SYNTHESIZE(bool, _lower_right_node_find, LowerRightNodeFind); ///右下のノードを発見
    CC_SYNTHESIZE(bool, _up_node_find, UpNodeFind); //上のノードを発見
    
    //移動先
    enum Distination{
        Right,
        Left,
        LowerLeft,
        LowerRight,
        Up
    };
    
    bool hasValidDestination(Distination distination) ; //与えられた方向に有効な移動先があるか
    Point getValidDistination(Distination distination) ; //与えられた方向の有効な移動先を返す
    
    void echoData() ;
    
};

#endif /* defined(__war__BracketMoveInfo__) */
