#ifndef __war__TournamentEventTopLayer__
#define __war__TournamentEventTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "RankingEventExplainSprite.h"
#include "RankingEventListSprite.h"
#include "EventPowerSprite.h"
#include "TournamentEventConfirmSprite.h"
#include "HmsVector.h"
#include "TournamentEventTopSprite.h"
#include "TournamentRemainTimeSprite.h"
#include "TournamentRecordLayer.h"
#include "TournamentStartAnimationLayer.hpp"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class TournamentEventTopLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Bg = 1,
        T_Help_MenuITem,
        T_Help_Menu,
        T_Title_Sprite,
        T_Info_Sprite,
        T_Info_Label,
        T_Navi_Sprite,
        T_Navi_Label,
        T_Tournament_List,
        T_Recode,
        T_Bracket,
        T_Tournament_Time,
        T_Popup_Layer,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Bg = 1,
        Z_Help_MenuITem,
        Z_Help_Menu,
        Z_Title_Sprite,
        Z_Info_Sprite,
        Z_Info_Label,
        Z_Navi_Sprite,
        Z_Navi_Label,
        Z_Tournament_List,
        Z_Recode,
        Z_Bracket,
        Z_Tournament_Time,
        Z_Popup_Layer,
    };
    
    Json _tournament_data_list; //参加しているトーナメントの情報
    
    Sprite* _info_bg; //トーナメント情報画像
    
public:
    
    //トーナメントの状態
    enum TournamentStatus{
        TournamentEvent_Not_Open = 0,
        Not_Entry,
        Wait_Bracket,
        Wait_Tournamnt_Start,
        Open_Tournament,
        Battle_Tournament,
        Close_Tournament,
    };
    
    TournamentStatus _tournament_status; //トップ画面に表示するトーナメント参加状況の表示
    TournamentEventTopLayer() ;
    
    float ratio;

    static TournamentEventTopLayer* create();
    virtual bool init();
//    CREATE_FUNC(TournamentEventTopLayer);
    
    void initEventData(); //イベントデータの初期化
    void initEventLitsener(); //イベントリスナーの初期化
    
    void createHelpButton(); //ヘルプボタンの作成
    void createTournamentTitle(); //トーナメントタイトル画像の作成
    void createTournamentInfoSprite(string image_path); //トーナメント情報の画像
    void createNaviSprite(string navi_comment); //ナビスプライトの作成
    void createTournamentButton(string image_path); //トーナメント一覧、又は参加キャンセルなどのボタン
    void createWebViewButton(); //戦績、報酬一覧などWebViewで表示するボタン
    void createNextBattleTimeSprite(); //次の対戦時間までの画像の表示
    
    void setCallbackTournamentData(); //コールバックで返ってきたデータの設定
    
    void blinkEyes(float dt); //貂蝉の瞬き
    
    //通信処理
    void httpRequestForEventInfo() ;
    void onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) ;
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentEventTopLayer__) */
