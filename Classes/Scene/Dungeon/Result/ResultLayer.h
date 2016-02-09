#ifndef __war__ResultLayer__
#define __war__ResultLayer__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "PointPopupSprite.h"
#include "BootyPopupSprite.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "TouchScreenSprite.h"
#include "FriendUserDetailSprite.h"
#include "ResultAchievementLayer.h"
#include "HmsCrypto.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class ResultLayer : public Layer
{
private:
    
    enum ResultLayerTag{
        TAG_MENU_ITEM,
    };
    
    EventListenerTouchOneByOne *listener;
    Sprite* _resultLabelSprite;
    Sprite* _titleSprite;
    //BootyPopupSprite* _bootyPopup;
    ResultAchievementLayer *_resultAchievementLayer ;
    PointPopupSprite* _pointPopup;
    FriendUserDetailSprite* _userDetailSprite;
    TouchScreenSprite* _touchScreenSprite ;
    
    Json json; //レスポンスJsonデータ
    int _item_count; //獲得アイテム数
    
    bool _revenge_mode = false;
    bool _revenge_effect_start_flg = false;
    bool _revenge_effect_finish_flg = false;
    bool _revenge_success_flg = false;
    
    bool _boss_event_flg = false;
    bool _boss_event_effect_start_flg = false;
    bool _boss_event_effect_finish_flg = false;
    
    //フレンド詳細の作成
    //FriendUserDetailSprite *createFriendUserDetailSprite() ;
    bool _follow_flg ; //フレンド登録対象の相手が既にフレンドであるか (オンラインでは対戦相手、オフライン系では協力カードがその対象)
    int _follow_target_friend_id; //戦闘後のフォロー対象のフレンドid
    string _follow_target_rank_name ;
    string _follow_target_level ;
    string _follow_target_user_name ;
    string _follow_target_comment ;
    string _follow_target_card_image_id ;
    
    
    //フォローのリクエスト
    void demandFollowRequest() ;
    void demandFollowCallback(HttpClient *sender, HttpResponse *response) ;
    
    //シーンきりかえ済み
    bool _change_scened_flg ;
    string _from_page_name; //どこから遷移してきたかのページ名
    
public:
    enum PopupType{
        POPUP_POINT,
        POPUP_BOOTY,
        POPUP_ACHIEVEMENT,
        POPUP_FRIENT,
    };
    
    //ポップアップを表示する順番で入れる配列（キューのように使用する）
    HmsVector<PopupType> _popup_types_queue;
    
    //ポップアップを表示する順番で入れる配列を初期化
    //void initPopupTypesOrder();
    
    //現在表示しているポップアップのインデックス
    int _now_popup_queue_index ;
    
    //直前のポップアップNodeを取得する
    Node *prevPopupNode();
    
    
    float ratio;
	BootyPopupSprite* _bootyPopup;
    ResultLayer(); //コンストラクタ
    
    static ResultLayer* create(bool winner_flg, string from_page_name = "DungeonScene");
    virtual bool init(bool winner_flg, string from_page_name);

    bool _winner_flg; //勝敗フラグ
    int _check_count; //タッチの数のチェック
    
    
    void showResultAchievementLayer() ; //ResultAchievementLayerを表示する
    ResultAchievementLayer *createResultAchievementLayer() ;//ResultAchievementLayerを生成する
    //void updateUserData(); //ユーザーデータの更新
    //void updatePresentCount(); //受け取りプレゼントカウントの更新
    void createEventListener(); //イベントリスナーの作成
    //void removeEventListener(); //イベントリスナーの削除
    void createPopup(); //ポップアップなどの作成
    //void checkRewardRankUp(); //報酬ポイントが必要条件を達成したか調べる
    void checkClearBiginnerGuide();//初心の手引きをクリアしているか調べる
    Menu *createFriendButtonMenu(); //使用したフレンドカードの持ち主をフォローするかどうかのボタンの作成

    void touchStartAction(); //タッチスタートの作成
    //void touchStartRemove(); //タッチスタート画像の削除
    
    //void createShadow(); //背景のシャドウの作成
    void showResultLabel(float dt); //バトル結果のラベル画像の表示
    //void showBattleResult(); //対戦結果の表示
    
    //void showPointPopupAction(); //ポイントゲージポップアップ画像の表示アクション
    //void showBootyPopupAction(); //獲得戦利品ポップアップ画像の表示アクション
    bool _showed_booty_popup_flg ; //獲得戦利品ポップアップ表示済みフラグ
    void showFriendRequestPopupAction(); //フレンド詳細の表示アクション
    
    void showRevengeEffect();
    void showBossEventEffect();
    void changeScene(); //ノーマルシーンへの遷移
    //void changeTutorialStatus(); //チュートリアルステータスの変更
    
    //void showEffect(Layer* layer, string filename, float scale, Point pos); //エフェクトの表示
    
    void _setCascadeOpacityEnabled(Node* parent, bool enabled); //再起的に全てに適用させる関数
    
    //通信処理
    void getPointDataRequest();
    void getPointDataCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__ResultLayer__) */
