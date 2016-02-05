#ifndef __War__TeamManagementBaseLayer__
#define __War__TeamManagementBaseLayer__

#include "cocos2d.h"
#include "common.h"
#include "HttpConnect.h"

USING_NS_CC;
using namespace std;

//管理ベース
class TeamManagementBaseLayer : public Layer
{
private:
    float ratio;
    EventListenerTouchOneByOne *listener;

    string _tab_name;
public:
    virtual bool init();
    CREATE_FUNC(TeamManagementBaseLayer);
    
    void onEnter();
    void setup();
    
    MenuItemSprite *createMenuItemSprite(string button_image, const ccMenuCallback& callback) ;
    
    //ページ遷移
    void changePage(Ref* sender) ;
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
};

#endif