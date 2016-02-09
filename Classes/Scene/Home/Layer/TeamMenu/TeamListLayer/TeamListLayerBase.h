#ifndef __war__TeamListLayerBase__
#define __war__TeamListLayerBase__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "HmsList.h"
#include "json11.hpp"
#include "HttpConnect.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
using namespace json11;

class TeamListSprite;

//チーム一覧
class TeamListLayerBase : public Layer, ScrollViewDelegate{
protected:
    float ratio;
    EventListenerTouchOneByOne *listener;
    //チームリストデータ
    Json teamListData;
    
    //
    CC_SYNTHESIZE(function<void(Json)>, listTouchAction, ListTouchAction);
    
    HmsVector<TeamListSprite*> teamListSprites;
    HmsVector<Point > teamListSpritePositionList; //ポジションインデックスリスト
    //スクロール用
    int cellSize;
    
public:
    
    //タグ
    enum Tag{
        T_Popup = 1,
        T_ScrollView,
        T_ScrollLayer,
        T_Button,
        T_Button_Text,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Popup = 1,
        Z_ScrollView,
        Z_ScrollLayer,
        Z_Button,
        Z_Button_Text,
    };

    virtual bool init();
    
    void onEnter();
    
    virtual void setHeaderSetting() = 0;
    virtual void setSpriteTouchAction() = 0;
    
    virtual void GetTeamListData(); //バザーの情報を取得
    void httpCallBack(HttpClient* sender, HttpResponse* response); //バザーの情報取得コールバック
    
    void createListLayer();
    void createPositionIndex(); //ポジションインデックスの作成
    void createScrollView(); //スクロールビューの作成
    Layer* createList();
    void setScrollViewData();
    //リストのリフレッシュ
    virtual void refreshList();
    
    //タッチイベント処理
    void InitEventListener();
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
};



#endif /* defined(__war__TeamListLayerBase__) */
