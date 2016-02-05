#ifndef __War__TeamListLayer__
#define __War__TeamListLayer__

#include "cocos2d.h"
#include "HmsScrollLayer.h"
#include "HmsList.h"

USING_NS_CC;
using namespace std;

//チーム一覧
class TeamListLayer : public Layer, public ScrollViewDelegate
{
private:
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    //用途
    string _use_name;

    //チームリストデータ
    Json _json_team_list;
    
    //フラグ
    bool _tap_flg = false;
    
    //スクロール用
    int _cell_count;
    int _cell_size;
public:
    virtual bool init();
    CREATE_FUNC(TeamListLayer);
    
    void onEnter();
    void createScrollView(); //スクロールビューの作成
    Layer *createList();

    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void applyCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void cancelCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void soliYesCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void soliNoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
};

#endif