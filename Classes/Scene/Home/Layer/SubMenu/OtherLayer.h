#ifndef __War__OtherLayer__
#define __War__OtherLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "OptionsLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "NativeBridge.h"
#include "ProfileLayer.h"
#include "SubMenuLayer.h"
#include "WebViewLoadLayer.h"

#define MESSEGE_BAR_TAG 1

#define ZORDER_BAR 1

#define TOUCH_LAYER_TAG 9 //スクロールするレイヤーのタグ
//ボタンタグ
#define OPTIONS_CHANGE_TAG 10 //オプションタグ
#define MODELCHANGE_CHANGE_TAG 12 //機種変更コード入力タグ
#define HELP_CHANGE_TAG 14 //ヘルプタグ
#define CREDIT_TAG 16 //クレジットタグ

#define SUBMENU_BUTTON_COUNT 7//ページ遷移するボタンの数

#define CREDIT_URL HOST"user/?&p=page__credit"  //クレジットを開く用URL
#define HELP_URL HOST"user/?&p=page__help"     //ヘルプを開く用URL

USING_NS_CC;

class OtherLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    bool move_flg = false;
    Vec2 first_touch;//移動前座標
    float move_f;
public:
    float ratio;
    
    virtual bool init();
    Size visibleSize;
    float header_height;
    float footer_height;
    float main_screen_size;
    float main_screen_size_w;
    string user_master_id = SaveData::loadString("user_master_id");
    CREATE_FUNC(OtherLayer);
    void initEventListener(); //イベントリスナーの設定
    //画像の設定
    void setupImage();
    
    void changePage(int tag_no);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter();
};
#endif /* defined(__War__OtherLayer__) */
