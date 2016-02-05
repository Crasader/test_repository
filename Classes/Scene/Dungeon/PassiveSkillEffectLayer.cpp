
#include "PassiveSkillEffectLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "Effect.h"
#include "SimpleAudioEngine.h"
#include "TurnManage.h"
#include "CharaList.h"
#include "SkillCutInLayer.h"

bool PassiveSkillEffectLayer::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	ratio = getVisibleRatio();
	
	return true;
}

void PassiveSkillEffectLayer::skillEffect(string data_string){
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    dung->passive_effect_data = "";
    
	//初期化
	Chara *use_chara = nullptr;
	target_chara.clear();
	prim_value_string="";
	
	HmsVector<string> value_data_list = split(data_string,"@");//コンボキャラデータとvalueのデータに分ける
	prim_value_string=value_data_list.at(1);
	
	HmsVector<string> data_list = split(value_data_list.at(0), ",");
	
    //スキル使用キャラ・スキル効果キャラ保存
    use_chara = CharaList::getAtPrimary(stoi(data_list.at(0)));

    vector<int> skip_passive = {97,100};//置いた時にエフェクト出ないように
    for(int test : skip_passive){
        if(test == use_chara->getPassiveSkillId()){
            dung->passiveSkillEffect(0.0f);//次へ
            return;
        }
    }
    
    for(int i=1; i<data_list.size(); i++){
        target_chara.push_back(CharaList::getAtPrimary(stoi(data_list.at(i))));
    }
    
    
	//オプションでエフェクトの表示有無を取得
	bool effect_onoff_flg = SaveData::loadBool("effect");
	
	//軍師が場に出た時のエフェクト。とりあえず共通で使う。いずれ差別化を行う。
	//パーティクル出力
	if(effect_onoff_flg){
		ParticleSystemQuad* green_ball = ParticleSystemQuad::create("download/Effect/Particle/green_ball.plist");
		green_ball->setAutoRemoveOnFinish(true);
		green_ball->setPosition(dung->fieldLayer->getCharaPosition(use_chara));
		green_ball->setScale(ratio);
		this->addChild(green_ball, 1);
	
		ParticleSystemQuad* skill_aura_yellow = ParticleSystemQuad::create("download/Effect/Particle/skill_aura_yellow.plist");
		skill_aura_yellow->setAutoRemoveOnFinish(true);
		skill_aura_yellow->setPosition(dung->fieldLayer->getCharaPosition(use_chara));
		skill_aura_yellow->setScale(ratio);
		this->addChild(skill_aura_yellow, 2);
	
		ParticleSystemQuad* white_flash = ParticleSystemQuad::create("download/Effect/Particle/white_flash.plist");
		white_flash->setAutoRemoveOnFinish(true);
		white_flash->setPosition(dung->fieldLayer->getCharaPosition(use_chara));
		white_flash->setScale(ratio);
		this->addChild(white_flash, 3);

		//攻撃SE
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/erase.mp3");
	}
	
    runAction(Sequence::create(DelayTime::create(1.4f), CallFunc::create([&,use_chara](){
        skillEffect2(use_chara);
    }), NULL));
    
    
}

