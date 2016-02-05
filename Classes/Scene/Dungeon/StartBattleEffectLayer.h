
#ifndef __war__StartBattleEffectLayer__
#define __war__StartBattleEffectLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"
#include "SuperAnimNodeV2.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;
using namespace json11;

class StartBattleEffectLayer : public Layer, public SuperAnim::SuperAnimNodeListener
{
private:
    enum Order{
        ORDER_SHADOW = 1,
        ORDER_SPRITE_ANIM_THUNDER,
        ORDER_SPRITE_ANIM_WORD,
        ORDER_CARD,
        ORDER_COMMENT
    };
    void makeUserDetailEffect(Json json_master) ;
    
public:
    float ratio;
    virtual bool init();
    CREATE_FUNC(StartBattleEffectLayer);
    
    void initialize();
    void startBattleEffect(Layer* parent, SEL_SCHEDULE s, Json json_master);
   
    Chara *chara_l;//左側キャラ
    Chara *chara_r;//右側キャラ
    int value_l;//左側キャラ戦闘力
    int value_r;//右側キャラ戦闘力
    
};

#endif /* defined(__war__StartBattleEffectLayer__) */
