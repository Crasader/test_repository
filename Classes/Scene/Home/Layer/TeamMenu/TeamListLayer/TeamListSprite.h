#ifndef __war__TeamListSprite__
#define __war__TeamListSprite__

#include "cocos2d.h"
#include "json11.hpp"


USING_NS_CC;
using namespace std;
using namespace json11;


class TeamListSprite :public Sprite{
private:
    EventListenerTouchOneByOne *listener;
    
    Sprite* backgorund;
    Label* teamNameLabel;
    Label* teamLeaderNameLabel;
    string teamType;
    Label* teamTypeLabel;
    string playType;
    Label* playTypeLabel;
    Label* memberNumLabel;
    
    Json teamData;
    function<void(Json)> touchAction;
    
    bool movingFlg; //動かしたかフラグ
public:
    
    //タグ
    enum Tag{
        T_Team_Name= 1,
        T_Team_Leader_Name,
        T_Team_type_Label,
        T_Play_type_Label,
        T_Member_Num_label
    };
    
    //ZOrder
    enum ZOrder{
        Z_Team_Name= 1,
        Z_Team_Leader_Name,
        Z_Team_type_Label,
        Z_Play_type_Label,
        Z_Member_Num_label
    };

    
    static TeamListSprite* create(Json teamData, function<void(Json)> touchAction);
    virtual bool init(Json teamData, function<void(Json)> touchAction);
    
    float ratio;
    
    void createTeamNameLabel();
    void createTeamLeaderNameLabel();
    void createTeamTypeLabel();
    void createPlayTypeLabel();
    void createMemberNumLabel();
    
    void initEventListener(); //イベントリスナーの設定
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数

    
};
#endif /* defined(__war__TeamListSprite__) */
