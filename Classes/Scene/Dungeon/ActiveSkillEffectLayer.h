
#ifndef __war__ActiveSkillEffectLayer__
#define __war__ActiveSkillEffectLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"

USING_NS_CC;

class ActiveSkillEffectLayer : public Layer
{
private:
    Chara *use_chara;					//スキル使用キャラ
    HmsVector<Chara *> target_chara;	//スキル命中キャラ
    vector<vector<int>> skill_val;//スキル効果値
    
    
	bool finish_flg;					//演出終了判定フラグ
	int effect_count;					//演出実行回数保持用
	int skill_effect_num;				//演出番号
	float ratio;						//画面の比率取得
public:
    virtual bool init();
	CREATE_FUNC(ActiveSkillEffectLayer);
    void setSkillEffectData(string data_string);
	void skillEffect();
	void missEffect(Point position);
    void turnSkillEffect(string data_string, int index);//ターン開始時発動スキルエフェクト
};
#endif /* defined(__war__ActiveSkillEffectLayer__) */
