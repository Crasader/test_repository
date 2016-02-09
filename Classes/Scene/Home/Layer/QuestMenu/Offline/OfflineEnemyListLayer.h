#ifndef __war__OfflineEnemyListLayer__
#define __war__OfflineEnemyListLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "UserInfoCellSprite.h"
#include "HomeCommon.h"
#include "EnemySprite.h"

USING_NS_CC;

class OfflineEnemyListLayer : public Layer, public ScrollViewDelegate
{
private:
    enum Order{
        ORDER_SCROLLVIEW=10,
    };
    
    EventListenerTouchOneByOne *listener;
    void saveUseDungeonData(string enemy_user_master_id) ; //ダンジョンシーンで使用するデータを保存
    HmsVector<EnemySprite *> _enemy_cells; //ユーザー情報のセルを格納
    
    int _cell_size; //セルの大きさ
    Json _battle_user_list; //対戦相手情報のリスト
    int _data_count; //対戦相手の数
    
    void initBattleUserSprite(); //対戦相手スプライトの作成
    bool touchButtonFlg;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(OfflineEnemyListLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void updateImage(); //画像の設定
    void changePage(int tag); //ページの遷移
    
    void createScrollView(); //スクロールビューの作成
    Layer* createList(); //リストの作成
    
    //通信処理
    void battleUserRequest();//対戦相手取得リクエスト
    void battleUserRequestCallback(HttpClient *sender, HttpResponse *response);//対戦相手選出レスポンス
    
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

#endif /* defined(__war__OfflineEnemyListLayer__) */
