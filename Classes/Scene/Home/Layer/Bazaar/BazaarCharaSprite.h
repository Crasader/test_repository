#ifndef __war__BazaarCharaSprite__
#define __war__BazaarCharaSprite__

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

class BazaarCharaSprite : public Sprite
{
private:
    EventListenerTouchOneByOne *listener;
    
    CC_SYNTHESIZE(string, bazaarLogId, BazaarLogId); //キャラID
    CC_SYNTHESIZE(string, charaId, CharaId); //キャラID
    CC_SYNTHESIZE(string, secondName, SecondName); //２つ名
    CC_SYNTHESIZE(string, name, Name); //キャラ名
    CC_SYNTHESIZE(string, nameKana, NameKana); //キャラフリガナ
    CC_SYNTHESIZE(string, rare, Rare); //レア度
    CC_SYNTHESIZE(CharaSprite::CharaType, charaType, CharaType); //キャラタイプ 1.攻撃 2.防御 3.軍師 4.総大将
    
    CC_SYNTHESIZE(string, hp, Hp); //キャラHP
    CC_SYNTHESIZE(string, power, Power); //キャラ戦闘力
    CC_SYNTHESIZE(string, needCost, NeedCost); //配置必要コスト
    CC_SYNTHESIZE(string, addCost, AddCost); //総大将の増える士気量
    CC_SYNTHESIZE(string, turnCount, TurnCount); //軍師の効力継続ターン数
    
    CC_SYNTHESIZE(string, activeSkillId, ActiveSkillId); //アクティブスキルID
    CC_SYNTHESIZE(string, passiveSkillId, PassiveSkillId); //パッシブスキルID
    
    //アクティブスキルデータ
    CC_SYNTHESIZE(string, activeSkillName, ActiveSkillName); //アクティブスキルの名前
    CC_SYNTHESIZE(string, activeSkillNeedCost, ActiveSkillNeedCost); //アクティブスキルの発動士気
    
    //パッシブスキルデータ
    CC_SYNTHESIZE(string, passiveSkillName, PassiveSkillName); //パッシブスキルの名前
    
    CC_SYNTHESIZE(string, price, Price); //必要交換チケット枚数
    
    //交換状況
    CC_SYNTHESIZE(string, exhibitStatus, ExhibitStatus); //交換状況ステータス
    
    bool movingFlg; //動かしたかフラグ
    bool shortagePriceFlg; //必要交換チケット枚数を満たしているかフラグ
    bool sreachFlg;
    
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
        T_Purchase_MenuItem,
        T_Purchase_Menu,
        T_Price,
        T_Exhibit_Menu,
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
        Z_Purchase_MenuItem,
        Z_Purchase_Menu,
        Z_Price,
        Z_Exhibit_Menu,
        Z_End_Date_Label,
        Z_Node,
    };
    
    Node* node; //キャラ交換レイヤーのポインタを保持しておく
    Json charaData; //キャラデータ
    
    BazaarCharaSprite();

    
    static BazaarCharaSprite* create(json11::Json json, Node* normal_scene, bool sreachFlg);
    virtual bool init(json11::Json json, Node* normal_scene, bool sreachFlg);
    
    float ratio;
    
    void initEventListener(); //イベントリスナーの設定
    string getBackSpritePath(json11::Json json); //背景画像のパスを調べて返す
    void setupData(json11::Json json); //データの設定
    void generateCharaSprite(); //キャラ画像の生成
    void createNameLabel(); //キャラ名の作成
    void createCharaTypeIcon(); //キャラタイプのアイコンの作成
    void showPower(); //戦闘力の表示
    void showNeedCost(); //配置士気の表示
    
    void showPriceLabel(); //必要交換チケット枚数
    void createButton(); //キャラ詳細と交換ボタンの作成
    void createOnButton(); //リストの上に置くボタンの作成
    
    void createDisableCascadeNode(); //ノードの作成
    
    void setUnselectedPurchaseButton(); //交換チケットが足りないときに、ボタンを押せなくする処理
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};


#endif /* defined(__war__BazaarCharaSprite__) */
