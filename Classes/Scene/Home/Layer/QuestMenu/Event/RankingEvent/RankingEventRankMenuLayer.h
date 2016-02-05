#ifndef __war__RankingEventRankMenuLayer__
#define __war__RankingEventRankMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "RankingEventExplainSprite.h"
#include "RankingEventListSprite.h"
#include "EventPowerSprite.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class RankingEventRankMenuLayer : public Layer, public ScrollViewDelegate
{
private:
    enum UserCondition{
        GO = 1,
        LESS_MAX_POWER,
        LESS_POWER,
        STAGE_CLOSE,
    };
    
    enum Tag{
        T_Sum_Bg = 12,
        T_Detail_MenuItem,
        T_Detail_Menu,
        T_eventPowerSprite=15,
    };
    
    enum Order{
        O_eventPowerSprite_weak=1,
        O_RankingEventExplainSprite,
        O_eventPowerSprite_strong,
    };
    
protected:
    EventListenerTouchOneByOne *listener;
    Json _ranking_event_stage_data; //イベントランクデータ
    int _event_stage_count; //イベントランクの数
    
    HmsVector<RankingEventListSprite *> _event_rank_sp; //イベントランクごとのボタン画像
    
public:
    ~RankingEventRankMenuLayer(); //デストラクタ
    
    int cell_size;
    float ratio;
    string name_or_reward = "name";    
    bool touchButtonFlg;
    RankingEventExplainSprite* _explain_sp; //イベント説明画像
    
    EventPowerSprite *_eventPowerSprite ;
    
    virtual bool init();
    CREATE_FUNC(RankingEventRankMenuLayer);
    
    void setEventRankData(); //イベントランクデータの設定
    void initEventEnemyData(); //イベントデータの初期化
    
    void createEventPower(); //パワーの表示
    void createEventExplainSprite(); //イベント説明画像の作成
//    void createSwitchButton();
    void createSumRewardSprite(); //報酬（星）の数の画像の作成
    
    void initEventLitsener(); //イベントリスナーの初期化
    void createScrollView(); //スクロールビューの作成
    void showKey(); //鍵の表示
    Layer* createList(); //リストの作成
    
    void createDetailButton(); //イベント詳細ボタンの作成
    
    UserCondition checkUserCondition(int need_power, bool open_flg); //体力が足りているか、開放済みかチェック
    void changePage(int tag); //ページの切り替え
    
    void recoveryPower(float dt);
    void saveUseDungeonData(string ranking_event_stage_id, string floor_count); //ダンジョンシーンで使用するデータを設定
    void saveBattleUserSpriteData(RankingEventListSprite* event_sp); //最終確認で表示する対戦相手画像のデータ
    void saveEventPointData(RankingEventListSprite* event_sp); //イベントのポイントデータを設定
    
    void _setCascadeOpacityEnabled(Node* parent, bool enabled); //再起的に透過率を全てに適用させる関数
    void _setCascadeColorEnabled(Node* parent, bool enabled); //再起的に色合いを全てに適用させる関数
    
    void soundNG(); //NG音の再生
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
   // void update(float dt); //現在の残行動力を調べる
    void updateCellInfo(float dt); //セルの状況を調べてアップデート
    void onEnter(); //初期実行関数
    
    
};


#endif /* defined(__war__RankingEventRankMenuLayer__) */