void PassiveSkillEffectLayer::skillEffect2(Chara *use_chara){
	float finish_time=0.5f;
	
    //オプションでエフェクトの表示有無を取得
    bool effect_onoff_flg = SaveData::loadBool("effect");
    
	//オプションでエフェクトオフの場合はエフェクトナンバーを0にする
	int skill_effect_num = use_chara->getPassiveSkillId();
	
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    
	switch (skill_effect_num) {
		case 1:{//戦闘力UP
			if(effect_onoff_flg){
				//パーティクル出力
				for(int i =0;i<target_chara.size();i++){
					ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
					kiryoku->setAutoRemoveOnFinish(true);
					kiryoku->setPosition(target_chara.at(i)->getPosition());
					kiryoku->setScale(ratio);
					this->addChild(kiryoku, 1);
					
					ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
					kiryoku_plus->setAutoRemoveOnFinish(true);
					kiryoku_plus->setPosition(target_chara.at(i)->getPosition());
					kiryoku_plus->setScale(ratio);
					this->addChild(kiryoku_plus, 2);

					//SE
					CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
				}
			}
			
			finish_time=0.5f;
			
			//アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
			break;
		}
		case 2:{//戦闘力down
			for(int i =0;i<target_chara.size();i++){
				if(effect_onoff_flg){
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

					//SE
					CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/status_down.mp3");

				}
			}
			
			//アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
			break;
		}
			
		case 3:{
            
            
            
			break;
		}
		case 4:{//毎ターンダメージ
			//アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
			dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
		case 5:{//毎ターン味方ダメージ
			dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
		case 6:{//毎ターンHP回復
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* kaihuku_passive1 = ParticleSystemQuad::create("download/Effect/Particle/kaihuku_passive1.plist");
				kaihuku_passive1->setAutoRemoveOnFinish(true);
                kaihuku_passive1->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
				kaihuku_passive1->setScale(ratio);
				this->addChild(kaihuku_passive1, 1);
			
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku_passive1.mp3");
			}
			//アイコンの追加
			dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
		case 7:{//毎ターンHP%回復
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* kaihuku_passive2 = ParticleSystemQuad::create("download/Effect/Particle/kaihuku_passive2.plist");
				kaihuku_passive2->setAutoRemoveOnFinish(true);
				kaihuku_passive2->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
				kaihuku_passive2->setScale(ratio);
				this->addChild(kaihuku_passive2, 1);
			
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku_passive2.mp3");
			}
			
			//アイコンの追加
			dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
		case 8:
        case 85:
        {//上昇士気UP
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* siki_up = ParticleSystemQuad::create("download/Effect/Particle/siki_up.plist");
				siki_up->setAutoRemoveOnFinish(true);
				siki_up->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
				siki_up->setScale(ratio);
				this->addChild(siki_up, 1);
			
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/siki_up.mp3");
			}
			//アイコンの追加
			dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
		case 9:{//上昇士気DOWN
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* siki_up = ParticleSystemQuad::create("download/Effect/Particle/siki_down.plist");
				siki_up->setAutoRemoveOnFinish(true);
                if(use_chara->getPassiveSkillTarget() == "own_all") {
                    siki_up->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
                }else{
                    siki_up->setPosition(CharaList::getLeader(reverseIndex(use_chara->getBelongingIndex()))->getPosition());
                }
                siki_up->setScale(ratio);
				this->addChild(siki_up, 1);

				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/siki_down.mp3");
			}
        
            //アイコンの追加
            if(use_chara->getPassiveSkillTarget() == "own_all") {
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }
            
		case 10:{
			break;
		}
		case 11:{
			break;
		}
		case 12:{
			break;
		}
		case 13:{
			break;
		}
		case 14:{
			break;
		}
		case 15:{
			break;
		}
		case 16:{//コンボ威力増
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* combo_up = ParticleSystemQuad::create("download/Effect/Particle/combo_up.plist");
				combo_up->setAutoRemoveOnFinish(true);
				combo_up->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
				combo_up->setScale(ratio);
				this->addChild(combo_up, 1);
			
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/combo_up.mp3");
			}
			
			//アイコンの追加
			dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
		case 17:{//コンボ威力減
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* combo_down = ParticleSystemQuad::create("download/Effect/Particle/combo_down.plist");
				combo_down->setAutoRemoveOnFinish(true);
				combo_down->setPosition(CharaList::getLeader(reverseIndex(use_chara->getBelongingIndex()))->getPosition());
				combo_down->setScale(ratio);
				this->addChild(combo_down, 1);

				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/combo_down.mp3");
			}
			//アイコンの追加
			dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
		case 18:{
			break;
		}
		case 19:{
			break;
		}
		case 20:{
			break;
		}
            
		case 21://戦闘力??以上行動不能
        case 78://コンボカラーXが無いの行動不能
        case 79://コストX以下行動不能
        case 80://コストX以上行動不能
        case 81://レア度X以下行動不能
        case 82://レア度X以上行動不能
        case 86:{//回復効果減少
			if(effect_onoff_flg){
				//パーティクル出力
				for(int i =0;i<target_chara.size();i++){
					ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
					hunou->setAutoRemoveOnFinish(true);
					hunou->setPosition(target_chara.at(i)->getPosition());
					hunou->setScale(ratio);
					this->addChild(hunou, 1);
				}
			
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/hunou.mp3");
			}
			
			//アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
			break;
		}

		case 22:{//自分の場を相手から見えなくする
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* darkness = ParticleSystemQuad::create("download/Effect/Particle/darkness.plist");
				darkness->setAutoRemoveOnFinish(true);
				darkness->setPosition(CharaList::getLeader(use_chara->getBelongingIndex())->getPosition());
				darkness->setScale(ratio);
				this->addChild(darkness, 1);

				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/darkness.mp3");
			}
			
			//アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
			dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
			
			break;
		}
        case 23:{//HP100以下の時　戦闘力UP
            break;
        }
        case 24:{//右、左、左右の武将の戦闘力数値UP
			if(effect_onoff_flg){
				//パーティクル出力
				for(int i =0;i<target_chara.size();i++){
					ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
					kiryoku->setAutoRemoveOnFinish(true);
					kiryoku->setPosition(target_chara.at(i)->getPosition());
					kiryoku->setScale(ratio);
					this->addChild(kiryoku, 1);
					
					ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
					kiryoku_plus->setAutoRemoveOnFinish(true);
					kiryoku_plus->setPosition(target_chara.at(i)->getPosition());
					kiryoku_plus->setScale(ratio);
					this->addChild(kiryoku_plus, 2);
					
					//SE
					CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
				}
			}
			finish_time=0.5f;
			
            break;
        }
        case 25:{//アクティブスキル封印
            if(use_chara->getPassiveSkillTarget() == "all" || use_chara->getPassiveSkillTarget() == "all_attack" || use_chara->getPassiveSkillTarget() == "all_defense" || use_chara->getPassiveSkillTarget() == "all_support"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }case 26:{//戦闘力UP 固定値
            if(effect_onoff_flg){
                //パーティクル出力
                for(int i =0;i<target_chara.size();i++){
                    ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                    kiryoku->setAutoRemoveOnFinish(true);
                    kiryoku->setPosition(target_chara.at(i)->getPosition());
                    kiryoku->setScale(ratio);
                    this->addChild(kiryoku, 1);
                    
                    ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                    kiryoku_plus->setAutoRemoveOnFinish(true);
                    kiryoku_plus->setPosition(target_chara.at(i)->getPosition());
                    kiryoku_plus->setScale(ratio);
                    this->addChild(kiryoku_plus, 2);
                    
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                }
            }
            finish_time=0.5f;
            
            //アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }

            break;
        }case 27:{//戦闘力DOWN 固定値
            for(int i =0;i<target_chara.size();i++){
                if(effect_onoff_flg){
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
                    
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/status_down.mp3");
                }
            }
            
            //アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            
            break;
        }case 28:{//
            
            
            break;
        }case 29:{//
            
            
            break;
        }case 30:{//
            
            
            break;
        }case 31:{//赤コンボアップ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else if(use_chara->getPassiveSkillTarget() == "own_all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }case 32:{//青コンボアップ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else if(use_chara->getPassiveSkillTarget() == "own_all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }case 33:{//緑コンボアップ
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else if(use_chara->getPassiveSkillTarget() == "own_all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }case 34:{//
            
            
            break;
        }case 35:{//
            
            
            break;
        }case 36:{//
            
            
            break;
        }case 42:{//総大将へのダメージ減少
            if(use_chara->getType() == CHARA_TYPE_LEADER || use_chara->getType() == CHARA_TYPE_SUPPORT){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            
            break;
            
        }case 44:{//コンボ無効
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else if(use_chara->getPassiveSkillTarget() == "enemy_all"){
                dung->uiLayer->insertPassiveIcon(reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }case 47:{//置いたターンのカードを置く士気をマイナス
            for(Chara *chara : CharaList::getOneSide(use_chara->getBelongingIndex())){
                if(chara->isInHand() || chara->isFriend()){
                    //手札かフレンドカード
                    if(use_chara->getPassiveSkillTarget() == "own_all" || (use_chara->getPassiveSkillTarget() == "own_attack" && chara->isAttacker()) || (use_chara->getPassiveSkillTarget() == "own_defense" && chara->isDefender()) || (use_chara->getPassiveSkillTarget() == "own_support" && chara->getType()==CHARA_TYPE_SUPPORT)){
                        
                        ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                        kiryoku->setAutoRemoveOnFinish(true);
                        kiryoku->setPosition(chara->getPosition());
                        kiryoku->setScale(ratio);
                        this->addChild(kiryoku, 1);
                        
                        ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                        kiryoku_plus->setAutoRemoveOnFinish(true);
                        kiryoku_plus->setPosition(chara->getPosition());
                        kiryoku_plus->setScale(ratio);
                        this->addChild(kiryoku_plus, 2);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                    }
                }
            }
            break;
        }
        case 50:{//戦闘力??以下行動不能
            if(effect_onoff_flg){
                //パーティクル出力
                for(int i =0;i<target_chara.size();i++){
                    ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
                    hunou->setAutoRemoveOnFinish(true);
                    hunou->setPosition(target_chara.at(i)->getPosition());
                    hunou->setScale(ratio);
                    this->addChild(hunou, 1);
                }
                
                //SE
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/hunou.mp3");
            }
            
            //アイコンの追加　　アイコン付ける側indexとスキル所持キャラ
            dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            
            break;
            
        }
        case 52://貫通
        case 64://
        case 83://戦闘力低下無効 軍師用
        case 87://スキルダメージX％減少
        case 95://スキルダメージ増加
        case 101://アクティブ封印耐性
        {
            cout << "標準出力？" << endl;
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }case 53:{//カウンター封じ
            dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            
            break;
        }case 54:{//置いた時に対象と同じカードを自分のデッキの一番上に生成する
            if(target_chara.size()>0){
                if(effect_onoff_flg){
                    //パーティクル出力
                    Chara *temp_chara2 = target_chara.at(0);
                    ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
                    hunou->setAutoRemoveOnFinish(true);
                    hunou->setPosition(temp_chara2->getPosition());
                    hunou->setScale(ratio);
                    this->addChild(hunou, 1);
                    float ratio2 = ratio;
                    runAction(Sequence::create(DelayTime::create(0.6f), CallFunc::create([this,temp_chara2,ratio2](){
                        ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                        hit1->setAutoRemoveOnFinish(true);
                        hit1->setPosition(temp_chara2->getPosition());
                        hit1->setScale(ratio2);
                        this->addChild(hit1, 1);
                    }),NULL));
                }
                
                Chara *temp_chara = target_chara.at(1);
                Vec2 posi = target_chara.at(0)->getPosition();
                
                temp_chara->runAction(Sequence::create(DelayTime::create(0.5f), MoveTo::create(0.0f, posi), EaseSineInOut::create(MoveTo::create(0.7f, dung->fieldLayer->deck_point[use_chara->getBelongingIndex()])), NULL));
                
                
            }
            finish_time=1.5f;
            break;
        }case 55:{//置いた時に対象と同じカードにコピーされる
            if(target_chara.size()>0){
                Chara *use_chara2 = use_chara;
                Chara *temp_chara2 = target_chara.at(0);
                if(effect_onoff_flg){
                    //パーティクル出力
                    ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
                    hunou->setAutoRemoveOnFinish(true);
                    hunou->setPosition(temp_chara2->getPosition());
                    hunou->setScale(ratio);
                    this->addChild(hunou, 1);
                    float ratio2 = ratio;
                    runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([this,use_chara2,ratio2](){
                        ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                        hit1->setAutoRemoveOnFinish(true);
                        hit1->setPosition(use_chara2->getPosition());
                        hit1->setScale(ratio2);
                        this->addChild(hit1, 1);
                    }),NULL));
                }
                
                runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([dung, use_chara2,temp_chara2](){
                    
                    use_chara2->setCharaId(temp_chara2->getCharaId());
                    use_chara2->setPlaceCost(temp_chara2->place_cost);
                    use_chara2->setBaseValue(temp_chara2->getBaseValue());
                    use_chara2->setCharaTurnCount(temp_chara2->getCharaTurnCount());
                    use_chara2->setCharaTurnCountDef(temp_chara2->getCharaTurnCountDef());
                    use_chara2->setType(temp_chara2->getType());
                    use_chara2->setCharaName(temp_chara2->getCharaName());
                    use_chara2->setCharaSecondName(temp_chara2->getCharaSecondName());
                    use_chara2->setRare(temp_chara2->getRare());
                    
                    use_chara2->place_type = temp_chara2->place_type;
                    use_chara2->place_priority = temp_chara2->place_priority;
                    use_chara2->skill_priority = temp_chara2->skill_priority;
                    
                    //アクティブスキル持ちの場合
                    use_chara2->setActiveSkillId(temp_chara2->getActiveSkillId());
                    use_chara2->setActiveSkillName(temp_chara2->getActiveSkillName());
                    use_chara2->setActiveSkillDetail(temp_chara2->getActiveSkillDetail());
                    use_chara2->setActiveSkillTarget(temp_chara2->getActiveSkillTarget());
                    use_chara2->setActiveSkillValue(temp_chara2->getActiveSkillValue());
                    use_chara2->setActiveSkillTurn(temp_chara2->getActiveSkillTurn());
                    use_chara2->setActiveSkillCost(temp_chara2->getActiveSkillCost());
                    
                    //パッシブスキル持ちの場合
                    use_chara2->setPassiveSkillId(temp_chara2->getPassiveSkillId());
                    use_chara2->setPassiveSkillName(temp_chara2->getPassiveSkillName());
                    use_chara2->setPassiveSkillDetail(temp_chara2->getPassiveSkillDetail());
                    use_chara2->setPassiveSkillTarget(temp_chara2->getPassiveSkillTarget());
                    use_chara2->setPassiveSkillValue(temp_chara2->getPassiveSkillValue());
                    
                    use_chara2->setCombinationL(temp_chara2->getCombinationL());
                    use_chara2->setCombinationR(temp_chara2->getCombinationR());
                    
                    Vec2 posi = use_chara2->getPosition();
                    int field = use_chara2->getFieldIndex();
                    int value = temp_chara2->getValue();
                    HmsVector< map<string, int> > active_temp;//アクティブバッファ保存
                    for(int i=0; i<temp_chara2->chara_status_active.size(); i++){
                        map<string,int> temp;
                        temp.insert( map<string, int>::value_type("primary", temp_chara2->chara_status_active.at(i)["primary"]));
                        temp.insert( map<string, int>::value_type("skill_id", temp_chara2->chara_status_active.at(i)["skill_id"]));
                        temp.insert( map<string, int>::value_type("value", temp_chara2->chara_status_active.at(i)["value"]));
                        temp.insert( map<string, int>::value_type("turn", temp_chara2->chara_status_active.at(i)["turn"]));
                        temp.insert( map<string, int>::value_type("state", temp_chara2->chara_status_active.at(i)["state"]));
                        active_temp.push_back(temp);
                    }
                    HmsVector< map<string, int> > passive_temp;//パッシブバッファ保存
                    for(int i=0; i<temp_chara2->chara_status_passive.size(); i++){
                        if(temp_chara2->chara_status_passive.at(i)["primary"] == temp_chara2->getPrimaryId()){
                            map<string,int> temp;
                            temp.insert( map<string, int>::value_type("primary", temp_chara2->chara_status_passive.at(i)["primary"]));
                            temp.insert( map<string, int>::value_type("skill_id", temp_chara2->chara_status_passive.at(i)["skill_id"]));
                            temp.insert( map<string, int>::value_type("value", temp_chara2->chara_status_passive.at(i)["value"]));
                            temp.insert( map<string, int>::value_type("state", temp_chara2->chara_status_passive.at(i)["state"]));
                            passive_temp.push_back(temp);
                        }
                    }
                    
                    use_chara2->dead_flg = true;
                    use_chara2->initialize();
                    use_chara2->setPosition(posi);
                    use_chara2->setFieldIndex(field);
                    use_chara2->setValue(value);
                    use_chara2->showCardParam();
                    
                    for(int i=0; i<active_temp.size(); i++){
                        map<string,int> temp;
                        temp.insert( map<string, int>::value_type("primary", active_temp.at(i)["primary"]));
                        temp.insert( map<string, int>::value_type("skill_id", active_temp.at(i)["skill_id"]));
                        temp.insert( map<string, int>::value_type("value", active_temp.at(i)["value"]));
                        temp.insert( map<string, int>::value_type("turn", active_temp.at(i)["turn"]));
                        temp.insert( map<string, int>::value_type("state", active_temp.at(i)["state"]));
                        use_chara2->chara_status_active.push_back(temp);
                    }
                    for(int i=0; i<passive_temp.size(); i++){
                        map<string,int> temp;
                        temp.insert( map<string, int>::value_type("primary", use_chara2->getPrimaryId()));
                        temp.insert( map<string, int>::value_type("skill_id", passive_temp.at(i)["skill_id"]));
                        temp.insert( map<string, int>::value_type("value", passive_temp.at(i)["value"]));
                        temp.insert( map<string, int>::value_type("state", passive_temp.at(i)["state"]));
                        use_chara2->chara_status_passive.push_back(temp);
                    }
                    
                    for(int ii=0; ii<2; ii++){//全体パッシブ追加
                        for(int i=0; i < dung->player[ii]->whole_status_passive.size(); i++){
                            if(dung->player[ii]->whole_status_passive.at(i)["primary"] == temp_chara2->getPrimaryId()){
                                map<string,int> temp;
                                temp["primary"] = use_chara2->getPrimaryId();
                                temp["value"] = dung->player[ii]->whole_status_passive.at(i)["value"];
                                temp["state"] = dung->player[ii]->whole_status_passive.at(i)["state"];
                                dung->player[use_chara2->reverseIndex(ii)]->whole_status_passive.push_back(temp);//反対側のプレイヤーに追加
                                dung->uiLayer->insertPassiveIcon(use_chara2->reverseIndex(ii),use_chara2->getPrimaryId(),use_chara2->getPassiveSkillId());//アイコン追加
                                break;
                            }
                        }
                    }
                    
                }), NULL));
            }
            finish_time=1.5f;
            break;
        }case 58:{//置いた時に前方の配置コストX以下の武将を自軍の武将にする
            if(target_chara.size()>0 && use_chara->getNearBlankFieldIndex()>0){
                Chara *use_chara2 = use_chara;
                Chara *temp_chara2 = target_chara.at(0);
                if(effect_onoff_flg){
                    //パーティクル出力
                    ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
                    hunou->setAutoRemoveOnFinish(true);
                    hunou->setPosition(temp_chara2->getPosition());
                    hunou->setScale(ratio);
                    this->addChild(hunou, 1);
                    float ratio2 = ratio;
                    runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([this,use_chara2,ratio2](){
                        ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                        hit1->setAutoRemoveOnFinish(true);
                        hit1->setPosition(use_chara2->getPosition());
                        hit1->setScale(ratio2);
                        this->addChild(hit1, 1);
                    }),NULL));
                }
                
                int field = use_chara2->getNearBlankFieldIndex();
                CCLOG("移動するフィールド%d",field);
                int value = temp_chara2->getValue();
                
                runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([&, field, value ,dung, use_chara2,temp_chara2](){
                    
                    HmsVector< map<string, int> > active_temp = temp_chara2->chara_status_active;//アクティブバッファ保存
                    HmsVector< map<string, int> > passive_temp = temp_chara2->chara_status_passive;//パッシブバッファ保存
                    
                    map<string,int> temp[2];
                    for(int ii=0; ii<2; ii++){//全体パッシブ保存
                        for(int i=0; i < dung->player[ii]->whole_status_passive.size(); i++){
                            if(dung->player[ii]->whole_status_passive.at(i)["primary"] == temp_chara2->getPrimaryId()){
                                temp[ii] = dung->player[ii]->whole_status_passive.at(i);
                                
                                break;
                            }
                        }
                    }
                    
                    temp_chara2->setBelongingIndex(use_chara2->getBelongingIndex());
                    Point temp_point = temp_chara2->getPosition();
                    temp_chara2->setDeadFlg(true);
                    temp_chara2->eliminate_flg = true;//これが無いとデッキに行く
                    temp_chara2->initialize();
                    
                    temp_chara2->setPosition(temp_point);
                    temp_chara2->setFieldIndex(field);
                    temp_chara2->showCardParam();
                    
                    temp_chara2->chara_status_active = active_temp;//アクティブバッファ復元
                    temp_chara2->chara_status_passive = passive_temp;//パッシブバッファ復元
                    
                    for(int ii=0; ii<2; ii++){//全体パッシブ追加
                        if(temp[ii].empty()==false){
                            dung->player[temp_chara2->reverseIndex(ii)]->whole_status_passive.push_back(temp[ii]);//反対側のプレイヤーに追加
                            dung->uiLayer->insertPassiveIcon(temp_chara2->reverseIndex(ii),temp_chara2->getPrimaryId(),temp_chara2->getPassiveSkillId());//アイコン追加
                        }
                    }
                    
                    temp_chara2->runAction(EaseSineInOut::create(MoveTo::create(0.7f, dung->fieldLayer->field_position_list[temp_chara2->getBelongingIndex()].at(field))));
                }),NULL));
                
            }
                                                                                
            finish_time=1.5f;
            break;
        }case 61:{///コンボカラー左右違う時コンボ倍率UP
            dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            
            break;
        }case 62:{//置いた時にこのカードと同じコンボカラーの武将がデッキにいたら、それをフレンドカードにする
            if(target_chara.size()>0){//対象がいた時
                Chara *delete_chara = nullptr;
                if(CharaList::getFriend(use_chara->getBelongingIndex())!=nullptr){//フレンドキャラがいる時
                    delete_chara = CharaList::getFriend(use_chara->getBelongingIndex());
                }
                for(int i=0; i<dung->deck[use_chara->getBelongingIndex()].size(); i++){
                    if(dung->deck[use_chara->getBelongingIndex()].at(i)==target_chara[0]){
                        dung->deck[use_chara->getBelongingIndex()].erase(dung->deck[use_chara->getBelongingIndex()].begin() + i);//デッキから削除
                        break;
                    }
                }
                target_chara[0]->setHandIndex(6);//フレンドカードに
                target_chara[0]->showCardParam();
                
                auto func = [delete_chara](){
                    if(delete_chara != nullptr){
                        delete_chara->dead_flg = true;
                        delete_chara->initialize();//消去
                    }
                };
                target_chara[0]->runAction(Sequence::create(EaseSineInOut::create(MoveTo::create(0.7f, dung->fieldLayer->friend_position_list[target_chara[0]->getBelongingIndex()])), CallFunc::create(func),NULL));
                
            }
            
            finish_time=1.5f;
            break;
        }case 65:{//HP1で耐える
            dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            
            break;
        }case 67:{//攻撃カード置いたら行動不能
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
        
            break;
        }case 68:{//置いた時にこのカードと同じカードを召喚
            if(use_chara->getNearBlankFieldIndex()>0){
                Chara *use_chara2 = use_chara;
                
                if(effect_onoff_flg){
                    //パーティクル出力
                    ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/hunou.plist");
                    hunou->setAutoRemoveOnFinish(true);
                    hunou->setPosition(use_chara2->getPosition());
                    hunou->setScale(ratio);
                    this->addChild(hunou, 1);
                    float ratio2 = ratio;
                    runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([this,use_chara2,ratio2](){
                        ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                        hit1->setAutoRemoveOnFinish(true);
                        hit1->setPosition(use_chara2->getPosition());
                        hit1->setScale(ratio2);
                        this->addChild(hit1, 1);
                    }),NULL));
                }
                int field = use_chara2->getNearBlankFieldIndex();
                CCLOG("移動するフィールド%d",field);
                //int value = temp_chara2->getValue();
                
                runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([&, field ,dung, use_chara2](){
                    
                    HmsVector< map<string, int> > active_temp = use_chara2->chara_status_active;//アクティブバッファ保存
                    HmsVector< map<string, int> > passive_temp = use_chara2->chara_status_passive;//パッシブバッファ保存
                    
                    Chara *chara = use_chara2->copy();//コピーの作成
                    Point temp_point = use_chara2->getPosition();
                    
                    CharaList::chara_list.insert(chara);
                    chara->setBelongingIndex(use_chara2->getBelongingIndex());
                    chara->initialize();
                    chara->setPosition(temp_point);
                    chara->setFieldIndex(field);
                    chara->showCardParam();
                    chara->eliminate_flg = true;
                    
                    chara->chara_status_active = active_temp;//アクティブバッファ復元
                    chara->chara_status_passive = passive_temp;//パッシブバッファ復元
                    
                    chara->runAction(EaseSineInOut::create(MoveTo::create(0.7f, dung->fieldLayer->field_position_list[chara->getBelongingIndex()].at(field))));
                }),NULL));
                
                
            }
            finish_time=1.5f;
            break;
        }case 71:{//相手はXコストのカードを出すことができない
            dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            
            break;
        }
            
        case 72:{//対象のパッシブスキル破壊
            for(Chara *chara : target_chara){
                if(effect_onoff_flg){
                    //パーティクル出力
                    float ratio2 = ratio;
                    runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([this,chara,ratio2](){
                        ParticleSystemQuad *p_break = ParticleSystemQuad::create("download/Effect/Particle/passive_break.plist");
                        p_break->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/passive_break.png"));
                        p_break->setAutoRemoveOnFinish(true);
                        p_break->setPosition(chara->getPosition());
                        p_break->setScale(ratio2);
                        this->addChild(p_break, 1);
                    }),NULL));
                }
            }
            break;
        }
            
        case 74:{//攻撃不可
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }
            
        case 77:
        case 73:{//対象除去
            for(Chara *chara : target_chara){
                if(effect_onoff_flg){
                    //パーティクル出力
                    float ratio2 = ratio;
                    runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([this,chara,ratio2](){
                        //パーティクル出力
                        ParticleSystemQuad* jokyo_success = ParticleSystemQuad::create("download/Effect/Particle/jokyo_success.plist");
                        jokyo_success->setAutoRemoveOnFinish(true);
                        jokyo_success->setPosition(chara->getPosition());
                        jokyo_success->setScale(ratio);
                        this->addChild(jokyo_success, 1);
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/jokyo_success.mp3");
                    }),NULL));
                }
                finish_time=1.1f;
            }
            break;
        }
        case 88:{///対象のアクティブ解除
            for(Chara *chara : target_chara){
                
                //パーティクル出力
                float ratio2 = ratio;
                runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([this,chara,ratio2,effect_onoff_flg](){
                    
                    chara->chara_status_active.clear();
                    
                    //パーティクル出力
                    if(effect_onoff_flg){
                        ParticleSystemQuad* heal = ParticleSystemQuad::create("download/Effect/Particle/heal2.plist");
                        heal->setAutoRemoveOnFinish(true);
                        heal->setPosition(chara->getPosition());
                        heal->setScale(ratio);
                        this->addChild(heal, 1);
                        
                        ParticleSystemQuad* heal_plus = ParticleSystemQuad::create("download/Effect/Particle/heal_plus.plist");
                        heal_plus->setAutoRemoveOnFinish(true);
                        heal_plus->setPosition(chara->getPosition());
                        heal_plus->setScale(ratio);
                    }
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/heal.mp3");
                }),NULL));
                
                finish_time=1.1f;
            }
            break;
        }
        case 90://士気がX以上の時、それ以上増加しなくなる
        case 91:{//ドロー数制限X枚
            if(use_chara->getPassiveSkillTarget() == "all"){
                dung->uiLayer->insertPassiveIcon(use_chara->getBelongingIndex(),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }else{
                dung->uiLayer->insertPassiveIcon(use_chara->reverseIndex(use_chara->getBelongingIndex()),use_chara->getPrimaryId(),use_chara->getPassiveSkillId());
            }
            break;
        }
            
        case 93:{///対象の戦闘力を１ターン下げる
            for(Chara *chara : target_chara){
                
                map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                temp.insert(make_pair("primary", use_chara->getPrimaryId()));
                temp.insert(make_pair("skill_id", 34));//対象戦闘力ダウン
                temp.insert(make_pair("value", use_chara->getPassiveSkillValue()));
                temp.insert(make_pair("turn", 1*2));
                chara->chara_status_active.push_back(temp);
                
                //パーティクル出力
                ParticleSystemQuad* status_down = ParticleSystemQuad::create("download/Effect/Particle/status_down.plist");
                status_down->setAutoRemoveOnFinish(true);
                status_down->setPosition(chara->getPosition());
                status_down->setScale(ratio);
                this->addChild(status_down, 1);
                
                ParticleSystemQuad* status_down_plus = ParticleSystemQuad::create("download/Effect/Particle/status_down_plus.plist");
                status_down_plus->setAutoRemoveOnFinish(true);
                status_down_plus->setPosition(chara->getPosition());
                status_down_plus->setScale(ratio);
                this->addChild(status_down_plus, 1);
            }
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/status_down.mp3");
            
            finish_time=1.1f;
            break;
        }
            
        case 94:{///対象の戦闘力を１ターン上げる
            for(Chara *chara : target_chara){
                
                map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                temp.insert(make_pair("primary", use_chara->getPrimaryId()));
                temp.insert(make_pair("skill_id", 33));//対象戦闘力アップ
                temp.insert(make_pair("value", use_chara->getPassiveSkillValue()));
                temp.insert(make_pair("turn", 1*2));
                chara->chara_status_active.push_back(temp);
                
                //パーティクル出力
                ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/kiryoku.plist");
                kiryoku->setAutoRemoveOnFinish(true);
                kiryoku->setPosition(chara->getPosition());
                kiryoku->setScale(ratio);
                this->addChild(kiryoku, 1);
                
                ParticleSystemQuad* kiryoku_plus = ParticleSystemQuad::create("download/Effect/Particle/kiryoku_plus.plist");
                kiryoku_plus->setAutoRemoveOnFinish(true);
                kiryoku_plus->setPosition(chara->getPosition());
                kiryoku_plus->setScale(ratio);
                this->addChild(kiryoku_plus, 1);
            }
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
            
            finish_time=1.1f;
            break;
        }
            
        case 96:{//置いた時「対象」を誘惑し、戦闘力を１ターンの間X増加させる
            for(auto target : target_chara){
                //Chara *target = target_chara.at(0);
                /*
                 auto sprite1 = Sprite::create("download/Dungeon/present.png");
                 sprite1->setScale(ratio/2);
                 sprite1->setPosition(chara->getPosition());
                 this->addChild(sprite1, 3);
                 sprite1->runAction(EaseElasticOut::create(ScaleTo::create(0.5, ratio),0.3f));
                 */
                auto node = Node::create();
                node->setScale(ratio/2);
                node->setPosition(use_chara->getPosition());
                this->addChild(node,3);
                
                auto sprite2 = Sprite::create("download/Dungeon/choco.png");
                sprite2->setScale(1);
                sprite2->setPosition(0,0);
                node->addChild(sprite2, 3);
                node->runAction(EaseElasticOut::create(ScaleTo::create(0.5, ratio),0.3f));
                node->runAction(RepeatForever::create(Sequence::create(EaseSineInOut::create(MoveBy::create(1.4f, Point(0,4*ratio))),EaseSineInOut::create( MoveBy::create(1.4f, Point(0,-4*ratio))), NULL)));
                
                auto pika = ParticleSystemQuad::create("download/Effect/Particle/love_kira.plist");
                pika->setAutoRemoveOnFinish(true);
                pika->setPosition(0,0);
                pika->setScale(1);
                node->addChild(pika, 1);
    
                
                runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create([this,use_chara](){
                    ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                    hit1->setAutoRemoveOnFinish(true);
                    hit1->setPosition(use_chara->getPosition());
                    hit1->setScale(ratio*3/4);
                    this->addChild(hit1, 1);
                }),NULL));
                
                float dis = 130.0f*ratio;//中心線から離れる距離
                float sin_1 = (target->getPositionY()-use_chara->getPositionY()) / (target->getPosition().distance(use_chara->getPosition()));
                float cos_1 = (target->getPositionX()-use_chara->getPositionX()) / (target->getPosition().distance(use_chara->getPosition()));
                
                Vec2 move_vec1 = Vec2(-dis*sin_1, dis*cos_1);
                Vec2 move_vec2 = Vec2(dis*sin_1, -dis*cos_1);
                
                /*auto around =      Sequence::create(MoveTo::create(0.3f, use_chara->getPosition()+(chara->getPosition()-use_chara->getPosition())/2+move_vec), MoveTo::create(0.3f, use_chara->getPosition()), NULL);
                 auto around2 = Sequence::create(MoveTo::create(0.3f, use_chara->getPosition()+(chara->getPosition()-use_chara->getPosition())/2+move_vec2), MoveTo::create(0.3f, chara->getPosition()), NULL);
                 */
                
                //auto around1 = Sequence::create(EaseSineOut::create(MoveBy::create(0.4f, move_vec1)), EaseSineIn::create(MoveBy::create(0.4f, -move_vec1)), NULL);
                //auto around2 = Sequence::create(EaseSineOut::create(MoveBy::create(0.4f, move_vec2)), EaseSineIn::create(MoveBy::create(0.4f, -move_vec2)), NULL);
                
                //auto action1 = Sequence::create(DelayTime::create(0.6f), EaseSineOut::create(MoveBy::create(0.8f,  use_chara->getPosition()-chara->getPosition())),NULL);
                
                runAction(Sequence::create(DelayTime::create(1.4f),CallFunc::create([this,target,use_chara](){
                    ParticleSystemQuad *p_break1 = ParticleSystemQuad::create("download/Effect/Particle/love.plist");
                    p_break1->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/love.png"));
                    p_break1->setAutoRemoveOnFinish(true);
                    p_break1->setPosition(target->getPosition());
                    p_break1->setScale(ratio);
                    //p_break1->setBlendAdditive(false);
                    this->addChild(p_break1, 1);
                    ParticleSystemQuad *p_break1_2 = ParticleSystemQuad::create("download/Effect/Particle/star_cloud.plist");
                    p_break1_2->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/star_cloud.png"));
                    p_break1_2->setAutoRemoveOnFinish(true);
                    p_break1_2->setPosition(target->getPosition());
                    p_break1_2->setScale(ratio*3/5);
                    p_break1_2->setDuration(1.7f);
                    this->addChild(p_break1_2, 1);
                    
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/temptation.mp3");
                    
                    map<string,int> temp;//ステータスバッファに、使用キャラ、効果量、残りターンを追加
                    temp.insert(make_pair("primary", use_chara->getPrimaryId()));//誘惑
                    temp.insert(make_pair("skill_id", 31));
                    temp.insert(make_pair("value", 0));
                    temp.insert(make_pair("turn", 2));
                    temp.insert(make_pair("state", STATUS_TYPE_CHARM));
                    target->chara_status_active.push_back(temp);
                    
                    temp.clear();
                    temp.insert(make_pair("primary", use_chara->getPrimaryId()));//戦闘力アップ
                    temp.insert(make_pair("skill_id", 33));
                    temp.insert(make_pair("value", use_chara->getPassiveSkillValue()));
                    temp.insert(make_pair("turn", 2));
                    target->chara_status_active.push_back(temp);
                }), NULL));
                
                
                auto action2 = Sequence::create(DelayTime::create(0.6f),EaseSineOut::create(MoveBy::create(0.8f,  target->getPosition()-use_chara->getPosition()))
                                                    ,NULL);
                
                node->runAction(Sequence::create(action2, RemoveSelf::create(),NULL) );
                
                finish_time=2.0f;
            }
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
                    chara->showValue(chara_new_value.at(i), finish_time);//戦闘力の更新
                }
            }
		}
	}
	for(int ii=0; ii<2; ii++){//総大将の上昇士気の更新
		CharaList::getLeader(ii)->showValue(dung->player[ii]->getPower());
        CCLOG("総大将の士気更新%d",dung->player[ii]->getPower());
	}
	
	
    if(use_chara->getType() == CHARA_TYPE_LEADER){
        
        
    }else{
        this->runAction(Sequence::create(DelayTime::create(finish_time),CallFunc::create([&,dung](){//X秒後に演出終了
            dung->passiveSkillEffect(0.0f);//次へ
        }), nullptr));
    }
}


