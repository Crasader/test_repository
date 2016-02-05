#ifndef __war__BracketDrawDataGenerator__
#define __war__BracketDrawDataGenerator__

#include "BracketNodeDataList.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "BracketUserPositionData.h"
#include "json11.hpp"

using namespace std ;
using namespace json11 ;

/*

 */
class BracketDrawDataGenerator : public Node
{
    
private:
    BracketNodeDataList *_bracket_node_data_list ;
    map<int, int> _chara_id_by_user_master_id ;
    void generateMap() ;
    int getTopNodeId() ; //頂点のnode_idを取得
    
    
    vector<vector<int>> _bracket_draw_data ; //対戦表情報
    vector<vector<int>> _red_bracket_draw_data ; //対戦表情報(赤)
    
    vector<Point> _now_battle_user_way_left;
    vector<Point> _now_battle_user_way_right;
    int _user_master_id_left ;
    int _user_master_id_right ;
    
    map<int, vector<vector<Point>>> _all_battle_user_way;// map<round_id, [ユーザーの通り道を入れたvectorを挿入]>
    
    map<int, BracketUserPositionData*> _user_positions; //key: user_master_id
    Point _vs_collision_point ; 
    
    
    //横幅 0:1round目の横幅 1:2round目 ...
    vector<int> _draw_width_list;
    void initDrawBaseData(int top_node_id) ;
    void initNodeData(int node_id) ;//ノードの情報を決定する
    void createBracketDrawData(int node_id) ; //描画情報を作成する
    void createRedBracketDrawData(int node_id) ; //赤い部分の描画情報を作成する
    void createUserWay(int node_id) ; //ユーザーの通り道を作成
    void createUserPosition(int node_id) ; //ユーザーの場所を決定
    
    void insertUserPosition(int user_master_id, Point user_position, BracketNodeData* node_data);
    bool existCheckUserPosition(int user_master_id);
    
    int _root_node_center_place ; //頂点のラウンドの中心位置
    int _draw_width_max_renge ; //描画の横幅
    int _top_round_no ;//頂点のラウンドno
    
    int getDrawWidth(int round_no);
public:
    static BracketDrawDataGenerator* create(BracketNodeDataList *bracket_node_data_list, const map<int, int> chara_id_by_user_master_id) ; //インスタンス生成
    BracketDrawDataGenerator(BracketNodeDataList *bracket_node_data_list, const map<int, int> _chara_id_by_user_master_id);
    ~BracketDrawDataGenerator();
    vector<vector<int>> getBracketDrawData() ; //
    vector<vector<int>> getRedBracketDrawData() ;
    BracketUserPositionData *getUserPosition(int user_master_id);
    Point getVsCollisionPoint();
    
    vector<Point> getNowBattleUserWay(int direction); //0:左 1:右
    int getNowBattleUserMaster(int direction); //0:左 1:右
    int isNowBattleUserMaster(int user_master_id); //今回バトルするユーザーに含まれる
    vector<vector<Point>> getBattleUserWay(int round_no);
    int getTopRoundNo();
    
    /*
     getBracketDrawData() の戻り値はこのような２次元のvector
     0000000000000000001000000000000000000
     0000000000000000001000000000000000000
     0000000422222222223222222222250000000
     0000000100000000000000000000010000000
     0000000100000000000000000000010000000
     0042222322225000000000004222232222500
     0010000000001000000000001000000000100
     0010000000001000000000001000000000100
     4232500000423250000000423250000042325
     1000100000100010000000100010000010001
     1000100000100010000000100010000010001
     9000900000900090000000900090000090009
     
     getRedBracketDrawData() の戻り値はこのような２次元のvector
     0000000000000000000000000000000000000
     0000000000000000000000000000000000000
     0000000000000000000000000000000000000
     0000000000000000000000000000000000000
     0000000000000000000000000000000000000
     0042222600000000000000004222202222500
     0010000000000000000000001000000000100
     0010000000000000000000001000000000100
     4260000000426000000000007250000000725
     1000000000100000000000000010000000001
     1000000000100000000000000010000000001
     0000000000000000000000000000000000000
     */
    
private:
    void debug_test();
};

#endif /* defined(__war__BracketDrawDataGenerator__) */