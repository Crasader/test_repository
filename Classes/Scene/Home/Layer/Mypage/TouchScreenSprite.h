#ifndef __War__TouchScreenSprite__
#define __War__TouchScreenSprite__

#include "cocos2d.h"
#include "common.h"
#include "HomeCommon.h"
#include "HmsConsoleUtil.h"

USING_NS_CC;
using namespace std;

class TouchScreenSprite : public cocos2d::Sprite
{
public:
    static TouchScreenSprite* create();
    virtual bool init();
    //    CREATE_FUNC(TouchScreenSprite);
    
    void blinkingAction(); //点滅アクションを実行
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__TouchScreenSprite__) */
