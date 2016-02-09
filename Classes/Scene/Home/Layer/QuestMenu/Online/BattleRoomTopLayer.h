#ifndef __War__BattleRoomTopLayer__
#define __War__BattleRoomTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class BattleRoomTopLayer : public Layer
{
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(BattleRoomTopLayer);
    
    void createButton(); //ボタンの作成
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__BattleRoomTopLayer__) */
