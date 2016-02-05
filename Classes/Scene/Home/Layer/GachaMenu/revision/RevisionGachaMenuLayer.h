#ifndef __war__RevisionGachaMenuLayer__
#define __war__RevisionGachaMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "TimeCounter.h"

USING_NS_CC;

class RevisionGachaMenuLayer : public Layer
{
private:
    enum Tag {
        T_GACHA_FREE = 1,
        T_GACHA_PREMIUM,
        T_GACHA_PREMIUM_PLUS,
        T_GACHA_UNKNOWN1,//現状不明
        T_END_DATE_BACK,
    };
    
    int T_BOX_GACHA = 100;
    
    enum EndDateBackChildTag {
        T_END_DATE_LABEL,
        T_SUFFIX_TIME_TEXT,
        T_PREFIX_TIME_TEXT,
    } ;
    
    EventListenerTouchOneByOne *listener;
//    HmsVector<BannerSprite *>_button_list;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(RevisionGachaMenuLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createList(Json json_gacha_menu_data_list); //リストの作成
    Sprite* createButton(Json json_gacha_menu_data); //ボタンの作成
    void changePage(int tag_no); //ページ遷移
    
    //キャラデータ通信処理
    void gachaMenuInfoRequest();
    void gachaMenuInfoCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
    //終了時刻表示スプライト作成
    Sprite* createEndDateBack(Sprite *button) ;
    //終了時刻表示スプライト更新
    void updateEndDateBack(Sprite *end_date_back, time_t remain_time_second) ;
};

#endif /* defined(__war__RevisionGachaMenuLayer__) */
