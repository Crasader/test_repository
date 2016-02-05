#ifndef __war__MapCharaLayer__
#define __war__MapCharaLayer__

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class MapCharaLayer : public Layer
{
private:
    float ratio;

    CC_SYNTHESIZE(int, _position, Point);//xy座標
    CC_SYNTHESIZE(int, _chara_type, CharaType);//タイプ、味方0、敵1
    CC_SYNTHESIZE(int, _chara_id, CharaId);//キャラID
    CC_SYNTHESIZE(string, _chara_name, CharaName);//キャラ名
    CC_SYNTHESIZE(int, _user_master_id, UserMasterId);//ユーザID
    CC_SYNTHESIZE(string, _user_name, UserName);//ユーザ名
    CC_SYNTHESIZE(string, _deck_chara_id_list, DeckCharaIdList);//デッキchara_idリスト
    CC_SYNTHESIZE(string, _comment, Comment);//コメント
    CC_SYNTHESIZE(int, _hp, Hp);//HP
    CC_SYNTHESIZE(bool, _camp_flg, CampFlg);//軍営地にいるかどうか
    CC_SYNTHESIZE(bool, _battle_flg, BattleFlg);//バトル中かどうか
    int _max_hp;
    
    //移動関連
    Point _target_position;
    float _move_speed;
    bool _effect_only_flg;
    int _direction = 4;
    int _img_no = 1;//1or2
public:
    virtual bool init();
    CREATE_FUNC(MapCharaLayer);
    
    void initialize(Point position, int chara_type, int user_master_id, string user_name, int level, int chara_id, string deck_chara_id_list, string comment, int hp, int max_hp, bool camp_flg);
    
    //表示関連
    void displayCharaMessage(string message);
    void displayCharaStamp(int stamp_id);
    void displayHpGauge(int heal_value);
    void displayDamage(int damage);
    void showEffect(Point position, string filename, bool auto_remove_flg, int tag_id);
    void campIn();
    void heal();

    //移動関連
    void moveStart(Point target_position, float move_speed, bool effect_only_flg);
    void move();
    void startMoveAnimation();
    void stopMoveAnimation();
    void changeSoldierImage(float dt);
};

#endif /* defined(__war__MapCharaLayer__) */
