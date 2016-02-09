
#include "TurnManage.h"

int TurnManage::player_index = 0;//これを入れないとエラーに・・・
int TurnManage::turn_index = 0;
int TurnManage::turn_count = 0;


void TurnManage::initialize(){
    player_index = 0;
    turn_index = 0;
    turn_count = 0;
}


void TurnManage::setPlayerIndex(int value){
    player_index = value;
}
void TurnManage::setTurnIndex(int value){
    turn_index = value;
}


int TurnManage::getPlayerIndex(){
    return player_index;
}
int TurnManage::getEnemyIndex(){
    return (player_index + 1) % 2;
}
int TurnManage::getPlayerNo(){
    return player_index + 1;
}
int TurnManage::getEnemyNo(){
    return (player_index + 1) % 2 + 1;
}
int TurnManage::getTurnIndex(){
    return turn_index;
}
int TurnManage::getWaitIndex(){
    return (turn_index + 1) % 2;
}
int TurnManage::getTurnNo(){
    return turn_index + 1;
}
int TurnManage::getTurnCount(){
    return turn_count;
}
void TurnManage::setTurnCount(int count){
    turn_count = count;
}
int TurnManage::getNowTurn(){
    return (int)ceil(getTurnCount()/2.0f);
}

void TurnManage::changeTurnIndex(){
    setTurnIndex((getTurnIndex()+1)%2);
}



