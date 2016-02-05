#ifndef __war__CharaSprite__
#define __war__CharaSprite__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace json11;

class CharaSprite : public Sprite
{
public:
    CharaSprite(); //コンストラクタ

    //タグ
    enum Tag{
        T_NeedCost_Sp = 1,
        T_NeedCost_Label,
        T_Favorite,
        T_Exchange_Ticket,
        T_Left_Joint,
        T_Right_Joint,
        T_Chara_Type,
        T_Power,
        T_Turn_Count,
        T_Add_Cost,
        T_Hp,
        T_NEW,
        T_Node,
    };

    //Zオーダー
    enum ZOrder{
        Z_NeedCost_Sp = 1,
        Z_NeedCost_Label,
        Z_Favorite,
        Z_Exchange_Ticket,
        Z_Left_Joint,
        Z_Right_Joint,
        Z_Chara_Type,
        Z_Power,
        Z_Turn_Count,
        Z_Add_Cost,
        Z_Hp,
        Z_NEW,
        Z_Node,
    };

    //コンボマーク
    enum Combo{
        None = 0,
        Red,
        Blue,
        Yellow,
    };

    //レア度
    enum Rarity{
        NoneRarity = 0,
        Normal,
        Rare,
        SuperRare,
        UltraRare,
        UltraRare_Plus,
        LegendRare,
    };

    //キャラの属性
    enum CharaType {
        ALL = 0,
        OFFENSE,
        DEFENSE,
        STRATEGIST,
        GENERAL,
    };

    enum CardSize{
        Thumb_100 = 1,
        Thumb_400,
        ALL_100,
        ALL_400,
    };

    float ratio;
    static CharaSprite* create(Json json, bool new_flg = true, CardSize card_size = CardSize::Thumb_100);
    virtual bool init(Json json, bool new_flg, CardSize card_size);

    CardSize _card_size; //表示する画像サイズ

    CC_SYNTHESIZE(string, _chara_list_id, CharaListId); //キャラリストID
    CC_SYNTHESIZE(string, _chara_id, CharaId); //キャラID
    CC_SYNTHESIZE(string, _chara_no, CharaNo); //キャラナンバー
    CC_SYNTHESIZE(string, _second_name, SecondName); //２つ名
    CC_SYNTHESIZE(string, _name, Name); //キャラ名
    CC_SYNTHESIZE(string, _name_kana, NameKana); //キャラフリガナ
    CC_SYNTHESIZE(string, _rare, Rare); //レア度
    CC_SYNTHESIZE(CharaType, _chara_type, CharaType); //キャラタイプ 1.攻撃 2.防御 3.軍師 4.総大将
    CC_SYNTHESIZE(Combo, _left_joint, LeftJoint); //左コンボマーク 0.なし 1.赤 2.青 3.黄
    CC_SYNTHESIZE(Combo, _right_joint, RightJoint); //右コンボマーク

    CC_SYNTHESIZE(string, _hp, Hp); //キャラHP
    CC_SYNTHESIZE(string, _power, Power); //キャラ戦闘力
    CC_SYNTHESIZE(string, _need_cost, NeedCost); //配置必要コスト
    CC_SYNTHESIZE(string, _add_cost, AddCost); //総大将の増える士気量
    CC_SYNTHESIZE(string, _turn_count, TurnCount); //軍師の効力継続ターン数
    CC_SYNTHESIZE(string, _limit_count, LimitCount); //デッキ制限枚数
    CC_SYNTHESIZE(string, _exchange_ticket, ExchangeTicket); //売却時の獲得交換チケット枚数
    CC_SYNTHESIZE(string, _next_chara_id, NextCharaId); //進化先のキャラID
    CC_SYNTHESIZE(string, _prev_chara_id, PrevCharaId); //進化前のキャラID

    CC_SYNTHESIZE(string, _active_skill_id, ActiveSkillId); //アクティブスキルID
    CC_SYNTHESIZE(string, _passive_skill_id, PassiveSkillId); //パッシブスキルID
    CC_SYNTHESIZE(string, _chara_comment, CharaComment); //キャラ説明
    CC_SYNTHESIZE(string, _add_timing, AddTiming); //キャラ追加のタイミング名
    CC_SYNTHESIZE(string, _insert_date, InsertDate); //キャラ取得日時
    CC_SYNTHESIZE(time_t, _insert_date_timestamp, InsertDateTimestamp); //キャラ取得日時のタイムスタンプ
    
    int _chara_id_int;
    int _rare_int;
    int _power_int;
    int _need_cost_int;
    time_t _insert_date_int;

    //アクティブスキルデータ

    //パッシブスキルデータ

    CC_SYNTHESIZE(bool, _deck_used_flg, DeckUsedFlg); //デッキ使用中フラグ
    CC_SYNTHESIZE(bool, _new_flg, NewFlg); //Newマーク表示フラグ
    CC_SYNTHESIZE(bool, _evolution_flg, EvolutionFlg); //デッキ使用中フラグ

    void setupData(Json json); //データの設定
    void showCharaSprite(); //キャラ画像の表示
    void showPower(); //戦闘力の表示
    void showNeedCost(); //配置コストの表示
    void showCharaType(); //キャラタイプの表示
    void showCombo(); //コンボマークの表示

    void showNewSprite(); //NEWの表示
    time_t calcTimeStamp(); //タイムスタンプの取得

    void actionGeneralLabel(); //総大将の士気と体力を交互に出すアクション
    void createDisableCascadeNode(); //透過、色を適用させないオブジェクトを載せるときこのNodeを使用する（売却の番号など）

    void setFavoriteSprite(); //お気に入り画像をつける
    void setExchangeTicketSprite(); //交換チケット画像の表示

    void onEnter(); //初期実行関数
    
    static HmsVector<CharaSprite *> sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param);
    
    
    void setCharaSpriteData(HmsVector<CharaSprite *> chara, HmsVector<int *> refine_index_list, HmsVector<Point  *> chara_pos_index_list, int Zorder); //キャラ画像一覧の表示
    void showCharaSpriteList();
    void showCharaSpriteListInterval(float f); //キャラ画像一覧の表示(順番に)
    int _view_count_default;
    int viewCount;
    int zOrder;
    HmsVector<CharaSprite *> _chara; //キャラ画像
    HmsVector<int> _refine_index_list; //絞り込み該当インデックスリスト
    HmsVector<Point> _position_index_list; //キャラのポジションインテックスリスト

};

#endif /* defined(__war__CharaSprite__) */
