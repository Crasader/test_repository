#ifndef __War__CardGalleryLayer__
#define __War__CardGalleryLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "MypageChara.h"
#include "CharacterDetailSprite.h"
#include "CharaDetailPopupLayer.h"

#define ZORDER_BACK_CARD 10
#define ZORDER_FRONT_CARD 20

#define BACK_CARD_TAG 100
#define FRONT_CARD_TAG 200

#define KIRA_START_POS_X -90
#define KIRA_START_POS_Y 
#define KIRA_END_POS_X
#define KIRA_END_POS_Y

#define CARD_IMAGE_SIZE 400
#define CARD_ARRAY_SIZE 20

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class CardGalleryLayer : public XTLayer
{
private:
    //カードが宙に浮くアニメーションのスケジュール
    void floatAnimationUpdate(float dt) ;
    float _float_time ;//アニメーションの経過時間
    float _floatEffectRate[CARD_ARRAY_SIZE] ; //カードインデックス毎の浮遊効果の倍率(中心に近い物のみ浮く)
    float calcFloatDistanceByTime(float time) ; //時間毎の浮遊距離を求める
    float f7(float x) ;
    
    
    //カードの中心位置
    Point _card_center_position ;
    
    EventListenerTouchOneByOne *listener;
    string _party_no; //現在のパーティナンバーの取得
    Json _user_deck_list; //現在選択しているデッキリストの取得
    
    //移動位置、回転角度を決める変数 0~19の値を取り 0以下や19以上になったときは戻す 名前は保留
    float _card_state_decide_number ;
    void updateCardPositionAndRotate();
    
    //キャラ
    int card_index;
    
    HmsVector<Node*> _nodes; //カードの中心点
	HmsVector<Sprite*> _cards;	//カードの画像
	HmsVector<Sprite*> _reflection_cards;	//反射しているカードの画像
    
    HmsVector<Sprite*> _card_names; //カードの名前画像
	HmsVector<Sprite*> _card_frame;	//カードのフレーム画像
    
	
	//レア度に関する変数
	HmsVector<Sprite*> _card_rare;	//カードのレア画像
    
	
    //タップ
    Point _touchPoint;
    bool touchCard = false;
    bool touchMove = false;
    int touchTag = 0;
    Point _movePoint;
    
    //詳細情報の表示を行う距離を判定するしきい値
    float show_detail_detect_distance ;
    
    Json _event_list; //イベント情報のリスト
    
    //微小距離xによってカードの位置を移動する もしここに微小距離以外を与えると不自然に動く
    void moveCardsByDeltaX(float delta_x) ;
    
    
    //スクロールの初速度を過去十回の微小区間の移動距離保存しておき、その平均を求めるために使用する
    float delta_xs[10] ;
    float _scroll_detect_speed;
    int on_touch_move_call_count ; //onTouchが呼ばれた回数
    
    //速度を与えてスクロールさせる  スクロールするときはtureを返してます
    bool scrollByVelocity(float velocity) ;
    //スクロールを計算するためのupdate関数
    void updateForScrollByVelocity(float dt) ;
    //スクロール中かどうか
    bool now_scroll ;
    
    //中心位置に自動で移動する
    void autoScrollToCenter() ;
    //自動で指定のindexまで移動する
    void autoScrollByTargetIndex(int target_index) ;
    //自動スクロールの為に格納する目的のindexを格納
    int _autoScrollDirectionIndex ;
    //自動スクロールの初速度を格納
    float _initial_velocity ;
    //自動スクロールの経過秒数
    float _autoScrollProgressTime;
    //中心位置に自動で移動するときに利用するupdate関数
    void updateForAutoScrollToCenter(float dt) ;
    
    //初速度
    float _init_scroll_velocity ;
    
    //加速度
    int _acceleration ;
    
    //スクロール開始から経過した時間
    float _now_time;
    
    
    //インデックスに応じた中心からのZ軸方向の距離を求める
    float calcCardCenterDifferZByIndex(int target_index) ;
    float f1(float x) ;
    float f5(float x) ;
    
    //インデックスに応じた中心からのX軸方向の距離を求める
    float calcCardCenterDifferXByIndex(int target_index) ;
    float f2(float x) ;
    float f3(float x) ;
    
    //インデックスに応じたY軸を回転軸としたときの回転角度を求める
    float calcCardRotationAxisYByIndex(int target_index) ;
    float f4(float x) ;
    
    //インデックスに応じたカードの色合いを求める
    int calcCardColorByteByIndex(float target_index) ;
    float f6(float x) ;
    
    //カードの浮遊効果の影響を受ける倍率を求める
    float calcFloatEffectRateByIndex(int target_index) ;
    float f8(float x); 
    
    //中心に最も近いカードを求める
    int calcNearestCenterCard(float target_index) ;
    
    //前回サウンドが鳴ったindexを保持
    int _pre_sound_index ;
    void swipeSound() ;
    
    void showCenterCardName();
    
public:
    CardGalleryLayer(); //コンストラクタ
    
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(CardGalleryLayer);
    
    void createEventListener(); //イベントリスナーの作成
    void setupUserDeckList(); //ユーザデッキリストの読み込み
    
    void initCards(); //カードの作成
    void initCamera(); //カメラの初期化
    
    CharaDetailPopupLayer *detail ; //カード詳細を表示するレイヤー
    
    //詳細画面表示中か
    bool _show_chara_detail_flg ;
    //詳細画像を表示
    void showCharaDetail(float dt) ;
    //詳細画面が閉じられたときに呼ばれるコールバック
    void closeCharaDetailCallback(float dt) ;
    
    //base_indexが変更されたときに呼び出されるコールバック
    void changeBaseCallback(float dt); 
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    
    void onEnter(); //初期実行関数
    
    //タッチ位置でカードをスクロールできるか判定
    bool checkContainCardPosition(Point touch_point) ;
};

#endif /* defined(__War__CardGalleryLayer__) */
