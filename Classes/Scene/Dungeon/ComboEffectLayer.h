
#ifndef __war__ComboEffectLayer__
#define __war__ComboEffectLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"
#include "SuperAnimNodeV2.h"
#include "Effect.h"

USING_NS_CC;
using namespace std;

class ComboEffectLayer : public Layer
{
public:
    float ratio;
    virtual bool init();
    CREATE_FUNC(ComboEffectLayer);
    
    void initialize();
    void combinationEffect(string combination_data,int index,int conbiNum);
    
    Chara *chara_l;//左側キャラ
    Chara *chara_r;//右側キャラ
    int value_l;//左側キャラ戦闘力
    int value_r;//右側キャラ戦闘力
    
    void combinationThunder(int player_index);
    HmsVector<Sprite *> thunder;
    
};

#endif /* defined(__war__ComboEffectLayer__) */
