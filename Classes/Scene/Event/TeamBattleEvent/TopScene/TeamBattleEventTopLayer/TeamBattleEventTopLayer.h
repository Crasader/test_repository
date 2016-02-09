#ifndef __War__TeamBattleEventTopLayer__
#define __War__TeamBattleEventTopLayer__

#include "cocos2d.h"
#include "TeamBattleEventTopBase.h"

USING_NS_CC;

//未エントリー時のイベントトップ画面
class TeamBattleEventTopLayer : public TeamBattleEventTopBase
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
    static TeamBattleEventTopLayer* create();
    
    void createButton();
    void createPage();
};
#endif /* defined(__War__TeamBattleEventTopLayer__) */
