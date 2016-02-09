/*全体効果のパッシブスキル（軍師など）のロジック*/


#include "PassiveSkill.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "CharaList.h"

string PassiveSkill::skillLogic(int used_primary){//使用キャラプライマリ・キャラリスト
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    string data_string ="";
    
    Chara *used_chara = CharaList::getAtPrimary(used_primary);//スキル使用キャラ保存
    
    int used_index = used_chara->getBelongingIndex();//使用キャラ側のインデックス
    int enemy_index;//使用キャラと逆側のインデックス
    string skill_target_type=used_chara->getPassiveSkillTarget();
    
    if(used_index==0){
        enemy_index=1;
    }else{
        enemy_index=0;
    }
    data_string += to_string(used_chara->getPrimaryId());//カードのプライマリID
    
    auto removeGuardRemove = [](vector<Chara *> &targets){//キャラのHMSベクターから除去無効持ちを排除するラムダ・参照渡し
        while(true){//除去無効スキル持ちを抜かす
            bool removing = false;
            for(int i=0; i<targets.size(); i++){
                if(targets.at(i)->statusCheck(STATUS_TYPE_REMOVE_GUARD)){//除去無効持ちはターゲットしない
                    targets.erase(targets.begin() + i);
                    removing = true;
                    break;
                }
            }
            if(removing == false) break;//除去無効持ちがいないなら抜ける
        }
    };
    
    switch(used_chara->getPassiveSkillId()){
            
        case 1:{//戦闘力UP
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            break;
        }
            
        case 2:{//戦闘力DOWN
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            break;
        }
        case 3:{//毎ターン回復　武将用
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            //temp["state"] = STATUS_TYPE_ANTI_ATTACKER;//攻撃タイプのみ攻撃対象にする
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 4:{//毎ターン敵総大将にダメージ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            
            break;
        }
        case 5:{//毎ターン味方総大将にダメージ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
        }
        case 6:{//毎ターンHP回復
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
        }
        case 7:{//毎ターンHPパーセント回復
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
        }
        case 8:{//ターン開始時に増える士気をUP
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            break;
        }
        case 9:{//ターン開始時に増える士気をDOWN
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            
            if(skill_target_type == "own_all") {
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            }else{
                dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            }
            break;
        }
        case 10:{//除去無効単体
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_REMOVE_GUARD;//除去無効
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 11:{//除去無効全体
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_REMOVE_GUARD;//除去無効
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
        }
        case 12:{//カウンター　戦闘力アップ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 13:{//カウンター　戦闘力ダウン
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 14:{//カウンター　攻撃した敵武将除去
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 15:{//カウンター　敵プレイヤーダメージ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 16:{//コンボ威力UP
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN;//コンボ威力
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            break;
        }
        case 17:{//コンボ威力DOWN
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = -1 * used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN;//コンボ威力
            dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            break;
        }
        case 18:{//前方敵武将行動不能
            vector<Chara *> forward_chara = used_chara->getAroundFieldChara("forward");
            if(forward_chara.empty() == false){//前方にキャラがいる時
                map<string,int> temp;
                temp["primary"] = used_chara->getPrimaryId();
                temp["skill_id"] = used_chara->getPassiveSkillId();
                temp["value"] = used_chara->getPassiveSkillValue();
                temp["state"] = STATUS_TYPE_CANT_ACTION;//行動不能
                forward_chara.front()->chara_status_passive.push_back(temp);
            }

            break;
        }
        case 19:{//前方敵スキル封印
            
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_SKILL;//スキル封印
            used_chara->chara_status_passive.push_back(temp);
            vector<Chara *> charas = used_chara->getAroundFieldChara(skill_target_type);
            
            //どの武将に演出するか
            for(int j=0; j<charas.size(); j++){
                data_string += "," + to_string(charas.at(j)->getPrimaryId());
            }

            break;
        }
        case 20:{//自分に貫通
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_SPEAR;//貫通
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 21:{//戦闘力？？以上のキャラを行動不能
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_ABOVE;//行動不能？？以下
            //dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            //dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//tekiにパッシブ効果の追加
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){
                    if(chara->getFieldIndex() > 1 && chara->getValue() >= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }else{
                dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getOneSide(used_chara->reverseIndex(used_chara->getBelongingIndex()))){
                    if(chara->getFieldIndex() > 1 && chara->getValue() >= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }
            
            break;
        }
        case 22:{//自分の場を相手から見えなくする
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_DARKNESS;
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
        }
        case 23:{//HP100以下の時　戦闘力UP
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            
            used_chara->chara_status_passive.push_back(temp);
            break;
        }case 24:{//右、左、左右の武将の戦闘力数値UP
            
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            
            used_chara->chara_status_passive.push_back(temp);
            vector<Chara *> charas = used_chara->getAroundFieldChara(skill_target_type);
        
           //どの武将に演出するか
            for(int j=0; j<charas.size(); j++){
                data_string += "," + to_string(charas.at(j)->getPrimaryId());
            }
            
            break;
        }case 25:{//アクティブスキル封印
            
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            
            if(skill_target_type == "all_attack" || skill_target_type == "enemy_attack"){
                 temp["state"] = STATUS_TYPE_CANT_SKILL_ATTACK;//スキル封印攻撃のみ
            }else if(skill_target_type == "all_defense" || skill_target_type == "enemy_defense"){
                 temp["state"] = STATUS_TYPE_CANT_SKILL_DEFENSE;//スキル封印防御のみ
            }else if(skill_target_type == "all_support" || skill_target_type == "enemy_support"){
                temp["state"] = STATUS_TYPE_CANT_SKILL_SUPPORT;//スキル封印軍師のみ
            }else{
                 temp["state"] = STATUS_TYPE_CANT_SKILL;//スキル封印
            }
            
            if(skill_target_type == "all" || skill_target_type == "all_attack" || skill_target_type == "all_defense" || skill_target_type == "all_support"){
                for(int i=0; i<2; i++)dung->player[i]->whole_status_passive.push_back(temp);//双方にパッシブ効果の追加
            }else{
                dung->player[reverseIndex(used_index)]->whole_status_passive.push_back(temp);//敵のみ
            }
            
            break;
        }case 26:{//戦闘力UP 固定値
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
            
        }case 27:{//戦闘力DOWN 固定値
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
            
        }case 28:{//前方フィールド配置不可
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_PLACE_FORBIDDEN_FORWARD;//前方配置不可
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 29:{//前方軍師配置不可
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_PLACE_FORBIDDEN_SUPPORT;//前方軍師配置不可
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 30:{//アンチアタッカー
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_ANTI_ATTACKER;//攻撃タイプのみ攻撃対象にする
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 31:{//赤コンボアップ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN_RED;
            if(skill_target_type == "own_all") {
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }else if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);
                //両方にパッシブ効果の追加
            }
            break;
        }case 32:{//青コンボアップ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN_BLUE;
            if(skill_target_type == "own_all") {
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }else if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);
                //両方にパッシブ効果の追加
            }
            break;
        }case 33:{//緑コンボアップ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN_GREEN;
            if(skill_target_type == "own_all") {
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }else if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);
                //両方にパッシブ効果の追加
            }
            break;
        }case 34:{///敵プレイヤーに毎ターンダメージ（武将用）
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            //temp["state"] = STATUS_TYPE_ANTI_ATTACKER;//攻撃タイプのみ攻撃対象にする
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 35:{//味方プレイヤーに継続ダメージ（武将用）
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            //temp["state"] = STATUS_TYPE_ANTI_ATTACKER;//攻撃タイプのみ攻撃対象にする
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 36:{//対象キャラ戦闘力コピー
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            if(target_charas.size()>0){
                
                map<string,int> temp;
                temp["primary"] = used_chara->getPrimaryId();
                temp["skill_id"] = used_chara->getPassiveSkillId();
                temp["value"] = target_charas.at(0)->getValue();
                used_chara->chara_status_passive.push_back(temp);

            }
            break;
        }case 37:{//置いた時に自分に一ターン後死亡する状態異常
            
            //ステータスバッファに、使用キャラ、効果量、残りターンを追加
            map<string,int> temp;
            temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
            temp.insert( map<string, int>::value_type("skill_id", 41));
            temp.insert( map<string, int>::value_type("value", 100));
            temp.insert( map<string, int>::value_type("turn", 3));
            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_NEXT_TURN_DEAD));
            
            used_chara->chara_status_active.push_back(temp);
            
            break;
        }case 38:{//自分のターンがくるたびに戦闘力アップ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = 0;
            //temp["state"] = STATUS_TYPE_ANTI_ATTACKER;//攻撃タイプのみ攻撃対象にする
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 39:{//常に自分のコンボ効果アップ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 40:{//カウンター行動不能
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 41:{//カウンター誘惑
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 42:{//攻撃による総大将ダメージの減少
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = -1 * used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_PLAYER_ATTACK_UPDOWN;
            
            if(used_chara->isAttackAndDefense()){
                temp["skill_id"] = used_chara->getPassiveSkillId();
                used_chara->chara_status_passive.push_back(temp);
            }else{
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }
            
            break;
        }case 43:{//行動不能ガード
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_GUARD;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
            
        }case 44:{//コンボ無効
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN_ZERO;
            if(skill_target_type == "enemy_all") {
                dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }else if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);
                //両方にパッシブ効果の追加
            }
            break;
        }case 45:{//攻撃で死なずに行動不能状態になる
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_ESCAPE_GUARD;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
            
        }case 46:{//満タンの時　戦闘力%UP
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            
            used_chara->chara_status_passive.push_back(temp);
            break;
        }case 47:{//置いたターンのカードを置く士気をマイナス
            
            map<string,int> temp;
            temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
            temp.insert( map<string, int>::value_type("skill_id", 44));//アイコン用
            temp.insert( map<string, int>::value_type("value", used_chara->getPassiveSkillValue()));
            temp.insert( map<string, int>::value_type("turn", 1*2));
            
            if(skill_target_type == "own_all"){
                temp.insert( map<string, int>::value_type("state", STATUS_TYPE_COST_DOWN_ALL));
            }else if(skill_target_type == "own_attack"){
                temp.insert( map<string, int>::value_type("state", STATUS_TYPE_COST_DOWN_ATTACK));
            }else if(skill_target_type == "own_defense"){
                temp.insert( map<string, int>::value_type("state", STATUS_TYPE_COST_DOWN_DEFENSE));
            }else if(skill_target_type == "own_support"){
                temp.insert( map<string, int>::value_type("state", STATUS_TYPE_COST_DOWN_SUPPORT));
            }
            used_chara->chara_status_active.push_back(temp);
            break;
        }case 48:{//置いた時に対象を行動不能に
            
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            if(target_charas.size()>0){
                
                for(int i=0; i<target_charas.size(); i++){
                    map<string,int> temp;
                    temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
                    temp.insert( map<string, int>::value_type("skill_id", used_chara->getPassiveSkillId()));
                    temp.insert( map<string, int>::value_type("value", used_chara->getPassiveSkillValue()));
                    temp.insert( map<string, int>::value_type("turn", 1*2));
                    temp.insert( map<string, int>::value_type("state", STATUS_TYPE_CANT_ACTION));
                    
                    target_charas.at(i)->chara_status_active.push_back(temp);
                }
            }
            break;
        }case 49:{//ダメージを与えた時に士気を得る
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_ATTACK_MORALE_GAIN;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
            
        }case 50:{//戦闘力？？以下のキャラを行動不能
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_BELOW;//行動不能？？以下
            //dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            //dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//tekiにパッシブ効果の追加
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){
                    if(chara->getFieldIndex() > 1 && chara->getValue() <= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }else{
                dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){
                    if(chara->getFieldIndex() > 1 && chara->getValue() <= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }
            
            break;
        }case 51:{//置いた時に鉄壁を持つ
            
            map<string,int> temp;
            temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
            temp.insert( map<string, int>::value_type("skill_id", 36));//鉄壁
            temp.insert( map<string, int>::value_type("value", used_chara->getPassiveSkillValue()));
            temp.insert( map<string, int>::value_type("turn", used_chara->getPassiveSkillValue()*2));
            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_STEEL_SHIELD));
            
            used_chara->chara_status_active.push_back(temp);
            
            break;
        }case 52:{//貫通全体軍師用
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_SPEAR;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }
            break;
        }case 53:{//カウンター不可
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_ANTI_COUNTER;
        
            dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
    
            break;
        }case 54:{//置いた時に対象と同じカードを自分のデッキの一番上に生成する
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            if(target_charas.size()>0){
                Chara *chara = target_charas[0]->copy();//コピーの作成
                CharaList::chara_list.insert(chara);
                chara->setBelongingIndex(used_index);
                chara->initialize();
                dung->deck[used_index].insert( dung->deck[used_index].begin() ,chara);
                
                data_string += "," + to_string(target_charas[0]->getPrimaryId());
                data_string += "," + to_string(chara->getPrimaryId());
                
            }
            break;
        }case 55:{//置いた時に対象と同じカードにコピーされる
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            if(target_charas.size()>0){
                
                data_string += "," + to_string(target_charas[0]->getPrimaryId());
            }
            break;
        }case 56:{//カウンター士気回復
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 57:{//攻撃した防御武将の戦闘力低下
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_DEFENSE_BREAK;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 58:{//置いた時に前方の配置コストX以下の武将を自軍の武将にする
            if(used_chara->getNearBlankFieldIndex()>0){
                vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), "forward");
                
                if(target_charas.size()>0 && target_charas[0]->place_cost <= used_chara->getPassiveSkillValue()){
                    
                    data_string += "," + to_string(target_charas[0]->getPrimaryId());
                }
            }
            
            break;
        }case 59:{//ターン経過するほど戦闘力アップ
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            //temp["state"] = STATUS_TYPE_DEFENSE_BREAK;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 60:{//2回攻撃
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_TWICE_ATTACK;//2回攻撃
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 61:{//コンボカラー左右違う時コンボ倍率UP
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COMBO_UPDOWN_DIFFERENT;
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            
            break;
        }case 62:{//置いた時にこのカードと同じコンボカラーの武将がデッキにいたら、それをフレンドカードにする
            Chara *target_chara = nullptr;
            for(Chara *chara : dung->deck[used_index]){
                if(chara->getCombinationL()==used_chara->getCombinationL() && chara->getCombinationR()==used_chara->getCombinationR()){
                    target_chara = chara;
                    break;
                }
            }
            
            if(target_chara != nullptr){//対象になるキャラがいた時
                data_string += "," + to_string(target_chara->getPrimaryId());
            }
            break;
        }
        case 63:{//カウンター　味方総大将回復
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 64:{//HP100以下の時　士気UP
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);
            //used_chara->chara_status_passive.push_back(temp);
            break;
        }
        case 65:{//根性
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_ENDURE;//根性
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);
            
            break;
        }
        case 66:{//迎撃態勢・貫通
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = 30;
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_ANTI_ATTACKER;//迎撃
            used_chara->chara_status_passive.push_back(temp);
            
            map<string,int> temp2;
            temp2["primary"] = used_chara->getPrimaryId();
            temp2["skill_id"] = 20;
            temp2["value"] = used_chara->getPassiveSkillValue();
            temp2["state"] = STATUS_TYPE_SPEAR;//貫通
            used_chara->chara_status_passive.push_back(temp2);
            break;
        }
        case 67:{//攻撃カードを置いた時にそのカードは行動不能になる
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_STOPPING_ATTACKER;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            }
            break;
        }
        case 68:{//置いた時このカードと同じカードを召喚する
            if(used_chara->getNearBlankFieldIndex()>0){//置く場所がある時
                data_string += "," + to_string(used_chara->getPrimaryId());
            }
            
            break;
        }
        case 69:{//カウンター　最も戦闘力の低い敵武将除去
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 70:{//コンボが連鎖しているカードの数だけ戦闘力＋
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            //temp["state"] = STATUS_TYPE_COMBO_CHAIN;//コンボチェーン
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }case 71:{//相手はXコストのカードを出すことができない
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_SUMMONS;//Xコスト召喚不可
            dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);
            break;
        }
        
        case 72:{//対象のパッシブスキル破壊
            vector<Chara *> target_charas_before = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            vector<Chara *> target_charas;
            for(Chara *chara : target_charas_before){
                if(chara->getPassiveSkillId()>0) target_charas.push_back(chara);
            }
            
            for(Chara *chara : target_charas){
                data_string += "," + to_string(chara->getPrimaryId());
                
                chara->setPassiveSkillId(0);
                chara->setPassiveSkillName("");
                chara->setPassiveSkillTarget("");
                chara->setPassiveSkillDetail("");
                chara->setPassiveSkillValue(0);
                
                while(true){//キャラに付いてるパッシブの削除
                    bool delete_flg = false;
                    for(int i=0; i<chara->chara_status_passive.size(); i++){
                        if(chara->chara_status_passive.at(i)["primary"]==chara->getPrimaryId()){
                            chara->chara_status_passive.remove(i);
                            delete_flg = true;
                        }
                    }
                    if(delete_flg==false) break;
                }
                
                for(int ii=0; ii<2; ii++){
                    for(int i=0; i<dung->player[ii]->whole_status_passive.size(); i++){
                        if(dung->player[ii]->whole_status_passive.at(i)["primary"] == chara->getPrimaryId()){
                            dung->player[ii]->whole_status_passive.erase(dung->player[ii]->whole_status_passive.begin()+i);//全体パッシブの削除
                        }
                    }
                }
                dung->uiLayer->removePassiveIcon(chara->getPrimaryId());//全体パッシブアイコンの削除
            }
            
            break;
        }
            
        case 73:{//対象の中のコストX以下の対象を破壊
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            removeGuardRemove(target_charas);//除去無効
            
            for(Chara *chara : target_charas){
                if(chara->place_cost <= used_chara->getPassiveSkillValue()){
                    chara->dead_flg = true;
                    data_string += "," + to_string(chara->getPrimaryId());
                }
            }
            break;
        }
            
            
        case 74:{//攻撃できない
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_WAITING_FORCE;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            }
            break;
        }
        
        case 75:{//基本戦闘力X上昇
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            for(Chara *chara : target_charas){
                chara->setBaseValue(chara->getBaseValue() + used_chara->getPassiveSkillValue());
                data_string += "," + to_string(chara->getPrimaryId());
            }
            break;
        }
            
        case 76:{//基本戦闘力X上昇
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            for(Chara *chara : target_charas){
                chara->setBaseValue(chara->getBaseValue() + used_chara->getPassiveSkillValue());
                data_string += "," + to_string(chara->getPrimaryId());
            }
            break;
        }

        case 77:{//対象を破壊
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            removeGuardRemove(target_charas);//除去無効
            for(Chara *chara : target_charas){
                chara->dead_flg = true;
                data_string += "," + to_string(chara->getPrimaryId());
            }
            break;
        }
           
        case 78:{//コンボカラーXが含まれていない
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_COMBO;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){//エフェクト付ける用
                    if(chara->getFieldIndex() > 1
                       && !(chara->getCombinationL() == used_chara->getPassiveSkillValue() || chara->getCombinationR() == used_chara->getPassiveSkillValue())){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }else{
                dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getOneSide(used_chara->reverseIndex(used_chara->getBelongingIndex()))){//エフェクト付ける用
                    if(chara->getFieldIndex() > 1
                       && (chara->getCombinationL() == used_chara->getPassiveSkillValue() || chara->getCombinationR() == used_chara->getPassiveSkillValue())){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }
            
            
            break;
        }
            
        case 79:{//コストX以下行動不能
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_COST_BELOW;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){
                    if(chara->getFieldIndex() > 1 && chara->getPlaceCost() <= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }else{
                dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getOneSide(used_chara->reverseIndex(used_chara->getBelongingIndex()))){
                    if(chara->getFieldIndex() > 1 && chara->getPlaceCost() <= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }
            
            break;
        }
            
        case 80:{//コストX以上行動不能
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_COST_MORE;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){
                    if(chara->getFieldIndex() > 1 && chara->getPlaceCost() >= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }else{
                dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getOneSide(used_chara->reverseIndex(used_chara->getBelongingIndex()))){
                    if(chara->getFieldIndex() > 1 && chara->getPlaceCost() >= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }
            
            break;
        }
            
        case 81:{//レア度X以下行動不能
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_RARE_BELOW;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){
                    if(chara->getFieldIndex() > 1 && chara->getRare() <= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }else{
                dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getOneSide(used_chara->reverseIndex(used_chara->getBelongingIndex()))){
                    if(chara->getFieldIndex() > 1 && chara->getRare() <= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }
        
            break;
        }
            
        case 82:{//レア度X以上行動不能
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_ACTION_RARE_MORE;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getAll()){
                    if(chara->getFieldIndex() > 1 && chara->getRare() >= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }else{
                dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
                
                for(Chara *chara : CharaList::getOneSide(used_chara->reverseIndex(used_chara->getBelongingIndex()))){
                    if(chara->getFieldIndex() > 1 && chara->getRare() >= used_chara->getPassiveSkillValue()){
                        data_string += "," + to_string(chara->getPrimaryId());
                    }
                }
            }
            break;
        }
            
        case 83:{//戦闘力低下無効 軍師用
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_INVALID_DECREASE;
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }
            break;
        }
            
        case 84:{//戦闘力低下無効 武将用
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_INVALID_DECREASE;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
        case 85:{//ターン開始時に増える士気をUP ターンがたつごとに+1
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            break;
        }
        case 86:{//敵の回復妨害　回復量Xパーセント減少
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_HEAL_JAMMING;
            
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            }
            break;
        }
        case 87:{//スキルダメージX％減少
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_SKILL_DAMAGE_CUT;
            
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }
            break;
        }
        case 88:{//対象のアクティブ解除
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            for(Chara *chara : target_charas){
                if(chara->chara_status_active.size()>0){
                    data_string += "," + to_string(chara->getPrimaryId());
                }
            }
            break;
        }
        case 89:{//カウンター特殊召喚
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
            
        case 90:{//士気がX以上の時、それ以上増加しなくなる
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_LIMITED_MAX_COST;//
            
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            }
            break;
        }
            
        case 91:{//ドロー数制限X枚
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_DRAW_CONFINE;
            
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.push_back(temp);//敵にパッシブ効果の追加
            }
            break;
        }
        
        case 92:{//カウンター氷漬け　クリスマス用
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_COUNTER;//カウンター
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
            
        case 93:{//対象の戦闘力を１ターン下げる
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            for(Chara *chara : target_charas){
                data_string += "," + to_string(chara->getPrimaryId());
            }
            break;
        }
            
        case 94:{//対象の戦闘力を１ターン上げる
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            for(Chara *chara : target_charas){
                data_string += "," + to_string(chara->getPrimaryId());
            }
            break;
        }
            
        case 95:{//スキルダメージX％増加
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_SKILL_DAMAGE_UP;
            
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }
            break;
        }
        
        case 96:{//置いた時「対象を誘惑し、戦闘力を１ターンの間X増加させる」
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            
            for(Chara *chara : target_charas){
                data_string += "," + to_string(chara->getPrimaryId());
            }
            break;
        }
        
        case 98:{//カードが攻撃で破壊される時　確率で無効にする
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_RANDOM_GUARD;
            used_chara->chara_status_passive.push_back(temp);
            
            break;
        }
            
        case 101:{//スキル封印耐性
            map<string,int> temp;
            temp["primary"] = used_chara->getPrimaryId();
            temp["skill_id"] = used_chara->getPassiveSkillId();
            temp["value"] = used_chara->getPassiveSkillValue();
            temp["state"] = STATUS_TYPE_CANT_SKILL_GUARD;
            
            if(skill_target_type == "all") {
                for(int ii=0; ii<2; ii++) dung->player[ii]->whole_status_passive.push_back(temp);//両方にパッシブ効果の追加
            }else{
                dung->player[used_chara->getBelongingIndex()]->whole_status_passive.push_back(temp);//味方にパッシブ効果の追加
            }
            break;
        }
            
    }
    return data_string;
}

Sprite* PassiveSkill::getIcon(int skill_id){//バッファのアイコン
    
    switch(skill_id){
            
        case 1://戦闘力UP
        case 26://固定値UP
            return Sprite::create("download/Chara/icon/icon01.png");
            
        case 2://戦闘力DOWN
        case 27://固定値DOWN
            return Sprite::create("download/Chara/icon/icon02.png");
            
        case 4://毎ターン敵にダメージ
        case 34:
            return Sprite::create("download/Chara/icon/icon03.png");
        //case 5://プレイヤーダメージ上昇・軽減
        //    return Sprite::create("download/Chara/icon/icon07.png");
        case 3://毎ターン回復
        case 6://毎ターン回復
        case 7:
            return Sprite::create("download/Chara/icon/icon04.png");
        case 8://上昇士気UP
            return Sprite::create("download/Chara/icon/icon09.png");
        case 9://上昇士気DOWN
            return Sprite::create("download/Chara/icon/icon10.png");
        case 10://除去無効
        case 11:
            return Sprite::create("download/Chara/icon/icon11.png");
        case 14://カウンター類
        case 69:
            return Sprite::create("download/Chara/icon/icon06.png");
            
        case 16://コンボ威力UP
        case 39:
            return Sprite::create("download/Chara/icon/icon12.png");
        case 17://コンボ威力DOWN
            return Sprite::create("download/Chara/icon/icon13.png");
        case 18://行動不能
            return Sprite::create("download/Chara/icon/icon15.png");
        case 19://スキル封印
            return Sprite::create("download/Chara/icon/icon14.png");
        case 20://貫通
        case 52:
            return Sprite::create("download/Chara/icon/icon05.png");
        case 21://？？以下行動不能
        case 50://？？以上行動不能
        case 78://コンボXがないカード行動不能
        case 79://コストX以下行動不能
        case 80://コストX以上行動不能
        case 81://レア度X以下行動不能
        case 82://レア度X以上行動不能
            return Sprite::create("download/Chara/icon/icon19.png");
        case 22://自分の場を相手から見えなくする
            return Sprite::create("download/Chara/icon/icon20.png");
        case 23://HP100以下の時戦闘力%UP
        case 64://HP100以下の時士気UP
            return Sprite::create("download/Chara/icon/icon22.png");
        case 24://右、左、左右の武将の戦闘力数値UP
            return Sprite::create("download/Chara/icon/icon23.png");
        case 25://アクティブスキル封印
            return Sprite::create("download/Chara/icon/icon24.png");
        case 28://前方配置不可
            return nullptr;
        case 29://前方軍師配置不可
            return nullptr;
        case 30://アンチアタッカー
            return Sprite::create("download/Chara/icon/icon25.png");
        case 31://赤コンボアップ
            return Sprite::create("download/Chara/icon/icon26.png");
        case 32://青コンボアップ
            return Sprite::create("download/Chara/icon/icon27.png");
        case 33://緑コンボアップ
            return Sprite::create("download/Chara/icon/icon28.png");
        case 35://味方総大将にダメージ
        case 5:
            return Sprite::create("download/Chara/icon/icon31.png");
        case 36://戦闘力コピー
            return Sprite::create("download/Chara/icon/icon32.png");
        case 37://置いた時に自分に一ターン後死亡する状態異常
            return Sprite::create("download/Chara/icon/icon33.png");
        case 38://自分のターンがくるたびに戦闘力アップ
        case 59:
            return Sprite::create("download/Chara/icon/icon34.png");
        case 42://総大将へのダメージ減少
            return Sprite::create("download/Chara/icon/icon35.png");
        case 43://行動不能無効
            return Sprite::create("download/Chara/icon/icon36.png");
        case 44://コンボ無効
            return Sprite::create("download/Chara/icon/icon37.png");
        case 45://攻撃で死なずに行動不能状態になる
            return Sprite::create("download/Chara/icon/icon38.png");
        case 46://HP満タンの時　戦闘力%UP
            return Sprite::create("download/Chara/icon/icon39.png");
        case 49://ダメージを与えた時に士気を得る
            return Sprite::create("download/Chara/icon/icon41.png");
        case 53://カウンター封じ
            return Sprite::create("download/Chara/icon/icon43.png");
        case 57://防御戦闘力ダウン攻撃
            return Sprite::create("download/Chara/icon/icon44.png");
        case 60://２回攻撃
            return Sprite::create("download/Chara/icon/icon45.png");
        case 61://コンボカラー左右違う時コンボ倍率UP
            return Sprite::create("download/Chara/icon/icon46.png");
        case 65://HP1で耐える
            return Sprite::create("download/Chara/icon/icon47.png");
        case 67://攻撃カード置いたら行動不能
            return Sprite::create("download/Chara/icon/icon48.png");
        case 70://コンボチェーン
            return Sprite::create("download/Chara/icon/icon51.png");
        case 12://カウンター　戦闘力アップ
            return Sprite::create("download/Chara/icon/icon52.png");
        case 13://カウンター　戦闘力ダウン
            return Sprite::create("download/Chara/icon/icon53.png");
        case 15://カウンター　プレイヤーダメージ
            return Sprite::create("download/Chara/icon/icon54.png");
        case 40://カウンター　行動不能
            return Sprite::create("download/Chara/icon/icon55.png");
        case 41://カウンター　誘惑
            return Sprite::create("download/Chara/icon/icon56.png");
        case 56://カウンター　士気回復
            return Sprite::create("download/Chara/icon/icon57.png");
        case 63://カウンター　HP回復
            return Sprite::create("download/Chara/icon/icon58.png");
        case 89://カウンター　特殊召喚
            return Sprite::create("download/Chara/icon/icon66.png");
        case 71://相手はXコストのカードを出すことができない
            return Sprite::create("download/Chara/icon/icon59.png");
        case 74://攻撃不可
            return Sprite::create("download/Chara/icon/icon60.png");
        case 83://戦闘力低下無効
        case 84:
            return Sprite::create("download/Chara/icon/icon61.png");
        case 85://上昇士気UP+
            return Sprite::create("download/Chara/icon/icon62.png");
        case 86://回復妨害
            return Sprite::create("download/Chara/icon/icon63.png");
        case 87://スキルダメージX％減少
            return Sprite::create("download/Chara/icon/icon64.png");
        case 90://士気がX以上の時、それ以上増加しなくなる
            return Sprite::create("download/Chara/icon/icon65.png");
        case 91://手札引く数制限
            return Sprite::create("download/Chara/icon/icon50.png");
        case 92://カウンター　氷漬け
            return Sprite::create("download/Chara/icon/icon67.png");
        case 95://スキルダメージX％増加
            return Sprite::create("download/Chara/icon/icon68.png");
        case 98://ランダムガード
            return Sprite::create("download/Chara/icon/icon70.png");
        case 101://スキル封印耐性
            return Sprite::create("download/Chara/icon/icon71.png");
            
        default:
            //CCLOG("アイコン無し？");
            return  nullptr;
    }
    
    
    HMSLOG("error : パッシブキャラバッファアイコンが見つかりません バッファid:%d",skill_id);
    return Sprite::create("download/Chara/icon/status_paralysis.png");//エラー時用アイコン;
}
int PassiveSkill::reverseIndex(int index){
    if(index==0){
        return 1;
    }else{
        return 0;
    }
}
