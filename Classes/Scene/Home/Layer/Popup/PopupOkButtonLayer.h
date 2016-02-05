#ifndef __War__PopupOkButtonLayer__
#define __War__PopupOkButtonLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class PopupOkButtonLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示するメッセージ
    
    int battle_list_id;
    Sprite* _bg; //背景画像
    
    function<void()> _ok_function; //OKボタンの実行関数
    bool _shadow_flg; //影をつけるかのフラグ
    
public:
    //背景の大きさ
    enum PopupSize{
        Popup_300 = 1,
        Popup_400,
        Popup_500,
        Popup_800,
        Popup_900,
    };
    
    float ratio;
    PopupSize _popup_size;
    
    static PopupOkButtonLayer* create(function<void()> ok_func, string message, bool shadow_flg = true, PopupSize popup_size = PopupSize::Popup_300);
    virtual bool init(function<void()> ok_func, string message, bool shadow_flg, PopupSize popup_size);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景画像の作成
    void createButton(); //ボタンの作成
    void showMessage(); //テキストの表示
    Sprite *getBackground() ; //ポップアップ背景の取得
    
    void setIdForBattleRestart(int battle_list_id);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PopupOkButtonLayer__) */
