#include "BracketDrawDataGenerator.h"


BracketDrawDataGenerator* BracketDrawDataGenerator::create(BracketNodeDataList *bracket_node_data_list, const map<int, int> chara_id_by_user_master_id)
{
    BracketDrawDataGenerator *bracketDrawDataGenerator = new BracketDrawDataGenerator(bracket_node_data_list, chara_id_by_user_master_id) ;
    bracketDrawDataGenerator->autorelease() ;
    return bracketDrawDataGenerator ;
}

BracketDrawDataGenerator::BracketDrawDataGenerator(BracketNodeDataList *bracket_node_data_list, const map<int, int> chara_id_by_user_master_id)
: _chara_id_by_user_master_id(chara_id_by_user_master_id)
, _bracket_node_data_list(bracket_node_data_list)
{
    generateMap() ;
}

BracketDrawDataGenerator::~BracketDrawDataGenerator()
{
    HMSLOG("BracketDrawDataGenerator::~BracketDrawDataGenerator()") ;
}

vector<vector<int>> BracketDrawDataGenerator::getBracketDrawData(){
    return _bracket_draw_data ;
}

vector<vector<int>> BracketDrawDataGenerator::getRedBracketDrawData(){
    return _red_bracket_draw_data ;
}

vector<Point> BracketDrawDataGenerator::getNowBattleUserWay(int direction){
    if(direction == 0){
        return _now_battle_user_way_left ;
    }else if(direction == 1){
        return _now_battle_user_way_right ;
    }
}

int BracketDrawDataGenerator::getNowBattleUserMaster(int direction){
    if(direction == 0){
        return _user_master_id_left ;
    }else if(direction == 1){
        return _user_master_id_right ;
    }
}

int BracketDrawDataGenerator::isNowBattleUserMaster(int user_master_id){
    bool contain ;
    int left_user_master_id = getNowBattleUserMaster(0) ;
    int right_user_master_id = getNowBattleUserMaster(1) ;
    if(user_master_id == left_user_master_id || user_master_id == right_user_master_id){
        return true ;
    }else{
        return false ;
    }
}

vector<vector<Point>> BracketDrawDataGenerator::getBattleUserWay(int round_no){
    return _all_battle_user_way[round_no] ;
}

BracketUserPositionData *BracketDrawDataGenerator::getUserPosition(int user_master_id){
    return _user_positions[user_master_id];
}

int BracketDrawDataGenerator::getTopRoundNo(){
    return _top_round_no ;
}

Point BracketDrawDataGenerator::getVsCollisionPoint(){
    return _vs_collision_point;
}

void BracketDrawDataGenerator::generateMap()
{
    int top_node_id = _bracket_node_data_list->getTopNodeId() ;
    BracketNodeData *top_node_data = _bracket_node_data_list->getNodeData(top_node_id) ;
    HMSLOG("頂上のnode_id: %d についての情報", top_node_id) ;
    top_node_data->echoData() ;
    
    initDrawBaseData(top_node_id) ;
    initNodeData(top_node_id) ;
    createBracketDrawData(top_node_id) ;
    createRedBracketDrawData(top_node_id) ;
    createUserWay(top_node_id);
    createUserPosition(top_node_id);
    
    _bracket_node_data_list->echoDataList() ;
}

