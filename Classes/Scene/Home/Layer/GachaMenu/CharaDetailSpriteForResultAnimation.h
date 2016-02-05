#ifndef __war__CharaDetailSpriteForResultAnimation__
#define __war__CharaDetailSpriteForResultAnimation__

#include "cocos2d.h"
#include "NormalScene.h"
#include "common.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "GachaMenuLayer.h"
#include "GachaResultLayer.h"

USING_NS_CC;

#define HTTP_SKILL_DATA_URL "chara/get_skill_data.php" //スキルデータ取得PHP

class CharaDetailSpriteForResultAnimation : public Sprite
{
private:
    enum Tag{
        T_Second_name = 30,
    };
    
    int _chara_list_id;//create時の引数受け取り
    EventListenerTouchOneByOne *listener;

//////////////////////ステータス///////////////////////
    string chara_id;                //該当キャラのID
    string chara_no;                //キャラNo
    string second_name;             //二つ名
    string chara_name;              //キャラ名
    string name_kana;               //キャラ名のフリガナ
    string rare;
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
    
    Label* setupScrollLabel(Label* scroll_label, string text, Point pos) ;
    void setScrollLavelAction(Label* text_label);
    float _clipping_masc_width ;
    
    string _skill_type = "active_skill";//アクティブ、パッシブ両スキル持ちの場合、どちらを先に表示するか＝アクティブから先に表示。
    bool _switch_skill_button_flg = false;
public:
    static CharaDetailSpriteForResultAnimation* create(int chara_list_id);
    virtual bool init(int chara_list_id);

    void setData();//キャラクター情報のセット
    void setupImage();//画像の設置
    void setupLabel();//文字の設置
    
    void touchButtun(Ref *sender);//タップしたポイントのタグを調べて返す。
    void switchSkill();//アクティブ、パッシブスキルの表示を切り替える
    void setSwitchSkillButtonFlg(bool switch_skill_button_flg);
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    
    void onEnter();//初期実行関数
};

#endif /* defined(__war__CharaDetailSpriteForResultAnimation__) */