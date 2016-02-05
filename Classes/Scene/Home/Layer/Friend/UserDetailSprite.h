#ifndef __War__UserDetailSprite__
#define __War__UserDetailSprite__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;
using namespace json11;

class UserDetailSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _user_master_id, UserMasterId); //フォローユーザマスターIDを設定
    CC_SYNTHESIZE(string, _friend_card_id, FriendCardId); //フレンドIDを設定
    CC_SYNTHESIZE(string, _friend_id, FriendId); //フレンドIDを設定
    CC_SYNTHESIZE(string, _level, Level);//ユーザレベル
    CC_SYNTHESIZE(string, _user_name, UserName);//ユーザ名を設定
    CC_SYNTHESIZE(string, _rank_name, RankName); //階級名を設定
    CC_SYNTHESIZE(string, _user_comment, UserComment); //ユーザーコメントの設定

public:
    UserDetailSprite(); //コンストラクタ
    float ratio;
    
    static UserDetailSprite* create();
    virtual bool init();
    
    void setupUserData(Json json); //ユーザデータの取得
    void showUserData(); //ユーザデータの表示
    
    void createFriendCard(); //フレンドカードの作成
    void createFriendId(); //フレンドID画像の作成
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__UserDetailSprite__) */