void BracketDrawDataGenerator::createUserPosition(int node_id)
{
    
    BracketNodeData* node_data = _bracket_node_data_list->getNodeData(node_id) ;
    BracketNodeDataList::WinDirection win_direction = _bracket_node_data_list->getWinDirection(node_id)  ;
    bool is_now_battle_node = node_data->getIsNowBattleNode() ;
    
    int left = node_data->getDrawX() - (node_data->getDrawWidth()-1)/2 ;
    int center = left + node_data->getDrawWidth()/2 ;
    int right = left + node_data->getDrawWidth() ;
    
    if(win_direction==BracketNodeDataList::WinDirection::LEFT){
        int user_master_id = node_data->getLeftUserMasterId() ;
        Point user_position = Point(center,node_data->getDrawY()) ;
        if(!existCheckUserPosition(user_master_id)){
            insertUserPosition(user_master_id, user_position, node_data);
        }
    }else if(win_direction==BracketNodeDataList::WinDirection::RIGHT){
        int user_master_id = node_data->getRightUserMasterId() ;
        Point user_position = Point(center,node_data->getDrawY()) ;
        if(!existCheckUserPosition(user_master_id)){
            insertUserPosition(user_master_id, user_position, node_data);
        }
    }else if(win_direction==BracketNodeDataList::WinDirection::NONE){
        //ノードに関して勝者がいない場合
    }
    
    
    if(is_now_battle_node){
        _vs_collision_point = Point(center,node_data->getDrawY()) ;
        /*
        int left_user_master_id = node_data->getLeftUserMasterId() ;
        Point left_user_position = Point(left,_bracket_draw_data.size()-1);
        if(!existCheckUserPosition(left_user_master_id)){
            insertUserPosition(left_user_master_id, left_user_position, node_data, false);
        }
        
        int right_user_master_id = node_data->getRightUserMasterId() ;
        Point right_user_position = Point(right-1,_bracket_draw_data.size()-1);
        if(!existCheckUserPosition(right_user_master_id)){
            insertUserPosition(right_user_master_id, right_user_position, node_data, false);
        }
         */
    }
    
    //左に子のノードが無い場合、下端にユーザーを置く
    if(!_bracket_node_data_list->hasLeftNode(node_id)){
        //下端にユーザーを置く
        int user_master_id = node_data->getLeftUserMasterId() ;
        Point user_position = Point(left,_bracket_draw_data.size()-1+3);
        if(!existCheckUserPosition(user_master_id)){
            insertUserPosition(user_master_id, user_position, node_data);
        }
        
        //左のノードのデフォルト位置の情報を追加
        _user_positions[user_master_id]->setDefaultUserPosition(user_position) ;
        //下端のキャラの位置を追加
        _user_positions[user_master_id]->setBottomDirection(BracketUserPositionData::BottomDirection::LEFT) ;
    }
    
    if(!_bracket_node_data_list->hasRightNode(node_id)){
        //下端にユーザーを置く
        int user_master_id = node_data->getRightUserMasterId() ;
        Point user_position = Point(right-1,_bracket_draw_data.size()-1+3);
        if(!existCheckUserPosition(user_master_id)){
            insertUserPosition(user_master_id, user_position, node_data);
        }
        
        //右のノードのデフォルト位置の情報を追加
        _user_positions[user_master_id]->setDefaultUserPosition(user_position) ;
        //下端のキャラの位置を追加
        _user_positions[user_master_id]->setBottomDirection(BracketUserPositionData::BottomDirection::RIGHT) ;
    }
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasLeftNode(node_id)){
        int left_node_id = node_data->getLeftTournamentEventBracketNodeId() ;
        createUserPosition(left_node_id) ;
    }
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasRightNode(node_id)){
        int right_node_id = node_data->getRightTournamentEventBracketNodeId() ;
        createUserPosition(right_node_id) ;
    }
    
}

void BracketDrawDataGenerator::insertUserPosition(int user_master_id, Point user_position, BracketNodeData* node_data)
{
    BracketUserPositionData *bracketUserPositionData = BracketUserPositionData::create(user_master_id ,node_data->getRoundNo() ,node_data->getIsNowBattleNode() ,_chara_id_by_user_master_id[user_master_id] ,user_position ,Point(node_data->getDrawX(),node_data->getDrawY())) ;
    this->addChild(bracketUserPositionData) ; //独自クラスをcocosシステムのメモリの管理下に置き、適切なタイミングでdeleteさせるためにaddChild
    _user_positions.insert(std::make_pair(user_master_id, bracketUserPositionData));
}

bool BracketDrawDataGenerator::existCheckUserPosition(int user_master_id)
{
    if(_user_positions.find(user_master_id) == _user_positions.end()) {
        return false ;
    }else{
        return true;
    }
}

