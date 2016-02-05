#include "TournamentEntryListSprite.h"
#include "NormalScene.h"

TournamentEntryListSprite::TournamentEntryListSprite()
:_tournament_event_list_id()
,_tournament_name()
,_item_type()
,_count()
,_reception_time()
,_condition_name()
,_join_number()
,_start_date_timestamp()
,_condition_match_flg(false)
,_entry_time_flg(false)
{
}

TournamentEntryListSprite* TournamentEntryListSprite::create(json11::Json json, Node* normal_scene)
{
    TournamentEntryListSprite *sprite = new TournamentEntryListSprite();
    if (sprite && sprite->init(json, normal_scene))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEntryListSprite::init(json11::Json json, Node* normal_scene)
{
    if ( !Sprite::initWithFile("init/Home/Event/TournamentEvent/bk_tournament6.png") )
    {
        return false;
    }
    
    //データの設定
    setupData(json);
    
    _normal_scene = normal_scene;
    
    return true;
}

void TournamentEntryListSprite::setupData(json11::Json json)
{
    //データの設定
    setTournamentEventListId(json["tournament_event_list_id"].string_value());
    setTournamentName(json["name"].string_value());
    setItemType(json["item_type"].string_value());
    setCount(json["count"].string_value());
    setReceptionTime(json["reception_time"].string_value());
    setConditionName(json["condition_name"].string_value());
    setJoinNumber(json["join_number_text"].string_value());
    setResultText(json["result_text"].string_value());
    setStartDateTimestamp(json["start_date_timestamp"].string_value());
    setConditionMatchFlg(json["condition_match_flg"].bool_value());
    
    setRewardDataList(json["reward_data_list"].object_items()) ;
}

void TournamentEntryListSprite::onEnter()
{
    Sprite::onEnter();
    
    //トーナメント名の作成
    createTournamentName();

    //参加条件の作成、表示
    createConditionLabel();
    
    //受付期間の表示
    showReceptionTime();
    
    //参加人数の表示
    showJoinNumber();
    
    //参加人数の表示
    showResultText();
    
    //報酬ボタンの作成
    createRewardButton();
    
    //トーナメント参加ボタンの作成
//    createEntryButton();
    
    //トーナメントエントリー時間かチェック
//    checkEntryTime();
    
    //エントリー必要アイテムの表示
//    createEntryNeedItem();
    
    //参加条件チェック
    if (getConditionMatchFlg() == false) setConditionOver();
}

void TournamentEntryListSprite::createTournamentName()
{
    //トーナメント名の作成
    auto tournament_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_tournament_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_tournament_name, 34, 400));
    tournament_name_label->setColor(Color3B(255, 234, 0));
    tournament_name_label->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height - 30));
    tournament_name_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(tournament_name_label, Z_Tournament_Name, T_Tournament_Name);
}

void TournamentEntryListSprite::createConditionLabel()
{
    //参加条件
    auto condition_label = HMSFontUtil::createWithTTFBySystemFontArgument(_condition_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_condition_name, 28, 375));
    condition_label->setAnchorPoint(Point(0, 0.5));
    condition_label->setPosition(Point(186, this->getContentSize().height - 96));
    condition_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(condition_label, Z_Condition_Label, T_Condition_Label);
}

void TournamentEntryListSprite::showReceptionTime()
{
    //受付期間の表示
    auto event_open_label = HMSFontUtil::createWithTTFBySystemFontArgument(_reception_time, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_condition_name, 28, 375));
    event_open_label->setAnchorPoint(Point(0, 0.5));
    event_open_label->setPosition(Point(186, this->getContentSize().height - 162));
    event_open_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(event_open_label, Z_Event_Open_Label, T_Event_Open_Label);
}

void TournamentEntryListSprite::showJoinNumber()
{
    //参加人数の表示
    auto join_number_label = HMSFontUtil::createWithTTFBySystemFontArgument(_join_number, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_condition_name, 28, 375));
    join_number_label->setAnchorPoint(Point(0, 0.5));
    join_number_label->setPosition(Point(186, this->getContentSize().height - 222));
    join_number_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(join_number_label, Z_Event_Open_Label, T_Event_Open_Label);
}

