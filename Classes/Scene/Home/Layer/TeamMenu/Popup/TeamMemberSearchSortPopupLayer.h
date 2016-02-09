#ifndef __war__TeamMemberSearchSortPopupLayer__
#define __war__TeamMemberSearchSortPopupLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"

USING_NS_CC;
using namespace std;

class TeamMemberListLayer;

class TeamMemberSearchSortPopupLayer: public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    TeamMemberListLayer* MemberListLayer;
    Sprite* background; //背景

    CC_SYNTHESIZE(function<void()>, _close_func, CloseFunc); //閉じるボタンを押したときに呼ばれるfunction
    
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Title_Text,
        T_Text_box,
        T_Reality_button,
        T_Type_button,
        T_Type_button2,
        T_Cost_button,
        T_Search_id_box,
        T_Search_name_box,
        T_Refresh_Button,
        T_Close_Button,
        T_Popup_Layer
    };
    
    //ZOrder
    enum ZOrder{
        Z_Shadow = 1,
        Z_Title_Text,
        Z_Text_box,
        Z_Reality_button,
        Z_Type_button,
        Z_Type_button2,
        Z_Cost_button,
        Z_Search_id_box,
        Z_Search_name_box,
        Z_Refresh_Button,
        Z_Close_Button,
        Z_Popup_Layer
    };
    
    bool charaType[4] = {false};
    float ratio;
    
    TeamMemberSearchSortPopupLayer(); //コンストラクタ
    
    static TeamMemberSearchSortPopupLayer* create(TeamMemberListLayer* layer);
    virtual bool init(TeamMemberListLayer* layer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createSearchTypeButton(); //検索条件内容ボタン
    void createSearchButton(); //検索ボタン
    void setToggleCallBack(Ref* sender);
    void setSelectButtonData();
    
    void onEnter() ; //初期実行関数
};

#endif