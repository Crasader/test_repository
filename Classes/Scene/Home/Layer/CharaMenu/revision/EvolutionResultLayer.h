#ifndef __war__EvolutionResultLayer__
#define __war__EvolutionResultLayer__

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
#include "Effect.h"
#include "SuperAnimNodeV2.h"
#include "EvolutionResultAnimationLayer.h"
#include "CharaDetailSprite.h"
#include "CharaDetailCellSprite.h"
#include "TimeMeasure.h"

#define SHADOW_BACK_LAYER_TAG 499
#define IMG_TEN_BTN "init/Home/Common/bt1.png"       //10連続でガチャを回すボタン画像

using namespace extension;

class EvolutionResultLayer : public cocos2d::Layer, public SuperAnim::SuperAnimNodeListener, public ScrollViewDelegate
{
private:
    SuperAnim::SuperAnimNode* mAnimNode;
    EventListenerTouchOneByOne *listener;
    CC_SYNTHESIZE(int,_gachatype, GachaType);
    CC_SYNTHESIZE(int,_card_count, CardCount);
    
    bool touchButtonFlg;
    
public:
    
    float ratio;
    Size visibleSize;
    float header_height;   //ヘッダーの高さ
    float footer_height;   //フッターの高さ
    float main_screen_size;//ヘッダーフッター以外の高さ
    float main_screen_w;   //画面の横幅
    float main_screen_size_split_10;
    float main_screen_size_split_5;
    
    int _rare_card[30];    //カードのレア度
    int _chara_list_id[30];//ユーザーでフォルトでのID
    int _hit_card[30];     //当たったカードのID
    int _chara_type[30];   //当たったカードのタイプ
    string _card_name[30]; //カード名
    Sprite *icon[30];

    virtual bool init();
    CREATE_FUNC(EvolutionResultLayer);
    void initEventListener(); //イベントリスナーの設定
    
    void setData(int gacha_type,int card_count,int hit_card[],int chara_list_id[], int rare_card[],string card_name_list[],int chara_type[]);//データの受け取りとセット
    
    void startAnimation();//リザルトアニメーションの開始
    
    void getSize();//サイズの取得
    
    void createBg();//背景画像の作成
    
    //初期実行関数
    void onEnter();
    
    //詳細を表示
    void showDetail(int tag)  ;
    
    void initCardDetailCell();
    void afterAnimation(float dt) ;
    vector<Sprite *> _chara_detail_cell; //ガチャで引いたキャラの情報を表示するスプライト
    Layer* createList(); //リストの作成
    void createScrollView(); //スクロールビューの作成
    int _cell_size; //スクロール時のセルの大きさ
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};
#endif /* defined(__war__EvolutionResultLayer__) */
