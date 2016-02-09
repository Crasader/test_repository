#include "TeamBattleEventEntryListSprite.h"
#include "TeamBattleEventRewardSprite.h"
#include "NormalScene.h"

TeamBattleEventEntryListSprite::TeamBattleEventEntryListSprite()
:_reception_time()
,_start_date_timestamp()
,_entry_time_flg(false)
{
}

TeamBattleEventEntryListSprite* TeamBattleEventEntryListSprite::create(json11::Json json, Node* node_)
{
    TeamBattleEventEntryListSprite *sprite = new TeamBattleEventEntryListSprite();
    if (sprite && sprite->init(json, node_))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TeamBattleEventEntryListSprite::init(json11::Json json, Node* node_)
{
    if ( !Sprite::initWithFile("init/Home/Event/TournamentEvent/bk_tournament6.png") )
    {
        return false;
    }
    
    //データの設定
    setupData(json);
    
    node = node_;
    
    return true;
}

void TeamBattleEventEntryListSprite::setupData(json11::Json json)
{
    //データの設定
    setTeamBattleEventListId(json["team_battle_event_list_id"].string_value());
    setTeamBattleName(json["name"].string_value());
    setReceptionTime(json["reception_time"].string_value());
    setStartDateTimestamp(json["start_date_timestamp"].string_value());
    setRewardDataList(json["reward_data_list"].object_items()) ;
}

void TeamBattleEventEntryListSprite::onEnter()
{
    Sprite::onEnter();
    
    //チーム対抗戦名の作成
    createTeamBattleName();

    //受付期間の表示
    showReceptionTime();
    
    //報酬ボタンの作成
    createRewardButton();
    
    //チーム対抗戦参加ボタンの作成
    createEntryButton();
    
    //チーム対抗戦エントリー時間かチェック
    checkEntryTime();
}

void TeamBattleEventEntryListSprite::createTeamBattleName()
{
    //チーム対抗戦名の作成
    auto team_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_team_battle_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_team_battle_name, 34, 400));
    team_name_label->setColor(Color3B(255, 234, 0));
    team_name_label->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height - 30));
    team_name_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(team_name_label, Z_Team_Battle_Name, T_Team_Battle_Name);
}

void TeamBattleEventEntryListSprite::showReceptionTime()
{
    //受付期間の表示
    auto event_open_label = HMSFontUtil::createWithTTFBySystemFontArgument(_reception_time, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_reception_time, 28, 375)); //TODO calcFlexibleFontSizeの中の変数だけ適応に変えた要確認
    event_open_label->setAnchorPoint(Point(0, 0.5));
    event_open_label->setPosition(Point(186, this->getContentSize().height - 162));
    event_open_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(event_open_label, Z_Event_Open_Label, T_Event_Open_Label);
}

void TeamBattleEventEntryListSprite::setConditionOver()
{
    //チーム対抗戦参加条件に満たなかった時の設定
    //参加ボタンを暗くする
    auto entry_btn = (MenuItemSprite *)this->getChildByTag(T_Entry_Menu)->getChildByTag(T_Entry_MenuItem);
    entry_btn->selected();
    
    //参加条件の文字列を赤く点滅させる
    auto condition_label = (Label *)this->getChildByTag(T_Condition_Label);
    auto tint1 = TintTo::create(0.5f, 255, 130, 130);
    auto tint2 = TintTo::create(0.5f, 255, 255, 255);
    condition_label->runAction(RepeatForever::create(Sequence::create(tint1, tint2, nullptr)));
}

void TeamBattleEventEntryListSprite::createRewardButton()
{
    //報酬ボタンの作成
    auto reward_off = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    auto reward_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    reward_on->setColor(Color3B(180, 180, 180));
    
    auto reward_btn = MenuItemSprite::create(reward_off, reward_on,[this](Ref* sender){
        
        //報酬のスプライトを生成しvectorに入れる
        HmsVector<Sprite *> item_list ;
        
        //勝ち
        TeamBattleEventRewardSprite *reward_sprite_win = TeamBattleEventRewardSprite::create(_reward_data_list["win"]);
        item_list.push_back(reward_sprite_win);
        
        //負け
        TeamBattleEventRewardSprite *reward_sprite_lose = TeamBattleEventRewardSprite::create(_reward_data_list["lose"]);
        item_list.push_back(reward_sprite_lose);
       
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
        node->getParent()->addChild(popup_layer, TeamBattleEventEntryListLayer::Z_Popup_Layer, TeamBattleEventEntryListLayer::T_Popup_Layer);
        
    });
    reward_btn->setTag(T_Reward_MenuItem);
    
    auto menu = Menu::create(reward_btn, nullptr);
    menu->setPosition(Point(330, 58));
    this->addChild(menu, Z_Reward_Menu, T_Reward_Menu);
}

