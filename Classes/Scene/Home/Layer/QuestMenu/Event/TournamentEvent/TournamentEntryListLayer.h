#ifndef __war__TournamentEntryListLayer__
#define __war__TournamentEntryListLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "TournamentEntryListSprite.h"

USING_NS_CC;

class TournamentEntryListLayer : public Layer, public ScrollViewDelegate
{
private:
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<TournamentEntryListSprite *> _tournament_list; //トーナメント画像リスト
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
    TournamentEntryListLayer(); //コンストラクタ
    
    static TournamentEntryListLayer* create();
    virtual bool init();
//    CREATE_FUNC(TournamentEntryListLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void updateCheckEntryStartDate(float dt); //エントリー開始時間になったらボタンを押せるようにする処理
    void createTournamentEntryListSprite(); //トーナメントリストの作成
    void createTournamentListButton();
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

#endif /* defined(__war__TournamentEntryListLayer__) */
