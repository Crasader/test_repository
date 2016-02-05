#ifndef __war__CharaSellConfirmPopupBase__
#define __war__CharaSellConfirmPopupBase__

#include "cocos2d.h"
#include "HmsVector.h"
#include "json11.hpp"
#include "HttpConnectHms.h"

USING_NS_CC;

class CharaSprite;

class CharaSellConfirmPopupBase : public Layer
{
protected:
    const int POS_X_SELL_FIRST = 63;
    const int POS_Y_SELL_FIRST = 340;
    const int POS_X_SELL_DISTANCE = 97;
    const int POS_Y_SELL_DISTANCE = 96;
    
    function<void(Ref*)> noAction;
    
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Bg,
        T_Title,
        T_Close_Menu,
        T_Chara_Bg,
        T_Exchange_Bg,
        T_Exchange_Label,
        T_ScrollLayer,
        T_ScrollView,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
        Z_Bg,
        Z_Title,
        Z_Close_Menu,
        Z_Chara_Bg,
        Z_Exchange_Bg,
        Z_Exchange_Label,
        Z_ScrollLayer,
        Z_ScrollView,
    };
    
    float ratio;
    Sprite* _bg; //背景画像
    HmsVector<CharaSprite *> _sell_chara_list; //売却キャラリスト
    HmsVector<CharaSprite *> _chara; //キャラ画像
    CC_SYNTHESIZE(int, _sell_price, SellPrice); //合計売却ゴールド
    
    virtual bool init(HmsVector<CharaSprite *> &sell_chara_list, int sell_price);
    
    void onEnter();
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createTitle(); //タイトルの作成
    void createButton(); //はい、いいえボタンの作成
    void createSellChara(); //売却キャラの作成
    json11::Json createCharaJsonData(int i); //キャラのJsonデータの作成
    Point calcPositionChara(int i, int j); //売却キャラのポジションの計算
    void showExchangeTicket(); //交換チケット枚数の表示
    void checkRarity(); //レア度のチェック
    bool checkOverMaxGold(); //売却価格の合計が最大値を超えていないかチェック
    
    string createRequestCharaSellList(); //リクエストで送るキャラリストの作成
    //キャラ売却通信処理
    void sellCharaDataRequest();
    void sellCharaDataCallback(HttpClient *sender, HttpResponse *response);
    
    
    virtual void showSellChara() = 0; //売却キャラの表示
    virtual void setNoAction() = 0;
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
private:
    
    
public:
    
};


#endif