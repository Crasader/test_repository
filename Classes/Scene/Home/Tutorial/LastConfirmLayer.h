#ifndef __War__LastConfirmLayer__
#define __War__LastConfirmLayer__

#include "cocos2d.h"
#include "TutorialScene.h"
#include "DungeonScene.h"
#include "MessageTalkSprite.h"

USING_NS_CC;

class LastConfirmLayer : public cocos2d::Layer
{
private:
    MessageTalkSprite *message_talk_sprite ;
    
    Point window_default_position;
    Point window_down_position;
    
    Point message_default_position;
    Point message_up_position;
    
    
    
    enum LastConfirmLayerTag{
       TAG_WINDOW,
       TAG_MENU,
       TAG_MESSAGE_SPRITE
    };
    int step_state ; //0: 初期状態 1:1つ目しゃべりだし後  2:2つ目しゃべりだし後  3:3つ目しゃべりだし後
    void nextStep(float dt) ; //ちょうせんの喋りを進める
    void exchageWindowForMessage() ; //ちょうせんとwindowの位置を交換する
    
    //タップイベント
    bool onTouchBegan(Touch *touch, Event *unused_event) ;
    EventListenerTouchOneByOne *listener;
    Sprite* type_image;
    
    //タイトルシーンに遷移させる
    void changeSceneToTitleScene() ;
    
    string _user_name;
    string _generalType;
    
public:
    float ratio;
    MessageSprite* message_bg;
    Sprite* popup_bg;
    
    virtual bool init();
    CREATE_FUNC(LastConfirmLayer);
    
    void createEventLitener(); 
    void createMessage(); //メッセージの作成
    void initDeckSprite(); //詳細画像
    void createButton(); //ボタンの作成
    
//    void showUserName(); //ユーザー名の確認表示
    
    //通信処理
    void userDataInsertRequest();
    void userDataInsertCallback(HttpClient *sender, HttpResponse *response);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__LastConfirmLayer__) */
