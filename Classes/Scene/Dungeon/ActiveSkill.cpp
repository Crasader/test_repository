/*アクティブスキルのダメージ計算を書く*/
#include "ActiveSkill.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "TurnManage.h"
#include "CharaList.h"

string ActiveSkill::skillLogic(Chara* used_chara,int skill_id,int skill_value,int used_index, string target_type, string extra_data){
	
	int enemy_index = 1;
	if(used_index>0) enemy_index = 0;
	
	use_skill_data = "";	//返り値
	data_string = "";	//演出データテキスト
    vector<Chara *> target_charas;
    HmsVector<int> value_dead_data;
	//スキル使用キャラ情報まとめ
    data_string += to_string(used_chara->getPrimaryId()) + "," + to_string(skill_id);
    HmsVector<string> target_types = split(target_type, "#");
    
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    
    auto removeGuardRemove = [](vector<Chara *> targets)->vector<Chara *> {//キャラのHMSベクターから除去無効持ちを排除するラムダ・参照渡し
        auto itr = targets.begin();
        while (itr != targets.end()) {
            if((*itr)->statusCheck(STATUS_TYPE_REMOVE_GUARD)){//除去無効持ちはターゲットしない
                targets.erase(itr);
            }else{
                itr++;
            }
        }
        return targets;
        
        /*while(true){//除去無効スキル持ちを抜かす
            bool removing = false;
            for(int i=0; i<targets.size(); i++){
                if(targets.at(i)->statusCheck(STATUS_TYPE_REMOVE_GUARD)){//除去無効持ちはターゲットしない
                    targets.erase(targets.begin() + i);
                    removing = true;
                    break;
                }
            }
            if(removing == false) break;//除去無効持ちがいないなら抜ける
        }*/
    };
    
	switch(skill_id){
		case 1:{//戦闘力UP：武将の戦闘力を？？％UPする
			//バフ処理(戦闘力UP/DOWN、貫通等)
			statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
			break;
		}
		case 2:{//戦闘力UP：味方武将の数＊？？％UP系
			int value = 0;
			int used_chara_count = 0;//発動した側の場にいる武将数
            for(Chara *chara : CharaList::getOneSide(used_index)){
                if(chara->getFieldIndex()>1) //軍師(0,1)を含まない※攻防武将のみカウント
                    used_chara_count++;
                
            }
            
			value = skill_value * used_chara_count;
			
			//バフ処理(戦闘力UP/DOWN、貫通等)
			statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
			break;
		}
		case 3:{//戦闘力UP：敵武将の数＊？？％UP系
			int value=0;
			int enemy_chara_count = 0;//発動した側の場にいる武将数
            for(Chara *chara : CharaList::getOneSide(enemy_index)){
                if(chara->getFieldIndex()>1) //軍師(0,1)を含まない※攻防武将のみカウント
                    enemy_chara_count++;
            }

			if(enemy_chara_count<=0) {
				//スキルエラー:敵武将がいない
				data_string = "error&2";
				break;
			}
			value = skill_value * enemy_chara_count;
			
			//バフ処理(戦闘力UP/DOWN、貫通等)
			statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
			break;
		}
		case 4:{//戦闘力DOWN：戦闘力＊？？％DOWN系
			//バフ処理(戦闘力UP/DOWN、貫通等)
			statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
			break;
		}
		case 5:{//戦闘力DOWN：味方武将の数＊？？％DOWN系
			int value = 0;
			int used_chara_count = 0;//発動した側の場にいる武将数
            for(Chara *chara : CharaList::getOneSide(used_index)){
                if(chara->getFieldIndex()>1) //軍師(0,1)を含まない※攻防武将のみカウント
                    used_chara_count++;
            }
            
			value = skill_value * used_chara_count;
			
			//バフ処理(戦闘力UP/DOWN、貫通等)
			statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
			break;
		}
		case 6:{//戦闘力DOWN：敵武将の数＊？？％DOWN系
			int value = 0;
			int enemy_chara_count = 0;//発動した側の場にいる武将数
            for(Chara *chara : CharaList::getOneSide(enemy_index)){
                if(chara->getFieldIndex()>1) //軍師(0,1)を含まない※攻防武将のみカウント
                    enemy_chara_count++;
            }
            
			value = skill_value * enemy_chara_count;
			
			//バフ処理(戦闘力UP/DOWN、貫通等)
			statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
			break;
		}
		case 7:{//プレイヤーダメージ：敵プレイヤーに？？ダメージ
            
			int damage1 = dung->player[enemy_index]->directDamage(skill_value);
			data_string += dataStringTargetChara(0, damage1);
            
            if(target_type == "all"){
                int damage2 = dung->player[used_index]->directDamage(skill_value);
                data_string += dataStringTargetChara(1, damage2);
            }
            
			break;
		}
		case 8:{//HP回復：固定値回復
			
			int heal_value = dung->player[used_index]->heal(skill_value);
			
			//スキル対象キャラ情報データ作成※対象がプレイヤーの為、0指定
			data_string += dataStringTargetChara(0, heal_value);
			break;
		}
		case 9:{//HP回復：？？％回復
			
            int heal_value = dung->player[used_index]->heal(dung->player[used_index]->getHp() * skill_value / 100.0f);
            CCLOG("回復量：%d",heal_value);
			//スキル対象キャラ情報データ作成※対象がプレイヤーの為、0指定
			data_string += dataStringTargetChara(0, heal_value);
			break;
		}
        case 10:{//除去：最も戦闘力の高い敵武将1体除去
            //除去対象を選定
            vector<Chara *> chara_list = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            //除去無効キャラを取り除いた上でターゲットを取得
            
            if(chara_list.size()>0) {//最も強いキャラを選定
                Chara *strong_chara;
                for(int i=0; i<chara_list.size(); i++){
                    if(i==0){
                        strong_chara = chara_list.at(i);
                    }else{
                        if(strong_chara->getValue() < chara_list.at(i)->getValue()){
                            strong_chara = chara_list.at(i);
                        }
                    }
                }
                target_charas.push_back(strong_chara);
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    int hit_flg = 0;
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    if(skill_value>rand) {
                        target_charas.at(i)->setDeadFlg(true);
                        hit_flg = 1;
                    }else{
                        hit_flg = 0;
                    }
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);
                }
            }
            break;
        }
        case 11:{//除去：自分より戦闘力の低い敵武将一体を除去
            //除去対象を選定
            vector<Chara *> chara_list = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            //除去無効キャラを取り除いた上でターゲットを取得
            
            vector<Chara *> weak_list;
            if(chara_list.size()>0) {//自分より戦闘力の低いキャラを選定
                for(int i=0; i<chara_list.size(); i++){
                    
                    if(used_chara->getValue() > chara_list.at(i)->getValue()){
                        weak_list.push_back(chara_list.at(i));
                    }
                }
                if(weak_list.size()>0) {//そのうち一体をターゲットに
                    target_charas.push_back(weak_list.at(std::uniform_int_distribution<int>(0, (int)weak_list.size()-1)(dung->rand_engine)));
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    int hit_flg = 0;
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    if(skill_value>rand) {
                        target_charas.at(i)->setDeadFlg(true);
                        hit_flg = 1;
                    }else{
                        hit_flg = 0;
                    }
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);
                }
            }
			break;
		}
            
		case 12:{//除去：敵軍師を除去する系  １４に統合
			
            
            
			break;
		}
		case 13:{//除去：戦闘力が？？以下の敵武将をすべて除去する系 確定
			//除去対象を選定
            vector<Chara *> chara_list = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            //除去無効キャラを取り除いた上でターゲットを取得
            
            if(chara_list.size()>0) {//自分より戦闘力の低いキャラを選定
                for(int i=0; i<chara_list.size(); i++){
                    
                    if(skill_value >= chara_list.at(i)->getValue()){
                        target_charas.push_back(chara_list.at(i));
                    }
                }
            }
            
			if(target_charas.empty()) {
				data_string = "error&1";//スキルエラー:対象がいない
			} else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
		}
		case 14:{//除去：敵武将を確率で除去する 前方　右前方　前方３体　など
			//除去対象を選定
			if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
			} else {//スキル使用側
				target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
                //除去無効キャラを取り除いた上でターゲットを取得
			}
            
			if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    if(skill_value>rand) {
                        target_charas.at(i)->setDeadFlg(true);
                        value_dead_data.insert(1);
                    }else{
                        value_dead_data.insert(0);
                    }
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(value_dead_data.at(i));
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(value_dead_data.at(i));
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), value_dead_data.at(i));
                }
            }
			break;
		}
		case 15:{//犠牲：自分を犠牲にして、敵武将を除去する
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
                //除去無効キャラを取り除いた上でターゲットを取得
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //スキル使用キャラを消滅
                used_chara->setDeadFlg(true);
                
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    int hit_flg = 0;
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    if(skill_value>rand) {
                        target_charas.at(i)->setDeadFlg(true);
                        hit_flg = 1;
                    }else{
                        hit_flg = 0;
                    }
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);
                }
            }
            break;
        }
        case 16:{//犠牲：自分を犠牲にして、最も戦闘力の高い敵武将を除去する
            //除去対象を選定
            vector<Chara *> chara_list = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            
            if(chara_list.size()>0) {//最も強いキャラを選定
                Chara *strong_chara;
                for(int i=0; i<chara_list.size(); i++){
                    if(i==0){
                        strong_chara = chara_list.at(i);
                    }else{
                        if(strong_chara->getValue() < chara_list.at(i)->getValue()){
                            strong_chara = chara_list.at(i);
                        }
                    }
                }
                target_charas.push_back(strong_chara);
            }
            
			if(target_charas.empty()) {
				data_string = "error&1";//スキルエラー:対象がいない
			} else {
                //スキル使用キャラを消滅
                used_chara->setDeadFlg(true);
                
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    int hit_flg = 0;
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    
                    if(skill_value>rand) {
                        target_charas.at(i)->setDeadFlg(true);
                        hit_flg = 1;
                    }else{
                        hit_flg = 0;
                    }
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);
                }
            }
            break;
		}
		case 17:{//犠牲：自分を犠牲にして、前方などの敵武将を除去する   15と同じ
			
			break;
		}
		/*case 18:{//犠牲：自分を犠牲にして、相手プレイヤーに？？ダメージ与える
			//スキル使用キャラを消滅
			used_chara->setDeadFlg(true);
			
            float plus_per = 0.0f;
            float minus_per = 0.0f;
            for(int i=0; i< dung->player[used_chara->getBelongingIndex()]->whole_status_passive.size(); i++){
                if(dung->player[used_chara->getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    plus_per += dung->player[used_chara->getBelongingIndex()]->whole_status_passive.at(i)["value"];
                }
            }
            for(int i=0; i< dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.size(); i++){
                if(dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
                    minus_per += dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.at(i)["value"];
                }
            }
            if(minus_per > 100.0f) minus_per = 100.0f;
            
            int after_value = skill_value * (100.0f + plus_per)/100.0f * (100.0f - minus_per)/100.0f;
			
			int damage = dung->player[enemy_index]->directDamage(after_value);
			
			//スキル対象キャラ情報データ作成※対象が相手プレイヤーの為、0指定
			data_string += dataStringTargetChara(0, damage);
			break;
		}*/
		case 19:{//貫通：武将に貫通効果をつける系
			//バフ処理
			statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_SPEAR,true);
			break;
		}
		case 20:{//ジャマー：敵武将を行動不能にする系
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_CANT_ACTION, true);
            break;
        }
            
		case 21:{//ジャマー：敵武将のアクティブスキルを封印する系
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_CANT_SKILL, true);
              break;
        }
            
		case 22:{//大撹乱:フィールドにいる武将の戦闘力を反転する。
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), 0, true);
			break;
		}
		case 23:{//敵のバフ解除系
            
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            }else{//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            for(auto chara : target_charas){
                if(use_skill_data=="") use_skill_data = to_string(chara->getPrimaryId()) + "&" + to_string(1);//データ送信に利用
                else use_skill_data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(1);
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if((*itr)->chara_status_active.size() == 0){//アクティブ効果ないなら
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                Chara *temp_chara = target_charas.at(std::uniform_int_distribution<int>(0, (int)target_charas.size()-1)(dung->rand_engine));//ランダム一体選択
                int hit_flg = 0;
                int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                if(skill_value>rand) {
                    temp_chara->chara_status_active.clear();//すべて消去
                    hit_flg = 1;
                }else{
                    hit_flg = 0;
                }
                
                data_string += dataStringTargetChara(temp_chara->getPrimaryId(), hit_flg);//エフェクトに利用
            }
            break;
        }
	
        case 24:{//デバフ解除
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            }else{//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            for(auto chara : target_charas){
                if(use_skill_data=="") use_skill_data = to_string(chara->getPrimaryId()) + "&" + to_string(1);//データ送信に利用
                else use_skill_data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(1);
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if((*itr)->chara_status_active.size() == 0){//アクティブ効果ないなら
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    int hit_flg = 0;
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    if(skill_value>rand) {
                        target_charas.at(i)->chara_status_active.clear();//すべて消去
                        hit_flg = 1;
                    }else{
                        hit_flg = 0;
                    }
                    
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);//エフェクトに利用
                }
            }
            break;
        }
		case 25:{//直接攻撃力UP
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_PLAYER_ATTACK_UPDOWN);
   			break;
		}
            
		case 26:{//他者犠牲:対象武将を犠牲にし、戦闘力を吸収する
			
			if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
			}
            
			if(target_charas.empty()) {
				data_string = "error&1";//スキルエラー:犠牲対象がいない
			} else {
				
				//生贄さん除去
                for(int i=0; i<target_charas.size(); i++){
                    int sacrifice_chara_value = target_charas.at(i)->getValue();//生贄さんの戦闘力取得
        
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //ステータスバッファに、使用キャラ、効果量、残りターンを追加
                    map<string,int> temp;
                    temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
                    temp.insert( map<string, int>::value_type("skill_id", skill_id));
                    temp.insert( map<string, int>::value_type("value", sacrifice_chara_value));
                    temp.insert( map<string, int>::value_type("turn", used_chara->getActiveSkillTurn()*2));
                    used_chara->chara_status_active.push_back(temp);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
			}
			break;
		}
        case 27:{//味方一体犠牲　対象除去
            if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));//対象の敵
            }
            
            vector<Chara *> target_chara_list = getTargetCharas(used_chara, CharaList::getAll().getVector(), "own_other");//自分以外の味方武将（犠牲予定）
            
            if(target_charas.empty() || target_chara_list.empty()){//犠牲にできるキャラがいないか対象敵キャラがいない時
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                
                Chara *target_cha = target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine));
                target_cha->setDeadFlg(true);//犠牲の味方に死亡フラグ
                
                data_string += dataStringTargetChara(target_cha->getPrimaryId(), 1);//エフェクトに利用
                
                
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    int hit_flg = 0;
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    if(skill_value>rand) {
                        target_charas.at(i)->setDeadFlg(true);
                        hit_flg = 1;
                    }else{
                        hit_flg = 0;
                    }
            
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);
                }
            }
            break;
            
        }
            
		case 28:{//他者犠牲:戦闘力が一番高い敵キャラを除去
            vector<Chara *> target_chara_list;
            target_chara_list = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));//対象の敵
            if(target_chara_list.size()>0) {//最も強いキャラを選定
                Chara *strong_chara;
                for(int i=0; i<target_chara_list.size(); i++){
                    if(i==0){
                        strong_chara = target_chara_list.at(i);
                    }else{
                        if(strong_chara->getValue() < target_chara_list.at(i)->getValue()){
                            strong_chara = target_chara_list.at(i);
                        }
                    }
                }
                target_charas.push_back(strong_chara);
            }
            
            vector<Chara *> gisei_chara_list = getTargetCharas(used_chara, CharaList::getAll().getVector(), "own_other");//自分以外の味方武将（犠牲予定）
            
            if(target_charas.empty() || gisei_chara_list.empty()){//犠牲にできるキャラがいないか対象敵キャラがいない時
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                
                Chara *target_cha = gisei_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine));
                target_cha->setDeadFlg(true);//犠牲の味方に死亡フラグ
                
                data_string += dataStringTargetChara(target_cha->getPrimaryId(), 1);//エフェクトに利用
                
                
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    int hit_flg = 0;
                    int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                    if(skill_value>rand) {
                        target_charas.at(i)->setDeadFlg(true);
                        hit_flg = 1;
                    }else{
                        hit_flg = 0;
                    }
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);
                }
            }
            break;
		}
            
		case 29:{//他者犠牲:対象の敵キャラ1体除去   27と同じ
          
			break;
		}
        case 18:
		case 30:{//他者犠牲:総大将にダメージを与える
            //生贄対象選出
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
			}
            
			if(target_charas.empty()) {
				data_string = "error&1";//スキルエラー:犠牲対象がいない
			} else {
				float plus_per = 0.0f;
				float minus_per = 0.0f;
				for(int i=0; i< dung->player[used_chara->getBelongingIndex()]->whole_status_passive.size(); i++){
					if(dung->player[used_chara->getBelongingIndex()]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
						plus_per += dung->player[used_chara->getBelongingIndex()]->whole_status_passive.at(i)["value"];
					}
				}
				for(int i=0; i< dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.size(); i++){
					if(dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.at(i)["state"] == STATUS_TYPE_DIRECT_UPDOWN){
						minus_per += dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->whole_status_passive.at(i)["value"];
					}
				}
				if(minus_per > 100.0f) minus_per = 100.0f;
				
				int after_value = skill_value * (100.0f + plus_per)/100.0f * (100.0f - minus_per)/100.0f;
				int damage = dung->player[enemy_index]->directDamage(after_value);
				
				//生贄さん除去
                for(int i=0; i<target_charas.size(); i++){
                
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), damage);
                }
			}
			break;
		}
		case 31:{//誘惑系
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_CHARM, true);
            break;
        }
            
		case 32:{//シャフル:場にいる武将の位置を全てランダムに変える
            
            int target_count = 0;
            
            for(int ii=0; ii<2; ii++) {
                //初期化
                vector<int> target_chara_list;
                vector<int> field_index;
                for(int c=2; c<8; c++) field_index.push_back(c);	//2~7までの配列を作成
                
                //各対象キャラを選出
                for(int i=0; i<CharaList::getOneSide(ii).size(); i++) {
                    if(CharaList::getOneSide(ii).at(i)->getFieldIndex()>1) {//軍師(0,1)を含まない※攻防武将のみカウント
                        target_chara_list.push_back(i);
                        target_count++;
                    }
                }
                
                //新しいフィールド番号を設定
                for(int t=0; t<target_chara_list.size(); t++) {
                    HMSLOG("field_index size : %d", field_index.size());
                    int rand_key = std::uniform_int_distribution<int>(0, (int)field_index.size()-1)(dung->rand_engine);
                    int new_field_index = field_index.at(rand_key);
                    
                    //フィールド番号更新
                    CharaList::getOneSide(ii).at(target_chara_list.at(t))->setFieldIndex(new_field_index);
                    
                    //extra_data作成
                    if(use_skill_data!="") use_skill_data += ",";//最初の入力でない場合、区切り文字を入れる
                    use_skill_data += to_string(CharaList::getOneSide(ii).at(target_chara_list.at(t))->getPrimaryId()) + "&" + to_string(new_field_index);//対象ID&新しいフィールド番号
                    
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(CharaList::getOneSide(ii).at(target_chara_list.at(t))->getPrimaryId(), new_field_index);
                    
                    //使用したフィールド番号は破棄する※重複を防ぐため
                    field_index.erase(field_index.begin() + rand_key);
                }
            }
            if(target_count<=0) data_string = "error&1";//スキルエラー:対象がいない
            
			break;
		}
        case 33:{//戦闘力固定値UP
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
            
            break;
        }
        case 34:{//戦闘力固定値DOWN
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
            
            break;
        }
        case 35:{//戦闘力指定
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
            
            break;
        }
        case 36:{//鉄壁
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_STEEL_SHIELD, true);
            break;
        }
        case 37:{//味方を犠牲にして固定値回復
            //生贄対象選出
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:犠牲対象がいない
            } else {
                
                dung->player[used_index]->heal(skill_value);
                
                //生贄さん除去
                for(int i=0; i<target_charas.size(); i++){
                    if(target_charas.at(i)>0) {
                        target_charas.at(i)->setDeadFlg(true);
                    }
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
            
        case 38:{//味方を犠牲にして回復
            //生贄対象選出
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:犠牲対象がいない
            } else {
                int hp_plus = 0;
                for(int i=0; i<target_charas.size(); i++){
                    hp_plus += target_charas.at(i)->getValue();
                }
                dung->player[used_index]->heal(hp_plus);
                
                //生贄さん除去
                for(int i=0; i<target_charas.size(); i++){
                    if(target_charas.at(i)>0) {
                        target_charas.at(i)->setDeadFlg(true);
                    }
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
        case 39:{//除去：自分より戦闘力の高い敵武将一体を除去
            //除去対象を選定
            vector<Chara *> chara_list = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            
            if(chara_list.size()>0) {//自分より戦闘力の高いキャラを選定
                for(int i=0; i<chara_list.size(); i++){
                    if(used_chara->getValue() < chara_list.at(i)->getValue()){
                        target_charas.push_back(chara_list.at(i));
                    }
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                auto target_cha = target_charas.at(std::uniform_int_distribution<int>(0, (int)target_charas.size()-1)(dung->rand_engine));
                
                int hit_flg = 0;
                int rand = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                if(skill_value>rand) {
                    target_cha->setDeadFlg(true);
                    hit_flg = 1;
                }else{
                    hit_flg = 0;
                }
                //extra_data作成
                if(use_skill_data=="") use_skill_data = to_string(target_cha->getPrimaryId()) + "&" + to_string(hit_flg);
                else use_skill_data += "," + to_string(target_cha->getPrimaryId()) + "&" + to_string(hit_flg);
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_cha->getPrimaryId(), hit_flg);
                
            }
            break;
        }
        case 40:{//除去：戦闘力が？？以上の敵武将をすべて除去する系 確定
            //特定の戦闘力以上の敵武将を判別
            vector<Chara *> chara_list = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            
            if(chara_list.size()>0) {//戦闘力がバリューより高いキャラを選定
                for(int i=0; i<chara_list.size(); i++){
                    if(skill_value <= chara_list.at(i)->getValue()){
                        target_charas.push_back(chara_list.at(i));
                    }
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
        case 41:{//次のターン対象死亡
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_NEXT_TURN_DEAD, true);
            
            break;
        }
        case 42:{//手札入れ替え
            
            break;
        }
        case 43:{//対象コンボカラーチェンジ
            HmsVector<int> combo_direction;//左側は１右側は２
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                    combo_direction.insert(stoi(mini_extra_data_list.at(1)));
                }
            } else {//スキル使用側
                
                vector<Chara *> target_chara_list = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));//対象の敵
                
                for(int i=0; i<target_chara_list.size(); i++){
                    //if(target_chara_list.at(i)->combination_l != skill_value){
                        target_charas.push_back(target_chara_list.at(i));
                        combo_direction.insert(1);
                    //}
                    //if(target_chara_list.at(i)->combination_r != skill_value){//コンボカラーに変更の余地がある場合
                        target_charas.push_back(target_chara_list.at(i));
                        combo_direction.insert(2);
                    //}
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    if(combo_direction.at(i) == 1){//左側
                        target_charas.at(i)->setCombinationL(skill_value);
                    }else if(combo_direction.at(i) == 2){//右側
                        target_charas.at(i)->setCombinationR(skill_value);
                    }
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(combo_direction.at(i));
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(combo_direction.at(i));
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), combo_direction.at(i));
                }
            }
            break;
        }
            
        case 44:{//前方の敵とコンボカラー交換
            HmsVector<int> combo_direction;//左側は１右側は２
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                if(target_charas.at(0)->getBelongingIndex() == TurnManage::getPlayerIndex()){
                    int temp_l = used_chara->getCombinationL();
                    int temp_r = used_chara->getCombinationR();
                    used_chara->setCombinationL(target_charas.at(0)->getCombinationL());
                    used_chara->setCombinationR(target_charas.at(0)->getCombinationR());
                    target_charas.at(0)->setCombinationL(temp_l);
                    target_charas.at(0)->setCombinationR(temp_r);
                }else{
                    int temp_l = used_chara->getCombinationL();
                    int temp_r = used_chara->getCombinationR();
                    used_chara->setCombinationL(target_charas.at(0)->getCombinationR());
                    used_chara->setCombinationR(target_charas.at(0)->getCombinationL());
                    target_charas.at(0)->setCombinationL(temp_r);
                    target_charas.at(0)->setCombinationR(temp_l);
                }
                    
                //extra_data作成
                if(use_skill_data=="") use_skill_data = to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(1);
                else use_skill_data += "," + to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(1);
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(0)->getPrimaryId(), 1);
            }
            break;
        }
         
        case 45:{//ランダムコンボカラー変化
            HmsVector<int> combo_direction;//左側は１右側は２
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                    combo_direction.insert(stoi(mini_extra_data_list.at(1)));
                }
            } else {//スキル使用側
                
                if(target_types.at(0) == "own_one"){
                   for(int i=0; i<CharaList::getOneSide(used_index).size(); i++){
                        if(CharaList::getOneSide(used_index).at(i)->getFieldIndex()>1){
                            if(CharaList::getOneSide(used_index).at(i)->getCombinationL() != skill_value){
                                target_charas.push_back(CharaList::getOneSide(used_index).at(i));
                                combo_direction.insert(1);
                            }
                            if(CharaList::getOneSide(used_index).at(i)->getCombinationR() != skill_value){
                                target_charas.push_back(CharaList::getOneSide(used_index).at(i));
                                combo_direction.insert(2);
                            }
                        }
                    }
                }else if(target_types.at(0) == "enemy_one"){
                    for(int i=0; i<CharaList::getOneSide(enemy_index).size(); i++){
                        if(CharaList::getOneSide(enemy_index).at(i)->getFieldIndex()>1){
                            if(CharaList::getOneSide(enemy_index).at(i)->getCombinationL() != skill_value){
                                target_charas.push_back(CharaList::getOneSide(enemy_index).at(i));
                                combo_direction.insert(1);
                            }
                            if(CharaList::getOneSide(enemy_index).at(i)->getCombinationR() != skill_value){
                                target_charas.push_back(CharaList::getOneSide(enemy_index).at(i));
                                combo_direction.insert(2);
                            }
                        }
                    }
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                
                int rand = random(0, (int)target_charas.size()-1);
                if(combo_direction.at(rand) == 1){//左側
                    target_charas.at(rand)->setCombinationL(skill_value);
                }else if(combo_direction.at(rand) == 2){//右側
                    target_charas.at(rand)->setCombinationR(skill_value);
                }
                //extra_data作成
                if(use_skill_data=="") use_skill_data = to_string(target_charas.at(rand)->getPrimaryId()) + "&" + to_string(combo_direction.at(rand));
                else use_skill_data += "," + to_string(target_charas.at(rand)->getPrimaryId()) + "&" + to_string(combo_direction.at(rand));
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(rand)->getPrimaryId(), combo_direction.at(rand));
                
            }
            break;
        }
            
        case 46:{//攻撃による総大将へのダメージを減少させる
            break;
        }
            
        case 47:{//手札公開
            break;
        }
            
        case 48:{//対象の中の軍師の残りターン数を減らす
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setValue(target_charas.at(i)->getValue()-skill_value);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
        case 49:{//対象の中の軍師の残りターン数を増やす
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setValue(target_charas.at(i)->getValue()+skill_value);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
        case 50:{//対象のキャラをランダムな場所に弾き飛ばす
            target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                
                vector<int> field_index;//ターゲットの場所以外で置けるフィールド
                vector<int> target_fields;//ターゲットの場所
                if(target_type == "forward"){
                    target_fields.push_back(9 - used_chara->getFieldIndex());
                }else if(target_type == "forward_3"){
                    if(10 - used_chara->getFieldIndex() >=2 && 10 - used_chara->getFieldIndex() <=7) target_fields.push_back(10 - used_chara->getFieldIndex());
                    if(9 - used_chara->getFieldIndex() >=2 && 9 - used_chara->getFieldIndex() <=7) target_fields.push_back(9 - used_chara->getFieldIndex());
                    if(8 - used_chara->getFieldIndex() >=2 && 8 - used_chara->getFieldIndex() <=7) target_fields.push_back(8 - used_chara->getFieldIndex());
                }else if(target_type == "cross"){
                    if(10 - used_chara->getFieldIndex() >=2 && 10 - used_chara->getFieldIndex() <=7) target_fields.push_back(10 - used_chara->getFieldIndex());
                    if(8 - used_chara->getFieldIndex() >=2 && 8 - used_chara->getFieldIndex() <=7) target_fields.push_back(8 - used_chara->getFieldIndex());
                }
                
                for(int i=2; i<=7; i++){
                    auto itr = target_fields.end();
                    itr = find(target_fields.begin(), target_fields.end(), i);
                    if(itr != target_fields.end()){//同じものがあったら
                        continue;
                    }
                    
                    if(CharaList::getAtField(enemy_index, i) != nullptr){//同じものがあったら
                        continue;
                    }
                    
                    field_index.push_back(i);
                }
                
                shuffle(field_index.begin(),field_index.end(),dung->rand_engine);//シャッフル
                shuffle(target_fields.begin(),target_fields.end(),dung->rand_engine);
                shuffle(target_charas.begin(),target_charas.end(),dung->rand_engine);
                
                
                for(int i=0; i<target_charas.size(); i++){
                    if(field_index.size()>0){
                        //CCLOG("フィールドインデックスがある時");
                        target_charas.at(i)->setFieldIndex(field_index.back());
                        field_index.pop_back();
                    }else{
                        int target_index = target_charas.at(i)->getFieldIndex();
                        auto itr = target_fields.end();
                        itr = find_if(target_fields.begin(), target_fields.end(),
                                      [target_index](int temp){return temp != target_index;});
                        if(itr != target_fields.end()){
                            target_charas.at(i)->setFieldIndex(*itr);
                            target_fields.erase(itr);
                            //CCLOG("インデックス%dに移動します",*itr);
                        }else{
                            auto itr3 = target_fields.end();
                            itr3 = find(target_fields.begin(), target_fields.end(), target_charas.at(i)->getFieldIndex());
                            if(itr3 != target_fields.end()) target_fields.erase(itr3);
                            //CCLOG("移動せず、%dのままです",target_charas.at(i)->getFieldIndex());
                        }
                    }
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(target_charas.at(i)->getFieldIndex());
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(target_charas.at(i)->getFieldIndex());
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), target_charas.at(i)->getFieldIndex());
                }
            }
            break;
        }
        case 51:{//行動不能キャラを除去
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if((*itr)->statusCheck(STATUS_TYPE_CANT_ACTION) == false){//行動不能じゃないなら
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
        case 52:{//対象と同じカードの配置士気を上げる
            vector<Chara *> temp_charas;
            if(extra_data!="") {//スキル使用された側
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            for(auto chara : temp_charas){
                if(use_skill_data=="") use_skill_data = to_string(chara->getPrimaryId()) + "&" + to_string(1);//データ送信に利用
                else use_skill_data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(1);
            }
            
            for(int i=0; i<temp_charas.size(); i++){
                for(auto chara : CharaList::getAll()){
                    if(temp_charas.at(i)->getCharaId() == chara->getCharaId()){
                        target_charas.push_back(chara);//ターゲットと同じキャラidのキャラがいた時
                    }
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    target_charas.at(i)->place_cost += skill_value;
                    
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
            
        case 53:{//対象と同じカードが相手の手札かデッキにある時、それらを消し去る
            vector<Chara *> temp_charas;
            if(extra_data!="") {//スキル使用された側
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            } else {//スキル使用側
                temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            for(auto chara : temp_charas){
                if(use_skill_data=="") use_skill_data = to_string(chara->getPrimaryId()) + "&" + to_string(1);//データ送信に利用
                else use_skill_data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(1);
            }
            
            for(int i=0; i<temp_charas.size(); i++){
                for(auto chara : CharaList::getOneSide(enemy_index)){
                    if(temp_charas.at(i)->getCharaId() == chara->getCharaId()){
                        target_charas.push_back(chara);//ターゲットと同じキャラidのキャラがいた時
                    }
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    if(target_charas.at(i)->getFieldIndex() < 0){//フィールド外のキャラを削除
                        target_charas.at(i)->setDeadFlg(true);
                        target_charas.at(i)->eliminate_flg = true;
                    }
                    
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
            
        case 54:{//敵総大将の最大HP低下
            dung->player[enemy_index]->setMaxHp(dung->player[enemy_index]->getMaxHp() - skill_value);
            
            //スキル対象キャラ情報データ作成※対象がプレイヤーの為、0指定
            data_string += dataStringTargetChara(CharaList::getLeader(enemy_index)->getPrimaryId(), skill_value);
            break;
        }
            
        case 55:{//味方総大将の最大HP上昇
            dung->player[used_index]->setMaxHp(dung->player[used_index]->getMaxHp() + skill_value);
            data_string += dataStringTargetChara(CharaList::getLeader(used_index)->getPrimaryId(), skill_value);
            break;
        }
            
        case 56:{//総大将の最大HPを現在のHPにする
            if(target_type == "enemy"){
                dung->player[enemy_index]->setMaxHp(dung->player[enemy_index]->getHp());
                data_string += dataStringTargetChara(CharaList::getLeader(enemy_index)->getPrimaryId(), skill_value);
            }else{
                dung->player[used_index]->setMaxHp(dung->player[used_index]->getHp());
                data_string += dataStringTargetChara(CharaList::getLeader(used_index)->getPrimaryId(), skill_value);
            }
            break;
        }
            
        case 57:{//対象の中で配置士気がX以下の武将を除去する
            if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if(((*itr)->getPlaceCost() <= skill_value) == false){//対象になるもの以外は排除
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
        case 58:{//根性
            
            
            break;
        }
        case 59:{//相手の士気をXにする
            dung->player[enemy_index]->setNowPower(skill_value);
            
            break;
        }
        case 60:{//上昇士気をXに固定する
            
            
            break;
        }
        case 61:{//武将の戦闘力を1~X上昇する
            statusBuffer(used_chara, skill_id, std::uniform_int_distribution<int>(1 ,skill_value)(dung->rand_engine), used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
            break;
        }
        case 62:{//武将の戦闘力を1~X下げる
            statusBuffer(used_chara, skill_id, std::uniform_int_distribution<int>(1 ,skill_value)(dung->rand_engine), used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn());
            break;
        }
        case 63:{//相手の手札の引く数を制限する
            
            break;
        }
        case 64:{//相手はXコストのカードを出すことができない
            
            break;
        }
        case 65:{//対象の戦闘タイプをXにする
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            }else{//スキル使用側
                vector<Chara *> temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
                
                for(int i=0; i<temp_charas.size(); i++) {
                    target_charas.push_back(temp_charas.at(i));
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setType(skill_value);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
            
        case 66:{//基本戦闘力をX％あげる
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            }else{//スキル使用側
                vector<Chara *> temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
                
                for(int i=0; i<temp_charas.size(); i++) {
                    target_charas.push_back(temp_charas.at(i));
                }
            }
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setBaseValue(target_charas.at(i)->getBaseValue() * (100.0f+skill_value)/100.0f);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
            
        case 67:{//基本戦闘力をXあげる
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            }else{//スキル使用側
                vector<Chara *> temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
                
                for(int i=0; i<temp_charas.size(); i++) {
                    target_charas.push_back(temp_charas.at(i));
                }
            }
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setBaseValue(target_charas.at(i)->getBaseValue()+skill_value);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
           
        case 68:{//コンボカラーXが含まれてい無いキャラを除去
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if((*itr)->getCombinationL() == skill_value || (*itr)->getCombinationR() == skill_value){//コンボカラーあるなら
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //int damage = 0;
                //if(skill_value > 0) damage = dung->player[enemy_index]->directDamage(skill_value * target_charas.size());
                CCLOG("てすーサイズ%d",(int)target_charas.size());
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
            
        case 69:{//行動不能キャラを除去+ダメージ
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    temp_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_types.at(0));
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if((*itr)->statusCheck(STATUS_TYPE_CANT_ACTION)==false){//行動不能でないなら
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                int damage = 0;
                if(skill_value > 0) damage = dung->player[enemy_index]->directDamage(skill_value * (int)target_charas.size());
                CCLOG("てすーサイズ%d",(int)target_charas.size());
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setDeadFlg(true);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), damage);
                }
            }
            break;
        }
            
        case 70:{//相手のHPをX/1000にする
            int rest_hp = (int)(dung->player[enemy_index]->getHp() * skill_value / 1000.0f);
            if(rest_hp==0) rest_hp=1;//このスキルで死亡させない
            
            int after_value =  dung->player[enemy_index]->getHp() - rest_hp;
            int damage = dung->player[enemy_index]->directDamage(after_value);
            
            //スキル対象キャラ情報データ作成※対象が相手プレイヤーの為、0指定
            data_string += dataStringTargetChara(0, damage);
            break;
        }
        case 71:{//行動不能無効を付与
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_CANT_ACTION_GUARD);
            break;
        }
        case 72:{//除去無効を付与
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_REMOVE_GUARD);
            break;
        }
        case 73:{//２回攻撃を付与
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_TWICE_ATTACK);
            break;
        }
        case 74:{//基本戦闘力をXにする
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    int target_primary_id = stoi(mini_extra_data_list.at(0));
                    for(Chara *chara : CharaList::getAll()){
                        if(chara->getPrimaryId()==target_primary_id) {
                            target_charas.push_back(chara);
                        }
                    }
                }
            }else{//スキル使用側
                vector<Chara *> temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
                
                for(int i=0; i<temp_charas.size(); i++) {
                    target_charas.push_back(temp_charas.at(i));
                }
            }
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(int i=0; i<target_charas.size(); i++){
                    
                    target_charas.at(i)->setBaseValue(skill_value);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
            
        case 75:{//対象のキャラを自分の前方に移動させる
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    int target_primary_id = stoi(mini_extra_data_list.at(0));
                    for(Chara *chara : CharaList::getAll()){
                        if(chara->getPrimaryId()==target_primary_id) {
                            target_charas.push_back(chara);
                        }
                    }
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            }else if(getTargetCharas(used_chara, CharaList::getAll().getVector(), "forward").empty() == false){
                data_string = "error&4";//スキルエラー:前方が埋まっている
            
            } else {
                target_charas.at(0)->setFieldIndex(9-used_chara->getFieldIndex());
                
                //extra_data作成
                if(use_skill_data=="") use_skill_data = to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(target_charas.at(0)->getFieldIndex());
                else use_skill_data += "," + to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(target_charas.at(0)->getFieldIndex());
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(0)->getPrimaryId(), target_charas.at(0)->getFieldIndex());
            }
            break;
        }
            
        case 76:{//対象を山札の一番上に戻す
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    int target_primary_id = stoi(mini_extra_data_list.at(0));
                    for(Chara *chara : CharaList::getAll()){
                        if(chara->getPrimaryId()==target_primary_id) {
                            target_charas.push_back(chara);
                        }
                    }
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(int i=0; i<target_charas.size(); i++){
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(1);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            break;
        }
        case 77:{//的総大将にXダメージを与え、士気１を得る
            
            
            int damage = dung->player[used_index]->directDamage(skill_value);
            data_string += dataStringTargetChara(0, damage);
            
            break;
        }
        
        case 78:{//対象のキャラ一体のアクティブを得る・奪う
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    int target_primary_id = stoi(mini_extra_data_list.at(0));
                    for(Chara *chara : CharaList::getAll()){
                        if(chara->getPrimaryId()==target_primary_id) {
                            target_charas.push_back(chara);
                        }
                    }
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                
                //extra_data作成
                if(use_skill_data=="") use_skill_data = to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(1);
                else use_skill_data += "," + to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(1);
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(0)->getPrimaryId(), 1);
            }
            break;
        }
        
        case 79:{//対象のキャラ一体とアクティブスキル交換
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    int target_primary_id = stoi(mini_extra_data_list.at(0));
                    for(Chara *chara : CharaList::getAll()){
                        if(chara->getPrimaryId()==target_primary_id) {
                            target_charas.push_back(chara);
                        }
                    }
                }
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                
                //extra_data作成
                if(use_skill_data=="") use_skill_data = to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(skill_value);
                else use_skill_data += "," + to_string(target_charas.at(0)->getPrimaryId()) + "&" + to_string(skill_value);
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(0)->getPrimaryId(), skill_value);
            }
            break;
        }
        
        case 80:{//雪だるま設置
            vector<pair<int, int>> place_vec;
            
            if(extra_data!="") {//スキル使用された側
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    place_vec.push_back(make_pair(stoi(mini_extra_data_list.at(1)),stoi(mini_extra_data_list.at(2))));
                    
                }
            } else {//スキル使用側
                place_vec = getTargetPlace(used_chara, target_types.at(0));
            }
            
            if(place_vec.empty()) {
                data_string = "error&5";//スキルエラー:対象がいない
            } else {
                for(pair<int,int> x : place_vec){
                    map<string,int> temp;
                    temp["primary"] = used_chara->getPrimaryId();
                    temp["skill_id"] = used_chara->getActiveSkillId();
                    temp["value"] = x.second;
                    temp["turn"] = used_chara->getActiveSkillTurn()*2;
                    temp["state"] = STATUS_TYPE_SNOWMAN;//
                    
                    dung->player[x.first]->whole_status_active.push_back(temp);//パッシブ効果の追加
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(used_chara->getPrimaryId()) + "&" + to_string(x.first)+ "&" + to_string(x.second);
                    else use_skill_data += "," + to_string(used_chara->getPrimaryId()) + "&" + to_string(x.first)+ "&" + to_string(x.second);
                    
                    data_string += "," + to_string(used_chara->getPrimaryId()) + "&" + to_string(x.first)+ "&" + to_string(x.second);
                }
            }
            break;
        }
            
        case 81:{//おみくじ
            int omi_result = 1;
            
            if(extra_data!="") {//スキル使用された側
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    omi_result = stoi(mini_extra_data_list.at(1));
                }
                
            } else {//スキル使用側
                int omi_rand = std::uniform_int_distribution<int>(1, 10)(dung->rand_engine);;
                
                if(omi_rand >= 1 && omi_rand <=3){
                    omi_result = 1;//大吉
                    CCLOG("大吉");
                }else if(omi_rand >=4 && omi_rand <=6){
                    omi_result = 2;//中吉
                    CCLOG("中吉");
                }else if(omi_rand >=7 && omi_rand <=9){
                    omi_result = 3;//小吉
                    CCLOG("小吉");
                }else{
                    omi_result = 4;//凶
                    CCLOG("凶");
                }
                //omi_result = 1;
            }
            
            if(omi_result == 4) used_chara->setDeadFlg(true);
            
            //extra_data作成
            if(use_skill_data=="") use_skill_data = to_string(used_chara->getPrimaryId()) + "&" + to_string(omi_result);
            else use_skill_data += "," + to_string(used_chara->getPrimaryId()) + "&" + to_string(omi_result);
            //スキル対象キャラ情報データ作成
            data_string += dataStringTargetChara(used_chara->getPrimaryId(), omi_result);
            
            break;
        }
        
        case 82:{//対象地点にサル召喚
            vector<pair<int, int>> place_vec;
            int rand_rare = 0;
            CCLOG("extra:%s",extra_data.c_str());
            if(extra_data!="") {//スキル使用された側か　場所指定
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    place_vec.push_back(make_pair(stoi(mini_extra_data_list.at(1)),stoi(mini_extra_data_list.at(2))));
                    if(mini_extra_data_list.size() > 3)rand_rare = stoi(mini_extra_data_list.at(3));
                }
                if(rand_rare == 0) rand_rare = std::uniform_int_distribution<int>(1, 4)(dung->rand_engine);
                
            } else {//スキル使用側
                place_vec = getTargetPlace(used_chara, target_types.at(0));
                rand_rare = std::uniform_int_distribution<int>(1, 4)(dung->rand_engine);
            }
            
            if(place_vec.empty()) {
                data_string = "error&5";//スキルエラー:対象がいない
            } else {
                for(pair<int,int> x : place_vec){
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(used_chara->getPrimaryId()) + "&" + to_string(x.first)+ "&" + to_string(x.second)+ "&" + to_string(rand_rare);
                    else use_skill_data += "," + to_string(used_chara->getPrimaryId()) + "&" + to_string(x.first)+ "&" + to_string(x.second)+ "&" + to_string(rand_rare);
                    
                    data_string += "," + to_string(used_chara->getPrimaryId()) + "&" + to_string(x.first)+ "&" + to_string(x.second)+ "&" + to_string(rand_rare);
                }
            }
            
            break;
        }
            
		case 83:{//デバフ解除＋行動不能無効を付与＋戦闘力UP
            //対象の取得
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    int target_primary_id = stoi(mini_extra_data_list.at(0));
                    for(Chara *chara : CharaList::getAll()){
                        if(chara->getPrimaryId()==target_primary_id) {
                            target_charas.push_back(chara);
                        }
                    }
                }
            }else{//スキル使用側
                vector<Chara *> temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
                
                for(int i=0; i<temp_charas.size(); i++) {
                    target_charas.push_back(temp_charas.at(i));
                }
            }
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                    for(int i=0; i<target_charas.size(); i++){
                        //extra_data作成
                        if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(skill_value);
                        else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(skill_value);
                        //スキル対象キャラ情報データ作成
                        data_string += dataStringTargetChara(target_charas.at(0)->getPrimaryId(), 1);
                    }
            }
            break;
            
        }
            
        case 84:{//低下無効を付与
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_INVALID_DECREASE);
            break;
        }
            
        case 85:{//不屈（攻撃を受けても破壊されず行動不能に）を付与
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_ESCAPE_GUARD);
            break;
        }
            
        case 86:{//現在のターンは行動できないが、次のターン開始時に戦闘力がX上昇する
            //対象の取得
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            }else{//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                for(Chara *chara : target_charas){
                    
                    //ステータスバッファに、使用キャラ、効果量、残りターンを追加
                    map<string,int> temp;
                    temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
                    temp.insert( map<string, int>::value_type("skill_id", skill_id));
                    temp.insert( map<string, int>::value_type("value", skill_value));//戦闘力アップ数値
                    temp.insert( map<string, int>::value_type("turn", used_chara->getActiveSkillTurn()*2+2));//１ターン目になるとvalue値の分戦闘力上昇に変化
                    temp.insert( map<string, int>::value_type("state", STATUS_TYPE_ACCUMULATION));//ためる
                    chara->chara_status_active.push_back(temp);
                    
                    //extra_data作成
                    if(use_skill_data=="") use_skill_data = to_string(chara->getPrimaryId()) + "&" + to_string(skill_value);
                    else use_skill_data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(skill_value);
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(chara->getPrimaryId(), 1);
                }
            }
            break;
            
        }
          
        case 87:{//誘惑状態の対象を味方にし、誘惑状態を解除する
            //対象の取得
            if(extra_data!="") {//スキル使用された側
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
            }else{//スキル使用側
                vector<Chara *> temp_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
                
                for(int i=0; i<temp_charas.size(); i++) {
                    target_charas.push_back(temp_charas.at(i));
                }
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if((*itr)->statusCheck(STATUS_TYPE_CHARM)==false){//誘惑効果ないなら
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            int blank_sum=0;//味方フィールドの空いているます数
            for(int i=2; i<=7; i++){
                if(dung->fieldLayer->fieldBlankCheck(i, used_index) == true){
                    blank_sum++;
                }
            }
            
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            }else if(blank_sum==0){//空いてる場所がない
                data_string = "error&1";//スキルエラー:対象がいない
            }else {
                
                shuffle(target_charas.begin(), target_charas.end(), dung->rand_engine);//シャッフル
                
                for (auto target : target_charas) {
                    if(blank_sum > 0){//場所も空いている
                        //extra_data作成
                        if(use_skill_data=="") use_skill_data = to_string(target->getPrimaryId()) + "&" + to_string(skill_value);
                        else use_skill_data += "," + to_string(target->getPrimaryId()) + "&" + to_string(skill_value);
                        //スキル対象キャラ情報データ作成
                        data_string += dataStringTargetChara(target->getPrimaryId(), 1);
                        
                        blank_sum--;
                    }else{
                        break;
                    }
                }
            }
            break;
        }
            
        case 88:{//氷漬け
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, 9999, STATUS_TYPE_FREEZE);
            break;
        }
            
        case 89:{//X以下除去・そうでないならXダウン
            //除去対象を選定
            if(extra_data!="") {//スキル使用された側
                vector<Chara *> temp_charas;
                //extra_dataから対象キャラを検索
                HmsVector<string> extra_data_list = split(extra_data, ",");
                for(int m=0; m<extra_data_list.size(); m++) {
                    HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
                    target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
                }
                //target_charas = removeGuardRemove(temp_charas);
            } else {//スキル使用側
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_types.at(0));
                //除去無効キャラを取り除いた上でターゲットを取得
            }
            
            auto itr = target_charas.begin();
            while (itr!=target_charas.end()) {
                if((*itr)->statusCheck(STATUS_TYPE_REMOVE_GUARD) && (*itr)->getValue()<=skill_value){//除去対象で除去回避があるのはなし
                    target_charas.erase(itr);//消す
                }else{
                    itr++;
                }
            }
            
            if(target_charas.empty()) {
                data_string = "error&1";//スキルエラー:対象がいない
            } else {
                //除去判定
                for(auto chara : target_charas){
                    if(chara->getValue() <= skill_value) {//除去いけるとき
                        chara->setDeadFlg(true);
                        
                        if(use_skill_data=="") use_skill_data = to_string(chara->getPrimaryId()) + "&" + to_string(1);
                        else use_skill_data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(1);
                        data_string += dataStringTargetChara(chara->getPrimaryId(), 1);
                        
                    }else{//低下の時
                        map<string,int> temp;
                        temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
                        temp.insert( map<string, int>::value_type("skill_id", 34));
                        temp.insert( map<string, int>::value_type("value", skill_value));
                        temp.insert( map<string, int>::value_type("turn", chara->getActiveSkillTurn()*2));
                        chara->chara_status_active.push_back(temp);
                        
                        if(use_skill_data=="") use_skill_data = to_string(chara->getPrimaryId()) + "&" + to_string(0);
                        else use_skill_data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(0);
                        data_string += dataStringTargetChara(chara->getPrimaryId(), 0);
                    }
                }
            }
            break;
        }
        case 90:{//アクティブスキル封印耐性
            statusBuffer(used_chara, skill_id, skill_value, used_index, target_types.at(0), extra_data, used_chara->getActiveSkillTurn(), STATUS_TYPE_CANT_SKILL_GUARD, true);
            break;
        }
            
            
            
            
	}
	
	//use_skill_data = extra_data(動的な対象キャラ情報) ## data_string(スキル演出用データ)
	use_skill_data += "##" + data_string;
	HMSLOG("use_skill_data = %s", use_skill_data.c_str());
    
    if(data_string != "error&1" && target_types.size()>=2 && target_types.at(1) == "only"){//一度しか使用できないスキルの場合
        HMSLOG("一度のみスキル");
        used_chara->active_skill_able_count = 0; //スキルずっと使用不能。ここから引いて-1に
    }else if(data_string != "error&1" && target_types.size()>=2 && target_types.at(1) == "once"){//一度しか使用できないスキルの場合
        used_chara->active_skill_able_count = 1; //スキル使用不能。ここから引いて0に
    }
    
	return use_skill_data;
}

