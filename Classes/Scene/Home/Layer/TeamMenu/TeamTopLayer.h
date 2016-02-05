#ifndef __War__TeamTopLayer__
#define __War__TeamTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;
using namespace std;

//チームトップ
class TeamTopLayer : public Layer
{
private:
    
    enum Tag{
        T_HELP_BUTTON,
    };
    
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    bool _is_boss_event_open = false;
    Json _boss_event_data ;
    int _boss_count = 0;
public:
    virtual bool init();
    CREATE_FUNC(TeamTopLayer);
    
    void onEnter(); //初期実行関数
    
    void getTeamData();
    void getTeamDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void setup();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

    //ボタンタップ時
    void onTouchButton(Ref *sender) ;
};

#endif