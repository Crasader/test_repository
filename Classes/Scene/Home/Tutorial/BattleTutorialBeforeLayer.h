#ifndef __War__BattleTutorialBeforeLayer__
#define __War__BattleTutorialBeforeLayer__

#include "cocos2d.h"
#include "TutorialScene.h"
#include "DungeonScene.h"


USING_NS_CC;

class BattleTutorialBeforeLayer : public cocos2d::Layer
{
private:
    //次の遷移に移動
    void nextStep(float dt) ;
    
    string _user_name;
    float ratio;
    
    MessageTalkSprite *message_talk_sprite ;
    EventListenerTouchOneByOne *listener ;
    
    int step_state ; //0: 初期状態 1:1つ目しゃべりだし後  2:2つ目しゃべりだし後  3:3つ目しゃべりだし後
    
    void initEventListener() ;
    bool onTouchBegan(Touch *touch, Event *unused_event) ;
    void onTouchMoved(Touch *touch, Event *unused_event) ;
    void onTouchEnded(Touch *touch, Event *unused_event) ;
    
public:
    virtual bool init();
    CREATE_FUNC(BattleTutorialBeforeLayer);
    void createMessage(); //メッセージの作成
    void changeSceneToDungeonScene();//DungeonSceneに遷移
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__BattleTutorialBeforeLayer__) */
