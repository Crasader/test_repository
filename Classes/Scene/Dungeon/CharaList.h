
#ifndef __war__CharaList__
#define __war__CharaList__

#include <stdio.h>
#include "Chara.h"


class CharaList //すべてのキャラリスト
{
private:
    
public:
    static HmsVector<Chara *> chara_list;
    
    static void initialize();//初期化
    
    static HmsVector<Chara *> getAll();//すべてのキャラを取得
    static HmsVector<Chara *> getPlayer();//プレイヤーキャラを取得
    static HmsVector<Chara *> getEnemy();//エネミーキャラを取得
    static HmsVector<Chara *> getOneSide(int index);//プレイヤーかエネミーを指定して取得
    
    static Chara* getAtPrimary(int primary_id);//プライマリからキャラを取得
    static Chara* getAtField(int index, int field_index);//フィールドからキャラ取得
    static Chara* getLeader(int index);
    static Chara* getFriend(int index);
    
};
#endif /* defined(__war__CharaList__) */
