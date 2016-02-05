#include "MapCharaLayer.h"
#include "SimpleAudioEngine.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsFontUtil.h"
#include "MapLayer.h"
#include "MapScene.h"
#include "SaveData.h"

#define MAP_CHARA_MESSAGE_LABEL_TAG 101
#define MAP_CHARA_HP_TAG 102
#define MAP_CHARA_HP_GAUGE_TAG 103
#define MAP_CHARA_HP_GAUGE_BK_TAG 104
#define MAP_CHARA_SOLDIER_TAG 105
#define MAP_CHARA_CAMP_EFFECT_TAG 106
#define MAP_CHARA_HEAL_EFFECT_TAG 107

#define MAP_CHARA_SOLDIER_Z 0
#define MAP_CHARA_LIGHT_Z 5
#define MAP_CHARA_IMG_Z 10
#define MAP_CHARA_FRAME_Z 20
#define MAP_CHARA_LABEL_Z 30
#define MAP_CHARA_HP_GAUGE_Z 50
#define MAP_CHARA_CAMP_EFFECT_Z 100
#define MAP_CHARA_DAMAGE_Z 100
#define MAP_CHARA_STAMP_Z 200

#define MAP_CHARA_Z 10000

bool MapCharaLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    return true;
}

void MapCharaLayer::initialize(Point position, int chara_type, int user_master_id, string user_name, int level, int chara_id, string deck_chara_id_list, string comment, int hp, int max_hp, bool camp_flg) {
    ratio = getVisibleRatio();
    
    _chara_type = chara_type;
    _user_master_id = user_master_id;
    _user_name = user_name;
    _chara_id = chara_id;
    _deck_chara_id_list = deck_chara_id_list;
    _comment = comment;
    _max_hp = max_hp;
    _hp = hp;
    _camp_flg = camp_flg;
    _battle_flg = false;
    
    //キャラ画像表示
    string filename = "download/Chara/thumb100/" + to_string(chara_id) + ".png";
    Sprite *img = Sprite::create(filename.c_str());
    img->setScale(0.7*ratio);
    img->setPosition(Point(0, 35*ratio));
    this->addChild(img, MAP_CHARA_IMG_Z);
    
    //枠画像表示
    filename = "init/Map/square_card_blue.png";
    if(chara_type==MAP_CHARA_TYPE_ENEMY) {
        filename = "init/Map/square_card_red.png";
    }
    if(to_string(user_master_id)==SaveData::loadString("user_master_id")) {
        filename = "init/Map/square_card_green.png";
    }
    img = Sprite::create(filename.c_str());
    img->setScale(0.7*ratio);
    img->setPosition(Point(0, 35*ratio));
    this->addChild(img, MAP_CHARA_FRAME_Z);
    
    if(chara_type==MAP_CHARA_TYPE_PLAYER) {
        //プレイヤー名表示
        Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(user_name, FONT_NAME_SYSTEM, 22*ratio);
        label->setAlignment(TextHAlignment::CENTER);
        label->setPosition(Point(0, 84*ratio));
        label->enableOutline(Color4B::BLACK, 4);
        this->addChild(label, MAP_CHARA_LABEL_Z);
        
        //レベル表示
        label = HMSFontUtil::createWithTTFBySystemFontArgument("Lv" + to_string(level), FONT_NAME_SYSTEM, 22*ratio);
        label->setAlignment(TextHAlignment::CENTER);
        label->setPosition(Point(0, -2*ratio));
        label->enableOutline(Color4B::BLACK, 4);
        this->addChild(label, MAP_CHARA_LABEL_Z);
        
        //自分用矢印表示
        if(to_string(user_master_id)==SaveData::loadString("user_master_id")) {
            int y = 100;
            filename = "init/Map/arrow.png";
            img = Sprite::create(filename.c_str());
            img->setScale(0.5*ratio);
            img->setPosition(Point(0, y*ratio));
            this->addChild(img, MAP_CHARA_LABEL_Z);
            
            filename = "init/Map/square_card_light.png";
            Sprite *img2 = Sprite::create(filename.c_str());
            img2->setScale(0.7*ratio);
            img2->setPosition(Point(0, 35*ratio));
            this->addChild(img2, MAP_CHARA_LIGHT_Z);
            
            float duration = 0.6f;
            
            MoveTo *move1 = MoveTo::create(duration, Point(0, (y+13)*ratio));
            MoveTo *move2 = MoveTo::create(duration, Point(0, y*ratio));
            RepeatForever *action1 = RepeatForever::create(Sequence::create(move1, move2, nullptr));
            img->runAction(action1);
            
            FadeOut *fade_out = FadeOut::create(duration);
            FadeIn *fade_in = FadeIn::create(duration);
            ScaleTo *scale1 = ScaleTo::create(duration, 0.7*ratio*1.2);
            ScaleTo *scale2 = ScaleTo::create(duration, 0.7*ratio);
            Spawn *spawn1 = Spawn::create(fade_out, scale1, nullptr);
            Spawn *spawn2 = Spawn::create(fade_in, scale2, nullptr);
            RepeatForever *action2 = RepeatForever::create(Sequence::create(spawn1, spawn2, nullptr));
            img2->runAction(action2);
        }
    }
    
    if(chara_type==MAP_CHARA_TYPE_ENEMY) {
        //キャラ名表示
        filename = "download/Chara/name/" + to_string(chara_id) + ".png";
        img = Sprite::create(filename.c_str());
        img->setScale(0.5*ratio);
        img->setPosition(Point(0, -2*ratio));
        this->addChild(img, MAP_CHARA_LABEL_Z);
        
        //キャンプ画像表示
        if(camp_flg==true) {
            HMSLOG("camp");
            filename = "init/Map/icon_flag.png";
            img = Sprite::create(filename.c_str());
            img->setScale(0.8*ratio);
            img->setPosition(Point(25*ratio, 60*ratio));
            this->addChild(img, MAP_CHARA_LABEL_Z);
        }
    }
    
    //兵画像表示
    string name = "soldier";
    if(_chara_type==MAP_CHARA_TYPE_ENEMY) name = "enemy";
    filename = "init/Map/soldier/" + name + "_" + to_string(_direction) + "_1.png";
    img = Sprite::create(filename.c_str());
    img->setScale(ratio);
    img->setPosition(Point(0*ratio, 0*ratio));
    this->addChild(img, MAP_CHARA_SOLDIER_Z, MAP_CHARA_SOLDIER_TAG);
    
    displayHpGauge(0);
}

