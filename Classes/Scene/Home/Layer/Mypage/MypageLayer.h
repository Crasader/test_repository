#ifndef __War__MypageLayer__
#define __War__MypageLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CardGalleryLayer.h"
#include "HmsUtilityFunction.h"
#include "BeginnerGuideLayer.h"
#include "DailyMissionLayer.h"
#include "GooglePlayReviewLayer.h"
#include "MypageInfoLayer.h"
#include "BannerSprite.h"
#include "TournamentEventTopSprite.h"
#include "TournamentRemainTimeSprite.h"
#include "ComebackCampaignLayer.hpp"
#include "HmsActionInstant.h"
#include "ChatBaseUpperLayer.h"


USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class MypageLayer : public Layer
{
private:
    
    HmsVector<BannerSprite *> _slide_banner; //バナーのスプライト
    HmsVector<Sprite *> _banner_dot; //バナーの個数を示す点
    bool _touch_disable ;
    int _now_center_slide_banner_index ;
    
    string _mutual_link_url ; //相互リンクurl
    
    EventListenerTouchOneByOne *listener;
    
    Json _comeback_present_data_list; //カムバックキャンペーンの付与プレゼントリスト
    Json _multi_event_list; //イベント情報のリスト
    Json _login_present_list; //ログインプレゼントリスト
    Json _tournament_data_list; //トーナメントデータリスト
    Json _mypage_info; //お知らせ情報
    
    //数を表示する赤いサークルを作成
    void showRedcircleCount(int count, Point p);
    
    
    //バナーの表示位置を示す点を表示
    void updateBannerDot() ;
    
public:
    
    enum Tag{
        T_CardGallery_Layer = 1,
        T_Beginner_MenuItem,
        T_Beginner_Menu,
        T_Challenger,
        T_GACHA_BANNER,
        T_BBS_BANNER,
        
        T_NPC_BATTLE_EVENT_BANNER,
        T_RANKING_EVENT_BANNER,
        T_BOSS_EVENT_BANNER,
        T_CHALLENGE_EVENT_BANNER,
        T_TOURNAMENT_EVENT_BANNER,
        T_MUTUAL_LINK_BANNER,
        T_CHANGE_PAGE_BANNER, 
        
        T_RED_CIRCLE_TAG,
        T_PRESENT_LABEL_TAG,
        T_RED_CIRCLE_INFO_TAG,
        T_INFO_LABEL_TAG,
        T_Beginner_Ballon,
        T_Tournament_Time,
        T_Popup_Layer,
    };
    
    enum ZOrder{
        Z_CardGallery_Layer = 1,
        Z_Beginner_MenuItem,
        Z_Beginner_Menu,
        Z_Challenger,
        Z_GACHA_BANNER,
        Z_BBS_BANNER,
        
        Z_NPC_BATTLE_EVENT_BANNER,
        Z_RANKING_EVENT_BANNER,
        Z_BOSS_EVENT_BANNER,
        Z_CHALLENGE_EVENT_BANNER,
        Z_TOURNAMENT_EVENT_BANNER,
        Z_MUTUAL_LINK_BANNER,
        
        Z_RED_CIRCLE_TAG,
        Z_PRESENT_LABEL_TAG,
        Z_RED_CIRCLE_INFO_TAG,
        Z_INFO_LABEL_TAG,
        Z_Beginner_Ballon,
        Z_Tournament_Time,
        Z_Popup_Layer,
    };
    
    enum PopupStatus{
        ComebackBonus,
        LoginBouns,
        MypageInfo,
        Beginner,
        Review,
        DailyMission,
    };
    
    std::queue<PopupStatus > _popup_status_queue; //キュー
    
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(MypageLayer);
    
    MypageLayer(); //コンストラクタ
    
    void createEventListener(); //イベントリスナーの作成
    bool checkEndDate(string end_date); //終了時刻を過ぎていないかチェックする
    void createCardGalleryLayer(); //createCardGalleryLayerの追加
    void createButton(); //ボタンの作成
    void createEventBanner(); //イベントのバナーの作成
    int eventTypeToBannerTag(string event_type) ; //イベントタイプからタグを取得
    void touchStart(); //タッチスタートアクションの開始
    
    void showBeginnerButton(); //初心の手引き表示
	
    void mypageRequest(); //ログインボーナスチェック・お知らせ数・NEWマーク表示項目のチェック
    void mypageCallback(HttpClient *sender, HttpResponse *response); //ログインボーナスチェック・お知らせ数・NEWマーク表示項目のチェック
    
    void dailyMissionCheckRequest() ;
    void dailyMissionCheckCallback(HttpClient *sender, HttpResponse *response); //小喬(デイリーミッション)の館条件の達成の判定
    
    void afterLoginBonusCheck() ; //ログインボーナスの後に呼ぶメソッド
    
    void checkUserLevel(Json json); //ユーザーレベルのチェック
    void showBeginnerComment(); //ビギナーコメントの表示
    
    void checkOpenCheckBeginnerGuideLayer(); //ビギナーレイヤーを表示するかチェック
    void showChallengerButton(Json json); //挑戦者ボタンの表示
    
    void checkOpenCheckDailyMissionLayer(); //デイリーミッションを表示するかチェック
    
    void updatePresentCount(int count); //プレゼント数を更新する
    
    void showNewSprite(Json json); //New表示の表示
    void showTournamentNextTimeLabel(); //トーナメント次戦残り時間の表示
    
    void createPopupStatusQueue(bool display_daily_mission); //ポップアップキューの作成
    void showNextPopupStatus(); //ポップアップステータスを進める
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    
    void blinkingEyes(float dt); //瞬きアクション
    
    void onEnter(); //初期実行関数
	
    //バナーをスライドする
    enum SwipeDirection{
        LEFT,
        RIGHT,
        CENTER
    };
    void updateAutoSwipeBanner(float dt) ;
    void swipeBanner(SwipeDirection swipe_direction) ;
    void initBannerPosition(SwipeDirection swipe_direction) ; //バナー位置調整
    void initBannerVisible(SwipeDirection swipe_direction) ; //バナー不可視状態調整
    
    bool containBanner(Point p) ;
        
    int calcRightBannerTagIndex(int index);
    int calcLeftBannerTagIndex(int index) ;
};

#endif /* defined(__War__MypageLayer__) */