void TeamBattleEventEntryListSprite::createEntryButton()
{
    //エントリーボタンの作成
    auto entry_off = Sprite::create("init/Home/Event/TournamentEvent/bt_entry.png");
    auto entry_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_entry.png");
    entry_on->setColor(Color3B(180, 180, 180));
    
    auto entry_btn = MenuItemSprite::create(entry_off, entry_on,[this](Ref* sender){
        
        auto menuItem = (MenuItem *)sender;
        
        if(getEntryTimeFlg() == false) {
            //エントリー時間になっていない場合
            auto func = [](){};
            
            string comment = "エントリー受付期間に\nなっておりません。";
            auto popup_layer = PopupOkButtonLayer::create(func, comment);
            node->addChild(popup_layer, TournamentListLayer::Z_Popup_Layer, TournamentListLayer::T_Popup_Layer);
            
            menuItem->selected();
        }
        else {
            //エントリー中だったら
            if(SaveData::loadString("team_battle_event_entry")=="1"){
                if(getTeamBattleEventListId()==SaveData::loadString("team_battle_event_list_id")){
                    auto normal_scene = (NormalScene *)node->getParent();
                    normal_scene->changePage("TournamentEventBattleWait");
                }else{
                    //別のチーム対抗戦に参加しようとした場合
                    auto func = [](){};
                    
                    string comment = "他のチーム対抗戦に\nエントリーしています。";
                    auto popup_layer = PopupOkButtonLayer::create(func, comment);
                    node->addChild(popup_layer, TournamentListLayer::Z_Popup_Layer, TournamentListLayer::T_Popup_Layer);
                    
                    menuItem->selected();
                    
                }
            }else{
                //選択したチーム対抗戦イベントのデータを保存
                SaveData::saveString("team_battle_event_list_id", _team_battle_event_list_id);
            
                //チーム対抗戦終了時刻の設定
                NormalScene::_team_battle_next_battle_time = fromString<time_t>(_start_date_timestamp);
                
                //エントリー確認ポップアップ
                auto ok_func = [this](){
                    auto parent = (TeamBattleEventEntryListLayer*)this->node;
                    parent->entryFinish(this->_team_battle_event_list_id);
                };
                auto ng_func = [](){}; //いいえを押したとき何もしない
                popupConfirmLayer *layer = popupConfirmLayer::create("このチーム対抗戦にエントリーします。\nよろしいですか？");
                layer->setFunction(ok_func, ng_func);
                
//                auto normal_scene = (NormalScene *)_normal_scene;
                node->addChild(layer, 100);
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

void TeamBattleEventEntryListSprite::checkEntryTime()
{
    auto menuItem = (MenuItem *)this->getChildByTag(T_Entry_Menu)->getChildByTag(T_Entry_MenuItem);
    
    //現在時刻を端末時間からサーバーとの差の時間を引いたものに設定し取得
    time_t now_time;
    time(&now_time);
    now_time = now_time - SaveData::loadInt("server_diff_time");
    
    time_t start_date_timestamp = fromString<time_t>(_start_date_timestamp);
    time_t team_battle_entry_interval = fromString<time_t>(SaveData::loadString("team_battle_entry_interval"));
    
    time_t entry_start_timestamp = start_date_timestamp - team_battle_entry_interval;
    
    if (entry_start_timestamp < now_time && now_time <= start_date_timestamp) {
        menuItem->unselected();
        setEntryTimeFlg(true);
        blinkingAction();
    }
}

void TeamBattleEventEntryListSprite::blinkingAction()
{
    //エントリー時間になったら光らせる処理（白画像を上に重ねて透過率を変更している）
    auto white_sp = (Sprite *)this->getChildByTag(T_Entry_White);
    
    auto fade1 = FadeTo::create(0.4f, 80);
    auto ease1 = EaseIn::create(fade1, 1);
    
    auto fade2 = FadeOut::create(0.2f);
    auto ease2 = EaseOut::create(fade2, 1);
    
    white_sp->runAction(Repeat::create(Sequence::create(ease1, DelayTime::create(0.2f), ease2, DelayTime::create(1.0f), nullptr), -1));
}

void TeamBattleEventEntryListSprite::blinkingStopAction()
{
    //光るアクションを止める
    auto white_sp = (Sprite *)this->getChildByTag(T_Entry_White);
    white_sp->stopAllActions();
    white_sp->setOpacity(0);
}