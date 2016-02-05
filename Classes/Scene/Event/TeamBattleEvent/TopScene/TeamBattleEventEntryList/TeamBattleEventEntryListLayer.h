#ifndef __war__TeamBattleEventEntryListLayer__
#define __war__TeamBattleEventEntryListLayer__

#include "cocos2d.h"
#include "TeamBattleEventEntryListSprite.h"

USING_NS_CC;

class TeamBattleEventEntryListLayer : public Layer, public ScrollViewDelegate
{
private:
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<TeamBattleEventEntryListSprite *> _team_battle_list; //チーム対抗戦画像リスト
    json11::Json _team_battle_data_list; //チーム対抗戦情報のリスト
    
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
    time_t _team_battle_entry_interval; //エントリー可能時間
    
    float ratio;
    TeamBattleEventEntryListLayer(); //コンストラクタ
    
    static TeamBattleEventEntryListLayer* create();
    virtual bool init();
//    CREATE_FUNC(TeamBattleEventEntryListLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void updateCheckEntryStartDate(float dt); //エントリー開始時間になったらボタンを押せるようにする処理
    void createTeamBattleEntryListSprite(); //チーム対抗戦リストの作成
    void createEntryListButton(); //チーム対抗戦リストの作成
    void createScrollView(); //スクロールビューの作成
    Layer* createList(); //リストの作成
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //通信処理
    void getTeamBattleEventInfoRequest();
    void teamBattleEventInfoCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    void entryFinish(string _team_battle_event_list_id); //エントリー通信
    void entryFinishCallBack(HttpClient *sender, HttpResponse *response);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TeamBattleEventEntryListLayer__) */