void BracketDrawDataGenerator::createUserWay(int node_id)
{
    BracketNodeData* node_data = _bracket_node_data_list->getNodeData(node_id) ;
    BracketNodeDataList::WinDirection win_direction = _bracket_node_data_list->getWinDirection(node_id)  ;
    bool is_now_battle_node = node_data->getIsNowBattleNode() ;
    int round_no = node_data->getRoundNo() ;
    
    vector<vector<Point>> *user_ways = &_all_battle_user_way[round_no] ;
    
    int left = node_data->getDrawX() - (node_data->getDrawWidth()-1)/2 ;
    int center = left + node_data->getDrawWidth()/2 ;
    int right = left + node_data->getDrawWidth() ;
    
    if(win_direction==BracketNodeDataList::WinDirection::LEFT){
        vector<Point> user_way  ;
        
        //ユーザーの通り道
        for(int x=center; x>=left ; x--){
            user_way.push_back(Point(x,node_data->getDrawY()));
        }
        
        //ノードから見て左側の下にのびる棒をたどる
        user_way.push_back(Point(left,node_data->getDrawY()+1));
        user_way.push_back(Point(left,node_data->getDrawY()+2));
        user_way.push_back(Point(left,node_data->getDrawY()+3));
        
        //左に子のノードが無い場合、下端まで棒をのばす
        if(!_bracket_node_data_list->hasLeftNode(node_id)){
            for(int i = node_data->getDrawY()+4; i<_red_bracket_draw_data.size(); i++){
                user_way.push_back(Point(left, i));
            }
        }
        
        reverse(user_way.begin(), user_way.end());
        
        user_ways->push_back(user_way) ;
    }else if(win_direction==BracketNodeDataList::WinDirection::RIGHT){
        vector<Point> user_way  ;
        
        //ユーザーの通り道
        for(int x=center; x<=right-1 ; x++){
            user_way.push_back(Point(x,node_data->getDrawY()));
        }
        
        
        //ノードから見て左側の下にのびる棒をたどる
        user_way.push_back(Point(right-1,node_data->getDrawY()+1));
        user_way.push_back(Point(right-1,node_data->getDrawY()+2));
        user_way.push_back(Point(right-1,node_data->getDrawY()+3));
        
        //右に子のノードが無い場合、下端まで棒をのばす
        if(!_bracket_node_data_list->hasRightNode(node_id)){
            for(int i = node_data->getDrawY()+4; i<_red_bracket_draw_data.size(); i++){
                user_way.push_back(Point(right-1, i));
            }
        }
        
        reverse(user_way.begin(), user_way.end()) ;
        
        user_ways->push_back(user_way) ;
    }else if(win_direction==BracketNodeDataList::WinDirection::NONE){
        //ノードに関して勝者がいない場合
    }
    
    
    if(is_now_battle_node){
        
        //左のユーザーの通り道
        for(int x=center-1; x>=left ; x--){
            _now_battle_user_way_left.push_back(Point(x,node_data->getDrawY()));
        }
        
        //ユーザーの通り道
        _now_battle_user_way_left.push_back(Point(left,node_data->getDrawY()+1));
        _now_battle_user_way_left.push_back(Point(left,node_data->getDrawY()+2));
        _now_battle_user_way_left.push_back(Point(left,node_data->getDrawY()+3));
        
        //左に子のノードが無い場合、下端まで棒をたどる
        if(!_bracket_node_data_list->hasLeftNode(node_id)){
            for(int i = node_data->getDrawY()+4; i<_red_bracket_draw_data.size(); i++){
                //ユーザーの通り道
                _now_battle_user_way_left.push_back(Point(left,i));
            }
        }
        
        reverse(_now_battle_user_way_left.begin(), _now_battle_user_way_left.end()) ;
        _user_master_id_left = node_data->getLeftUserMasterId() ;
        
        
        
        //右のユーザーの通り道
        for(int x=center+1; x<=right-1 ; x++){
            _now_battle_user_way_right.push_back(Point(x,node_data->getDrawY()));
        }
        
        //ユーザーの通り道
        _now_battle_user_way_right.push_back(Point(right-1,node_data->getDrawY()+1));
        _now_battle_user_way_right.push_back(Point(right-1,node_data->getDrawY()+2));
        _now_battle_user_way_right.push_back(Point(right-1,node_data->getDrawY()+3));
        
        //右に子のノードが無い場合、下端まで棒をたどる
        if(!_bracket_node_data_list->hasRightNode(node_id)){
            for(int i = node_data->getDrawY()+4; i<_red_bracket_draw_data.size(); i++){
                //ユーザーの通り道
                _now_battle_user_way_right.push_back(Point(right-1,i));
            }
        }
        
        reverse(_now_battle_user_way_right.begin(), _now_battle_user_way_right.end()) ;
        _user_master_id_right = node_data->getRightUserMasterId() ;
    }
    
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasLeftNode(node_id)){
        int left_node_id = node_data->getLeftTournamentEventBracketNodeId() ;
        createUserWay(left_node_id) ;
    }
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasRightNode(node_id)){
        int right_node_id = node_data->getRightTournamentEventBracketNodeId() ;
        createUserWay(right_node_id) ;
    }
}

