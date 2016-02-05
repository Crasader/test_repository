#ifndef __War__popupConfirmLayer__
#define __War__popupConfirmLayer__

#include "cocos2d.h"
#include "HmsGeometricFunction.h"
#include "HomeCommon.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "SaveData.h"
#include "PopupNoChangePageLayer.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class popupConfirmLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _message; //表示するテキスト
    
    function<void()> _ok_function = nullptr; //OKボタンの実行関数
    function<void()> _ng_function = nullptr; //NGボタンの実行関数
    
    bool _favorite_flg;
    bool _for_replay_flg = false;
    string _battle_list_id;
    
    Sprite* _bg; //背景画像
    
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Message,
        T_Yes_MenuItem,
        T_No_MenuItem,
        T_Menu,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Shadow = 1,
        Z_Message,
        Z_Yes_MenuItem,
        Z_No_MenuItem,
        Z_Menu,
    };
    
    //背景の大きさ
    enum PopupSize{
        Popup_300 = 1,
        Popup_400,
        Popup_500,
    };
    
    float ratio;
    PopupSize _popup_size;
    
    static popupConfirmLayer* create(string message, PopupSize popup_size = Popup_300);
    virtual bool init(string message, PopupSize popup_size);
    
    void initEventListener(); //イベントリスナーの設定
    void showImage(); //画像の表示
    void setFunction(function<void()> ok_func, function<void()> ng_func);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //お気に入り関連
    void setFavoriteFlg(bool favorite_flg, string battle_list_id);
    void favoriteRequest();
    void favoriteRequestCallback(HttpClient *sender, HttpResponse *response);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__popupConfirmLayer__) */
