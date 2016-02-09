#ifndef __war__BracketUserPositionData__
#define __war__BracketUserPositionData__


using namespace std ;
#include "HMSConsoleUtil.h"

class BracketUserPositionData : public Node
{
public:
    
    enum BottomDirection{
        LEFT,
        RIGHT
    };
    
    //インスタンス生成
    static BracketUserPositionData* create(int user_master_id, int round_no, bool is_now_battle, int chara_id, Point user_position, Point depend_node_position) ;
    BracketUserPositionData(int user_master_id, int round_no, bool is_now_battle, int chara_id, Point user_position, Point depend_node_position);
    ~BracketUserPositionData();
 
    CC_SYNTHESIZE(int, _user_master_id, UserMasterId);
    CC_SYNTHESIZE(int, _round_no, RoundNo);
    CC_SYNTHESIZE(bool, _is_now_battle, IsNowBattle);
    CC_SYNTHESIZE(int, _chara_id, CharaId);
    CC_SYNTHESIZE(Point, _user_position, UserPosition);
    CC_SYNTHESIZE(Point, _depend_node_position, DependNodePosition);
    
    //下端のノードから見てキャラが右か左か
    CC_SYNTHESIZE(BottomDirection, _bottom_direction, BottomDirection);
    
    CC_SYNTHESIZE(Point, _default_user_position, DefaultUserPosition);
    
    void echoData() ;
    bool isDefaultPosition() ; //デフォルトの位置と同値の場合true
};

#endif /* defined(__war__BracketUserPositionData__) */
