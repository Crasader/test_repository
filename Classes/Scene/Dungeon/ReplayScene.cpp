#include "ReplayScene.h"
#include "cocos-ext.h"
#include "SimpleAudioEngine.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "Chara.h"
#include "Effect.h"
#include "SkillConfirmLayer.h"
#include "SkillErrorLayer.h"
#include "ReplaceConfirmLayer.h"
#include "BackgroundLayer.h"
#include "UILayer.h"
#include "FieldLayer.h"
#include "PassiveSkill.h"
#include "PassiveSkillEffectLayer.h"
#include "ComboEffectLayer.h"
#include "ActiveBuffer.h"
#include "PassiveBuffer.h"
#include "UIEffectLayer.h"
#include "TutorialTipLayer.h"
#include "TutorialNaviLayer.h"
#include "TurnManage.h"
#include "CharaList.h"

Scene* ReplayScene::createScene()
{
    HMSLOG("ReplayScene");
    auto scene = Scene::create();
    auto layer = ReplayScene::create();
    scene->addChild(layer);
    return scene;
}

bool ReplayScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("ReplayScene::init()");
    
    //メモリのキャッシュクリア
    Director::getInstance()->purgeCachedData();
    
    TurnManage::initialize();
    CharaList::initialize();
    
    return true;
}

void ReplayScene::onEnter()
{
    Layer::onEnter();
    ratio = getVisibleRatio();
    
    
    replay_flg = true;
    SaveData::saveString("battle_mode", "vs");
    
    getReplayData();
}


void ReplayScene::getReplayData()
{
    battle_list_id = fromString<int>(SaveData::loadString("battle_list_id_for_replay"));
    string user_master_id = SaveData::loadString("user_master_id");

    string url = HOST;
    url += "battle/get_replay_data.php?";
    url += "user_master_id=" + user_master_id;
    url += "&battle_list_id=" + toString(battle_list_id);
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        getReplayDataCallback(sender, response);
    });
}

void ReplayScene::getReplayDataCallback(HttpClient *sender, HttpResponse *response)
{
    if(response->isSucceed()==false){//通信失敗したら
        CCLOG("通信失敗");
        runAction(Sequence::create(DelayTime::create(1.5f),CallFunc::create([this](){//少し時間をおき
            auto popup = PopupOk::create();
            popup->setPopup("タイムアウトしました。再送信します。",[this](){//ポップアップ出して再送信
                getReplayData();//再送信
            });
            addChild(popup,10000001);
        }), NULL));
        return;
    }
    
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("result=%s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    string data_text = json["data_text"].string_value();
    json_master = Json::parse(data_text, err);
    //HMSLOG("ahead_no=%d", json_master["ahead_no"].int_value());
    HMSLOG("json_master=%s", json_master.dump().c_str());
    
    if(json_master["other_flg"].int_value()==1) {
        replay_other_flg = true;
    }
    
    if(json_master["favorite_flg"].int_value()==1) {
        replay_favorite_flg = true;
    }
    
    //data_text = json["battle_log_list"].string_value();
    //json_battle_log = Json::parse(data_text, err);
    json_battle_log = json["battle_log_list"].object_items();
    count_battle_log = countJsonData(json_battle_log);
    HMSLOG("count_battle_log=%d", count_battle_log);
    
    string url = HOST;
    url += "battle/get_skill_data.php?";
    url += "battle_list_id=" + toString(battle_list_id);
    //auto selector = httpresponse_selector(ReplayScene::getReplayDataCallback);
    //auto selector = httpresponse_selector(ReplayScene::getReplayDataCallback2);
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        getReplayDataCallback2(sender,response);
    });
}

