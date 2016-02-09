#ifndef __War__TeamBattleEventTopBase__
#define __War__TeamBattleEventTopBase__

#include "cocos2d.h"
#include "json11.hpp"

USING_NS_CC;

//TeamBattleEventTopLayer
//TeamBattleEventEntryTopLayer
//上記２つのスーパークラス
class TeamBattleEventTopBase : public Layer
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
    
protected:
    int event_item_count;
    int team_id;
    int team_battle_event_list_id;
    bool is_leader;
    bool is_entry_time;
    json11::Json team_battle_data;
    json11::Json team_battle_list_data;
    
public:
    float ratio;
    
    virtual bool init();
    
    void onEnter(); //初期実行関数
    void saveJsonData();
    void createDisplay(json11::Json json_data); //画面生成
    void createEventItem(); //所持イベントアイテム数表示
    void createHelpButton(); //Helpボタン
    virtual void createButton();
    virtual void createPage();
};
#endif /* defined(__War__TeamBattleEventTopBase__) */
