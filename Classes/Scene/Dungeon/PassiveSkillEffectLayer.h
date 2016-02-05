
#ifndef __war__PassiveSkillEffectLayer__
#define __war__PassiveSkillEffectLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"

USING_NS_CC;
using namespace std;

class PassiveSkillEffectLayer : public Layer
{
private:
    HmsVector<int> damage;
    HmsVector<float> percentage;//HPゲージの減少パーセンテージ
    
    Chara *use_chara;
    vector<Chara *> target_chara;//スキル命中キャラ
    string prim_value_string;//キャラのプライマリと新旧のvalue
    //bool effect_onoff_flg;				//エフェクト表示フラグ
	float ratio;						//画面の比率取得
public:
    virtual bool init();
    CREATE_FUNC(PassiveSkillEffectLayer);
    
    void skillEffect(string data_string);//基本エフェクト
    void skillEffect2(Chara *use_chara);//基本エフェクトの後、時間差で表示するエフェクト
    void turnSkillEffect(string data_string,int index);//ターン開始時エフェクト
    void counterEffect(string data_string);//カウンターエフェクト
    int reverseIndex(int index);
};

#endif /* defined(__war__PassiveSkillEffectLayer__) */
