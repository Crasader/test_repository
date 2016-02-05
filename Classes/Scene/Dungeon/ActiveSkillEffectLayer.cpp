/*アクティブスキルのエフェクトを書くレイヤー*/
#include "ActiveSkillEffectLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "Effect.h"
#include "SimpleAudioEngine.h"
#include "SkillCutInLayer.h"
#include "ComboEffectLayer2.h"
#include "TurnManage.h"
#include "CharaList.h"

bool ActiveSkillEffectLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}
   
	return true;
}

void ActiveSkillEffectLayer::setSkillEffectData(string data_string){
	HMSLOG("データストリング　%s",data_string.c_str());
	
	//初期化
	skill_val.clear();
    //skill_value2.clear();
	target_chara.clear();
	finish_flg = false;
	effect_count = 1;
	//bool effect_onoff_flg = true;
	ratio = getVisibleRatio();
    
	//オプションでエフェクトの表示有無を取得
	//bool effect_onoff_flg = SaveData::loadBool("effect");
	
	//スキル使用キャラとスキル効果値を保存
	HmsVector<int> target_primary_id;
	HmsVector<string> data_list = split(data_string, ",");
	
    
    use_chara = CharaList::getAtPrimary(stoi(data_list.at(0)));

	
	skill_effect_num = stoi(data_list.at(1).c_str());
	
	for(int i=2; i<data_list.size(); i++){
		HmsVector<string> mini_data_list = split(data_list.at(i), "&");
		target_primary_id.insert(stoi(mini_data_list.at(0)));
        vector<int> temp;
		temp.push_back(stoi(mini_data_list.at(1)));
        if(mini_data_list.size()>2) temp.push_back(stoi(mini_data_list.at(2)));
        if(mini_data_list.size()>3) temp.push_back(stoi(mini_data_list.at(3)));
        
        skill_val.push_back(temp);
    }
	
	//対象キャラ保存
	for(int i=0; i<target_primary_id.size(); i++){
        target_chara.push_back(CharaList::getAtPrimary(target_primary_id.at(i)));
	}
	
	//カットイン生成
	SkillCutInLayer* skill_cut_in_layer = SkillCutInLayer::create();
	float skill_cut_in_time = 1.5f;//カットイン再生時間
    
    skill_cut_in_layer->setData(use_chara->getCharaId(), use_chara->getActiveSkillName(), skill_cut_in_time);
    this->runAction(Sequence::create(DelayTime::create(skill_cut_in_time),														//カットイン再生分の遅延処理
                                     CallFunc::create([&](){static_cast<DungeonScene*>(getParent())->addPowerEffect(0.0f);}),	//消費士気の更新演出
                                     CallFunc::create([this](){
        this->skillEffect();//発動スキルの演出ロジック
    }), nullptr));
    
    
    static_cast<DungeonScene*>(getParent())->dialogLayer->addChild(skill_cut_in_layer, 100);
    
    
}

