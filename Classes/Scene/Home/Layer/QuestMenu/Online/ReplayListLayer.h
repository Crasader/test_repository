#ifndef __War__ReplayListLayer__
#define __War__ReplayListLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class ReplayListLayer : public Layer, public ScrollViewDelegate
{
private:
    float ratio;
    EventListenerTouchOneByOne *listener;
    bool touchButtonFlg;
    string _page_name;

    Json json_master;
    int json_count;
    int cell_size; //スクロール時のセルの大きさ
    
    HmsVector<string> battle_list_id_list;
    
    string type = "all";//all,myself,favorite
public:
    virtual bool init();
    CREATE_FUNC(ReplayListLayer);
    
    //イベントリスナーの作成
    void initEventListener();
    
    //HTTP処理
    void getReplayList();
    void getReplayListCallback(HttpClient *sender, HttpResponse *response);
    
    //スクロールビューの作成
    void createScrollView();
    Layer* createList();
    
    //リプレイへ
    void replayStart();
    
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

#endif /* defined(__War__ReplayListLayer__) */
