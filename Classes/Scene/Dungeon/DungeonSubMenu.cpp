#include "DungeonSubMenu.h"
#include "TurnManage.h"
#include "CharaList.h"

bool DungeonSubMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("DungeonSubMenuOptionLayer::()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //画面の半分のサイズの取得
    halfWidth = DISPLAY_WIDTH / 2 ;
    halfHeight = DISPLAY_HEIGHT / 2 ;
    
    return true;
}

void DungeonSubMenuLayer::onEnter()
{
    Layer::onEnter();
   
    //イベントリスナーの作成
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //背景画像の作成
    createBg();
    
    //ポップアップアクション
    auto children = this->getChildren();
    for (auto child : children) {
        child->getChildByTag(POPUP_ACTION_TAG) ;
        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
    
    //降伏ボタンの変更
    //changeSurrenderButton();
}

void DungeonSubMenuLayer::createBg()
{
    auto dungeon_scene = (DungeonScene *)this->getParent();
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3",false);//エフェクト音
    
    //影
    shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(halfWidth, halfHeight), 0.5, 0.5));
    shadow->setOpacity(0);
    this->addChild(shadow, 1);
    shadow->runAction(FadeTo::create(0.15f, 255));
    
    
    //背景
    bg = Sprite::create("init/Home/Popup/popup_450.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(halfWidth, halfHeight), 0.5, 0.5));
    this->addChild(bg, 2, POPUP_ACTION_TAG);
    
    //メニュー画像
    auto menuImg = Sprite::create("init/Home/Popup/popup_index_s.png");
    menuImg->setPosition(Point(320-35, 650-255));
    bg->addChild(menuImg, 5);
    
    //対戦相手
    auto enemy_board = Sprite::create("init/Dungeon/design/opponent_bk.png");
    enemy_board->setPosition(Point(320-35, 544-255));
    bg->addChild(enemy_board, 5);
    
    //ターン数
    auto turn_board = Sprite::create("init/Dungeon/design/act_bk.png");
    turn_board->setPosition(Point(320-35, 433-255));
    bg->addChild(turn_board, 5);
    
    //降伏ボタン
    Sprite *surrender_off;
    Sprite *surrender_on;
    if(dungeon_scene->isTutorial() == false){
        surrender_off = Sprite::create("init/Dungeon/design/bt_surrender.png");
        surrender_on  = Sprite::create("init/Dungeon/design/bt_surrender.png");
    }else{
        surrender_off = Sprite::create("download/Dungeon/bt_skip.png");
        surrender_on  = Sprite::create("download/Dungeon/bt_skip.png");
    }
        surrender_on->setColor(Color3B(180, 180, 180));
        _surrender_btn = MenuItemSprite::create(surrender_off, surrender_on, CC_CALLBACK_1(DungeonSubMenuLayer::onSurrenderButton, this));
        auto *menu1 = Menu::create(_surrender_btn, NULL);
        menu1->setPosition(Point(bg->getContentSize().width/2 - 117, 65));
        bg->addChild(menu1, 10);
    /*}else{
        auto surrender_on  = Sprite::create("init/Dungeon/design/bt_surrender.png");
        //surrender_on->setScale(0);
        surrender_on->setColor(Color3B(180, 180, 180));
        surrender_on->setPosition(Point(bg->getContentSize().width/2 - 117, 65));
        bg->addChild(surrender_on, 10);

    }*/
    
    //オプションボタン
    auto option_off = Sprite::create("init/Dungeon/design/bt_option.png");
    auto option_on  = Sprite::create("init/Dungeon/design/bt_option.png");
    option_on->setColor(Color3B(180, 180, 180));
    _option_btn = MenuItemSprite::create(option_off, option_on, CC_CALLBACK_1(DungeonSubMenuLayer::onOptionButton, this));
    auto *menu2 = Menu::create(_option_btn, NULL);
    menu2->setPosition(Point(bg->getContentSize().width/2 + 117, 65));
    bg->addChild(menu2, 10);


    Label* menuLabel;
    if(dungeon_scene->isTutorial()){
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("チュートリアル", FONT_NAME_SYSTEM, 33);
    } else if(dungeon_scene->isEventBattle()) {
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("イベント対戦", FONT_NAME_SYSTEM, 33);
    } else if(dungeon_scene->isQuestBattle()) {
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("中華統一戦記", FONT_NAME_SYSTEM, 33);
    } else if(dungeon_scene->isRankingEventBattle()) {
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("国士無双イベント", FONT_NAME_SYSTEM, 33);
    } else if(dungeon_scene->isChallengeEventBattle()) {
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("試練", FONT_NAME_SYSTEM, 33);
    } else if(dungeon_scene->getBattleMode() == "vs"){
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("オンライン対戦", FONT_NAME_SYSTEM, 33);
    } else if(dungeon_scene->getBattleMode() == "solo"){
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("オフライン対戦", FONT_NAME_SYSTEM, 33);
    } else{
        menuLabel = HMSFontUtil::createWithTTFBySystemFontArgument("対戦", FONT_NAME_SYSTEM, 33);
    }
    
    menuLabel->setPosition(Point(320-35, 648-255));
    menuLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(menuLabel, 12);


    //チュートリアル終了ボタン（※現在使用していない）
//    _tutorial_finish_btn = MenuItemImage::create("init/Home/Common/bt3.png", "init/Home/Common/bt3.png", CC_CALLBACK_1(DungeonSubMenuLayer::onTutorialEnd, this));
//    auto tutorialLabel = HMSFontUtil::createWithTTFBySystemFontArgument("チュートリアル終了", FONT_NAME_SYSTEM, 35);
//    tutorialLabel->setPosition(_tutorial_finish_btn->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
//    tutorialLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//    _tutorial_finish_btn->setScale(1.0f);
//    _tutorial_finish_btn->setAnchorPoint(Point(0.5, 0));
//    _tutorial_finish_btn->addChild(tutorialLabel);
    
    
/*    if (!(dungeon_scene->isTutorial())) {
        //降伏できない警告ラベル
        _alert_surrender_label = HMSFontUtil::createWithTTFBySystemFontArgument("※自分のターンでしか降伏できません", FONT_NAME_SYSTEM, 22);
        _alert_surrender_label->setPosition(Point(320-35, 374-255));
        bg->addChild(_alert_surrender_label, 10);
    }
*/
    if(dungeon_scene->isQuestBattle() || dungeon_scene->isEventBattle()) {//クエスト・イベント
        
        string level = dungeon_scene->json_master["player_data" + to_string(TurnManage::getEnemyNo())]["stage_name"].string_value() ;
        auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument(level, FONT_NAME_SYSTEM, 30);
        level_label->setAnchorPoint(Point(0, 0.5));
        //level_label->setColor(Color3B(255, 204, 0));
        level_label->setPosition(Point(200-35, 552-255));
        bg->addChild(level_label,11) ;
        
        //階級名
        string rank_name = dungeon_scene->json_master["player_data" + to_string(TurnManage::getEnemyNo())]["rank_name"].string_value() ;
        auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(rank_name, FONT_NAME_SYSTEM, 36);
        rank_label->setAnchorPoint(Point(0, 0.5));
        rank_label->setColor(Color3B(255, 204, 0));
        rank_label->setPosition(Point(200-35, 503-255));
        bg->addChild(rank_label ,11) ;

        Label* vs_player;
        string vs_string = dungeon_scene->player[TurnManage::getEnemyIndex()]->getName();
        vs_player = HMSFontUtil::createWithTTFBySystemFontArgument(vs_string, FONT_NAME_SYSTEM, 36);
        vs_player->setPosition(Point(200 + rank_label->getContentSize().width +23-35, 503-255));
        vs_player->setAnchorPoint(Point(0, 0.5));
        bg->addChild(vs_player, 10);
        
    }else if(dungeon_scene->isRankingEventBattle()){//国士無双イベント
        string level = dungeon_scene->json_master["player_data" + to_string(TurnManage::getEnemyNo())]["stage_name"].string_value() +"戦目" ;
        auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument(level, FONT_NAME_SYSTEM, 36);
        level_label->setAnchorPoint(Point(0, 0.5));
        level_label->setColor(Color3B(255, 204, 0));
        level_label->setPosition(Point(200-35, 503-255));
        bg->addChild(level_label,11) ;
        
        //階級名
        string rank_name = dungeon_scene->json_master["player_data" + to_string(TurnManage::getEnemyNo())]["rank_name"].string_value() ;
        auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(rank_name, FONT_NAME_SYSTEM, 33);
        rank_label->setAnchorPoint(Point(0, 0.5));
        //rank_label->setColor(Color3B(255, 204, 0));
        rank_label->setPosition(Point(200-35, 552-255));
        bg->addChild(rank_label ,11) ;
        
        Label* vs_player;
        string vs_string = dungeon_scene->player[TurnManage::getEnemyIndex()]->getName();
        vs_player = HMSFontUtil::createWithTTFBySystemFontArgument(vs_string, FONT_NAME_SYSTEM, 36);
        vs_player->setPosition(Point(200 + level_label->getContentSize().width +23-35, 502-255));
        vs_player->setAnchorPoint(Point(0, 0.5));
        bg->addChild(vs_player, 10);

        
    }else{//対戦の時
        Label* vs_player;
        string vs_string = dungeon_scene->player[TurnManage::getEnemyIndex()]->getName();
        vs_player = HMSFontUtil::createWithTTFBySystemFontArgument(vs_string, FONT_NAME_SYSTEM, 36);
        vs_player->setPosition(Point(200-35, 503-255));
        vs_player->setAnchorPoint(Point(0, 0.5));
        bg->addChild(vs_player, 10);
        
        //文字「Lv.」
        auto lavel_lv = HMSFontUtil::createWithTTFBySystemFontArgument("Lv.", FONT_NAME_SYSTEM, 27);
        lavel_lv->setAnchorPoint(Point(0, 0.5));
        lavel_lv->setColor(Color3B(255, 204, 0));
        lavel_lv->setPosition(Point(200-35, 550-255));
        bg->addChild(lavel_lv ,11) ;
        
        string level = dungeon_scene->json_master["player_data" + to_string(TurnManage::getEnemyNo())]["level"].string_value() ;
        if(dungeon_scene->isTutorial()) level = "1";//チュートリアルの時は１で
        auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument(level, FONT_NAME_SYSTEM, 27);
        level_label->setAnchorPoint(Point(0, 0.5));
        level_label->setColor(Color3B(255, 204, 0));
        level_label->setPosition(Point(248-35, 550-255));
        bg->addChild(level_label,11) ;
        
        //階級名
        string rank_name = dungeon_scene->json_master["player_data" + to_string(TurnManage::getEnemyNo())]["rank_name"].string_value() ;
        auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(rank_name, FONT_NAME_SYSTEM, 27);
        rank_label->setAnchorPoint(Point(0, 0.5));
        rank_label->setColor(Color3B(255, 204, 0));
        rank_label->setPosition(Point(330-35, 550-255));
        bg->addChild(rank_label ,11) ;
    }
    
    //ターンの数
    string turn_string = to_string(TurnManage::getNowTurn());
    turn_label = HMSFontUtil::createWithTTFBySystemFontArgument(turn_string, FONT_NAME_SYSTEM, 35);
    turn_label->setPosition(Point(288-35, 431-255));
    turn_label->setColor(Color3B(225, 225, 225));
    turn_label->setAnchorPoint(Point(1, 0.5));
    bg->addChild(turn_label, 10);
    
    //ターン目
    auto turn_label2 = HMSFontUtil::createWithTTFBySystemFontArgument("ターン目", FONT_NAME_SYSTEM, 27);
    turn_label2->setPosition(Point(342-35, 431-255));
    turn_label2->setColor(Color3B(225, 225, 225));
    bg->addChild(turn_label2, 10);
    
    
    auto back_spr = Sprite::create("download/Chara/parts100/card_bk" + to_string(CharaList::getLeader(TurnManage::getEnemyIndex())->getRare()) + ".png");
    auto chara_spr = Sprite::create("download/Chara/chara100/" + to_string(CharaList::getLeader(TurnManage::getEnemyIndex())->getCharaId()) + ".png");
    auto rare_spr = Sprite::create("download/Chara/parts100/card_rare" + to_string(CharaList::getLeader(TurnManage::getEnemyIndex())->getRare()) + ".png");
    back_spr->setPosition(Point(132-35, 534-255));
    chara_spr->setPosition(Point(132-35, 534-255));
    rare_spr->setPosition(Point(132-35, 534-255));
    bg->addChild(back_spr, 11);
    bg->addChild(chara_spr, 12);
    bg->addChild(rare_spr, 13);
    
    //モーダルレイヤーを消すボタン
    auto back = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", CC_CALLBACK_0(DungeonSubMenuLayer::deleteAction, this));
    auto Menu1 = Menu::create(back, nullptr);
    Menu1->setPosition(Point(bg->getContentSize().width - 10, bg->getContentSize().height - 10));
    bg->addChild(Menu1, 3);
}

