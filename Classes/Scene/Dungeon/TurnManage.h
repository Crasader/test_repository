
#ifndef __war__TurnManage__
#define __war__TurnManage__

#include <stdio.h>
#include <math.h>

class TurnManage //インデックスとターンを管理
{
private:
    
public:
    static int player_index;
    static int turn_index;
    static int turn_count;
    
    static void initialize();//初期化
    
    static void setPlayerIndex(int value);
    static void setTurnIndex(int value);
    
    static int getPlayerIndex();
    static int getEnemyIndex();
    static int getPlayerNo();
    static int getEnemyNo();
    static int getTurnIndex();
    static int getWaitIndex();
    static int getTurnNo();
    static int getTurnCount();
    static int getNowTurn();
    static void setTurnCount(int count);
    
    static void changeTurnIndex();//ターン変更時用
};





#endif /* defined(__war__TurnManage__) */
