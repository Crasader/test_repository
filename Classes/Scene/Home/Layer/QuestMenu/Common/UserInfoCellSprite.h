#ifndef __War__UserInfoCellSprite__
#define __War__UserInfoCellSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "json11.hpp"
#include <time.h>

#define DAY_1_SECOND 86400

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class UserInfoCellSprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _user_master_id, UserMasterId); //user_master_idを設定
    CC_SYNTHESIZE(string, _friend_id, FriendId); //フレンドIDを設定
    CC_SYNTHESIZE(string, _card_image_id, CardImageId); //画像カードIDを設定
    CC_SYNTHESIZE(string, _user_name, UserName);//ユーザ名を設定
    CC_SYNTHESIZE(string, _last_play_date, LastPlayDate); //最終プレイ時間
    CC_SYNTHESIZE(string, _comment, Comment); //コメントの設定
    CC_SYNTHESIZE(string, _rank_name, RankName); //階級名を設定
    CC_SYNTHESIZE(string, _battle_point_rate, BattlePointRate); //ポイント倍率を設定
    
    Sprite* _card_image; //カード画像のスプライト
    void showCardImage(); //カードの表示
    void setupData(Json json, int count);//フレンド情報の設定
    string parseLastPlayDate(string lastLogin); //最終ログイン日時を分解
    float ratio;
    
public:
    CC_SYNTHESIZE(string , _json, Json); //Jsonの情報を保持
    UserInfoCellSprite(); //コンストラクタ
    
    void showData(); //フレンド情報の表示

    static UserInfoCellSprite* create(json11::Json json) ;
    static UserInfoCellSprite* create(json11::Json json, int count);
    
    virtual bool init(json11::Json json, int count);
    
    void onEnter(); //初期実行関数
};


#endif /* defined(__War__UserInfoCellSprite__) */
