#ifndef __War__TeamCreateTeamLayer__
#define __War__TeamCreateTeamLayer__

#include "cocos2d.h"
#include "common.h"
#include "cocos-ext.h"
#include "EditBoxUtil.h"
#include "HttpConnect.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

//チーム作成
class TeamCreateTeamLayer : public Layer
{
private:
    enum Tag{
        FRAME,
        EDIT_BOX,
        CREATE_BUTTON,
    };
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    //入力チーム名
    string _input_team_name;
public:
    virtual bool init();
    CREATE_FUNC(TeamCreateTeamLayer);
    
    void onEnter();
    void setup();
    void createLabels(Node *parent_sprite) ;//文字列の生成とaddChild
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
};

#endif