#ifndef __War__ChatBaseUpperLayer__
#define __War__ChatBaseUpperLayer__

#include "cocos2d.h"
#include "HmsVector.h"
#include "MapCharaLayer.h"
#include "HttpConnect.h"
#include "Utility.h"
#include "cocos-ext.h"

#include "json11.hpp"

using namespace json11;
USING_NS_CC;
USING_NS_CC_EXT;

class ChatBaseUpperLayer : public cocos2d::Layer, public ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    
    Sprite *_img_all;
    Sprite *_img_team;
    Sprite *_img_arrow;
    Sprite *_img_submit;
    Sprite *_img_new_mark;
    bool _enable_submit;
    
    void setEnableSubmit(bool flg);
    
    CC_SYNTHESIZE(bool, _tap_flg, TapFlg);
    Point _tap_point;
    int _arrow_height;
    int _y_lowest;
    int _y_highest;
    
    string _chat_type ; 
    
    //入力チャットコメント
    string _input_chat_comment;
    
    //前回のチャット内容
    string _pre_team_chat_data_json_dump;
    int _unread_count;
    
    //タップ位置がチャット部分に含まれるか
    bool checkPositionContainChatPlace(Point touch_point);
    
    //ダブルタップ判定
    bool checkDoubleTap() ;
    bool _just_before_arrow_tap_flg ;
    void updateDoubleTapCheck(float dt) ;
    
    //使用しているシーンの名前
    string _scene_name;
    
    enum tag{
        T_CHAT_ARROW = 101,
        T_CHAT_ALL,
        T_CHAT_TEAM,
        T_CHAT_SCROLL,
        T_CHAT_INPUT,
        T_CHAT_EDIT,
        T_CHAT_SELECT,
        T_CHAT_RED,
    };
    
    enum zOrder{
        Z_CHAT_SCROLL = 1,
        Z_CHAT_ARROW,
        Z_CHAT_ALL,
        Z_CHAT_TEAM,
        Z_CHAT_INPUT,
        Z_CHAT_SELECT,
        Z_CHAT_RED,
    };
    
    const float MARK_SIZE = 0.8;
    
    
    bool checkChatLayerClose() ; //チャットがしまわれているか
public:
    float ratio;
    Label *_mark_number_label;
    
    ChatBaseUpperLayer() ;
    static ChatBaseUpperLayer* create(string scene_name);
    virtual bool init(string scene_name);
    //virtual bool init();
    //CREATE_FUNC(ChatBaseUpperLayer);
    
    void onEnter(); //初期実行関数
    void onExit();
    void initEventListener(); //イベントリスナーの設定
    
    //チャット投稿リクエスト
    void httpTeamCommentContributeRequest(string comment) ;
    //チームチャット投稿完了
    void httpTeamCommentContributeCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    //チームチャット削除完了
    void httpTeamCommentDeleteCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    //チームチャット更新
    void httpTeamCommentRefreshRequest(float dt) ;
    //チームチャット更新完了
    void httpTeamCommentRefreshCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    //チャットタイプ切り替え
    void switchChatType(string chat_type) ;
    
    //ポップアップ表示
    void showPopup(string message);
    void showPopupSprite(Sprite *sprite);
    void createNewMark();
    void spawnNewMark(int number);
    void dispawnNewMark();
    void hideChatLayer() ;//チャットレイヤーを隠す
    void showHalfChatLayer() ;//チャットレイヤーを半分表示する
   
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void recreateChatLayer(Json json_user_data_list);
};

#endif /* defined(__War__ChatBaseUpperLayer__) */