void MapCharaLayer::displayCharaMessage(string message)
{
    if(this->getChildByTag(MAP_CHARA_MESSAGE_LABEL_TAG)!=nullptr) {
        this->removeChildByTag(MAP_CHARA_MESSAGE_LABEL_TAG);
    }
    
    if(message!="") {
        //メッセージ表示
        Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(message, FONT_NAME_SYSTEM, 24*ratio);
        label->setAlignment(TextHAlignment::CENTER);
        label->setPosition(Point(0, 110*ratio));
        label->enableOutline(Color4B::RED, 4);
        this->addChild(label, 50, MAP_CHARA_MESSAGE_LABEL_TAG);
    }
}

void MapCharaLayer::displayCharaStamp(int stamp_id)
{
    if(SaveData::loadInt("map_stamp_mute") == 1) return; //ミュートの時

    MapLayer *map_layer = (MapLayer *)this->getParent();
    float map_scale = map_layer->getScale();
    float stamp_ratio = ratio / map_scale;

    Sprite *stamp = Sprite::create("download/Map/stamp" + to_string(stamp_id) +".png");
    stamp->setScale(stamp_ratio);
    stamp->setAnchorPoint(Point(1, 0));
    stamp->setPosition(Point(0, 60));
    this->addChild(stamp, MAP_CHARA_STAMP_Z);
    
    auto fadein = FadeIn::create(0.1f);
    auto scale = ScaleTo::create(0.1f, stamp_ratio*1.15f);
    auto ease1 = EaseBackOut::create(scale);
    auto scale2 = ScaleTo::create(0.15f, stamp_ratio);
    auto ease2 = EaseSineInOut::create(scale2);
    
    auto delay = DelayTime::create(2.5f);
    auto fadeout = FadeOut::create(1.2f);
    stamp->runAction(Sequence::create(fadein,ease1,ease2,delay,fadeout,RemoveSelf::create(), NULL));
}