void TournamentEntryListSprite::showResultText()
{
    auto result_ribbon = Sprite::create("init/Home/Event/TournamentEvent/ribbon.png");
    result_ribbon->setAnchorPoint(Point(0, 0.5));
    result_ribbon->setPosition(Point(50, 50));
    result_ribbon->setScale(0.3, 0.8);
    this->addChild(result_ribbon, Z_Event_Open_Label, T_Event_Open_Label);
    
    int fontSize;
    if(_result_text=="優勝"){
        fontSize = 40;
    }else if(_result_text=="準優勝"){
        fontSize = 32;
    }else{
        fontSize = 28;
    }
    
    //参加結果の表示
    auto result_text_label = HMSFontUtil::createWithTTFBySystemFontArgument(_result_text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_condition_name, fontSize, 375));
    result_text_label->setAnchorPoint(Point(0.5, 0.5));
    result_text_label->setPosition(Point(result_ribbon->getContentSize().width/2, result_ribbon->getContentSize().height/2 + 13));
    result_text_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    result_text_label->cocos2d::Node::setScaleX(1/0.3);
    result_text_label->cocos2d::Node::setScaleY(1/0.8);

    result_ribbon->addChild(result_text_label, Z_Event_Open_Label, T_Event_Open_Label);
}

void TournamentEntryListSprite::setConditionOver()
{
    //トーナメント参加条件に満たなかった時の設定
    
    //参加ボタンを暗くする
    auto entry_btn = (MenuItemSprite *)this->getChildByTag(T_Entry_Menu)->getChildByTag(T_Entry_MenuItem);
    entry_btn->selected();
    
    //参加条件の文字列を赤く点滅させる
    auto condition_label = (Label *)this->getChildByTag(T_Condition_Label);
    auto tint1 = TintTo::create(0.5f, 255, 130, 130);
    auto tint2 = TintTo::create(0.5f, 255, 255, 255);
    condition_label->runAction(RepeatForever::create(Sequence::create(tint1, tint2, nullptr)));
}

void TournamentEntryListSprite::createRewardButton()
{
    //報酬ボタンの作成
    auto reward_off = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    auto reward_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    reward_on->setColor(Color3B(180, 180, 180));
    
    auto reward_btn = MenuItemSprite::create(reward_off, reward_on,[this](Ref* sender){
        
        //報酬のスプライトを生成しvectorに入れる
        HmsVector<Sprite *> item_list ;
        int json_count = countJsonData(_reward_data_list) ;
        for(int i=0; i<json_count; i++){
            string str_index = toString<int>(i) ;
            TournamentEventRewardSprite *tournament_event_reward_sprite = TournamentEventRewardSprite::create(_reward_data_list[str_index]) ;
            item_list.push_back(tournament_event_reward_sprite) ;
        }
        
        //スクロール範囲を黒くする
        Sprite* black_sprite = Sprite::create();
        Rect rect = Rect(0, 0, 540, 660) ;
        black_sprite->setColor(Color3B::BLACK) ;
        black_sprite->setOpacity(80) ;
        black_sprite->setTextureRect(rect);
        black_sprite->setAnchorPoint(Point(0.5,1)) ;
        black_sprite->setPosition(Point(285,780)) ;
        
        //vectorからスクロールスプライトを作成
        ScrollSprite *scroll_sprite = ScrollSprite::create(item_list, Size(540, 660), ScrollSprite::ScrollDirection::VERTICAL, true) ;
        scroll_sprite->setAnchorPoint(Point(0.5,1)) ;
        scroll_sprite->setPosition(Point(285,780)) ;
        
        //ポップアップを作成
        auto func = [](){};
        auto popup_layer = PopupOkButtonLayer::create(func, "", true, PopupOkButtonLayer::PopupSize::Popup_800);
        Sprite *popup_bg = popup_layer->getBackground() ;
        popup_bg->addChild(black_sprite);
        popup_bg->addChild(scroll_sprite);
        _normal_scene->addChild(popup_layer, TournamentEntryListLayer::Z_Popup_Layer, TournamentEntryListLayer::T_Popup_Layer);
        
    });
    reward_btn->setTag(T_Reward_MenuItem);
    
   
    //対戦表
    auto bracket_off = Sprite::create("init/Home/Event/TournamentEvent/bt_competition_table.png");
    auto bracket_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_competition_table.png");
    bracket_on->setColor(Color3B(180, 180, 180));
    auto bracket_btn = MenuItemSprite::create(bracket_off, bracket_on,[this](Ref* sender){
        //対戦表ページへ遷移
        SaveData::saveString("bracket_type", "display") ;
        SaveData::saveString("bracket_back", "entry") ;
        SaveData::saveString("tournament_event_list_id", getTournamentEventListId()) ;
        auto normal_scene = (NormalScene *)_normal_scene;
        normal_scene->changePageFadeAction("TournamentEventBracket");
    });
    bracket_btn->setScale(0.62);
    bracket_btn->setTag(T_Bracket);
    
    auto menu2 = Menu::create(reward_btn, bracket_btn, nullptr);
    menu2->alignItemsHorizontallyWithPadding(0);
    menu2->setPosition(400, 58);
    this->addChild(menu2, Z_Bracket);
    
//    auto menu = Menu::create(reward_btn, nullptr);
//    menu->setPosition(Point(330, 58));
//    this->addChild(menu, Z_Reward_Menu, T_Reward_Menu);
}