void PassiveSkillEffectLayer::turnSkillEffect(string data_string, int index){//ターン開始時発動スキルエフェクト
    DungeonScene *dung = static_cast<DungeonScene *>(getParent());
	//初期化
	target_chara.clear();
	float finish_time=0.0f;
	prim_value_string="";
	
	HMSLOG("エフェクトストリング%s",data_string.c_str());
	HmsVector<string> value_data_list = split(data_string,"@");//コンボキャラデータとvalueのデータに分ける
	prim_value_string=value_data_list.at(1);
	
	HmsVector<string> data_list = split(value_data_list.at(0), ",");
	
    Chara *used_chara = CharaList::getAtPrimary(stoi(data_list.at(0)));
	
	//オプションでエフェクトオフの場合はエフェクトナンバーを0にする
	int skill_effect_num = used_chara->getPassiveSkillId();
	
    //オプションでエフェクトの表示有無を取得
    bool effect_onoff_flg = SaveData::loadBool("effect");
    
	//スキルごとの演出
	switch (skill_effect_num) {
        case 4:
        case 5:
        case 35:{//毎ターンプレイヤーダメージ
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* player_damage_passive = ParticleSystemQuad::create("download/Effect/Particle/player_damage_passive.plist");
				player_damage_passive->setAutoRemoveOnFinish(true);
                player_damage_passive->setPosition(CharaList::getLeader(index)->getPosition());
				player_damage_passive->setScale(ratio);
				this->addChild(player_damage_passive, 1);

				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/player_damage_passive.mp3");
			}
			dung->playerHpUpdate(index, stoi(data_list.at(1)), stoi(data_list.at(2)));
			finish_time = 1.0f;
			break;
		}
        case 3:
		case 6:
		case 7:{//毎ターンHP回復
			if(effect_onoff_flg){
				//パーティクル出力
				ParticleSystemQuad* kaihuku_passive3 = ParticleSystemQuad::create("download/Effect/Particle/kaihuku_passive3.plist");
				kaihuku_passive3->setAutoRemoveOnFinish(true);
                kaihuku_passive3->setPosition(CharaList::getLeader(index)->getPosition());
				kaihuku_passive3->setScale(ratio);
				this->addChild(kaihuku_passive3, 1);
			
				//SE
				CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku_passive3.mp3");
			}

			//アイコン追加
			dung->playerHpUpdate(index, -1 * stoi(data_list.at(1)), stoi(data_list.at(2)));
			finish_time = 1.0f;
			
			break;
		}
        case 34:{//毎ターンプレイヤーダメージ
            if(effect_onoff_flg){
                //パーティクル出力
                ParticleSystemQuad* player_damage_passive = ParticleSystemQuad::create("download/Effect/Particle/player_damage_passive.plist");
                player_damage_passive->setAutoRemoveOnFinish(true);
                player_damage_passive->setPosition(CharaList::getLeader(reverseIndex(index))->getPosition());
                player_damage_passive->setScale(ratio);
                this->addChild(player_damage_passive, 1);
                
                //SE
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/player_damage_passive.mp3");
            }
            dung->playerHpUpdate(reverseIndex(index), stoi(data_list.at(1)), stoi(data_list.at(2)));
            finish_time = 1.0f;
            break;
        }
            
        case 97:{//手札にある時、対象の武将の基本戦闘力アップ
            if(effect_onoff_flg){
                //SE
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
           
                for(int i=1; i<data_list.size(); i++){
                    int primary = stoi(data_list.at(i));
                    
                    auto chara = CharaList::getAtPrimary(primary);
                    
                    
                    auto mask = Sprite::create();
                    mask->setTextureRect(Rect(-50,-50,100,100));
                    mask->setPosition(Point(0,0));
                    mask->setColor(Color3B::WHITE);
                    mask->setScale(ratio);
                    
                    auto clip = ClippingNode::create();
                    //clip->setScale(ratio);
                    clip->setCascadeOpacityEnabled(true);
                    clip->setAlphaThreshold(0);
                    clip->setPosition(chara->getPosition());
                    this->addChild(clip);
                    
                    clip->setStencil(mask);
                    
                    ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/naname.plist");
                    kiryoku->setAutoRemoveOnFinish(true);
                    kiryoku->setPosition(-50*ratio,-50*ratio);
                    kiryoku->setScale(ratio);
                    clip->addChild(kiryoku, 1);
                    
                    finish_time = 1.2f;
                }
            }
            break;
        }
            
        case 100:{//手札にある時、対象の武将の基本戦闘力ダウン
            if(effect_onoff_flg){
                //SE
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/status_down.mp3");
                
                for(int i=1; i<data_list.size(); i++){
                    int primary = stoi(data_list.at(i));
                    
                    auto chara = CharaList::getAtPrimary(primary);
                    
                    
                    auto mask = Sprite::create();
                    mask->setTextureRect(Rect(-50,-50,100,100));
                    mask->setPosition(Point(0,0));
                    mask->setColor(Color3B::WHITE);
                    mask->setScale(ratio);
                    
                    auto clip = ClippingNode::create();
                    //clip->setScale(ratio);
                    clip->setCascadeOpacityEnabled(true);
                    clip->setAlphaThreshold(0);
                    clip->setPosition(chara->getPosition());
                    this->addChild(clip);
                    
                    clip->setStencil(mask);
                    
                    ParticleSystemQuad* kiryoku = ParticleSystemQuad::create("download/Effect/Particle/naname2.plist");
                    kiryoku->setAutoRemoveOnFinish(true);
                    kiryoku->setPosition(50*ratio,50*ratio);
                    kiryoku->setScale(ratio);
                    clip->addChild(kiryoku, 1);
                    
                    finish_time = 1.2f;
                }
            }
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
            auto chara = CharaList::getAtPrimary(chara_primary.at(i));
                if(chara->isOnField() || chara->isInHand() || chara->isFriend()){
                    chara->showValue(chara_new_value.at(i), 0.2f);//戦闘力の更新
                }
		}
	}
	this->runAction(Sequence::create(DelayTime::create(finish_time),CallFunc::create([&,dung](){//X秒後に演出終了
		dung->turnBufferEffect(0.0f);//次へ
	}), nullptr));
}

