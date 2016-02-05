#include "MapTreasureLayer.h"
#include "SimpleAudioEngine.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsFontUtil.h"
#include "MapLayer.h"
#include "MapScene.h"
#include "SaveData.h"
#include "PlistAnimationSprite.h"

#define MAP_TREASURE_IMG_TAG 1

#define MAP_TREASURE_IMG_Z 1

bool MapTreasureLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    return true;
}

void MapTreasureLayer::initialize(Point position, string treasure_id, string type, bool open_flg)
{
    ratio = getVisibleRatio();
    
    _treasure_id = treasure_id;
    _type = type;
    _open_flg = open_flg;
    
    if(open_flg==false) {
        auto loading_sprite = PlistAnimationSprite::setData(this, "download/Map/treasure_box1", 0, 0.06f, MAP_TREASURE_IMG_Z);
        loading_sprite->setPosition(Point(0, 0));
        loading_sprite->setScale(ratio);
        loading_sprite->setTag(MAP_TREASURE_IMG_TAG);
    }
}

void MapTreasureLayer::open()
{
    _open_flg = true;
    
    this->removeChildByTag(MAP_TREASURE_IMG_TAG);
    /*
    string filename = "init/Map/treasure_box1_2.png";
    Sprite *img = Sprite::create(filename.c_str());
    img->setScale(ratio*1.5);
    img->setPosition(Point(0, 30*ratio));
    this->addChild(img, MAP_TREASURE_IMG_Z, MAP_TREASURE_IMG_TAG);
     */
    auto loading_sprite = PlistAnimationSprite::setData(this, "download/Map/treasure_box_open1", 1, 0.06f, MAP_TREASURE_IMG_Z);
    loading_sprite->setPosition(Point(0, 0));
    loading_sprite->setScale(ratio);
    loading_sprite->setTag(MAP_TREASURE_IMG_TAG);
}