// 降伏ボタンstart
void DungeonSubMenuLayer::onSurrenderButton(Ref *obj)
{    
    deleteAction();
    
    auto popup_layer = DungeonSubMenuSurrenderLayer::create();
    auto dungeon_scene = (DungeonScene *)this->getParent();
    dungeon_scene->addChild(popup_layer, ORDER_RECEIVE_LAYER);
}

// オプションボタンstart
void DungeonSubMenuLayer::onOptionButton(Ref *obj)
{
    deleteAction();
    
    auto popup_layer = DungeonSubMenuOptionLayer::create();
    auto dungeon_scene = (DungeonScene *)this->getParent();
    dungeon_scene->addChild(popup_layer, ORDER_RECEIVE_LAYER);
}

//窓をひっこめ削除する
void DungeonSubMenuLayer::deleteAction()
{
    shadow->runAction(FadeTo::create(0.15f, 0));
    
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == POPUP_ACTION_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
        
    runAction(Sequence::create(DelayTime::create(0.2f), RemoveSelf::create(), NULL));
}

/*void DungeonSubMenuLayer::changeSurrenderButton()
{
    //自分のターンにのみ降伏できるようにする
    auto dungeon_scene = (DungeonScene *)this->getParent();
    
    if (!(dungeon_scene->isTutorial())) {
        if (dungeon_scene->isMyTurn()) {
            //ボタンを押せる状態にする
            _surrender_btn->setEnabled(true);
            _surrender_btn->unselected();
            _alert_surrender_label->setVisible(false);
        }
        else {
            //ボタンを押せない状態にする
            _surrender_btn->setEnabled(false);
            _surrender_btn->selected();
            _alert_surrender_label->setVisible(true);
        }
    }
    
    string turn_string = to_string(TurnManage::getNowTurn());
    turn_label->setString(turn_string);
}*/