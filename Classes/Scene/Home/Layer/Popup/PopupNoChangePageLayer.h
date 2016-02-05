#ifndef __War__PopupNoChangePageLayer__
#define __War__PopupNoChangePageLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class popupNoChangePageLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示するメッセージ
    
    void setupImage(); //ポップアップ表示画像
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(float dt); //ポップアップ削除アクション
    
public:
    float ratio;
    
    static popupNoChangePageLayer* create(string message);
    static popupNoChangePageLayer* create(string message, float remove_duration);
    virtual bool init(string message, float remove_duration);
    //    CREATE_FUNC(popupNoChangePageLayer);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupNoChangePageLayer__) */
