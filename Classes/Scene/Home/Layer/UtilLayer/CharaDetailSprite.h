#ifndef __war__CharaDetailSprite__
#define __war__CharaDetailSprite__

/*
 このスプライトはCharaDetailPopupLayerを介して使用して下さい
 */

#include "cocos2d.h"
#include "common.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "SimpleAudioEngine.h"
#include "HmsGeometricFunction.h"
#include "cocos-ext.h"
#include "HmsTemplateFunction.h"
#include "HmsUtilityFunction.h"
#include "SaveData.h"

#include "network/HttpClient.h"
#include "json11.hpp"

USING_NS_CC;

#define HTTP_SKILL_DATA_URL "chara/get_skill_data.php" //スキルデータ取得PHP
#define SWITCH_SKILL_BUTTON_TAG 13

using namespace std;
using namespace json11;
using namespace CocosDenshion;

class CharaDetailSprite : public Sprite
{
private:
    
    enum Tag{
        T_Second_name = 30,
    };
    
    int _chara_list_id;//create時の引数受け取り
    EventListenerTouchOneByOne *listener;
    
    string json_text_as; //アクティブスキル情報一覧
    string json_text_ps; //パッシブスキル情報一覧

//////////////////////ステータス///////////////////////
    string chara_id;                //該当キャラのID
    string chara_no;                //キャラNo
    string second_name;             //二つ名
    string chara_name;              //キャラ名
    string name_kana;               //キャラ名のフリガナ
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
    string skill_icon_detail;       //スキルアイコン(表示用
///////////////////////////////////////////////////////////
    
    Label* setupScrollLabel(Label* scroll_label, string text, Point pos) ;
    void setScrollLavelAction(Label* text_label) ;
    float _clipping_masc_width ;
    
    
    //窓を閉じるときに呼び出し元から登録されたメソッドを呼び出すために保持する変数
    Layer *_parent;
    SEL_SCHEDULE _sel;
    
    //登録されたメソッドを呼び出す
    void callback() ;
    //ロックしているかどうか情報を格納
    bool _lock_flg ;
    //最初のロック情報を取得
    bool _default_lock_flg;
    
    bool _hide_lock_button ;
    
    json11::Json _json_chara;
    json11::Json _json_active_skill;
    json11::Json _json_passive_skill;
    
    bool _right_flg = false;
    bool _left_flg = false;
    bool _skill_set_flg = false;
    float _delay_time = 0.0f;
    
    string _skill_type = "active_skill";//アクティブ、パッシブ両スキル持ちの場合、どちらを先に表示するか＝アクティブから先に表示。
    bool _switch_skill_button_flg = false;
public:
    int diff = 0;
    
    //ロックしているかどうか
    bool getLockFlg() ;
    //ロック情報がウィンドウを開いたときから変更されているか
    bool isLockStateChanged() ;
    
    
    //窓を閉じるときに呼び出すメソッドを登録する
    void setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel) ;
    
    string ind;
    static CharaDetailSprite* create(int chara_list_id, bool hide_lock_button);
    virtual bool init(int chara_list_id, bool hide_lock_button);
    
    void setLeftRightFlg(bool left_flg, bool right_flg, float delay_time);
    void setSkillInfo(json11::Json json_chara, string skill_name, string skill_detail, string skill_icon_detail, string need_cost);
    void setData();//キャラクター情報のセット
    void setupImage();//画像の設置
    void setupLabel();//文字の設置
    bool getLockData(int chara_list_id);//ロック情報の取得
    void setLockData(int chara_list_id);//ロック状態にする
    void removeLockData(int chara_list_id);//ロック状態の解除
    void touchButtun(Ref *sender);//タップしたポイントのタグを調べて返す。
    void deleteAction(Ref *sender);//スプライトを消す
    void switchSkill();//アクティブ、パッシブスキルの表示を切り替える
    void setSwitchSkillButtonFlg(bool switch_skill_button_flg);
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    
    void onEnter();//初期実行関数
};

#endif /* defined(__war__CharaDetailSprite__) */