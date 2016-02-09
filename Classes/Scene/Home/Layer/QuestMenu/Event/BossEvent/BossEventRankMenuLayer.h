#ifndef __war__BossEventRankMenuLayer__
#define __war__BossEventRankMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "BossEventExplainSprite.h"
#include "BossEventListSprite.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class BossEventRankMenuLayer : public Layer, public ScrollViewDelegate
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
    };
    
protected:
    EventListenerTouchOneByOne *listener;
    Json _boss_event_stage_data; //イベントランクデータ
    int _event_stage_count; //イベントランクの数
    
    HmsVector<BossEventListSprite *> _event_rank_sp; //イベントランクごとのボタン画像
    
public:
    ~BossEventRankMenuLayer(); //デストラクタ
    
    int cell_size;
    float ratio;
    string name_or_reward = "name";    
    bool touchButtonFlg;
    BossEventExplainSprite* _explain_sp; //イベント説明画像
    
    virtual bool init();
    CREATE_FUNC(BossEventRankMenuLayer);
    
    void setEventRankData(); //イベントランクデータの設定
    void initEventEnemyData(); //イベントデータの初期化
    
    void createEventExplainSprite(); //イベント説明画像の作成
    
    void initEventLitsener(); //イベントリスナーの初期化
    void createScrollView(); //スクロールビューの作成
    void showKey(); //鍵の表示
    Layer* createList(); //リストの作成
    
    void createDetailButton(); //イベント詳細ボタンの作成
    
    UserCondition checkUserCondition(int need_power, bool open_flg); //体力が足りているか、開放済みかチェック
    void changePage(int tag); //ページの切り替え
    
    void saveUseDungeonData(string boss_event_stage_id); //ダンジョンシーンで使用するデータを設定
    void saveBattleUserSpriteData(BossEventListSprite* event_sp); //最終確認で表示する対戦相手画像のデータ
    
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
    
    void onEnter(); //初期実行関数
    
    
};


#endif /* defined(__war__BossEventRankMenuLayer__) */
