

#ifndef __war__UIEffectLayer__
#define __war__UIEffectLayer__

#include <stdio.h>

#include "cocos2d.h"
#include "Chara.h"
#include "HmsVector.h"

USING_NS_CC;
using namespace std;

class UIEffectLayer : public Layer
{
public:
    float ratio;
    virtual bool init();
    CREATE_FUNC(UIEffectLayer);
    
    void initialize();
    
    void place_frame_effect(Point point);
    void place_frame_effect_support(Point point);
    void reset_place_frame_effect();
    
    HmsVector<Sprite *> placeFrameSprite;
};

#endif /* defined(__war__UIEffectLayer__) */
