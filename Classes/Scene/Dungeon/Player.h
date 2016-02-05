#ifndef __war__Player__
#define __war__Player__

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class Player : public Node //プレイヤーのステータスを保存しているクラス
{
    CC_SYNTHESIZE(string, _name, Name);
    CC_PROPERTY(int, _hp, Hp);
    CC_PROPERTY(int, _max_hp, MaxHp);
    CC_PROPERTY(int, _power, Power);
    CC_SYNTHESIZE(int, _power_default, PowerDefault);//基本士気
    CC_SYNTHESIZE(int, _power_old, PowerOld);//保存用士気
    CC_PROPERTY(int, _now_power, NowPower);
    CC_SYNTHESIZE(int, _start_add_power, StartAddPower);//初ターンに増加する士気
public:
    void damage(int value, bool insert_flg = true);//ダメージを与える時はこれを使う
    int directDamage(int value);//スキル攻撃によるダメージ
    int heal(int value);//回復する時はこれを使う
    void consumePower(int consume_power);
    void generatePower(int generate_power);
    
    //全体効果バッファ使用キャラprimid と 値 と 状態異常id と 残りターン
    vector< map<string, int> > whole_status_active;//"primary" "value" "state" "turn"
    
    //かかっているパッシブ。かけたキャラのプライマリid とターン変化効果の値 と　状態異常のid
    vector< map<string, int> > whole_status_passive;//"primary" "value" "state"*/
    
    CREATE_FUNC(Player);
};
#endif /* defined(__war__Player__) */
