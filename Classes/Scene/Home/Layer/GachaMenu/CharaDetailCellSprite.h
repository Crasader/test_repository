#ifndef __war__CharaDetailCellSprite__
#define __war__CharaDetailCellSprite__

#include "cocos2d.h"
#include "NormalScene.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"

USING_NS_CC;

class CharaDetailCellSprite : public Sprite
{
private:
    
    enum Tag{
        T_Second_Name = 30,
    };
    
    int _chara_list_id;//create時の引数受け取り
    EventListenerTouchOneByOne *listener;

//////////////////////ステータス///////////////////////
    string chara_id;                //該当キャラのID
    string chara_no;                //キャラNo
    string chara_name;              //キャラ名
    string second_name;             //キャラ名(二つ名)
    string name_kana;               //キャラの仮名
    string rare;                    //レア度
    string chara_power;             //該当キャラの戦闘力
    string _chara_type;             //キャラタイプ
    string need_cost;               //配置コスト
    string hp;                      //体力
    string add_cost;                //上昇士気
    string turn_count;              //継続ターン
    string chara_comment;           //キャラコメント
    string active_skill_id;         //アクティブスキルID
    string active_skill_name;       //アクティブスキル名
    string active_skill_detail;     //アクティブスキル詳細
    string active_skill_icon_detail;//アクティブスキルアイコン
    string active_skill_need_cost;  //アクティブスキル必要コスト
    string passive_skill_id;        //パッシブスキルID
    string passive_skill_name;      //パッシブスキル名
    string passive_skill_detail;    //パッシブスキル詳細
    string passive_skill_icon_detail;//パッシブスキルアイコン
    string skill_name;              //スキル名(表示用)
    string skill_detail;            //スキル詳細(表示用)
    string skill_icon_detail;       //スキルアイコン(表示用)
///////////////////////////////////////////////////////////
    
public:
    string ind;
    static CharaDetailCellSprite* create(int chara_list_id);
    virtual bool init(int chara_list_id);

    void setData();//キャラクター情報のセット
    void setupImage();//画像の設置
    void setupLabel();//文字の設置
    bool getLockData(int chara_list_id);//ロック情報の取得
    void setLockData(int chara_list_id);//ロック状態にする
    void removeLockData(int chara_list_id);//ロック状態の解除
    void touchButtun(Ref *sender);//タップしたポイントのタグを調べて返す。
    void deleteAction(Ref *sender);//スプライトを消す
    
    void onEnter();//初期実行関数
};

#endif /* defined(__war__CharaDetailCellSprite__) */