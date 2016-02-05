
#include "Player.h"
#include "DungeonScene.h"
#include "TurnManage.h"
#include "CharaList.h"
#include "Chara.h"

int Player::getHp(){
    return _hp;
}
void Player::setHp(int hp){//HPを範囲内に矯正
    if(hp > getMaxHp()){
        _hp = getMaxHp();
    }else if(hp < 0){
        _hp = 0;
    }else{
        _hp = hp;
    }
}
void Player::damage(int value, bool insert_flg){//ダメージを与える時はここから
    DungeonScene *dung = (DungeonScene *)this->getParent();
    if(value <= 0) return;
    
    
    if(getHp() >= 2 && getHp() - value <= 0){//HP2以上で、死ぬ時で
        if(this == dung->player[TurnManage::getPlayerIndex()]){//プレイヤー側がダメージを受ける時
            for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())){
                if(chara->statusCheck(STATUS_TYPE_ENDURE)){//HP1で耐えるスキルを持っていた時
                    value = getHp() - 1;//HP1で耐えるようにする
                    break;
                }
            }
        }else{
            for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())){
                if(chara->statusCheck(STATUS_TYPE_ENDURE)){//HP1で耐えるスキルを持っていた時
                    value = getHp() - 1;//HP1で耐えるようにする
                    break;
                }
            }
        }
    }
    
    if(this == dung->player[TurnManage::getPlayerIndex()]){
        dung->param_nodamage_flg = 0;//操作側がダメージを負った場合、ノーダメージフラグを０
        dung->score_param_damage += value;//食らったダメージ量を加算
        CCLOG("被ダメージ%d",value);
    }
    if(this == dung->player[TurnManage::getEnemyIndex()]){
        dung->score_param_attack_damage += value;//敵に与えたダメージ量を加算
        CCLOG("与ダメージ%d",value);
        
        if(dung->isBossEventBattle() && insert_flg==true) {
            dung->damage_list.insert(value);
        }
    }
    
    int rest_hp = getHp() - value;
    if(rest_hp < 0 && this == dung->player[TurnManage::getEnemyIndex()]){//HPがマイナスでこのプレイヤーが敵だった場合、オーバーキルを加算する
        CCLOG("オーバーキルダメージ%d",abs(rest_hp));
        dung->score_param_overkill += abs(rest_hp);
    }
    
    if(rest_hp<=0) dung->game_end_flg = true;//HP０以下ならゲーム終了
        
    setHp(rest_hp);
}

int Player::directDamage(int value){//スキル攻撃によるダメージ
    DungeonScene *dungeon_scene = (DungeonScene *)this->getParent();
    int index = 0;//このプレイヤーのインデックス
    if(this == dungeon_scene->player[1]) index = 1;
    int rival_index = (index+1)%2;//相手プレイヤーのインデックス
    
    float damage_up_par = dungeon_scene->wholeStatusValue(STATUS_TYPE_SKILL_DAMAGE_UP, rival_index, "sum");
    float damege_cut_par = dungeon_scene->wholeStatusValueMultiply(STATUS_TYPE_SKILL_DAMAGE_CUT, index);
    
    int damage_value = (100.0f + damage_up_par)/100.0f * (100.0f - damege_cut_par)/100.0f * value;
    
    damage(damage_value);
    return damage_value;
}

int Player::heal(int value){//回復をする時はここから 最終的な回復量を返す
    if(value <= 0) return 0;
    if(getHp() <= 0) return 0;//死んでる時は回復不可
    DungeonScene *dungeon_scene = (DungeonScene *)this->getParent();
    int index =0;
    if(this == dungeon_scene->player[1]) index = 1;
    
    int heal_value = (100.0f - dungeon_scene->wholeStatusValueMultiply(STATUS_TYPE_HEAL_JAMMING, index)) / 100.0f * value;
    int rest_hp = getHp() + heal_value;
    setHp(rest_hp);
    return heal_value;
}

int Player::getMaxHp(){
    return _max_hp;
}

void Player::setMaxHp(int max_hp){//最大HPを範囲内に矯正
    int rate = 1;
    //ボスイベントは最大HPを100倍=99900に訂正
    DungeonScene *dungeon_scene = (DungeonScene *)this->getParent();
    if(dungeon_scene->isBossEventBattle()) rate = 100;
    
    if(max_hp > MAX_CHARA_HP * rate){
        _max_hp = MAX_CHARA_HP * rate;
    }else if(max_hp < MIN_CHARA_HP){
        _max_hp = MIN_CHARA_HP;
    }else{
        _max_hp = max_hp;
    }
    
    if(_hp > _max_hp){//HPが最大HPを超えないよう矯正
        _hp = _max_hp;
    }
}

int Player::getPower(){
    for(int i=0; i<whole_status_active.size(); i++){
        if(whole_status_active.at(i)["state"] == STATUS_TYPE_FIXED_POWER){
            CCLOG("フィックスド%d",whole_status_active.at(i)["value"]);
            return whole_status_active.at(i)["value"];
        }
    }
    
    return _power;
}
void Player::setPower(int power){//上昇士気を範囲内に矯正
    if(power > 99){
        _power = 99;
    }else if(power < 0){
        _power = 0;
    }else{
        _power = power;
    }
}

int Player::getNowPower(){
    return _now_power;
}

void Player::setNowPower(int now_power){
    
    if(now_power > MAX_POWER_COST){
        _now_power = MAX_POWER_COST;
    }else if(now_power < 0){
        _now_power = 0;
    }else{
        _now_power = now_power;
    }
}

void Player::consumePower(int consume_power){//士気消費
    
    setNowPower(getNowPower() - consume_power);
    
    
}

void Player::generatePower(int generate_power){//士気回復
    DungeonScene *dung = (DungeonScene *)this->getParent();
    int index = 0;
    if(this != dung->player[index]) index = 1;
    
    int limited_cost = dung->wholeStatusValue(STATUS_TYPE_LIMITED_MAX_COST, index, "min");
    if(limited_cost > 0){
        if(limited_cost <= getNowPower()){//既にリミット以上の時
            //士気を上げない

        }else if(getNowPower()+generate_power >= limited_cost){//まだリミットに行ってないが、足すとリミットに行く時
            setNowPower(limited_cost);
        }else{//足してもリミットに行かない時は普通
            setNowPower(getNowPower() + generate_power);
        }
    }else{
        setNowPower(getNowPower() + generate_power);
    }
}






