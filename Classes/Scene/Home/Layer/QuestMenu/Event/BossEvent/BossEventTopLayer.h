#ifndef __war__BossEventTopLayer__
#define __war__BossEventTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "BossEventExplainSprite.h"
#include "BossEventListSprite.h"
#include "PopupOkButtonLayer.h"
#include "HmsVector.h"


USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class BossEventTopLayer : public Layer
{
protected:
    EventListenerTouchOneByOne *listener;
    BossEventExplainSprite *explain_sp;
    Json _boss_event_data; //イベントデータ
    Json _boss_event_score_data_personal; //イベントのスコアデータ(個人)
    Json _boss_event_score_data_team; //イベントのスコアデータ(チーム)
    Json _boss_event_stage_data; //イベントのステージデータ
    int _event_count; //イベントの数
    int _boss_count = 0;
public:
    BossEventTopLayer() ;
    
    int cell_size;
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(BossEventTopLayer);
    
    void initEventData(); //イベントデータの初期化
    
    void initEventLitsener(); //イベントリスナーの初期化
    
    void createPage(); //ページ全体の作成
    void createBossEventExplainSprite(); //イベント説明画像の作成
    void createButton(); //ボタン作成
    void createRewardListButton(); //リワード一覧ボタン
    void createHelpButton(); //イベント詳細ボタンの作成
    void createRankingButton(); //ランキング
    
    void createRecordTeam(); //チームの順位、スコア表示
    void createRecordPersonal(); //個人の順位、スコア表示
    
    string commaSeparatedString(string update_date); //文字列を三桁毎のカンマで区切る
    string createUpdateTime(string score) ;//最終更新時間の文字列を整形
    
    void changeBattlePage(); //ページの切り替え(出陣)
    void changeTeamPage(); //ページの切り替え(チーム)
    
    void createEventExplainData(); //イベント説明画像のデータの作成
    Sprite* createEventEndDate(string end_date); //イベント終了時刻の作成
    
    void changeExplainSprite(); //イベント説明画像
    
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

#endif /* defined(__war__BossEventTopLayer__) */
