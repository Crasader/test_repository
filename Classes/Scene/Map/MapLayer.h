#ifndef __war__MapLayer__
#define __war__MapLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "HMSVector.h"
#include "HexLayer.h"
#include "MapCharaLayer.h"
#include "MapTreasureLayer.h"

#define MAP_CHARA_TYPE_PLAYER 0
#define MAP_CHARA_TYPE_ENEMY 1

USING_NS_CC;
using namespace std;

class MapLayer : public Layer
{
private:
    float ratio;
    float _contact_distance;
    float _camp_distance;
    HmsVector<MapCharaLayer *> _chara_list[2];
    HmsVector<Sprite *> _camp_list;
    HmsVector<MapTreasureLayer *> _treasure_list;
public:
    virtual bool init();
    CREATE_FUNC(MapLayer);
    
    void initialize(float contact_distance, float camp_distance);
    HmsVector<MapCharaLayer *> getCharaList(int type);

    //******************************マップ関連******************************
    //タップされた座標から、マップ上の座標を取得する
    Point getPoint(Point tap_point);
    
    //指定の座標に軍営地を置く
    void addCamp(Point position, bool effect_flg);
    
    //指定のキャラが軍営地の範囲に入っているかどうか
    Sprite *getCampNearBy(MapCharaLayer *chara);
    
    //指定の座標に宝箱を置く
    void addTreasure(Point position, string treasure_id, string type, bool open_flg);
    
    //指定のキャラの近くにいるお宝を取得する
    MapTreasureLayer *getTreasureNearBy(MapCharaLayer *chara);
    
    //指定のお宝を開ける
    void openTreasure(string treasure_id);
    //******************************キャラ操作関連******************************
    //指定の座標にキャラを置く
    void addChara(Point position, int chara_type, int user_master_id, string user_name, int level, int chara_id, string deck_chara_id_list, string comment, int hp, int max_hp, bool camp_flg, bool effect_flg);
    
    //指定のキャラの近くにいる敵を取得する
    MapCharaLayer *getCharaNearBy(MapCharaLayer *chara);
    
    //指定のキャラの移動範囲内のキャラのうち戦闘中のキャラを取得する
    HmsVector<MapCharaLayer *> getCharaListNearBy(MapCharaLayer *chara, float movable_distance);
    
    //タイプとユーザIDを指定してキャラを取得する
    MapCharaLayer *getChara(int chara_type, int user_master_id);
    
    //指定のキャラを削除する
    void removeChara(int chara_type, int user_master_id);
    
    //******************************演出関連******************************
    void showEffect(Point position, string filename, bool auto_remove_flg);

    void addBattleAnimation(MapCharaLayer *chara, MapCharaLayer *chara_enemy);
    void removeBattleAnimation(MapCharaLayer *chara, int enemy_user_master_id);
    
    void addMovableArea(Point target_point, int user_master_id, float movable_distance);
    void removeMovableArea(int user_master_id);
};

#endif /* defined(__war__MapLayer__) */
