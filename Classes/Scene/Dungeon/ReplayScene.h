#ifndef __War__ReplayScene__
#define __War__ReplayScene__

#include "cocos2d.h"
#include "DungeonScene.h"
#include "HttpConnect.h"

USING_NS_CC;

class ReplayScene : public DungeonScene
{
private:
    Json json_battle_log;
    int count_battle_log;
    int index_battle_log = 0;
    
    Label *act_label;
    Sprite *finish_button = nullptr;
    Sprite *favorite_button = nullptr;
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(ReplayScene);
    
    void onEnter(); //初期実行関数

    void getReplayData();
    void getReplayDataCallback(HttpClient *sender, HttpResponse *response);
    void getReplayDataCallback2(HttpClient *sender, HttpResponse *response);
    
    void updateForReplay(float dt);
    
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif /* defined(__War__ReplayScene__) */