void ReplayScene::getReplayDataCallback2(HttpClient *sender, HttpResponse *response)
{
    getDungeonDataCallback2(sender, response);
    
    
    //リプレイ専用
    Sprite *img = Sprite::create("init/Home/Quest/replay_shadow.png") ;
    img->setScale(ratio) ;
    img->setAnchorPoint(Point(0.5, 0.5)) ;
    img->setPosition(convertFromDisplaySize(Point(320, 907), 0.5, 1.0));
    this->addChild(img, 20);
    
    //戻る
    finish_button = Sprite::create("init/Home/Quest/bt_replay_return.png") ;
    finish_button->setScale(ratio) ;
    finish_button->setAnchorPoint(Point(0.5, 0.5)) ;
    finish_button->setPosition(convertFromDisplaySize(Point(56, 907), 0.5, 1.0));
    this->addChild(finish_button, 21, REPLAY_FINISH_BUTTON_TAG);
    
    if(replay_other_flg==false) {
        //お気に入りボタン
        string filename = "init/Home/Quest/bt_favorite.png";
        if(replay_favorite_flg==true) filename = "init/Home/Quest/bt_favorite_delete.png";
        
        favorite_button = Sprite::create(filename.c_str()) ;
        favorite_button->setScale(ratio) ;
        favorite_button->setAnchorPoint(Point(0.5, 0.5)) ;
        favorite_button->setPosition(convertFromDisplaySize(Point(205, 907), 0.5, 1.0));
        this->addChild(favorite_button, 21, REPLAY_FAVORITE_BUTTON_TAG);
    }
    
    //残りターン数
    /*
     Sprite *turn_num_bg = Sprite::create("init/Home/Quest/bt_replay_return.png") ;
     turn_num_bg->setScale(ratio) ;
     turn_num_bg->setAnchorPoint(Point(1, 1)) ;
     turn_num_bg->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH-10, DISPLAY_HEIGHT-10), 1.0, 1.0));
     this->addChild(turn_num_bg, 21, REPLAY_TURN_NUM_TAG);
     */
    
    act_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString(count_battle_log), FONT_NAME_SYSTEM, 40);
    act_label->setAlignment(TextHAlignment::CENTER);
    act_label->setTextColor(Color4B::WHITE);
    act_label->setPosition(convertFromDisplaySize(Point(520, 888), 0.5, 1.0));
    act_label->setScale(ratio);
    act_label->enableOutline(Color4B::BLACK, 2);
    this->addChild(act_label, 21, REPLAY_TURN_NUM_TAG);
    
    auto turn_num_label = HMSFontUtil::createWithTTFBySystemFontArgument("リプレイ終了まで", FONT_NAME_SYSTEM, 24);
    turn_num_label->setAlignment(TextHAlignment::CENTER);
    turn_num_label->setTextColor(Color4B::WHITE);
    turn_num_label->setPosition(convertFromDisplaySize(Point(532, 930), 0.5, 1.0));
    turn_num_label->setScale(ratio);
    turn_num_label->enableOutline(Color4B::BLACK, 2);
    this->addChild(turn_num_label, 21);
    
    turn_num_label = HMSFontUtil::createWithTTFBySystemFontArgument("あと", FONT_NAME_SYSTEM, 24);
    turn_num_label->setAlignment(TextHAlignment::CENTER);
    turn_num_label->setTextColor(Color4B::WHITE);
    turn_num_label->setPosition(convertFromDisplaySize(Point(464, 888), 0.5, 1.0));
    turn_num_label->setScale(ratio);
    turn_num_label->enableOutline(Color4B::BLACK, 2);
    this->addChild(turn_num_label, 21);
    
    turn_num_label = HMSFontUtil::createWithTTFBySystemFontArgument("ACT", FONT_NAME_SYSTEM, 30);
    turn_num_label->setAlignment(TextHAlignment::CENTER);
    turn_num_label->setTextColor(Color4B::WHITE);
    turn_num_label->setPosition(convertFromDisplaySize(Point(590, 888), 0.5, 1.0));
    turn_num_label->setScale(ratio);
    turn_num_label->enableOutline(Color4B::BLACK, 2);
    this->addChild(turn_num_label, 21);

    this->schedule(schedule_selector(ReplayScene::updateForReplay), 2.0f);
}