void BracketDrawDataGenerator::createRedBracketDrawData(int node_id)
{
    BracketNodeData* node_data = _bracket_node_data_list->getNodeData(node_id) ;
    BracketNodeDataList::WinDirection win_direction = _bracket_node_data_list->getWinDirection(node_id)  ;
    bool is_now_battle_node = node_data->getIsNowBattleNode() ;
    int round_no = node_data->getRoundNo() ;
    
    int left = node_data->getDrawX() - (node_data->getDrawWidth()-1)/2 ;
    int center = left + node_data->getDrawWidth()/2 ;
    int right = left + node_data->getDrawWidth() ;
    if(win_direction==BracketNodeDataList::WinDirection::LEFT){
        //nodeの分かれ道の行を取得し情報に基づき置換
        vector<int> *node_line = &_red_bracket_draw_data[node_data->getDrawY()] ;
        replace(node_line->begin()+left, node_line->begin()+left+1, 0,4) ;
        replace(node_line->begin()+left+1, node_line->begin()+center, 0,2) ;
        replace(node_line->begin()+center, node_line->begin()+center+1, 0,6) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down1 = &_red_bracket_draw_data[node_data->getDrawY()+1] ;
        replace(line_down1->begin()+left, line_down1->begin()+left+1, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down2 = &_red_bracket_draw_data[node_data->getDrawY()+2] ;
        replace(line_down2->begin()+left, line_down2->begin()+left+1, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down3 = &_red_bracket_draw_data[node_data->getDrawY()+3] ;
        replace(line_down3->begin()+left, line_down3->begin()+left+1, 0,1) ;
        
        //左に子のノードが無い場合、下端まで棒をのばす
        if(!_bracket_node_data_list->hasLeftNode(node_id)){
            vector<int> *left_down3 ;
            for(int i = node_data->getDrawY()+3; i<_red_bracket_draw_data.size(); i++){
                left_down3 = &_red_bracket_draw_data[i] ;
                replace(left_down3->begin()+left, left_down3->begin()+left+1, 0,1) ;
            }
        }
    }else if(win_direction==BracketNodeDataList::WinDirection::RIGHT){
        //nodeの分かれ道の行を取得し情報に基づき置換
        vector<int> *node_line = &_red_bracket_draw_data[node_data->getDrawY()] ;
        replace(node_line->begin()+center, node_line->begin()+center+1, 0,7) ;
        replace(node_line->begin()+center+1, node_line->begin()+right-1, 0,2) ;
        replace(node_line->begin()+right-1, node_line->begin()+right, 0,5) ;
        
        //ノードから見て右側の下にのびる棒を作成
        vector<int> *line_down1 = &_red_bracket_draw_data[node_data->getDrawY()+1] ;
        replace(line_down1->begin()+right-1, line_down1->begin()+right, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down2 = &_red_bracket_draw_data[node_data->getDrawY()+2] ;
        replace(line_down2->begin()+right-1, line_down2->begin()+right, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down3 = &_red_bracket_draw_data[node_data->getDrawY()+3] ;
        replace(line_down3->begin()+right-1, line_down3->begin()+right, 0,1) ;
        
        //右に子のノードが無い場合、下端まで棒をのばす
        if(!_bracket_node_data_list->hasRightNode(node_id)){
            vector<int> *right_down3 ;
            for(int i = node_data->getDrawY()+3; i<_red_bracket_draw_data.size(); i++){
                right_down3 = &_red_bracket_draw_data[i] ;
                replace(right_down3->begin()+right-1, right_down3->begin()+right, 0,1) ;
            }
        }
    }else if(win_direction==BracketNodeDataList::WinDirection::NONE){
        //ノードに関して勝者がいない場合
    }
    
    if(is_now_battle_node){
        
        //nodeの分かれ道の行を取得し情報に基づき置換
        vector<int> *node_line_left_side = &_red_bracket_draw_data[node_data->getDrawY()] ;
        replace(node_line_left_side->begin()+left, node_line_left_side->begin()+left+1, 0,4) ;
        replace(node_line_left_side->begin()+left+1, node_line_left_side->begin()+center, 0,2) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down_left_side1 = &_red_bracket_draw_data[node_data->getDrawY()+1] ;
        replace(line_down_left_side1->begin()+left, line_down_left_side1->begin()+left+1, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down_left_side2 = &_red_bracket_draw_data[node_data->getDrawY()+2] ;
        replace(line_down_left_side2->begin()+left, line_down_left_side2->begin()+left+1, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down_left_side3 = &_red_bracket_draw_data[node_data->getDrawY()+3] ;
        replace(line_down_left_side3->begin()+left, line_down_left_side3->begin()+left+1, 0,1) ;
        
        //左に子のノードが無い場合、下端まで棒をのばす
        if(!_bracket_node_data_list->hasLeftNode(node_id)){
            vector<int> *left_down ;
            for(int i = node_data->getDrawY()+3; i<_red_bracket_draw_data.size(); i++){
                left_down = &_red_bracket_draw_data[i] ;
                replace(left_down->begin()+left, left_down->begin()+left+1, 0,1) ;
                
            }
        }
        
        //nodeの分かれ道の行を取得し情報に基づき置換
        vector<int> *node_line_right_side = &_red_bracket_draw_data[node_data->getDrawY()] ;
        replace(node_line_right_side->begin()+center+1, node_line_right_side->begin()+right-1, 0,2) ;
        replace(node_line_right_side->begin()+right-1, node_line_right_side->begin()+right, 0,5) ;
        
        
        //ノードから見て右側の下にのびる棒を作成
        vector<int> *line_down_right_side1 = &_red_bracket_draw_data[node_data->getDrawY()+1] ;
        replace(line_down_right_side1->begin()+right-1, line_down_right_side1->begin()+right, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down_right_side2 = &_red_bracket_draw_data[node_data->getDrawY()+2] ;
        replace(line_down_right_side2->begin()+right-1, line_down_right_side2->begin()+right, 0,1) ;
        
        //ノードから見て左側の下にのびる棒を作成
        vector<int> *line_down_right_side3 = &_red_bracket_draw_data[node_data->getDrawY()+3] ;
        replace(line_down_right_side3->begin()+right-1, line_down_right_side3->begin()+right, 0,1) ;
        
        //右に子のノードが無い場合、下端まで棒をのばす
        if(!_bracket_node_data_list->hasRightNode(node_id)){
            vector<int> *right_down ;
            for(int i = node_data->getDrawY()+3; i<_red_bracket_draw_data.size(); i++){
                right_down = &_red_bracket_draw_data[i] ;
                replace(right_down->begin()+right-1, right_down->begin()+right, 0,1) ;
                
            }
        }
        
    }
    
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasLeftNode(node_id)){
        int left_node_id = node_data->getLeftTournamentEventBracketNodeId() ;
        createRedBracketDrawData(left_node_id) ;
    }
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasRightNode(node_id)){
        int right_node_id = node_data->getRightTournamentEventBracketNodeId() ;
        createRedBracketDrawData(right_node_id) ;
    }
}

void BracketDrawDataGenerator::createBracketDrawData(int node_id)
{
    BracketNodeData* node_data = _bracket_node_data_list->getNodeData(node_id) ;
    
    int left = node_data->getDrawX() - (node_data->getDrawWidth()-1)/2 ;
    int center = left + node_data->getDrawWidth()/2 ;
    int right = left + node_data->getDrawWidth() ;
    
    //nodeの分かれ道の行を取得し情報に基づき置換
    vector<int> *node_line = &_bracket_draw_data[node_data->getDrawY()] ;
    replace(node_line->begin()+left, node_line->begin()+left+1, 0,4) ;
    replace(node_line->begin()+left+1, node_line->begin()+center, 0,2) ;
    replace(node_line->begin()+center, node_line->begin()+center+1, 0,3) ;
    replace(node_line->begin()+center+1, node_line->begin()+right-1, 0,2) ;
    replace(node_line->begin()+right-1, node_line->begin()+right, 0,5) ;
    
    //nodeの分かれ道の上の行を取得し情報に基づき置換
    vector<int> *line_up1 = &_bracket_draw_data[node_data->getDrawY()-1] ;
    replace(line_up1->begin()+center, line_up1->begin()+center+1, 0,1) ;
    
    //nodeの分かれ道の上の行を取得し情報に基づき置換
    vector<int> *line_up2 = &_bracket_draw_data[node_data->getDrawY()-2] ;
    replace(line_up2->begin()+center, line_up2->begin()+center+1, 0,1) ;
    
    //nodeの分かれ道の上の行を取得し情報に基づき置換
    vector<int> *line_up3 = &_bracket_draw_data[node_data->getDrawY()-3] ;
    replace(line_up3->begin()+center, line_up3->begin()+center+1, 0,1) ;
    
    
    //左に子のノードが無い場合、下端まで棒をのばし、最終行にはユーザーを入れる
    if(!_bracket_node_data_list->hasLeftNode(node_id)){
        vector<int> *left_line ;
        for(int i = node_data->getDrawY(); i<_bracket_draw_data.size(); i++){
            left_line = &_bracket_draw_data[i] ;
            replace(left_line->begin()+left, left_line->begin()+left+1, 0,1) ;
        }
    }
    
    //右に子のノードが無い場合、下端まで棒をのばし、最終行にはユーザーを入れる
    if(!_bracket_node_data_list->hasRightNode(node_id)){
        vector<int> *right_line ;
        for(int i = node_data->getDrawY(); i<_bracket_draw_data.size(); i++){
            right_line = &_bracket_draw_data[i] ;
            replace(right_line->begin()+right-1, right_line->begin()+right, 0,1) ;
        }
    }
    
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasLeftNode(node_id)){
        int left_node_id = node_data->getLeftTournamentEventBracketNodeId() ;
        createBracketDrawData(left_node_id) ;
    }
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasRightNode(node_id)){
        int right_node_id = node_data->getRightTournamentEventBracketNodeId() ;
        createBracketDrawData(right_node_id) ;
    }
}

void BracketDrawDataGenerator::initNodeData(int node_id)
{
    BracketNodeData* node_data = _bracket_node_data_list->getNodeData(node_id) ;
    //長さを決める
    int draw_width = getDrawWidth(node_data->getRoundNo()) ;
    node_data->setDrawWidth(draw_width) ;
    
    //ノードの縦方向の位置を決定する
    node_data->setDrawY((_top_round_no-node_data->getRoundNo())*4+3) ;
    
    //ノードの横方向の位置を決める
    if(_bracket_node_data_list->isTopNode(node_id)) {
        node_data->setDrawX(_root_node_center_place) ;
    }else{
        //親の情報を取得
        int next_node_id = node_data->getNextTournamentEventBracketNodeId() ;
        BracketNodeData* next_node_data = _bracket_node_data_list->getNodeData(next_node_id) ;
        int parent_draw_center_place = next_node_data->getDrawX() ;
        int parent_arm_width = (next_node_data->getDrawWidth() - 1)/2 ; //中心をのぞいた左右の長さ
        
        ///親の位置から相対的に自分の位置を決定する
        if(_bracket_node_data_list->getDirectionAtParent(node_id) == 0){
            node_data->setDrawX(parent_draw_center_place-parent_arm_width) ;
        }else if(_bracket_node_data_list->getDirectionAtParent(node_id) == 1){
            node_data->setDrawX(parent_draw_center_place+parent_arm_width) ;
        }
    }
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasLeftNode(node_id)){
        int left_node_id = node_data->getLeftTournamentEventBracketNodeId() ;
        initNodeData(left_node_id) ;
    }
    
    //子のノードがある場合、そのノードの情報も決める
    if(_bracket_node_data_list->hasRightNode(node_id)){
        int right_node_id = node_data->getRightTournamentEventBracketNodeId() ;
        initNodeData(right_node_id) ;
    }
    
}


int BracketDrawDataGenerator::getDrawWidth(int round_no)
{
    if(1 <= round_no && round_no <= _draw_width_list.size()){
        return _draw_width_list[round_no-1] ;
    }else{
        HMSLOG("想定外のround_no:%d", round_no) ;
        return 0 ;
    }
        
}

void BracketDrawDataGenerator::initDrawBaseData(int top_node_id)
{
    BracketNodeData *top_node_data = _bracket_node_data_list->getNodeData(top_node_id) ;
    _top_round_no = top_node_data->getRoundNo() ;
    
    //各ノードからのびる横方向の棒の長さをラウンド毎に決定する
    for(int i=0; i<_top_round_no; i++){
        int draw_width ;
        if(i==0){
            draw_width=5;
        }else{
            draw_width=_draw_width_list[i-1]*2+1 ;
        }
        _draw_width_list.push_back(draw_width) ;
    }
    int sum = accumulate(_draw_width_list.begin(), _draw_width_list.end(), 0) ;
    //ノードの中心位置を決定する
    _root_node_center_place = (sum - _draw_width_list.size())/2 ;
    //描画範囲を決定
    _draw_width_max_renge = _root_node_center_place*2+1 ;
    
    
    //vectorの初期化
    _bracket_draw_data = vector<vector<int>>(_top_round_no*4+3) ;
    for(int i=0; i<_bracket_draw_data.size(); i++){
        vector<int> line(_draw_width_max_renge, 0);
        _bracket_draw_data[i] = line ;
    }
    
    //vectorの初期化
    _red_bracket_draw_data = vector<vector<int>>(_top_round_no*4+3) ;
    for(int i=0; i<_red_bracket_draw_data.size(); i++){
        vector<int> line(_draw_width_max_renge, 0);
        _red_bracket_draw_data[i] = line ;
    }
    
}

void BracketDrawDataGenerator::debug_test(){
    HMSLOG("----------test--------------------") ;
    for(vector<vector<int>>::iterator iterator = _bracket_draw_data.begin(); iterator != _bracket_draw_data.end(); iterator++){
        
        vector<int> line = *iterator ;
        string line_str ;
        for(std::vector<int>::iterator iterator = line.begin(); iterator != line.end(); iterator++){
            line_str += toString<int>(*iterator);
        }
        
        HMSLOG("%s", line_str.c_str()) ;
    }
    
    HMSLOG("----------test(red)--------------------") ;
    for(vector<vector<int>>::iterator iterator = _red_bracket_draw_data.begin(); iterator != _red_bracket_draw_data.end(); iterator++){
        
        vector<int> line = *iterator ;
        string line_str ;
        for(std::vector<int>::iterator iterator = line.begin(); iterator != line.end(); iterator++){
            line_str += toString<int>(*iterator);
        }
        
        HMSLOG("%s", line_str.c_str()) ;
    }
    
    HMSLOG("----------test(user_positions)--------------------") ;
    for(map<int, BracketUserPositionData*>::iterator iterator = _user_positions.begin(); iterator != _user_positions.end(); iterator++){
        HMSLOG(" -------------------------------------");
        HMSLOG("user_master_id:%d", iterator->first) ;
        HMSLOG("   ----------Member variables of BracketUserPositionData:-----------");
        iterator->second->echoData() ;
    }
}


