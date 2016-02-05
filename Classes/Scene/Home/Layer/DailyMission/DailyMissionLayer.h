#ifndef __war__DailyMissionLayer__
#define __war__DailyMissionLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "DailyMissionClearAnimLayer.h"
#include "DailyMissionJsonParser.h"
#include "DailyMissionGroupData.h"
#include "DailyMissionGroupSprite.h"
#include "ScrollSprite.h"
#include "json11.hpp"
#include "HmsVector.h"
#include "cocos-ext.h"
USING_NS_CC_EXT;

#define MISSION_GROUP_LIST_SIZE_Y 322

USING_NS_CC;
using namespace json11;

class DailyMissionLayer : public cocos2d::Layer
{
private:
    DailyMissionLayer(); //コンストラクタ
    
    HmsVector<DailyMissionGroupData *> loadData() ;
    void createDailyMissionGroup(HmsVector<DailyMissionGroupData *> daily_mission_group_data_list) ;
    void createBg(HmsVector<DailyMissionGroupData *> daily_mission_group_data_list); //背景画像の作成
    
    HmsVector<DailyMissionGroupSprite *> _daily_mission_group ;
    
    ScrollSprite *_scroll_sprite ;
    
    
    enum Tag{
        T_BACK,
        T_BACK_TOP,
        T_SCROLL,
        T_DAILY_MISSION_GROUP,
    };
    
    enum Order{
        O_CHALLENGE_BUTTON,
        O_SHADOW,
        O_BACK,
        O_BACK_TOP,
        O_SCROLL,
        O_DAILY_MISSION_GROUP,
        O_CLOSE,
    };
    
    
    float ratio;
    
    
    EventListenerTouchOneByOne *listener;
    void popupAction(); //ポップアップ表示
    void showStamp() ; //スタンプ表示
    void deleteAction(float dt); //ポップアップ削除アクション
    
    ui::Scale9Sprite * _bg; //背景画像
    Label* _reward_count_label; //初心の手引きの報酬の個数のラベル
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
    
    //アニメーションが一つ終了した時のコールバック
    void endClearAnimationCallback(float dt) ;
    //次のアニメーションの表示
    void showNextClearAnimation() ;
    //次のアニメーションがあるか
    bool hasNextClearAnimation() ;
    //スタンプが押されるActionの生成
    ActionInterval* createStampAction() ;
    
    void showCompleteAnimation(); //全て達成したときのアニメーションレイヤーの表示
    
    Menu* createGuideDetail(string change_page) ;
    void showActiveAction() ;
    void showGuideDetailAction(); 
    
    
    //スタンプを全て表示(アニメなし)
    void showAllNewClearStamp();
    void showAllOldClearStamp();
    
public:
    
    static DailyMissionLayer* create();
    virtual bool init();
    
    
    void initEventListener(); //イベントリスナーの登録
    void createButton(); //ボタンの作成
    
    void changeChallengePage(string change_page); //チャレンジページへの遷移
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
};

#endif /* defined(__war__DailyMissionLayer__) */
