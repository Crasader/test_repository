//
//  ComboEffectLayer2.cpp
//  war
//
//  Created by HMS on 2015/02/10.
//
//

#include "ComboEffectLayer2.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "SimpleAudioEngine.h"
#include "HmsUtilityFunction.h"
#include "Effect.h"

USING_NS_CC;

bool ComboEffectLayer2::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	HMSLOG("ComboEffectLayer2::init()");
	
	return true;
}
void ComboEffectLayer2::setData(Chara* chara_l, Chara* chara_r, float time)
{
	ratio = getVisibleRatio();
	play_back_time = time;
	
	listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event){
		return true ;
	};
	//タップイベントの登録
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	//キャラ同士の
	Point middle = (chara_l->getPosition() + chara_r->getPosition())/2;
	
//	//SuperAnimeアニメーション
//	string spritesheet_anim = FileUtils::getInstance()->fullPathForFilename("download/Effect/SAM/combo/combo.sam");
//	SuperAnim::SuperAnimNode* animNode1 = SuperAnim::SuperAnimNode::create(spritesheet_anim, 0, this);
//	this->addChild(animNode1);
//	animNode1->setTag(0);
//	animNode1->setPosition(Point(middle.x,middle.y + 100 * ratio));
//	animNode1->PlaySection("start");
	
	//スプライトシートアニメーション
	Effect *combo = Effect::create();
	combo->setData(this, "download/Effect/Sprite/combo", 27, Point(middle.x,middle.y + 100 * ratio), 1, 1);
	
	//スプライトシートアニメーション
	Effect *thunder = Effect::create();
	thunder->setData(this, "download/Effect/Sprite/thunder_long", 20, middle, 1, 1);
	
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/giga_thunderbolt.mp3");
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/katana.mp3");
	
	this->runAction(Sequence::create(DelayTime::create(play_back_time),CallFunc::create([&](){//再生時間後に終了処理
		this->removeFromParentAndCleanup(true);
	}), nullptr));
}
