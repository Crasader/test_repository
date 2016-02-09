#include "DungeonSubMenuSurrender.h"
#include "TurnManage.h"

bool DungeonSubMenuSurrenderLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("DungeonSubMenuSurrenderLayer::()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //画面の半分のサイズの取得
    halfWidth = DISPLAY_WIDTH / 2 ;
    halfHeight = DISPLAY_HEIGHT / 2 ;
    
    return true;
}

void DungeonSubMenuSurrenderLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    createEventListener();
    
    //背景画像の作成
    createBg();
    
    //ボタンの作成
    createButton();
    
    //注意文の作成
    createLabel();

    //ポップアップアクション
    popupAction();
}

void DungeonSubMenuSurrenderLayer::createEventListener()
{
    //イベントリスナーの作成
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void DungeonSubMenuSurrenderLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(halfWidth, halfHeight), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    bg = Sprite::create("init/Home/Popup/popup_300.png");
    bg->setPosition(convertFromDisplaySize(Point(halfWidth, halfHeight), 0.5, 0.5));
    bg->setScale(0);
    this->addChild(bg, 2, POPUP_ACTION_TAG);
}

void DungeonSubMenuSurrenderLayer::createButton()
{
    //降伏の確認ボタン
    //キャンセル
    MenuItemImage* cancel_btn = MenuItemImage::create("init/Home/Common/bt_no.png", "init/Home/Common/be_no.png", CC_CALLBACK_1(DungeonSubMenuSurrenderLayer::onCancelButton, this));
    cancel_btn->setAnchorPoint(Point(0.5, 0));
    
    //OK
    MenuItemImage* ok_btn = MenuItemImage::create("init/Home/Common/bt_yes.png", "init/Home/Common/bt_yes.png",  CC_CALLBACK_1(DungeonSubMenuSurrenderLayer::onOkButton, this));
    ok_btn->setAnchorPoint(Point(0.5,0));

    Menu* menu = Menu::create(ok_btn, cancel_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(30);
    menu->setPosition(Point(bg->getContentSize().width / 2, bg->getContentSize().height / 4 - 50));
    bg->addChild(menu, 3);
    
    //モーダルレイヤーを消すボタン
    auto back = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", CC_CALLBACK_0(DungeonSubMenuSurrenderLayer::deleteAction, this));
    auto Menu1 = Menu::create(back, nullptr);
    Menu1->setPosition(Point(bg->getContentSize().width - 10, bg->getContentSize().height - 10));
    bg->addChild(Menu1, 3);
}

void DungeonSubMenuSurrenderLayer::createLabel()
{
	DungeonScene* dungeon_scene = static_cast<DungeonScene*>(getParent());
	string confirmMessage = "このまま降伏すると\n称号Ptが失われます。\n\nよろしいですか？";
    if(dungeon_scene->isTutorial()){//チュートリアルの場合
        confirmMessage = "\nチュートリアルをスキップします。\n本当によろしいですか？";
    }else if(dungeon_scene->isEventBattle()) {//イベント対戦の場合
		confirmMessage = "このまま降伏すると\n経験値が獲得できません。\n\n本当によろしいですか？";
	} else if(dungeon_scene->getBattleMode()=="vs") {//オンライン対戦の場合
		confirmMessage = "このまま降伏すると\n称号Pt、経験値が獲得できません。\n\n本当によろしいですか？";
    } else if(dungeon_scene->isQuestBattle()) { //クエスト対戦の場合
        confirmMessage = "このまま降伏すると\n経験値が獲得できません。\n\n本当によろしいですか？";
    } else if(dungeon_scene->isBossEventBattle()) { //ボスイベントの場合
        confirmMessage = "このまま降伏すると\n体力0で戦場に戻ります。\n\n本当によろしいですか？";
    } else {//オフライン対戦の場合(その他、該当なしの場合)
		confirmMessage = "このまま降伏すると\nホーム画面に戻ります。\n\n本当によろしいですか？";
	}
    
    //注意書きメッセージ
    auto confirmTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument(confirmMessage, FONT_NAME_SYSTEM, 27);
    confirmTextLabel->setHorizontalAlignment(TextHAlignment::CENTER);
    confirmTextLabel->setPosition(Point(bg->getContentSize().width/2, bg->getContentSize().height/2 + 50));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    bg->addChild(confirmTextLabel, 3);
}

//窓を出現させる
void DungeonSubMenuSurrenderLayer::popupAction()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3",false);

    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer();
    
    //自身の子からPOPUP_ELEMENTの要素をすべて拡大する
    auto children = this->getChildren();
    for (auto child : children) {
        HMSLOG("children:%d", child->getTag());
        if (child->getTag() == POPUP_ACTION_TAG) {
            auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
            child->runAction(scale);
        }
    }
    
    //指定秒数待ってからタップを可能にする。
    auto func = CallFunc::create([normal_scene](){
        normal_scene->removeTapDisableLayer() ;
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

//窓をひっこめ削除する
void DungeonSubMenuSurrenderLayer::deleteAction()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer() ;
    
    //自身の子からPOPUP_ELEMENTの要素をすべて縮小する
    auto children = this->getChildren();
    for (auto child : children) {
        HMSLOG("children:%d", child->getTag());
        if (child->getTag() == POPUP_ACTION_TAG) {
            auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this,normal_scene](){
        this->removeFromParent();
        //タップ禁止レイヤー削除
        normal_scene->removeTapDisableLayer() ;
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

//いいえボタンコールバック
void DungeonSubMenuSurrenderLayer::onCancelButton(Ref *obj)
{
    deleteAction();
}

//はいボタンコールバック
void DungeonSubMenuSurrenderLayer::onOkButton(Ref *obj)
{
    deleteAction();
    auto dungeon_scene = (DungeonScene *)this->getParent();
    
//    if(dungeon_scene->getBattleMode()=="vs" && !dungeon_scene->isMyTurn()) return;//対戦で敵のターンの時は降伏不能
    
    if(dungeon_scene->isTutorial()){//チュートリアルスキップ
        
        dungeon_scene->gameoverJudge("surrender");//敵が降参
        
    }else{//普通の幸福
        dungeon_scene->player[TurnManage::getPlayerIndex()]->setHp(0);
        if(dungeon_scene->getBattleMode()=="vs") {
            map<string , Json> send_map;
            send_map.insert(make_pair("type", Json("surrender")));
            send_map.insert(make_pair("player_no", Json(TurnManage::getPlayerNo())));
            
            dungeon_scene->send_message_vec.push_back(send_map);//送信待ちベクターに追加
            
            //dungeon_scene->sendMessage("surrender:" + toString(TurnManage::getPlayerNo()));
            //オンライン対戦時の降伏判定フラグを保存
            SaveData::saveInt("OnlineBattleSurrenderFlg", 1);
        }
        dungeon_scene->gameoverJudge("give_up");
        
    }
}


