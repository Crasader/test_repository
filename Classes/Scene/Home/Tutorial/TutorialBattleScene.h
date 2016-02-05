#ifndef __War__TutorialBattleScene__
#define __War__TutorialBattleScene__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;

class TutorialBattleScene : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    
    static cocos2d::Scene* createScene();
    virtual bool init();
    void menuCloseCallback(cocos2d::Ref* pSender);
    CREATE_FUNC(TutorialBattleScene);
    
    void createEventLitener(); //イベントリスナーの作成
    void createBg(); //背景の作成
    void showMessage(); //ページ内容の表示
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__TutorialBattleScene__) */
