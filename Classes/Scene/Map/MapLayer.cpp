#include "MapLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "MapScene.h"
#include "MapCharaLayer.h"
#include "MapTreasureLayer.h"
#include "SaveData.h"
#include "PlistAnimationSprite.h"

#define MAP_MOVABLE_AREA_TAG_ID 1000
#define MAP_CHARA_TAG_ID 2000
#define MAP_BATTLE_ANIMATION_TAG_ID 3000

#define MAP_MOVABLE_AREA_Z 1000
#define MAP_CHARA_Z 10000
#define MAP_BATTLE_ANIMATION_Z 90000
#define MAP_EFFECT_Z 100000

bool MapLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    return true;
}
void MapLayer::initialize(float contact_distance, float camp_distance){
    ratio = getVisibleRatio();
    
    _contact_distance = contact_distance;
    _camp_distance = camp_distance;
    
    MapScene *map_scene = (MapScene *)this->getParent();
    
    //0,0の地点にマップを置く
    Sprite *img = Sprite::create(map_scene->getMapFilename());
    img->setScale(ratio);
    img->setAnchorPoint(Point(0, 0));
    this->addChild(img);
}

HmsVector<MapCharaLayer *> MapLayer::getCharaList(int type)
{
    return _chara_list[type];
}

//******************************マップ関連******************************

//タップされた座標から、マップ上の座標を取得する
Point MapLayer::getPoint(Point tap_point)
{
    Point map_position = this->getPosition();
    float map_scale = this->getScale();
    Point target_position = Point((tap_point.x - map_position.x)/map_scale, (tap_point.y - map_position.y)/map_scale);
    
    return target_position;
}

//指定の座標に軍営地を置く
void MapLayer::addCamp(Point position, bool effect_flg)
{
    //イフェクト
    if(effect_flg==true) {
        showEffect(Point(position.x*ratio, position.y*ratio), "download/Effect/Particle/map/add_camp.plist", true);
        //マップに出現 SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/map_appear.mp3");
    }
    
    Sprite *img = Sprite::create("init/Map/camp.png");
    img->setScale(ratio);
    img->setPosition(Point(position.x*ratio, position.y*ratio));
    this->addChild(img, 1);
    
    _camp_list.insert(img);
}

//指定のキャラが軍営地の範囲に入っているかどうか
Sprite *MapLayer::getCampNearBy(MapCharaLayer *chara)
{
    Point p1 = chara->getPosition();
    
    for(int i=0; i<_camp_list.size(); i++) {
        Sprite *camp = _camp_list.at(i);
        Point p2 = camp->getPosition();
        float distance = calculateDistanceAtHexMap(p1, p2);
        if(distance<_camp_distance) {
            HMSLOG("in camp");
            return camp;
        }
    }
    return nullptr;
}

//指定の座標に宝箱を置く
void MapLayer::addTreasure(Point position, string treasure_id, string type, bool open_flg)
{
    /*
    Sprite *img = Sprite::create("init/Map/treasure_box1_1.png");
    img->setScale(ratio*1.5);
    img->setPosition(Point(position.x*ratio, position.y*ratio));
    this->addChild(img, 1);
    */
    int z_order = MAP_CHARA_Z - position.y*ratio;
    MapTreasureLayer *treasure = MapTreasureLayer::create();
    this->addChild(treasure, z_order);
    treasure->initialize(position, treasure_id, type, open_flg);
    treasure->setPosition(Point(position.x*ratio, position.y*ratio));
    
    _treasure_list.insert(treasure);
}

//指定のキャラの近くにいるお宝を取得する
MapTreasureLayer *MapLayer::getTreasureNearBy(MapCharaLayer *chara)
{
    Point p1 = chara->getPosition();
    for(int j=0; j<_treasure_list.size(); j++) {
        MapTreasureLayer *treasure = _treasure_list[j];
        Point p2 = treasure->getPosition();
        float distance = calculateDistanceAtHexMap(p1, p2);
        if(distance<_contact_distance*ratio) {
            return treasure;
        }
    }
    return nullptr;
}

//指定のお宝を開ける
void MapLayer::openTreasure(string treasure_id)
{
    for(int j=0; j<_treasure_list.size(); j++) {
        MapTreasureLayer *treasure = _treasure_list[j];
        if(treasure->getTreasureId()==treasure_id) {
            treasure->open();
            return;
        }
    }
}

//******************************キャラ操作関連******************************
//指定の座標にキャラを置く
void MapLayer::addChara(Point position, int chara_type, int user_master_id, string user_name, int level, int chara_id, string deck_chara_id_list, string comment, int hp, int max_hp, bool camp_flg, bool effect_flg)
{
    for(int i=0; i<_chara_list[chara_type].size(); i++) {
        MapCharaLayer *chara = _chara_list[chara_type].at(i);
        if(chara->getUserMasterId()==user_master_id) {
            HMSLOG("duplicate");
            return;
        }
    }
    
    //y座標に応じてz座標を決める
    int z_order = MAP_CHARA_Z - position.y*ratio;
    
    //イフェクト
    if(effect_flg==true) {
        showEffect(Point(position.x*ratio, position.y*ratio), "download/Effect/Particle/map/add_chara.plist", true);
        //マップに出現 SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/map_appear.mp3");
    }
    
    //キャラ配置
    MapCharaLayer *chara = MapCharaLayer::create();
    this->addChild(chara, z_order, MAP_CHARA_TAG_ID);
    chara->initialize(position, chara_type, user_master_id, user_name, level, chara_id, deck_chara_id_list, comment, hp, max_hp, camp_flg);
    chara->setPosition(Point(position.x*ratio, position.y*ratio));
    
    _chara_list[chara_type].insert(chara);
}

