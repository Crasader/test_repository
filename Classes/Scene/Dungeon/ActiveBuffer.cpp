/*アクティブバッファ（アクティブスキルによって付いたターン経過で消えるバッファ）の効果計算*/

#include "ActiveBuffer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "CharaList.h"
#include "TurnManage.h"

void ActiveBuffer::bufferLogic(Chara* buff_chara, map<string, int> buffer){//全体buffの時はかけたキャラ、単体buffの時はかけられたキャラ
	string data_string = "";
	int after_value = 0;
	int usedIndex = buff_chara->getBelongingIndex();
	int enemyIndex = 1;
	if(usedIndex>0) enemyIndex = 0;
    //HMSLOG("buff_id : %d", buff_id);
	
    switch(buffer["skill_id"]){
		case 1:
		case 2:
		case 3:{//戦闘力UP
            buff_chara->valueUpSkill(buffer["value"], "percent");
            break;
		}
		case 4:
		case 5:
		case 6:{//戦闘力DOWN
            buff_chara->valueUpSkill(-1 * buffer["value"], "percent");
			break;
		}
		case 22:{//大撹乱:戦闘力の1桁目と2桁目を交換する(例:120→102=102)
			int base_value = buff_chara->getValue();//元の戦闘力
			HMSLOG("before_value : %d", base_value);
			int three_value = base_value / 100;//3桁目
			int two_value = (base_value % 100) / 10;//2桁目
			int one_value = (base_value % 100) % 10;//1桁目
			string value_str = "";
			
			if(three_value>0) value_str += to_string(three_value);
			value_str += to_string(one_value);
			value_str += to_string(two_value);

			after_value = stoi(value_str);//文字列を数値に変換
			HMSLOG("after_value : %d", after_value);
			buff_chara->setValue(after_value);
			break;
		}
		case 26:{//他者犠牲:味方武将を犠牲にし、戦闘力を吸収する
			after_value = buff_chara->getValue() + buffer["value"];
			buff_chara->setValue(after_value);
			break;
		}
        case 33:
        case 61:
        case 1005:{//固定値UP
            buff_chara->valueUpSkill(buffer["value"], "integer");
            break;
        }
        case 34:
        case 62:
        case 1006:{//固定値DOWN
            buff_chara->valueUpSkill(-1 * buffer["value"], "integer");
            break;
        }
        case 35:{//戦闘力指定
            after_value = buffer["value"];
            buff_chara->setValue(after_value);
            break;
        }
        /*case 86:{//ためる
            CCLOG("ためる1：%d",buffer["turn"]);
            if(buffer["turn"]<=1){
                CCLOG("ためる２：%d",buffer["value"]);
                buff_chara->valueUpSkill(buffer["value"], "integer");
            }
            break;
        }*/
            
            
		default:{
			//HMSLOG("active buffer logic error : There is no buff_id of the target");
			break;
		}
    }
}

//毎ターン効果バッファ
string ActiveBuffer::turnBufferLogic(Chara *target_chara, map<string, int> &buffer){
    //DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    string effectString ="";
    
    Chara *use_chara = CharaList::getAtPrimary(buffer["primary"]);//スキル使用キャラ保存
    
    effectString += to_string(use_chara->getPrimaryId());//カードのプライマリID
    
    switch(buffer["skill_id"]){
            
        case 86:
        {//ためる
            if(buffer["state"] == STATUS_TYPE_ACCUMULATION && buffer["turn"] <= 2*2){//ため状態を戦闘力アップに変更
                buffer["skill_id"] = 33;
                buffer["state"] = 0;
                effectString += ","+to_string(target_chara->getPrimaryId());//カードのプライマリID
            }else{
                effectString ="";
            }
            
            break;
        }
            
        default:{
            
            effectString ="";//ターンチェンジ時発動スキル以外はナシ
            
        }
    }
    
    HMSLOG("毎ターンバッファストリング%s",effectString.c_str());
    return effectString;
}




