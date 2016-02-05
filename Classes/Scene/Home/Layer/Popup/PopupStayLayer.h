#ifndef __War__PopupStayLayer__
#define __War__PopupStayLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class PopupStayLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示するメッセージ
    string _type = "";
    
    void setupImage(); //ポップアップ表示画像
    void popupAction(); //ポップアップ表示アクション
//    void deleteAction(float dt); //ポップアップ削除アクション
    
public:
    float ratio;
    
    static PopupStayLayer* create(string message, string type);
    virtual bool init(string message, string type);
    //    CREATE_FUNC(PopupStayLayer);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupStayLayer__) */
