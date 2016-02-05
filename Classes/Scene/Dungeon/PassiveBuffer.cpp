

#include "PassiveBuffer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "Chara.h"
#include "TurnManage.h"
#include "CharaList.h"

void PassiveBuffer::bufferLogic(int used_primary,int buff_id,int variationValue,int index){//全体buffの時はかけたキャラ、単体buffの時はかけられたキャラ
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    Chara *used_chara;//スキル使用キャラ保存
    for(Chara *chara : CharaList::getAll()){
        if(chara->getPrimaryId()==used_primary){
            used_chara = chara;
            break;
        }
    }
   
    string skill_target_type = used_chara->getPassiveSkillTarget();//ターゲットタイプ保存
    int passive_skill_value = used_chara->getPassiveSkillValue();//スキルvalue保存
    HMSLOG("インデックス%dにスキル:%d発動中ターゲットタイプ:%s",index,buff_id,skill_target_type.c_str());
    switch(buff_id){
            
        case 1:{//戦闘力UP
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            for(int i=0; i<target_charas.size(); i++){
                target_charas.at(i)->valueUpSkill(passive_skill_value, "percent");
                //target_charas.at(i)->setValue(target_charas.at(i)->getValue() + target_charas.at(i)->combo_value * passive_skill_value/100.0f);//戦闘力UP
            }
            break;
        }
        case 2:{//戦闘力DOWN
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            for(int i=0; i<target_charas.size(); i++){
                target_charas.at(i)->valueUpSkill(-1 * passive_skill_value, "percent");
                //target_charas.at(i)->setValue(target_charas.at(i)->getValue() - target_charas.at(i)->combo_value * passive_skill_value/100.0f);//戦闘力DOWN
            }
            break;
        }
        case 3:{//戦闘力UP(毎ターン数値UP)
            /*for(Chara *chara : CharaList::getOneSide(index)){
                if(chara->getFieldIndex()>=2){
                    chara->setValue(chara->getValue() + chara->combo_value * variationValue/100.0f);//戦闘力UP
                }
            }
            break;*/
        }
        case 4:{//毎ターンプレイヤーダメージ
            //なし
            
            break;
        }
        case 5:{//毎ターンプレイヤーダメージ味方
            //なし
        
            break;
        }
        case 8:{//ターン開始時に増える士気をUP
            dung->player[index]->setPower(dung->player[index]->getPower() + passive_skill_value);
            
            break;
        }
        case 9:{//敵のターン開始時に増える士気をDOWN
            dung->player[index]->setPower(dung->player[index]->getPower() - passive_skill_value);
            
            break;
        }
        case 10:{//除去無効単体
            //used_chara->remove_guard_flg = true;
            
            break;
        }
        case 11:{//除去無効全体
            
            
            break;
        }
        case 12:{//敵プレイヤーの消費士気を上げる
            
            
            break;
        }
        case 13:{//相手の手札を見る
            
            
            break;
        }
        case 14:{//単体除去スキル無効
            
            
            break;
        }
        case 15:{//全体除去スキル無効
            
            
            break;
        }
        case 16:{//コンボ威力上昇
            
            
            break;
        }
        case 17:{//コンボ威力下降
            
            
            break;
        }
        case 18:{//前方敵武将行動不能
            
            
            break;
        }
        case 19:{//前方敵スキル封印
            
            
            break;
        }
        case 20:{//自分に貫通
            
            
            break;
        }
        case 21:{//戦闘力？？以上のキャラを行動不能
            
            
            break;
        }
        case 22:{//自分の場を相手から見えなくする
            
            
            break;
        }case 23:{//HP100以下の時　戦闘力%UP
            if(dung->player[used_chara->getBelongingIndex()]->getHp() <= 100){
                used_chara->valueUpSkill(passive_skill_value, "percent");
                //used_chara->setValue(used_chara->getValue() + used_chara->combo_value * passive_skill_value/100.0f);//戦闘力UP
            }
            break;
        }case 24:{//右、左、左右の武将の戦闘力数値UP
            vector<Chara *> charas = used_chara->getAroundFieldChara(skill_target_type);
            
            for(Chara *chara : charas){//ここの確認から
                chara->valueUpSkill(passive_skill_value, "integer");
                //charas.at(j)->setValue(charas.at(j)->getValue() + passive_skill_value);//戦闘力UP
            }
            break;
        }case 25:{//アクティブスキル封印
            
            
            break;
        }case 26:{//戦闘力UP 固定値
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            for(Chara *chara : target_charas){
                chara->valueUpSkill(passive_skill_value, "integer");
                //target_charas.at(i)->setValue(target_charas.at(i)->getValue() + passive_skill_value);//戦闘力UP
            }
            
            break;
        }case 27:{//戦闘力DOWN 固定値
            vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), skill_target_type);
            for(Chara *chara : target_charas){
                chara->valueUpSkill(-1 * passive_skill_value, "integer");
                //target_charas.at(i)->setValue(target_charas.at(i)->getValue() - passive_skill_value);//戦闘力UP
            }
            
            break;
        }case 28:{//前方フィールド配置不可
            
            
            break;
        }case 29:{//前方軍師配置不可
            
            
            break;
        }case 30:{//アンチアタッカー
            
            
            break;
        }case 31:{//赤コンボアップ
           
            break;
        }case 32:{//青コンボアップ
            
            
            break;
        }case 33:{//緑コンボアップ
            
            
            break;
        }case 34:{//敵総大将に毎ターンダメージ
            
            
            break;
        }case 35:{//味方総大将に毎ターンダメージ
            
            
            break;
        }case 36:{//対象の戦闘力をコピー
            used_chara->setValue(variationValue);
            
            break;
        }case 37:{//
            
            
            break;
        }case 38:{//戦闘力毎ターンアップ
            for(int i=0; i<used_chara->chara_status_passive.size(); i++){
                if(used_chara->chara_status_passive.at(i)["primary"] == used_chara->getPrimaryId()){//自分のそのスキルに対して
                    used_chara->valueUpSkill(used_chara->chara_status_passive.at(i)["value"], "integer");
                    //used_chara->setValue(used_chara->getValue() + used_chara->chara_status_passive.at(i)["value"]);//加算
                    //HMSLOG("戦闘力アップ%d",used_chara->chara_status_passive.at(i)["value"]);
                }
            }
            break;
        }case 46:{//HP満タンの時　戦闘力%UP
            if(dung->player[used_chara->getBelongingIndex()]->getHp() == dung->player[used_chara->getBelongingIndex()]->getMaxHp()){
                used_chara->valueUpSkill(passive_skill_value, "percent");
                //used_chara->setValue(used_chara->getValue() + used_chara->combo_value * passive_skill_value/100.0f);//戦闘力UP
            }
            break;
        }case 59:{//ターン数に応じて戦闘力アップ
            for(int i=0; i<used_chara->chara_status_passive.size(); i++){
                if(used_chara->chara_status_passive.at(i)["primary"] == used_chara->getPrimaryId()){//自分のそのスキルに対して
                    used_chara->valueUpSkill(used_chara->chara_status_passive.at(i)["value"] * TurnManage::getNowTurn(), "integer");
                    //used_chara->setValue(used_chara->getValue() + used_chara->chara_status_passive.at(i)["value"] * TurnManage::getNowTurn());//加算
                    //HMSLOG("戦闘力アップ%d",used_chara->chara_status_passive.at(i)["value"]);
                }
            }
            break;
        }case 64:{//HP100以下の時　士気UP
            if(dung->player[used_chara->getBelongingIndex()]->getHp() <= 100){
                dung->player[index]->setPower(dung->player[index]->getPower() + passive_skill_value);
            }
            break;
        }case 70:{//コンボが連鎖しているカードの数だけ戦闘力＋
            int FI = used_chara->getFieldIndex();
            int chain_count = 0;
            for(int i=FI; i>=3; i--){
                Chara *chara_temp = CharaList::getAtField(used_chara->getBelongingIndex(), i);
                if(chara_temp != nullptr && chara_temp->combination_count/10.0f >= 1.0f){//その場所にキャラが居て、左側がコンボで繋がっている時
                    chain_count++;
                }else{
                    break;
                }
            }
            for(int i=FI; i<=6; i++){
                Chara *chara_temp = CharaList::getAtField(used_chara->getBelongingIndex(), i);
                if(chara_temp != nullptr && chara_temp->combination_count%10 >= 1.0f){//その場所にキャラが居て、右側がコンボで繋がっている時
                    chain_count++;
                }else{
                    break;
                }
            }
            CCLOG("%d連鎖中！！戦闘力%dアップ",chain_count,passive_skill_value * chain_count);
            used_chara->valueUpSkill(passive_skill_value * chain_count, "integer");
            //used_chara->setValue(used_chara->getValue() + passive_skill_value * chain_count);//加算
            
            break;
        }
        case 85:{//ターン開始時に増える士気をUP
            dung->player[index]->setPower(dung->player[index]->getPower() + variationValue);
            CCLOG("士気上昇値１：%d",variationValue);
            break;
        }
    }
}


