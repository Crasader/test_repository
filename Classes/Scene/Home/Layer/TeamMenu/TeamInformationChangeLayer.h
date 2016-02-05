#ifndef __War__TeamInformationChangeLayer__
#define __War__TeamInformationChangeLayer__

#include "cocos2d.h"
#include "common.h"
#include "cocos-ext.h"
#include "HttpConnect.h"
#include "json11.hpp"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
using namespace json11;

//情報変更
class TeamInformationChangeLayer : public Layer , public ui::EditBoxDelegate
{
private:
    enum ZOrder{
       O_BACK_POPUP,
    };
    
    enum Tag{
       T_BACK_POPUP=300,
       T_SAVE,
       T_EDITBOX_NAME,
       T_EDITBOX_COMMENT,
       T_COMMENT_LABEL,
    };
    
    float ratio;
    EventListenerTouchOneByOne *listener;
    
    Json _json_team;
    
    //チーム名
    string _input_team_name;
    //チームコメント
    string _input_team_comment;
    //ギルドタイプ
    string _team_type;
    //プレイタイプ
    string _play_type;
public:
    virtual bool init();
    CREATE_FUNC(TeamInformationChangeLayer);
    
    void onEnter(); //初期実行関数
    void getTeamData();
    void getTeamDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
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
    
    virtual void editBoxEditingDidBegin(ui::EditBox* editBox);
    virtual void editBoxEditingDidEnd(ui::EditBox* editBox);
    virtual void editBoxTextChanged(ui::EditBox* editBox, const std::string& text);
    virtual void editBoxReturn(ui::EditBox* editBox);

};

#endif