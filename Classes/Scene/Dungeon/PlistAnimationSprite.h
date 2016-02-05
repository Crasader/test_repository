
#ifndef __war__PlistAnimationSprite__
#define __war__PlistAnimationSprite__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsConsoleUtil.h"

USING_NS_CC;
using namespace std;

class PlistAnimationSprite : public Node
{
public:
    virtual bool init();
    CREATE_FUNC(PlistAnimationSprite);
    
    static Sprite* setData(Layer* layer, string filename, int roop_count, float frame_rate ,int z_order = 0);
};




#endif /* defined(__war__PlistAnimationSprite__) */
