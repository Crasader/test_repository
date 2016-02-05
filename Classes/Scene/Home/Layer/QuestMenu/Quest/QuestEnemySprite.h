#ifndef __war__QuestQuestEnemySprite__
#define __war__QuestQuestEnemySprite__

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

class QuestEnemySprite : public Sprite
{
private:
    CC_SYNTHESIZE(string, _chara_id, CharaId); //キャラID
    CC_SYNTHESIZE(string, _rare, Rare); //キャラのレア度
    CC_SYNTHESIZE(string, _rank_name, RankName); //階級名
    CC_SYNTHESIZE(string, _npc_name, NpcName);//npcユーザー名
    CC_SYNTHESIZE(string, _battle_point_rate, BattlePointRate); //ポイント倍率を設定
    
    CC_SYNTHESIZE(string, _offense, Offense);       //攻撃
    CC_SYNTHESIZE(string, _defense, Defense);       //防御
    CC_SYNTHESIZE(string, _strategist, Strategist); //軍師
    CC_SYNTHESIZE(string, _general, General);       //大将
    
    void showCardImage(); //カードの表示
    
public:
    float ratio;
    QuestEnemySprite(); //コンストラクタ
    
    static QuestEnemySprite* create(json11::Json json);
    virtual bool init(json11::Json json);
    
    void setupData(Json json); //ユーザー情報の設定
    void showData(); //ユーザー情報の表示
    void showDeckParam(); //デッキパラメータの表示
    void showExperience(); //経験値の表示
    
    void createCharaSprite(); //キャラ画像の作成
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__QuestQuestEnemySprite__) */
