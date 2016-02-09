//
//  SkillErrorLayer.cpp
//  war
//
//  Created by HMS on 2015/01/20.
//
//

#include "SkillErrorLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"
#include "common.h"

USING_NS_CC;

bool SkillErrorLayer::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	HMSLOG("SkillErrorLayer::init()");
	
	return true;
}
void SkillErrorLayer::setData(int error_type, float time)
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
	this->addChild(shadow_layer, 1);
	
	frame = Sprite::create("init/Dungeon/design/battle_message.png");
	frame->setScale(0);//アクションで大きくするため
	frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
	this->addChild(frame, 2, POPUP_ACTION_TAG);
	
	Label* error_text;
	int font_size = 30;
	switch(error_type) {
		case 1:
			error_text = HMSFontUtil::createWithTTFBySystemFontArgument("スキルの対象がいません", FONT_NAME_SYSTEM, font_size);
			break;
		case 2:
			error_text = HMSFontUtil::createWithTTFBySystemFontArgument("敵武将がいません", FONT_NAME_SYSTEM, font_size);
			break;
		case 3:
			error_text = HMSFontUtil::createWithTTFBySystemFontArgument("犠牲にする味方武将がいません", FONT_NAME_SYSTEM, font_size);
			break;
        case 4:
            error_text = HMSFontUtil::createWithTTFBySystemFontArgument("前方に既に敵武将がいます", FONT_NAME_SYSTEM, font_size);
            break;
        case 5:
            error_text = HMSFontUtil::createWithTTFBySystemFontArgument("スキルの対象にする場所がありません", FONT_NAME_SYSTEM, font_size);
            break;
            
		default:
			error_text = HMSFontUtil::createWithTTFBySystemFontArgument("error", FONT_NAME_SYSTEM, font_size);
			break;
	}
	error_text->setPosition(Point(frame->getContentSize().width / 2, frame->getContentSize().height / 2));
//enableOutline(Color4B(0,0,0,255),1);
	frame->addChild(error_text, 1);

	//クローズボタン表示
	MenuItemImage* close_btn = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close.png", [&](Ref* pSender){
		deleteAction();
	});
	close_btn->setPosition(frame->getContentSize().width - 10, frame->getContentSize().height - 10);
	Menu* close_menu = Menu::create(close_btn, nullptr);
	close_menu->setPosition( Point::ZERO );
	frame->addChild(close_menu, 2);
	
	popupAction();
	
//	this->runAction(Sequence::create(DelayTime::create(play_back_time),CallFunc::create([&](){//再生時間後に終了処理
//		static_cast<DungeonScene *>(getParent())->useSkillEnd();
//		this->removeFromParentAndCleanup(true);
//	}), nullptr));
}
//窓を出現させる
void SkillErrorLayer::popupAction()
{
	//自身の子からPOPUP_ELEMENTの要素をすべて拡大する
	auto child = getChildByTag(POPUP_ACTION_TAG);
	auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
	child->runAction(scale);
}

//窓をひっこめ削除する
void SkillErrorLayer::deleteAction()
{
	//自身の子からPOPUP_ELEMENTの要素をすべて縮小する
	auto child = getChildByTag(POPUP_ACTION_TAG);
	auto scale = ScaleTo::create(0.1f, 0);
	child->runAction(scale);
	
	runAction(Sequence::create(DelayTime::create(0.2f), CallFunc::create([&](){//再生時間後に終了処理
		static_cast<DungeonScene *>(getParent())->useSkillEnd();
		this->removeFromParentAndCleanup(true);
	}), nullptr));
}
