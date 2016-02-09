
#include "UIEffectLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "SimpleAudioEngine.h"
#include "HmsUtilityFunction.h"

#define IMG_RATIO 0.333f



bool UIEffectLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    ratio = getVisibleRatio();
    return true;
}
void UIEffectLayer::initialize(){
    
    
    
    
    
}
void UIEffectLayer::place_frame_effect(Point point){//移動できる場所を色付け＋点滅
  
    placeFrameSprite.push_back(Sprite::create("init/Dungeon/design/battle_skill_select.png"));
    placeFrameSprite.back()->setScale(ratio * 0.73, ratio * 0.81);
    placeFrameSprite.back()->setPosition(point);
    placeFrameSprite.back()->setOpacity(0);
    this->addChild(placeFrameSprite.back(), 1);
    
    auto tinto = TintTo::create(0.8, 127,127,127);
    auto ease1 = EaseInOut::create(tinto, 3);
    auto fade1 = FadeTo::create(0.1, 255);
    auto spa1 = Spawn::create(tinto,ease1,fade1, NULL);
    
    
    auto delay = DelayTime::create(0.1);
    
    
    auto tinto2 = TintTo::create(0.8, 255, 255, 255);
    auto ease2 = EaseInOut::create(tinto2, 3);
    auto fade2 = FadeTo::create(0.5, 255);
    auto spa2 = Spawn::create(tinto2,ease2,fade2, NULL);
    
    
    auto seq = Sequence::create(spa2,delay,spa1,delay, NULL);
    auto rep = RepeatForever::create(seq);
    placeFrameSprite.back()->runAction(rep);
    
}

void UIEffectLayer::place_frame_effect_support(Point point){//移動できる場所を色付け＋点滅
    
    placeFrameSprite.push_back(Sprite::create("init/Dungeon/design/battle_skill_select.png"));
    placeFrameSprite.back()->setScale(ratio * 0.88, ratio * 0.88);
    placeFrameSprite.back()->setPosition(point);
    //placeFrameSprite.back()->setOpacity(128);
    this->addChild(placeFrameSprite.back(), 1);
    
    auto tinto = TintTo::create(0.4, 127,127,127);
    auto delay = DelayTime::create(0.4);
    auto tinto2 = TintTo::create(0.4, 255, 255, 255);
    auto seq = Sequence::create(delay,tinto,delay,tinto2, NULL);
    auto rep = RepeatForever::create(seq);
    placeFrameSprite.back()->runAction(rep);
    
}

void UIEffectLayer::reset_place_frame_effect(){
    for(int i=0;i<placeFrameSprite.size();i++){
        removeChild(placeFrameSprite.at(i));
        
    }
}