string ActiveSkill::dataStringTargetChara(int primary_id, int value)//スキル対象キャラ情報データ作成
{
	string data = "";
	data = "," + to_string(primary_id) + "&" + to_string(value);
	HMSLOG("data:%s",data.c_str());
	return data;
}

//バフ処理(戦闘力UP/DOWN、貫通等)
void ActiveSkill::statusBuffer(Chara* used_chara, int skill_id, int skill_value, int used_index, string target_type, string extra_data, int invoke_turn, int state, bool probability_flg)
{
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
	int enemy_index = 1;
	if(used_index>0) enemy_index = 0;
	
	vector<Chara *> target_charas;
    
    if(extra_data!="") {//スキル使用された側
        //extra_dataから対象キャラを検索
        HmsVector<string> extra_data_list = split(extra_data, ",");
        for(int m=0; m<extra_data_list.size(); m++) {
            HmsVector<string> mini_extra_data_list = split(extra_data_list.at(m), "&");
            target_charas.push_back(CharaList::getAtPrimary(stoi(mini_extra_data_list.at(0))));
        }
    }else{//スキル使用する側 or extra_data無し
        target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_type);
    }
    
    if(target_charas.empty()) {
        data_string = "error&1";//スキルエラー:対象がいない
    } else {
        for(int i=0; i<target_charas.size(); i++){
            int hit_flg = 1;
            if(probability_flg == true){//スキル効果の確率判定
                int rand_num = std::uniform_int_distribution<int>(0, 99)(dung->rand_engine);
                if(rand_num<skill_value){
                    hit_flg = 1;
                }else{
                    hit_flg = 0;
                }
            }
            
            if(hit_flg == 1){//確率ではないか、確率で効果適用のとき
                if(state == STATUS_TYPE_CANT_ACTION && target_charas.at(i)->statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)){//行動不能をガードしてるキャラにはつけない
                }else if(state == STATUS_TYPE_CANT_SKILL && target_charas.at(i)->statusCheck(STATUS_TYPE_CANT_SKILL_GUARD)){//スキル封印をガードしてるキャラにはつけない
                    
                }else{
                    //ステータスバッファに、使用キャラ、効果量、残りターンを追加
                    map<string,int> temp;
                    temp.insert( map<string, int>::value_type("primary", used_chara->getPrimaryId()));
                    temp.insert( map<string, int>::value_type("skill_id", skill_id));
                    temp.insert( map<string, int>::value_type("value", skill_value));
                    temp.insert( map<string, int>::value_type("turn", invoke_turn*2));
                    if(state > 0) temp.insert( map<string, int>::value_type("state", state));
                    
                    target_charas.at(i)->chara_status_active.push_back(temp);
                }
            }
            
            if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
            else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(hit_flg);
            
            //スキル対象キャラ情報データ作成
            data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), hit_flg);
        }
    }
    //if(target_charas.empty() && skill_id < 1000) data_string = "error&1"; //スキルエラー:対象がいない //カウンターの時これを出してはいけない
}

