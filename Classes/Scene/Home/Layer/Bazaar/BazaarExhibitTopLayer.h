#ifndef __War__BazaarExhibitTopLayer__
#define __War__BazaarExhibitTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "BazaarCharaSprite.h"

USING_NS_CC;

class BazaarExhibitTopLayer : public Layer, public ScrollViewDelegate
{
private:
    
    enum Tag {
        T_ScrollView = 1,
        T_ScrollLayer,
        T_Inquiry,
        T_ExhibitList,
        T_BazaarTicketBg,
        T_NoExhibit,
        T_BazaarTicket,
        T_Message,
        T_Search,
        T_ExhibitBtn,
        T_Popup_Layer = 100
    };
    
    enum Zorder {
        Z_ScrollView = 1,
        Z_ScrollLayer,
        Z_Button,
        Z_BazaarTicketBg,
        Z_NoExhibit,
        Z_Message,
        Z_BazaarTicket,
        Z_ExhibitList,
        Z_Popup_Layer = 100
    };
    
    EventListenerTouchOneByOne *listener;
    string exhibit_data_text;
    HmsVector<BazaarCharaSprite *> exhibits; //出品情報のリスト
    bool touchButtonFlg;
    
    json11::Json _my_bazaar_data_list; //出品のリスト
    int _cell_size; //セルの大きさ
    HmsVector<Point > _vec_position_index; //ポジションインデックスリスト
    string cansel_bazaar_log_id; //キャンセルするバザーログID
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(BazaarExhibitTopLayer);
    
    void initData();            //変数初期化
    void initEventListener();   //イベントリスナーの設定
    void createButton();        //ボタンの作成
    void createExhibitList();   //出品物リストを生成
    void getExhibitData();      //出品物データ取得
    void createPositionIndex(); //ポジションインデックスの作成
    void confirmExhibitCansel(int tag); //出品物取り下げ確認
    void getExhibitDataCallBack(HttpClient *sender, HttpResponse *response);    //出品リストデータ取得コールバック
    void exhibitComPopup(string bazaar_log_id, string status);                  //出品物関連PHPポップアップ
    void exhibitGetCharaCallback(HttpClient *sender, HttpResponse *response);   //出品キャンセル物受け取りコールバック
    void exhibitGetExchangeTicketCallback(HttpClient *sender, HttpResponse *response);   //取引成立報酬受け取りコールバック
    void exhibitCanselCallback(HttpClient *sender, HttpResponse *response);     //出品キャンセルコールバック
    void sortExhibitChara(); //キャンセル済みと取引済みを下に持っていくソート
    void actionUpdatePage(); 

    //スクロール関連
    void createScrollView();
    void refreshScrollView();
    void setScrollViewData();
    Layer* createScrollList();
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__War__BazaarExhibitTopLayer__) */
