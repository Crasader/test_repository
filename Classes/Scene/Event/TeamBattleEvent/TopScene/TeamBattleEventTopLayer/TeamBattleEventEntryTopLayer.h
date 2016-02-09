#ifndef __War__TeamBattleEventEntryTopLayer__
#define __War__TeamBattleEventEntryTopLayer__

#include "cocos2d.h"
#include "TeamBattleEventTopBase.h"
#include "HttpConnect.h"

USING_NS_CC;

//エントリー時のイベントトップ画面
class TeamBattleEventEntryTopLayer : public TeamBattleEventTopBase
{
private:
    enum Tag {
        T_Inquiry = 1,
        T_BG,
    };
    
    enum Zorder {
        Z_BG = 1,
        Z_Button,
        Z_Popup=100,
    };
    
public:
    static TeamBattleEventEntryTopLayer* create();
    
    void createButton();
    void createPage();
    void entryCancel();
    void entryCancelCallback(HttpClient *sender, HttpResponse *response);
};
#endif /* defined(__War__TeamBattleEventEntryTopLayer__) */
