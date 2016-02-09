#ifndef __war__ReplayListSprite__
#define __war__ReplayListSprite__

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "json11.hpp"

USING_NS_CC;

class ReplayListSprite : public Sprite
{
private:
    
    CC_SYNTHESIZE(string, _battle_list_id, BattleListId); //バトルリストID
    CC_SYNTHESIZE(string, _enemy_user_master_id, EnemyUserMasterId); //対戦相手ユーザーID
    CC_SYNTHESIZE(string, _enemy_chara_id, EnemyCharaId); //対戦相手のキャラID
    CC_SYNTHESIZE(string, _enemy_user_name, EnemyUserName); //対戦相手の名前
    CC_SYNTHESIZE(bool, _result_flg, ResultFlg); //リザルトフラグ
    
public:
    
    //タグ
    enum Tag{
        T_Enemy_Sprite = 1,
        T_Enemy_Name,
        T_Result_Sprite,
        T_Replay_MenuItem,
        T_Replay_Menu,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Enemy_Sprite = 1,
        Z_Enemy_Name,
        Z_Result_Sprite,
        Z_Replay_MenuItem,
        Z_Replay_Menu,
    };
    
    Node* _normal_scene; //ノーマルシーンのポインタを保持しておく
    
    ReplayListSprite(); //コンストラクタ
    
    static ReplayListSprite* create(json11::Json json, Node* normal_scene);
    virtual bool init(json11::Json json, Node* normal_scene);
    
    float ratio;
    void setupData(json11::Json json); //データの設定
    
    void showEnemySprite();
    void showEnemyLabel();
    void showResultSprite();
    void createReplaySprite();
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__ReplayListSprite__) */