void PassiveSkillEffectLayer::counterEffect(string data_string){

    //初期化
    vector<int> skill_value;
    vector<int> skill_value2;
    
    ratio = getVisibleRatio();
    
    //オプションでエフェクトの表示有無を取得
    //effect_onoff_flg = true;
    bool effect_onoff_flg = SaveData::loadBool("effect");
    
    //スキル使用キャラとスキル効果値を保存
    HmsVector<int> target_primary_id;
    HmsVector<string> data_list = split(data_string, ",");
    
    
    use_chara = CharaList::getAtPrimary(stoi(data_list.at(0)));
    
    int skill_effect_num = stoi(data_list.at(1).c_str());
    
    for(int i=2; i<data_list.size(); i++){
        HmsVector<string> mini_data_list = split(data_list.at(i), "&");
        target_primary_id.insert(stoi(mini_data_list.at(0)));
        skill_value.push_back(stoi(mini_data_list.at(1)));
        if(mini_data_list.size()>2) skill_value2.push_back(stoi(mini_data_list.at(2)));
    }
    
    //対象キャラ保存
    target_chara.clear();
    for(int i=0; i<target_primary_id.size(); i++){
        target_chara.push_back(CharaList::getAtPrimary(target_primary_id.at(i)));
    }


    auto counter_effect_list = CallFunc::create([&, skill_effect_num, skill_value, skill_value2, effect_onoff_flg](){
        
        DungeonScene *dung = static_cast<DungeonScene *>(getParent());
        float finish_delay = 1.0f;
        int used_index = use_chara->getBelongingIndex();
        int enemy_index = use_chara->reverseIndex(use_chara->getBelongingIndex());
        
        
        switch (skill_effect_num) {
            case 12:{//対象の戦闘力を上げる
                if(target_chara.empty()==true) {//カウンターで失敗した場合
                    dung->activeSkillEffectLayer->missEffect(use_chara->getPosition());//missエフェクト
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");//スキル失敗SE
                    finish_delay = 1.0f;//演出終了までの時間
                } else {
                    
                    if(effect_onoff_flg) {
                        
                        for(int i=0; i<target_chara.size(); i++) {
                            if(target_chara.at(i)->isInDeck()){
                                //なし
                                
                            }else if(skill_value[i]>0) {//状態異常成功の時
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
                            }
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kiryoku.mp3");
                        finish_delay = 1.1f;//演出終了までの時間
                        break;
                    }
                }
                break;
                
            }case 13:{//対象の戦闘力を下げる
                if(target_chara.empty()==true) {//カウンターで失敗した場合
                    dung->activeSkillEffectLayer->missEffect(use_chara->getPosition());//missエフェクト
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");//スキル失敗SE
                    finish_delay = 1.0f;//演出終了までの時間
                } else {
                    
                    if(effect_onoff_flg) {
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
                    }
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/status_down.mp3");
                    finish_delay = 1.1f;//演出終了までの時間
                }
                
                break;
                
            }case 14:
            case 69:{//カウンター除去
                if(target_chara.empty()==true) {//カウンターで失敗した場合
                    dung->activeSkillEffectLayer->missEffect(use_chara->getPosition());//missエフェクト
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");//スキル失敗SE
                    finish_delay = 1.0f;//演出終了までの時間
                } else {
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* counter = ParticleSystemQuad::create("download/Effect/Particle/counter.plist");
                            counter->setAutoRemoveOnFinish(true);
                            counter->setPosition(target_chara.at(i)->getPosition());
                            counter->setScale(ratio);
                            this->addChild(counter, 1);
                            ParticleSystemQuad* counter_plus = ParticleSystemQuad::create("download/Effect/Particle/counter_plus.plist");
                            counter_plus->setAutoRemoveOnFinish(true);
                            counter_plus->setPosition(target_chara.at(i)->getPosition());
                            counter_plus->setScale(ratio);
                            this->addChild(counter_plus, 1);
                        }
                        
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/counter.mp3");
                        finish_delay = 2.0f;//演出終了までの時間
                    }
                }
                
                break;
            }case 15:{
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
            
                }
                runAction(Sequence::create(DelayTime::create(0.9f),CallFunc::create([&,used_index,enemy_index,skill_value](){
                    static_cast<DungeonScene *>(getParent())->playerHpUpdate(enemy_index, skill_value[0], static_cast<DungeonScene *>(getParent())->player[enemy_index]->getHp());
                    if(skill_value.size() >= 2){
                        static_cast<DungeonScene *>(getParent())->playerHpUpdate(used_index, skill_value[1], static_cast<DungeonScene *>(getParent())->player[used_index]->getHp());
                    }
                }), NULL));
                
                finish_delay = 1.8f;//演出終了までの時間
                break;
                
            }case 40:{//カウンター行動不能
                if(target_chara.empty()==true) {//カウンターで失敗した場合
                    dung->activeSkillEffectLayer->missEffect(use_chara->getPosition());//missエフェクト
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");//スキル失敗SE
                    finish_delay = 1.0f;//演出終了までの時間
                } else {
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
                    }
                    runAction(Sequence::create(DelayTime::create(0.9f),CallFunc::create([&](){
                        if(effect_onoff_flg) {
                            for(int i=0; i<target_chara.size(); i++) {
                                //パーティクル出力
                                ParticleSystemQuad* hunou_success = ParticleSystemQuad::create("download/Effect/Particle/hunou_success.plist");
                                hunou_success->setAutoRemoveOnFinish(true);
                                hunou_success->setPosition(target_chara.at(i)->getPosition());
                                hunou_success->setScale(ratio);
                                this->addChild(hunou_success, 0);
                            }
                            //スキル成功SE
                            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/success.mp3");
                            
                        }
                    }), NULL));
                    
                    runAction(Sequence::create(DelayTime::create(1.8f),CallFunc::create([&](){
                        for(int i=0; i<target_chara.size(); i++){
                            if(use_chara->getPrimaryId() == target_chara.at(i)->getPrimaryId() || target_chara.at(i)->getDeadFlg() == true || target_chara.at(i)->isOnField()==false) continue;
                            target_chara.at(i)->showCardParam();
                        }
                    }), NULL));
                    
                    finish_delay = 1.9f;
                }
                break;
                
            }case 41:{//カウンター誘惑
                if(target_chara.empty()==true) {//カウンターで失敗した場合
                    dung->activeSkillEffectLayer->missEffect(use_chara->getPosition());//missエフェクト
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");//スキル失敗SE
                    finish_delay = 1.0f;//演出終了までの時間
                } else {
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
                    }
                    runAction(Sequence::create(DelayTime::create(1.3f),CallFunc::create([&](){
                        for(int i=0; i<target_chara.size(); i++) {
                           
                            if(effect_onoff_flg) {
                                //パーティクル出力
                                ParticleSystemQuad* temptation = ParticleSystemQuad::create("download/Effect/Particle/temptation.plist");
                                temptation->setAutoRemoveOnFinish(true);
                                temptation->setPosition(target_chara.at(i)->getPosition());
                                temptation->setScale(ratio);
                                this->addChild(temptation, 1);
                                //スキル成功SE
                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/success.mp3");
                            }
                        }
                    }), NULL));
                    
                    finish_delay = 2.3f;
                }
                break;
            }case 56:{//士気回復カウンター
                
                if(effect_onoff_flg) {
                    //パーティクル出力
                    ParticleSystemQuad* resorb = ParticleSystemQuad::create("download/Effect/Particle/resorb.plist");
                    resorb->setAutoRemoveOnFinish(true);
                    resorb->setPosition(convertFromDisplaySize(dung->uiLayer->power_point[used_index], 0.5, 0.5));
                    resorb->setScale(ratio);
                    this->addChild(resorb, 1);
                    
                    dung->uiLayer->updatePower(dung->player[used_index]->getNowPower(), used_index);
                    
                    finish_delay = 0.7f;//演出終了までの時間
                }
                break;
            }case 63:{//カウンター回復
                if(effect_onoff_flg) {
                    //パーティクル出力
                    ParticleSystemQuad* kaihuku = ParticleSystemQuad::create("download/Effect/Particle/kaihuku.plist");
                    kaihuku->setAutoRemoveOnFinish(true);
                    kaihuku->setPosition(CharaList::getLeader(used_index)->getPosition());
                    kaihuku->setScale(ratio);
                    this->addChild(kaihuku, 1);
                    
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kaihuku.mp3");
                }
                runAction(Sequence::create(DelayTime::create(1.1f),CallFunc::create([&,skill_value,used_index](){
                    static_cast<DungeonScene *>(getParent())->playerHpUpdate(used_index,-1* skill_value.at(0),static_cast<DungeonScene *>(getParent())->player[used_index]->getHp());
                }), NULL));
                
                finish_delay = 1.9f;//演出終了までの時間
                break;
                
            }case 89:{//カウンター：Xコスを手札から召喚
                
                if(target_chara.empty()==true) {//カウンターで失敗した場合
                    dung->activeSkillEffectLayer->missEffect(use_chara->getPosition());//missエフェクト
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");//スキル失敗SE
                    finish_delay = 1.0f;//演出終了までの時間
                } else {
                    
                    for(int i=0; i<target_chara.size(); i++) {
                        
                        if(effect_onoff_flg) {
                            //パーティクル出力
                            ParticleSystemQuad* counter = ParticleSystemQuad::create("download/Effect/Particle/cant_action_guard.plist");
                            counter->setAutoRemoveOnFinish(true);
                            counter->setPosition(0,0);
                            counter->setScale(ratio/3);
                            counter->setStartColor(Color4F(50, 205, 50, 1.0));
                            counter->setEndColor(Color4F(50, 205, 50, 0.3));
                            counter->setPositionType(ParticleSystem::PositionType::GROUPED);
                            target_chara.at(i)->addChild(counter, 100);
                        }
                    }
                    
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/counter.mp3");
                    
                    runAction(Sequence::create(DelayTime::create(1.5f),CallFunc::create([&,skill_value,used_index](){
                        for(int i=0; i<target_chara.size(); i++) {
                            target_chara.at(i)->showCardParam();
                            target_chara.at(i)->runAction(EaseSineInOut::create(MoveTo::create(0.7f, static_cast<DungeonScene *>(getParent())->fieldLayer->field_position_list[target_chara.at(i)->getBelongingIndex()].at(target_chara.at(i)->getFieldIndex()))));
                            
                            if(effect_onoff_flg) {
                                
                                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/move_star.plist");
                                hit1->setAutoRemoveOnFinish(true);
                                hit1->setPosition(0,0);
                                hit1->setScale(ratio);
                                hit1->setDuration(2.0f);
                                
                                target_chara.at(i)->addChild(hit1, 1);
                            }
                        }
                    }), NULL));
                    
                    
                    
                    finish_delay = 2.2f;//演出終了までの時間
                }
                break;
                
            }case 92:{//カウンター氷漬け
                if(target_chara.empty()==true) {//カウンターで失敗した場合
                    dung->activeSkillEffectLayer->missEffect(use_chara->getPosition());//missエフェクト
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/faild.mp3");//スキル失敗SE
                    finish_delay = 1.0f;//演出終了までの時間
                } else {
                    if(effect_onoff_flg) {
                        for(int i=0; i<target_chara.size(); i++) {
                            //パーティクル出力
                            ParticleSystemQuad* hunou = ParticleSystemQuad::create("download/Effect/Particle/freeze.plist");
                            hunou->setTexture(Director::getInstance()->getTextureCache()->addImage("download/Effect/Particle/freeze.png"));
                            hunou->setAutoRemoveOnFinish(true);
                            hunou->setPosition(target_chara.at(i)->getPosition());
                            hunou->setDuration(1.2f);
                            hunou->setScale(ratio);
                            this->addChild(hunou, 1);
                            hunou->runAction(ScaleTo::create(1.2f, ratio/2));
                        }
                        //SE
                        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/hunou.mp3");
                    }
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
                    
                    finish_delay = 1.2f;
                }
                
                break;
            }
                
            default:{
                
                break;
            }
        }
    
        auto dead_call = CallFunc::create([&](){//キャラ消去エフェクト
            static_cast<DungeonScene *>(getParent())->eraseCharacterEffect(use_chara);
            for(int i=0; i<target_chara.size(); i++){
                static_cast<DungeonScene *>(getParent())->eraseCharacterEffect(target_chara.at(i));
            }
            static_cast<DungeonScene *>(getParent())->eraseCharacterEffect(use_chara);
        });
        auto dead_delay = DelayTime::create(0.3f);
        
        
        
        this->runAction(Sequence::create(DelayTime::create(finish_delay), dead_call, dead_delay, CallFunc::create([&](){
            //状態異常などの表示更新
            if(static_cast<DungeonScene *>(getParent())->counter_character_data.empty()){//最後の演出時の更新
                for(Chara *chara : CharaList::getAll()){
                    if(use_chara->getPrimaryId() == chara->getPrimaryId() || chara->getDeadFlg() == true || chara->isOnField()==false) continue;
                    chara->showCardParam();
                }
            }
            
            static_cast<DungeonScene *>(getParent())->counterEffect(0.0f);
        }), nullptr));
        
        
    });
    
    
    
    //カットイン生成からエフェクトの実行
    SkillCutInLayer* skill_cut_in_layer = SkillCutInLayer::create();
    skill_cut_in_layer->setData(use_chara->getCharaId(), use_chara->getPassiveSkillName(), 1.5f);
    this->runAction(Sequence::create(DelayTime::create(1.5f),counter_effect_list, nullptr));
    
    
    static_cast<DungeonScene*>(getParent())->dialogLayer->addChild(skill_cut_in_layer, 100);
    
    
}


int PassiveSkillEffectLayer::reverseIndex(int index){
	if(index==0){
		return 1;
	}else{
		return 0;
	}
}