vector<pair<int, int>> ActiveSkill::getTargetPlace(Chara *used_chara, string target_type){
    
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    int used_index = used_chara->getBelongingIndex();
    int enemy_index = used_chara->reverseIndex(used_chara->getBelongingIndex());
    
    vector<pair<int, int>> empty_place;//敵味方の空き地
    vector<pair<int, int>> result_place;//返り値

    for(int i=0; i<=7; i++){//味方の空き地の追加
        if(dung->fieldLayer->getCharaFieldIndex(i, used_index) == nullptr
           && dung->fieldLayer->placeAbleField(i, TurnManage::getPlayerIndex())){
            empty_place.push_back(make_pair(used_index, i));
        }
    }
    
    for(int i=0; i<=7; i++){//敵の空き地の追加
        if(dung->fieldLayer->getCharaFieldIndex(i, enemy_index) == nullptr
           && dung->fieldLayer->placeAbleField(i, TurnManage::getEnemyIndex())){
            empty_place.push_back(make_pair(enemy_index, i));
        }
    }
    
    if(target_type=="all") {//武将フィールド全体
        for(int j=0; j<empty_place.size(); j++){
            if(empty_place.at(j).second >=2 || empty_place.at(j).second <=7){
                result_place.push_back(empty_place.at(j));
            }
        }
        
    }else if(target_type=="own_all") {//味方の武将フィールド全体
        for(int j=0; j<empty_place.size(); j++){
            if(empty_place.at(j).first == used_index && empty_place.at(j).second >=2 && empty_place.at(j).second <=7){
                result_place.push_back(empty_place.at(j));
            }
        }
        
    }else if(target_type=="enemy_all") {//敵の武将フィールド全体
        for(int j=0; j<empty_place.size(); j++){
            if(empty_place.at(j).first == enemy_index && empty_place.at(j).second >=2 && empty_place.at(j).second <=7){
                result_place.push_back(empty_place.at(j));
            }
        }
        
    }else if(target_type=="enemy_field_all") {//敵の武将・軍師フィールド全体
        for(int j=0; j<empty_place.size(); j++){
            if(empty_place.at(j).first == enemy_index){
                result_place.push_back(empty_place.at(j));
            }
        }
    }else if(target_type=="forward") {//自分の前方の空き地
        for(int j=0; j<empty_place.size(); j++){
            if(empty_place.at(j).first == enemy_index && empty_place.at(j).second == 9-used_chara->getFieldIndex()){
                result_place.push_back(empty_place.at(j));
            }
        }
    }else if(target_type=="enemy_one") {//敵の武将の空き地ランダム一つ
        auto itr = empty_place.begin();
        while(itr != empty_place.end()){
            if((*itr).first != enemy_index || (*itr).second == 0 || (*itr).second == 1){
                empty_place.erase(itr);
            }else{
                itr++;
            }
        }
        
        /*for(auto itr = empty_place.begin(); itr != empty_place.end(); itr++){
            if((*itr).first != enemy_index || !((*itr).second == 0||(*itr).second == 1)){
                empty_place.erase(itr);
            }
        }*/
        
        if(empty_place.size()>0){
            pair<int, int> result = empty_place.at(std::uniform_int_distribution<int>(0,(int)empty_place.size()-1)(dung->rand_engine));
            result_place.push_back(result);
        }
    }else if(target_type=="enemy_field_one"){//敵の武将・軍師の空き地ランダム一つ
        auto itr = empty_place.begin();
        while(itr != empty_place.end()){
            if((*itr).first != enemy_index || (*itr).second >= 0){
                empty_place.erase(itr);
            }else{
                itr++;
            }
        }
        
        /*for(auto itr = empty_place.begin(); itr != empty_place.end(); itr++){
            if((*itr).first != enemy_index){
                empty_place.erase(itr);
            }
        }*/
        if(empty_place.size()>0){
            pair<int, int> result = empty_place.at(std::uniform_int_distribution<int>(0,(int)empty_place.size()-1)(dung->rand_engine));
            result_place.push_back(result);
        }
    }else if(target_type=="enemy_support_one"){
        
        auto itr = empty_place.begin();
        while(itr != empty_place.end()){
            if((*itr).first != enemy_index || (*itr).second >= 2){
                empty_place.erase(itr);
            }else{
                itr++;
            }
        }
        if(empty_place.size()>0){
            pair<int, int> result = empty_place.at(std::uniform_int_distribution<int>(0,(int)empty_place.size()-1)(dung->rand_engine));
            result_place.push_back(result);
        }
    }else if(target_type=="near"){
        
        int near_field = used_chara->getNearBlankFieldIndex();//キャラの近くのフィールド
        
        if(near_field != 0){
            result_place.push_back(make_pair(used_chara->getBelongingIndex(), near_field));
        }
    }
    
    return result_place;
}

