#ifndef __War__TeamBattleEventTypeSelectLayer__
#define __War__TeamBattleEventTypeSelectLayer__

#include "cocos2d.h"

USING_NS_CC;

class TeamBattleEventTypeSelectLayer : public Layer
{
private:
    
    enum Tag {
        T_Inquiry = 1,
        T_BG,
    };
    
    enum Zorder {
        Z_BG = 1,
        Z_Button,
    };
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TeamBattleEventTypeSelectLayer);
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__War__TeamBattleEventTypeSelectLayer__) */
