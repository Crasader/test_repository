#include "ReplaceConfirmLayer.h"
#include "HmsGeometricFunction.h"
#include "DungeonScene.h"
#include "Chara.h"
#include "CharaList.h"

#define YES_TAG 2
#define NO_TAG 3

bool ReplaceConfirmLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
	
	ratio = getVisibleRatio();
    HMSLOG("ReplaceConfirmLayer::init()");
    
    return true;
}

void ReplaceConfirmLayer::setData(Chara *replace_chara, Chara *tapped_chara)
{
    ratio = getVisibleRatio();
    
    //DungeonScene *dung = static_cast<DungeonScene *>(getParent());
    for(Chara * chara : CharaList::getAll()){
        chara->charaNameTab(false);//名前表示の消去
    }
    
    this->replace_chara=replace_chara;
    this->tapped_chara=tapped_chara;
	
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
	
	
    //窓の表示
    Sprite* frame = Sprite::create("init/Dungeon/design/battle_card_change_bk.png");
    frame->setScale(0);//アクションで大きくするため。
    frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 0.5, 0.5));
    this->addChild(frame, 2, POPUP_ACTION_TAG);
	
	//テキスト枠
	Sprite* text_frame = Sprite::create("init/Home/Popup/popup_index_s.png");
	text_frame->setPosition(frame->getContentSize().width / 2, frame->getContentSize().height - 10);
	frame->addChild(text_frame, 1);
	
	//テキスト
	Label* text = Label::createWithSystemFont("カードを置き換えますか？", FONT_NAME_SYSTEM, 28);
	text->setPosition(Point(text_frame->getContentSize().width / 2, text_frame->getContentSize().height / 2 - 8));
	text->enableOutline(Color4B(0,0,0,255),1);
	text_frame->addChild(text, 1);

    //キャラ画像
    Sprite* left_chara_img = Sprite::create("download/Chara/all400/" + to_string(replace_chara->getCharaId()) + ".png");
	left_chara_img->setScale(0.5f);
    left_chara_img->setPosition(Point(frame->getContentSize().width / 2 + 150, frame->getContentSize().height - 170));
    frame->addChild(left_chara_img, 2);
	
    Sprite* right_chara_img = Sprite::create("download/Chara/all400/" + to_string(tapped_chara->getCharaId()) + ".png");
	right_chara_img->setScale(0.5f);
    right_chara_img->setPosition(Point(frame->getContentSize().width / 2 - 150, frame->getContentSize().height - 170));
    frame->addChild(right_chara_img, 3);
    
    auto delay1 = DelayTime::create(0.3f);
    auto delay2 = DelayTime::create(1.6f);
    auto move1 = MoveTo::create(0.8f, Point(frame->getContentSize().width / 2 + 150, frame->getContentSize().height - 170));
    auto move2 = MoveTo::create(0, Point(frame->getContentSize().width / 2 - 150, frame->getContentSize().height - 170));
    auto seq = Sequence::create(delay1,move1,delay2,move2, NULL);
    auto rep = RepeatForever::create(seq);
    right_chara_img->runAction(rep);
    
	//矢印
	Sprite* arrow_img = Sprite::create("init/Dungeon/design/battle_card_change_arrow.png");
	arrow_img->setPosition(Point(frame->getContentSize().width / 2, frame->getContentSize().height - 170));
	frame->addChild(arrow_img, 2);
	
    auto delayArrow = DelayTime::create(0.7f);
    auto fadeArrow = FadeOut::create(0);
    auto delayArrow2 = DelayTime::create(2.0f);
    auto fadeinArrow = FadeIn::create(0);
    auto seqArrow = Sequence::create(delayArrow,fadeArrow,delayArrow2,fadeinArrow, NULL);
    auto repArrow = RepeatForever::create(seqArrow);
    arrow_img->runAction(repArrow);
    
	//『はい』ボタン
	Sprite* yes_off = Sprite::create("init/Home/Common/bt_yes.png");
	Sprite* yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
	yes_on->setColor(Color3B::GRAY);
	MenuItemSprite* yes_btn = MenuItemSprite::create(yes_off, yes_on, [&](Ref* pSender){
		replace();
	});
	yes_btn->setPosition(Point((frame->getContentSize().width / 2) - (yes_btn->getContentSize().width / 2) - 10, (yes_btn->getContentSize().height / 2) + 10));
		
	//『いいえ』ボタン
	Sprite* no_off = Sprite::create("init/Home/Common/bt_no.png");
	Sprite* no_on  = Sprite::create("init/Home/Common/bt_no.png");
	no_on->setColor(Color3B::GRAY);
	MenuItemSprite* no_btn = MenuItemSprite::create(no_off, no_on, [&](Ref* pSender){
		deleteAction();
	});
	no_btn->setPosition(Point((frame->getContentSize().width / 2) + (no_btn->getContentSize().width / 2) + 10, (no_btn->getContentSize().height / 2) + 10));
		
	Menu* menu = Menu::create(yes_btn, no_btn, nullptr);
	menu->setPosition( Point::ZERO );
	frame->addChild(menu, 5);
	
	popupAction();
}

void ReplaceConfirmLayer::replace()
{
    DungeonScene *dungeon_scene = (DungeonScene *)this->getParent();
    if(dungeon_scene->isCharacterPlacable()){//自分のターンでplace_character中のみ
        dungeon_scene->replaceChara(tapped_chara, replace_chara);
    }else{
        dungeon_scene->restoreChara(tapped_chara);
    }
    this->removeFromParentAndCleanup(true);
}

void ReplaceConfirmLayer::close()
{
    DungeonScene *dungeon_scene = (DungeonScene *)this->getParent();
    dungeon_scene->restoreChara(tapped_chara);
    this->removeFromParentAndCleanup(true);
}

//窓を出現させる
void ReplaceConfirmLayer::popupAction()
{
	//自身の子からPOPUP_ELEMENTの要素をすべて拡大する
	auto child = getChildByTag(POPUP_ACTION_TAG);
	auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
	child->runAction(scale);
}

//窓をひっこめ削除する
void ReplaceConfirmLayer::deleteAction()
{
	//自身の子からPOPUP_ELEMENTの要素をすべて縮小する
	auto child = getChildByTag(POPUP_ACTION_TAG);
	auto scale = ScaleTo::create(0.1f, 0);
	child->runAction(scale);
	
	runAction(Sequence::create(DelayTime::create(0.2f), CallFunc::create(CC_CALLBACK_0(ReplaceConfirmLayer::close,this)), nullptr));
}
