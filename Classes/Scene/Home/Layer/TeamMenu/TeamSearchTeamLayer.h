#ifndef __War__TeamSearchTeamLayer__
#define __War__TeamSearchTeamLayer__

#include "cocos2d.h"
#include "common.h"
#include "cocos-ext.h"
#include "HttpConnect.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

//チーム検索
class TeamSearchTeamLayer : public Layer
{
private:
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    enum Tag{
       T_BACK_POPUP=300,
       T_SEARCH,
       T_EDITBOX_NAME,
    };
    
    //入力チーム名
    string _input_team_name;
    
    //チームタイプ
    string _team_type;
    //プレイタイプ
    string _play_type;
public:
    virtual bool init();
    CREATE_FUNC(TeamSearchTeamLayer);
    
    void onEnter(); //初期実行関数
    void setup();
    
    //選択項目変更時処理
    void optionChange(int tap_tag);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
};

#endif