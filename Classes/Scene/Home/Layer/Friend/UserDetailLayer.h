#ifndef __War__UserDetailLayer__
#define __War__UserDetailLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "UserDetailSprite.h"
#include "popupConfirmLayer.h"

USING_NS_CC;
using namespace std;

class UserDetailLayer : public cocos2d::Layer
{
private:
    bool _follow_flg; //フォローしているか
    Json _user_data; //取得したユーザデータ
    UserDetailSprite* userDetail; //ユーザ情報画像
    string _page_name; //どこから来たか
	
public:
    float ratio;
	
    static UserDetailLayer* create(string page_name);
    virtual bool init(string page_name);
    
    void createTitleBar(); //タイトルバーの作成
    void createUserDetailSprite(); //ユーザー情報の作成
    void showSprite(); //ユーザ情報、スキル画像の表示
    void createButton(); //ボタンの作成
    
    void onEnter(); //初期実行関数
    
    //ユーザデータの取得通信処理
    void getUserDataRequest();
    void UserDataCallback(HttpClient *sender, HttpResponse *response);
    
    //フォロー申請通信処理
    void demandFollowRequest();
    void demandFollowCallback(HttpClient *sender, HttpResponse *response);
    
    //フォロー解除通信処理
    void removeFollowRequest();
    void removeFollowCallback(HttpClient *sender, HttpResponse *response);
};

#endif /* defined(__War__UserDetailLayer__) */
