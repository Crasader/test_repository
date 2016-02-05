#ifndef __War__BattleFriendChoiceLayer__
#define __War__BattleFriendChoiceLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "UserInfoCellSprite.h"
#include "HelperSprite.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace extension;
using namespace json11;;

class BattleFriendChoiceLayer : public Layer, public ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    Json _helper_user_list; //助っ人ユーザー情報
    vector<HelperSprite *> _helper_cells; //助っ人ユーザー情報を格納
    
    string _page_name; //遷移元のページ名を取得
    int _cell_size; //スクロール時のセルの大きさ
    bool touchButtonFlg;

    //フレンドカードの詳細を表示
    void showDetail(int tag) ;
    
    
    //フレンドタップ時の詳細情報取得処理
    void getCharaDatahttpRequest(string chara_id) ;
    void getCharaDatahttpCallback(HttpClient *sender, HttpResponse *response);
    
public:
    float ratio;
    
    static BattleFriendChoiceLayer* create(string page_name);
    virtual bool init(string page_name);
    
    void initEventListener(); //イベントリスナーの作成
    void showTitleBar(); //ページのタイトルを表示
    void initFollowSprite(); //助っ人画像の作成
    
    Layer* createList(); //リストの作成
    void createScrollView(); //スクロールビューの作成
    void changePage(int tag); //ページ遷移
    
    void saveUseDungeonData(Json json) ; //ダンジョンシーンで使用するデータを保存
    Json jsonParseForUserInfoCellSprite(int count) ;
    
    //通信処理
    void getFollowListRequest();
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

#endif /* defined(__War__BattleFriendChoiceLayer__) */
