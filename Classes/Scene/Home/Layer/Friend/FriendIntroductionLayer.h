#ifndef __War__FriendIntroductionLayer__
#define __War__FriendIntroductionLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "NativeBridge.h"

USING_NS_CC;

class FriendIntroductionLayer : public Layer
{
public:
    
    virtual bool init();
    CREATE_FUNC(FriendIntroductionLayer);
    
    void setupData();
};

#endif /* defined(__War__FriendIntroductionLayer__) */
