#ifndef __War__FollowListLayer__
#define __War__FollowListLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "FollowSprite.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace extension;
using namespace json11;;

class FollowListLayer : public Layer, public ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    bool touchButtonFlg;
    string _page_name;
    
public:
    float ratio;
    
    static FollowListLayer* create(string page_name);
    virtual bool init(string page_name);
//    CREATE_FUNC(FollowListLayer);
    
    void showTitleBar(); //ページのタイトルを表示
    void initFollowSprite(Json json, int count); //フォロー画像の作成
    HmsVector<FollowSprite *> follows; //フォロー情報を格納
    int cell_size; //スクロール時のセルの大きさ
    
    Layer* createList(); //リストの作成
    void createScrollView(); //スクロールビューの作成
    void initEventListener(); //イベントリスナーの作成
    void changeUserDetail(); //ユーザー情報ページに遷移
    
    //通信処理
    void getFollowListRequest(string status);
    void FollowListCallback(HttpClient *sender, HttpResponse *response);
    
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

#endif /* defined(__War__FollowListLayer__) */