vector<Chara *> ActiveSkill::getTargetCharas(Chara *used_chara, vector<Chara *> all_charas, string target_type){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    int used_index = used_chara->getBelongingIndex();
    int enemy_index = 1;
    if(used_index>0) enemy_index = 0;
    vector<Chara *> target_charas;
    
    if(target_type=="one" || target_type=="") {//自分自身を対象
        for(Chara *chara : all_charas){
            if(chara->getPrimaryId() == used_chara->getPrimaryId()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all") {//敵味方全体
        for(Chara *chara : all_charas){
            if(chara->getFieldIndex() >= 2) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_attack") {//敵味方全体 攻撃
        for(Chara *chara : all_charas){
            if(chara->getFieldIndex() >= 2 && chara->isAttacker()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_defense") {//敵味方全体　防御
        for(Chara *chara : all_charas){
            if(chara->getFieldIndex() >= 2 && chara->isDefender()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_balance") {//敵味方全体　攻防
        for(Chara *chara : all_charas){
            if(chara->getFieldIndex() >= 2 && chara->getType()==CHARA_TYPE_BALANCE) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_one") {//ランダムで味方武将１体を対象
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="own_all") {//全自武将を対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_attack") {//自分の攻撃武将を全て対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->isAttacker()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_defense") {//自分の防御武将を全て対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->isDefender()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_balance") {//自分の攻撃防御武将を全て対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getType()==CHARA_TYPE_BALANCE){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_one") {//ランダムで敵武将１体を対象
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="enemy_all") {//全敵武将を対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_attack") {//相手の攻撃武将を全て対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isAttacker()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_defense") {//相手の防御武将を全て対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isDefender()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_balance") {//相手の防御武将を全て対象
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getType()==CHARA_TYPE_BALANCE){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_one_attack") {//相手の攻撃武将のうち一体を対象
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isAttacker()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="enemy_one_defense") {//相手の防御武将のうち一体を対象
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isDefender()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="enemy_one_balance") {//相手の攻防武将のうち一体を対象
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getType()==CHARA_TYPE_BALANCE) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="right") {//右
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttackAndDefense() && chara->isOnField() && chara->getFieldIndex()==1+used_chara->getFieldIndex()){
                target_charas.push_back(chara);
                break;
            }
        }
        
    }else if(target_type=="left") {//左
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttackAndDefense() && chara->isOnField() && chara->getFieldIndex()==-1+used_chara->getFieldIndex()){
                target_charas.push_back(chara);
                break;
            }
        }
        
    }else if(target_type=="side") {//右と左
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttackAndDefense() && chara->isOnField()
               && (chara->getFieldIndex()==1+used_chara->getFieldIndex() || chara->getFieldIndex()==-1+used_chara->getFieldIndex())){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="forward") {//前方
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->isAttackAndDefense() && chara->isOnField() && chara->getFieldIndex()==9-used_chara->getFieldIndex()){
                target_charas.push_back(chara);
                break;
            }
        }
        
    }else if(target_type=="forward_attack") {//前方の攻撃武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->isAttacker() && chara->isOnField() && chara->getFieldIndex()==9-used_chara->getFieldIndex()){
                target_charas.push_back(chara);
                break;
            }
        }
        
    }else if(target_type=="forward_defense") {//前方の防御武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->isDefender() && chara->isOnField() &&chara->getFieldIndex()==9-used_chara->getFieldIndex()){
                target_charas.push_back(chara);
                break;
            }
        }
        
    }else if(target_type=="forward_right") {//右前方武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttackAndDefense()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_right_attack") {//右前方の攻撃
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttacker()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()){//要確認
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_right_defense") {//右前方の防御
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isDefender()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()){//要確認
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_left") {//左前方武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttackAndDefense()){
                if(chara->getFieldIndex()==10-used_chara->getFieldIndex()){//要確認
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_left_attack") {//左前方の攻撃
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttacker()){
                if(chara->getFieldIndex()==10-used_chara->getFieldIndex()){//要確認
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_left_defense") {//左前方の防御
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isDefender()){
                if(chara->getFieldIndex()==10-used_chara->getFieldIndex()){//要確認
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="cross") {//前方２方向
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttackAndDefense()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==10-used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="cross_attack") {//前方２方向の攻撃
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttacker()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==10-used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="cross_defense") {//前方２方向の防御
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isDefender()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==10-used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_3") {//前方3方向
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttackAndDefense()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==10-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==9-used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_3_one") {//前方３体の中のランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2){
                if(chara->getFieldIndex()==9-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==8-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==10-used_chara->getFieldIndex()){
                    
                    target_chara_list.push_back(chara);
                }
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="forward_3_attack") {//前方3方向の攻撃
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isAttacker()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==10-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==9-used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="forward_3_defense") {//前方3方向の防御
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex()>=2 && chara->isDefender()){
                if(chara->getFieldIndex()==8-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==10-used_chara->getFieldIndex()
                   || chara->getFieldIndex()==9-used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="own_3") {//右と左と自分
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex()>=2){
                if(chara->getFieldIndex()==used_chara->getFieldIndex()
                   || chara->getFieldIndex()==1+used_chara->getFieldIndex()
                   || chara->getFieldIndex()==-1+used_chara->getFieldIndex()){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="own_other") {//自分以外の味方武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getPrimaryId()!=used_chara->getPrimaryId()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_other_one") {//自分以外の味方武将ランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getPrimaryId()!=used_chara->getPrimaryId()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="own_other_attack") {//自分以外の味方攻撃武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getPrimaryId()!=used_chara->getPrimaryId() && chara->isAttacker()) {
                target_charas.push_back(chara);
            }
        }

    }else if(target_type=="own_other_defense") {//自分以外の味方防御武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getPrimaryId()!=used_chara->getPrimaryId() && chara->isDefender()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_other_one_attack") {//自分以外の味方攻撃武将ランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getPrimaryId()!=used_chara->getPrimaryId() && chara->isAttacker()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="own_other_one_defense") {//自分以外の味方防御武将ランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getPrimaryId()!=used_chara->getPrimaryId() && chara->isDefender()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
      
    }else if(target_type=="enemy_strong") {//敵の最大戦闘力の武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="enemy_attack_strong") {//敵の最大戦闘力の攻撃武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isAttacker()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="enemy_defense_strong") {//敵の最大戦闘力の防御武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isDefender()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="enemy_balance_strong") {//敵の最大戦闘力の攻防武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getType()==CHARA_TYPE_BALANCE) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="own_strong") {//味方の最大戦闘力の武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
    
    }else if(target_type=="own_attack_strong") {//味方の最大戦闘力の攻撃武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->isAttacker()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="own_defense_strong") {//味方の最大戦闘力の防御武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->isDefender()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="own_balance_strong") {//味方の最大戦闘力の攻防武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getType()==CHARA_TYPE_BALANCE) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*max_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }

    }else if(target_type=="enemy_weak") {//敵の最低戦闘力の武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*min_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="enemy_attack_weak") {//敵の最低戦闘力の攻撃武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isAttacker()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*min_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="enemy_defense_weak") {//敵の最低戦闘力の防御武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->isDefender()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*min_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="own_weak") {//味方の最低戦闘力の武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*min_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="own_attack_weak") {//味方の最低戦闘力の攻撃武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->isAttacker()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*min_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="own_defense_weak") {//味方の最低戦闘力の防御武将一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->isDefender()) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(*min_element(target_chara_list.begin(), target_chara_list.end(), [](Chara *a, Chara *b){
                return a->getValue() < b->getValue();
            }));
        }
        
    }else if(target_type=="enemy_support_one") {//敵軍師ランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 0 && chara->getFieldIndex() <= 1) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="own_support_one") {//味方軍師ランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 0 && chara->getFieldIndex() <= 1) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)target_chara_list.size()-1)(dung->rand_engine)));
        }
        
    }else if(target_type=="forward_support") {//前方軍師
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index){
                if(chara->getFieldIndex()==1 && used_chara->getFieldIndex()>=2 && used_chara->getFieldIndex()<=4){
                    target_charas.push_back(chara);
                }
                if(chara->getFieldIndex()==0 && used_chara->getFieldIndex()>=5 && used_chara->getFieldIndex()<=7){
                    target_charas.push_back(chara);
                }
                if(chara->getFieldIndex()==1 && used_chara->getFieldIndex()==0){
                    target_charas.push_back(chara);
                }
                if(chara->getFieldIndex()==0 && used_chara->getFieldIndex()==1){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="enemy_support") {//敵軍師全体
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 0 && chara->getFieldIndex() <= 1) {
                target_charas.push_back(chara);
            }
        }
    
    }else if(target_type=="own_support") {//味方軍師全体
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 0 && chara->getFieldIndex() <= 1) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_support") {//軍師全体
        for(Chara *chara : all_charas){
            if(chara->getFieldIndex() >= 0 && chara->getFieldIndex() <= 1) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="support_forward_own") {//前方味方武将３体（軍師用）
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index){
                if(used_chara->getFieldIndex() == 0 && chara->getFieldIndex()>=2 && chara->getFieldIndex()<=4){
                    target_charas.push_back(chara);
                }
                if(used_chara->getFieldIndex() == 1 && chara->getFieldIndex()>=5 && chara->getFieldIndex()<=7){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="support_forward_enemy") {//前方敵武将３体（軍師用）
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index){
                if(used_chara->getFieldIndex() == 0 && chara->getFieldIndex()>=5 && chara->getFieldIndex()<=7){
                    target_charas.push_back(chara);
                }
                if(used_chara->getFieldIndex() == 1 && chara->getFieldIndex()>=2 && chara->getFieldIndex()<=4){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="back_support") {//後方の味方軍師
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index){
                if(used_chara->getFieldIndex()>=2 && used_chara->getFieldIndex()<=4 && chara->getFieldIndex() == 0){
                    target_charas.push_back(chara);
                }else if(used_chara->getFieldIndex()>=5 && used_chara->getFieldIndex()<=7 && chara->getFieldIndex() == 1){
                    target_charas.push_back(chara);
                }
            }
        }
        
    }else if(target_type=="own_cards"){//自分のすべての武将（手札デッキ含む）
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttackAndDefense()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_cards_attack"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttacker()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_cards_defense"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isDefender()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_cards_under_SR"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttackAndDefense() && (chara->getRare()==1 || chara->getRare()==2 || chara->getRare()==3)){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_deck"){//自分のデッキ内のすべての武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttackAndDefense() && chara->isInDeck()){
                target_charas.push_back(chara);
            }
        }
     
    }else if(target_type=="own_deck_attack"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttacker() && chara->isInDeck()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_deck_defense"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isDefender() && chara->isInDeck()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_hand"){//自分の手札内のすべての武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttackAndDefense() && chara->isInHand()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_hand_attack"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isAttacker() && chara->isInHand()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_hand_defense"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isDefender() && chara->isInHand()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_all_field"){//敵全体（場に出ている武将・軍師）
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->isOnField()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_all_field"){//味方全体（場に出ている武将・軍師）
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->isOnField()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_field"){//全体（場に出ている武将・軍師）
        for(Chara *chara : all_charas){
            if(chara->isOnField()){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_all_area"){//敵全体（場に出ている武将・軍師・総大将）
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && (chara->isOnField() || chara->getType()==CHARA_TYPE_LEADER)){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_all_area"){//味方全体（場に出ている武将・軍師・総大将）
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && (chara->isOnField() || chara->getType()==CHARA_TYPE_LEADER)){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_area"){//全体（場に出ている武将・軍師・総大将）
        for(Chara *chara : all_charas){
            if(chara->isOnField() || chara->getType()==CHARA_TYPE_LEADER){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="enemy_leader"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getType()==CHARA_TYPE_LEADER){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="own_leader"){
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getType()==CHARA_TYPE_LEADER){
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_leader"){
        for(Chara *chara : all_charas){
            if(chara->getType()==CHARA_TYPE_LEADER){
                target_charas.push_back(chara);
            }
        }
    
    }else if(target_type=="value_less_enemy") {//自分の戦闘力以下の敵武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getValue() <= used_chara->getValue()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_less_enemy_attack") {//自分の戦闘力以下の敵攻撃武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getValue() <= used_chara->getValue() && chara->isAttacker()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_less_enemy_defense") {//自分の戦闘力以下の敵防御武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getValue() <= used_chara->getValue() && chara->isDefender()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_more_enemy") {//自分の戦闘力以上の敵武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getValue() >= used_chara->getValue()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_more_enemy_attack") {//自分の戦闘力以上の敵攻撃武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getValue() >= used_chara->getValue() && chara->isAttacker()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_more_enemy_defense") {//自分の戦闘力以上の敵防御武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == enemy_index && chara->getFieldIndex() >= 2 && chara->getValue() >= used_chara->getValue() && chara->isDefender()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_less_own") {//自分の戦闘力以下の味方武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getValue() <= used_chara->getValue()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_less_own_attack") {//自分の戦闘力以下の味方攻撃武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getValue() <= used_chara->getValue() && chara->isAttacker()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_less_own_defense") {//自分の戦闘力以下の味方防御武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getValue() <= used_chara->getValue() && chara->isDefender()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_more_own") {//自分の戦闘力以上の味方武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getValue() >= used_chara->getValue()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_more_own_attack") {//自分の戦闘力以上の味方攻撃武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getValue() >= used_chara->getValue() && chara->isAttacker()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="value_more_own_defense") {//自分の戦闘力以上の味方防御武将
        for(Chara *chara : all_charas){
            if(chara->getBelongingIndex() == used_index && chara->getFieldIndex() >= 2 && chara->getValue() >= used_chara->getValue() && chara->isDefender()) {
                target_charas.push_back(chara);
            }
        }
        
    }else if(target_type=="all_one"){//敵と味方の武将の中からランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getFieldIndex() >= 2) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)(target_chara_list.size()-1))(dung->rand_engine)));
        }
        
    }else if(target_type=="all_field_one"){//敵と味方の武将と軍師の中からランダム一体
        vector<Chara *> target_chara_list;
        for(Chara *chara : all_charas){
            if(chara->getFieldIndex() >= 0) {
                target_chara_list.push_back(chara);
            }
        }
        if(target_chara_list.size()>0) {
            target_charas.push_back(target_chara_list.at(std::uniform_int_distribution<int>(0, (int)(target_chara_list.size()-1))(dung->rand_engine)));
        }
    }
    
    
    else{//今は使わない
        CCLOG("ターゲットタイプエラー！！！");
        target_charas = used_chara->getAroundFieldChara(target_type);
    }
    
    return target_charas;
}

Sprite* ActiveSkill::getIcon(int buff_id){
	//スキルパターンから使用アイコン判別
    switch(buff_id){
        case 1:
		case 2:
		case 3://戦闘力UP
        case 33:
        case 61:
        case 1005:
            return Sprite::create("download/Chara/icon/icon01.png");
		case 4:
		case 5:
		case 6://戦闘力DOWN
        case 34:
        case 62:
        case 1006:
			return Sprite::create("download/Chara/icon/icon02.png");
		case 19://貫通
			return Sprite::create("download/Chara/icon/icon05.png");
		case 22://撹乱
			return Sprite::create("download/Chara/icon/icon19.png");
		case 25://直接攻撃ダメUP
			return Sprite::create("download/Chara/icon/icon18.png");
		case 26://味方犠牲:吸収
			return Sprite::create("download/Chara/icon/icon16.png");
		case 31://誘惑
        case 1003:
			return Sprite::create("download/Chara/icon/icon17.png");
        case 36://鉄壁
            return Sprite::create("download/Chara/icon/icon21.png");
        case 35://戦闘力固定
            return Sprite::create("download/Chara/icon/icon30.png");
        case 41://前方一ターン後破壊
            return Sprite::create("download/Chara/icon/icon33.png");
        case 42://コンボ効果アップ（パッシブ用）
            return Sprite::create("download/Chara/icon/icon12.png");
        case 44://置いたターンのカードを置く士気をマイナス（パッシブ用）
            return Sprite::create("download/Chara/icon/icon40.png");
        case 46://攻撃による総大将へのダメージを減少させる
            return Sprite::create("download/Chara/icon/icon35.png");
        case 47://手札公開
            return Sprite::create("download/Chara/icon/icon42.png");
        case 58://根性
            return Sprite::create("download/Chara/icon/icon47.png");
        case 60://上昇する士気固定
            return Sprite::create("download/Chara/icon/icon49.png");
        case 63://手札引く数制限
            return Sprite::create("download/Chara/icon/icon50.png");
        case 64:////相手はXコストのカードを出すことができない
            return Sprite::create("download/Chara/icon/icon59.png");
        case 71://行動不能無効
            return Sprite::create("download/Chara/icon/icon36.png");
        case 72://除去無効を付与
            return Sprite::create("download/Chara/icon/icon11.png");
        case 73://２回攻撃を付与
            return Sprite::create("download/Chara/icon/icon45.png");
        case 84://低下無効を付与
            return Sprite::create("download/Chara/icon/icon61.png");
        case 85://死んでも行動不能を付与
            return Sprite::create("download/Chara/icon/icon38.png");
        case 86://ためるバフ
            return Sprite::create("download/Chara/icon/icon69.png");
        case 90://スキル封印耐性
            return Sprite::create("download/Chara/icon/icon71.png");
    }
	
    return nullptr;//なし
}

//カウンターパッシブのかかったキャラ・
string ActiveSkill::counterPassiveLogic(Chara* used_chara,int skill_id,int skill_value, Chara *atk_chara){
    
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    string counter_data = "";
    string target_type = used_chara->getPassiveSkillTarget();//パッシブ持っているキャラのターゲットタイプを持ってくる
    vector<Chara *> target_charas;
    data_string = "";	//演出データテキスト
    data_string += to_string(used_chara->getPrimaryId()) + "," + to_string(skill_id);
    
    auto removeGuardRemove = [](vector<Chara *> targets)->vector<Chara *> {//キャラのHMSベクターから除去無効持ちを排除するラムダ・参照渡し
        auto itr = targets.begin();
        while (itr != targets.end()) {
            if((*itr)->statusCheck(STATUS_TYPE_REMOVE_GUARD)){//除去無効持ちはターゲットしない
                targets.erase(itr);
            }else{
                itr++;
            }
        }
        return targets;
    };

    switch(skill_id){
        case 12:{//武将戦闘力UP
            target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_type);
            
            for(int i=0; i<target_charas.size(); i++){
                if(target_charas.at(i)->getPrimaryId() == used_chara->getPrimaryId()) continue;//自分にはかけない
                
                map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                temp.insert(make_pair("primary", used_chara->getPrimaryId()));
                temp.insert(make_pair("skill_id", 33));
                temp.insert(make_pair("value", skill_value));
                temp.insert(make_pair("turn", 2*2));
                target_charas.at(i)->chara_status_active.push_back(temp);
                
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), skill_value);
            }
            //counter_data = skillLogic(used_chara, 1005, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 13:{//武将戦闘力DOWN
            if(target_type == "attacker" && atk_chara!=nullptr){
                target_charas.push_back(atk_chara);
            }else{
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_type);
            }
            
            for(int i=0; i<target_charas.size(); i++){
                
                map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                temp.insert(make_pair("primary", used_chara->getPrimaryId()));
                temp.insert(make_pair("skill_id", 34));
                temp.insert(make_pair("value", skill_value));
                temp.insert(make_pair("turn", 2*2));
                target_charas.at(i)->chara_status_active.push_back(temp);
                
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), skill_value);
            }
            //counter_data = skillLogic(used_chara, 1006, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 14:{//対象の武将を除去する
            if(target_type == "attacker" && atk_chara!=nullptr){
                target_charas.push_back(atk_chara);
            }else{
                target_charas = getTargetCharas(used_chara, removeGuardRemove(CharaList::getAll().getVector()), target_type);
            }
            
            if(target_charas.empty()==false){
                for(int i=0; i<target_charas.size(); i++){
                    target_charas.at(i)->setDeadFlg(true);
    
                    //スキル対象キャラ情報データ作成
                    data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), 1);
                }
            }
            //counter_data = skillLogic(used_chara, 1001, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 15:{//敵プレイヤーにダメージ
            int damage = dung->player[used_chara->reverseIndex(used_chara->getBelongingIndex())]->directDamage(skill_value);
            
            //スキル対象キャラ情報データ作成※対象が相手プレイヤーの為、0指定
            data_string += dataStringTargetChara(0, damage);
            
            //counter_data = skillLogic(used_chara, 1004, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 40:{//対象の武将を行動不能
            if(target_type == "attacker" && atk_chara!=nullptr){
                target_charas.push_back(atk_chara);
            }else{
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_type);
            }
            
            for(int i=0; i<target_charas.size(); i++){
                
                map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                temp.insert(make_pair("primary", used_chara->getPrimaryId()));
                temp.insert(make_pair("skill_id", 20));
                temp.insert(make_pair("value", skill_value));
                temp.insert(make_pair("turn", 2*2));
                temp.insert(make_pair("state", STATUS_TYPE_CANT_ACTION));
                target_charas.at(i)->chara_status_active.push_back(temp);
                
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), skill_value);
            }
            
            //counter_data = skillLogic(used_chara, 1002, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 41:{//対象の武将を誘惑
            if(target_type == "attacker" && atk_chara!=nullptr){
                target_charas.push_back(atk_chara);
            }else{
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_type);
            }
            
            for(int i=0; i<target_charas.size(); i++){
                
                map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                temp.insert(make_pair("primary", used_chara->getPrimaryId()));
                temp.insert(make_pair("skill_id", 31));
                temp.insert(make_pair("value", skill_value));
                temp.insert(make_pair("turn", 2*2));
                temp.insert(make_pair("state", STATUS_TYPE_CHARM));
                target_charas.at(i)->chara_status_active.push_back(temp);
                
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), skill_value);
            }
            
            //counter_data = skillLogic(used_chara, 1003, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 56:{//士気回復
            dung->player[used_chara->getBelongingIndex()]->generatePower(skill_value);
            //counter_data = skillLogic(used_chara, 1007, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 63:{//総大将回復
            int heal_value = dung->player[used_chara->getBelongingIndex()]->heal(skill_value);
            
            //スキル対象キャラ情報データ作成※対象が相手プレイヤーの為、0指定
            data_string += dataStringTargetChara(0, heal_value);
            //counter_data = skillLogic(used_chara, 1008, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 69:{//戦闘力の低いキャラ除去
            vector<Chara *> targets = CharaList::getOneSide(used_chara->reverseIndex(used_chara->getBelongingIndex())).getVector();//敵側のキャラから
            removeGuardRemove(targets);//除去無効キャラを取り除く
            
            Chara *temp_chara = nullptr;
            if(target_type == "attack"){
                for(Chara *chara : targets){
                    if(chara->isAttacker() && chara->isOnField() && chara->isAlive()){
                        if(temp_chara == nullptr || temp_chara->getValue() > chara->getValue()){
                            temp_chara = chara;
                        }
                    }
                }
            }else if(target_type == "defense"){
                for(Chara *chara : targets){
                    if(chara->isDefender() && chara->isOnField() && chara->isAlive()){
                        if(temp_chara == nullptr || temp_chara->getValue() > chara->getValue()){
                            temp_chara = chara;
                        }
                    }
                }
            }else{
                for(Chara *chara : targets){
                    if(chara->isAttackAndDefense() && chara->isOnField() && chara->isAlive()){
                        if(temp_chara == nullptr || temp_chara->getValue() > chara->getValue()){
                            temp_chara = chara;
                        }
                    }
                }
            }
            
            if(temp_chara != nullptr){
                temp_chara->setDeadFlg(true);
                
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(temp_chara->getPrimaryId(), 1);
            }
            //counter_data = skillLogic(used_chara, 1009, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 89:{//特殊召喚
            Chara *summon_chara = nullptr;
            for(Chara *chara : CharaList::getOneSide(used_chara->getBelongingIndex())){
                if(chara->isInHand() && (chara->isAttacker() || chara->isDefender()) && chara->getPlaceCost() == skill_value){
                    if(summon_chara == nullptr){
                        summon_chara = chara;
                    }else{
                        if(summon_chara->getHandIndex() > chara->getHandIndex()) summon_chara = chara;
                    }
                }
            }
            if(summon_chara){
                //キャラにデータを設定する
                summon_chara->setFieldIndex(used_chara->getFieldIndex());
                summon_chara->hand_index = -1;//手札インデックスのリセット
                
                //スキル対象キャラ情報データ作成※対象が相手プレイヤーの為、0指定
                data_string += dataStringTargetChara(summon_chara->getPrimaryId(), used_chara->getPassiveSkillValue());
            }
            //counter_data = skillLogic(used_chara, 1010, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
        case 92:{//氷漬け
            if(target_type == "attacker" && atk_chara!=nullptr){
                target_charas.push_back(atk_chara);
            }else{
                target_charas = getTargetCharas(used_chara, CharaList::getAll().getVector(), target_type);
            }
            
            for(int i=0; i<target_charas.size(); i++){
                
                map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                temp.insert(make_pair("primary", used_chara->getPrimaryId()));
                temp.insert(make_pair("skill_id", used_chara->getActiveSkillId()));
                temp.insert(make_pair("value", skill_value));
                temp.insert(make_pair("turn", 9999));
                temp.insert(make_pair("state", STATUS_TYPE_FREEZE));
                target_charas.at(i)->chara_status_active.push_back(temp);
                
                if(use_skill_data=="") use_skill_data = to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(skill_value);
                else use_skill_data += "," + to_string(target_charas.at(i)->getPrimaryId()) + "&" + to_string(skill_value);
                
                //スキル対象キャラ情報データ作成
                data_string += dataStringTargetChara(target_charas.at(i)->getPrimaryId(), skill_value);
            }
            //counter_data = skillLogic(used_chara, 1011, skill_value, used_chara->getBelongingIndex(), target_type, "");
            break;
        }
    }
    
    //use_skill_data += "##" + data_string;
    //HmsVector<string> return_string = split(counter_data, "##");//##はいらない
    //HMSLOG("カウンターデータ%s",return_string.at(1).c_str());
    return data_string;
}
//バフデバフを持っているか判定
bool ActiveSkill::buffDebuffCheck(Chara* target_chara, int type)
{
	for(int n=0; n<target_chara->chara_status_active.size(); n++){
		if(target_chara->chara_status_active.at(n)["buffer_type"] == type) {
			return true;
		}
	}
	return false;
}
//対象のバフデバフを消す
void ActiveSkill::buffDebuffErase(Chara* target_chara, int type)
{
    
    while (true) {
        bool delete_flg = false;
        for(int n=0; n<target_chara->chara_status_active.size(); n++){//対象にかかっているスキルを全てチェック
            if(target_chara->chara_status_active.at(n)["buffer_type"] == type) {//特定のスキル効果なのか判断
                target_chara->chara_status_active.erase(target_chara->chara_status_active.begin()+n);//スキル効果消去
                delete_flg = true;
                break;
            }
        }
        if(delete_flg == true) continue;//一つ消したらもう一度最初から
        break;
    }
	
}
