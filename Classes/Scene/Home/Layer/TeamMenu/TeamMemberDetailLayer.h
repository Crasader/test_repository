#ifndef __War__TeamMemberDetailLayer__
#define __War__TeamMemberDetailLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "TeamMemberDetailSprite.h"
#include "popupConfirmLayer.h"

USING_NS_CC;
using namespace std;

class TeamMemberDetailLayer : public cocos2d::Layer, public ui::EditBoxDelegate
{
private:
    Json _user_data; //取得したユーザデータ
    TeamMemberDetailSprite* userDetail; //ユーザ情報画像
    string _page_name; //どこから来たか
    string invited_comment; //勧誘メッセージ
    
    enum Tag{
        T_invited_comment_box = 100,
        T_COMMENT_LABEL,
        T_BACK_POPUP,
    };
	
public:
    float ratio;
    EventListenerTouchOneByOne *listener;
	
    static TeamMemberDetailLayer* create(string page_name);
    virtual bool init(string page_name);
    
    void onEnter(); //初期実行関数
    
    void createTitleBar(); //タイトルバーの作成
    void createTeamMemberDetailSprite(); //ユーザー情報の作成
    void showSprite(); //ユーザ情報、スキル画像の表示
    void createButton(); //ボタンの作成
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //httpコールバック
    void appYesCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void appNoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void resignCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void inviteCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void cancelInvitationCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void changeLeaderCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    /* EditBox処理 */
    virtual void editBoxEditingDidBegin(ui::EditBox *editBox);
    virtual void editBoxEditingDidEnd(ui::EditBox *editBox);
    virtual void editBoxTextChanged(ui::EditBox *editBox, const std::string& text);
    virtual void editBoxReturn(ui::EditBox *editBox);
};

#endif /* defined(__War__TeamMemberDetailLayer__) */
