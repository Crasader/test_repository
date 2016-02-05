#include "TeamBattleEventEntryTopLayer.h"
#include "TeamBattleEventRewardSprite.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "json11.hpp"
#include "HeaderLayer.h"

TeamBattleEventEntryTopLayer* TeamBattleEventEntryTopLayer::create(){
    TeamBattleEventEntryTopLayer *layer = new TeamBattleEventEntryTopLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void TeamBattleEventEntryTopLayer::createButton()
{
    //エントリー受付時間内　：「参加キャンセル」参加キャンセルポップアップ
    //エントリー受付時間終了：「出陣」待機画面に遷移
    string main_button_path = "";
    function<void(Ref*)> call_func;
    Color3B wait_btn_off_color = Color3B::WHITE;
    //エントリー受付時間内
    if(is_entry_time){
        //リーダー：参加キャンセルポップアップ
        if(is_leader){
            main_button_path = "init/Home/Event/TournamentEvent/bt_tournament_list.png";
            call_func = [this](Ref* sender){
                auto ok_func = [this](){
                    this->entryCancel();
                };
                auto ng_func = [](){};
                popupConfirmLayer *layer = popupConfirmLayer::create("エントリーをキャンセルします。\nよろしいですか？");
                layer->setFunction(ok_func, ng_func);
                this->addChild(layer, Z_Popup);
            };
        }
        //リーダー以外：ボタンを押しても反応がない
        else {
            main_button_path = "init/Home/Event/TournamentEvent/bt_tournament_list.png";
            call_func = [this](Ref* sender){/*アクションなし*/};
            wait_btn_off_color = Color3B(180, 180, 180); //ボタンを黒くして押せない用に見せる
        }
    }
    //エントリー受付時間外
    else {
        main_button_path = "init/Home/Event/TournamentEvent/bt_tournament_list.png";
        call_func = [this](Ref* sender){
            auto normal_scene = (NormalScene *)this->getParent()->getParent();
            normal_scene->changePage("TeamBattleEventBattleWait");
        };
    }
    auto wait_btn_on = Sprite::create(main_button_path);
    auto wait_btn_off = Sprite::create(main_button_path);
    wait_btn_on->setColor(Color3B(180, 180, 180));
    wait_btn_off->setColor(wait_btn_off_color);
    auto wait_btn = MenuItemSprite::create(wait_btn_off, wait_btn_on, call_func);
    wait_btn->setScale(ratio);
    auto wait_btn_menu = Menu::create(wait_btn, nullptr);
    wait_btn_menu->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(wait_btn_menu, Z_Button);
    
    //デッキ編成画面へのリンク
    auto deck_btn_on = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    auto deck_btn_off = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    deck_btn_on->setColor(Color3B(180, 180, 180));
    auto deck_btn = MenuItemSprite::create(deck_btn_off, deck_btn_on, [this](Ref* sender){
        auto normal_scene = (NormalScene *)this->getParent()->getParent();
        normal_scene->changePage("TeamBattleEventDeckChoice");
    });
    deck_btn->setScale(ratio);
    auto deck_btn_menu = Menu::create(deck_btn, nullptr);
    deck_btn_menu->setPosition(convertFromDisplaySize(Point(480, 300), 0.5, 0.5));
    this->addChild(deck_btn_menu, Z_Button);
    
    //報酬画面ポップアップ
    auto reward_off = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    auto reward_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_reward.png");
    reward_on->setColor(Color3B(180, 180, 180));
    auto reward_btn = MenuItemSprite::create(reward_off, reward_on,[this](Ref* sender){
        
        //報酬のスプライトを生成しvectorに入れる
        HmsVector<Sprite *> item_list ;
        
        //勝ち
        TeamBattleEventRewardSprite *reward_sprite_win = TeamBattleEventRewardSprite::create(team_battle_list_data["reward_data_list"]["win"]);
        item_list.push_back(reward_sprite_win);
        //負け
        TeamBattleEventRewardSprite *reward_sprite_lose = TeamBattleEventRewardSprite::create(team_battle_list_data["reward_data_list"]["lose"]);
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
        this->getParent()->getParent()->addChild(popup_layer, TeamBattleEventEntryListLayer::Z_Popup_Layer, TeamBattleEventEntryListLayer::T_Popup_Layer);
        
    });
    reward_btn->setScale(ratio);
    auto menu = Menu::create(reward_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(160, 300), 0.5f, 0.5f));
    this->addChild(menu, Z_Button);
}

void TeamBattleEventEntryTopLayer::createPage()
{
    
}

void TeamBattleEventEntryTopLayer::entryCancel()
{
    //チーム対抗戦参加通信処理
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/team_battle_event/top_scene/team_battle_cancel.php";
    auto selector = httpresponse_selector(TeamBattleEventEntryTopLayer::entryCancelCallback);
    string body = "user_master_id=" + user_master_id + "&team_id=" + toString(team_id) + "&team_battle_event_list_id=" + toString(team_battle_event_list_id);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TeamBattleEventEntryTopLayer::entryCancelCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //チーム対抗戦
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    Json cancel_data_list = json["cancel_data_list"].object_items();
    
    //キャンセルコメント出して画面遷移
    NormalScene* normal_scene = (NormalScene*)this->getParent()->getParent();
    auto ok_func = [normal_scene](){
        normal_scene->changePage("TeamBattleEventTopAgain");
    };
    auto popup_layer = PopupOkButtonLayer::create(ok_func, cancel_data_list["cancel_text"].string_value());
    normal_scene->addChild(popup_layer, Z_Popup);
}