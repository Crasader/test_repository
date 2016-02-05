#ifndef __War__TeamBattleEventLayer__
#define __War__TeamBattleEventLayer__

#include "cocos2d.h"
#include "TeamBattleEventTopBase.h"
#include "HttpConnect.h"

USING_NS_CC;

//チーム対抗戦のトップ画面
class TeamBattleEventLayer : public Layer
{
private:
    enum Tag {
        T_Inquiry = 1,
        T_BG,
        T_LayerBase,
    };
    
    enum Zorder {
        Z_BG = 1,
        Z_Button,
        Z_LayerBase,
    };
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TeamBattleEventLayer);
    
    void onEnter();     //初期実行関数
    void eventCheck();  //イベント開催とエントリー有無のチェック
    void eventCheckCallBack(HttpClient *sender, HttpResponse *response) ;
    void createDisplay();
};
#endif /* defined(__War__TeamBattleEventLayer__) */
