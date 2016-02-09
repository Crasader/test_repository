#ifndef __War__PopupStayButtonLayer__
#define __War__PopupStayButtonLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class PopupStayButtonLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示するメッセージ
    int battle_list_id;
    
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
    Sprite* _bg; //背景画像
    
    function<void()> _yes_function; //はいボタンの実行関数
    function<void()> _no_function; //いいえボタンの実行関数
    
    bool _shadow_visible_flg; //影表示フラグ
public:
    float ratio;
    
    static PopupStayButtonLayer* create(function<void()> yes_func, function<void()> no_func, string message);
    virtual bool init(function<void()> yes_func, function<void()> no_func, string message);
    
    void createBg(); //背景画像の作成
    void createButton(); //ボタンの作成
    void showMessage(); //テキストの表示
    
    void setIdForBattleRestart(int battle_list_id);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupStayButtonLayer__) */
