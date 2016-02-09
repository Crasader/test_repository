#ifndef __war__TournamentEventConfirmLayer__
#define __war__TournamentEventConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "TournamentEventConfirmSprite.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class TournamentEventConfirmLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    CC_SYNTHESIZE(string, _tournament_event_list_id, TournamentEventListId); //選択トーナメント
    CC_SYNTHESIZE(string, _deck_no, DeckNo); //選択デッキ番号
    CC_SYNTHESIZE(string, _item_type, ItemType); //エントリー必要アイテム
    CC_SYNTHESIZE(string, _count, Count); //エントリー必要個数
    
    
    //タグ
    enum Tag{
        T_Bg = 1,
        T_Tournament_Info,
        T_Entry_MenuItem,
        T_Entry_Menu,
        T_Consume,
        T_Consume_Item,
        T_Consume_Label,
        T_Cancel_Bg,
        T_Cancel_Label1,
        T_Cancel_Label2,
        T_Popup_Layer,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Bg = 1,
        Z_Tournament_Info,
        Z_Entry_MenuItem,
        Z_Entry_Menu,
        Z_Consume,
        Z_Consume_Item,
        Z_Consume_Label,
        Z_Cancel_Bg,
        Z_Cancel_Label1,
        Z_Cancel_Label2,
        Z_Popup_Layer,
    };
    
    Json _tournament_data; //参加するトーナメントのデータ
    Json _entry_data_list; //参加データリスト
    Json _cancel_data_list; //キャンセルデータリスト
    
    
    Sprite* _info_bg; //情報画像
    
public:
    
    enum ConfirmStatus{
        Entry = 1,
        Cancel,
    };
    
    ConfirmStatus _confirm_status;
    
    TournamentEventConfirmLayer() ;
    
    float ratio;
    static TournamentEventConfirmLayer* create(string page_name);
    virtual bool init(string page_name);
//    CREATE_FUNC(TournamentEventConfirmLayer);
    
    void initEventData(); //イベントデータの初期化
    void initEventLitsener(); //イベントリスナーの初期化
    
    void createTournamentInfoSprite(); //トーナメント情報の画像
    
    void createTournamentJoinButton(); //トーナメントに参加ボタンの作成
    void showJewelConsumeSprite(); //宝玉使用警告画像
    void createCancelLabel(); //キャンセル文字列の表示
    
    //トーナメント情報取得通信処理
    void httpRequestForEventInfo() ;
    void onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) ;
    
    //トーナメント参加通信処理
    void httpRequestForTournamentEntry() ;
    void getTournamentEntryCallback(HttpClient *sender, HttpResponse *response) ;
    
    //トーナメントキャンセル通信処理
    void httpRequestForTournamentCancel() ;
    void getTournamentCancelCallback(HttpClient *sender, HttpResponse *response) ;
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentEventConfirmLayer__) */
