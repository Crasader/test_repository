#ifndef __War__PopupNameConfirmLayer__
#define __War__PopupNameConfirmLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;
using namespace std;

class PopupNameConfirmLayer : public cocos2d::Layer
{
private:
    
    function<void()> _yesFunc ;
    function<void()> _noFunc ;
    
    EventListenerTouchOneByOne *listener;
    string _name; //入力した名前
    Sprite* message_bg; //メッセージ背景
    Sprite* bg; //ボタン背景画像
    
    void createMessageBg(); //メッセージ背景の作成
    void showMessage(); //メッセージの表示
    void createButtonBg(); //ボタン背景の作成
    void createButton(); //ボタンの作成
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
public:
    float ratio;
    
    static PopupNameConfirmLayer* create(function<void()> yesFunc, function<void()> noFunc, string name);
    virtual bool init(function<void()> &yesFunc,function<void()> &noFunc, string name);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupNameConfirmLayer__) */
