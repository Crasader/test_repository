#ifndef __War__PopupChangePageLayer__
#define __War__PopupChangePageLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class popupChangePageLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    string _message; //表示するメッセージ
    string _change_page_name; //変わるページ名
    
    Sprite* _bg; //背景画像
    
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Message,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Shadow = 1,
        Z_Message,
    };
    
    //背景の大きさ
    enum PopupSize{
        Popup_300 = 1,
        Popup_400,
        Popup_500,
    };
    
    float ratio;
    PopupSize _popup_size; //ポップアップの大きさ
    
    static popupChangePageLayer* create(string message, string change_page_name, PopupSize popup_size = Popup_300);
    virtual bool init(string message, string change_page_name, PopupSize popup_size);
    //    CREATE_FUNC(popupChangePageLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void setupImage(); //ポップアップ表示画像
    void closeAndchangePage(float dt); //閉じてページ遷移する
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupChangePageLayer__) */
