#ifndef __War__TeamNoBelongLayer__
#define __War__TeamNoBelongLayer__

#include "cocos2d.h"
#include "common.h"
#include "HttpConnect.h"

USING_NS_CC;
using namespace std;

//チーム未所属トップ
class TeamNoBelongLayer : public Layer
{
private:
    enum Tag{
        T_HELP_BUTTON,
    };
    
    float ratio;
    EventListenerTouchOneByOne *listener;
public:
    virtual bool init();
    CREATE_FUNC(TeamNoBelongLayer);
    
    void onEnter();
    
    void getTeamData();
    void getTeamDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void setup();
    void createLabels(Node *parent_sprite) ; //文字列の生成とaddChild

    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void petitionCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void invitationCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    //ボタンタップ時
    void onTouchButton(Ref *sender) ;
};

#endif