#ifndef __war__RankingEventTopLayer__
#define __war__RankingEventTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "RankingEventExplainSprite.h"
#include "RankingEventListSprite.h"
#include "EventPowerSprite.h"
#include "HmsVector.h"


USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class RankingEventTopLayer : public Layer
{
private:
    
    void setPageState();
    enum PageState{
        EVENT_OPEN,
        RESULT_DISPLAY,
    };
    
    enum Order{
        O_eventPowerSprite_weak=1,
        O_explain_sp,
        O_event_time,
        O_eventPowerSprite_strong,
        O_button,
    };
    
    enum Tag{
        T_eventPowerSprite=1,
    };
        
    PageState _page_state ;
    
protected:
    EventListenerTouchOneByOne *listener;
    Json _ranking_event_data; //イベントデータ
    Json _ranking_event_score_data; //イベントのスコアデータ
    

public:
    RankingEventTopLayer() ;
    
    int cell_size;
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(RankingEventTopLayer);
    
    void initEventData(); //イベントデータの初期化
    
    void initEventLitsener(); //イベントリスナーの初期化
    
    
   
    void createPage(); //ページ全体の作成
    
    void createEventPower() ;
    void createEventExplainSprite(); //イベント説明画像の作成
    void createResultButton(); //結果ボタン作成
    void createChallengeButton(); //チャレンジボタン作成
    void createRewardListButton(); //リワード一覧ボタン
    void createHelpButton(); //イベント詳細ボタンの作成
    void createFollowInfoButton(); //友達の状況
    void createRankingButton(); //ランキング
    
    void createEndImage(); //イベントが終了したことを示す画像の作成
    void createScoreBoard(); //ランキングやスコアを表示するスプライトの作成
    string commaSeparatedString(string update_date); //文字列を三桁毎のカンマで区切る
    string createUpdateTime(string score) ;//最終更新時間の文字列を整形
    
    void changePage(); //ページの切り替え
    
    void createEventExplainData(); //イベント説明画像のデータの作成
    Sprite* createEventEndDate(string end_date); //イベント終了時刻の作成
    
    void changeExplainSprite(); //イベント説明画像
    
    //通信処理
    void httpRequestForEventInfo() ;
    void onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) ;
    void savePowerData(string event_power, string max_event_power, string event_power_use_time, string timestamp_event_power_use_time ) ;
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__RankingEventTopLayer__) */