void MapCharaLayer::displayHpGauge(int heal_value)
{
    if(_chara_type==MAP_CHARA_TYPE_PLAYER) {
        float percentage = (float)_hp / (float)_max_hp;
        float base_ratio = ratio * 0.7f;
        
        int bk_x = -42;
        int bk_y = 98;
        
        if(this->getChildByTag(MAP_CHARA_HP_GAUGE_BK_TAG)==nullptr) {
            Sprite *bk = Sprite::create("init/Map/gauge_bk.png");
            bk->setScale(base_ratio);
            bk->setAnchorPoint(Point(0, 0));
            bk->setPosition(Point(bk_x*ratio, bk_y*ratio));
            this->addChild(bk, 40, MAP_CHARA_HP_GAUGE_BK_TAG);
        }
        
        string color = "green";
        if(percentage<0.4) color = "yellow";
        if(percentage<0.2) color = "red";
     
        string filename = "init/Map/gauge_" + color + ".png";
        Sprite *gauge = Sprite::create(filename.c_str());
        gauge->setScale(base_ratio);
        gauge->setScaleX(base_ratio * percentage);
        gauge->setAnchorPoint(Point(0, 0));
        gauge->setPosition(Point((bk_x+12*0.7f)*ratio, (bk_y+11*0.7f)*ratio));
        this->removeChildByTag(MAP_CHARA_HP_GAUGE_TAG);
        this->addChild(gauge, MAP_CHARA_HP_GAUGE_Z, MAP_CHARA_HP_GAUGE_TAG);
        
        //回復したときは数字を表示
        if(heal_value>0) {
            Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(to_string(heal_value), FONT_NAME_SYSTEM, 30*ratio);
            label->setAlignment(TextHAlignment::CENTER);
            label->setTextColor(Color4B::GREEN);
            label->setPosition(Point(0, 120*ratio));
            label->enableOutline(Color4B::BLACK, 2);
            this->addChild(label, 50);
            MoveBy *move = MoveBy::create(0.5f, Point(0, 20*ratio));
            RemoveSelf *remove = RemoveSelf::create();
            label->runAction(Sequence::create(move, remove, nullptr));
        }
        
        return;
    }
    
    string text = to_string(_hp) + "/" + to_string(_max_hp);
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 16*ratio);
    label->setAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(0, 84*ratio));
    label->enableOutline(Color4B::BLACK, 2);
    this->removeChildByTag(MAP_CHARA_HP_TAG);
    this->addChild(label, MAP_CHARA_HP_GAUGE_Z, MAP_CHARA_HP_TAG);
}

void MapCharaLayer::displayDamage(int damage)
{
    int random = (arc4random()%10 - 5)*10;
    int random2 = (arc4random()%10 - 5)*10;
    random2 += 96;//顔画像上にあるHPゲージあたりに出るよう補正
    
    Sprite *dmg_pop = Sprite::create("init/Dungeon/design/balloon_damage.png");
    dmg_pop->setAnchorPoint(Vec2(0.5,0.5));
    dmg_pop->setScale(0.2f*ratio);
    dmg_pop->setPosition(Point(random*ratio, random2*ratio));
    //dmg_pop->setGlobalZOrder(1000);
    dmg_pop->setCascadeOpacityEnabled(true);//透明度を子にも適用
    dmg_pop->setCascadeColorEnabled(true);
    this->addChild(dmg_pop, MAP_CHARA_DAMAGE_Z);
    
    //TTFConfig ttfConfig(FONT_NAME, 50, GlyphCollection::DYNAMIC);
    //TTFConfig ttfConfigEffect(ttfConfig);
    //ttfConfigEffect.distanceFieldEnabled = true;
    
    //auto label = Label::createWithTTF(ttfConfigEffect, damageStr);
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(to_string(damage), FONT_NAME_SYSTEM, 50);
    
    label->setAnchorPoint(Point(0.5f,0.5f));
    
    label->setTextColor(Color4B::WHITE);
    
    label->setPosition(Point(dmg_pop->getContentSize().width/2, dmg_pop->getContentSize().height/2));
    
    //label->enableOutline(Color4B::BLACK, 2);
    //label->setGlobalZOrder(1001);
    dmg_pop->addChild(label);
    
    
    ScaleTo *scale;
    if(damage>=400){
        scale = ScaleTo::create(0.5f, 1.25f*ratio);
    }else if(damage>=300){
        scale = ScaleTo::create(0.5f, 1.1f*ratio);
    }else if(damage>=200){
        scale = ScaleTo::create(0.5f, 0.95f*ratio);
    }else if(damage>=100){
        scale = ScaleTo::create(0.5f, 0.85f*ratio);
    }else if(damage<=9){
        scale = ScaleTo::create(0.5f, 0.6f*ratio);
    }else{
        scale = ScaleTo::create(0.5f, 0.7f*ratio);
    }
    auto easeE = EaseElasticOut::create(scale, 0.01);
    auto delay = DelayTime::create(0.1f);
    auto fadeO = FadeOut::create(0.15f);
    auto remove = RemoveSelf::create();
    dmg_pop->runAction(Sequence::create(easeE,delay,fadeO,remove, NULL));
    
    _hp -= damage;
    if(_hp<0) _hp = 0;
    displayHpGauge(0);
}

