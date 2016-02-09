#ifndef __War__HelperSprite__
#define __War__HelperSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "json11.hpp"

#define DAY_1_SECOND 86400

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class HelperSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _user_master_id, UserMasterId); //user_master_idを設定
    CC_SYNTHESIZE(string, _card_image_id, CardImageId); //画像カードIDを設定
    CC_SYNTHESIZE(string, _rank_name, RankName); //階級名を設定
    CC_SYNTHESIZE(string, _user_name, UserName);//ユーザ名を設定
	CC_SYNTHESIZE(string, _user_level, UserLevel);//ユーザレベルを設定
    CC_SYNTHESIZE(string, _friend_id, FriendId);//フレンドIDを設定
    CC_SYNTHESIZE(string, _last_play_date, LastPlayDate);//最終ログイン日時を設定
    CC_SYNTHESIZE(string, _comment, Comment);//最終ログイン日時を設定
    CC_SYNTHESIZE(bool, _follow_flg, FollowFlg);//フォローしているか判定設定
	CC_SYNTHESIZE(string, _chara_type, CharaType);//フレンドカードのタイプを設定
	CC_SYNTHESIZE(string, _chara_param1, CharaParam1);//フレンドカードのパラメータ1を設定
	CC_SYNTHESIZE(string, _chara_param2, CharaParam2);//フレンドカードのパラメータ2を設定
    
    CC_SYNTHESIZE(string , _json, Json); //Jsonの情報を保持
    
    Sprite* _card_image; //カード画像のスプライト
    void showCardImage(); //カードの表示
    
public:
    float ratio;
    HelperSprite(); //コンストラクタ
    
    static HelperSprite* create(json11::Json json);
    virtual bool init(json11::Json json);
    
    void setupData(Json json); //ユーザー情報の設定
    void showData(); //ユーザー情報の表示
    string parseLastPlayDate(); //最終ログイン日時を分解
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__HelperSprite__) */