void TournamentEntryListSprite::createEntryButton()
{
    //エントリーボタンの作成
    auto entry_off = Sprite::create("init/Home/Event/TournamentEvent/bt_entry.png");
    auto entry_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_entry.png");
    entry_on->setColor(Color3B(180, 180, 180));
    
    auto entry_btn = MenuItemSprite::create(entry_off, entry_on,[this](Ref* sender){
        
        auto menuItem = (MenuItem *)sender;
        
        if (getConditionMatchFlg() == false) {
            //条件を満たしていないポップアップの表示
            auto func = [](){};
            
            string comment = "このトーナメントへの参加条件を\n満たしていない為、参加できません。";
            auto popup_layer = PopupOkButtonLayer::create(func, comment);
            _normal_scene->addChild(popup_layer, TournamentEntryListLayer::Z_Popup_Layer, TournamentEntryListLayer::T_Popup_Layer);
            
            menuItem->selected();
        }
        else if(getEntryTimeFlg() == false) {
            //エントリー時間になっていない場合
            auto func = [](){};
            
            string comment = "エントリー受付期間に\nなっておりません。";
            auto popup_layer = PopupOkButtonLayer::create(func, comment);
            _normal_scene->addChild(popup_layer, TournamentEntryListLayer::Z_Popup_Layer, TournamentEntryListLayer::T_Popup_Layer);
            
            menuItem->selected();
        }
        else {
            //エントリー中だったら
            if(SaveData::loadString("tournament_event_entry")=="1"){
                if(getTournamentEventListId()==SaveData::loadString("tournament_event_list_id")){
                    auto normal_scene = (NormalScene *)_normal_scene;
                    normal_scene->changePage("TournamentEventBattleWait");
                }else{
                    //別のトーナメントに参加しようとした場合
                    auto func = [](){};
                    
                    string comment = "他のトーナメントに\nエントリーしています。";
                    auto popup_layer = PopupOkButtonLayer::create(func, comment);
                    _normal_scene->addChild(popup_layer, TournamentEntryListLayer::Z_Popup_Layer, TournamentEntryListLayer::T_Popup_Layer);
                    
                    menuItem->selected();
                    
                }
            }else{
                //選択したトーナメントイベントのデータを保存
                SaveData::saveString("tournament_event_list_id", _tournament_event_list_id);
            
                //トーナメント終了時刻の設定
                NormalScene::_tournament_next_battle_time = fromString<time_t>(_start_date_timestamp);
                
                //デッキ選択画面へ
                auto normal_scene = (NormalScene *)_normal_scene;
                normal_scene->changePage("TournamentEventDeckChoice");
            }
        }
    });
    entry_btn->setTag(T_Entry_MenuItem);
    entry_btn->selected();
    
    auto menu = Menu::create(entry_btn, nullptr);
    menu->setPosition(Point(495, 58));
    this->addChild(menu, Z_Entry_Menu, T_Entry_Menu);
    
    //光らせる為の白画像を透明で配置
    auto white_sp = Sprite::create("init/Home/Event/TournamentEvent/bt_white.png");
    white_sp->setPosition(Point(495, 58));
    white_sp->setOpacity(0); //透明で配置
    this->addChild(white_sp, Z_Entry_White, T_Entry_White);
}


