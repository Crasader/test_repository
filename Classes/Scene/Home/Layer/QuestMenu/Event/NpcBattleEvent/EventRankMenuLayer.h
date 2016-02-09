#ifndef __war__EventRankMenuLayer__
#define __war__EventRankMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "EventExplainSprite.h"
#include "EventListSprite.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class EventRankMenuLayer : public Layer, public ScrollViewDelegate
{
private:
    enum Power{
        GO = 1,
        LESS_MAX_POWER,
        LESS_POWER,
    };
    
    enum Tag{
        T_Sum_Bg = 12,
        T_Detail_MenuItem,
        T_Detail_Menu,
    };
    
protected:
    EventListenerTouchOneByOne *listener;
    Json _event_rank_data; //イベントランクデータ
    int _event_rank_count; //イベントランクの数
    
    HmsVector<EventListSprite *> _event_rank_sp; //イベントランクごとのボタン画像
    
public:
    ~EventRankMenuLayer(); //デストラクタ
    
    int cell_size;
    float ratio;
    string name_or_reward = "name";    
    bool touchButtonFlg;
    EventExplainSprite* _explain_sp; //イベント説明画像
    
    virtual bool init();
    CREATE_FUNC(EventRankMenuLayer);
    
    void setEventRankData(); //イベントランクデータの設定
    void initEventEnemyData(); //イベントデータの初期化
    void createEventExplainSprite(); //イベント説明画像の作成
    void createSwitchButton();
    void createSumRewardSprite(); //報酬（星）の数の画像の作成
    void createStarHighlight(); //スターを「星を5つ集めよう」という表示
    
    void initEventLitsener(); //イベントリスナーの初期化
    void createScrollView(); //スクロールビューの作成
    Layer* createList(); //リストの作成
    
    void createDetailButton(); //イベント詳細ボタンの作成
    
    Power checkPower(int need_power); //体力が足りているかチェック
    void changePage(int tag); //ページの切り替え
    void saveUseDungeonData(string npc_battle_event_rank_list_id); //ダンジョンシーンで使用するデータを設定
    void saveBattleUserSpriteData(EventListSprite* event_sp); //最終確認で表示する対戦相手画像のデータ
    void saveEventPointData(EventListSprite* event_sp); //イベントのポイントデータを設定
    
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
    
    void update(float dt); //現在の残行動力を調べる
    void onEnter(); //初期実行関数
};


#endif /* defined(__war__EventRankMenuLayer__) */
