#ifndef __war__EventMenuLayer__
#define __war__EventMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "EventExplainSprite.h"
#include "EventListSprite.h"


USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class EventMenuLayer : public Layer, public ScrollViewDelegate
{
protected:
    EventListenerTouchOneByOne *listener;
    Json _event_data; //イベントデータ
    int _event_count; //イベントの数

    HmsVector<Sprite *> _event_sp; //イベント画像
    
public:
    int cell_size;
    float ratio;
    bool touchButtonFlg;
    
    virtual bool init();
    CREATE_FUNC(EventMenuLayer);
    
    void initEventData(); //イベントデータの初期化
    
    void initEventLitsener(); //イベントリスナーの初期化
    void createScrollView(); //スクロールビューの作成
    Layer* createList(); //リストの作成
    
    void createDetailButton(); //イベント詳細ボタンの作成
    void changePage(int tag); //ページの切り替え
    
    void createEventExplainData(int tag); //イベント説明画像のデータの作成
    Sprite* createEventEndDate(string end_date); //イベント終了時刻の作成
    
    void changeExplainSprite(); //イベント説明画像
    
    //通信処理
    void httpRequestForEventInfo() ;
    void onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) ;
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__EventMenuLayer__) */
