#ifndef __war__TournamentEventResultLayer__
#define __war__TournamentEventResultLayer__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "PointPopupSprite.h"
#include "BootyPopupSprite.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "TouchScreenSprite.h"
//#include "FriendUserDetailSprite.h"
//#include "ResultAchievementLayer.h"
#include "HmsCrypto.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class TournamentEventResultLayer : public Layer
{
public:
    TournamentEventResultLayer(); //コンストラクタ
    virtual bool init(bool winner_flg, string from_page_name);
    void onEnter(); //初期実行関数
    static TournamentEventResultLayer* create(bool winner_flg, string from_page_name = "DungeonScene");
    
private:
    
    enum TournamentResultType{
        LOSE=1, //負け
        EXIST_NEXT_BATTLE, //次のバトルがある
        VICTORY, //優勝
    };
    
    enum Order{
        O_shadow=1,
        O_confetti,
        O_navi_chara,
        O_navi_balloon,
        O_touch_screen,
        O_battle_result_circle,
        O_result_sprite,
    };
    
    TournamentResultType _tournament_result_type ;//表示内容のタイプ
    
    void createSprites(Json json); //ポップアップなどを全て作成
    
    Sprite* _battle_result_circle; ////「対戦結果」と書かれた画像の表示
    Sprite* _result_sprite; //結果表示の枠
    Sprite* _navi_sprite; //ナビキャラのスプライト
    Sprite* _navi_balloon; //ナビキャラの吹き出し
    
    //紙吹雪の作成関係
    void createConfetti() ;
    void moveConfetti(float dt) ;
    Sprite *_confetti_1;
    Sprite *_confetti_2;
    
    float ratio;
    
    enum Tag{
    };
    
    EventListenerTouchOneByOne *listener;
    Sprite* _resultLabelSprite; // 辛勝 完全勝利 勝利の表示を行うスプライト
    
    
    //シーンきりかえ済み
    bool _change_scened_flg ;
    string _from_page_name; //どこから遷移してきたかのページ名

    bool _winner_flg; //勝敗フラグ
    
    //通信処理
    void getPointDataRequest();
    void getPointDataCallback(HttpClient *sender, HttpResponse *response);
    
    //結果を表示する
    void showResult();
    
    void updateUserData(); //ユーザーデータの更新
    
    void createEventListener(); //イベントリスナーの作成
    void removeEventListener(); //イベントリスナーの削除
    

    void touchStartAction(); //タッチスタートの作成
    void touchStartRemove(); //タッチスタート画像の削除
    
    void createShadow(); //背景のシャドウの作成
    void showResultLabel(float dt); //バトル結果のラベル画像の表示
    void showBattleResult(); //対戦結果の表示
    
    void changeScene(); //ノーマルシーンへの遷移
    
    void showEffect(Layer* layer, string filename, float scale, Point pos); //エフェクトの表示
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
};

#endif /* defined(__war__TournamentEventResultLayer__) */
