#ifndef __war__TournamentListLayer__
#define __war__TournamentListLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "TournamentListSprite.h"

USING_NS_CC;

class TournamentListLayer : public Layer, public ScrollViewDelegate
{
private:
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<TournamentListSprite *> _tournament_list; //トーナメント画像リスト
    json11::Json _tournament_data_list; //トーナメント情報のリスト
    
public:
    
    //タグ
    enum Tag{
        T_Title = 1,
        T_Scroll_Layer,
        T_Popup_Layer = 100,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Title = 1,
        Z_Scroll_Layer,
        Z_Popup_Layer = 100,
    };
    
    int _cell_size;
    time_t _tournament_entry_interval; //エントリー可能時間
    
    float ratio;
    TournamentListLayer(); //コンストラクタ
    
    static TournamentListLayer* create();
    virtual bool init();
//    CREATE_FUNC(TournamentListLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void updateCheckEntryStartDate(float dt); //エントリー開始時間になったらボタンを押せるようにする処理
    void createTournamentListSprite(); //トーナメントリストの作成
    void createEntryListButton(); //トーナメントリストの作成
    void createScrollView(); //スクロールビューの作成
    Layer* createList(); //リストの作成
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //通信処理
    void getTournamentEventInfoRequest();
    void tournamentEventInfoCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentListLayer__) */
