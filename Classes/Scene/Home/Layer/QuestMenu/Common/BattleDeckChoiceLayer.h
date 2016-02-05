#ifndef __war__BattleDeckChoiceLayer__
#define __war__BattleDeckChoiceLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "EnemySprite.h"
#include "QuestEnemySprite.h"
#include "DeckSprite.h"
#include "EventEnemySprite.h"
#include "HomeCommon.h"

USING_NS_CC;
using namespace std;

class BattleDeckChoiceLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _page_name;
    
    //デッキ画像の表示位置
    enum Pos{
        Left = 1,
        Middle,
        Right,
    };
    
    vector<DeckSprite *> _deck_sp; //デッキ画像
    CC_SYNTHESIZE(int, _now_deck_no, NowDeckNo); //現在のデッキ番号
    
    bool _touch_flg; //デッキ画像タッチフラグ
    
    Sprite* _left_arrow; //左矢印
    Sprite* _right_arrow; //右矢印
    
    Json _user_party_list; //ユーザーのパーティーリスト
    
public:
    
    //タグ
    enum Tag{
        T_Left_Arrow = 1,
        T_Right_Arrow,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Left_Arrow = 1,
        Z_Right_Arrow,
    };
    
    float ratio;
    BattleDeckChoiceLayer();
    
    static BattleDeckChoiceLayer* create(string page_name);
    virtual bool init(string page_name);
    
    void initEventListener(); //イベントリスナーの作成
    void showTitleBar(); //タイトルバーの表示
    void setupData(); //データの読み込み
    
    void createButton(); //出陣ボタン
    void createEnemySprite(); //敵画像の作成
    void showVsSprite(); //vs画像の表示
    void createDeckSprite(); //デッキ画像の作成
    void createArrowSprite(); //矢印画像の作成
    void actionArrowSprite(); //矢印画像のアクション
    
    void onTouchShutujinButton(Ref* sender) ;
    void changeDungeonScene(); //ダンジョンシーンへの遷移
    
    void checkEventTimeRequest(); //イベント期間中か確認するRequest
    void checkEventTimeCallbabck(HttpClient *sender, HttpResponse *response) ; //イベント期間中か確認するcallback
    void checkRankingEventTimeRequest(); //ランキングイベント期間中か確認するRequest
    void checkRankingEventTimeCallbabck(HttpClient *sender, HttpResponse *response) ; //ランキングイベント期間中か確認するcallback
    void checkBossEventTimeRequest(); //ボスイベント期間中か確認するRequest
    void checkBossEventTimeCallbabck(HttpClient *sender, HttpResponse *response) ; //ボスイベント期間中か確認するcallback
    
    bool isEventBattle();
    bool isRankingEventBattle();
    bool isBossEventBattle();
    bool isChallengeEventBattle();
    
    int calcArrayNo(Pos position, int deck_no); //デッキ番号から配列上での番号を計算する
    void showTouchDeckSprite(); //タップされた時左右に画像を表示させる関数
    void moveTouchDeckSprite(float delta); //タップ時に画像を移動させる関数
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__BattleDeckChoiceLayer__) */
