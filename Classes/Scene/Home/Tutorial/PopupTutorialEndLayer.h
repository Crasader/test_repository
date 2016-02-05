#ifndef __War__PopupTutorialEndLayer__
#define __War__PopupTutorialEndLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;
using namespace std;

class PopupTutorialEndLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    void createEventListener(); //イベントリスナーの作成
    void createMessageBg(); //メッセージ背景の作成
    void showMessage(); //メッセージの表示
    
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
public:
    float ratio;
    Sprite* message_bg; //メッセージ画像の背景
    
    virtual bool init();
    CREATE_FUNC(PopupTutorialEndLayer);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupTutorialEndLayer__) */
