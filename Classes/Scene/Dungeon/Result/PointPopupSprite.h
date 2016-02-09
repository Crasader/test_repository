#ifndef __War__PointPopupSprite__
#define __War__PointPopupSprite__

#include "cocos2d.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "SimpleAudioEngine.h"

#define MAX_RANK_ID 35
#define MAX_REWARD_ID 86

#define EFFECT_TAG 1

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class PointPopupSprite : public Sprite
{
private:
    CC_SYNTHESIZE(int, _get_rank_point, RankPoint);
    CC_SYNTHESIZE(int, _get_reward_point, RewardPoint);
    CC_SYNTHESIZE(int, _now_rank_point, NowRankPoint);
    CC_SYNTHESIZE(int, _need_rank_point, NeedRankPoint);
    CC_SYNTHESIZE(int, _now_reward_point, NowRewardPoint);
    CC_SYNTHESIZE(int, _need_reward_point, NeedRewardPoint);
    CC_SYNTHESIZE(string, _next_rank_name, NextRankName);
    CC_SYNTHESIZE(int, _next_level, NextLevel);
    
    CC_SYNTHESIZE(int, _before_rank_point_id, BeforeRankPointId);
    CC_SYNTHESIZE(int, _after_rank_point_id, AfterRankPointId);
    CC_SYNTHESIZE(int, _before_reward_point_id, BeforeRewardPointId);
    CC_SYNTHESIZE(int, _after_reward_point_id, AfterRewardPointId);
    CC_SYNTHESIZE(int, _before_level, BeforeLevel);
    CC_SYNTHESIZE(int, _after_level, AfterLevel);
    CC_SYNTHESIZE(int, _max_reward_point_list_id, MaxRewardPointListId);
    CC_SYNTHESIZE(int, _max_rank_point_list_id, MaxRankPointListId);
    
    
    //Jsonで送られてきたリスト
    Json _before_user_data_list;
    Json _after_user_data_list;
    Json _add_point_list;
    Json _rank_data_list;
    Json _reward_data_list;
    
    Sprite* _rank_point_bar; //ランクポイントのゲージ
    Sprite* _reward_point_bar; //報酬ポイントのゲージ
    Label* _get_rank_point_label; //獲得ランクポイント
    Label* _get_reward_point_label; //獲得報酬ポイント
    Label* _need_rank_point_label; //必要ランクポイント
    Label* _need_reward_point_label; //必要報酬ポイント
    Label* _rank_name_label; //ランク名
    Label* _level_label; //レベル
    
    int _rank_data_count; //ランクポイントデータの数
    int _reward_data_count; //報酬ポイントデータの数
    int _change_rank_count; //レベルアップしたランクの数
    int _change_reward_count; //レベルアップした報酬ポイントの数
    
    bool _rank_effect_flg; //ランクエフェクトフラグ
    bool _reward_effect_flg; //リワードエフェクトフラグ
    int _gauge_speed_up_flg; //ゲージの増加量を増やすフラグ
    
public:
    enum PopupType{
        TYPE_ALL_POINT = 0, //獲得金, 称号Pt, 経験値
        TYPE_NO_RANK_POINT //獲得金, 経験値
    };
    
    //ポップアップの種類によって表示位置を切り替えるための変数
    PopupType _popup_type ;
    
    float ratio;
    
    PointPopupSprite(); //コンストラクタ
    
    static PointPopupSprite* create(PopupType popup_type, Json json);
    virtual bool init(PopupType popup_type, Json json);
    
    //void loadData(Json json); //データの読み込み
    void setData(); //データの設定
    void createRewardMoneyLabel(); //獲得金の文字作成
    void createNextRankLabel(); //次のランクの文字作成
    void createNextLevelLabel(); //次のレベルの文字作成
    void createGaugeBg(); //ゲージの背景画像の作成
    void showCurrentPointGauge(); //現在のポイントを表示
    void showGetPoint(); //獲得したポイントの表示
    void showNeedPoint(); //レベルアップに必要なポイントの表示
    void checkRankPointMax(); //ランクが最大チェック
    void checkRewardPointMax(); //報酬ランクが最大チェック
    
    void gaugeAction(float dt); //ゲージ増減アクション
    void rankPointUpAction(float dt); //ランクポイント増加アクション
    void rankPointDownAction(float dt); //ランクポイント減少アクション
    void changeRankData(); //ランクデータの更新
    
    void rewardPointAction(float dt); //報酬ポイントアクション
    void rewardPointUpAction(float dt); //報酬ポイント増加アクション
    void rewardPointDownAction(float dt); //報酬ポイント減少アクション
    void changeRewardData(); //報酬ポイントデータの更新

    void gaugeSpeedUp() ; //ゲージの増加スピードを早くする
    void showfinishData(); //アクション完了後の表示
    void showfinishRankData(); //振り分け後のランクデータ表示
    void showfinishRewardData(); //振り分け後の報酬ポイントデータ表示
    int gaugeSpeedByNeedPoint(int need_point) ; //ゲージの増減スピードを必要ポイントによって求める
    
    void showEffect(string filename, float scale, Point pos, function<void()> _func); //エフェクトの表示
    function<void()> removeEffectFunc(); //エフェクトを消すラムダ式を返す関数
    
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__PointPopupSprite__) */
