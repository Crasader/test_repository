#ifndef __war__CardDetailLayer__
#define __war__CardDetailLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"

USING_NS_CC;
using namespace std;

class CardDetailLayer: public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    CC_SYNTHESIZE(string, _chara_list_id, CharaListId); //キャラリストID
    CC_SYNTHESIZE(string, _chara_id, CharaId); //キャラID
    CC_SYNTHESIZE(string, _chara_no, CharaNo); //キャラナンバー
    CC_SYNTHESIZE(string, _second_name, SecondName); //２つ名
    CC_SYNTHESIZE(string, _name, Name); //キャラ名
    CC_SYNTHESIZE(string, _name_kana, NameKana); //キャラフリガナ
    CC_SYNTHESIZE(string, _rare, Rare); //レア度
    CC_SYNTHESIZE(CharaSprite::CharaType, _chara_type, CharaType); //キャラタイプ 1.攻撃 2.防御 3.軍師 4.総大将
    CC_SYNTHESIZE(CharaSprite::Combo, _left_joint, LeftJoint); //左コンボマーク 0.なし 1.赤 2.青 3.黄
    CC_SYNTHESIZE(CharaSprite::Combo, _right_joint, RightJoint); //右コンボマーク
    
    CC_SYNTHESIZE(string, _hp, Hp); //キャラHP
    CC_SYNTHESIZE(string, _power, Power); //キャラ戦闘力
    CC_SYNTHESIZE(string, _need_cost, NeedCost); //配置必要コスト
    CC_SYNTHESIZE(string, _add_cost, AddCost); //総大将の増える士気量
    CC_SYNTHESIZE(string, _turn_count, TurnCount); //軍師の効力継続ターン数
    CC_SYNTHESIZE(string, _limit_count, LimitCount); //デッキ制限枚数
    
    CC_SYNTHESIZE(string, _active_skill_id, ActiveSkillId); //アクティブスキルID
    CC_SYNTHESIZE(string, _passive_skill_id, PassiveSkillId); //パッシブスキルID
    CC_SYNTHESIZE(string, _chara_comment, CharaComment); //キャラ説明
    CC_SYNTHESIZE(string, _add_timing, AddTiming); //キャラ追加のタイミング名
    
    //アクティブスキルデータ
    CC_SYNTHESIZE(string, _active_skill_name, ActiveSkillName); //アクティブスキル名
    CC_SYNTHESIZE(string, _active_skill_detail, ActiveSkillDetail); //スキル詳細
    CC_SYNTHESIZE(string, _active_skill_turn_count, ActiveSkillTurnCount); //継続ターン
    CC_SYNTHESIZE(string, _active_skill_need_cost, ActiveSkillNeedCost); //必要コスト
    
    
    //パッシブスキルデータ
    CC_SYNTHESIZE(string, _passive_skill_name, PassiveSkillName); //パッシブスキル名
    CC_SYNTHESIZE(string, _passive_skill_detail, PassiveSkillDetail); //スキル詳細
    CC_SYNTHESIZE(string, _passive_skill_turn_count, PassiveSkillTurnCount); //継続ターン
    CC_SYNTHESIZE(string, _passive_skill_need_cost, PassiveSkillNeedCost); //必要コスト
    
    //通信で返ってきたデータを格納する変数
    json11::Json _chara_data;
    json11::Json _active_skill_data;
    json11::Json _passive_skill_data;
    
    Sprite* _bg; //背景
    
    Label* _skill_name;
    Label* _skill_detail;
    Label* _skill_need_cost;
    Label* _skill_text; //発動士気という文字
    
    CC_SYNTHESIZE(function<void()>, _close_func, CloseFunc); //閉じるボタンを押したときに呼ばれるfunction
    
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Chara_Sprite,
        T_Close_MenuItem,
        T_Close_Menu,
        T_Lock_MenuItem,
        T_Lock_Menu,
        T_Name,
        T_Second_Name,
        T_Type_Icon,
        T_Power,
        T_Cost,
        T_Skill_Switch_Button,
        T_Skill_Tab,
        T_Skill_MenuItem,
        T_Skill_Menu,
        T_Skill_Name,
        T_Skill_Text,
        T_Skill_Need_Cost,
        T_Skill_Turn_Count,
        T_Skill_Detail,
        T_Comment,
        T_Popup_Layer,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Shadow = 1,
        Z_Chara_Sprite,
        Z_Close_MenuItem,
        Z_Close_Menu,
        Z_Lock_MenuItem,
        Z_Lock_Menu,
        Z_Name,
        Z_Second_Name,
        Z_Type_Icon,
        Z_Power,
        Z_Cost,
        Z_Skill_Switch_Button,
        Z_Skill_Tab,
        Z_Skill_MenuItem,
        Z_Skill_Menu,
        Z_Skill_Name,
        Z_Skill_Text,
        Z_Skill_Need_Cost,
        Z_Skill_Turn_Count,
        Z_Skill_Detail,
        Z_Comment,
        Z_Popup_Layer,
    };
    
    //表示ステータス
    enum SkillStatus{
        Active_Skill = 1,
        Passive_Skill,
        None,
    };
    
    SkillStatus _skill_status;
    
    CardDetailLayer(); //コンストラクタ
    
    static CardDetailLayer* create(string chara_id, string chara_list_id = "");
    virtual bool init(string chara_id, string chara_list_id);
    
    float ratio;
    
    void initEventListener(); //イベントリスナーの設定
    void setCharaData(json11::Json json); //キャラデータの設定
    void getCharaData();
    void createLayer();
    void createBg(); //背景の作成
    void createCharaSprite(); //キャラ画像の生成
    void createFavoriteButton(); //お気に入りボタンの作成
    void createNameLabel(); //名前の作成
    void createCharaStatus(); //キャラのステータス（戦闘力など）の生成
    void showSkill(); //スキルの表示
    void showCharaComment(); //キャラ説明の表示
    
    void changeSkillDetail(Ref* sender); //パッシブ、アクティブ両方持ちだった場合切り変える
    
    //カード情報取得通信処理
    void getCharaDataRequest();
    void getCharaDataCallback(HttpClient *sender, HttpResponse *response);
    
    void onEnter() ; //初期実行関数
};

#endif /* defined(__war__CardDetailLayer__) */
