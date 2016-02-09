//
//  SkillCutInLayer.cpp
//  war
//
//  Created by HMS on 2015/01/09.
//
//

#include "SkillCutInLayer.h"
#include "HmsGeometricFunction.h"
//#include "DungeonScene.h"
#include "common.h"

USING_NS_CC;

bool SkillCutInLayer::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	HMSLOG("SkillCutInLayer::init()");
	
	return true;
}
void SkillCutInLayer::setData(int chara_id, string skill_name, float time)
{
	ratio = getVisibleRatio();
	play_back_time = time;
	
	//レイヤーの後ろにタップ処理が伝搬しないように設定する
	listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event){
		return true ;
	};
	//タップイベントの登録
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	//背景となる影レイヤー
	auto shadow_layer = LayerColor::create(Color4B(0, 0, 0, 100));
	this->addChild(shadow_layer);
	
	//------------------カットインレイヤーを構成------------------//
	//フレーム構築
	frame = Sprite::create("init/Dungeon/design/cat_in_bg.png");
	//画像の大きさを調整
	frame->setScale(ratio);
	//配置設定
	float frame_center_width = DISPLAY_WIDTH / 2;
	float frame_center_height = DISPLAY_HEIGHT / 2;
	frame->setPosition(convertFromDisplaySize(Point(frame_center_width, frame_center_height), 0.5, 0.5));
	//レイヤーの子に設定
	this->addChild(frame);
    
    //エフェクト音
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/skill_hatudou1.mp3",false);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/skill_hatudou2.mp3",false);
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/skill_hatudou3.mp3",false);
	
	//パーティクル出力
	//フレーム上のエフェクト
	ParticleSystemQuad* p1 = ParticleSystemQuad::create("download/Effect/Particle/fireline1.plist");
	p1->setScale(ratio);
	p1->setAutoRemoveOnFinish(true);
	p1->setPosition(Point(frame->getContentSize().width / 2, frame->getContentSize().height + p1->getContentSize().height / 2));
	frame->addChild(p1, 1);
	//フレーム下のエフェクト
	ParticleSystemQuad* p2 = ParticleSystemQuad::create("download/Effect/Particle/fireline1.plist");
	p2->setScale(ratio);
	p2->setAutoRemoveOnFinish(true);
	p2->runAction(RotateBy::create(0,180));
	p2->setPosition(Point(frame->getContentSize().width / 2, -(p2->getContentSize().height / 2)));
	frame->addChild(p2, 2);
	
	//フレーム内の構築
	
	//キャラシルエット
	Sprite* charaImg = Sprite::create("download/Chara/chara400/" + to_string(chara_id) + ".png");
	int chara_img_width = charaImg->getContentSize().width / 2;
	int chara_img_height = frame->getContentSize().height / 2 + 20;
	charaImg->setPosition(Point(-chara_img_width, chara_img_height));
	MoveBy* move1 = MoveBy::create(0.2f,Point(charaImg->getContentSize().width, 0));
	MoveBy* move2 = MoveBy::create(1.0f,Point(50, 0));
	MoveBy* move3 = MoveBy::create(0.3f,Point(frame->getContentSize().width, 0));
	charaImg->runAction(Sequence::create(move1,move2,move3,nullptr));
	frame->addChild(charaImg, 3);
	
	//スキル名の背景
	Sprite* skill_name_bg = Sprite::create("init/Dungeon/design/cat_in_bk.png");
	float skill_name_heght = skill_name_bg->getContentSize().height + skill_name_bg->getContentSize().height / 2;
	skill_name_bg->setPosition(Point((skill_name_bg->getContentSize().width / 2) + (frame->getContentSize().width / 2), skill_name_heght));
	MoveBy* skill_name_bg_move1 = MoveBy::create(1.2f, Point(-200, 0));
	MoveTo* skill_name_bg_move2 = MoveTo::create(0.3f, Point(skill_name_bg->getContentSize().width / 2, skill_name_heght));
	skill_name_bg->runAction(Sequence::create(skill_name_bg_move1, skill_name_bg_move2, nullptr));
	frame->addChild(skill_name_bg, 4);
	
	//スキル名
	Label* skill_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(skill_name, FONT_NAME_SYSTEM, 40);
	skill_name_label->setPosition(Point(frame->getContentSize().width + skill_name_label->getContentSize().width, skill_name_heght));
    skill_name_label->enableOutline(Color4B(0, 0, 0, 255), 3);
	skill_name_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
	MoveBy* label_move1 = MoveBy::create(0.1f,Point(-skill_name_label->getContentSize().width - 100, 0));
	MoveBy* label_move2 = MoveBy::create(0.3f,Point(-100, 0));
	MoveBy* label_move3 = MoveBy::create(0.6f,Point(-frame->getContentSize().width, 0));
	auto ease_move1 = EaseExponentialIn::create(label_move1);
	auto ease_move3 = EaseExponentialIn::create(label_move3);
	skill_name_label->runAction(Sequence::create(ease_move1,
												 label_move2,
												 DelayTime::create(0.4f),
												 ease_move3,
												 nullptr
												 )
								);
	frame->addChild(skill_name_label, 5);
	
	this->runAction(Sequence::create(DelayTime::create(play_back_time),CallFunc::create([&](){//X秒後に終了
		this->removeFromParentAndCleanup(true);
	}), nullptr));
}