void ReplayScene::updateForReplay(float dt)
{
    if(replay_pause_flg==true) return;
    
    //終了判定
    if(index_battle_log>=count_battle_log) {
        HMSLOG("終了");
        return;
    }
    
    //state,vs_state,data_exec_flgなどでコマンド実行可能かどうかを判定する
    string vs_state = getVsState();
    HMSLOG("updateForReplay() start. state=%s, vs_state=%s", state.c_str(), vs_state.c_str());
    if(vs_state=="regist") {
        int ahead_no = json_master["ahead_no"].int_value();
        setTurn(ahead_no);
        data_recovery_flg = true;
    } else if(vs_state=="ok") {
        
    } else {
        return;
    }

    
    if(getState()!="Operationing") return;
    if(cpu_moving==true) return;
    if(data_exec_flg==true) return;
    
    //以下コマンド実行
    Json json_log = json_battle_log[toString(index_battle_log)].object_items();
    HMSLOG("index_battle_log=%d, json_log=%s", index_battle_log, json_log.dump().c_str());
    
    string err;
    Json json = Json::parse(json_log["command_text"].string_value(), err);
    
    int command_turn_no = json["player_no"].int_value();
    
    //実行しようとするコマンドのturn_noと現在のturn_noが異なる場合、setTurnを実施する。
    if(command_turn_no!=TurnManage::getTurnNo()) {
        cpu_moving = true;
        setTurn(command_turn_no);
        return;
    }
    
    //敵のコマンドの場合、各種実行を行う。
    data_send_flg = false;//通信相手にデータを送るかどうか→送らない
    //cpu_moving = true;
    runBattleData(json_log["command_text"].string_value());
    
    
    index_battle_log++;
    
    //残りターン数の更新
    //Label *turn_num_label = (Label *)this->getChildByTag(REPLAY_TURN_NUM_TAG);
    //Point pos = turn_num_label->getPosition();
    removeChild(act_label);
    act_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString(count_battle_log-index_battle_log), FONT_NAME_SYSTEM, 40);
    act_label->setAlignment(TextHAlignment::CENTER);
    act_label->setTextColor(Color4B::WHITE);
    act_label->setPosition(convertFromDisplaySize(Point(520, 888), 0.5, 1.0));
	act_label->setScale(ratio);
    act_label->enableOutline(Color4B::BLACK, 2);
    this->addChild(act_label, 21, REPLAY_TURN_NUM_TAG);
}

void ReplayScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event){
    CCLOG("test");
    
    //リプレイ時の終了ボタン判定
    //Sprite *finish_button = (Sprite *)this->getChildByTag(REPLAY_FINISH_BUTTON_TAG);
    if(finish_button!=nullptr) {
        Rect rect = finish_button->getBoundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            HMSLOG("replay finish");
            
            auto ok_func = [this](){
                //キャンセル音
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
                
                //スケジュールの停止
                unscheduleAllCallbacks();
                
                //ページ遷移の処理
                string change_page_name = "ReplayList";
                if (SaveData::loadBool("tournament_replay_flg")) change_page_name = "TournamentEventTop";
                
                auto scene = NormalScene::createScene(change_page_name);
                auto transition = TransitionFade::create(3.0f, scene);
                Director::getInstance()->replaceScene(transition);
                //((NormalScene *)scene)->changePage("QuestMenu");
            };
            
            auto ng_func = [this](){
                replay_pause_flg = false;
                
            };
            
            replay_pause_flg = true;
            popupConfirmLayer *layer = popupConfirmLayer::create("リプレイを終了しますか？");
            layer->setFunction(ok_func, ng_func);
            this->addChild(layer, 20);
        }
    }
    //リプレイ時のお気に入りボタン判定
    //Sprite *favorite_button = (Sprite *)this->getChildByTag(REPLAY_FAVORITE_BUTTON_TAG);
    if(favorite_button!=nullptr) {
        Rect rect = favorite_button->getBoundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            HMSLOG("replay favorite button");
            
            string message = "お気に入りに登録しますか？";
            if(replay_favorite_flg==true) message = "お気に入りから削除しますか？";
            popupConfirmLayer *layer = popupConfirmLayer::create(message);
            layer->setFavoriteFlg(replay_favorite_flg, toString(battle_list_id));
            
            auto ok_func = [this, layer](){
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
                layer->favoriteRequest();
            };
            
            auto ng_func = [this](){
                
            };
            
            layer->setFunction(ok_func, ng_func);
            this->addChild(layer, 20);
        }
    }

    //DungeonScene::onTouchEnded(touch, event);
}
