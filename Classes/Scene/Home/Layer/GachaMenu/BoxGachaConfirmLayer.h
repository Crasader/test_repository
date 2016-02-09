#ifndef __war__BoxGachaConfirmLayer__
#define __war__BoxGachaConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "XTLayer.h"
#include "cocos-ext.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "BoxGachaLayer.h"
#include "network/HttpClient.h"
#include "HeaderLayer.h"
#include "GachaAnimationLayer.h"
#include "PlusGachaAnimationLayer.h"
#include "GachaResultLayer.h"
#include "TimeMeasure.h"

#define BUTTON_LABEL_Z 11     //ボタンラベル Z座標
#define ZORDER_SHADOW 4 //shadowのZ座標(メインレイヤーの上)
#define SHADOW_TAG 11   //シャドウレイヤータグ
#define CANCEL_TAG 99   //キャンセルボタン タグ

class BoxGachaConfirmLayer : public Layer
{
private:
    Size visibleSize;
    float header_height;   //ヘッダーの高さ
    float footer_height;   //フッターの高さ
    float main_screen_size;//ヘッダーフッター以外の高さ
    float main_screen_w;   //画面の横幅
    float main_screen_y;   //画面の縦幅
    float main_screen_size_split_10;
    float main_screen_size_split_5;
public:
    float ratio;
    
    string _user_master_id;   //ユーザーID
    string jewel;
    string gold;
    
    string _box_gacha_id;
    int _gacha_type;           //ガチャの種類1
    string _gacha_type_label;  //ガチャの種類ラベル
    int _gacha_for_count;      //ガチャを回す数
    int _max_chara_count;      //BOX最大値
    int err_check;             //エラー判定
    int hit_card[30];          //引いたカードのID
    int rare_card[30];         //引いたカードのレアリティ
    int chara_list_id[30];     //引いたカードのcard_list_id
    int chara_type[30];        //引いたカードのchara_type
    string card_name_list[30]; //引いたカードの名前
    bool _gacha_flg=false;     //ガチャ判定
    
    Label *title_frame_label; //タイトル枠画像
    Label *use_items;         //使用宝玉or使用資金ラベル
    Label *have_items;        //所持宝玉or資金
    Sprite *shadow;           //影スプライト

    virtual bool init(int gacha_type, string box_gacha_id);
    static BoxGachaConfirmLayer* create(int gacha_type, string box_gacha_id);
    
    void setData();//必要な情報を取得
    void getSize();//サイズの取得
    void setImg();//背景を生成
    void setLabel();//ラベルの設置
    void setBtn();//ボタンの設置
    void setCompletion();//準備完了後処理
    void gachaCancel(Ref* sender);//ガチャキャンセル処理
    void gachaStart(Ref* sender);//ガチャ通信処理
    void gachaCallback(HttpClient *sender, HttpResponse *response);//ガチャ通信レスポンス
    void screenTransition();//画面遷移処理
    //初期実行関数
    void onEnter();
};
#endif /* defined(__war__BoxGachaConfirmLayer__) */