void ActiveSkillEffectLayer::skillEffect(){
	HMSLOG("ActiveSkillEffectLayer::skillEffect [%d] count: %d", skill_effect_num, effect_count);
	
    //オプションでエフェクトの表示有無を取得
    bool effect_onoff_flg = SaveData::loadBool("effect");
    
	float delay_time = 0.0f;
	int used_index = use_chara->getBelongingIndex();
	int enemy_index = 1;
	if(used_index > 0) enemy_index = 0;
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
	//スキルごとの演出
	switch(skill_effect_num) {
		case 1:	//戦闘力UP：自分の戦闘力を？？％UP系
		case 2:	//戦闘力UP：味方武将の数＊？？％UP系
		case 3://戦闘力UP：敵武将の数＊？？％UP系
        case 33://戦闘力UP：自分の戦闘力を？？固定値UP系
        case 35://戦闘力UP：自分の戦闘力を？？にする
        case 36://鉄壁
        case 49://軍師のターン数増やす
        case 61:
        case 66:
        case 67:{
            if(effect_onoff_flg) {
                switch(effect_count) {
                    case 1:{
                        for(int i=0; i<target_chara.size(); i++) {
                            if(target_chara.at(i)->isInDeck()){
                                //なし
                                
                            }else if(skill_val.at(i).at(0) > 0) {//状態異常成功の時
                                //パーティクル出力
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(target_chara.at(i)->getPosition());
                                kiryoku->setScale(ratio);
                                this->addChild(kiryoku, 1);
                                
                                ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                                kiryoku_plus->setAutoRemoveOnFinish(true);
                                kiryoku_plus->setPosition(target_chara.at(i)->getPosition());
                                kiryoku_plus->setScale(ratio);
                                this->addChild(kiryoku_plus, 1);
                            } else {//状態異常失敗の時
                                //missエフェクト
                                missEffect(target_chara.at(i)->getPosition());
                                //スキル失敗SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }
                        }
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
						delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
		case 4:	//戦闘力DOWN：戦闘力＊？？％DOWN系
		case 5:	//戦闘力DOWN：味方武将の数＊？？％DOWN系
		case 6://戦闘力DOWN：敵武将の数＊？？％DOWN系
        case 34://戦闘力DOWN：自分の戦闘力を？？固定値DOWN系
        case 48://軍師のターン数を減らす
        case 52://対象と同じカードの配置士気を上げる
        case 62:{
			if(effect_onoff_flg) {
				switch(effect_count) {
					case 1:{
						for(int i=0; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* status_down = ParticleSystemQuad::create("download/Effect/Particle/status_down.plist");
							status_down->setAutoRemoveOnFinish(true);
							status_down->setPosition(target_chara.at(i)->getPosition());
							status_down->setScale(ratio);
							this->addChild(status_down, 1);
							
							ParticleSystemQuad* status_down_plus = ParticleSystemQuad::create("download/Effect/Particle/status_down_plus.plist");
							status_down_plus->setAutoRemoveOnFinish(true);
							status_down_plus->setPosition(target_chara.at(i)->getPosition());
							status_down_plus->setScale(ratio);
							this->addChild(status_down_plus, 1);
						}
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/status_down.mp3");
						delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: EnemyCharaPowerDownType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
		case 7:{//プレイヤーダメージ：敵プレイヤーに？？ダメージ
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						//パーティクル出力
						ParticleSystemQuad* zan1 = ParticleSystemQuad::create("download/Effect/Particle/zan1.plist");
						zan1->setAutoRemoveOnFinish(true);
                        zan1->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan1->setScale(ratio);
						this->addChild(zan1, 1);
						
						ParticleSystemQuad* zan2 = ParticleSystemQuad::create("download/Effect/Particle/zan2.plist");
						zan2->setAutoRemoveOnFinish(true);
						zan2->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan2->setScale(ratio);
						this->addChild(zan2, 1);
						
						ParticleSystemQuad* zan3 = ParticleSystemQuad::create("download/Effect/Particle/zan3.plist");
						zan3->setAutoRemoveOnFinish(true);
						zan3->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan3->setScale(ratio);
						this->addChild(zan3, 1);
						
						ParticleSystemQuad* zan4 = ParticleSystemQuad::create("download/Effect/Particle/zan4.plist");
						zan4->setAutoRemoveOnFinish(true);
						zan4->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan4->setScale(ratio);
						this->addChild(zan4, 1);
                    
                        if(skill_val.size() >= 2){
                            //パーティクル出力
                            ParticleSystemQuad* zan1 = ParticleSystemQuad::create("download/Effect/Particle/zan1.plist");
                            zan1->setAutoRemoveOnFinish(true);
                            zan1->setPosition(CharaList::getLeader(used_index)->getPosition());
                            zan1->setScale(ratio);
                            this->addChild(zan1, 1);
                            
                            ParticleSystemQuad* zan2 = ParticleSystemQuad::create("download/Effect/Particle/zan2.plist");
                            zan2->setAutoRemoveOnFinish(true);
                            zan2->setPosition(CharaList::getLeader(used_index)->getPosition());
                            zan2->setScale(ratio);
                            this->addChild(zan2, 1);
                            
                            ParticleSystemQuad* zan3 = ParticleSystemQuad::create("download/Effect/Particle/zan3.plist");
                            zan3->setAutoRemoveOnFinish(true);
                            zan3->setPosition(CharaList::getLeader(used_index)->getPosition());
                            zan3->setScale(ratio);
                            this->addChild(zan3, 1);
                            
                            ParticleSystemQuad* zan4 = ParticleSystemQuad::create("download/Effect/Particle/zan4.plist");
                            zan4->setAutoRemoveOnFinish(true);
                            zan4->setPosition(CharaList::getLeader(used_index)->getPosition());
                            zan4->setScale(ratio);
                            this->addChild(zan4, 1);
                        }
                        
                        
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/player_damage.mp3");
						delay_time = 0.9f;//演出終了までの時間
					}
					break;
				}
				case 2:{
					dung->playerHpUpdate(enemy_index, skill_val.at(0).at(0), dung->player[enemy_index]->getHp());
                    if(skill_val.size() >= 2){
                        dung->playerHpUpdate(used_index, skill_val.at(1).at(0), dung->player[used_index]->getHp());
                    }
                    
					delay_time = 1.0f;//演出終了までの時間
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
			}
			break;
		}
		case 8:	//HP回復：固定値回復
        case 9:{//HP回復：？？％回復
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						//パーティクル出力
						ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/kaihuku.plist");
						kaihuku->setAutoRemoveOnFinish(true);
						kaihuku->setPosition(CharaList::getLeader(used_index)->getPosition());
						kaihuku->setScale(ratio);
						this->addChild(kaihuku, 1);
						
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku.mp3");
						delay_time = 1.3f;//演出終了までの時間
					}
					break;
				}
				case 2:{
					int skill_recovery = skill_val.at(0).at(0);
					dung->playerHpUpdate(used_index, -1 * skill_recovery,dung->player[used_index]->getHp());
					delay_time = 1.0f;//演出終了までの時間
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
			}
			break;
		}
		case 10: //除去：最も戦闘力の高い敵武将1体除去
		case 11: //除去：自分より戦闘力の低い敵武将を除去する系
		case 12: //除去：敵軍師を除去する系
		case 13: //除去：戦闘力が？？以下の敵武将を除去する系
		case 14:
        case 39: //除去：自分より戦闘力の高い敵武将を除去する系
        case 40: //除去：戦闘力が？？以上の敵武将を除去する系
        case 51://行動不能除去
        case 57://対象の中で配置士気がX以下の武将を除去する
        case 68://コンボカラーXが含まれていない除去
        {
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						for(int i=0; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* jokyo = ParticleSystemQuad::create("download/Effect/Particle/jokyo.plist");
							jokyo->setAutoRemoveOnFinish(true);
							jokyo->setPosition(target_chara.at(i)->getPosition());
							jokyo->setScale(ratio);
							this->addChild(jokyo, 1);
						}
						
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo.mp3");
						delay_time = 1.0f;//演出終了までの時間
					}
					break;
				}
				case 2:{
					for(int i=0; i<target_chara.size(); i++) {
						if(skill_val.at(i).at(0)>0) {//除去成功の時
                            CCLOG("てすと%d",skill_val.at(i).at(0));
							if(effect_onoff_flg) {
								//パーティクル出力
								ParticleSystemQuad* jokyo_success = ParticleSystemQuad::create("download/Effect/Particle/jokyo_success.plist");
								jokyo_success->setAutoRemoveOnFinish(true);
								jokyo_success->setPosition(target_chara.at(i)->getPosition());
								jokyo_success->setScale(ratio);
								this->addChild(jokyo_success, 1);
								
								//SE
								CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo_success.mp3");
								delay_time = 1.0f;//演出終了までの時間
							}
						} else {//除去失敗の時
							//missエフェクト
							missEffect(target_chara.at(i)->getPosition());
							delay_time = 1.0f;//演出終了までの時間
						}
					}
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: SkillDeadType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
			}
			break;
		}
		case 15: //犠牲：自分を犠牲にして、敵武将1体を除去する
		case 16: //犠牲：自分を犠牲にして、最も戦闘力の高い敵武将を除去する
		case 17:{//犠牲：自分を犠牲にして、前方の敵武将を1体を除去する
			if(effect_onoff_flg) {
				//パーティクル出力
				ParticleSystemQuad* gisei1 = ParticleSystemQuad::create("download/Effect/Particle/gisei.plist");
				gisei1->setAutoRemoveOnFinish(true);
				gisei1->setPosition(use_chara->getPosition());
				gisei1->setScale(ratio);
				this->addChild(gisei1, 1);
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/gisei.mp3");
				
				skill_effect_num = 10;//除去の演出番号に変更
				effect_count--;
				
				delay_time = 1.3f;//演出終了までの時間
			} else {
				skill_effect_num = 10;//除去の演出番号に変更
				effect_count--;
				delay_time = 0.0f;//演出終了までの時間
			}
			break;
		}
		/*case 18:{//犠牲：自分を犠牲にして、相手プレイヤーに？？ダメージ与える
			if(effect_onoff_flg) {
				//パーティクル出力
				ParticleSystemQuad* gisei2 = ParticleSystemQuad::create("download/Effect/Particle/gisei2.plist");
				gisei2->setAutoRemoveOnFinish(true);
				gisei2->setPosition(use_chara->getPosition());
				gisei2->setScale(ratio);
				this->addChild(gisei2, 1);
				
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/gisei2.mp3");
				
				skill_effect_num = 7;//プレイヤーダメージの演出番号に変更
				effect_count--;
				
				delay_time = 1.3f;//演出終了までの時間
			} else {
				skill_effect_num = 7;//プレイヤーダメージの演出番号に変更
				effect_count--;
			}
			break;
		}*/
		case 19:{//貫通：武将に貫通効果をつける系
			if(effect_onoff_flg) {
				switch(effect_count) {
					case 1:{
						for(int i=0; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* kantu = ParticleSystemQuad::create("download/Effect/Particle/kantu.plist");
							kantu->setAutoRemoveOnFinish(true);
							kantu->setPosition(target_chara.at(i)->getPosition());
							kantu->setScale(ratio);
							this->addChild(kantu, 1);
						}
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kantu.mp3");
						delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
        case 20:{//ジャマー：敵武将を行動不能にする系
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						for(int i=0; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
							hunou->setAutoRemoveOnFinish(true);
							hunou->setPosition(target_chara.at(i)->getPosition());
							hunou->setScale(ratio);
							this->addChild(hunou, 1);
						}
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/hunou.mp3");
						delay_time = 1.0f;//演出終了までの時間
						break;
					}
				case 2:{
					for(int i=0; i<target_chara.size(); i++) {
						if(skill_val.at(i).at(0)>0) {//状態異常成功の時
							if(effect_onoff_flg) {
								//パーティクル出力
								ParticleSystemQuad* hunou_success = ParticleSystemQuad::create("download/Effect/Particle/hunou_success.plist");
								hunou_success->setAutoRemoveOnFinish(true);
								hunou_success->setPosition(target_chara.at(i)->getPosition());
								hunou_success->setScale(ratio);
								this->addChild(hunou_success, 0);
								//スキル成功SE
								CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/success.mp3");
								delay_time = 0.8f;//演出終了までの時間
							}
						} else {//状態異常失敗の時
							//missエフェクト
							missEffect(target_chara.at(i)->getPosition());
							//スキル失敗SE
							CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
							delay_time = 0.6f;//演出終了までの時間
						}
					}
					break;
				}
                case 3:{
                    for(int i=0; i<target_chara.size(); i++) {
                        target_chara.at(i)->showCardParam();
                    }
                    delay_time = 0.1f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                    
				default:
					HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
				}
			}
			break;
		}
		case 21:{//ジャマー：敵武将のアクティブスキルを封印する系
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						for(int i=0; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* huin = ParticleSystemQuad::create("download/Effect/Particle/huin.plist");
							huin->setAutoRemoveOnFinish(true);
							huin->setPosition(target_chara.at(i)->getPosition());
							huin->setScale(ratio);
							this->addChild(huin, 1);
						}
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/huin.mp3");
						delay_time = 1.3f;//演出終了までの時間
						break;
					}
				case 2:{
					for(int i=0; i<target_chara.size(); i++) {
						if(skill_val.at(i).at(0)>0) {//状態異常成功の時
							if(effect_onoff_flg) {
								//パーティクル出力
								ParticleSystemQuad* huin_success = ParticleSystemQuad::create("download/Effect/Particle/huin_success.plist");
								huin_success->setAutoRemoveOnFinish(true);
								huin_success->setPosition(target_chara.at(i)->getPosition());
								huin_success->setScale(ratio);
								this->addChild(huin_success, 0);
								//スキル成功SE
								CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/success.mp3");
								delay_time = 1.0f;//演出終了までの時間
							}
						} else {//状態異常失敗の時
							//missエフェクト
							missEffect(target_chara.at(i)->getPosition());
							//スキル失敗SE
							CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
							delay_time = 1.0f;//演出終了までの時間
						}
					}
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
				}
			}
			break;
		}
		case 22:{//撹乱：フィールドにいる武将の戦闘力を反転する
            if(effect_onoff_flg) {
                switch(effect_count) {
                    case 1:{
                        for(int i=0; i<target_chara.size(); i++) {
                            if(skill_val.at(i).at(0)>0) {//状態異常成功の時
                                //パーティクル出力
                                ParticleSystemQuad* question = ParticleSystemQuad::create("download/Effect/Particle/question.plist");
                                question->setAutoRemoveOnFinish(true);
                                question->setPosition(target_chara.at(i)->getPosition());
                                question->setScale(ratio);
                                this->addChild(question, 1);
                            } else {//状態異常失敗の時
                                //missエフェクト
                                missEffect(target_chara.at(i)->getPosition());
                                //スキル失敗SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/question.mp3");
                        delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
		case 23:{//バフ解除
			if(effect_onoff_flg) {
				switch(effect_count) {
					case 1:{
						for(int i=0; i<target_chara.size(); i++) {
                            if(skill_val.at(i).at(0)>0) {//状態異常成功の時
                                //パーティクル出力
                                ParticleSystemQuad* wave = ParticleSystemQuad::create("download/Effect/Particle/wave.plist");
                                wave->setAutoRemoveOnFinish(true);
                                wave->setPosition(target_chara.at(i)->getPosition());
                                wave->setScale(ratio);
                                this->addChild(wave, 1);
                                
                                ParticleSystemQuad* smoke = ParticleSystemQuad::create("download/Effect/Particle/smoke.plist");
                                smoke->setAutoRemoveOnFinish(true);
                                smoke->setPosition(target_chara.at(i)->getPosition());
                                smoke->setScale(ratio);
                                this->addChild(smoke, 1);
                            } else {//状態異常失敗の時
                                //missエフェクト
                                missEffect(target_chara.at(i)->getPosition());
                                //スキル失敗SE
                                //CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }
                        }
                        //SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/remove.mp3");
						delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
		case 24:{//デバフ解除
			if(effect_onoff_flg) {
				switch(effect_count) {
					case 1:{
                        for(int i=0; i<target_chara.size(); i++) {
                            if(skill_val.at(i).at(0)>0) {//状態異常成功の時
                                //パーティクル出力
                                ParticleSystemQuad* heal = ParticleSystemQuad::create("download/Effect/Particle/heal2.plist");
                                heal->setAutoRemoveOnFinish(true);
                                heal->setPosition(target_chara.at(i)->getPosition());
                                heal->setScale(ratio);
                                this->addChild(heal, 1);
                                
                                ParticleSystemQuad* heal_plus = ParticleSystemQuad::create("download/Effect/Particle/heal_plus.plist");
                                heal_plus->setAutoRemoveOnFinish(true);
                                heal_plus->setPosition(target_chara.at(i)->getPosition());
                                heal_plus->setScale(ratio);
                                this->addChild(heal_plus, 1);
                            } else {//状態異常失敗の時
                                //missエフェクト
                                missEffect(target_chara.at(i)->getPosition());
                                //スキル失敗SE
                                //CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }
                            
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/heal.mp3");
                        delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
		case 25:{//直接攻撃力UP系
			if(effect_onoff_flg) {
				switch(effect_count) {
					case 1:{
						for(int i=0; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* energy = ParticleSystemQuad::create("download/Effect/Particle/energy.plist");
							energy->setAutoRemoveOnFinish(true);
							energy->setPosition(target_chara.at(i)->getPosition());
							energy->setScale(ratio);
							this->addChild(energy, 1);
						}
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/energy.mp3");
						delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
		case 26:{//他者犠牲：戦闘力吸収
			if(effect_onoff_flg) {
                switch(effect_count) {
                    case 1:{//犠牲にしたキャラの演出
                        for(int i=0; i<target_chara.size(); i++) {
                            if(skill_val.at(i).at(0)>0) {//状態異常成功の時
                                //パーティクル出力
                                ParticleSystemQuad* circle = ParticleSystemQuad::create("download/Effect/Particle/circle.plist");
                                circle->setAutoRemoveOnFinish(true);
                                circle->setPosition(target_chara.at(i)->getPosition());
                                circle->setScale(ratio);
                                this->addChild(circle, 1);
                                
                                ParticleSystemQuad* death = ParticleSystemQuad::create("download/Effect/Particle/death.plist");
                                death->setAutoRemoveOnFinish(true);
                                death->setPosition(target_chara.at(i)->getPosition());
                                death->setScale(ratio);
                                this->addChild(death, 1);
                            } else {//状態異常失敗の時
                                //missエフェクト
                                missEffect(target_chara.at(i)->getPosition());
                                //スキル失敗SE
                                //CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/death.mp3");
						delay_time = 1.3f;//演出終了までの時間
						break;
					}
					case 2:{//吸収したキャラの演出
						//パーティクル出力
						ParticleSystemQuad* resorb = ParticleSystemQuad::create("download/Effect/Particle/resorb.plist");
						resorb->setAutoRemoveOnFinish(true);
						resorb->setPosition(use_chara->getPosition());
						resorb->setScale(ratio);
						this->addChild(resorb, 1);
						
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/resorb.mp3");
						delay_time = 1.3f;//演出終了までの時間
						finish_flg = true;//最後の演出終了フラグ
						break;
					}
					default:
						HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
						finish_flg = true;//エラーにより演出終了
						break;
				}
			} else {
				finish_flg = true;//最後の演出終了フラグ
			}
			break;
		}
		case 27: //他者犠牲：敵キャラ1体除去
		case 28: //他者犠牲：戦闘力が1番強い敵キャラ除去
		case 29:{//他者犠牲：前方の敵キャラ除去
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						//パーティクル出力
						ParticleSystemQuad* front_jokyo = ParticleSystemQuad::create("download/Effect/Particle/front_jokyo.plist");
						front_jokyo->setAutoRemoveOnFinish(true);
						front_jokyo->setPosition(target_chara.at(0)->getPosition());
						front_jokyo->setScale(ratio);
						this->addChild(front_jokyo, 1);
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/front_jokyo.mp3");
						delay_time = 1.3f;//演出終了までの時間
					}
					break;
				}
				case 2:{
					if(effect_onoff_flg) {
						for(int i=1; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* front_jokyo = ParticleSystemQuad::create("download/Effect/Particle/front_jokyo.plist");
							front_jokyo->setAutoRemoveOnFinish(true);
							front_jokyo->setPosition(target_chara.at(i)->getPosition());
							front_jokyo->setScale(ratio);
							this->addChild(front_jokyo, 1);
						}
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/front_jokyo.mp3");
						delay_time = 1.0f;//演出終了までの時間
					}
					break;
				}
				case 3:{
					for(int i=1; i<target_chara.size(); i++) {
						if(skill_val.at(i).at(0)>0) {//除去成功の時
							if(effect_onoff_flg) {
								//パーティクル出力
								ParticleSystemQuad* front_jokyo_success = ParticleSystemQuad::create("download/Effect/Particle/front_jokyo_success.plist");
								front_jokyo_success->setAutoRemoveOnFinish(true);
								front_jokyo_success->setPosition(target_chara.at(i)->getPosition());
								front_jokyo_success->setScale(ratio);
								this->addChild(front_jokyo_success, 1);
								//スキル成功SE
								CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/success.mp3");
								delay_time = 1.0f;//演出終了までの時間
							}
						} else {//除去失敗の時
							//missエフェクト
							missEffect(target_chara.at(i)->getPosition());
							//スキル失敗SE
							CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
							delay_time = 1.0f;//演出終了までの時間
						}
					}
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: SkillDeadType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
			}
			break;
		}
        case 18:
        case 30:{//他者犠牲：敵総大将に固定ダメージ
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* gisei2 = ParticleSystemQuad::create("download/Effect/Particle/gisei2.plist");
                            gisei2->setAutoRemoveOnFinish(true);
                            gisei2->setPosition(target_chara.at(i)->getPosition());
                            gisei2->setScale(ratio);
                            this->addChild(gisei2, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/gisei2.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
				case 2:{
					if(effect_onoff_flg) {
						//パーティクル出力
						ParticleSystemQuad* zan1 = ParticleSystemQuad::create("download/Effect/Particle/zan1.plist");
						zan1->setAutoRemoveOnFinish(true);
						zan1->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan1->setScale(ratio);
						this->addChild(zan1, 1);
						
						ParticleSystemQuad* zan2 = ParticleSystemQuad::create("download/Effect/Particle/zan2.plist");
						zan2->setAutoRemoveOnFinish(true);
						zan2->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan2->setScale(ratio);
						this->addChild(zan2, 1);
						
						ParticleSystemQuad* zan3 = ParticleSystemQuad::create("download/Effect/Particle/zan3.plist");
						zan3->setAutoRemoveOnFinish(true);
						zan3->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan3->setScale(ratio);
						this->addChild(zan3, 1);
						
						ParticleSystemQuad* zan4 = ParticleSystemQuad::create("download/Effect/Particle/zan4.plist");
						zan4->setAutoRemoveOnFinish(true);
						zan4->setPosition(CharaList::getLeader(enemy_index)->getPosition());
						zan4->setScale(ratio);
						this->addChild(zan4, 1);

						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/player_damage.mp3");
						delay_time = 0.7f;//演出終了までの時間
					}
					break;
				}
				case 3:{
					dung->playerHpUpdate(enemy_index, skill_val.at(0).at(0), dung->player[enemy_index]->getHp());
					delay_time = 1.0f;//演出終了までの時間
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
			}
			break;
		}
		case 31:{//誘惑系
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						for(int i=0; i<target_chara.size(); i++) {
							//パーティクル出力
							ParticleSystemQuad* temptation = ParticleSystemQuad::create("download/Effect/Particle/temptation.plist");
							temptation->setAutoRemoveOnFinish(true);
							temptation->setPosition(target_chara.at(i)->getPosition());
							temptation->setScale(ratio);
							this->addChild(temptation, 1);
						}
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/temptation.mp3");
						delay_time = 1.3f;//演出終了までの時間
					}
					break;
				}
				case 2:{
					for(int i=0; i<target_chara.size(); i++) {
						if(skill_val.at(i).at(0)>0) {//状態異常成功の時
							if(effect_onoff_flg) {
								//パーティクル出力
								ParticleSystemQuad* temptation = ParticleSystemQuad::create("download/Effect/Particle/temptation.plist");
								temptation->setAutoRemoveOnFinish(true);
								temptation->setPosition(target_chara.at(i)->getPosition());
								temptation->setScale(ratio);
								this->addChild(temptation, 1);
								//スキル成功SE
								CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/success.mp3");
								delay_time = 1.0f;//演出終了までの時間
							}
						} else {//状態異常失敗の時
							//missエフェクト
							missEffect(target_chara.at(i)->getPosition());
							//スキル失敗SE
							CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
							delay_time = 1.0f;//演出終了までの時間
						}
					}
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
			}
			break;
		}
		case 32:{//シャッフル：フィールドにいるキャラの配置をランダムに置き換える
			switch(effect_count) {
				case 1:{
					if(effect_onoff_flg) {
						//パーティクル出力
						ParticleSystemQuad* cyclone = ParticleSystemQuad::create("download/Effect/Particle/cyclone.plist");
						cyclone->setAutoRemoveOnFinish(true);
						cyclone->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
						cyclone->setScale(ratio);
						this->addChild(cyclone, 1);
						
						//SE
						CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/cyclone.mp3");
						delay_time = 1.3f;//演出終了までの時間
					}
					break;
				}
				case 2:{
					//新しいフィールドに移動
					for(int i=0; i<target_chara.size(); i++) {
						Point target_position = static_cast<DungeonScene*>(getParent())->fieldLayer->field_position_list[target_chara.at(i)->getBelongingIndex()].at(skill_val.at(i).at(0));
						MoveTo *move = MoveTo::create(0.2f, target_position);
						target_chara.at(i)->runAction(move);
					}
					delay_time = 0.5f;//演出終了までの時間
					break;
				}
				case 3:{
					//移動後のコンビネーションエフェクト
					DungeonScene* dungeon_scene = static_cast<DungeonScene *>(getParent());
					for(int ii=0; ii<2; ii++){
						HmsVector<Chara*> chara_list;
						chara_list.clear();
                        chara_list = CharaList::getOneSide(ii);
						dungeon_scene->combination_dataL = "";
						dungeon_scene->combination_dataR = "";
						
						for(int i=0; i<target_chara.size(); i++) {
							if(target_chara.at(i)->getBelongingIndex() == ii) {
								for(int j=0; j<chara_list.size(); j++){
									if(target_chara.at(i)->getPrimaryId() != chara_list.at(j)->getPrimaryId() &&
									   (target_chara.at(i)->getFieldIndex()==2 || target_chara.at(i)->getFieldIndex()==4 || target_chara.at(i)->getFieldIndex()==6) &&
									   (chara_list.at(j)->getFieldIndex()>1)){//フィールドのキャラのみ
										if(chara_list.at(j)->getFieldIndex()==target_chara.at(i)->leftCharaFieldIndex()){//左側のキャラがいるとき
											if(target_chara.at(i)->getCombinationL()==(chara_list.at(j)->getCombinationR())){//コンボマークが一致した時
												HMSLOG("左コンボ！！");
												HMSLOG("%d vs %d", target_chara.at(i)->getPrimaryId(), chara_list.at(j)->getPrimaryId());
												ComboEffectLayer2* combo_effect_layer = ComboEffectLayer2::create();
												float combo_effect_time = 1.0f;//カットイン再生時間
												combo_effect_layer->setData(target_chara.at(i), chara_list.at(j), combo_effect_time);
												dungeon_scene->addChild(combo_effect_layer, 100);
											}
										}
										if(chara_list.at(j)->getFieldIndex()==target_chara.at(i)->rightCharaFieldIndex()) {//右側のキャラがいるとき
											if(target_chara.at(i)->getCombinationR()==(chara_list.at(j)->getCombinationL())) {
												HMSLOG("右コンボ！！");
												HMSLOG("%d vs %d", target_chara.at(i)->getPrimaryId(), chara_list.at(j)->getPrimaryId());
												ComboEffectLayer2* combo_effect_layer = ComboEffectLayer2::create();
												float combo_effect_time = 1.0f;//カットイン再生時間
												combo_effect_layer->setData(target_chara.at(i), chara_list.at(j), combo_effect_time);
												dungeon_scene->addChild(combo_effect_layer, 100);
											}
										}
									}
								}
							}
						}
					}
					finish_flg = true;//最後の演出終了フラグ
					break;
				}
				default:
					HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
					finish_flg = true;//エラーにより演出終了
					break;
			}
			break;
		}
            
        case 37:{//味方を犠牲にして固定値回復
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* gisei2 = ParticleSystemQuad::create("download/Effect/Particle/gisei2.plist");
                            gisei2->setAutoRemoveOnFinish(true);
                            gisei2->setPosition(target_chara.at(i)->getPosition());
                            gisei2->setScale(ratio);
                            this->addChild(gisei2, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/gisei2.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/kaihuku.plist");
                        kaihuku->setAutoRemoveOnFinish(true);
                        kaihuku->setPosition(CharaList::getLeader(used_index)->getPosition());
                        kaihuku->setScale(ratio);
                        this->addChild(kaihuku, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku.mp3");
                        delay_time = 1.2f;//演出終了までの時間
                    }
                    break;
                }
                case 3:{
                    int skill_recovery = use_chara->getActiveSkillValue();
                    dung->playerHpUpdate(used_index, -1 * skill_recovery, dung->player[used_index]->getHp());
                    delay_time = 0.8f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        
        case 38:{//味方を犠牲にしてその戦闘力の数値分回復
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* gisei2 = ParticleSystemQuad::create("download/Effect/Particle/gisei2.plist");
                            gisei2->setAutoRemoveOnFinish(true);
                            gisei2->setPosition(target_chara.at(i)->getPosition());
                            gisei2->setScale(ratio);
                            this->addChild(gisei2, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/gisei2.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/kaihuku.plist");
                        kaihuku->setAutoRemoveOnFinish(true);
                        kaihuku->setPosition(CharaList::getLeader(used_index)->getPosition());
                        kaihuku->setScale(ratio);
                        this->addChild(kaihuku, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku.mp3");
                        delay_time = 1.2f;//演出終了までの時間
                    }
                    break;
                }
                case 3:{
                    int skill_recovery = 0;
                    for(int i=0; i<target_chara.size(); i++){
                        skill_recovery += target_chara.at(i)->getValue();
                    }
                    dung->playerHpUpdate(used_index, -1 * skill_recovery, dung->player[used_index]->getHp());
                    delay_time = 0.8f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 41:{//次のターン死亡
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            
                            if(skill_val.at(i).at(0)>0) {//状態異常成功の時
                                ParticleSystemQuad* circle = ParticleSystemQuad::create("download/Effect/Particle/circle.plist");
                                circle->setAutoRemoveOnFinish(true);
                                circle->setPosition(target_chara.at(i)->getPosition());
                                circle->setScale(ratio);
                                this->addChild(circle, 1);
                                
                                ParticleSystemQuad* death = ParticleSystemQuad::create("download/Effect/Particle/death.plist");
                                death->setAutoRemoveOnFinish(true);
                                death->setPosition(target_chara.at(i)->getPosition());
                                death->setScale(ratio);
                                this->addChild(death, 1);
                            } else {//状態異常失敗の時
                                //missエフェクト
                                missEffect(target_chara.at(i)->getPosition());
                                //スキル失敗SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }

                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/gisei2.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 42:{//手札入れ替え
            switch(effect_count) {
                case 1:{
                    vector<int> position_index;
                    for(int i=0; i<CharaList::getOneSide(used_index).size(); i++){
                        if(CharaList::getOneSide(used_index).at(i)->isInHand()){
                            position_index.push_back(CharaList::getOneSide(used_index).at(i)->getHandIndex());
                        }
                    }
                    std:sort(position_index.begin(),position_index.end(), std::greater<int>());
                    for(int i=0; i<position_index.size(); i++){
                        
                        CCLOG("手札入れ替え%d",position_index.at(i));
                        
                        
                        for(int j=0; j<CharaList::getOneSide(used_index).size(); j++){
                            if(CharaList::getOneSide(used_index).at(j)->getHandIndex() == position_index.at(i)){
                                
                                Chara *temp_chara = CharaList::getOneSide(used_index).at(j);
                                auto delay = DelayTime::create(i*0.0f);
                                auto move = MoveTo::create(0.5f, dung->fieldLayer->deck_point[used_index]);
                                auto call = CallFunc::create([temp_chara,&dung](){
                                    temp_chara->setDeadFlg(true);
                                    temp_chara->initialize();
                                    //temp_chara->showCardParam(TurnManage::getPlayerIndex());
                                });
                                CharaList::getOneSide(used_index).at(j)->runAction(Sequence::create(delay,move,call, NULL));
                            }
                        }
                    
                    }
                    
                    for(int i=0; i<position_index.size(); i++){
                        Chara *chara = dung->deck[used_index].at(0);//デッキシャッフルはinitializeでやったので、一番最初を取る
                        dung->deck[used_index].erase(dung->deck[used_index].begin());//デッキから取り除いて
                        chara->setHandIndex(position_index.at(i));//手札として
                        chara->reverse_flg = true;
                        dung->cardLayer->reorderChild(chara, 1);//一旦前面に
                    }
                    
                    delay_time = 1.0f;//演出終了までの時間
                    break;
                }
                case 2:{
                    for(int i=0; i<CharaList::getOneSide(used_index).size(); i++) {
                        Chara *chara = CharaList::getOneSide(used_index).at(i);
                        
                        if(chara->isInHand() && chara->getPosition() != dung->fieldLayer->hand_position_list[used_index].at(chara->hand_index)) {
                            //HMSLOG("カード配布");
                            DelayTime *delay = DelayTime::create(0.1f*chara->hand_index);
                            MoveTo *move1 = MoveTo::create(0.35f,dung->fieldLayer->hand_position_list[used_index].at(chara->getHandIndex()));
                            EaseOut *ease = EaseOut::create(move1, 2.5f);
                            DelayTime *delay2 = DelayTime::create(0.12f+0.03*chara->hand_index);
                            int test = TurnManage::getPlayerIndex();
                            
                            CallFunc *func = CallFunc::create([&,chara,test,dung](){
                                
                                if(chara->getBelongingIndex() == test){
                                    chara->cardReverse(test, false);//表にする
                                    dung->cardLayer->reorderChild(chara, 0);//後面に移動
                                }
                            });
                            
                            chara->runAction(Sequence::create(delay,ease,delay2,func, nullptr));
                            
                        }
                    }
                    
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 43:
        case 45:{//コンボカラーチェンジ
            switch(effect_count) {
                case 1:{
                    for(int i=0; i<target_chara.size(); i++) {
                        if(target_chara.at(i)->getBelongingIndex() == TurnManage::getPlayerIndex()){
                            if(skill_val.at(i).at(0) == 1){//左側
                                target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_l"]);//コンボマークが違っていたら画像変更
                                string filename = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationL()) +"_l.png";
                                target_chara.at(i)->sprite_map["combination_l"] = Sprite::create(filename);
                                target_chara.at(i)->sprite_map["combination_l"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_l"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_l"]->setTag(target_chara.at(i)->getCombinationL());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_l"], 2);
                            }else if(skill_val.at(i).at(0) == 2){//右側
                                target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_r"]);
                                string filename2 = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationR()) +"_r.png";
                                target_chara.at(i)->sprite_map["combination_r"] = Sprite::create(filename2);
                                target_chara.at(i)->sprite_map["combination_r"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_r"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_r"]->setTag(target_chara.at(i)->getCombinationR());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_r"], 2);
                            }
                        }else{
                            if(skill_val.at(i).at(0) == 2){//左側
                                target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_l"]);//コンボマークが違っていたら画像変更
                                string filename = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationR()) +"_l.png";
                                target_chara.at(i)->sprite_map["combination_l"] = Sprite::create(filename);
                                target_chara.at(i)->sprite_map["combination_l"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_l"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_l"]->setTag(target_chara.at(i)->getCombinationR());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_l"], 2);
                            }else if(skill_val.at(i).at(0) == 1){//右側
                                target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_r"]);
                                string filename2 = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationL()) +"_r.png";
                                target_chara.at(i)->sprite_map["combination_r"] = Sprite::create(filename2);
                                target_chara.at(i)->sprite_map["combination_r"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_r"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_r"]->setTag(target_chara.at(i)->getCombinationL());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_r"], 2);
                            }
                        }
                    }
                    
                    if(effect_onoff_flg) {
                        
                        for(int i=0; i<target_chara.size(); i++) {
                            
                            Vec2 temp;
                            if((target_chara.at(i)->getBelongingIndex() == TurnManage::getPlayerIndex() && skill_val.at(i).at(0) == 1)||(target_chara.at(i)->getBelongingIndex() == TurnManage::getEnemyIndex() && skill_val.at(i).at(0) == 2)){//左側
                                temp = Vec2(target_chara.at(i)->getPosition().x - 42*ratio ,target_chara.at(i)->getPosition().y);
                            }else if((target_chara.at(i)->getBelongingIndex() == TurnManage::getPlayerIndex() && skill_val.at(i).at(0) == 2)||(target_chara.at(i)->getBelongingIndex() == TurnManage::getEnemyIndex() && skill_val.at(i).at(0) == 1)){//右側
                                temp = Vec2(target_chara.at(i)->getPosition().x + 42*ratio ,target_chara.at(i)->getPosition().y);
                            }
                            
                            //パーティクル出力
                            ParticleSystemQuad* heal = ParticleSystemQuad::create("download/Effect/Particle/heal2.plist");
                            heal->setAutoRemoveOnFinish(true);
                            heal->setPosition(temp);
                            heal->setScale(ratio);
                            this->addChild(heal, 1);
                            
                            ParticleSystemQuad* heal_plus = ParticleSystemQuad::create("download/Effect/Particle/heal_plus.plist");
                            heal_plus->setAutoRemoveOnFinish(true);
                            heal_plus->setPosition(temp);
                            heal_plus->setScale(ratio);
                            this->addChild(heal_plus, 1);
                            
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/heal.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
                    
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 44:{//コンボカラー交換
            switch(effect_count) {
                case 1:{
                    for(int i=0; i<target_chara.size(); i++) {
                        if(target_chara.at(i)->getBelongingIndex() == TurnManage::getPlayerIndex()){
                            
                            if(target_chara.at(i)->sprite_map["combination_l"] != nullptr) target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_l"]);//コンボマークが違っていたら画像変更
                            target_chara.at(i)->sprite_map["combination_l"] = nullptr;
                            if(target_chara.at(i)->getCombinationL()>0){
                                string filename = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationL()) +"_l.png";
                                target_chara.at(i)->sprite_map["combination_l"] = Sprite::create(filename);
                                target_chara.at(i)->sprite_map["combination_l"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_l"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_l"]->setTag(target_chara.at(i)->getCombinationL());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_l"], 2);
                            }
                            if(target_chara.at(i)->sprite_map["combination_r"] != nullptr) target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_r"]);
                            target_chara.at(i)->sprite_map["combination_r"] = nullptr;
                            if(target_chara.at(i)->getCombinationR()>0){
                                string filename2 = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationR()) +"_r.png";
                                target_chara.at(i)->sprite_map["combination_r"] = Sprite::create(filename2);
                                target_chara.at(i)->sprite_map["combination_r"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_r"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_r"]->setTag(target_chara.at(i)->getCombinationR());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_r"], 2);
                            }
                        }else{
                            
                            if(target_chara.at(i)->sprite_map["combination_l"] != nullptr) target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_l"]);//コンボマークが違っていたら画像変更
                            target_chara.at(i)->sprite_map["combination_l"] = nullptr;
                            if(target_chara.at(i)->getCombinationR()>0){
                                string filename = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationR()) +"_l.png";
                                target_chara.at(i)->sprite_map["combination_l"] = Sprite::create(filename);
                                target_chara.at(i)->sprite_map["combination_l"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_l"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_l"]->setTag(target_chara.at(i)->getCombinationR());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_l"], 2);
                            }
                            if(target_chara.at(i)->sprite_map["combination_r"] != nullptr) target_chara.at(i)->removeChild(target_chara.at(i)->sprite_map["combination_r"]);
                            target_chara.at(i)->sprite_map["combination_r"] = nullptr;
                            if(target_chara.at(i)->getCombinationL()>0){
                                string filename2 = "download/Chara/parts100/card_combo"+ to_string(target_chara.at(i)->getCombinationL()) +"_r.png";
                                target_chara.at(i)->sprite_map["combination_r"] = Sprite::create(filename2);
                                target_chara.at(i)->sprite_map["combination_r"]->setScale(ratio);
                                target_chara.at(i)->sprite_map["combination_r"]->setPosition(Point(0, 0));
                                target_chara.at(i)->sprite_map["combination_r"]->setTag(target_chara.at(i)->getCombinationL());
                                target_chara.at(i)->addChild(target_chara.at(i)->sprite_map["combination_r"], 2);
                            }
                        }
                    }
                    
                    if(use_chara->getBelongingIndex() == TurnManage::getPlayerIndex()){
                        
                        if(use_chara->sprite_map["combination_l"]!=nullptr) use_chara->removeChild(use_chara->sprite_map["combination_l"]);//コンボマークが違っていたら画像変更
                        use_chara->sprite_map["combination_l"] = nullptr;
                        if(use_chara->getCombinationL()>0){
                            string filename = "download/Chara/parts100/card_combo"+ to_string(use_chara->getCombinationL()) +"_l.png";
                            use_chara->sprite_map["combination_l"] = Sprite::create(filename);
                            use_chara->sprite_map["combination_l"]->setScale(ratio);
                            use_chara->sprite_map["combination_l"]->setPosition(Point(0, 0));
                            use_chara->sprite_map["combination_l"]->setTag(use_chara->getCombinationL());
                            use_chara->addChild(use_chara->sprite_map["combination_l"], 2);
                        }
                        
                        if(use_chara->sprite_map["combination_r"]!=nullptr) use_chara->removeChild(use_chara->sprite_map["combination_r"]);
                        use_chara->sprite_map["combination_r"] = nullptr;
                        if(use_chara->getCombinationR()>0){
                            string filename2 = "download/Chara/parts100/card_combo"+ to_string(use_chara->getCombinationR()) +"_r.png";
                            use_chara->sprite_map["combination_r"] = Sprite::create(filename2);
                            use_chara->sprite_map["combination_r"]->setScale(ratio);
                            use_chara->sprite_map["combination_r"]->setPosition(Point(0, 0));
                            use_chara->sprite_map["combination_r"]->setTag(use_chara->getCombinationR());
                            use_chara->addChild(use_chara->sprite_map["combination_r"], 2);
                        }
                    }else{
                        
                        if(use_chara->sprite_map["combination_l"]!=nullptr) use_chara->removeChild(use_chara->sprite_map["combination_l"]);//コンボマークが違っていたら画像変更
                        use_chara->sprite_map["combination_l"] = nullptr;
                        if(use_chara->getCombinationR()>0){
                            string filename = "download/Chara/parts100/card_combo"+ to_string(use_chara->getCombinationR()) +"_l.png";
                            use_chara->sprite_map["combination_l"] = Sprite::create(filename);
                            use_chara->sprite_map["combination_l"]->setScale(ratio);
                            use_chara->sprite_map["combination_l"]->setPosition(Point(0, 0));
                            use_chara->sprite_map["combination_l"]->setTag(use_chara->getCombinationR());
                            use_chara->addChild(use_chara->sprite_map["combination_l"], 2);
                        }
                        
                        if(use_chara->sprite_map["combination_r"]!=nullptr) use_chara->removeChild(use_chara->sprite_map["combination_r"]);
                        use_chara->sprite_map["combination_r"] = nullptr;
                        if(use_chara->getCombinationL()>0){
                            string filename2 = "download/Chara/parts100/card_combo"+ to_string(use_chara->getCombinationL()) +"_r.png";
                            use_chara->sprite_map["combination_r"] = Sprite::create(filename2);
                            use_chara->sprite_map["combination_r"]->setScale(ratio);
                            use_chara->sprite_map["combination_r"]->setPosition(Point(0, 0));
                            use_chara->sprite_map["combination_r"]->setTag(use_chara->getCombinationL());
                            use_chara->addChild(use_chara->sprite_map["combination_r"], 2);
                        }
                    }
                    
                    if(effect_onoff_flg) {
                        
                        for(int i=0; i<target_chara.size(); i++) {//ターゲット用
                            Vec2 temp = Vec2(target_chara.at(i)->getPosition().x - 42*ratio ,target_chara.at(i)->getPosition().y);
                            Vec2 temp2 = Vec2(target_chara.at(i)->getPosition().x + 42*ratio ,target_chara.at(i)->getPosition().y);
                            
                            //パーティクル出力
                            ParticleSystemQuad* heal = ParticleSystemQuad::create("download/Effect/Particle/heal2.plist");
                            heal->setAutoRemoveOnFinish(true);
                            heal->setPosition(temp);
                            heal->setScale(ratio);
                            this->addChild(heal, 1);
                            
                            ParticleSystemQuad* heal_plus = ParticleSystemQuad::create("download/Effect/Particle/heal_plus.plist");
                            heal_plus->setAutoRemoveOnFinish(true);
                            heal_plus->setPosition(temp);
                            heal_plus->setScale(ratio);
                            this->addChild(heal_plus, 1);
                            //パーティクル出力
                            ParticleSystemQuad* heal2 = ParticleSystemQuad::create("download/Effect/Particle/heal2.plist");
                            heal2->setAutoRemoveOnFinish(true);
                            heal2->setPosition(temp2);
                            heal2->setScale(ratio);
                            this->addChild(heal2, 1);
                            
                            ParticleSystemQuad* heal_plus2 = ParticleSystemQuad::create("download/Effect/Particle/heal_plus.plist");
                            heal_plus2->setAutoRemoveOnFinish(true);
                            heal_plus2->setPosition(temp2);
                            heal_plus2->setScale(ratio);
                            this->addChild(heal_plus2, 1);
                        }
                        //自分用
                        Vec2 temp = Vec2(use_chara->getPosition().x - 42*ratio ,use_chara->getPosition().y);
                        Vec2 temp2 = Vec2(use_chara->getPosition().x + 42*ratio ,use_chara->getPosition().y);
                        
                        //パーティクル出力
                        ParticleSystemQuad* heal = ParticleSystemQuad::create("download/Effect/Particle/heal2.plist");
                        heal->setAutoRemoveOnFinish(true);
                        heal->setPosition(temp);
                        heal->setScale(ratio);
                        this->addChild(heal, 1);
                        
                        ParticleSystemQuad* heal_plus = ParticleSystemQuad::create("download/Effect/Particle/heal_plus.plist");
                        heal_plus->setAutoRemoveOnFinish(true);
                        heal_plus->setPosition(temp);
                        heal_plus->setScale(ratio);
                        this->addChild(heal_plus, 1);
                        //パーティクル出力
                        ParticleSystemQuad* heal2 = ParticleSystemQuad::create("download/Effect/Particle/heal2.plist");
                        heal2->setAutoRemoveOnFinish(true);
                        heal2->setPosition(temp2);
                        heal2->setScale(ratio);
                        this->addChild(heal2, 1);
                        
                        ParticleSystemQuad* heal_plus2 = ParticleSystemQuad::create("download/Effect/Particle/heal_plus.plist");
                        heal_plus2->setAutoRemoveOnFinish(true);
                        heal_plus2->setPosition(temp2);
                        heal_plus2->setScale(ratio);
                        this->addChild(heal_plus2, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/heal.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
                    
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 46:{//攻撃による総大将へのダメージを減少させる
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                            
                        //パーティクル出力
                        ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                        kiryoku->setAutoRemoveOnFinish(true);
                        kiryoku->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
                        kiryoku->setScale(ratio);
                        this->addChild(kiryoku, 1);
                        
                        ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                        kiryoku_plus->setAutoRemoveOnFinish(true);
                        kiryoku_plus->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
                        kiryoku_plus->setScale(ratio);
                        this->addChild(kiryoku_plus, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                    }
                    //ステータスバッファに、使用キャラ、効果量、残りターンを追加
                    map<string,int> temp;
                    temp["primary"] = use_chara->getPrimaryId();
                    temp["turn"] = use_chara->getActiveSkillTurn()*2;
                    temp["value"] = -1 * use_chara->getActiveSkillValue();
                    temp["state"] = STATUS_TYPE_PLAYER_ATTACK_UPDOWN;
                    dung->player[use_chara->getBelongingIndex()]->whole_status_active.push_back(temp);
                    
                    dung->uiLayer->insertWholeBufferIcon(use_chara->getBelongingIndex(),use_chara);
                    
                    break;
                }
                    
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 47:{//手札公開
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<CharaList::getOneSide(use_chara->reverseIndex(used_index)).size(); i++){
                            if(CharaList::getOneSide(use_chara->reverseIndex(used_index)).at(i)->isInHand()){
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(CharaList::getOneSide(use_chara->reverseIndex(used_index)).at(i)->getPosition());
                                hit1->setScale(ratio);
                                this->addChild(hit1, 1);
                            }
                            
                        }
            
                        delay_time = 0.1f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<CharaList::getOneSide(use_chara->reverseIndex(used_index)).size(); i++){
                            if(CharaList::getOneSide(use_chara->reverseIndex(used_index)).at(i)->isInHand()){
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(CharaList::getOneSide(use_chara->reverseIndex(used_index)).at(i)->getPosition());
                                hit1->setScale(ratio);
                                this->addChild(hit1, 1);
                            }
                            
                        }
                        
                        delay_time = 0.1f;//演出終了までの時間
                    }
                    //ステータスバッファに、使用キャラ、効果量、残りターンを追加
                    map<string,int> temp;
                    temp["primary"] = use_chara->getPrimaryId();
                    temp["turn"] = use_chara->getActiveSkillTurn()*2;
                    temp["value"] = use_chara->getActiveSkillValue();
                    temp["state"] = STATUS_TYPE_HAND_OPEN;
                    dung->player[use_chara->getBelongingIndex()]->whole_status_active.push_back(temp);
                    
                    dung->uiLayer->insertWholeBufferIcon(use_chara->getBelongingIndex(),use_chara);
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 50:{//対象のキャラをランダムな場所に弾き飛ばす
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            //パーティクル出力
                            ParticleSystemQuad* cyclone = ParticleSystemQuad::create("download/Effect/Particle/cyclone.plist");
                            cyclone->setAutoRemoveOnFinish(true);
                            cyclone->setPosition(target_chara.at(i)->getPosition());
                            cyclone->setScale(ratio/3);
                            cyclone->setEmissionRate(2000.0f);
                            //cyclone->setStartSize(2.0f);
                            //cyclone->setEndSize(2.0f);
                            cyclone->setDuration(1.0f);
                            this->addChild(cyclone, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/cyclone.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    //新しいフィールドに移動
                    for(int i=0; i<target_chara.size(); i++) {
                        Point target_position = static_cast<DungeonScene*>(getParent())->fieldLayer->field_position_list[target_chara.at(i)->getBelongingIndex()].at(skill_val.at(i).at(0));
                        //CCLOG("フィールド%dに移動",skill_value.at(i));
                        MoveTo *move = MoveTo::create(0.2f, target_position);
                        target_chara.at(i)->runAction(move);
                    }
                    delay_time = 0.5f;//演出終了までの時間
                    break;
                }
                case 3:{
                    //移動後のコンビネーションエフェクト
                    DungeonScene* dungeon_scene = static_cast<DungeonScene *>(getParent());
                    for(int ii=0; ii<2; ii++){
                        HmsVector<Chara*> chara_list;
                        chara_list.clear();
                        chara_list = CharaList::getOneSide(ii);
                        dungeon_scene->combination_dataL = "";
                        dungeon_scene->combination_dataR = "";
                        
                        for(int i=0; i<target_chara.size(); i++) {
                            if(target_chara.at(i)->getBelongingIndex() == ii) {
                                for(int j=0; j<chara_list.size(); j++){
                                    if(target_chara.at(i)->getPrimaryId() != chara_list.at(j)->getPrimaryId() &&
                                       (target_chara.at(i)->getFieldIndex()==2 || target_chara.at(i)->getFieldIndex()==4 || target_chara.at(i)->getFieldIndex()==6) &&
                                       (chara_list.at(j)->getFieldIndex()>1)){//フィールドのキャラのみ
                                        if(chara_list.at(j)->getFieldIndex()==target_chara.at(i)->leftCharaFieldIndex()){//左側のキャラがいるとき
                                            if(target_chara.at(i)->getCombinationL()==(chara_list.at(j)->getCombinationR())){//コンボマークが一致した時
                                                HMSLOG("左コンボ！！");
                                                HMSLOG("%d vs %d", target_chara.at(i)->getPrimaryId(), chara_list.at(j)->getPrimaryId());
                                                ComboEffectLayer2* combo_effect_layer = ComboEffectLayer2::create();
                                                float combo_effect_time = 1.0f;//カットイン再生時間
                                                combo_effect_layer->setData(target_chara.at(i), chara_list.at(j), combo_effect_time);
                                                dungeon_scene->addChild(combo_effect_layer, 100);
                                            }
                                        }
                                        if(chara_list.at(j)->getFieldIndex()==target_chara.at(i)->rightCharaFieldIndex()) {//右側のキャラがいるとき
                                            if(target_chara.at(i)->getCombinationR()==(chara_list.at(j)->getCombinationL())) {
                                                HMSLOG("右コンボ！！");
                                                HMSLOG("%d vs %d", target_chara.at(i)->getPrimaryId(), chara_list.at(j)->getPrimaryId());
                                                ComboEffectLayer2* combo_effect_layer = ComboEffectLayer2::create();
                                                float combo_effect_time = 1.0f;//カットイン再生時間
                                                combo_effect_layer->setData(target_chara.at(i), chara_list.at(j), combo_effect_time);
                                                dungeon_scene->addChild(combo_effect_layer, 100);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 53://対象と同じカードが相手の手札かデッキにある時、それらを消し去る
        {
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* jokyo = ParticleSystemQuad::create("download/Effect/Particle/jokyo.plist");
                            jokyo->setAutoRemoveOnFinish(true);
                            jokyo->setPosition(target_chara.at(i)->getPosition());
                            jokyo->setScale(ratio);
                            this->addChild(jokyo, 1);
                        }
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    for(int i=0; i<target_chara.size(); i++) {
                        //if(target_chara.at(i)->getFieldIndex() < 0){
                            if(effect_onoff_flg) {
                                //パーティクル出力
                                ParticleSystemQuad* jokyo_success = ParticleSystemQuad::create("download/Effect/Particle/jokyo_success.plist");
                                jokyo_success->setAutoRemoveOnFinish(true);
                                jokyo_success->setPosition(target_chara.at(i)->getPosition());
                                jokyo_success->setScale(ratio);
                                this->addChild(jokyo_success, 1);
                                
                                //SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo_success.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }
                        //}
                    }
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: SkillDeadType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 54:{//最大HPダウン
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/kaihuku.plist");
                        kaihuku->setAutoRemoveOnFinish(true);
                        kaihuku->setPosition(CharaList::getLeader(enemy_index)->getPosition());
                        kaihuku->setScale(ratio*1.2f);
                        kaihuku->setStartColor(Color4F::RED);
                        kaihuku->setEndColor(Color4F::RED);
                        kaihuku->setStartColorVar(Color4F::GREEN);
                        kaihuku->setEndColorVar(Color4F::BLACK);
                        this->addChild(kaihuku, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
    
                    dung->uiLayer->updatePlayerInfo(dung->player[enemy_index]->getHp(), dung->player[enemy_index]->getMaxHp(), enemy_index);
                    delay_time = 1.0f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 55:{//最大HP上昇
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/kaihuku.plist");
                        kaihuku->setAutoRemoveOnFinish(true);
                        kaihuku->setPosition(CharaList::getLeader(used_index)->getPosition());
                        kaihuku->setScale(ratio*1.2f);
                        kaihuku->setStartColor(Color4F::BLUE);
                        kaihuku->setEndColor(Color4F::BLUE);
                        kaihuku->setStartColorVar(Color4F::GREEN);
                        kaihuku->setEndColorVar(Color4F::WHITE);
                        this->addChild(kaihuku, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    dung->uiLayer->updatePlayerInfo(dung->player[used_index]->getHp(), dung->player[used_index]->getMaxHp(), used_index);
                    delay_time = 1.0f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 56:{//最大HPを残りHPに
            int temp_index = used_index;
            if(use_chara->getActiveSkillTarget()=="enemy") temp_index = enemy_index;
            
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/kaihuku.plist");
                        kaihuku->setAutoRemoveOnFinish(true);
                        kaihuku->setPosition(CharaList::getLeader(temp_index)->getPosition());
                        kaihuku->setScale(ratio*1.2f);
                        kaihuku->setStartColor(Color4F::ORANGE);
                        kaihuku->setEndColor(Color4F::ORANGE);
                        kaihuku->setStartColorVar(Color4F::BLACK);
                        kaihuku->setEndColorVar(Color4F::GRAY);
                        this->addChild(kaihuku, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    dung->uiLayer->updatePlayerInfo(dung->player[temp_index]->getHp(), dung->player[temp_index]->getMaxHp(), temp_index);
                    delay_time = 1.0f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 58:{//根性
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        
                        //パーティクル出力
                        ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                        kiryoku->setAutoRemoveOnFinish(true);
                        kiryoku->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
                        kiryoku->setScale(ratio);
                        this->addChild(kiryoku, 1);
                        
                        ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                        kiryoku_plus->setAutoRemoveOnFinish(true);
                        kiryoku_plus->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
                        kiryoku_plus->setScale(ratio);
                        this->addChild(kiryoku_plus, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                    }
                    map<string,int> temp;
                    temp["primary"] = use_chara->getPrimaryId();
                    temp["turn"] = use_chara->getActiveSkillTurn()*2;
                    temp["value"] = use_chara->getActiveSkillValue();
                    temp["state"] = STATUS_TYPE_ENDURE;
                    dung->player[use_chara->getBelongingIndex()]->whole_status_active.push_back(temp);
                    
                    dung->uiLayer->insertWholeBufferIcon(use_chara->getBelongingIndex(),use_chara);
                    
                    break;
                }
                    
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 59:{//士気を指定の数値に
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* resorb = ParticleSystemQuad::create("download/Effect/Particle/resorb.plist");
                        resorb->setAutoRemoveOnFinish(true);
                        resorb->setPosition(convertFromDisplaySize(dung->uiLayer->power_point[enemy_index], 0.5, 0.5));
                        resorb->setScale(ratio);
                        resorb->setStartColor(Color4F(Color4B(129,41,139,255)));
                        resorb->setEndColor(Color4F(Color4B(129,41,139,255)));
                        resorb->setStartColorVar(Color4F::BLACK);
                        resorb->setEndColorVar(Color4F::GRAY);
                        
                        this->addChild(resorb, 1);
                        
                        dung->uiLayer->updatePower(dung->player[enemy_index]->getNowPower(), enemy_index);
                        
                        delay_time = 0.6f;//演出終了までの時間
                    }
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 60:{//上昇士気をXに固定する
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        HmsVector<Chara *> charas;
                        if(use_chara->getActiveSkillTarget()=="player"){
                            charas.insert(CharaList::getLeader(used_index));
                        }else if (use_chara->getActiveSkillTarget()=="enemy"){
                            charas.insert(CharaList::getLeader(enemy_index));
                        }else{
                            charas.insert(CharaList::getLeader(used_index));
                            charas.insert(CharaList::getLeader(enemy_index));
                        }
                        for(Chara *chara : charas){
                            //パーティクル出力
                            ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                            kiryoku->setAutoRemoveOnFinish(true);
                            kiryoku->setPosition(chara->getPosition());
                            kiryoku->setScale(ratio);
                            kiryoku->setStartColor(Color4F(Color4B(11,218,81,255)));
                            kiryoku->setEndColor(Color4F(Color4B(11,218,81,255)));
                            kiryoku->setStartColorVar(Color4F::BLACK);
                            kiryoku->setEndColorVar(Color4F::GRAY);
                            this->addChild(kiryoku, 1);
                            
                            ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                            kiryoku_plus->setAutoRemoveOnFinish(true);
                            kiryoku_plus->setPosition(chara->getPosition());
                            kiryoku_plus->setScale(ratio);
                            this->addChild(kiryoku_plus, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                    }
                    map<string,int> temp;
                    temp["primary"] = use_chara->getPrimaryId();
                    temp["turn"] = use_chara->getActiveSkillTurn()*2;
                    temp["value"] = use_chara->getActiveSkillValue();
                    temp["state"] = STATUS_TYPE_FIXED_POWER;
                    
                    if(use_chara->getActiveSkillTarget() == "player"){
                        dung->player[use_chara->getBelongingIndex()]->whole_status_active.push_back(temp);
                    }else if(use_chara->getActiveSkillTarget() == "enemy"){
                        dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_active.push_back(temp);
                    }else{
                        dung->player[use_chara->getBelongingIndex()]->whole_status_active.push_back(temp);
                        dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_active.push_back(temp);
                    }
                    
                    
                    if(use_chara->getActiveSkillTarget()=="player"){
                        dung->uiLayer->insertWholeBufferIcon(used_index,use_chara);
                    }else if (use_chara->getActiveSkillTarget()=="enemy"){
                        dung->uiLayer->insertWholeBufferIcon(enemy_index,use_chara);
                    }else{
                        dung->uiLayer->insertWholeBufferIcon(used_index,use_chara);
                        dung->uiLayer->insertWholeBufferIcon(enemy_index,use_chara);
                    }
            
                    break;
                }
                    
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
            
        }
        case 63://ドロー枚数を制限する
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        HmsVector<Chara *> charas;
                        
                        charas.insert(CharaList::getLeader(enemy_index));
                        
                        for(Chara *chara : charas){
                            //パーティクル出力
                            ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                            kiryoku->setAutoRemoveOnFinish(true);
                            kiryoku->setPosition(chara->getPosition());
                            kiryoku->setScale(ratio);
                            kiryoku->setStartColor(Color4F(Color4B(11,218,81,255)));
                            kiryoku->setEndColor(Color4F(Color4B(11,218,81,255)));
                            kiryoku->setStartColorVar(Color4F::BLACK);
                            kiryoku->setEndColorVar(Color4F::GRAY);
                            this->addChild(kiryoku, 1);
                            
                            ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                            kiryoku_plus->setAutoRemoveOnFinish(true);
                            kiryoku_plus->setPosition(chara->getPosition());
                            kiryoku_plus->setScale(ratio);
                            this->addChild(kiryoku_plus, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                    }
                    map<string,int> temp;
                    temp["primary"] = use_chara->getPrimaryId();
                    temp["turn"] = use_chara->getActiveSkillTurn()*2;
                    temp["value"] = use_chara->getActiveSkillValue();
                    temp["state"] = STATUS_TYPE_DRAW_CONFINE;
                    dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_active.push_back(temp);
                    
                    dung->uiLayer->insertWholeBufferIcon(enemy_index,use_chara);
                    
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
            
        case 64:{//相手はXコストのカードを出すことができない
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        HmsVector<Chara *> charas;
                        
                        charas.insert(CharaList::getLeader(enemy_index));
                        
                        for(Chara *chara : charas){
                            //パーティクル出力
                            ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                            kiryoku->setAutoRemoveOnFinish(true);
                            kiryoku->setPosition(chara->getPosition());
                            kiryoku->setScale(ratio);
                            kiryoku->setStartColor(Color4F(Color4B(11,218,81,255)));
                            kiryoku->setEndColor(Color4F(Color4B(11,218,81,255)));
                            kiryoku->setStartColorVar(Color4F::BLACK);
                            kiryoku->setEndColorVar(Color4F::GRAY);
                            this->addChild(kiryoku, 1);
                            
                            ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                            kiryoku_plus->setAutoRemoveOnFinish(true);
                            kiryoku_plus->setPosition(chara->getPosition());
                            kiryoku_plus->setScale(ratio);
                            this->addChild(kiryoku_plus, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                    }
                    map<string,int> temp;
                    temp["primary"] = use_chara->getPrimaryId();
                    temp["turn"] = use_chara->getActiveSkillTurn()*2;
                    temp["value"] = use_chara->getActiveSkillValue();
                    temp["state"] = STATUS_TYPE_CANT_SUMMONS;
                    dung->player[use_chara->reverseIndex(use_chara->getBelongingIndex())]->whole_status_active.push_back(temp);
                    
                    dung->uiLayer->insertWholeBufferIcon(enemy_index,use_chara);
                
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
            
        }
        case 65:{//対象の戦闘タイプをXにする
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        
                        for(Chara *chara : target_chara){
                            //パーティクル出力
                            ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/type_change.plist");
                            kiryoku->setAutoRemoveOnFinish(true);
                            kiryoku->setPosition(chara->getPosition() + chara->sprite_map["type"]->getPosition());
                            kiryoku->setScale(ratio);
                            this->addChild(kiryoku, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                    }
                    
                    for(int i=0; i<target_chara.size(); i++) {
                        target_chara.at(i)->setTypeSprite();
                        target_chara.at(i)->sprite_map["type"]->setGlobalZOrder(1.0f);
                        float pre_scale = target_chara.at(i)->sprite_map["type"]->getScale();
                        
                        
                        ScaleTo *scale = ScaleTo::create(0.3,2.8*pre_scale);
                        EaseOut *ease_o = EaseOut::create(scale, 2);
                        
                        
                        DelayTime *delay = DelayTime::create(0.2);
                        ScaleTo *scale2 = ScaleTo::create(0.6, pre_scale);
                        EaseIn *ease_i = EaseIn::create(scale2, 2);
                        
                        Sprite *sprite = target_chara.at(i)->sprite_map["type"];
                        Sequence *seq = Sequence::create(ease_o,delay,ease_i,CallFunc::create([sprite](){
                            sprite->setGlobalZOrder(0.);
                        }), nullptr);
                        
                        target_chara.at(i)->sprite_map["type"]->runAction(seq);

                    }
                    
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        
        case 70:{//敵のHPをX/1000にする
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* zan1 = ParticleSystemQuad::create("download/Effect/Particle/gravity.plist");
                        zan1->setPosition(CharaList::getLeader(enemy_index)->getPosition());
                        zan1->setScale(ratio *3/4);
                        this->addChild(zan1, 1);
                        
                        ParticleSystemQuad* zan2 = ParticleSystemQuad::create("download/Effect/Particle/gravity2.plist");
                        zan2->setPosition(CharaList::getLeader(enemy_index)->getPosition());
                        zan2->setScale(ratio *3/4);
                        this->addChild(zan2, 0);
                        
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/player_damage.mp3");
                        delay_time = 1.5f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    //int skill_damage;
                    
                    //skill_damage = use_chara->getActiveSkillValue();
                    
                    dung->playerHpUpdate(enemy_index, skill_val.at(0).at(0), dung->player[enemy_index]->getHp());
                    delay_time = 0.5f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 69://行動不能除去 と　ダメージ
        {
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* jokyo = ParticleSystemQuad::create("download/Effect/Particle/jokyo.plist");
                            jokyo->setAutoRemoveOnFinish(true);
                            jokyo->setPosition(target_chara.at(i)->getPosition());
                            jokyo->setScale(ratio);
                            this->addChild(jokyo, 1);
                        }
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo.mp3");
                        delay_time = 1.0f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    if(use_chara->getActiveSkillValue() > 0){
                        dung->playerHpUpdate(enemy_index, target_chara.size() * skill_val.at(0).at(0), dung->player[enemy_index]->getHp());
                    }
                    CCLOG("てすーサイズ+%d",target_chara.size());
                    for(int i=0; i<target_chara.size(); i++) {
                        
                        if(effect_onoff_flg) {
                            //パーティクル出力
                            ParticleSystemQuad* jokyo_success = ParticleSystemQuad::create("download/Effect/Particle/jokyo_success.plist");
                            jokyo_success->setAutoRemoveOnFinish(true);
                            jokyo_success->setPosition(target_chara.at(i)->getPosition());
                            jokyo_success->setScale(ratio);
                            this->addChild(jokyo_success, 1);
                            
                            //SE
                            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo_success.mp3");
                            delay_time = 1.0f;//演出終了までの時間
                        }
                    }
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: SkillDeadType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        
        case 71:{//選択対象行動不能無効
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            //パーティクル出力
                            ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                            kaihuku->setAutoRemoveOnFinish(true);
                            kaihuku->setStartColor(Color4F(Color4B(255,140,0,255)));
                            kaihuku->setEndColor(Color4F(Color4B(255,140,0,100)));
                            kaihuku->setPosition(target_chara.at(i)->getPosition());
                            kaihuku->setScale(ratio*0.4f);
                            this->addChild(kaihuku, 3);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 72:{//選択対象除去回避
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            //パーティクル出力
                            ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                            kaihuku->setAutoRemoveOnFinish(true);
                            kaihuku->setPosition(target_chara.at(i)->getPosition());
                            kaihuku->setScale(ratio*0.4f);
                            this->addChild(kaihuku, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 73:{//攻撃回数追加
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            //パーティクル出力
                            ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/twice_attack.plist");
                            kaihuku->setAutoRemoveOnFinish(true);
                            kaihuku->setPosition(target_chara.at(i)->getPosition());
                            kaihuku->setScale(ratio*1.2f);
                            
                            this->addChild(kaihuku, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
        
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 75:{//対象のキャラを自分の前方に移動させる
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            //パーティクル出力
                            ParticleSystemQuad* cyclone = ParticleSystemQuad::create("download/Effect/Particle/cyclone.plist");
                            cyclone->setAutoRemoveOnFinish(true);
                            cyclone->setPosition(target_chara.at(i)->getPosition());
                            cyclone->setScale(ratio/3);
                            cyclone->setEmissionRate(2000.0f);
                            //cyclone->setStartSize(2.0f);
                            //cyclone->setEndSize(2.0f);
                            cyclone->setDuration(1.0f);
                            this->addChild(cyclone, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/cyclone.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    //新しいフィールドに移動
                    for(int i=0; i<target_chara.size(); i++) {
                        Point target_position = static_cast<DungeonScene*>(getParent())->fieldLayer->field_position_list[target_chara.at(i)->getBelongingIndex()].at(skill_val.at(i).at(0));
                        //CCLOG("フィールド%dに移動",skill_value.at(i));
                        MoveTo *move = MoveTo::create(0.2f, target_position);
                        target_chara.at(i)->runAction(move);
                    }
                    delay_time = 0.5f;//演出終了までの時間
                    break;
                }
                case 3:{
                    //移動後のコンビネーションエフェクト
                    DungeonScene* dungeon_scene = static_cast<DungeonScene *>(getParent());
                    for(int ii=0; ii<2; ii++){
                        HmsVector<Chara*> chara_list;
                        chara_list.clear();
                        chara_list = CharaList::getOneSide(ii);
                        dungeon_scene->combination_dataL = "";
                        dungeon_scene->combination_dataR = "";
                        
                        for(int i=0; i<target_chara.size(); i++) {
                            if(target_chara.at(i)->getBelongingIndex() == ii) {
                                for(int j=0; j<chara_list.size(); j++){
                                    if(target_chara.at(i)->getPrimaryId() != chara_list.at(j)->getPrimaryId() &&
                                       (target_chara.at(i)->getFieldIndex()==2 || target_chara.at(i)->getFieldIndex()==4 || target_chara.at(i)->getFieldIndex()==6) &&
                                       (chara_list.at(j)->getFieldIndex()>1)){//フィールドのキャラのみ
                                        if(chara_list.at(j)->getFieldIndex()==target_chara.at(i)->leftCharaFieldIndex()){//左側のキャラがいるとき
                                            if(target_chara.at(i)->getCombinationL()==(chara_list.at(j)->getCombinationR())){//コンボマークが一致した時
                                                HMSLOG("左コンボ！！");
                                                HMSLOG("%d vs %d", target_chara.at(i)->getPrimaryId(), chara_list.at(j)->getPrimaryId());
                                                ComboEffectLayer2* combo_effect_layer = ComboEffectLayer2::create();
                                                float combo_effect_time = 1.0f;//カットイン再生時間
                                                combo_effect_layer->setData(target_chara.at(i), chara_list.at(j), combo_effect_time);
                                                dungeon_scene->addChild(combo_effect_layer, 100);
                                            }
                                        }
                                        if(chara_list.at(j)->getFieldIndex()==target_chara.at(i)->rightCharaFieldIndex()) {//右側のキャラがいるとき
                                            if(target_chara.at(i)->getCombinationR()==(chara_list.at(j)->getCombinationL())) {
                                                HMSLOG("右コンボ！！");
                                                HMSLOG("%d vs %d", target_chara.at(i)->getPrimaryId(), chara_list.at(j)->getPrimaryId());
                                                ComboEffectLayer2* combo_effect_layer = ComboEffectLayer2::create();
                                                float combo_effect_time = 1.0f;//カットイン再生時間
                                                combo_effect_layer->setData(target_chara.at(i), chara_list.at(j), combo_effect_time);
                                                dungeon_scene->addChild(combo_effect_layer, 100);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        case 76:{//対象を山札の一番上に戻す
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            //パーティクル出力
                            ParticleSystemQuad* cyclone = ParticleSystemQuad::create("download/Effect/Particle/cyclone.plist");
                            cyclone->setAutoRemoveOnFinish(true);
                            cyclone->setPosition(target_chara.at(i)->getPosition());
                            cyclone->setScale(ratio/3);
                            cyclone->setEmissionRate(500.0f);
                            //cyclone->setEndColor(Color4F::GREEN);
                            //cyclone->setEndSize(2.0f);
                            cyclone->setDuration(0.8f);
                            this->addChild(cyclone, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/cyclone.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    //新しいフィールドに移動
                    for(int i=0; i<target_chara.size(); i++) {
                        Point target_position = target_chara.at(i)->getPosition();
                        
                        target_chara.at(i)->setDeadFlg(true);
                        target_chara.at(i)->initialize();
                        
                        dung->deck[target_chara.at(i)->getBelongingIndex()].pop_back();
                        dung->deck[target_chara.at(i)->getBelongingIndex()].insert(dung->deck[target_chara.at(i)->getBelongingIndex()].begin(), target_chara.at(i));
                        
                        target_chara.at(i)->setPosition(target_position);
                        
                        
                        //Point target_position = static_cast<DungeonScene*>(getParent())->fieldLayer->field_position_list[target_chara.at(i)->getBelongingIndex()].at(skill_value.at(i)).first;
                        //CCLOG("フィールド%dに移動",skill_value.at(i));
                        MoveTo *move = MoveTo::create(0.4f, dung->fieldLayer->deck_point[target_chara.at(i)->getBelongingIndex()]);
                        target_chara.at(i)->runAction(move);
                    }
                    delay_time = 0.8f;//演出終了までの時間
                    break;
                }
                case 3:{
                    //移動後のコンビネーションエフェクト
                    //DungeonScene* dungeon_scene = static_cast<DungeonScene *>(getParent());
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 77:{//味方総大将にXダメージを与え、士気を１得る
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        ParticleSystemQuad* jokyo = ParticleSystemQuad::create("download/Effect/Particle/jokyo.plist");
                        jokyo->setAutoRemoveOnFinish(true);
                        jokyo->setPosition(CharaList::getLeader(used_index)->getPosition());
                        jokyo->setScale(ratio*1.6f);
                        jokyo->setStartColor(Color4F::RED);
                        jokyo->setEndColor(Color4F::RED);
                        this->addChild(jokyo, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo.mp3");
                        delay_time = 0.5f;//演出終了までの時間
                    }
                    break;
                }
                case 2:{
                    dung->playerHpUpdate(used_index, skill_val.at(0).at(0), dung->player[used_index]->getHp());
                    delay_time = 0.8f;//演出終了までの時間
                    break;
                }
                    
                case 3:{
                    
                    //パーティクル出力
                    ParticleSystemQuad* resorb = ParticleSystemQuad::create("download/Effect/Particle/resorb.plist");
                    resorb->setAutoRemoveOnFinish(true);
                    resorb->setPosition(convertFromDisplaySize(dung->uiLayer->power_point[used_index], 0.5, 0.5));
                    resorb->setScale(ratio);
                    this->addChild(resorb, 1);
                    
                    
                    dung->player[used_index]->generatePower(1);
                    dung->uiLayer->updatePower(dung->player[used_index]->getNowPower(), used_index);
                    
                    
                    
                    delay_time = 0.3f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: PlayerDamageType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        
        case 78:{//対象のアクティブスキルを得る
            switch(effect_count) {
                case 1:{
                    int skill_value = use_chara->getActiveSkillValue();
                    //相手のスキルを自分にコピー
                    use_chara->setActiveSkillId(target_chara.at(0)->getActiveSkillId());
                    use_chara->setActiveSkillName(target_chara.at(0)->getActiveSkillName());
                    use_chara->setActiveSkillTarget(target_chara.at(0)->getActiveSkillTarget());
                    use_chara->setActiveSkillDetail(target_chara.at(0)->getActiveSkillDetail());
                    use_chara->setActiveSkillTurn(target_chara.at(0)->getActiveSkillTurn());
                    use_chara->setActiveSkillValue(target_chara.at(0)->getActiveSkillValue());
                    use_chara->setActiveSkillCost(target_chara.at(0)->getActiveSkillCost());
                    
                    if(skill_value == 2){//奪う場合は消す
                        target_chara.at(0)->setActiveSkillId(0);
                        target_chara.at(0)->setActiveSkillName("");
                        target_chara.at(0)->setActiveSkillTarget("");
                        target_chara.at(0)->setActiveSkillDetail("");
                        target_chara.at(0)->setActiveSkillTurn(0);
                        target_chara.at(0)->setActiveSkillValue(0);
                        target_chara.at(0)->setActiveSkillCost(0);
                    }
                    
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        float ratio2 = ratio;
                        Chara *chara = target_chara.at(0);
                        
                        ParticleSystemQuad *p_break = ParticleSystemQuad::create("download/Effect/Particle/passive_break.plist");
                        p_break->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/passive_break.png"));
                        p_break->setAutoRemoveOnFinish(true);
                        p_break->setPosition(chara->getPosition());
                        p_break->setScale(ratio2);
                        p_break->setDuration(1.6f);
                        this->addChild(p_break, 1);
                        
                        runAction(Sequence::create(DelayTime::create(0.3f), CallFunc::create([this,chara,ratio2](){
                            ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                            hit1->setAutoRemoveOnFinish(true);
                            hit1->setPosition(chara->getPosition());
                            hit1->setScale(ratio2);
                            this->addChild(hit1, 1);
                        }),NULL));
                        
                        p_break->runAction(Sequence::create(DelayTime::create(0.3f), MoveTo::create(0.6f, use_chara->getPosition()),NULL));
                        
                        
                        delay_time = 1.4f;//演出終了までの時間
                    }
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 79:{//対象とアクティブスキルを交換する
            switch(effect_count) {
                case 1:{
                    //自分のスキルを保存
                     int this_skill_id = use_chara->getActiveSkillId();
                     string this_skill_name = use_chara->getActiveSkillName();
                     string this_skill_target = use_chara->getActiveSkillTarget();
                     string this_skill_detail = use_chara->getActiveSkillDetail();
                     int this_skill_turn = use_chara->getActiveSkillTurn();
                     int this_skill_value = use_chara->getActiveSkillValue();
                     int this_skill_cost = use_chara->getActiveSkillCost();
                    
                    //相手のスキルを自分にコピー
                    use_chara->setActiveSkillId(target_chara.at(0)->getActiveSkillId());
                    use_chara->setActiveSkillName(target_chara.at(0)->getActiveSkillName());
                    use_chara->setActiveSkillTarget(target_chara.at(0)->getActiveSkillTarget());
                    use_chara->setActiveSkillDetail(target_chara.at(0)->getActiveSkillDetail());
                    use_chara->setActiveSkillTurn(target_chara.at(0)->getActiveSkillTurn());
                    use_chara->setActiveSkillValue(target_chara.at(0)->getActiveSkillValue());
                    use_chara->setActiveSkillCost(target_chara.at(0)->getActiveSkillCost());
                    
                    //自分のスキルを相手にコピー
                    target_chara.at(0)->setActiveSkillId(this_skill_id);
                    target_chara.at(0)->setActiveSkillName(this_skill_name);
                    target_chara.at(0)->setActiveSkillTarget(this_skill_target);
                    target_chara.at(0)->setActiveSkillDetail(this_skill_detail);
                    target_chara.at(0)->setActiveSkillTurn(this_skill_turn);
                    target_chara.at(0)->setActiveSkillValue(this_skill_value);
                    target_chara.at(0)->setActiveSkillCost(this_skill_cost);
                    
                    if(effect_onoff_flg) {
                        //パーティクル出力
                        float ratio2 = ratio;
                        Chara *chara = target_chara.at(0);
                        
                        if(skill_val.at(0).at(0)==1){//クリスマス仕様
                            
                            auto sprite1 = Sprite::create("download/Dungeon/present.png");
                            sprite1->setScale(ratio2/2);
                            sprite1->setPosition(chara->getPosition());
                            this->addChild(sprite1, 3);
                            sprite1->runAction(EaseElasticOut::create(ScaleTo::create(0.5, ratio2),0.3f));
                            
                            
                            auto sprite2 = Sprite::create("download/Dungeon/present.png");
                            sprite2->setScale(ratio2/2);
                            sprite2->setPosition(use_chara->getPosition());
                            this->addChild(sprite2, 3);
                            sprite2->runAction(EaseElasticOut::create(ScaleTo::create(0.5, ratio2),0.3f));
                            
                            
                            ParticleSystemQuad *p_break1 = ParticleSystemQuad::create("download/Effect/Particle/star_dust.plist");
                            p_break1->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/star_dust.png"));
                            p_break1->setAutoRemoveOnFinish(true);
                            p_break1->setPosition(chara->getPosition());
                            p_break1->setScale(ratio2*3/5);
                            p_break1->setDuration(2.2f);
                            this->addChild(p_break1, 1);
                            ParticleSystemQuad *p_break1_2 = ParticleSystemQuad::create("download/Effect/Particle/star_cloud.plist");
                            p_break1_2->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/star_cloud.png"));
                            p_break1_2->setAutoRemoveOnFinish(true);
                            p_break1_2->setPosition(chara->getPosition());
                            p_break1_2->setScale(ratio2*3/5);
                            p_break1_2->setDuration(2.2f);
                            this->addChild(p_break1_2, 1);
                            
                            
                            ParticleSystemQuad *p_break2 = ParticleSystemQuad::create("download/Effect/Particle/star_dust.plist");
                            p_break2->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/star_dust.png"));
                            p_break2->setAutoRemoveOnFinish(true);
                            p_break2->setPosition(use_chara->getPosition());
                            p_break2->setScale(ratio2*3/5);
                            p_break2->setDuration(2.2f);
                            this->addChild(p_break2, 1);
                            ParticleSystemQuad *p_break2_2 = ParticleSystemQuad::create("download/Effect/Particle/star_cloud.plist");
                            p_break2_2->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/star_cloud.png"));
                            p_break2_2->setAutoRemoveOnFinish(true);
                            p_break2_2->setPosition(use_chara->getPosition());
                            p_break2_2->setScale(ratio2*3/5);
                            p_break2_2->setDuration(2.2f);
                            this->addChild(p_break2_2, 1);
                            
                            
                            
                            runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create([this,chara,ratio2](){
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(chara->getPosition());
                                hit1->setScale(ratio2);
                                this->addChild(hit1, 1);
                            }),NULL));
                            
                            runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create([this,ratio2](){
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(use_chara->getPosition());
                                hit1->setScale(ratio2);
                                this->addChild(hit1, 1);
                            }),NULL));
                            
                            float dis = 130.0f*ratio2;//中心線から離れる距離
                            float sin_1 = (chara->getPositionY()-use_chara->getPositionY()) / (chara->getPosition().distance(use_chara->getPosition()));
                            float cos_1 = (chara->getPositionX()-use_chara->getPositionX()) / (chara->getPosition().distance(use_chara->getPosition()));
                            
                            Vec2 move_vec1 = Vec2(-dis*sin_1, dis*cos_1);
                            Vec2 move_vec2 = Vec2(dis*sin_1, -dis*cos_1);
                            
                            /*auto around = Sequence::create(MoveTo::create(0.3f, use_chara->getPosition()+(chara->getPosition()-use_chara->getPosition())/2+move_vec), MoveTo::create(0.3f, use_chara->getPosition()), NULL);
                             auto around2 = Sequence::create(MoveTo::create(0.3f, use_chara->getPosition()+(chara->getPosition()-use_chara->getPosition())/2+move_vec2), MoveTo::create(0.3f, chara->getPosition()), NULL);
                             */
                            
                            auto around1 = Sequence::create(EaseSineOut::create(MoveBy::create(0.4f, move_vec1)), EaseSineIn::create(MoveBy::create(0.4f, -move_vec1)), NULL);
                            auto around2 = Sequence::create(EaseSineOut::create(MoveBy::create(0.4f, move_vec2)), EaseSineIn::create(MoveBy::create(0.4f, -move_vec2)), NULL);
                            
                            auto action1 = Sequence::create(DelayTime::create(0.6f), Spawn::create(EaseSineOut::create(MoveBy::create(0.8f,  use_chara->getPosition()-chara->getPosition())), around1, NULL),NULL);
                            auto action2 = Sequence::create(DelayTime::create(0.6f), Spawn ::create(EaseSineOut::create(MoveBy::create(0.8f,  chara->getPosition()-use_chara->getPosition())), around2, NULL),NULL);
                            
                            p_break1->runAction(action1);
                            p_break1_2->runAction(action1->clone());
                            sprite1->runAction(Sequence::create(action1->clone(), RemoveSelf::create(),NULL) );

                            
                            p_break2->runAction(action2);
                            p_break2_2->runAction(action2->clone());
                            sprite2->runAction(Sequence::create(action2->clone(), RemoveSelf::create(),NULL) );
                            
                            delay_time = 2.0f;//演出終了までの時間
                            
                        }else{
                            
                            ParticleSystemQuad *p_break = ParticleSystemQuad::create("download/Effect/Particle/passive_break.plist");
                            p_break->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/passive_break.png"));
                            p_break->setAutoRemoveOnFinish(true);
                            p_break->setPosition(chara->getPosition());
                            p_break->setScale(ratio2);
                            p_break->setDuration(1.6f);
                            this->addChild(p_break, 1);
                            
                            ParticleSystemQuad *p_break2 = ParticleSystemQuad::create("download/Effect/Particle/passive_break.plist");
                            p_break2->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/passive_break.png"));
                            p_break2->setAutoRemoveOnFinish(true);
                            p_break2->setPosition(use_chara->getPosition());
                            p_break2->setScale(ratio2);
                            p_break2->setDuration(1.6f);
                            this->addChild(p_break2, 1);
                            
                            runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create([this,chara,ratio2](){
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(chara->getPosition());
                                hit1->setScale(ratio2);
                                this->addChild(hit1, 1);
                            }),NULL));
                            
                            runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create([this,ratio2](){
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(use_chara->getPosition());
                                hit1->setScale(ratio2);
                                this->addChild(hit1, 1);
                            }),NULL));
                            
                            float dis = 100.0f*ratio2;//中心線から離れる距離
                            float sin_1 = (chara->getPositionY()-use_chara->getPositionY()) / (chara->getPosition().distance(use_chara->getPosition()));
                            float cos_1 = (chara->getPositionX()-use_chara->getPositionX()) / (chara->getPosition().distance(use_chara->getPosition()));
                            
                            Vec2 move_vec = Vec2(-dis*sin_1, dis*cos_1);
                            Vec2 move_vec2 = Vec2(dis*sin_1, -dis*cos_1);
                            
                            /*auto around = Sequence::create(MoveTo::create(0.3f, use_chara->getPosition()+(chara->getPosition()-use_chara->getPosition())/2+move_vec), MoveTo::create(0.3f, use_chara->getPosition()), NULL);
                             auto around2 = Sequence::create(MoveTo::create(0.3f, use_chara->getPosition()+(chara->getPosition()-use_chara->getPosition())/2+move_vec2), MoveTo::create(0.3f, chara->getPosition()), NULL);
                             */
                            
                            auto around = Sequence::create(MoveBy::create(0.3f, move_vec), MoveBy::create(0.3f, -move_vec), NULL);
                            auto around2 = Sequence::create(MoveBy::create(0.3f, move_vec2), MoveBy::create(0.3f, -move_vec2), NULL);
                            
                            p_break->runAction(Sequence::create(DelayTime::create(0.3f), Spawn::create(MoveBy::create(0.6f,  use_chara->getPosition()-chara->getPosition()), around, NULL),NULL));
                            p_break2->runAction(Sequence::create(DelayTime::create(0.3f), Spawn ::create(MoveBy::create(0.6f,  chara->getPosition()-use_chara->getPosition()), around2, NULL),NULL));
                            
                            delay_time = 1.4f;//演出終了までの時間
                        }
                    }
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
        
        case 80:{//雪だるま召喚
            switch(effect_count) {
                case 1:{
                    
                    if(effect_onoff_flg) {
                        ParticleSystemQuad *p_break = ParticleSystemQuad::create("download/Effect/Particle/snow.plist");
                        p_break->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/snow.png"));
                        p_break->setAutoRemoveOnFinish(true);
                        p_break->setPosition(dung->fieldLayer->field_position_list[skill_val.at(0).at(0)].at(skill_val.at(0).at(1)).x, dung->fieldLayer->field_position_list[skill_val.at(0).at(0)].at(skill_val.at(0).at(1)).y - 47*ratio);
                        p_break->setScale(ratio*5/9);
                        p_break->setDuration(1.8f);
                        this->addChild(p_break, 1);
                        
                        delay_time = 0.9f;//演出終了までの時間
                    }
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 81:{//おみくじ
            switch(effect_count) {
                case 1:{
                    int omi_result = skill_val.at(0).at(0);//おみくじ結果１−４
                    auto node = Node::create();
                    node->setScale(0.3);
                    node->setPosition(use_chara->getPosition());
                    node->setRotation(90.0f);
                    node->setCascadeOpacityEnabled(true);
                    addChild(node,300);
                    auto omi_spr = Sprite::create("download/Dungeon/omikuji0.png");
                    omi_spr->setScale(ratio);
                    node->addChild(omi_spr,5);
                    auto omi_hand = Sprite::create("download/Dungeon/omikuji5.png");
                    omi_hand->setPosition(-40*ratio,0);
                    omi_hand->setScale(ratio);
                    node->addChild(omi_hand,10);
                    auto omi_paper = Sprite::create("download/Dungeon/omikuji"+to_string(omi_result)+".png");
                    omi_paper->setPosition(-40*ratio,0);
                    omi_paper->setScale(ratio);
                    //node->addChild(omi_paper,8);
                    
                    auto mask = Sprite::create();
                    mask->setTextureRect(Rect(-50,-50,120,100));
                    mask->setPosition(Point(50*ratio,0));
                    mask->setColor(Color3B::WHITE);
                    mask->setScale(ratio);
                    
                    auto clip = ClippingNode::create();
                    //clip->setScale(ratio);
                    clip->setCascadeOpacityEnabled(true);
                    clip->setAlphaThreshold(0);
                    clip->setPosition(0,0);
                    node->addChild(clip);
                    
                    clip->setStencil(mask);
                    clip->addChild(omi_paper);
                    
                    
                    
                    
                    
                    node->runAction(Sequence::create(DelayTime::create(0.2f),EaseSineOut::create(Spawn::create(RotateTo::create(0.5f,0.0f),ScaleTo::create(0.5f,1.0f),MoveTo::create(0.5f, use_chara->getPosition()+Point(-50*ratio,120*ratio)), NULL)), NULL));
                    node->runAction(RepeatForever::create(Sequence::create(EaseSineInOut::create(MoveBy::create(1.4f, Point(0,4*ratio))),EaseSineInOut::create( MoveBy::create(1.4f, Point(0,-4*ratio))), NULL)));
                    node->runAction(Sequence::create(DelayTime::create(3.0f),FadeTo::create(0.5f, 0),RemoveSelf::create(), NULL));
                    
                    auto combo_act = Sequence::create(DelayTime::create(1.6f),EaseSineOut::create(MoveBy::create(0.4f, Point(96*ratio,0))), NULL);
                    omi_paper->runAction(combo_act);
                    omi_hand->runAction(combo_act->clone());
                    
                    if(omi_result == 1){//大吉
                        if(effect_onoff_flg) {
                            
                            //戦闘力アップエフェクト
                            runAction(Sequence::create(DelayTime::create(2.2f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(use_chara->getPosition());
                                kiryoku->setScale(ratio);
                                this->addChild(kiryoku, 2);
                                
                            
                                ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                                kiryoku_plus->setAutoRemoveOnFinish(true);
                                kiryoku_plus->setPosition(use_chara->getPosition());
                                kiryoku_plus->setScale(ratio);
                                this->addChild(kiryoku_plus, 2);
                                
                                //SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                            }),NULL));
                            
                            //貫通エフェクト
                            runAction(Sequence::create(DelayTime::create(2.2f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kantu = ParticleSystemQuad::create("download/Effect/Particle/kantu2.plist");
                                kantu->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/kantu2.png"));
                                kantu->setAutoRemoveOnFinish(true);
                                kantu->setDuration(0.5f);
                                kantu->setPosition(use_chara->getPosition());
                                kantu->setScale(ratio/2);
                                this->addChild(kantu, 1);
                            }),NULL));
                            
                            //低下無効エフェクト
                            runAction(Sequence::create(DelayTime::create(2.2f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kantu = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                                kantu->setAutoRemoveOnFinish(true);
                                kantu->setStartColor(Color4F(Color4B(255,255,0,255)));
                                kantu->setEndColor(Color4F(Color4B(255,255,0,100)));
                                kantu->setPosition(use_chara->getPosition());
                                kantu->setScale(ratio*0.35f);
                                this->addChild(kantu, 3);
                            }),NULL));
                            
                            //除去回避エフェクト
                            runAction(Sequence::create(DelayTime::create(2.6f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                                kaihuku->setAutoRemoveOnFinish(true);
                                kaihuku->setPosition(use_chara->getPosition()+Point(-35*ratio,35*ratio));
                                kaihuku->setScale(ratio*0.35f);
                                this->addChild(kaihuku, 3);
                            }),NULL));
                            
                            //行動不能無効エフェクト
                            runAction(Sequence::create(DelayTime::create(2.4f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                                kaihuku->setAutoRemoveOnFinish(true);
                                kaihuku->setStartColor(Color4F(Color4B(255,140,0,255)));
                                kaihuku->setEndColor(Color4F(Color4B(255,140,0,100)));
                                kaihuku->setPosition(use_chara->getPosition()+Point(35*ratio,-35*ratio));
                                kaihuku->setScale(ratio*0.35f);
                                this->addChild(kaihuku, 3);
                            }),NULL));
                            
                            runAction(Sequence::create(DelayTime::create(2.4f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/type_change.plist");
                                kiryoku->setStartColor(Color4F(Color4B(25,135,22,255)));
                                kiryoku->setEndColor(Color4F(Color4B(25,135,22,100)));
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(use_chara->getPosition()+Point(-25*ratio,-25*ratio));
                                kiryoku->setScale(ratio*3/4);
                                this->addChild(kiryoku, 2);
                            }),NULL));
                            
                            runAction(Sequence::create(DelayTime::create(2.4f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/type_change.plist");
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(use_chara->getPosition()+Point(25*ratio,25*ratio));
                                kiryoku->setScale(ratio*3/4);
                                this->addChild(kiryoku, 2);
                            }),NULL));
                        }
                        
                        runAction(Sequence::create(DelayTime::create(2.5f), CallFunc::create([&,omi_result,dung](){
                            //戦闘力アップ
                            map<string,int> temp;
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 33));
                            temp.insert( map<string, int>::value_type("value", use_chara->getActiveSkillValue()));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            use_chara->chara_status_active.push_back(temp);
                            
                            //貫通
                            temp.clear();
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 19));
                            temp.insert( map<string, int>::value_type("value", 1));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_SPEAR));
                            use_chara->chara_status_active.push_back(temp);
                            
                            //低下無効
                            temp.clear();
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 84));
                            temp.insert( map<string, int>::value_type("value", 1));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_INVALID_DECREASE));
                            use_chara->chara_status_active.push_back(temp);
                            
                            //除去回避
                            temp.clear();
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 72));
                            temp.insert( map<string, int>::value_type("value", 1));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_REMOVE_GUARD));
                            use_chara->chara_status_active.push_back(temp);
                            
                            //行動不能無効
                            temp.clear();
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 71));
                            temp.insert( map<string, int>::value_type("value", 1));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_CANT_ACTION_GUARD));
                            use_chara->chara_status_active.push_back(temp);
                            
                        }),NULL));
                        
                        delay_time = 2.7f;//演出終了までの時間
                        
                    }else if(omi_result == 2){//中吉
                        
                        if(effect_onoff_flg) {
                            
                            //戦闘力アップエフェクト
                            runAction(Sequence::create(DelayTime::create(2.0f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(use_chara->getPosition());
                                kiryoku->setScale(ratio);
                                this->addChild(kiryoku, 2);
                                
                                ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                                kiryoku_plus->setAutoRemoveOnFinish(true);
                                kiryoku_plus->setPosition(use_chara->getPosition());
                                kiryoku_plus->setScale(ratio);
                                this->addChild(kiryoku_plus, 2);
                                
                                //SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                            }),NULL));
                            
                            //貫通エフェクト
                            runAction(Sequence::create(DelayTime::create(2.2f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kantu = ParticleSystemQuad::create("download/Effect/Particle/kantu2.plist");
                                kantu->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/kantu2.png"));
                                kantu->setAutoRemoveOnFinish(true);
                                kantu->setDuration(0.5f);
                                kantu->setPosition(use_chara->getPosition());
                                kantu->setScale(ratio/2);
                                this->addChild(kantu, 1);
                            }),NULL));
                            
                            //低下無効エフェクト
                            runAction(Sequence::create(DelayTime::create(2.0f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kantu = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                                kantu->setAutoRemoveOnFinish(true);
                                kantu->setStartColor(Color4F(Color4B(255,255,0,255)));
                                kantu->setEndColor(Color4F(Color4B(255,255,0,100)));
                                kantu->setPosition(use_chara->getPosition());
                                kantu->setScale(ratio*0.35f);
                                this->addChild(kantu, 3);
                            }),NULL));
                            
                            runAction(Sequence::create(DelayTime::create(2.2f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/type_change.plist");
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(use_chara->getPosition());
                                kiryoku->setScale(ratio*3/4);
                                this->addChild(kiryoku, 2);
                            }),NULL));
                        }
                        
                        runAction(Sequence::create(DelayTime::create(2.4f), CallFunc::create([&,omi_result,dung](){
                            //戦闘力アップ
                            map<string,int> temp;
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 33));
                            temp.insert( map<string, int>::value_type("value", use_chara->getActiveSkillValue()));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            use_chara->chara_status_active.push_back(temp);
                            
                            //貫通
                            temp.clear();
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 19));
                            temp.insert( map<string, int>::value_type("value", 1));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_SPEAR));
                            use_chara->chara_status_active.push_back(temp);
                            
                            //低下無効
                            temp.clear();
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 84));
                            temp.insert( map<string, int>::value_type("value", 1));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_INVALID_DECREASE));
                            use_chara->chara_status_active.push_back(temp);
                            
                        }),NULL));
                        delay_time = 2.5f;//演出終了までの時間
                    }else if(omi_result == 3){//小吉
                        
                        if(effect_onoff_flg) {
                            
                            //戦闘力アップエフェクト
                            runAction(Sequence::create(DelayTime::create(2.0f), CallFunc::create([&, omi_result ,dung](){
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(use_chara->getPosition()/*+Point(35*ratio,35*ratio)*/);
                                kiryoku->setScale(ratio);
                                this->addChild(kiryoku, 2);
                                
                                ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                                kiryoku_plus->setAutoRemoveOnFinish(true);
                                kiryoku_plus->setPosition(use_chara->getPosition()/*+Point(35*ratio,35*ratio)*/);
                                kiryoku_plus->setScale(ratio);
                                this->addChild(kiryoku_plus, 2);
                                
                                //SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                            }),NULL));
                        }
                        runAction(Sequence::create(DelayTime::create(2.2f), CallFunc::create([&, omi_result ,dung](){
                            //戦闘力アップ
                            map<string,int> temp;
                            temp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            temp.insert( map<string, int>::value_type("skill_id", 33));
                            temp.insert( map<string, int>::value_type("value", use_chara->getActiveSkillValue()));
                            temp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            use_chara->chara_status_active.push_back(temp);
                            
                        }),NULL));
                        delay_time = 2.5f;//演出終了までの時間
                        
                    }else{//凶
                        if(effect_onoff_flg) {
                            
                            runAction(Sequence::create(DelayTime::create(2.3f), CallFunc::create([&, omi_result ,dung](){
                                //爆発エフェクト
                                ParticleSystemQuad* jokyo_success = ParticleSystemQuad::create("download/Effect/Particle/jokyo_success.plist");
                                jokyo_success->setAutoRemoveOnFinish(true);
                                jokyo_success->setPosition(use_chara->getPosition());
                                jokyo_success->setScale(ratio);
                                this->addChild(jokyo_success, 1);
                                
                                //SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                                
                                //SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo_success.mp3");
                                
                            }),NULL));
                        }
                        delay_time = 2.5f;//演出終了までの時間
                    }
                    
                    break;
                }
                    
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 82:{//対象地点にサル召喚
            switch(effect_count) {
                case 1:{
                    if(skill_val.empty() == false){
                        int saru_rare = skill_val.at(0).at(2);
                        int saru_field = skill_val.at(0).at(1);
                        
                        if(effect_onoff_flg){
                            //パーティクル出力
                            ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
                            hunou->setAutoRemoveOnFinish(true);
                            hunou->setPosition(dung->fieldLayer->field_position_list[use_chara->getBelongingIndex()].at(saru_field));
                            hunou->setScale(ratio);
                            hunou->setStartColor(Color4F(Color4B(255,140,0,255)));
                            hunou->setEndColor(Color4F(Color4B(255,140,0,0)));
                            this->addChild(hunou, 1);
                            float ratio2 = ratio;
                            runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([this,ratio2,saru_field,dung](){
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(dung->fieldLayer->field_position_list[use_chara->getBelongingIndex()].at(saru_field));
                                hit1->setScale(ratio2);
                                this->addChild(hit1, 1);
                            }),NULL));
                        }
                
                        runAction(Sequence::create(DelayTime::create(0.3f), CallFunc::create([&, saru_rare, saru_field ,dung](){
                            
                            Chara *chara = Chara::create();
                            dung->cardLayer->addChild(chara);
                            
                            chara->setCharaId(99999);//サルのID
                            chara->setPlaceCost(use_chara->getActiveSkillCost());
                            chara->setBaseValue(use_chara->getActiveSkillValue()*saru_rare);
                            chara->setCharaTurnCount(0);
                            chara->setCharaTurnCountDef(0);
                            chara->setType(CHARA_TYPE_ATTACK);
                            chara->setCharaName("サル");
                            chara->setCharaSecondName("");
                            chara->setRare(saru_rare);
                            chara->active_skill_able_count = use_chara->active_skill_able_count;
                            
                            chara->setCombinationL(1);//赤
                            chara->setCombinationR(3);//緑
                            
                            chara->place_type = use_chara->place_type;
                            chara->place_priority = use_chara->place_priority;
                            chara->skill_priority = use_chara->skill_priority;
                            
                            CharaList::chara_list.insert(chara);
                            chara->setBelongingIndex(use_chara->getBelongingIndex());
                            chara->initialize();
                            chara->setPosition(dung->fieldLayer->field_position_list[use_chara->getBelongingIndex()].at(saru_field));
                            chara->setFieldIndex(saru_field);
                            chara->showCardParam();
                            chara->eliminate_flg = true;//死んだ時にデッキに戻さずに消去するフラグ
                            chara->setScale(0.0f, 1.0f);
                            chara->runAction(EaseSineInOut::create(ScaleTo::create(0.7f, 1.0f, 1.0f)));
                        }),NULL));
                    }
                    delay_time = 1.2f;
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 83:{//デバフ解除＋行動不能無効を付与＋戦闘力UP
            switch(effect_count) {
                case 1:{
                    for(int i=0; i<target_chara.size(); i++) {
                        if(effect_onoff_flg) {
                            auto hama_node = Node::create();
                            hama_node->setScale(ratio/2);
                            hama_node->setPosition(target_chara.at(i)->getPosition() +Point(0,125*ratio));
                            hama_node->setOpacity(50);    //最初は透明
                            hama_node->setCascadeOpacityEnabled(true);
                            this->addChild(hama_node, 2);
                            
                            hama_node->runAction(RepeatForever::create(Sequence::create(EaseSineInOut::create(MoveBy::create(1.4f, Point(0,4*ratio))),EaseSineInOut::create( MoveBy::create(1.4f, Point(0,-4*ratio))), NULL)));
                            
                            //破魔矢画像の追加・初期はサイズ0、透明、位置はキャラの頭上
                            auto hamayaSprite = Sprite::create("download/Effect/Sprite/hamaya_sprite.png");
                            hama_node->addChild(hamayaSprite, 2);
                            
                            //アニメーション1・徐々に表示しながら拡大
                            auto *fadeToAction_1 = FadeTo::create(0.2f, 255);
                            auto *fade_ease_1 = EaseSineInOut::create(fadeToAction_1);
                            ScaleTo *scale1 = ScaleTo::create(0.2, ratio * 1.20f);
                            EaseIn *ease_1 = EaseIn::create(scale1, 2);
                            Spawn *animation_1 =Spawn::createWithTwoActions(fade_ease_1, ease_1);
                            
                            //一定時間待機
                            DelayTime *delay_1 = DelayTime::create(1.0f);
                            
                            //Particleの表示
                            ParticleSystemQuad* hamayaParticle = ParticleSystemQuad::create("download/Effect/Particle/hamaya_particle.plist");
                            hamayaParticle->setAutoRemoveOnFinish(true);
                            hamayaParticle->setPosition(0,0);
                            hamayaParticle->setStartColor(Color4F(Color4B(255,255,76,220)));
                            hamayaParticle->setPositionType(ParticleSystem::PositionType::RELATIVE);
                            hamayaParticle->setScale(0.7f);
                            hamayaParticle->setBlendAdditive(true);
                            hama_node->addChild(hamayaParticle, 1);
                            
                            
                            //アニメーション2・縮小しながら移動、フェードアウト(同時実行)
                            auto *fadeOutAction_2 = FadeTo::create(0.6, 30);
                            auto *fade_ease_2 = EaseSineInOut::create(fadeOutAction_2);
                            MoveTo *move_to_2 = MoveTo::create(0.6, target_chara.at(i)->getPosition());
                            auto ease3 = EaseIn::create(move_to_2, 2.0f);
                            ScaleTo *scale2 = ScaleTo::create(0.6, ratio/2);
                            EaseIn *ease_2 = EaseIn::create(scale2, 2);
                            auto animation_2 = Spawn::create(fade_ease_2, ease_2, ease3, NULL);
                            
                            //アニメーションの実行＋画像の削除
                            Sequence *seq = Sequence::create(animation_1,delay_1,animation_2,RemoveSelf::create(), nullptr);//removeSelfの使い方間違ってました
                            hama_node->runAction(seq);
                            
                            
                            //戦闘力アップエフェクト
                            runAction(Sequence::create(DelayTime::create(1.7f), CallFunc::create([& ,dung ,i](){//コピーのiを入れる
                                //パーティクル出力
                                ParticleSystemQuad* wave = ParticleSystemQuad::create("download/Effect/Particle/wave.plist");
                                wave->setAutoRemoveOnFinish(true);
                                wave->setPosition(target_chara.at(i)->getPosition());
                                wave->setScale(ratio/2);
                                wave->setOpacity(150);
                                this->addChild(wave, 1);
                                
                                
                                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                                kiryoku->setAutoRemoveOnFinish(true);
                                kiryoku->setPosition(target_chara.at(i)->getPosition());
                                kiryoku->setScale(ratio);
                                this->addChild(kiryoku, 2);
                                
                                ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                                kiryoku_plus->setAutoRemoveOnFinish(true);
                                kiryoku_plus->setPosition(target_chara.at(i)->getPosition());
                                kiryoku_plus->setScale(ratio);
                                this->addChild(kiryoku_plus, 2);
                                
                                //SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                            }),NULL));
                        }
                            
                        runAction(Sequence::create(DelayTime::create(1.7f), CallFunc::create([& ,dung ,i](){//コピーのiを入れる
                            //ステータスの更新
                            //対象のステータスバフを全て消去
                            target_chara.at(i)->chara_status_active.clear();//すべて消去
                            
                            //行動不能無効を付加
                            map<string,int> tempTargetGuard;
                            tempTargetGuard.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            tempTargetGuard.insert( map<string, int>::value_type("skill_id", 71));
                            tempTargetGuard.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            tempTargetGuard.insert( map<string, int>::value_type("buffer_type", STATUS_BUFFER_TYPE_DEBUFF));
                            tempTargetGuard.insert( map<string, int>::value_type("state", STATUS_TYPE_CANT_ACTION_GUARD));
                            target_chara.at(i)->chara_status_active.push_back(tempTargetGuard);
                            
                            //戦闘力UPを付与
                            map<string,int> tempTargetStatusUp;
                            tempTargetStatusUp.insert( map<string, int>::value_type("primary", use_chara->getPrimaryId()));
                            tempTargetStatusUp.insert( map<string, int>::value_type("skill_id", 33));//固定値アップ
                            tempTargetStatusUp.insert( map<string, int>::value_type("turn", use_chara->getActiveSkillTurn()*2));
                            tempTargetStatusUp.insert( map<string, int>::value_type("buffer_type", STATUS_BUFFER_TYPE_DEBUFF));
                            tempTargetStatusUp.insert( map<string, int>::value_type("value", use_chara->getActiveSkillValue()));
                            target_chara.at(i)->chara_status_active.push_back(tempTargetStatusUp);
                            
                        }),NULL));
                    }
                    //SE
                    //CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                    delay_time = 1.9f;//演出終了までの時間
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }

        case 84:{//選択対象低下無効
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            ParticleSystemQuad* kantu = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                            kantu->setAutoRemoveOnFinish(true);
                            kantu->setStartColor(Color4F(Color4B(255,255,0,255)));
                            kantu->setEndColor(Color4F(Color4B(255,255,0,100)));
                            kantu->setPosition(target_chara.at(i)->getPosition());
                            kantu->setScale(ratio*0.4f);
                            this->addChild(kantu, 3);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 85:{//不屈（攻撃を受けても破壊されず行動不能に）を付与
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++){
                            ParticleSystemQuad* kantu = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                            kantu->setAutoRemoveOnFinish(true);
                            kantu->setStartColor(Color4F(Color4B(255,76,116,255)));
                            kantu->setEndColor(Color4F(Color4B(255,76,116,100)));
                            kantu->setPosition(target_chara.at(i)->getPosition());
                            kantu->setScale(ratio*0.4f);
                            this->addChild(kantu, 3);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                    }
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: RecoveryType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
        case 86:{//ためる
            if(effect_onoff_flg) {
                switch(effect_count) {
                    case 1:{
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/accumulation.plist");
                            kiryoku->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/accumulation.png"));
                            kiryoku->setAutoRemoveOnFinish(true);
                            kiryoku->setPosition(target_chara.at(i)->getPosition());
                            kiryoku->setScale(ratio*2/5);
                            this->addChild(kiryoku, 1);
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        delay_time = 1.3f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                        break;
                    }
                    default:
                        HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                        finish_flg = true;//エラーにより演出終了
                        break;
                }
            } else {
                finish_flg = true;//最後の演出終了フラグ
            }
            break;
        }
            
        case 87:{//誘惑状態の対象を味方にし、誘惑状態を解除する
            switch(effect_count) {
                case 1:{
                    for(int i=0; i<target_chara.size(); i++) {
                        Chara *use_chara2 = use_chara;
                        Chara *target = target_chara.at(i);
                        
                        auto pika = ParticleSystemQuad::create("download/Effect/Particle/love2.plist");
                        pika->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/love.png"));
                        pika->setAutoRemoveOnFinish(true);
                        pika->setPosition(target->getPosition());
                        pika->setScale(ratio*1.2f);
                        pika->setDuration(1.9f);
                        pika->setPositionType(ParticleSystem::PositionType::GROUPED);
                        this->addChild(pika, 1000);
                        
                        runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create([& ,dung, use_chara2,target,pika](){
                            int field = use_chara2->getNearBlankFieldIndex();
                            HmsVector< map<string, int> > active_temp = target->chara_status_active;//アクティブバッファ保存
                            HmsVector< map<string, int> > passive_temp = target->chara_status_passive;//パッシブバッファ保存
                            
                            auto itr = active_temp.begin();
                            while (itr!=active_temp.end()) {
                                if((*itr)["state"]==STATUS_TYPE_CHARM){//誘惑持っていたら
                                    active_temp.erase(itr);//消す
                                }else{
                                    itr++;
                                }
                            }
                            
                            map<string,int> temp[2];
                            for(int ii=0; ii<2; ii++){//全体パッシブ保存
                                for(int i=0; i < dung->player[ii]->whole_status_passive.size(); i++){
                                    if(dung->player[ii]->whole_status_passive.at(i)["primary"] == target->getPrimaryId()){
                                        temp[ii] = dung->player[ii]->whole_status_passive.at(i);
                                        
                                        break;
                                    }
                                }
                            }
                            
                            target->setBelongingIndex(use_chara2->getBelongingIndex());
                            Point temp_point = target->getPosition();//位置保存
                            target->setDeadFlg(true);
                            target->initialize();
                            
                            target->setPosition(temp_point);
                            target->setFieldIndex(field);
                            target->showCardParam();
                            
                            target->chara_status_active = active_temp;//アクティブバッファ復元
                            target->chara_status_passive = passive_temp;//パッシブバッファ復元
                            
                            for(int ii=0; ii<2; ii++){//全体パッシブ追加
                                if(temp[ii].empty()==false){
                                    dung->player[target->reverseIndex(ii)]->whole_status_passive.push_back(temp[ii]);//反対側のプレイヤーに追加
                                    dung->uiLayer->insertPassiveIcon(target->reverseIndex(ii),target->getPrimaryId(),target->getPassiveSkillId());//アイコン追加
                                }
                            }
                            
                            target->runAction(EaseSineInOut::create(MoveTo::create(0.7f, dung->fieldLayer->field_position_list[target->getBelongingIndex()].at(field))));
                            pika->runAction(EaseSineInOut::create(MoveTo::create(0.7f, dung->fieldLayer->field_position_list[target->getBelongingIndex()].at(field))));
                            
                            
                        }),NULL));
                    }
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                    delay_time = 1.3f;//演出終了までの時間
                    break;
                }
                default:
                    HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            
            break;
        }
            
        case 88:{//氷漬け
            if(effect_onoff_flg) {
                switch(effect_count) {
                    case 1:{
                        for(int i=0; i<target_chara.size(); i++) {
                            if(skill_val.at(i).at(0)>0) {//状態異常成功の時
                                //パーティクル出力
                                ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/freeze.plist");
                                hunou->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/freeze.png"));
                                hunou->setAutoRemoveOnFinish(true);
                                hunou->setPosition(target_chara.at(i)->getPosition());
                                hunou->setDuration(1.2f);
                                hunou->setScale(ratio);
                                this->addChild(hunou, 1);
                                hunou->runAction(ScaleTo::create(1.2f, ratio/2));
                                
                                runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([&](){
                                    if(effect_onoff_flg) {
                                        for(int i=0; i<target_chara.size(); i++) {
                                            //パーティクル出力
                                            ParticleSystemQuad* hunou_success = ParticleSystemQuad::create("download/Effect/Particle/freeze2.plist");
                                            hunou_success->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/freeze2.png"));
                                            hunou_success->setAutoRemoveOnFinish(true);
                                            hunou_success->setPosition(target_chara.at(i)->getPosition());
                                            hunou_success->setScale(ratio*2/3);
                                            this->addChild(hunou_success, 0);
                                        }
                                        //スキル成功SE
                                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/success.mp3");
                                        
                                    }
                                }), NULL));
                                
                                runAction(Sequence::create(DelayTime::create(1.1f),CallFunc::create([&](){
                                    for(int i=0; i<target_chara.size(); i++){
                                        if(use_chara->getPrimaryId() == target_chara.at(i)->getPrimaryId() || target_chara.at(i)->getDeadFlg() == true || target_chara.at(i)->isOnField()==false) continue;
                                        target_chara.at(i)->showCardParam();
                                    }
                                }), NULL));
                                
                            } else {//状態異常失敗の時
                                //missエフェクト
                                missEffect(target_chara.at(i)->getPosition());
                                //スキル失敗SE
                                //CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");
                                delay_time = 1.0f;//演出終了までの時間
                            }
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/remove.mp3");
                        delay_time = 1.2f;//演出終了までの時間
                        finish_flg = true;//最後の演出終了フラグ
                        break;
                    }
                    default:
                        HMSLOG("ActiveSkillEffectError [%d]: CharaPowerUpType", effect_count);
                        finish_flg = true;//エラーにより演出終了
                        break;
                }
            } else {
                finish_flg = true;//最後の演出終了フラグ
            }
            break;
        }
          
        case 89://X以下除去・そうでないならXダウン
        {
            switch(effect_count) {
                case 1:{
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            auto chara = target_chara.at(i);
                            if(skill_val.at(i).at(0)>0) {//除去成功の時
                                
                                //パーティクル出力
                                ParticleSystemQuad* jokyo = ParticleSystemQuad::create("download/Effect/Particle/jokyo.plist");
                                jokyo->setAutoRemoveOnFinish(true);
                                jokyo->setPosition(chara->getPosition());
                                jokyo->setScale(ratio);
                                this->addChild(jokyo, 1);
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo.mp3");
                                
                                runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this,chara](){
                                    ParticleSystemQuad* jokyo_success = ParticleSystemQuad::create("download/Effect/Particle/jokyo_success.plist");
                                    jokyo_success->setAutoRemoveOnFinish(true);
                                    jokyo_success->setPosition(chara->getPosition());
                                    jokyo_success->setScale(ratio);
                                    this->addChild(jokyo_success, 1);
                                }), NULL));
                                
                                delay_time = 2.0f;//演出終了までの時間
                            }else{//低下の時
                                ParticleSystemQuad* status_down = ParticleSystemQuad::create("download/Effect/Particle/status_down.plist");
                                status_down->setAutoRemoveOnFinish(true);
                                status_down->setPosition(target_chara.at(i)->getPosition());
                                status_down->setScale(ratio);
                                this->addChild(status_down, 1);
                                
                                ParticleSystemQuad* status_down_plus = ParticleSystemQuad::create("download/Effect/Particle/status_down_plus.plist");
                                status_down_plus->setAutoRemoveOnFinish(true);
                                status_down_plus->setPosition(target_chara.at(i)->getPosition());
                                status_down_plus->setScale(ratio);
                                this->addChild(status_down_plus, 1);
                                
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/status_down.mp3");
                                delay_time = 1.3f;//演出終了までの時間
                            }
                        }
                        
            
                    
                    }
                    finish_flg = true;//最後の演出終了フラグ
                    break;
                }

                default:
                    HMSLOG("ActiveSkillEffectError [%d]: SkillDeadType", effect_count);
                    finish_flg = true;//エラーにより演出終了
                    break;
            }
            break;
        }
            
            
		default:{//スキルパターンがヒットしなかった場合の保険処理
			delay_time = 0.0f;//演出終了までの時間
			finish_flg = true;//最後の演出終了フラグ
			HMSLOG("error : You do not have the appropriate skills pattern.");
			break;
		}
	}
	
	if(effect_count>99) {
		finish_flg = true;
		HMSLOG("スキル演出が過剰のため強制終了!!");
	}
	effect_count++;//演出回数更新
	
	if(finish_flg) {//演出終了
        for(int ii=0; ii<2; ii++){//総大将の上昇士気の更新
            CharaList::getLeader(ii)->showValue(dung->player[ii]->getPower());
            CCLOG("総大将の士気更新%d",dung->player[ii]->getPower());
        }
        
        bool value_delay_flg = false;
        
        for(Chara *chara : CharaList::getAll()){
            if(chara->getType()!=CHARA_TYPE_LEADER) value_delay_flg = chara->showValue(chara->getValue(),0.3f);
        }
        
        if(value_delay_flg == true) delay_time += 0.3f;
        
        
        this->runAction(Sequence::create(DelayTime::create(delay_time), CallFunc::create([&](){
            static_cast<DungeonScene *>(getParent())->useSkillEnd();
        }), nullptr));
        
    } else {//次の演出に移行
        this->runAction(Sequence::create(DelayTime::create(delay_time),															//演出終了までの遅延処理
                                         CallFunc::create(CC_CALLBACK_0(ActiveSkillEffectLayer::skillEffect,this)), nullptr));	//次の演出実行
    }
}
void ActiveSkillEffectLayer::missEffect(Point position)
{
    ratio = getVisibleRatio();
	float start_posy = position.y - 50;
	float stop_posy = position.y;
	float max_height_posy = position.y + 50;
	float stop_delay = 0.7f;
	float delay = 0.0f;
	
	vector<Sprite*> miss_label;
	vector<float> miss_label_posx;
	
	for(int c=0; c<3; c++) {
		miss_label.push_back(Sprite::create("init/Dungeon/design/miss" + to_string(c+1) + ".png"));
		miss_label.at(c)->setScale(0.7f * ratio);
		if(c==0) miss_label_posx.push_back(position.x - miss_label.at(c)->getContentSize().width*ratio + 20*ratio);
		else if(c==1) miss_label_posx.push_back(position.x);
		else if(c==2) miss_label_posx.push_back(position.x + miss_label.at(c)->getContentSize().width*ratio + 10*ratio);
		miss_label.at(c)->setPosition(Point(miss_label_posx.at(c),start_posy));
		miss_label.at(c)->setOpacity(0);//透明状態にする
		this->addChild(miss_label.at(c), 100);
	}
	
	for(int c=0; c<miss_label_posx.size(); c++) {
		Spawn* up = Spawn::create(MoveTo::create(0.1f, Point(miss_label_posx.at(c), stop_posy)), FadeIn::create(0.1f), nullptr);
		Sequence* move = Sequence::create(MoveTo::create(0.1f, Point(miss_label_posx.at(c), max_height_posy)), MoveTo::create(0.1f, Point(miss_label_posx.at(c), stop_posy)), nullptr);
		Spawn* down = Spawn::create(MoveTo::create(0.2f, Point(miss_label_posx.at(c), start_posy)), FadeOut::create(0.2f), nullptr);
		Sequence* seq = Sequence::create(DelayTime::create(delay), up, move, DelayTime::create(stop_delay - delay), down, nullptr);
        miss_label.at(c)->runAction(Sequence::create(seq, RemoveSelf::create(), nullptr));
		delay += 0.1f;
	}
}


void ActiveSkillEffectLayer::turnSkillEffect(string data_string, int index){//ターン開始時発動スキルエフェクト
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
    //オプションでエフェクトの表示有無を取得
    bool effect_onoff_flg = SaveData::loadBool("effect");
    
    //初期化
    target_chara.clear();
    float finish_time=0.0f;
    string prim_value_string="";
    
    HMSLOG("エフェクトストリング%s",data_string.c_str());
    HmsVector<string> value_data_list = split(data_string,"@");//コンボキャラデータとvalueのデータに分ける
    prim_value_string=value_data_list.at(1);
    
    HmsVector<string> data_list = split(value_data_list.at(0), ",");
    
    Chara *use_chara = CharaList::getAtPrimary(stoi(data_list.at(0)));
    
    //スキルごとの演出
    switch (use_chara->getActiveSkillId()) {
            
        case 86:{//ためる
            if(effect_onoff_flg){
                auto target_chara = CharaList::getAtPrimary(stoi(data_list.at(1)));
                
                //パーティクル出力
                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                kiryoku->setAutoRemoveOnFinish(true);
                kiryoku->setPosition(target_chara->getPosition());
                kiryoku->setScale(ratio);
                this->addChild(kiryoku, 1);
                
                ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                kiryoku_plus->setAutoRemoveOnFinish(true);
                kiryoku_plus->setPosition(target_chara->getPosition());
                kiryoku_plus->setScale(ratio);
                this->addChild(kiryoku_plus, 1);
                
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
            }
            finish_time = 1.0f;
            break;
        }
    }
    
    if(prim_value_string!=""){
        HmsVector<string> prim_value_list= split(prim_value_string,",");
        
        HmsVector<int> chara_primary;
        HmsVector<int> chara_new_value;
        
        for(int i=0;i<prim_value_list.size();i++){
            HmsVector<string> temp =split(prim_value_list.at(i),"&");
            chara_primary.push_back(stoi(temp.at(0)));
            chara_new_value.push_back(stoi(temp.at(1)));
        }
        
        for(int i=0;i<chara_primary.size();i++){
            for(Chara *chara : CharaList::getAll()){
                if(chara->isOnField() && chara->getPrimaryId() == chara_primary.at(i)){
                    chara->showValue(chara_new_value.at(i), 0.2f);//戦闘力の更新
                }
            }
        }
    }
    this->runAction(Sequence::create(DelayTime::create(finish_time),CallFunc::create([&,dung](){//X秒後に演出終了
        dung->turnBufferEffect(0.0f);//次へ
    }), nullptr));
}
