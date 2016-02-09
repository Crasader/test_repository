#ifndef __war__CharaExchangeLayer__
#define __war__CharaExchangeLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaExchangeSprite.h"
#include "CharaExchangeConfirmLayer.h"

USING_NS_CC;

class CharaExchangeLayer : public Layer, public ScrollViewDelegate
{
private:
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<CharaExchangeSprite *> _exchange_chara_sp; //交換キャラの画像
    json11::Json _exchange_chara_data_list; //交換キャラの情報リスト
    
    HmsVector<Point > _vec_position_index; //ポジションインデックスリスト
    
    int _cell_size; //セルの大きさ
    int _exchange_ticket; //所持交換チケット枚数
    
public:
    
    //タグ
    enum Tag{
        T_ScrollView = 1,
        T_ScrollLayer,
        T_Exchange_Ticket_Bg,
        T_Exchange_Ticket_Label,
        T_Help_MenuItem,
        T_Help_Menu,
        T_Popup_Layer = 100,
    };
    
    //ZOrder
    enum ZOrder{
        Z_ScrollView = 1,
        Z_ScrollLayer,
        Z_Exchange_Ticket_Bg,
        Z_Exchange_Ticket_Label,
        Z_Help_MenuItem,
        Z_Help_Menu,
        Z_Popup_Layer = 100,
    };
    
    CharaExchangeLayer(); //コンストラクタ
    
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(CharaExchangeLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void showExchangeTicketSprite(); //所持チケット枚数
    void createHelpButton(); //ヘルプボタンの作成
    void createPositionIndex(); //ポジションインデックスの作成
    void createExchangeCharaList(); //交換キャラのリスト作成
    void sortExchangeChara(); //交換済みキャラを下に持っていくソート処理
    void checkExchangedCharaAndShortage(); //交換済みのキャラか、又は所持チケットが足りているかチェックする
    void actionUpdatePage(); //ページ更新の際の移動アクション
    
    void createScrollView();
    void setScrollViewData();
    Layer* createScrollList();
    
    void updateCharaExchangeLayerPage(); //交換処理をした後のページ更新処理
    
    //通信処理
    void getExchangeCharaInfoRequest();
    void exchangeCharaInfoCallback(HttpClient *sender, HttpResponse *response);
    
    //キャラ交換通信処理
    void exchangeCharaDataRequest(string exchange_chara_id);
    void exchangeCharaDataCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__CharaExchangeLayer__) */
