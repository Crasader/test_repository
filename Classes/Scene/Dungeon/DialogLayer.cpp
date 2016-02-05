/*最前面に表示されるダイアログやアニメーションなど*/
#include "DialogLayer.h"
#include "DungeonScene.h"
#include "HmsGeometricFunction.h"

bool DialogLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    return true;
}

void DialogLayer::changeTurnDialog(string player_name, bool my_trun){
	float ratio = getVisibleRatio();
	
	Sprite* frame;//ダイアログの画像
	float frame_height = 0.0f;//ダイアログの高さ
	
	if(my_trun) {//自ターン(青)
		frame = Sprite::create("init/Dungeon/design/battle_turn_bk_blue.png");
		frame_height = 390.0f;
	} else {//相手ターン(赤)
		frame = Sprite::create("init/Dungeon/design/battle_turn_bk_red.png");
		frame_height = DISPLAY_HEIGHT - 380.0f;
	}
	
    auto node = Node::create();
    node->setScale(ratio, 0.0f);
    node->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, frame_height), 0.5, 0.5));
    addChild(node);
    
	//ダイアログの大きさを調整
	//frame->setScale(ratio, 0.0f);
	//frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, frame_height), 0.5, 0.5));
	//frame->setOpacity(180);
	node->addChild(frame);
    //frame->setGlobalZOrder(10.0f);
    
	//テキスト
	Label* txt_label = HMSFontUtil::createWithTTFBySystemFontArgument(player_name + "のターン", FONT_NAME_SYSTEM, 30);
	float frame_internal_height = node->getContentSize().height / 2;
	txt_label->setPosition(Point(node->getContentSize().width + txt_label->getContentSize().width, frame_internal_height));
	node->addChild(txt_label,5);
    //txt_label->setGlobalZOrder(10.0f);
	//アクション作成
	node->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.2f, ratio, ratio)),
									  DelayTime::create(1.2f),
                                      Spawn::create(ScaleTo::create(0.2f, ratio, 0.0f),FadeTo::create(0.2f, 100), NULL),
									  RemoveSelf::create(true), nullptr));
	
	txt_label->runAction(Sequence::create(MoveTo::create(0.2f, Point(node->getContentSize().width / 2 + 40, frame_internal_height)),
										  MoveTo::create(1.2f, Point(node->getContentSize().width / 2 - 40, frame_internal_height)),
										  MoveTo::create(0.2f, Point(-(txt_label->getContentSize().width + 100), frame_internal_height)), nullptr));
}