//毎ターン効果バッファ
string PassiveBuffer::turnBufferLogic(map<string, int> &buffer){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    Chara *use_chara = CharaList::getAtPrimary(buffer["primary"]);//スキル使用キャラ保存
    string effectString = to_string(use_chara->getPrimaryId());//カードのプライマリID
    
    switch(use_chara->getPassiveSkillId()){
            
        case 4:
        case 35:
        {//毎ターンプレイヤーダメージ
            float plus_per = 0.0f;
            float minus_per = 0.0f;
            for(int i=0; i< dung->player[use_chara->getBelongingIndex()]->whole_status_passive.size(); i++){
                if(dung->player[use_chara->getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    plus_per += dung->player[use_chara->getBelongingIndex()]->whole_status_passive.at(i)["value"];
                }
            }
            for(int i=0; i< dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.size(); i++){
                if(dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    minus_per += dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.at(i)["value"];
                }
            }
            if(minus_per > 100.0f) minus_per = 100.0f;
            
            int after_value = use_chara->getPassiveSkillValue() * (100.0f + plus_per)/100.0f * (100.0f - minus_per)/100.0f;
            
            int damage = dung->player[TurnManage::getTurnIndex()]->directDamage(after_value);
            effectString += ",";
            effectString += to_string(damage);
            effectString += ",";
            effectString += to_string(dung->player[TurnManage::getTurnIndex()]->getHp());

            break;
        }
        case 5:
        {//毎ターン味方プレイヤーダメージ
            float plus_per = 0.0f;
            float minus_per = 0.0f;
            for(int i=0; i< dung->player[use_chara->getBelongingIndex()]->whole_status_passive.size(); i++){
                if(dung->player[use_chara->getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    plus_per += dung->player[use_chara->getBelongingIndex()]->whole_status_passive.at(i)["value"];
                }
            }
            for(int i=0; i< dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.size(); i++){
                if(dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    minus_per += dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.at(i)["value"];
                }
            }
            if(minus_per > 100.0f) minus_per = 100.0f;
            
            int after_value = use_chara->getPassiveSkillValue() * (100.0f + plus_per)/100.0f * (100.0f - minus_per)/100.0f;
            
            int damage = dung->player[TurnManage::getTurnIndex()]->directDamage(after_value);
            effectString += ",";
            effectString += to_string(damage);
            effectString += ",";
            effectString += to_string(dung->player[TurnManage::getTurnIndex()]->getHp());
            
            break;
        }
            
            
        case 3:
        case 6:{//毎ターンHP回復
            int heal_value = dung->player[TurnManage::getTurnIndex()]->heal(use_chara->getPassiveSkillValue());
           
            effectString += ",";
            effectString += to_string(heal_value);
            effectString += ",";
            effectString += to_string(dung->player[TurnManage::getTurnIndex()]->getHp());
        
            break;
        }
        case 7:{//毎ターンHPパーセント回復
            int heal_value = dung->player[TurnManage::getTurnIndex()]->heal(dung->player[TurnManage::getTurnIndex()]->getHp() * use_chara->getPassiveSkillValue() / 100);
            
            effectString += ",";
            effectString += to_string(heal_value);
            effectString += ",";
            effectString += to_string(dung->player[TurnManage::getTurnIndex()]->getHp());
            break;
        }
        
        
        case 34:{//敵プレイヤーに毎ターンダメージ（武将用）
            float plus_per = 0.0f;
            float minus_per = 0.0f;
            for(int i=0; i< dung->player[use_chara->getBelongingIndex()]->whole_status_passive.size(); i++){
                if(dung->player[use_chara->getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    plus_per += dung->player[use_chara->getBelongingIndex()]->whole_status_passive.at(i)["value"];
                }
            }
            for(int i=0; i< dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.size(); i++){
                if(dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    minus_per += dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_passive.at(i)["value"];
                }
            }
            if(minus_per > 100.0f) minus_per = 100.0f;
            
            int after_value = use_chara->getPassiveSkillValue() * (100.0f + plus_per)/100.0f * (100.0f - minus_per)/100.0f;
            
            int damage = dung->player[use_chara->reverseIndex(TurnManage::getTurnIndex())]->directDamage(after_value);
            effectString += ",";
            effectString += to_string(damage);
            effectString += ",";
            effectString += to_string(dung->player[use_chara->reverseIndex(TurnManage::getTurnIndex())]->getHp());
            
            break;
        }
        
        case 38:{//毎ターン戦闘力アップ
            HMSLOG("毎ターン%d",use_chara->getPassiveSkillValue());
            for(int i=0; i<use_chara->chara_status_passive.size(); i++){
                if(use_chara->chara_status_passive.at(i)["primary"] == use_chara->getPrimaryId()){//自分のそのスキルに対して
                    
                    map<string,int> temp;
                    temp["primary"] = use_chara->chara_status_passive.at(i)["primary"];
                    temp["skill_id"] = use_chara->chara_status_passive.at(i)["skill_id"];
                    temp["value"] = use_chara->chara_status_passive.at(i)["value"] + use_chara->getPassiveSkillValue();//攻撃力を足して再追加
                    
                    use_chara->chara_status_passive.erase(use_chara->chara_status_passive.begin() + i);//一旦削除
                    use_chara->chara_status_passive.push_back(temp);
  
                    HMSLOG("加算%d",use_chara->chara_status_passive.at(i)["value"]);
                    break;
                }
            }
            
            break;
        }
        case 85:{//毎ターン士気上昇値アップ
            buffer["value"] = buffer["value"] + 1;
            CCLOG("士気上昇値2：%d",buffer["value"]);
            
            break;
        }
            
        default:{
            
            effectString ="";//ターンチェンジ時発動スキル以外はナシ
            
        }
    }
    
    HMSLOG("毎ターンバッファストリング%s",effectString.c_str());
    return effectString;
}

string PassiveBuffer::turnHandBufferLogic(Chara *used_chara){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    
    string effectString = to_string(used_chara->getPrimaryId());//最初に使用者のプライマリを保存
    
    if(used_chara->getPassiveSkillId() == 97){//手札にある時、対象の武将の基本戦闘力アップ
        vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), used_chara->getPassiveSkillTarget());
        for(auto target : target_charas){
            target->setBaseValue(target->getBaseValue() + used_chara->getPassiveSkillValue());
            effectString += "," + to_string(target->getPrimaryId());
        }
        
    }else if(used_chara->getPassiveSkillId() == 100){//手札にある時、対象の武将の基本戦闘力ダウン
        vector<Chara *> target_charas = dung->activeSkill->getTargetCharas(used_chara, CharaList::getAll().getVector(), used_chara->getPassiveSkillTarget());
        for(auto target : target_charas){
            target->setBaseValue(target->getBaseValue() - used_chara->getPassiveSkillValue());
            effectString += "," + to_string(target->getPrimaryId());
        }
        
    }else{//手札発動キャラではない
        return "";
    }
    
    return effectString;//演出ストリングの追加
}

