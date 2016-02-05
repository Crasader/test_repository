#ifndef __War__HeaderLayer__
#define __War__HeaderLayer__

#include "cocos2d.h"
#include "common.h"
#include "NormalScene.h"
#include <time.h>
#include "HmsVector.h"
#include "TitleBarLayer.h"

#define JEWEL_LABEL_TAG 200
#define GOLD_LABEL_TAG 250
#define NAME_LABEL_TAG 300

#define MAX_GOLD_COUNT 999999
#define MAX_JEWEL_COUNT 999

#define FRAME_SIZE 324

USING_NS_CC;
using namespace std;

class HeaderLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    
    enum Tag{
        T_Point_Bg = 1,
        T_Point_Bar,
        T_Header_Bg,
        T_Label,
        T_Power_Bar,
        T_Power_Label,
        T_Stone_Shop_Button,
        T_Recovery_Label,
        T_Frame,
        T_ChatLayerTag,
    };
    
    enum ZOrder{
        Z_Point_Bg = 1,
        Z_Point_Bar,
        Z_Header_Bg,
        Z_Label,
        Z_Power_Bar,
        Z_Power_Label,
        Z_Stone_Shop_Button,
        Z_Recovery_Label,
        Z_Frame,
    };
    
    enum LabelTag{
        T_Jewel_Label = 20,
        T_Gold_Label,
        T_Name_Label,
    };

    CC_SYNTHESIZE(string, _rank_name, RankName); //称号の名前
    CC_SYNTHESIZE(string, _user_name, UserName); //ユーザー名
    CC_SYNTHESIZE(string, _gold, Gold); //資金
    CC_SYNTHESIZE(string, _jewel, Jewel); //宝玉
    CC_SYNTHESIZE(int, _level, Level); //現在のレベル
    CC_SYNTHESIZE(int, _now_battle_point, NowBattlePoint); //現在のバトルpt
    CC_SYNTHESIZE(int, _max_battle_point, MaxBattlePoint); //最大バトルpt
    CC_SYNTHESIZE(int, _now_rank_point, NowRankPoint); //現在の称号pt
    CC_SYNTHESIZE(int, _need_rank_point, NeedRankPoint); //ランクアップ必要称号pt(※減少あり)
    CC_SYNTHESIZE(int, _now_reward_point, NowRewardPoint); //現在の報酬pt
    CC_SYNTHESIZE(int, _need_reward_point, NeedRewardPoint); //必要報酬pt(※減少しない)
    
    float ratio;
    
    Label* _battle_point_label; //バトルポイントのラベル
    Sprite * _battle_point_gauge; //バトルポイントのゲージ
    
    time_t _check_time; //時間の経過をチェックする変数
    time_t _power_use_time; //最後に体力を使った時間(更新あり)
    time_t _server_diff_time; //サーバータイムとの差
    
    void createTimeStamp(); //タイムスタンプの作成
    void setRecoveryTime(float dt); //回復時間の設定
    void initEventListener(); //イベントリスナーの作成
    
    void recoveryAction(); //回復アクション
    void updateNowBattlePoint(); //現在のバトルポイントの更新
    
    Node *createLevelNumber(int num); //レベルの画像を依存させたNodeを作成(Nodeの中心がレベルの画像の中心)
    int calcDigit(int num) ; //桁数を調べる
    
    HeaderLayer(); //コンストラクタ
    virtual bool init();
    CREATE_FUNC(HeaderLayer);
    
    void loadData(); //UserDefaultのデータを読み込む
    void showBg(); //ヘッダー背景関連の表示
    void showData(); //データの表示
    void createBattlePoint(); //バトルptの作成
    void createBattlePointRecoveryTime(); //バトルpt回復時間の作成
    void showBattlePoint(); //バトルptの表示
    void setMaxBattlePointAction(); //バトルポイントを最大にする
    
    enum TitleBarType{
        WIDTH_466, //横466 通常時
        WIDTH_390, //見える部分の横幅390(実際の画像サイズは横466で右側が透過になっている)
        WIDTH_324, //見える部分の横幅324(実際の画像サイズは横466で右側が透過になっている)
        WIDTH_215, //見える部分の横幅215(実際の画像サイズは横466で右側が透過になっている)
        WIDTH_348, //見える部分の横幅348(実際の画像サイズは横466で右側が透過になっている)
    };
    
    enum TitleBarPosition{
        BELOW_HEADER, //ヘッダーの下端にタイトルバーの上端をそろえる
        ALIGN_LAYER_TOP, //レイヤーの上端とタイトルバーの上端をそろえる
    };
    
    static void setupTitleBar(string titlename, string return_page_name, Layer* page, TitleBarType titleBarType, TitleBarPosition titleBarPosition); //タイトルバーの作成
    static void setupTitleBar(string titlename, string return_page_name, Layer* page, TitleBarType titleBarType); //タイトルバーの作成
    static void setupTitleBar(string titlename, string return_page_name, Layer* page); //タイトルバーの作成
//    static void setTitleBarAddSprite(Layer* layer, Node* node, bool menu_flg = false); //タイルバーの上にnodeを置くメソッド
    
    void  setFrameData(); //フレームデータの設定
    float calcFrameX(cocos2d::Touch *touch); //タッチ位置からフレームの位置を計算
    
    //タッチイベントON OFF切り替え
    void touchON();
    void touchOFF();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

    void update(float dt); //アップデート処理
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__HeaderLayer__) */
