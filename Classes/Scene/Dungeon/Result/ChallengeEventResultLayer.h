#ifndef __war__ChallengeEventResultLayer__
#define __war__ChallengeEventResultLayer__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "TouchScreenSprite.h"
#include "ScorePopupSprite.h"
#include "ChallengeEventPopupSprite.h"

#include "HmsCrypto.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class ChallengeEventResultLayer : public Layer
{
private:
    
    enum ChallengeEventResultLayerTag{
        TAG_MENU_ITEM,
    };
    
    EventListenerTouchOneByOne *listener;
    Sprite* _resultLabelSprite;
    Sprite* _titleSprite;
    ChallengeEventPopupSprite* _popup;
    TouchScreenSprite* _touchScreenSprite ;
    
    Json json; //レスポンスJsonデータ
    
    //シーンきりかえ済み
    bool _change_scened_flg ;
    string _from_page_name; //どこから遷移してきたかのページ名
    
public:
    enum PopupType{
        POPUP_POINT,
    };
    
    //ポップアップを表示する順番で入れる配列（キューのように使用する）
    HmsVector<PopupType> _popup_types_queue;
    
    //ポップアップを表示する順番で入れる配列を初期化
    void initPopupTypesOrder();
    
    //現在表示しているポップアップのインデックス
    int _now_popup_queue_index ;
    
    //直前のポップアップNodeを取得する
    Node *prevPopupNode();
    
    
    float ratio;
	BootyPopupSprite* _bootyPopup;
    ChallengeEventResultLayer(); //コンストラクタ
    
    static ChallengeEventResultLayer* create(bool winner_flg, string from_page_name = "DungeonScene");
    virtual bool init(bool winner_flg, string from_page_name);

    bool _winner_flg; //勝敗フラグ
    int _check_count; //タッチの数のチェック
    
    void createEventListener(); //イベントリスナーの作成
    void removeEventListener(); //イベントリスナーの削除
    void createPopup(); //ポップアップなどの作成

    void touchStartAction(); //タッチスタートの作成
    void touchStartRemove(); //タッチスタート画像の削除
    
    void createShadow(); //背景のシャドウの作成
    void showChallengeEventResultLabel(float dt); //バトル結果のラベル画像の表示
    void showBattleChallengeEventResult(); //対戦結果の表示
    
    void showPointPopupAction(); //ポイントゲージポップアップ画像の表示アクション
    
    void changeScene(); //ノーマルシーンへの遷移
    
    void showEffect(Layer* layer, string filename, float scale, Point pos); //エフェクトの表示
    
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

#endif /* defined(__war__ChallengeEventResultLayer__) */
