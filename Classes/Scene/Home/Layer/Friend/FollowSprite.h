#ifndef __War__FollowSprite__
#define __War__FollowSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "json11.hpp"
#include <time.h>

#define DAY_1_SECOND 86400

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class FollowSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _user_master_id, UserMasterId); //フォローユーザマスターIDを設定
    CC_SYNTHESIZE(string, _friend_id, FriendId); //フレンドIDを設定
    CC_SYNTHESIZE(string, _friend_card_id, FriendCardId); //フレンドカードIDを設定
    CC_SYNTHESIZE(string, _friend_name, FriendName);//フレンドユーザ名を設定
    CC_SYNTHESIZE(string, _level, Level); //レベル
    CC_SYNTHESIZE(string, _last_play_date, LastPlayDate); //最終プレイ時間
    CC_SYNTHESIZE(string, _user_comment, UserComment); //ユーザーコメントの設定
    CC_SYNTHESIZE(string, _rank_name, RankName); //階級名を設定
    
    Sprite* friendCard; //フレンドカード
    
public:
    FollowSprite(); //コンストラクタ
    
    float ratio;
    void createFriendCard(); //フレンドカードの作成
    void showfriendCard(); //フレンドカードの表示
    
    void setupData(Json json, int count); //フレンド情報の設定
    void showData(); //フレンド情報の表示
    
    string parseLastPlayDate(); //最終ログイン日時を分解
    
    static FollowSprite* create(json11::Json json, int count);
    virtual bool init(json11::Json json, int count);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__FollowSprite__) */
