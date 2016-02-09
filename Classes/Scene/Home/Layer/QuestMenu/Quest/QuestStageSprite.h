#ifndef __war__QuestStageSprite__
#define __war__QuestStageSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "json11.hpp"

#define STAGE_ARROW_TAG 1
#define STAGE_NEW_TAG 2

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class QuestStageSprite : public Sprite
{
public:
    string _name_or_reward;
    string filename;
public:
    QuestStageSprite(); //コンストラクタ
    
    static QuestStageSprite* create(Json json, string name_or_reward);
    virtual bool init(Json json);
    
    CC_SYNTHESIZE(string, _stage_id, StageId); //ステージID
    CC_SYNTHESIZE(string, _stage_level, StageLevel); //ステージレベル
    CC_SYNTHESIZE(string, _stage_rank_name, StageRankName); //ステージランク名
    CC_SYNTHESIZE(string, _stage_name, StageName); //ステージ名
    CC_SYNTHESIZE(string, _stage_chara_id, StageCharaId); //ステージキャラID
    CC_SYNTHESIZE(string, _stage_chara_rare, StageCharaRare); //ステージキャラのレア度
    CC_SYNTHESIZE(string, _stage_npc_name, StageNpcName); //ステージnpc名
    
    CC_SYNTHESIZE(string, _gain_rank_point, GainRankPoint); //獲得ランクポイント
    CC_SYNTHESIZE(string, _gain_reward_point, GainRewardPoint); //獲得称号ポイント
    CC_SYNTHESIZE(string, _gain_gold, GainGold); //獲得資金
    
    
    CC_SYNTHESIZE(bool, _new_flg, QuestStageNewFlg); //ステージNewフラグ
    CC_SYNTHESIZE(bool, _clear_flg, QuestStageClearFlg); //ステージクリアフラグ
    
    CC_SYNTHESIZE(string, _need_power, NeedPower); //必要消費体力
    
    CC_SYNTHESIZE(string, _reward_text1, RewardText1);
    CC_SYNTHESIZE(string, _reward_text2, RewardText2);
    CC_SYNTHESIZE(string, _reward_text3, RewardText3);
    CC_SYNTHESIZE(int, _reward_flg1, RewardFlg1);
    CC_SYNTHESIZE(int, _reward_flg2, RewardFlg2);
    CC_SYNTHESIZE(int, _reward_flg3, RewardFlg3);
    
    void showStarSprite(); //星画像の表示
    
    float ratio;
    
    void setupData(Json json); //データの設定
    void showQuestStageName(); //クエスト名の表示
    void showNeedPower(); //必要体力の表示
    void showNewSprite(); //New画像の表示
    void actionNewSprite(); //New画像のアクション
    
    void showClearSprite(); //クリア済み画像の表示
    void showArrow(); //矢印の表示
    void actionArrow(); //矢印のアクション
    
    void changeVisibleSprite(bool visible_flg); //スプライトの表示切り替え
    
    void showReward();
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__QuestStageSprite__) */
