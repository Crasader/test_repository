#ifndef __War__BattleTutorialAfterLayer__
#define __War__BattleTutorialAfterLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;

class BattleTutorialAfterLayer : public cocos2d::Layer
{
private:
    float ratio;
    
public:
    MessageSprite* message_bg;
    
    virtual bool init();
    CREATE_FUNC(BattleTutorialAfterLayer);
        
    void createMessageBg(); //メッセージ画像の作成
    void createButton(); //ボタンの作成
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__BattleTutorialAfterLayer__) */