void MapCharaLayer::showEffect(Point position, string filename, bool auto_remove_flg, int tag_id)
{
    //イフェクト
    ParticleSystemQuad* particle = ParticleSystemQuad::create(filename);
    particle->setAutoRemoveOnFinish(auto_remove_flg);
    particle->setPositionType(ParticleSystem::PositionType::GROUPED);
    particle->setScale(ratio);
    particle->setPosition(position);
    this->addChild(particle, MAP_CHARA_CAMP_EFFECT_Z, tag_id);
}

void MapCharaLayer::campIn()
{
    if(this->getChildByTag(MAP_CHARA_CAMP_EFFECT_TAG)==nullptr) {
        showEffect(Point(0, 0), "download/Effect/Particle/map/camp.plist", false, MAP_CHARA_CAMP_EFFECT_TAG);
    }
}

void MapCharaLayer::heal()
{
    if(this->getChildByTag(MAP_CHARA_HEAL_EFFECT_TAG)==nullptr) {
        showEffect(Point(0, 0), "download/Effect/Particle/map/heal.plist", true, MAP_CHARA_HEAL_EFFECT_TAG);
    }
}

void MapCharaLayer::moveStart(Point target_position, float move_speed, bool effect_only_flg)
{
    _target_position = target_position;
    _move_speed = move_speed;
    _effect_only_flg = effect_only_flg;
    
    startMoveAnimation();

    this->removeChildByTag(MAP_CHARA_CAMP_EFFECT_TAG);
    
    move();
}

