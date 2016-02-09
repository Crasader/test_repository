#ifndef __War__PopupModelChangeLayer__
#define __War__PopupModelChangeLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class PopupModelChangeLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示するメッセージ
    int battle_list_id;
    
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
    Sprite* _bg; //背景画像
    
    bool _shadow_visible_flg; //影表示フラグ
public:
    float ratio;
    
    //タグ
    enum Tag{
        T_EDIT_BOX,
        T_CONFIRM_POPUP,
    };
    
    //ZOrder
    enum ZOrder{
        Z_SHADOW=1,
        Z_EDIT_BOX=5,
        Z_CONFIRM_POPUP=10
    };
    
    static PopupModelChangeLayer* create();
    virtual bool init();
    
    void createBg(); //背景画像の作成
    void createMessage(); //テキストの表示
    void createEditBox() ; //editboxの作成
    void createButton(); //ボタンの作成
    
    void setIdForBattleRestart(int battle_list_id);
    
    string getTextID();
    
    void modelchangeCheckRequest();
    void modelchangeFinishRequest(string uid);
    void modelchangeCallback(HttpClient *sender, HttpResponse *response);
    void modelchangeFinishCallback(HttpClient *sender, HttpResponse *response);
    
    void moveTitle();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupModelChangeLayer__) */
