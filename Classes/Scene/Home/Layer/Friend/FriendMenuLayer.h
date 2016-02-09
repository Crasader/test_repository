#ifndef __War__FriendMenuLayer__
#define __War__FriendMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"

#define FOLLOW_LIST_TAG 1
#define FOLLOWER_LIST_TAG 2
#define FRIEND_SEARCH_TAG 3
#define FRIEND_INTRODUCTION_TAG 4

#define MENU_BUTTON_T 200 //タグ
#define MENU_BUTTON_Z 200 //Zオーダー

#define FRIEND_INTRODUCTION_URL HOST"user/?&p=page__friend_introduction"  //フレンド招待画面を開く用URL

USING_NS_CC;

class FriendMenuLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;

public:
    float ratio;
    string user_master_id = SaveData::loadString("user_master_id");

    virtual bool init();
    CREATE_FUNC(FriendMenuLayer);

    void setupButton(); //ボタンの設定
    void changePage(Ref* sender); //メニューボタン処理
    void initTwitterLine(); //ツイッター、ラインスプライトの作成
    
    void shopMenuRequest();
    void shopMenuCallback(HttpClient *sender, HttpResponse *response);
	//メニューボタンのSpriteを作成
	Sprite* menuButtonCreateSprite(string img_name, int count);
	
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__FriendMenuLayer__) */
