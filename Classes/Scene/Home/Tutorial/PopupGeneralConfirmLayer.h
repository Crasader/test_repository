#ifndef __War__PopupGeneralConfirmLayer__
#define __War__PopupGeneralConfirmLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;
using namespace std;

class PopupGeneralConfirmLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _type; //選択した属性
    Sprite* message_bg; //メッセージ背景
    Sprite* bg; //ボタン背景画像
    Sprite* type_image; //属性画像
    Label* _deck_label; //デッキ名
    
    void createMessageBg(); //メッセージ背景の作成
    void showMessage(); //メッセージの表示
    void createButtonBg(); //ボタン背景の作成
    void createButton(); //ボタンの作成
    void createTypeImage(); //属性の表示
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
    void showDeckName(); //選んだデッキの名前
    
public:
    float ratio;
    
    static PopupGeneralConfirmLayer* create(string type);
    virtual bool init(string type);
    //    CREATE_FUNC(PopupGeneralConfirmLayer);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupGeneralConfirmLayer__) */
