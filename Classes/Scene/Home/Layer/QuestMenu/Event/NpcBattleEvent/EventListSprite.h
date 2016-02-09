#ifndef __War__EventListSprite__
#define __War__EventListSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "json11.hpp"

#define FIRST_CARD_POS_X 190
#define FIRST_CARD_POS_Y 68

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class EventListSprite : public Sprite
{
public:
    string _name_or_reward;
    string filename;
private:
    
    enum TAG{
        NEW_TAG = 1,
        ARROW_TAG,
        T_NEED_LABEL,
        T_NEED_POWER,
    };
    
    HmsVector<Sprite *> _reward_sp; //報酬画像
    HmsVector<Json >_reward_data_list; //報酬データ
    Json _event_rank_data; //イベントランクデータ
    
public:
    CC_SYNTHESIZE(string, _npc_battle_event_rank_list_id, NpcBattleEventRankListId); //イベントのID
    CC_SYNTHESIZE(string, _event_rank_name, EventRankName); //イベントランク名
    CC_SYNTHESIZE(string, _event_rank_level, EventRankLevel); //イベントランクレベル
    CC_SYNTHESIZE(string, _need_power, NeedPower); //必要行動力
    
    CC_SYNTHESIZE(string, _card_image_id, CardImageId); //最終確認画面で表示するキャラID
    CC_SYNTHESIZE(string, _user_name, UserName); //最終確認画面で表示するユーザ名
    CC_SYNTHESIZE(string, _comment, Comment); //最終確認画面で表示するコメント
    CC_SYNTHESIZE(string, _gold, Gold); //最終確認画面で表示するコメント
    CC_SYNTHESIZE(string, _reward_point, RewardPoint); //最終確認画面で表示するコメント
    CC_SYNTHESIZE(string, _rank_point, RankPoint); //最終確認画面で表示するコメント
    
    CC_SYNTHESIZE(bool, _new_flg, NewFlg); //イベントNewフラグ
    CC_SYNTHESIZE(bool, _clear_flg, ClearFlg); //イベントクリアフラグ
    
    CC_SYNTHESIZE(string, _reward_text1, RewardText1);
    CC_SYNTHESIZE(string, _reward_text2, RewardText2);
    CC_SYNTHESIZE(string, _reward_text3, RewardText3);
    CC_SYNTHESIZE(int, _reward_flg1, RewardFlg1);
    CC_SYNTHESIZE(int, _reward_flg2, RewardFlg2);
    CC_SYNTHESIZE(int, _reward_flg3, RewardFlg3);
    
    EventListSprite(); //コンストラクタ
    
    float ratio;
    
    static EventListSprite* create(Json json, string name_or_reward);
    virtual bool init(Json json);
    
    void setData(); //データの設定
    void createRewardSprite(); //報酬画像の作成
    string getImagePass(Json reward_data); //画像パスの取得
    void showRankName(); //ランク名の表示
    void showNeedPower(); //消費バトルポイントの表示
    
    void createRewardList(); //報酬一覧の並び替え
    
    void showNewSprite(); //New画像の表示
    void actionNewSprite(); //New画像のアクション
    void showArrow(); //矢印の表示
    void actionArrow(); //矢印のアクション
    
    void showClearSprite(); //クリア済み画像の表示
    
    void changeVisibleSprite(bool visible_flg); //スプライトの表示切り替え
    
    void showReward();
    void showStarSprite(); //星画像の表示
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__War__EventListSprite__) */
