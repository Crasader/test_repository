#ifndef __War__BossEventListSprite__
#define __War__BossEventListSprite__

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

class BossEventListSprite : public Sprite
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
    Json _boss_event_stage_data; //イベントステージ情報
    
public:
    CC_SYNTHESIZE(string, _boss_event_id, BossEventId); //イベントID
    CC_SYNTHESIZE(string, _boss_event_stage_log_id, BossEventStageLogId); //イベントステージID
    CC_SYNTHESIZE(string, _boss_name, BossName); //イベントランク名
    CC_SYNTHESIZE(string, _comment_line1, CommentLine1); //コメント1
    CC_SYNTHESIZE(string, _comment_line2, CommentLine2); //コメント2
    CC_SYNTHESIZE(int, _player_count, PlayerCount); //参加プレイヤー数
    CC_SYNTHESIZE(string, _boss_event_stage_level, BossEventStageLevel); //イベントレベル
    CC_SYNTHESIZE(string, _find_user_name, FindUserName); //発見者の名前
    CC_SYNTHESIZE(int, _hp, Hp); //HP
    CC_SYNTHESIZE(int, _max_hp, MaxHp); //最大HP
    CC_SYNTHESIZE(long, _rest_time, RestTime); //残り時間
    CC_SYNTHESIZE(long, _end_time, EndTime); //終了時間
    CC_SYNTHESIZE(string, _need_power, NeedPower); //必要行動力
    
    CC_SYNTHESIZE(string, _card_image_id, CardImageId); //最終確認画面で表示するキャラID
    CC_SYNTHESIZE(string, _user_name, UserName); //最終確認画面で表示するユーザ名
    CC_SYNTHESIZE(string, _comment, Comment); //最終確認画面で表示するコメント
    CC_SYNTHESIZE(string, _gold, Gold); //最終確認画面で表示するコメント
    CC_SYNTHESIZE(string, _reward_point, RewardPoint); //最終確認画面で表示するコメント
    CC_SYNTHESIZE(string, _rank_point, RankPoint); //最終確認画面で表示するコメント
    
    CC_SYNTHESIZE(bool, _new_flg, NewFlg); //イベントNewフラグ
    CC_SYNTHESIZE(bool, _clear_flg, ClearFlg); //イベントクリアフラグ
    CC_SYNTHESIZE(bool, _open_flg, OpenFlg); //開放済みか
    
    BossEventListSprite(); //コンストラクタ
    
    float ratio;
    
    static BossEventListSprite* create(Json json, string name_or_reward);
    virtual bool init(Json json);
    
    void setData(); //データの設定
    void createRewardSprite(); //報酬画像の作成
    string getImagePass(Json reward_data); //画像パスの取得
    void showText(); //ランク名の表示
    
    Sprite *_key ;
    void createKey(); //鍵の作成
    void showKey(); //開放済みかどうか表示
    
    void createRewardList(); //報酬一覧の並び替え
    
    void showNewSprite(); //New画像の表示
    void actionNewSprite(); //New画像のアクション
    void showArrow(); //矢印の表示
    void actionArrow(); //矢印のアクション
    
    void showClearSprite(); //クリア済み画像の表示
    
    void changeVisibleSprite(bool visible_flg); //スプライトの表示切り替え
    
    string convertRestTime(long rest_time);
    void updateRestTime(float dt);
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__War__BossEventListSprite__) */
