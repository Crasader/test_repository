#ifndef __War__Chara__
#define __War__Chara__

#include "cocos2d.h"
#include "Numeric.h"
#include "HmsVector.h"
#include "SimpleAudioEngine.h"
#include "json11.hpp"
#define CHARA_TYPE_ATTACK 1
#define CHARA_TYPE_DEFENSE 2
#define CHARA_TYPE_SUPPORT 3
#define CHARA_TYPE_LEADER 4
#define CHARA_TYPE_BALANCE 5

USING_NS_CC;
using namespace std;

class Chara : public Layer
{
public:
    map<string, Sprite *> sprite_map; //chara type cost_back combination_r combination_l reverse nokori rare_shine turn　name
    ParticleSystemQuad* particle_rare = nullptr;
    ParticleSystemQuad* particle_back = nullptr;
    ParticleSystemQuad* skill_able = nullptr;
    
    HmsVector<Sprite *> cost_sprite;
    HmsVector<Sprite *> turn_count_sprite;
    Numeric *numeric;
    RenderTexture *render_chara = nullptr;
    
    float ratio;
    int field_index=-1;//初期値-1　　0~7配置
    int hand_index=-1;//手札index 初期値-1　　0~5配置
    //int friendState=-1;//フレンドカードの状態。-1違う　0手札　1使用済み   hand_indexの6がフレンドに
    CC_SYNTHESIZE(int, belonging_index_, BelongingIndex);;//player_indexかenemy_indexが入る //getBelongIndex()で取るように変更
    CC_SYNTHESIZE(int, belonging_index_def_, BelongingIndexDef);
    bool reverse_flg = false;//表か裏か
    string card_state = "";
    
    CC_SYNTHESIZE(int, type_, Type);//1攻撃、2防御、3軍師、４総大将
    CC_SYNTHESIZE(int, type_def_, TypeDef);
    
    CC_SYNTHESIZE(int, chara_id_, CharaId);
    CC_SYNTHESIZE(int, chara_id_def_, CharaIdDef);
    
	CC_SYNTHESIZE(string, chara_name_, CharaName);//カード名
    CC_SYNTHESIZE(string, chara_name_def_, CharaNameDef);
    
	CC_SYNTHESIZE(string, chara_second_name_, CharaSecondName);//二つ名
    CC_SYNTHESIZE(string, chara_second_name_def_, CharaSecondNameDef);
    
	CC_SYNTHESIZE(int, rare_, Rare);//レア度→ N:1 R:2 SR:3 UR:4 LE:5
    CC_SYNTHESIZE(int, rare_def_, RareDef);
    
    int base_value;//カードの基本戦闘力（変わらない）
    CC_SYNTHESIZE(int, base_value_def_, BaseValueDef);
    
    int combo_value = -1;//コンボの効果を足した戦闘力
    int value = -1;//ステータス効果などを足した最終的な戦闘力
    
    int combination_count;
    int place_cost;
    CC_SYNTHESIZE(int, place_cost_def_, PlaceCostDef);
    
    CC_SYNTHESIZE(int, combination_l_, CombinationL);
    CC_SYNTHESIZE(int, combination_l_def_, CombinationLDef);
    CC_SYNTHESIZE(int, combination_r_, CombinationR);
    CC_SYNTHESIZE(int, combination_r_def_, CombinationRDef);
	CC_SYNTHESIZE(int, chara_turn_count_, CharaTurnCount);//軍師の生存ターン数
    CC_SYNTHESIZE(int, chara_turn_count_def_, CharaTurnCountDef);//軍師の生存ターン数初期値
    
    CC_SYNTHESIZE(int, active_skill_id_, ActiveSkillId);//アクティブスキルID※スキルパターンID
    CC_SYNTHESIZE(int, active_skill_id_def_, ActiveSkillIdDef);
    
    CC_SYNTHESIZE(int, active_skill_value_, ActiveSkillValue);//アクティブスキル効果量
    CC_SYNTHESIZE(int, active_skill_value_def_, ActiveSkillValueDef);
    CC_SYNTHESIZE(int, active_skill_cost_, ActiveSkillCost);//アクティブスキルコスト
    CC_SYNTHESIZE(int, active_skill_cost_def_, ActiveSkillCostDef);
	CC_SYNTHESIZE(int, active_skill_turn_, ActiveSkillTurn);//アクティブスキルの効果ターン数
    CC_SYNTHESIZE(int, active_skill_turn_def_, ActiveSkillTurnDef);
    
	CC_SYNTHESIZE(string, active_skill_name_, ActiveSkillName);//アクティブスキル名
    CC_SYNTHESIZE(string, active_skill_name_def_, ActiveSkillNameDef);
    CC_SYNTHESIZE(string, active_skill_target_, ActiveSkillTarget);//ターゲットタイプ
    CC_SYNTHESIZE(string, active_skill_target_def_, ActiveSkillTargetDef);
    CC_SYNTHESIZE(string, active_skill_detail_, ActiveSkillDetail);//アクティブスキル説明文
    CC_SYNTHESIZE(string, active_skill_detail_def_, ActiveSkillDetailDef);
	
