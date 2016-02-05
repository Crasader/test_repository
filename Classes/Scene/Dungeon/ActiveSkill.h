#ifndef __war__ActiveSkill__
#define __war__ActiveSkill__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"

USING_NS_CC;

#define STATUS_BUFFER_TYPE_BUFF 0 //バフ(戦闘力UP、貫通など)
#define STATUS_BUFFER_TYPE_DEBUFF 1 //デバフ(戦闘力DOWN、スキル封印、行動不能など)

class ActiveSkill : public Node
{
public:
	string use_skill_data;	//返り値
	string data_string;		//演出データテキスト
	
    CREATE_FUNC(ActiveSkill);
    void initialize();
    string skillLogic(Chara* used_chara,int skill_id,int skill_value,int used_index, string target_type, string extra_data);
	string dataStringTargetChara(int primary_id, int value);//スキル対象キャラ情報データ作成
	//バフ処理(戦闘力UP/DOWN、貫通等)
    void statusBuffer(Chara* used_chara, int skill_id, int skill_value, int use_index, string target_type, string extra_data, int invoke_turn, int state=0, bool probability_flg = false);
    
    vector<pair<int, int>> getTargetPlace(Chara *used_chara, string target_type);
    vector<Chara *> getTargetCharas(Chara *used_chara, vector<Chara *> all_charas, string target_type);
        
	void setDataRemoveChara(HmsVector<Chara *> target_chara, HmsVector<int> dead_flg);
    void setDataremoveCharaRandom(HmsVector<Chara *> target_chara, int rate);
	Sprite* getIcon(int buff_id);
    
    string counterPassiveLogic(Chara* used_chara,int skill_id,int skill_value, Chara *atk_chara = nullptr);
	bool buffDebuffCheck(Chara* target_chara, int type);//バフデバフを持っているか判定
	void buffDebuffErase(Chara* target_chara, int type);//対象のバフデバフを消す
};
#endif /* defined(__war__ActiveSkill__) */