//指定のキャラの近くいる敵を取得する
MapCharaLayer *MapLayer::getCharaNearBy(MapCharaLayer *chara)
{
    Point p1 = chara->getPosition();
    for(int j=0; j<_chara_list[MAP_CHARA_TYPE_ENEMY].size(); j++) {
        MapCharaLayer *chara_ = _chara_list[MAP_CHARA_TYPE_ENEMY][j];
        Point p2 = chara_->getPosition();
        float distance = calculateDistanceAtHexMap(p1, p2);
        if(distance<_contact_distance*ratio) {
            return chara_;
        }
    }
    return nullptr;
}

//指定のキャラの移動範囲内のキャラのうち戦闘中のキャラを取得する
HmsVector<MapCharaLayer *> MapLayer::getCharaListNearBy(MapCharaLayer *chara, float movable_distance)
{
    HmsVector<MapCharaLayer *> chara_list;
    
    Point p1 = chara->getPosition();
    for(int j=0; j<_chara_list[MAP_CHARA_TYPE_PLAYER].size(); j++) {
        MapCharaLayer *chara_ = _chara_list[MAP_CHARA_TYPE_PLAYER][j];
        if(chara==chara_||chara_->getBattleFlg()==false) continue;
        Point p2 = chara_->getPosition();
        float distance = calculateDistanceAtHexMap(p1, p2);
        if(distance<movable_distance *ratio) {
            chara_list.insert(chara_);
        }
    }
    return chara_list;

}

//タイプとユーザIDを指定してキャラを取得する
MapCharaLayer *MapLayer::getChara(int chara_type, int user_master_id)
{
    for(int i=0; i<_chara_list[chara_type].size(); i++) {
        MapCharaLayer *chara_ = _chara_list[chara_type][i];
        if(chara_->getUserMasterId()==user_master_id) return chara_;
    }
    return nullptr;
}

//指定のキャラを削除する
void MapLayer::removeChara(int chara_type, int user_master_id)
{
    HMSLOG("removeChara. chara_type=%d, user_master_id=%d", chara_type, user_master_id);
    for(int i=0; i<_chara_list[chara_type].size(); i++) {
        MapCharaLayer *chara = _chara_list[chara_type].at(i);
        if(chara->getUserMasterId()==user_master_id) {
            HMSLOG("remove ok");
            
            //イフェクト
            showEffect(chara->getPosition(), "download/Effect/Particle/map/remove_chara.plist", true);

            //リストから削除
            _chara_list[chara_type].remove(i);
            
            //removeChildして開放
            this->removeChild(chara);
            return;
        }
    }
}

//******************************演出関連******************************
void MapLayer::showEffect(Point position, string filename, bool auto_remove_flg)
{
    //イフェクト
    ParticleSystemQuad* particle = ParticleSystemQuad::create(filename);
    particle->setAutoRemoveOnFinish(auto_remove_flg);
    particle->setPositionType(ParticleSystem::PositionType::GROUPED);
    particle->setPosition(position);
    particle->setScale(ratio);
    this->addChild(particle, MAP_EFFECT_Z);
}

void MapLayer::addBattleAnimation(MapCharaLayer *chara, MapCharaLayer *chara_enemy)
{
    Point p1 = chara->getPosition();
    Point p2 = chara_enemy->getPosition();
    Point p = Point((p1.x+p2.x)/2, (p1.y+p2.y)/2+40);

    int y = p1.y;
    if(p2.y<p1.y) y = p2.y;
    int z_order = MAP_CHARA_Z - y;
    
    auto loading_sprite = PlistAnimationSprite::setData(this, "init/Map/battle", 0, 0.06f, z_order);
    loading_sprite->setPosition(p);
    loading_sprite->setScale(ratio);
    loading_sprite->setTag(MAP_BATTLE_ANIMATION_TAG_ID+chara_enemy->getUserMasterId());
    
    chara->setBattleFlg(true);
}

void MapLayer::removeBattleAnimation(MapCharaLayer *chara, int enemy_user_master_id)
{
    while(true){
        if(this->getChildByTag(MAP_BATTLE_ANIMATION_TAG_ID+enemy_user_master_id)!=nullptr) {
            this->removeChildByTag(MAP_BATTLE_ANIMATION_TAG_ID+enemy_user_master_id);
        } else {
            break;
        }
    }
    
    chara->setBattleFlg(false);
}

void MapLayer::addMovableArea(Point target_point,int user_master_id, float movable_distance)
{
    Sprite *img = Sprite::create("init/Map/area_green.png");
    img->setScale(ratio*movable_distance*2.0f/640.0f);
    img->setPosition(target_point);
    this->addChild(img, MAP_MOVABLE_AREA_Z, MAP_MOVABLE_AREA_TAG_ID);
    
    //アニメーション
    img->setScale(0);
    ScaleTo *scale = ScaleTo::create(0.3f, ratio*movable_distance*2.0f/640.0f);
    img->runAction(scale);
    
    //キャンプのエリアも表示する
    for(int i=0; i<_camp_list.size(); i++) {
        Sprite *camp = _camp_list.at(i);
        img = Sprite::create("init/Map/area_blue.png");
        img->setScale(ratio*_camp_distance*2.0f/640.0f);
        img->setPosition(camp->getPosition());
        this->addChild(img, MAP_MOVABLE_AREA_Z+1, MAP_MOVABLE_AREA_TAG_ID);
    }
}

void MapLayer::removeMovableArea(int user_master_id)
{
    while(true){
        if(this->getChildByTag(MAP_MOVABLE_AREA_TAG_ID)!=nullptr) {
            this->removeChildByTag(MAP_MOVABLE_AREA_TAG_ID);
        } else {
            break;
        }
    }
}