    int active_skill_able_count;//アクティブスキル残り使用可能回数 毎ターン回復 -1は死ぬまでずっと使用不能
    
    CC_SYNTHESIZE(int, passive_skill_id_, PassiveSkillIdDef);//パッシブスキルID※スキルパターンID
    CC_SYNTHESIZE(int, passive_skill_id_def_, PassiveSkillId);
    CC_SYNTHESIZE(int, passive_skill_value_, PassiveSkillValue);//パッシブスキル効果量
    CC_SYNTHESIZE(int, passive_skill_value_def_, PassiveSkillValueDef);
	CC_SYNTHESIZE(string, passive_skill_name_, PassiveSkillName);//パッシブスキル名
    CC_SYNTHESIZE(string, passive_skill_name_def_, PassiveSkillNameDef);
	CC_SYNTHESIZE(string, passive_skill_target_, PassiveSkillTarget);//ターゲットタイプ
    CC_SYNTHESIZE(string, passive_skill_target_def_, PassiveSkillTargetDef);
    CC_SYNTHESIZE(string, passive_skill_detail_, PassiveSkillDetail);//パッシブスキル説明文
    CC_SYNTHESIZE(string, passive_skill_detail_def_, PassiveSkillDetailDef);
    
    string place_type = "combo";//AIの配置タイプ
    int place_priority = 10;//AIの配置優先度
    int skill_priority = 10;//AIのスキル優先度
    
    bool dead_flg = false;
    bool eliminate_flg = false;//完全削除フラグ
    bool used_skill_flg = false;//このターンにスキルを使ったか（一ターンに一回しかスキルを使えない仕様の場合） 現在はCPU用
    Point value_point;//valueの位置
    
    //状態異常・バッファ使用キャラと 効果量と 状態異常idと 残りターン
    HmsVector< map<string, int> > chara_status_active;//"primary" "skill_id" "value" "state" "turn"

    //かかっているパッシブ。かけたキャラのプライマリid とターン変化効果の値 と　状態異常のid
    HmsVector< map<string, int> > chara_status_passive;//"primary" "skill_id" "value" "state"
    
    
    void updateStatusIcon();//ステータスバッファアイコン更新
    Sprite *iconSprite[3];
    int currentIcon=100;
    
    
    virtual bool init();
    CREATE_FUNC(Chara);

    void initialize();
    void revival();//死亡からのリセット
    void showCardParam();
    void showSpriteDeck();
    void showSpriteHand();
    void showSpriteField();
    void showCost(string kind);
    void cardReverse(int player_index, bool open_flg);
    
    int getPrimaryId();
    bool isOnField();
    bool isInDeck();
    bool isInHand();//手札にあるか
    bool isFriend();
    
    int rightCharaFieldIndex();
    int leftCharaFieldIndex();
    void setFieldIndex(int field_index);
    int getFieldIndex();
    void setHandIndex(int hand_index);//手札index用
    int getHandIndex();
    bool isMyChara();
    bool isEnemyChara();
    
    bool isAttacker();
    bool isDefender();
    bool isAttackAndDefense();
    
	void setValue(int value);
    int getValue();
    void setBaseValue(int value);
    int getBaseValue();
    void resetCombinationCount();
    int getCombinationCount();
    void setPlaceCost(int place_cost);
    int getPlaceCost();

    bool statusCheck(int state);//状態異常を判定
    int skillCount(int state);
    float statusUpdown(int state);//状態異常のvalueを数える
    void valueUpSkill(int value, string type);//スキル用戦闘力増減
    
	//キャラの生存判定
    void setDeadFlg(bool dead_flg);
    bool getDeadFlg();
    bool isAlive();
    
    void displayImage();
    Sprite *getImage();
    vector<Chara *> getAroundFieldChara(string target);
    bool getAroundBool(string target);
    int getNearBlankFieldIndex();
    int reverseIndex(int index);
    void showValue();
    bool showValue(int new_value ,float delay=0.0f);
    void showTurnCount(int new_value ,float delay=0.0f);
    void charaNameTab(bool appear_flg = true);
    void setTypeSprite();
    
    void erase();
    void eraseLeader();
    void rebornLeader();
    bool erase_leader_flg = false;//総大将が消えるアニメをしたか
    void displayRedBlink(float delay_time);
    void displayDamage(int damage);
    void displayOverkill(int damage);
    void displayWhiteBlink(float delay_time);
    void showYellowBlink();
    void hideYellowBlink();
    void showSkillAble();//スキル使用可能エフェクト
    void hideSkillAble();
    bool skillAble();
    bool moveAble();
    void comboSpriteUpdate();
    
    void scaleUp(bool up_flg);
    //void mascotScale();
    Chara *copy();//このキャラのコピーを作る
    
    json11::Json exportData();
    void importData(json11::Json json);
};

#endif /* defined(__War__Chara__) */
