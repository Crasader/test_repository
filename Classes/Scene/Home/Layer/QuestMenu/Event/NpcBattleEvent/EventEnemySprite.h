#ifndef __war__EventEnemySprite__
#define __war__EventEnemySprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class EventEnemySprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _user_master_id, UserMasterId); //user_master_idを設定
    CC_SYNTHESIZE(string, _card_image_id, CardImageId); //画像カードIDを設定
    CC_SYNTHESIZE(string, _level, Level); //レベル
    CC_SYNTHESIZE(string, _rank_name, RankName); //階級名を設定
    CC_SYNTHESIZE(string, _user_name, UserName);//ユーザ名を設定
    CC_SYNTHESIZE(string, _battle_point_rate, BattlePointRate); //ポイント倍率を設定
    
    CC_SYNTHESIZE(string, _offense, Offense);       //攻撃
    CC_SYNTHESIZE(string, _defense, Defense);       //防御
    CC_SYNTHESIZE(string, _strategist, Strategist); //軍師
    CC_SYNTHESIZE(string, _general, General);       //大将
    CC_SYNTHESIZE(string , _json, Json); //Jsonの情報を保持
    
    Sprite* _card_image; //カード画像のスプライト
    void showCardImage(); //カードの表示
    
public:
    float ratio;
    EventEnemySprite(); //コンストラクタ
    
    static EventEnemySprite* create(json11::Json json);
    virtual bool init(json11::Json json);
    
    void setupData(Json json); //ユーザー情報の設定
    void showData(); //ユーザー情報の表示
    void showDeckParam(); //デッキパラメータの表示
    void showMagnification(); //獲得倍率の表示
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__EventEnemySprite__) */
