#ifndef __War__FriendUserDetailSprite__
#define __War__FriendUserDetailSprite__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;
using namespace json11;

class FriendUserDetailSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _friend_card_id, FriendCardId); //フレンドIDを設定
    CC_SYNTHESIZE(string, _friend_id, FriendId); //フレンドIDを設定
    CC_SYNTHESIZE(string, _user_name, UserName);//ユーザ名を設定
    CC_SYNTHESIZE(string, _level, Level);//ユーザ名を設定
    CC_SYNTHESIZE(string, _rank_name, RankName); //階級名を設定
    CC_SYNTHESIZE(string, _user_comment, UserComment); //ユーザーコメントの設定

public:
    FriendUserDetailSprite(); //コンストラクタ
    float ratio;
    
    static FriendUserDetailSprite* create(string friend_id, string rank_name, string user_name, string level,string comment, string card_image_id);
    virtual bool init(string friend_id, string rank_name, string user_name, string level,string comment, string card_image_id);
    
    void showUserData(); //ユーザデータの表示
    
    void createFriendCard(); //フレンドカードの作成
    void createFriendId(); //フレンドID画像の作成
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__FriendUserDetailSprite__) */
