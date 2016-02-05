#ifndef __War__BoxGachaLayer__
#define __War__BoxGachaLayer__

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
#include "GachaResultLayer.h"
#include "GachaConfirmationLayer.h"
#include "BannerContentsSlideSprite.h"

#define TAG_BOX_GACHA_BTN_TICKET1 101
#define TAG_BOX_GACHA_BTN_JEWEL1 102
#define TAG_BOX_GACHA_BTN_JEWEL10 103
#define TAG_BOX_GACHA_BTN_JEWEL30 104
#define TAG_BOX_GACHA_RATE 105

USING_NS_CC;
class BoxGachaLayer : public Layer, public ScrollViewDelegate
{
private:
    float ratio;
    EventListenerTouchOneByOne *listener;
    string _box_gacha_id;
    int _box_height;
    Json _json;
public:
    virtual bool init();
    CREATE_FUNC(BoxGachaLayer);

    void onEnter();//初期実行関数
	
	//HTTP通信
	void maintenanceRequest();//メンテ確認用
	void maintenanceCallback(HttpClient *sender, HttpResponse *response);
    void gachaRequest();//ガチャデータ取得
    void gachaCallback(HttpClient *sender, HttpResponse *response);

    //セットアップ
    void initEventListener(); //イベントリスナーの設定
    void setupButton();
    void createScrollView(); //スクロールビューの作成
    Layer *createLayer();
	
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif /* defined(__War__BoxGachaLayer__) */
