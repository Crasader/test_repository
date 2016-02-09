#ifndef __war__MapNaviLayer__
#define __war__MapNaviLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "MapCharaLayer.h"
#include "HmsVector.h"
#include "AnzSound.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;
using namespace CocosDenshion;

class MapNaviLayer : public Layer
{
private:
    float ratio;
    Layer *_footer_layer = nullptr;
    
    HmsVector<string> _message_list;
    bool _display_message_flg = false;
    
    int _personal_point;
    int _team_point;
    int _now_point[2];
    bool _point_set_flg = false;
public:
    virtual bool init();
    CREATE_FUNC(MapNaviLayer);
    
    void initialize();
    void displayBattleMessage(float dt);
    void addBattleMessage(string message);
    void displayCharaInfo(string message, MapCharaLayer *chara);
    void displayTurnChangeMessage(string message, bool is_player_turn);
    void displaySituationInfo(string player_team_name, string player_count, string enemy_team_name, string enemy_count, int turn_no, int turn_time, int  turn_time_max);
    void displayRestTime(int rest_time);
    void setEventPoint(int personal_point, int team_point);
    void updateEventPoint(float dt);
};

#endif /* defined(__war__MapNaviLayer__) */
