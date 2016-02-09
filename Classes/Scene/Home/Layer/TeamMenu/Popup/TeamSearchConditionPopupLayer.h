#ifndef __war__TeamSearchConditionPopupLayer__
#define __war__TeamSearchConditionPopupLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"

USING_NS_CC;
using namespace std;

class TeamSearchListLayer;

class TeamSearchConditionPopupLayer: public Layer, public ui::EditBoxDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    TeamSearchListLayer* teamSearchListLayer;
    Sprite* background; //背景
    
    CC_SYNTHESIZE(function<void()>, _close_func, CloseFunc); //閉じるボタンを押したときに呼ばれるfunction
    
    bool playType[4] = {false};
    bool teamType[4] = {false};
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Title_Text,
        T_Text_box,
        T_Reality_button,
        T_TEAM_TYPE,
        T_PLAY_TYPE,
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
        Z_TEAM_TYPE,
        Z_PLAY_TYPE,
        Z_Search_id_box,
        Z_Search_name_box,
        Z_Refresh_Button,
        Z_Close_Button,
        Z_Popup_Layer
    };
    
    float ratio;
    
    TeamSearchConditionPopupLayer(); //コンストラクタ
    
    static TeamSearchConditionPopupLayer* create(TeamSearchListLayer* layer);
    virtual bool init(TeamSearchListLayer* layer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createInputName(); //キャラ名
    void createPlayTypeButton(); //プレイタイプ
    void createTeamTypeButton(); //チームタイプ
    void createSearchButton(); //検索ボタン
    
    void setSelectButtonData();
    void setToggleTeamTypeCallBack(Ref* sender);
    void setTogglePlayTypeCallBack(Ref* sender);
    void onEnter() ; //初期実行関数
    
    /* EditBox処理 */
    virtual void editBoxEditingDidBegin(ui::EditBox *editBox);
    virtual void editBoxEditingDidEnd(ui::EditBox *editBox);
    virtual void editBoxTextChanged(ui::EditBox *editBox, const std::string& text);
    virtual void editBoxReturn(ui::EditBox *editBox);
};

#endif /* defined(__war__TeamSearchConditionPopupLayer__) */
