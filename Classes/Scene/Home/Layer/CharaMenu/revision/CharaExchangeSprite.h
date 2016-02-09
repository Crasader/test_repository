#ifndef __war__CharaExchangeSprite__
#define __war__CharaExchangeSprite__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"
#include "Utility.h"
#include "CharaSprite.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class CharaExchangeSprite : public Sprite
{
private:
    EventListenerTouchOneByOne *listener;
    
    CC_SYNTHESIZE(string, _exchange_chara_id, ExchangeCharaId); //交換キャラID
    CC_SYNTHESIZE(string, _chara_id, CharaId); //キャラID
    CC_SYNTHESIZE(string, _second_name, SecondName); //２つ名
    CC_SYNTHESIZE(string, _name, Name); //キャラ名
    CC_SYNTHESIZE(string, _name_kana, NameKana); //キャラフリガナ
    CC_SYNTHESIZE(string, _rare, Rare); //レア度
    CC_SYNTHESIZE(CharaSprite::CharaType, _chara_type, CharaType); //キャラタイプ 1.攻撃 2.防御 3.軍師 4.総大将
    
    CC_SYNTHESIZE(string, _hp, Hp); //キャラHP
    CC_SYNTHESIZE(string, _power, Power); //キャラ戦闘力
    CC_SYNTHESIZE(string, _need_cost, NeedCost); //配置必要コスト
    CC_SYNTHESIZE(string, _add_cost, AddCost); //総大将の増える士気量
    CC_SYNTHESIZE(string, _turn_count, TurnCount); //軍師の効力継続ターン数
    
    CC_SYNTHESIZE(string, _active_skill_id, ActiveSkillId); //アクティブスキルID
    CC_SYNTHESIZE(string, _passive_skill_id, PassiveSkillId); //パッシブスキルID
    
    CC_SYNTHESIZE(string, _start_date, StartDate); //交換掲載開始時間
    CC_SYNTHESIZE(string, _end_date, EndDate); //交換掲載終了時間
    
    //アクティブスキルデータ
    CC_SYNTHESIZE(string, _active_skill_name, ActiveSkillName); //アクティブスキルの名前
    CC_SYNTHESIZE(string, _active_skill_need_cost, ActiveSkillNeedCost); //アクティブスキルの発動士気
    
    //パッシブスキルデータ
    CC_SYNTHESIZE(string, _passive_skill_name, PassiveSkillName); //パッシブスキルの名前

    CC_SYNTHESIZE(string, _need_exchange_ticket, NeedExchngeTicket); //必要交換チケット枚数
    CC_SYNTHESIZE(bool, _exchange_flg, ExchangeFlg); //既に交換済みかチェック
    CC_SYNTHESIZE(bool, _show_date_flg, ShowDateFlg); //日付の表示フラグ
    
    bool _moving_flg; //動かしたかフラグ
    bool _shortage_exchange_ticket_flg; //必要交換チケット枚数を満たしているかフラグ
    
    MenuItem* exchange_btn;
    MenuItem* detail_btn;
public:
    
    //タグ
    enum Tag{
        T_Chara_Sprite = 1,
        T_Name,
        T_Icon_Sprite,
        T_Power,
        T_Need_Cost,
        T_Left_Label,
        T_Right_Label,
        T_Active_Skill_Need_Cost,
        T_Chara_Detail_MenuItem,
        T_Chara_Detail_Menu,
        T_Exchange_MenuItem,
        T_Exchange_Menu,
        T_Exchange_Ticket,
        T_End_Date_Label,
        T_Node,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Chara_Sprite = 1,
        Z_Name,
        Z_Icon_Sprite,
        Z_Power,
        Z_Need_Cost,
        Z_Left_Label,
        Z_Right_Label,
        Z_Active_Skill_Need_Cost,
        Z_Chara_Detail_MenuItem,
        Z_Chara_Detail_Menu,
        Z_Exchange_MenuItem,
        Z_Exchange_Menu,
        Z_Exchange_Ticket,
        Z_End_Date_Label,
        Z_Node,
    };
    
    Node* _node; //キャラ交換レイヤーのポインタを保持しておく
    Json _chara_data; //キャラデータ
    
    CharaExchangeSprite(); //コンストラクタ
    
    static CharaExchangeSprite* create(json11::Json json, Node* normal_scene);
    virtual bool init(json11::Json json, Node* normal_scene);
    
    float ratio;
    
    void initEventListener(); //イベントリスナーの設定
    string getBackSpritePath(json11::Json json); //背景画像のパスを調べて返す
    void setupData(json11::Json json); //データの設定
    void generateCharaSprite(); //キャラ画像の生成
    void createNameLabel(); //キャラ名の作成
    void createCharaTypeIcon(); //キャラタイプのアイコンの作成
    void showPower(); //戦闘力の表示
    void showNeedCost(); //配置士気の表示
    void showSkillName(); //スキル名の表示
    
    void showNeedExchangeTicketLabel(); //必要交換チケット枚数
    void createButton(); //キャラ詳細と交換ボタンの作成
    void showExchangeEndDate(); //交換終了期間の文字列
    string createExchageEndDateText(); //掲載終了時間の文字列の作成
    
    void createDisableCascadeNode(); //ノードの作成
    
    void setExchangedChara(); //交換済みの状態にする
    void setUnselectedExchangeButton(); //交換チケットが足りないときに、ボタンを押せなくする処理
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__CharaExchangeSprite__) */
