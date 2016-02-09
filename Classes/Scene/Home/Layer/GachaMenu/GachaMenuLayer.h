#ifndef __War__GachaMenuLayer__
#define __War__GachaMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "XTLayer.h"
#include "cocos-ext.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "GachaMenuLayer.h"
#include "network/HttpClient.h"
#include "HeaderLayer.h"
#include "GachaAnimationLayer.h"
#include "GachaResultLayer.h"
#include "GachaConfirmationLayer.h"
#include "BannerContentsSlideSprite.h"

//URL
#define HTTP_GACHA_URL "chara/gacha.php"; //ガチャ内容取得処理参照先
#define IMG_CANCEL_BT "init/Home/Common/bt_cancel.png" //キャンセルボタン画像

#define GACHA_LAYER_Z 8       //ガチャレイヤー Z座標
#define GACHA_BANNER_Z 9      //ガチャバナー Z座標
#define BUTTON_GACHA_Z 10     //ガチャボタン Z座標
#define BUTTON_POPUP_Z 17     //確認ボタン Z座標

#define GACHA_BANNER_TAG 85               //ガチャバナー タグ
#define PRICE_LABEL 86                    //宝玉消費ラベル タグ
#define SINGLE_POPUP_TAG 102              //課金ガチャ最終確認 タグ
#define TENCONTINUOUS_POPUP_TAG 103       //課金10連ガチャ最終確認 タグ
#define CONTINUOUS30_POPUP_TAG 106       //課金30連ガチャ最終確認 タグ
#define FREE_POPUP_TAG 104                //無料ガチャ最終確認 タグ
#define FREE_TENCONTINUOUS_POPUP_TAG 105  //無料10連ガチャ最終確認 タグ
#define PLUS_1_POPUP_TAG 107
#define PLUS_30_POPUP_TAG 108

#define FREE_GACHA_TAB_TAG 111      //無料ガチャタブタグ
#define PREMIUM_GACHA_TAB_TAG 112   //課金ガチャタブタグ
#define PLUS_GACHA_TAB_TAG 113      //課金UR+ガチャタブタグ

#define FREE_GACHA_TAB_IMG_TAG 121
#define PREMIUM_GACHA_TAB_IMG_TAG 122
#define PLUS_GACHA_TAB_IMG_TAG 123   //課金UR+ガチャタブタグ

#define FREE_GACHA_LAYER_TAG 131
#define PREMIUM_GACHA_LAYER_TAG 132
#define PLUS_GACHA_LAYER_TAG 133

#define ROTATION_TIME 0.2f //半回転辺りの時間
#define DETAIL_ARRAY_SIZE 3 // detailの配列のサイズ

USING_NS_CC;
class GachaMenuLayer : public Layer
{
private:
    
    //バナーのパスを設定
    void setupBannerPath();
    
    //スライド式のバナーのパスを格納
    HmsVector<string> premium_slide_banner_file_paths ;
    HmsVector<string> free_slide_banner_file_paths ;
    HmsVector<string> plus_slide_banner_file_paths ;
    
    //固定式のバナーのパスを格納
    string premium_fix_banner_file_path ;
    string free_fix_banner_file_path ;
    string plus_fix_banner_file_path ;
    
    //課金のガチャ一覧のスライドするスプライト
    BannerContentsSlideSprite *contents_slide_sprite_premium;
    //無料のガチャ一覧のスライドするスプライト
    BannerContentsSlideSprite *contents_slide_sprite_free;
    //課金UR+のガチャ一覧のスライドするスプライト
    BannerContentsSlideSprite *contents_slide_sprite_plus;
    
    //現在のスライドするスプライトを取得する
    BannerContentsSlideSprite *getNowTabSlideBanner() ;
    
    EventListenerTouchOneByOne *listener;
    Size visibleSize;
    float header_height;   //ヘッダーの高さ
    float footer_height;   //フッターの高さ
    float main_screen_size;//ヘッダーフッター以外の高さ
    float main_screen_w;   //画面の横幅
    float main_screen_size_split_10;
    float main_screen_size_split_5;
    
    //ガチャスワイプ機能関係
    bool containDetail(Point point) ;//タップ位置がキャラ詳細に触れているか判定
    
    bool checkContainTouchPointByTag(Point touch_point, int tag);
    int _centerDetailIndex ; //今中心にあるデッキ
    void initBanner() ;
    void initArrowSprite();
    Sprite *detail_list[3];
    Point deck_center_point ;

    //左右にスワイプするときに詳細が見えないようにマスクを生成して詳細にかぶせる
    DrawNode* getMascShapeByTargetSprite(Node *masc_target_node) ;
    ClippingNode *createMascByTargetSprite(Node *masc_target_node) ;
    
    //タップ可能、不可能の切り替え
    bool getEnableTap() ;
    void setEnableTap(bool b) ;
    bool _enable_tap;
    
    int calcLeftDeckIndex(int center_index) ; //現在の中心のカードから見て左側のカードのインデックスを取得する
    int calcRightDeckIndex(int center_index) ; //現在の中心のカードから見て右側のカードのインデックスを取得する
    
    enum GachaMenuLayerTag{
        TAG_LEFT_ARROW,
        TAG_RIGHT_ARROW
    };
    
    //矢印のアニメーション
    void repeatArrowsAction() ;
    
    //矢印の表示、非表示の切り替え
    void showArrows(float dt) ;
    void hideArrows() ;
    
    //選択肢の自動移動
    void autoSwipeLeft();
    void autoSwipeRight();
    void autoSwipeCenter();
    
    //選択肢を数秒毎に自動移動する
    void autoSwipeUpdate(float dt) ;
    
    //ガチャタブ関連
    int gacha_tab;//現在の表示ガチャタブ 1=無料 2=課金 3=課金UR+
public:
    
    float ratio;
    int jewel_count;
    int gold_count;
    string user_master_id;
    Label* price_label;
    virtual bool init();
    CREATE_FUNC(GachaMenuLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void getSize();//サイズの取得
    void getData();//必要な情報を取得
    void setupImage();//画像、ラベルの設置
    void setupButton();//ボタンの設置
    void popupConfirmation(Ref* sender);//ガチャ確認ポップアップの生成処理
    void gachaRateBtn();//出現割合ボタン
    void changeGachaTab_sender(Ref* sender);//ガチャタブの切り替え処理へタグを送る
    void changeGachaTab(int tab_no);//ガチャタブの切り替え処理
    void showGachaTab(int tab_no);//ガチャタブの表示処理
	
	//コールバック処理
	void maintenanceRequest();//メンテ確認用
	void maintenanceCallback(HttpClient *sender, HttpResponse *response);
    void gachaRequest();//ガチャデータ取得
    void gachaCallback(HttpClient *sender, HttpResponse *response);
	
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter();//初期実行関数
};

#endif /* defined(__War__GachaMenuLayer__) */
