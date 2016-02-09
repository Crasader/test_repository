#include "TeamBattleEventBattleWaitLayer.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "json11.hpp"
#include "HeaderLayer.h"

bool TeamBattleEventBattleWaitLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamBattleEventBattleWaitLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamBattleEventBattleWaitLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チーム対抗戦待機", "TeamBattleEventTop", this);
    
    //砂時計の作成
    createSandglass();
    
    //残り時間テキスト
    createWaitingLabel();
    
    //待機通信処理の開始
    scheduleOnce(schedule_selector(TeamBattleEventBattleWaitLayer::waitBattle), 0);
    
    //スケジューラーの開始
    schedule(schedule_selector(TeamBattleEventBattleWaitLayer::updateWaitingLabel), 1.0f);
}

void TeamBattleEventBattleWaitLayer::createSandglass()
{
    //砂時計画像の作成
    auto glass_sp = Sprite::create("init/Home/Event/TournamentEvent/base.png");
    glass_sp->setScale(ratio);
    glass_sp->setPosition(convertFromDisplaySize(Point(320, 520), 0.5, 0.5));
    this->addChild(glass_sp, Z_Glass, T_Glass);
    
    auto sand_sp = Sprite::create("init/Home/Event/TournamentEvent/san.png");
    sand_sp->setPosition(glass_sp->getContentSize() / 2);
    glass_sp->addChild(sand_sp, 1);
    
    auto glass_hi_sp = Sprite::create("init/Home/Event/TournamentEvent/hi.png");
    glass_hi_sp->setPosition(glass_sp->getContentSize() / 2);
    glass_sp->addChild(glass_hi_sp, 2);
    
    //回転アクション
    auto rotate = RotateBy::create(1.0f, 360);
    auto ease = EaseBackInOut::create(rotate);
    auto seq = Sequence::create(DelayTime::create(5.0f), ease, nullptr);
    auto repeat = Repeat::create(seq, -1);
    glass_sp->runAction(repeat);
}

void TeamBattleEventBattleWaitLayer::createWaitingLabel()
{
    waiting_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 40);
    waiting_label->setPosition(convertFromDisplaySize(Point(320, 330), 0.5, 0.5));
    waiting_label->setScale(ratio);
    waiting_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(waiting_label, Z_Waiting_Label, T_Waiting_Label);
    
    remain_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 50);
    remain_label->setScale(ratio);
    remain_label->setColor(Color3B(255, 255, 80));
    remain_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    remain_label->setPosition(convertFromDisplaySize(Point(320, 230), 0.5, 0.5));
    this->addChild(remain_label, Z_Waiting_Time_Label, T_Waiting_Time_Label);
}

void TeamBattleEventBattleWaitLayer::waitBattle(float dt)
{
    //対戦待機通信処理（ループ）
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/team_battle_event/top_scene/waiting_team_battle.php";
    string body = "user_master_id=" + user_master_id;
    body += "&team_battle_event_list_id=" + SaveData::loadString("team_battle_event_list_id");
    
    HttpConnectHms::sendHttpRequest(url,[this](HttpClient *client, HttpResponse *response){
        waitBattleCallback(client, response);
    },body);
}

void TeamBattleEventBattleWaitLayer::waitBattleCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    vector<char> vec_char = *response->getResponseData();
    string str = string(vec_char.begin(),vec_char.end());
    HMSLOG("response:%s", str.c_str());
    
    string err;
    Json json = Json::parse(str, err);
    
    //通信エラー以外
    if(response->isSucceed()) {
        
        //待機状態の取得
        _waiting_text = json["waiting_text"].string_value();
        _waiting_status = (WaitingStatus)fromString<int>(json["waiting_status"].string_value());
        
        switch (_waiting_status) {
            case No_Data:
            {
                //現在時刻の取得
                time_t now_time;
                time(&now_time);
                now_time = now_time - SaveData::loadInt("server_diff_time");
                
                _wait_remain_time = fromString<time_t>(json["wait_start_date_timestamp"].string_value()) - now_time;
                
                //トーナメント作成待ち状態、再帰処理
                scheduleOnce(schedule_selector(TeamBattleEventBattleWaitLayer::waitBattle), 1.0f);
                
            }break;
            case Next_Waiting:
            {
                //現在時刻の取得
                time_t now_time;
                time(&now_time);
                now_time = now_time - SaveData::loadInt("server_diff_time");
                
                _wait_remain_time = fromString<time_t>(json["wait_start_date_timestamp"].string_value()) - now_time;
                
                //次戦対戦待ち状態
                scheduleOnce(schedule_selector(TeamBattleEventBattleWaitLayer::waitBattle), 1.0f);
                
            }break;
            case Exception:
            {
                //ポップアップ処理
                auto func = [&](){
                    auto normal_scene = (NormalScene *)this->getParent();
                    normal_scene->changePage("TeamBattleEventTop");
                };
                
                string comment = _waiting_text;
                auto popup_layer = PopupOkButtonLayer::create(func, comment);
                this->addChild(popup_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
            }break;
            case Battle_Start:
            {
                //タップ禁止レイヤー生成
                auto normal_scene = (NormalScene *)this->getParent();
                AllTapDisableLayer *allTapDisaleLayer = AllTapDisableLayer::create();
                normal_scene->addChild(allTapDisaleLayer,8,ALL_TAP_DISABLE_LAYER_TAG);
                
                //時間になったらチーム対抗戦開始！
                int battle_list_id = fromString<int>(json["battle_list_id"].string_value());
                string enemy_user_master_id = json["enemy_user_master_id"].string_value();
                string tournament_event_list_id = SaveData::loadString("tournament_event_list_id");
                CCLOG("battle_list_id = %d, enemy_user_master_id = %s, tournament_event_list_id = %s", battle_list_id, enemy_user_master_id.c_str(), tournament_event_list_id.c_str());
                
                //対戦相手が来たらDungeonSceneへ移行(遷移の処理はTournamentEventBracketLayer)
                Utility::setOnlineBattleData(battle_list_id, enemy_user_master_id);
                
                SaveData::saveString("bracket_type", "now_battle") ;
                normal_scene->changePageFadeAction("TournamentEventBracket");
            }break;
        }
    }
}

void TeamBattleEventBattleWaitLayer::updateWaitingLabel(float dt)
{
    //待機テキストの更新処理
    if (_waiting_text != "") {
        string text = waiting_label->getString() + ".";
        if (_update_count % 4 == 0) text = _waiting_text;
        waiting_label->setString(text);
        
        _update_count++;
    }
    
    if (_waiting_status == Exception) {
        
        //ポップアップ表示の場合
        waiting_label = (Label *)this->getChildByTag(T_Waiting_Label);
        waiting_label->setString("");
        unschedule(schedule_selector(TeamBattleEventBattleWaitLayer::updateWaitingLabel));
    }
    
    if (_waiting_status == Next_Waiting) {
        
        //残り時間の更新
        if (_wait_remain_time >= 0) {
            //残り時間文字列の作成
            string remain_text = "あと";
            
            int minute = (int)_wait_remain_time / 60; //分
            int second = (int)_wait_remain_time % 60; //秒
            
            if (minute > 0) remain_text += toString<int>(minute) + "分";
            else if (second > 0) remain_text += toString<int>(second) + "秒";
            
            remain_label->setString(remain_text);
            
            _wait_remain_time--;
        }
    }
}