void MapCharaLayer::move()
{
    Point now_position = this->getPosition();
    float distance = calculateDistanceAtHexMap(now_position, _target_position);
    float diff_x = (_target_position.x-now_position.x)/distance;
    float diff_y = (_target_position.y-now_position.y)/distance;
    Point target_position = Point(now_position.x + diff_x*_move_speed, now_position.y + diff_y*_move_speed);
    int angle = static_cast<int>( atan2(diff_y, diff_x) * 180/PI + 360 ) % 360;
    //HMSLOG("angle=%d", angle);
    if(angle<22.5+45.0*0) _direction = 2;
    else if(angle<22.5+45.0*1) _direction = 1;
    else if(angle<22.5+45.0*2) _direction = 0;
    else if(angle<22.5+45.0*3) _direction = 7;
    else if(angle<22.5+45.0*4) _direction = 6;
    else if(angle<22.5+45.0*5) _direction = 5;
    else if(angle<22.5+45.0*6) _direction = 4;
    else if(angle<22.5+45.0*7) _direction = 3;
    else if(angle<22.5+45.0*8) _direction = 2;
    
    float dt = calculateDistanceAtHexMap(now_position, target_position) * 0.01f;

    //移動させる
    startMoveAnimation();
    MoveTo *move = MoveTo::create(dt, target_position);
    CallFunc *expire = CallFunc::create([this, distance, target_position]()
                                        {
                                            this->setZOrder(MAP_CHARA_Z - target_position.y);
                                            
                                            MapLayer *map_layer = (MapLayer *)this->getParent();
                                            MapScene *map_scene = (MapScene *)map_layer->getParent();
                                            MapCharaLayer *chara_enemy = nullptr;
                                            if(_chara_type==MAP_CHARA_TYPE_PLAYER && _effect_only_flg==false) {
                                                chara_enemy = map_layer->getCharaNearBy(this);
                                            }
                                            
                                            //目的地に達したか、敵と接触したら停止する
                                            if(distance<this->_move_speed || chara_enemy!=nullptr) {
                                                this->stopMoveAnimation();
                                                
                                                //演出ではない場合
                                                if(_effect_only_flg==false) {
                                                    //サーバにコマンド送信
                                                    map_scene->insertSendCommandList("moveuser_" + to_string(_user_master_id) + "_" + to_string(target_position.x/ratio) + "," + to_string(target_position.y/ratio));
                                                    map_scene->sendCommandData();

                                                    //バトル突入チェック
                                                    if(chara_enemy!=nullptr) {
                                                        HMSLOG("enemy found");
                                                        //this->scheduleOnce(schedule_selector(MapScene::checkBattle), 1.0f);
                                                        DelayTime *delay = DelayTime::create(1.0f);
                                                        CallFunc *expire1 = CallFunc::create([this, map_scene]()
                                                                                             {
                                                                                                 map_scene->checkBattle(0.0f);
                                                                                             });//ラムダ式
                                                        this->runAction(Sequence::create(delay, expire1, NULL));
                                                    }
                                                    
                                                    //バトル突入ではない場合に限り、お宝チェック
                                                    if(chara_enemy==nullptr && _chara_type==MAP_CHARA_TYPE_PLAYER && _effect_only_flg==false) {
                                                        MapTreasureLayer *treasure = map_layer->getTreasureNearBy(this);
                                                        if(treasure!=nullptr && treasure->getOpenFlg()==false) {
                                                            HMSLOG("treasure found. treasure_id=%s", treasure->getTreasureId().c_str());
                                                            map_scene->insertSendCommandList("treasure_" + to_string(_user_master_id) + "_" + treasure->getTreasureId());
                                                        }
                                                    }
                                                }
                                                
                                                /*
                                                //キャラの到達点がキャンプの近くならばイフェクトを表示する
                                                Sprite *camp = map_layer->getCampNearBy(this);
                                                if(camp!=nullptr) {
                                                    //イフェクト
                                                    showEffect(Point(0, 0), "download/Effect/Particle/map/camp.plist", false, MAP_CHARA_CAMP_EFFECT_TAG);
                                                }
                                                 */
                                                
                                                return;
                                            }
                                            //それ以外の場合、移動を続ける
                                            this->move();
                                        });//ラムダ式
    this->stopAllActions()  ; 
    this->runAction(Sequence::create(move, expire, NULL));
}

void MapCharaLayer::startMoveAnimation()
{
    if(isScheduled(schedule_selector(MapCharaLayer::changeSoldierImage))==false) {
        //歩く効果音を鳴らす
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/walk_on_gravel.mp3",false);
        this->schedule(schedule_selector(MapCharaLayer::changeSoldierImage), 0.2f);
    }
}

void MapCharaLayer::stopMoveAnimation()
{
    this->unschedule(schedule_selector(MapCharaLayer::changeSoldierImage));
}

void MapCharaLayer::changeSoldierImage(float dt)
{
    //画像切替
    if(_img_no==1) _img_no = 2;
    else _img_no = 1;
    
    string name = "soldier";
    if(_chara_type==MAP_CHARA_TYPE_ENEMY) name = "enemy";
    string filename = "init/Map/soldier/" + name + "_" + to_string(_direction) + "_" + to_string(_img_no) + ".png";
    Sprite *img = Sprite::create(filename.c_str());
    img->setScale(ratio);
    img->setPosition(Point(0*ratio, 0*ratio));
    this->removeChildByTag(MAP_CHARA_SOLDIER_TAG);
    this->addChild(img, MAP_CHARA_SOLDIER_Z, MAP_CHARA_SOLDIER_TAG);
}