void TournamentEntryListSprite::createEntryNeedItem()
{
    //エントリー参加必要画像
    string back_path = "";
    string item_path = "";
    if (_item_type == "jewel") {
        back_path = "init/Home/Event/TournamentEvent/necessary_jewel.png";
        item_path = "init/Home/Event/TournamentEvent/jewel.png";
    }
    else if (_item_type == "gold") {
        back_path = "init/Home/Event/TournamentEvent/necessary_coin.png";
        item_path = "init/Home/Event/TournamentEvent/coin.png";
    }
    
    if (back_path != "") {
        
        //背景画像
        auto necessary_sp = Sprite::create(back_path);
        necessary_sp->setPosition(Point(140, 58));
        this->addChild(necessary_sp, Z_Need_Sprite, T_Need_Sprite);
        
        //アイテム画像
        auto item_sp = Sprite::create(item_path);
        item_sp->setScale(0.72);
        item_sp->setPosition(Point(30, 25));
        necessary_sp->addChild(item_sp);
        
        //文字
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument("×", FONT_NAME_SYSTEM, 28);
        label->setPosition(Point(65, 25));
        label->enableOutline(Color4B(0, 0, 0, 255), 3);
        necessary_sp->addChild(label);
        
        auto label_count = HMSFontUtil::createWithTTFBySystemFontArgument(_count, FONT_NAME_SYSTEM, 28);
        label_count->setAnchorPoint(Point(1.0, 0.5));
        label_count->setPosition(Point(184, 25));
        label_count->enableOutline(Color4B(0, 0, 0, 255), 3);
        necessary_sp->addChild(label_count);
    }
}

void TournamentEntryListSprite::checkEntryTime()
{
    auto menuItem = (MenuItem *)this->getChildByTag(T_Entry_Menu)->getChildByTag(T_Entry_MenuItem);
    
    //現在時刻を端末時間からサーバーとの差の時間を引いたものに設定し取得
    time_t now_time;
    time(&now_time);
    now_time = now_time - SaveData::loadInt("server_diff_time");
    
    time_t start_date_timestamp = fromString<time_t>(_start_date_timestamp);
    time_t tournament_entry_interval = fromString<time_t>(SaveData::loadString("tournament_entry_interval"));
    
    time_t entry_start_timestamp = start_date_timestamp - tournament_entry_interval;
    
    if (entry_start_timestamp < now_time && now_time <= start_date_timestamp) {
        menuItem->unselected();
        setEntryTimeFlg(true);
        blinkingAction();
    }
}

void TournamentEntryListSprite::blinkingAction()
{
    //エントリー時間になったら光らせる処理（白画像を上に重ねて透過率を変更している）
    auto white_sp = (Sprite *)this->getChildByTag(T_Entry_White);
    
    auto fade1 = FadeTo::create(0.4f, 80);
    auto ease1 = EaseIn::create(fade1, 1);
    
    auto fade2 = FadeOut::create(0.2f);
    auto ease2 = EaseOut::create(fade2, 1);
    
    white_sp->runAction(Repeat::create(Sequence::create(ease1, DelayTime::create(0.2f), ease2, DelayTime::create(1.0f), nullptr), -1));
}

void TournamentEntryListSprite::blinkingStopAction()
{
    //光るアクションを止める
    auto white_sp = (Sprite *)this->getChildByTag(T_Entry_White);
    white_sp->stopAllActions();
    white_sp->setOpacity(0);
}