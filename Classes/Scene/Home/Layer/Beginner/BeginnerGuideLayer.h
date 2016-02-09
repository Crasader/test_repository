#ifndef __war__BeginnerGuideLayer__
#define __war__BeginnerGuideLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "BeginnerGuideClearAnimLayer.h"
#include "json11.hpp"

#define MAX_STAMP_COUNT 12

#define COMPLETE_ID 13

USING_NS_CC;
using namespace json11;

class BeginnerGuideLayer : public cocos2d::Layer
{
private:
    BeginnerGuideLayer(); //コンストラクタ
    
    float ratio;
    
    enum Tag{
        T_Popup = 1,
    };
    
    enum Order{
        O_Popup = 1,
        O_Stamp_Back,
        O_Back_Label,
        O_Active,
        O_Guide_Detail,
        O_Stamp,
    };
    
    enum Type{
        Free_Gacha = 1,
        Friend_Follow,
        Deck_Organization,
        Level_Three,
        Level_Eight,
        Level_Thirteen,
        Event_Easy,
        Event_Normal,
        Event_Hard,
        Online_Quick,
        Online_Room,
        Online_Win,
    };
    
    EventListenerTouchOneByOne *listener;
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(float dt); //ポップアップ削除アクション
    
    Sprite* _bg; //背景画像
    Label* _guide_detail_label ; //初心の手引き詳細
    Label* _reward_count_label; //初心の手引きの報酬の個数のラベル
    Sprite* _reward_prite ;
    Sprite* _guide_detail_sprite ; //詳細のスプライト
    bool _beginner_guide_complete_flg; //ビギナーガイドをコンプリートしたかフラグ
    
    //無効なチャレンジボタン画像
    Sprite *_disable_challenge_btn ;
    
    //チャレンジボタン
    MenuItemSprite *_challenge_btn ;
    
    //クリア済みの判子
    Sprite *_detail_cleared_mark ;
    Json json ;
    
    //stampの画像
    HmsVector<Sprite *> _stamp;
    
    //stampの背景画像
    HmsVector<Sprite *> _panel;
    
    //クリアー状況の取得と報酬付与のリクエスト
    void getBeginnerGuideDataRequest();
    void getBeginnerGuideDataCallback(HttpClient *sender, HttpResponse *response) ;
    
    
    void createStamp() ;//クリアー状況のスタンプを作成
    void createPanels() ;//クリアー状況のスタンプを作成
    void showStampAndSetAnimationData(); //アニメーションを表示するデータを設定する
    HmsVector<int> _animation_guide_ids; //アニメーションを行うidを保存する
    int _next_animation_guide_index; //アニメーションを行うindexを保持
    
    //アニメーションが一つ終了した時のコールバック
    void endClearAnimationCallback(float dt) ;
    //次のアニメーションの表示
    void showNextClearAnimation() ;
    //次のアニメーションがあるか
    bool hasNextClearAnimation() ;
    //スタンプが押されるActionの生成
    ActionInterval* createStampAction() ;
    
    void showCompleteAnimation(); //全て達成したときのアニメーションレイヤーの表示
    
    void createGuideDetail() ;
    void updateActiveGuideDetail(int beginner_guide_id) ; //アクティブな詳細を切り替える(ここで詳細の切り替えは行う)
    void showActiveAction() ;
    void showGuideDetailAction(); 
    
    Sprite *_active_sprite ;
    int _active_guide_detail_id ;//現在詳細を表示しているID 0のときはポップアップ表示時
    int calcInitialGuideDetailId() ; //クリア状況を見て最初に表示する詳細を計算する
    
public:
    
    static BeginnerGuideLayer* create();
    virtual bool init();
    
    void initEventListener(); //イベントリスナーの登録
    void createBg(); //背景画像の作成
    void createButton(); //ボタンの作成
    
    void changeChallengePage(); //チャレンジページへの遷移
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__BeginnerGuideLayer__) */
