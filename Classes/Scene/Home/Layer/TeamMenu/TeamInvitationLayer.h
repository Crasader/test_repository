#ifndef __War__TeamInvitationLayer__
#define __War__TeamInvitationLayer__

#include "cocos2d.h"
#include "common.h"
#include "cocos-ext.h"
#include "HttpConnect.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

//勧誘
class TeamInvitationLayer : public Layer
{
private:
    float ratio;
    EventListenerTouchOneByOne *listener;

    //ユーザID
    string _input_friend_id;
    //ユーザ名
    string _input_user_name;
    //ユーザレベル//
    string _input_user_level_min;
    string _input_user_level_max;
public:
    virtual bool init();
    CREATE_FUNC(TeamInvitationLayer);
    
    void onEnter(); //初期実行関数
    void setup();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
};

#endif