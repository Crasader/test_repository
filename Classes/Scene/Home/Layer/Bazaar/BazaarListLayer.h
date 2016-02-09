#ifndef __war__BazaarListLayer__
#define __war__BazaarListLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "BazaarCharaSprite.h"

class BazaarListLayer : public Layer, public ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne* listener;
    json11::Json bazaarSeachList;
    HmsVector<BazaarCharaSprite *> bazaarCharaSpriteList; //交換キャラの画像
    HmsVector<Point > charaSpritePositionList; //ポジションインデックスリスト
    int cellSize; //セルの大きさ
public:
    float ratio;
    
    //タグ
    enum Tag{
        T_ScrollView = 1,
        T_ScrollLayer
    };
    
    //ZOrder
    enum ZOrder{
        Z_ScrollView = 1,
        Z_ScrollLayer
    };
    
    BazaarListLayer();
    
    static BazaarListLayer* create(Json bazaarSeachList);
    virtual bool init(Json bazaarSeachList);
    
    void InitEventListener(); //イベントリスナーの設定
    void GetBazaarData(); //バザーの情報を取得
    void GetBazaarData(string name, int rare, int type, int cost); //バザーの情報を取得
    void createPositionIndex(); //ポジションインデックスの作成
    void GetBazaarDataCallBack(HttpClient *sender, HttpResponse *response); //バザーの情報取得コールバック
    void CreateScrollView();
    Layer* CreateList();
    void setScrollViewData();
    void checkPurcaseShortage();
    void createListLayer();
    
    //更新、検索時の再生成
    void RefreshList(string name, int rare, int type, int cost);

    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
    
    
    
};

#endif /* defined(__war__BazaarListLayer__) */
