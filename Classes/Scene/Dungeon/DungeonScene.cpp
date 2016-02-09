#include "DungeonScene.h"
#include "cocos-ext.h"
#include "SimpleAudioEngine.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsCrypto.h"
#include "Chara.h"
#include "Effect.h"
#include "SkillErrorLayer.h"
#include "BackgroundLayer.h"
#include "UILayer.h"
#include "FieldLayer.h"
#include "PassiveSkill.h"
#include "PassiveSkillEffectLayer.h"
#include "ReplaceConfirmLayer.h"
#include "ComboEffectLayer.h"
#include "ActiveBuffer.h"
#include "PassiveBuffer.h"
#include "UIEffectLayer.h"
#include "TutorialTipLayer.h"
#include "TutorialNaviLayer.h"
#include "RankingEventResultLayer.h"
#include "ChallengeEventResultLayer.h"
#include "BossEventResultLayer.h"
#include "TournamentEventResultLayer.h"
#include "TurnManage.h"
#include "CharaList.h"
#include "PlistAnimationSprite.h"
#include "HttpConnectHms.h"
#include "ContinueLayer.h"
#include "UserCutInLayer.h"
#include "PopupOk.h"

#define HAND_SIZE 6

#define ZORDER_RESULT_MODAL_LAYER 19999
#define ZORDER_RESULT_LAYER 20000

#define TUTORIAL_TIP_LAYER_TAG_ID 999998
#define TUTORIAL_TIP_LAYER_Z 999998

#define TUTORIAL_NAVI_LAYER_Z 999999

Scene* DungeonScene::createScene()
{
    auto scene = Scene::create();
    auto layer = DungeonScene::create();
    scene->addChild(layer);
    return scene;
}

bool DungeonScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("DungeonScene::init()");
    
    if(isTutorial()){
        //BGM再生
        this->runAction(Sequence::create(DelayTime::create(1.6f),
                        PlayBGM::create("download/BGM/07_battle.mp3"),
                        FadeToVolumeOfBGM::create(0.0f, SaveData::loadFloat("bgm_volume")),
                        CallFunc::create([](){HMSLOG("volume:%f",SimpleAudioEngine::getInstance()->getBackgroundMusicVolume());}),nullptr)
                        );
        //ダンジョンチュートリアル
        TutorialNaviLayer::initCompleteData();
    }
    
    //バックグラウンドフラグを初期化
    SaveData::saveBool("background_flg", false);
    
    //次回マイページ遷移時にデイリーミッションの判定を行うフラグ
    SaveData::saveBool("daily_mission_check_triger", true) ;
    
    //イベントランクデータの初期化（イベントバトル以外のときイベントランクレベルを0に初期化）
    if (!isEventBattle()) SaveData::saveString("event_rank_level", "0");
    
    //メモリのキャッシュクリア
    Director::getInstance()->purgeCachedData();
	
    TurnManage::initialize();
    CharaList::initialize();
    
    return true;
}

void DungeonScene::onEnter()
{
    Layer::onEnter();
    ratio = getVisibleRatio();
    
    if (isTournamentEventBattle()) {
        //トーナメントイベントオンラインバトルデータの設定
        battle_list_id = SaveData::loadInt("battle_list_id");
        getDungeonData();
        return;
    }
    
    if(getBattleMode()=="vs") {
        battle_list_id = SaveData::loadInt("battle_list_id");
        
        //背景
        auto bg = Sprite::create("init/Home/Mypage/back.png");
        bg->setScale(ratio);
        bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        bg->setCascadeOpacityEnabled(true);
        this->addChild(bg, 1, 2);
        
        checkBattleStart();
    } else {
        //初期ダンジョンデータ取得通信
        getDungeonData();
    }
    
    LoadingManager::setLoading(this);//ローディング作成
}

void DungeonScene::checkBattleStart()
{
    string url = HOST;
    url += "battle/check_battle_start.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    url += "&battle_list_id=" + toString(battle_list_id);
    
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *cliant, HttpResponse *response){
        checkBattleStartCallback(cliant, response);
    });
}

void DungeonScene::checkBattleStartCallback(HttpClient *sender, HttpResponse *response)
{
    if(response->isSucceed()==false){//通信失敗したら
        CCLOG("通信失敗");
        runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this](){//少し時間をおき
            
            //auto popup = PopupOk::create();
            //popup->setPopup("タイムアウトしました。再送信します。",[this](){//ポップアップ出して再送信
                checkBattleStart();//再送信
            //});
            //addChild(popup,10000001);
            
        }), NULL));
        
        return;
    }
    
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("result=%s", result.c_str());
    
    if(result=="wait") {
        DelayTime *delay = DelayTime::create(1.0f);
        CallFunc *expire1 = CallFunc::create([this]()
                                             {
                                                 this->checkBattleStart();
                                             });//ラムダ式
        this->runAction(Sequence::create(delay, expire1, NULL));
    } else if(result=="cancel") {
        
        auto popuplayer = SubMenuPopup::create("SubMenu");
        popuplayer->setupImage("対戦相手との接続が\n確立できませんでした。\nホーム画面に戻ります。");
        
        CallFunc *expire1 = CallFunc::create([this](){
            //ページ遷移の処理
            auto scene = NormalScene::createScene("");
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        });//ラムダ式
        
        this->addChild(popuplayer,10000001);
        runAction(Sequence::create(DelayTime::create(5.0f), expire1, NULL));
        
    } else if(result=="start"){
        
        this->removeChildByTag(2);

        //初期ダンジョンデータ取得通信
        getDungeonData();
        
    } else {
        
        auto popuplayer = SubMenuPopup::create("SubMenu");
        popuplayer->setupImage("対戦開始のための接続が\n確立できませんでした。\nホーム画面に戻ります。");
        
        CallFunc *expire1 = CallFunc::create([this](){
            //ページ遷移の処理
            auto scene = NormalScene::createScene("");
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        });//ラムダ式
        
        this->addChild(popuplayer,10000001);
        runAction(Sequence::create(DelayTime::create(5.0f), expire1, NULL));
    }
}

void DungeonScene::getDungeonData()
{
    int ic = SaveData::loadInt("ic");
    string ics = SaveData::loadString("ics");
    string checksum = HmsCrypto::makeCheckSum("hms" + toString(ic));
    
    if(ic>0 || ics!=checksum) {
        CCLOG("イリーガル１");
        CCLOG("ics::%s",ics.c_str());
        CCLOG("chs::%s",checksum.c_str());
        SaveData::saveInt("ic", 0);
        checksum = HmsCrypto::makeCheckSum("hms0");
        SaveData::saveString("ics", checksum);
        
        if(replay_flg==false) {
            string user_master_id = SaveData::loadString("user_master_id");
            string url = HOST;
            url += "battle/illegal.php";
            url += "?user_master_id=" + user_master_id;
            url += "&ic=" + toString(ic);
            auto selector = httpresponse_selector(DungeonScene::notifyIllegalCallback);
            HttpConnect::sendHttpRequest(this, url, selector);
        }
    }
    
    string restart_flg = "";
    if (SaveData::loadBool("battle_restart_flg")) {
        restart_flg = "true";
    }
    else{
        restart_flg = "false";
    }
    
    battle_list_id = 0;
    if(getBattleMode()=="vs") {
        battle_list_id = SaveData::loadInt("battle_list_id");
    }
    
    string url = HOST;
    if(isTutorial()){
        url += "battle/get_tutorial_dungeon_data.php";
    } else if(isEventBattle()) {
        url += "battle/get_event_dungeon_data.php";
    } else if(isQuestBattle()) {
        url += "battle/get_quest_dungeon_data.php";
    } else if(isRankingEventBattle()) {
        url += "battle/get_ranking_event_dungeon_data.php";
    } else if(isBossEventBattle()) {
        url += "battle/get_boss_event_dungeon_data.php";
    } else if(isChallengeEventBattle()) {
        url += "battle/get_challenge_event_dungeon_data.php";
    } else {
        url += "battle/get_dungeon_data.php";
    }

    string body = "user_master_id=" + SaveData::loadString("user_master_id");
    body += "&uid1=" + SaveData::loadString("user_master_id");
    body += "&uid2=" + SaveData::loadString("enemy_user_master_id");
	body += "&friend_id=" + SaveData::loadString("choice_friend_id");
	body += "&deck_no=" + SaveData::loadString("party_no");
	body += "&npci=" + SaveData::loadString("npc_battle_event_rank_list_id");
    body += "&dsi=" + SaveData::loadString("quest_stage_id");
    body += "&resi=" + SaveData::loadString("ranking_event_stage_id");
    body += "&besli=" + SaveData::loadString("boss_event_stage_log_id");
    body += "&cesi=" + SaveData::loadString("challenge_event_stage_id");
    body += "&restart_flg=" + restart_flg;
      
    if(isEventBattle()) {
        string id = SaveData::loadString("event_battle_result_log_id");
        if(id!="") {
            body += "&log_id=" + id;
            SaveData::saveString("event_battle_result_log_id", "");
        }
    } else if(isQuestBattle()) {
        string id = SaveData::loadString("quest_battle_result_log_id");
        if(id!="") {
            body += "&log_id=" + id;
            SaveData::saveString("quest_battle_result_log_id", "");
        }
    } else if(isRankingEventBattle()) {
        string id = SaveData::loadString("ranking_event_stage_log_id");
        if(id!="") {
            body += "&log_id=" + id;
            SaveData::saveString("ranking_event_stage_log_id", "");
        }
    } else if(isBossEventBattle()) {
        string id = SaveData::loadString("boss_event_battle_log_id");
        if(id!="") {
            body += "&log_id=" + id;
        }
    } else if(isChallengeEventBattle()) {
        string id = SaveData::loadString("challenge_event_stage_log_id");
        if(id!="") {
            body += "&log_id=" + id;
            SaveData::saveString("challenge_event_stage_log_id", "");
        }
    }
    
    if(getBattleMode()=="vs") {
        body += "&battle_list_id=" + toString<int>(battle_list_id);
    }
    
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *cliant, HttpResponse *response){
        if(response->isSucceed()){
            getDungeonDataCallback(cliant, response);
        }else{
            this->runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this](){//接続失敗したら時間おいて再送信
                getDungeonData();
            }), NULL));
        
        }
    },body);
    
}

void DungeonScene::getDungeonDataCallback(HttpClient *cliant, HttpResponse *response)
{
	string result = HttpConnectHms::getHttpResponse(this, cliant, response);
	
	string err;
	json_master = Json::parse(result, err);
	
    HMSLOG("アヘッドーー%d",json_master["ahead_no"].int_value());
    
    //リザルトログIDの保存
    SaveData::saveString("result_log_id", json_master["result_log_id"].string_value());
    
	string url = HOST;
    if(isTutorial()){
        url += "battle/get_tutorial_skill_data.php";
    } else if(isEventBattle()) {
        url += "battle/get_event_skill_data.php";
    } else if(isQuestBattle()) {
        url += "battle/get_quest_skill_data.php";
    } else if(isRankingEventBattle()) {
        url += "battle/get_ranking_event_skill_data.php";
    } else if(isBossEventBattle()) {
        url += "battle/get_boss_event_skill_data.php";
    } else if(isChallengeEventBattle()) {
        url += "battle/get_challenge_event_skill_data.php";
    } else {
    	url += "battle/get_skill_data.php";
    }
    
	string body = "&umi=" + SaveData::loadString("user_master_id");
	body += "&eumi=" + SaveData::loadString("enemy_user_master_id");
	body += "&ufi=" + SaveData::loadString("choice_friend_id");
	body += "&usdn=" + SaveData::loadString("party_no");
	body += "&npci=" + SaveData::loadString("npc_battle_event_rank_list_id");
    body += "&dsi=" + SaveData::loadString("quest_stage_id");
    body += "&resi=" + SaveData::loadString("ranking_event_stage_id");
    body += "&besli=" + SaveData::loadString("boss_event_stage_log_id");
    body += "&cesi=" + SaveData::loadString("challenge_event_stage_id");
	
	if(getBattleMode()=="vs") {
		body += "&battle_list_id=" + to_string(battle_list_id);
	}
    
    if(isBossEventBattle()) {
        string id = SaveData::loadString("boss_event_battle_log_id");
        body += "&log_id=" + id;
    }
	

    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        if(response->isSucceed()){
            getDungeonDataCallback2(sender, response);
        }else{
            this->runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this,sender, response](){//接続失敗したら時間おいて再送信
                getDungeonDataCallback(sender, response);
            }), NULL));
            
        }
    }, body);
}

void DungeonScene::getDungeonDataCallback2(HttpClient *sender, HttpResponse *response)
{
    HMSLOG("getDungeonDataCallback2");
    
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("result=%s", result.c_str());
    
    LoadingManager::removeLoading();//ローディング消去
    
    string err;
    json_skill_master = Json::parse(result, err);
    
//TurnManageの初期化
    if(getBattleMode()=="vs") {
        TurnManage::setPlayerIndex(json_master["player_no"].int_value() - 1);
    } else {
        TurnManage::setPlayerIndex(0);
    }
    
    //先攻・後攻判定
    if(getBattleMode()=="solo") {
        int ahead_no = json_master["ahead_no"].int_value();
        if(ahead_no==0){
            ahead_no = stoi(json_master["ahead_no"].string_value());//応急処置
        }
        ahead_no = ahead_no % 2 + 1;
        
        if(ahead_no==TurnManage::getPlayerNo()) {
            TurnManage::setTurnIndex(TurnManage::getPlayerIndex());
        } else {
            TurnManage::setTurnIndex(TurnManage::getEnemyIndex());
        }
        
        HMSLOG("player_no=%d, ahead_no=%d", TurnManage::getPlayerNo(), ahead_no);
    }
    
//レイヤー作成
    int area_battle_flg = fromString<int>(json_master["area_battle_flg"].string_value());
    backgroundLayer = BackgroundLayer::create(json_master["background_path"].string_value(), area_battle_flg);
    this->addChild(backgroundLayer,1);
    
    uiLayer = UILayer::create();
    this->addChild(uiLayer,2);
    
    fieldLayer=FieldLayer::create();
    this->addChild(fieldLayer,3);
    
    uiEffectLayer=UIEffectLayer::create();
    this->addChild(uiEffectLayer,4);
    
    cardLayer=CardLayer::create();//Charaを載せるレイヤー
    this->addChild(cardLayer,5);
    
    if(getBattleMode()!="vs" && isTutorial()==false) {
        chat_base_upper_layer = ChatBaseUpperLayer::create("DungeonScene");
        this->addChild(chat_base_upper_layer, 99);
    }
    
    if(isBossEventBattle()) {
        support_info_layer = SupportInfoLayer::create();
        this->addChild(support_info_layer,6);
    }
    
    activeSkillEffectLayer=ActiveSkillEffectLayer::create();//アクティブスキルの演出
    this->addChild(activeSkillEffectLayer,6);
    
    passiveSkillEffectLayer = PassiveSkillEffectLayer::create();//パッシブスキルの演出
    this->addChild(passiveSkillEffectLayer,7);
    
    comboEffectLayer = ComboEffectLayer::create();
    this->addChild(comboEffectLayer,8);
    
    startBattleEffectLayer = StartBattleEffectLayer::create();
    this->addChild(startBattleEffectLayer,9);
    
    effectLayer = Layer::create();
    this->addChild(effectLayer,10);
    
    damageLayer = Layer::create();
    this->addChild(damageLayer,11);
    
    dialogLayer = DialogLayer::create();
    this->addChild(dialogLayer,12);
    dialogLayer->setGlobalZOrder(100);//一番上
    
    activeSkill = ActiveSkill::create();//アクティブスキルの処理実行クラス
    this->addChild(activeSkill);
    
    passiveSkill = PassiveSkill::create();//全体パッシブスキルの処理実行クラス
    this->addChild(passiveSkill);
    
    activeBuffer = ActiveBuffer::create();//ステータスバッファ（アクティブスキルによる一時効果）処理実行）
    this->addChild(activeBuffer);
    
    passiveBuffer = PassiveBuffer::create();
    this->addChild(passiveBuffer);
	
	skillCheckLayer = SkillCheckLayer::create();
	this->addChild(skillCheckLayer , 10);
	
	skill_confirm_layer = SkillConfirmLayer::create();
	this->addChild(skill_confirm_layer,10);
	
    if(isTutorial()){
        tutorialNaviLayer = TutorialNaviLayer::create();
        this->addChild(tutorialNaviLayer, TUTORIAL_NAVI_LAYER_Z);
    }
    
    
    //初期化
    CCLOG("initialize()");
    //変数初期化
    tapped_chara = nullptr;
    
    //オンライン対戦時の降伏判定フラグを初期化
    SaveData::saveInt("OnlineBattleSurrenderFlg", 0);
    
    
//プレイヤー情報・キャラクター情報の初期化
    for(int ii=0; ii<2; ii++) {
        player[ii] = Player::create();
        addChild(player[ii]);
        
        string err;
        
        map<string, Json> json_player = json_master["player_data" + to_string(ii+1)].object_items();
        vector<Json> json_charas = json_player["card"].array_items();//キャラのベクターを取り出す
        
        for(int i=0; i<json_charas.size(); i++) {
            
            CCLOG("キャラNo.%d,%s",i,json_charas.at(i).dump().c_str());
            map<string, Json> json_chara = json_charas.at(i).object_items();
            Chara *chara = Chara::create();
            cardLayer->addChild(chara);
            
            
            if(i==0){//リーダーの時
                player[ii]->setMaxHp(json_chara["hp"].int_value());//最大HP
                player[ii]->setPower(json_chara["add_cost"].int_value());//上昇士気
                player[ii]->setPowerDefault(json_chara["add_cost"].int_value());//上昇士気デフォルト
                player[ii]->setHp(player[ii]->getMaxHp());
                if(isBossEventBattle()) {
                    //ボスイベントではサーバから最新のHPを受け取る。
                    player[ii]->setHp(json_chara["hp2"].int_value());//HP
                }
                player[ii]->setName(json_player["player_name"].string_value());
                player[ii]->setStartAddPower(json_player["start_add_cost"].int_value());
                
            }else if(json_chara["friend"].string_value()=="true"){//フレンドカードの時
                chara->setHandIndex(6);
                //chara->eliminate_flg = true;//フレンドカードは死んだら消去  入れる？？
            }else{//普通のカードの時
                deck[ii].push_back(chara);
            }
            chara->setBelongingIndex(ii);
            chara->setBelongingIndexDef(ii);
            
            chara->setPosition(convertFromDisplaySize(Point(0,-500), 0.5, 0.5));
            chara->setCharaId(json_chara["id"].int_value());
            chara->setCharaIdDef(json_chara["id"].int_value());
            
            chara->setPlaceCost(json_chara["need_cost"].int_value());
            chara->setPlaceCostDef(json_chara["need_cost"].int_value());
            
            chara->setBaseValue(json_chara["power"].int_value());
            chara->setBaseValueDef(json_chara["power"].int_value());
            
            chara->setCharaTurnCount(json_chara["turn_count"].int_value());
            chara->setCharaTurnCountDef(json_chara["turn_count"].int_value());
            
            chara->setType(json_chara["type"].int_value());
            chara->setTypeDef(json_chara["type"].int_value());
            
            chara->setCharaName(json_chara["name"].string_value());
            chara->setCharaNameDef(json_chara["name"].string_value());
            
            chara->setCharaSecondName(json_chara["second_name"].string_value());
            chara->setCharaSecondNameDef(json_chara["second_name"].string_value());
            
            chara->setRare(json_chara["rare"].int_value());
            chara->setRareDef(json_chara["rare"].int_value());
            
            
            if((isTutorial() || isEventBattle() || isQuestBattle()) && TurnManage::getEnemyIndex()==ii){
                chara->place_type = json_chara["pla_t"].string_value();
                chara->place_priority = json_chara["pla_pri"].int_value();
                chara->skill_priority = json_chara["ski_pri"].int_value();
            }
            
            Json json_active_skill = Json::parse(json_skill_master["active_skill"].dump(), err);
            Json json_passive_skill = Json::parse(json_skill_master["passive_skill"].dump(), err);
            int active_skill_data_count = json_skill_master["active_skill_data_count"].int_value();
            int passive_skill_data_count = json_skill_master["passive_skill_data_count"].int_value();
            
            //アクティブスキル持ちの場合
            for (int i=0; i < active_skill_data_count; i++) {
                if(json_chara["as_id"].int_value() == json_active_skill[to_string(i)]["active_skill_id"].int_value()) {
                    chara->setActiveSkillId(json_active_skill[to_string(i)]["skill_pattern_id"].int_value());
                    chara->setActiveSkillIdDef(json_active_skill[to_string(i)]["skill_pattern_id"].int_value());
                    chara->setActiveSkillName(json_active_skill[to_string(i)]["name"].string_value());
                    chara->setActiveSkillNameDef(json_active_skill[to_string(i)]["name"].string_value());
                    chara->setActiveSkillDetail(json_active_skill[to_string(i)]["detail"].string_value());
                    chara->setActiveSkillDetailDef(json_active_skill[to_string(i)]["detail"].string_value());
                    chara->setActiveSkillTarget(json_active_skill[to_string(i)]["target_type"].string_value());
                    chara->setActiveSkillTargetDef(json_active_skill[to_string(i)]["target_type"].string_value());
                    chara->setActiveSkillValue(json_active_skill[to_string(i)]["value"].int_value());
                    chara->setActiveSkillValueDef(json_active_skill[to_string(i)]["value"].int_value());
                    chara->setActiveSkillTurn(json_active_skill[to_string(i)]["turn_count"].int_value());
                    chara->setActiveSkillTurnDef(json_active_skill[to_string(i)]["turn_count"].int_value());
                    chara->setActiveSkillCost(json_active_skill[to_string(i)]["need_cost"].int_value());
                    chara->setActiveSkillCostDef(json_active_skill[to_string(i)]["need_cost"].int_value());
                    break;
                }
            }
            //パッシブスキル持ちの場合
            for (int i=0; i < passive_skill_data_count; i++) {
                if(json_chara["ps_id"].int_value() == json_passive_skill[to_string(i)]["passive_skill_id"].int_value()) {
                    chara->setPassiveSkillId(json_passive_skill[to_string(i)]["skill_pattern_id"].int_value());
                    chara->setPassiveSkillIdDef(json_passive_skill[to_string(i)]["skill_pattern_id"].int_value());
                    chara->setPassiveSkillName(json_passive_skill[to_string(i)]["name"].string_value());
                    chara->setPassiveSkillNameDef(json_passive_skill[to_string(i)]["name"].string_value());
                    chara->setPassiveSkillDetail(json_passive_skill[to_string(i)]["detail"].string_value());
                    chara->setPassiveSkillDetailDef(json_passive_skill[to_string(i)]["detail"].string_value());
                    chara->setPassiveSkillTarget(json_passive_skill[to_string(i)]["target_type"].string_value());
                    chara->setPassiveSkillTargetDef(json_passive_skill[to_string(i)]["target_type"].string_value());
                    chara->setPassiveSkillValue(json_passive_skill[to_string(i)]["value"].int_value());
                    chara->setPassiveSkillValueDef(json_passive_skill[to_string(i)]["value"].int_value());
                    break;
                }
            }
            
            chara->setCombinationL(json_chara["cmb_l"].int_value());
            chara->setCombinationR(json_chara["cmb_r"].int_value());
            chara->setCombinationLDef(json_chara["cmb_l"].int_value());
            chara->setCombinationRDef(json_chara["cmb_r"].int_value());
            CharaList::chara_list.insert(chara);
            chara->initialize();
        }
    }
    
    
    //デッキランダムシャッフル
    map<string, Json> json_player = json_master["player_data" + to_string(TurnManage::getEnemyNo())].object_items();
    string suffle_string = json_player["suffle"].string_value();
    
    if(isTutorial() || suffle_string == "doNot"){//シャッフルしたくないとき
        
    }else{//チュートリアル以外で
        for(int ii=0; ii<2; ii++){
            
            HMSLOG("シャッフル%d %d %d",deck[ii].at(0)->getPrimaryId(),deck[ii].at(1)->getPrimaryId(),deck[ii].at(2)->getPrimaryId());
            
            //シャッフル
            unsigned int seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
            shuffle(deck[ii].begin(),deck[ii].end(),std::mt19937(seed));//メルセンヌツイスター
            //CCLOG("シード：%d",seed);
            
            /*std::sort(deck[ii].begin(),deck[ii].end(),[](Chara *a,Chara *b){//戦闘力の低い順に並べるテスト。stl超便利
                return (a->getBaseValue() < b->getBaseValue());
            });*/
            
            //HMSLOG("シャッフル%d %d %d",deck[ii].at(0)->getPrim aryId(),deck[ii].at(1)->getPrimaryId(),deck[ii].at(2)->getPrimaryId());
        }
    }
    
    data_exec_flg = false;
    data_send_flg = false;
    cpu_moving = false;
    
    
    setState("Directioning");
    
    HMSLOG("initialize finish");
    
    
    //レイヤー初期化
    uiLayer->initialize();

    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(DungeonScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DungeonScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DungeonScene::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //開戦エフェクト後の処理のセレクタ
    SEL_SCHEDULE after_selector = schedule_selector(DungeonScene::afterStartBattleEffect);
    
    if(isTutorial()) {//共通条件
        //攻撃ボタンを無効にする
        uiLayer->changeAttackButton("attack_cant") ;//攻撃ボタンを暗く
        uiLayer->attack_button_compel_false = true;//攻撃ボタンを押せないように
        
        if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_1)==false) {//個別条件
            //ナビキャラの登場後バトル開始(開戦エフェクトは無し)
            tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_1);
            tutorialNaviLayer->setScheduler(this, after_selector);
        }
    }else{
        //開戦エフェクト(6.0f秒間)
        startBattleEffect(after_selector);
    }
    
    nume = NumericCount::create();
    nume->png_path = "init/Dungeon/design/count";
    dialogLayer->addChild(nume, 0);
    nume->setPosition(convertFromDisplaySize(Vec2(320,485), 0.5, 0.5));
    nume->setScale(ratio*2/3);
    
    nume->setVisible(false);
    
    //ボスイベントでの制限時間設定
    if(isBossEventBattle()) {
        boss_event_limit_time = json_master["boss_event_limit_time"].int_value();
        boss_event_stage_log_id = fromString<long>(json_master["boss_event_stage_log_id"].string_value());
        boss_event_stage_command_history_id = fromString<long>(json_master["boss_event_stage_command_history_id"].string_value());
        boss_event_enemy_user_master_id = fromString<int>(json_master["enemy_user_master_id"].string_value());
        string support_rate_up = json_master["support_rate_up"].string_value();
        support_rate_up_list = split(support_rate_up, ":");
        HMSLOG("boss_event_limit_time=%ld", boss_event_limit_time);
        HMSLOG("boss_event_stage_log_id=%ld", boss_event_stage_log_id);
    }
}

void DungeonScene::afterStartBattleEffect(float dt){//開戦エフェクト終了後
	
	//チュートリアル以外はこのタイミングで再生。
	if(!isTutorial()){
	    //BGM再生
    	this->runAction(Sequence::create(DelayTime::create(1.2f),
        	            PlayBGM::create("download/BGM/07_battle.mp3"),
            	        FadeToVolumeOfBGM::create(0.0f, SaveData::loadFloat("bgm_volume")),
                	    CallFunc::create([](){HMSLOG("volume1:%f",SimpleAudioEngine::getInstance()->getBackgroundMusicVolume());}),nullptr)
                    	);
	}

    //初期化その2
    HMSLOG("initializeEffect");
    for(int ii=0; ii<2; ii++) {
        
        //フレンド
        if(CharaList::getFriend(ii) != nullptr) {
            CharaList::getFriend(ii)->setPosition(fieldLayer->friend_position_list[ii]);
        }
        
        //デッキ
        for(int i=0; i<deck[ii].size(); i++) {
            deck[ii].at(i)->setPosition(fieldLayer->deck_point[ii]);
        }
        
        //プレイヤー情報の表示
        uiLayer->updatePlayerInfo(player[ii]->getHp(), player[ii]->getMaxHp(), ii);
    }
    
    //アイコンの点滅用
    schedule(schedule_selector(DungeonScene::iconChangeOrder), 1.8f);
    
    
    this->runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create([this](){
        initialize_effect_flg = true;
        
        //初期化が終了したらregistを送る
        if(getBattleMode()=="vs" && replay_flg==false) {
            map<string, Json> send_map;
            send_map.insert(make_pair("type", Json("regist")));
            send_message_vec.push_back(send_map);//送信待ちベクターに追加
            
            
            this->schedule(schedule_selector(DungeonScene::sendMessageOthers), 2.0f);
        }
    }), nullptr));
    
    
    CCLOG("総大将パッシブの時のターン%d",TurnManage::getTurnCount());
    string battle_data = SaveData::loadString("battle_data_for_restart");
    for(int ii=0; ii<2; ii++){//総大将のパッシブ
        if(CharaList::getLeader(ii)->getPassiveSkillId()>0 && TurnManage::getTurnCount()==0 && battle_data.length()<5){//初めのターンの時のみ(再開時はなし)
            CCLOG("初ターンのパッシブ=%s", battle_data.c_str());
            
            passive_effect_data = passiveSkill->skillLogic(CharaList::getLeader(ii)->getPrimaryId());
            
            resetAllCharaValue();	//戦闘力リセット
            calcCombinationValue();	//コンボによる攻撃力上昇
            
            //パッシブの追加とデータの保存
            calcPassiveBuffer();
            calcActiveBuffer();//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
            passive_effect_data += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
            
            passiveSkillEffectLayer->skillEffect(passive_effect_data);//パッシブエフェクト再生
            passive_effect_data = "";
        }
    }
    
    
    //スケジューラー登録
    this->schedule(schedule_selector(DungeonScene::updateOne), 0.30f);//高速化
    this->schedule(schedule_selector(DungeonScene::updateSendMessage), 0.15f);//コマンド送信用
    
    

    if(getBattleMode()=="solo") {
        
        int ranking_event_power = SaveData::loadInt("ranking_event_power");
        int ranking_event_hp = SaveData::loadInt("ranking_event_hp");
        string ranking_event_cs = SaveData::loadString("ranking_event_cs");
        string key = "key" + toString(ranking_event_power) + ":" + toString(ranking_event_hp);
        string checksum = HmsCrypto::makeCheckSum(key);
        
        if(ranking_event_hp>0) {
            if(ranking_event_cs==checksum) {
                if(!isChallengeEventBattle()) {
                    player[0]->setNowPower(ranking_event_power);
                }
                player[0]->setHp(ranking_event_hp);
                SaveData::saveInt("ranking_event_power", 0);
                SaveData::saveInt("ranking_event_hp", 0);
                
                //以下全体データに対する演出上の補正
                //プレイヤーのHP表示を更新する
                for(int ii=0; ii<2; ii++) uiLayer->updatePlayerInfo(player[ii]->getHp(), player[ii]->getMaxHp(), ii);
                
                //パワー表示も更新
                for(int ii=0; ii<2; ii++) {
                    uiLayer->updatePower(player[ii]->getNowPower(), ii);
                }
                
                checkIllegal("RankingEvent2ndFloorStart", false);
            } else {
                CCLOG("イリーガル２");
                
                int illegal_count = SaveData::loadInt("ic");
                illegal_count++;
                SaveData::saveInt("ic", illegal_count);
                string checksum = HmsCrypto::makeCheckSum("hms" + toString(illegal_count));
                SaveData::saveString("ics", checksum);

                if(replay_flg==false) {
                    string user_master_id = SaveData::loadString("user_master_id");
                    string url = HOST;
                    url += "battle/illegal.php";
                    url += "?user_master_id=" + user_master_id;
                    url += "&ranking_event=fail";
                    auto selector = httpresponse_selector(DungeonScene::notifyIllegalCallback);
                    HttpConnect::sendHttpRequest(this, url, selector);
                }
            }
        }
        
        string battle_data = SaveData::loadString("battle_data_for_restart");
        CCLOG("battle_data_for_restart=%s", battle_data.c_str());
        
        if(battle_data!="") {//クエスト・イベントの再開時にデータ復元
            runAction(Sequence::create(DelayTime::create(2.0f),CallFunc::create([this,battle_data](){
                string err;
                Json json = Json::parse(battle_data.c_str(), err);
                TurnManage::setTurnIndex(json["player_no"].int_value()-1);//現在のターンをセット
                cpu_moving = true;
                runBattleData(battle_data);
                
            }), NULL));
            
        }else{//再開データが無い時は保存
            int rand_seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
            rand_engine = std::mt19937(rand_seed);//ランダムシード生成
            
            map<string , Json> send_map;
            send_map.insert(make_pair("player_no", Json(TurnManage::getTurnNo())));
            send_map.insert(make_pair("type", Json("initialize")));
            send_map.insert(make_pair("primary_id", Json(set_chara->getPrimaryId())));
            //send_map.insert(make_pair("extra_data", Json(to_string(field_index))));
            send_map.insert(make_pair("battle_data", getBattleData()));
            send_map.insert(make_pair("random", Json(rand_seed)));
            SaveData::saveString("battle_data_for_restart", Json(send_map).dump());
            
            scheduleOnce(schedule_selector(DungeonScene::changeTurn), 2.0f);
        }
    }
}


//自分のターン・ソロプレイのときは　this_comp_turnは true
//相手のターンのときは　this_comp_turnは false
bool DungeonScene::useSkill(int primary_id, string extra_data, int rand_seed_rec)
{
    checkIllegal("useSkill()", true);
    
    if(isMyTurn()) score_param_skill_count++;
    
    data_exec_flg = true;
    
    setState("Directioning");
    
    int rand_seed;
    if(rand_seed_rec == -1){
        rand_seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
    }else{
        rand_seed = rand_seed_rec;//相手からもらったシードで
    }
    CCLOG("ランダムシード：%d",rand_seed);
    rand_engine = std::mt19937(rand_seed);//placeCharacter用のランダムシード生成
    
    
    Chara *use_chara = CharaList::getAtPrimary(primary_id);
    
    Json get_battle_data_string;
    if(getBattleMode()=="vs") get_battle_data_string = getBattleData();//対戦の場合ここでデータを保存しておく
    
    //攻撃情報をデータとして保存する
    use_skill_data = "";
    
    string temp = activeSkill->skillLogic(use_chara, use_chara->getActiveSkillId(), use_chara->getActiveSkillValue(), use_chara->getBelongingIndex(),use_chara->getActiveSkillTarget(), extra_data);
    
    HMSLOG("スキルテスト%s",temp.c_str());
    
    HmsVector<string> skill_string = split(temp, "##");//アクティブスキル効果計算
    
    
    
    extra_data = skill_string.at(0);//追加情報がある場合
	
	//スキルが失敗した場合、失敗のポップアップ生成＆表示
	HmsVector<string> skill_string_list = split(skill_string.at(1), ",");
	HmsVector<string> error_data_list = split(skill_string_list.at(0), "&");
	if(error_data_list.at(0) == "error"){
        if(getBattleMode()=="solo" && !isMyTurn()){//敵AIがスキルを使おうとしている時
            HMSLOG("敵：対象がいないので発動できないっす");
            setState("Operationing");
        }else{//自分が操作している時
            SkillErrorLayer* skill_error_layer = SkillErrorLayer::create();
            float time = 1.0f;//再生時間
            skill_error_layer->setData(stoi(error_data_list.at(1)), time);
            this->addChild(skill_error_layer, 100);
        }
        this->data_exec_flg = false;//これ入れないとアタックボタン押せなくなる
        return false;//スキル失敗により処理終了
    }
    
    if(isBossEventBattle()) {
        sendCommand();
    }
    
    //対戦時データ送信
    map<string , Json> send_map;
    send_map.insert(make_pair("player_no", Json(TurnManage::getTurnNo())));
    send_map.insert(make_pair("type", Json("use_skill")));
    send_map.insert(make_pair("primary_id", Json(use_chara->getPrimaryId())));
    send_map.insert(make_pair("extra_data", Json(extra_data)));
    send_map.insert(make_pair("battle_data", get_battle_data_string));
    send_map.insert(make_pair("random", Json(rand_seed)));
    
    if(getBattleMode()=="vs" && data_send_flg==true) {
        send_message_vec.push_back(send_map);//送信待ちベクターに追加
    }else if(getBattleMode()=="solo"){
        SaveData::saveString("battle_data_for_restart", Json(send_map).dump());
    }
    
	//スキル使用側の士気を消費
	player[use_chara->getBelongingIndex()]->consumePower(use_chara->getActiveSkillCost());
	
    //アクティブスキル使用可能回数を減らす
    use_chara->active_skill_able_count --;
    
    resetAllCharaValue();	//戦闘力リセット
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
	
	
	//HP0のキャラを判定する
    for(Chara *chara : CharaList::getAll()){
        if(chara->getDeadFlg()) {
            for(int j=0;j<2;j++){
                auto itr = player[j]->whole_status_passive.begin();
                while(itr != player[j]->whole_status_passive.end()){
                    if((*itr)["primary"] == chara->getPrimaryId()){
                        player[j]->whole_status_passive.erase(itr);//パッシブの削除
                        uiLayer->removePassiveIcon(chara->getPrimaryId());//アイコン削除
                    }else{
                        itr++;
                    }
                }
            }
        }
    }
	
    //スキル使用フラグを立てる
    if(isMyTurn()) param_noskill_flg = 0;
    
    //順次演出を行う。
    activeSkillEffectLayer->setSkillEffectData(skill_string.at(1));//スキルエフェクト
    
    checkIllegal("useSkill()", false);
    return true;
}

//アクティブスキルのエフェクト終了後に呼ばれる
void DungeonScene::useSkillEnd()
{
    for(Chara *chara : CharaList::getAll()){
        chara->showCardParam();
    }
    
    vector<Chara *> delete_chara;
    
    for(Chara *chara : CharaList::getAll()){
        if(chara->getType()==CHARA_TYPE_SUPPORT && chara->isOnField() && chara->getValue()<=0){//フィールドにいる軍師で残りターン無し
            for(int k=0; k<2; k++){
            
                for(int j=0; j<player[k]->whole_status_passive.size(); j++){
                    if(chara->getPrimaryId()==player[k]->whole_status_passive.at(j)["primary"]){
                        player[k]->whole_status_passive.erase(player[k]->whole_status_passive.begin() + j);//効果の削除
                    }
                }
                for(int j=0; j<uiLayer->passiveBufferIcon[k].size(); j++){
                    if(uiLayer->passiveBufferIcon[k].at(j).first==chara->getPrimaryId()){
                        uiLayer->removePassiveIcon(chara->getPrimaryId());//アイコンの削除
                    }
                }
            }
            delete_chara.push_back(chara);
        }
    }
    
    //軍師の削除
    for(int i=0; i<delete_chara.size(); i++){
        delete_chara.at(i)->setDeadFlg(true);
        delete_chara.at(i)->erase();
    }
    
    selectAbleEffect();
    this->scheduleOnce(schedule_selector(DungeonScene::postLogicCleanChara), 0.3f);
    this->scheduleOnce(schedule_selector(DungeonScene::postLogicSetStatusPlace), 0.5f);
    this->scheduleOnce(schedule_selector(DungeonScene::finishRivalEffect), 0.51f);//ステートが"place_character"になってからdata_exec_flgをfalseに
}

//スキル使用前後の戦闘力をデータ出力
string DungeonScene::beforeAfterCharaValue()
{
	string data = "@";
	bool first_flg = false;
    
    for(Chara *chara : CharaList::getAll()){
        if(chara->isOnField() || chara->isInHand() || chara->isFriend()) {
            //データ:主キー&計算前の戦闘力&計算後の戦闘力
            if(first_flg) {
                data += "," + to_string(chara->getPrimaryId()) + "&" + to_string(chara->getValue());
            } else {
                data += to_string(chara->getPrimaryId()) + "&" + to_string(chara->getValue());
                first_flg = true;
            }
        }
    }
	return data;
}

void DungeonScene::resetAllCharaValue(){
    for(int ii=0; ii<2; ii++){//元の戦闘力を保存
        for(int i=0; i<CharaList::getOneSide(ii).size(); i++) {
            if(CharaList::getOneSide(ii).at(i)->isAttackAndDefense() && CharaList::getOneSide(ii).at(i)->isOnField()){
                CharaList::getOneSide(ii).at(i)->value = CharaList::getOneSide(ii).at(i)->base_value;
                CharaList::getOneSide(ii).at(i)->combo_value = CharaList::getOneSide(ii).at(i)->base_value;
            }
        }
        player[ii]->setPower(player[ii]->getPowerDefault());//上昇士気初期化
    }
}

void DungeonScene::placeCharacter(Chara *set_chara,int field_index, int rand_seed_rec)//キャラ配置時
{
    checkIllegal("placeCharacter()", true);
    
    if(isMyTurn()) score_param_chara_count++;
    
    this->set_chara = set_chara;
    
	setState("Directioning");
    data_exec_flg = true;
    selectAbleEffect();
    reCalcCombination(); //コンボ再計算
    
    int rand_seed;
    if(rand_seed_rec == -1){
        rand_seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
    }else{
        rand_seed = rand_seed_rec;//相手からもらったシードで
    }
    rand_engine = std::mt19937(rand_seed);//placeCharacter用のランダムシード生成
    
    if(isBossEventBattle()) {
        sendCommand();
    }
    
    //対戦時データ送信
    map<string , Json> send_map;
    send_map.insert(make_pair("player_no", Json(TurnManage::getTurnNo())));
    send_map.insert(make_pair("type", Json("place_character")));
    send_map.insert(make_pair("primary_id", Json(set_chara->getPrimaryId())));
    send_map.insert(make_pair("extra_data", Json(to_string(field_index))));
    send_map.insert(make_pair("battle_data", getBattleData()));
    send_map.insert(make_pair("random", Json(rand_seed)));
    
    if(getBattleMode()=="vs" && data_send_flg==true) {
        send_message_vec.push_back(send_map);//送信待ちベクターに追加
    }else if(getBattleMode()=="solo"){
        SaveData::saveString("battle_data_for_restart", Json(send_map).dump());
    }
    
    set_chara_data = "";
    combination_dataL = "";
    combination_dataR = "";
    passive_effect_data = "";
    
    //キャラにデータを設定する
    set_chara->setFieldIndex(field_index);
    set_chara->showCardParam();
    set_chara->hand_index = -1;//手札インデックスのリセット
    if(set_chara->statusCheck(STATUS_TYPE_STOPPING_ATTACKER) && set_chara->isAttacker()){//攻撃カード行動不能にする軍師がいる場合
        map<string,int> temp;
        temp.insert( map<string, int>::value_type("primary", set_chara->getPrimaryId()));
        temp.insert( map<string, int>::value_type("skill_id", 0));
        temp.insert( map<string, int>::value_type("value", 0));
        temp.insert( map<string, int>::value_type("turn", 1*2));
        temp.insert( map<string, int>::value_type("state", STATUS_TYPE_CANT_ACTION));//行動不能
        set_chara->chara_status_active.push_back(temp);
    }
    
    
    set_chara_data+= to_string(set_chara->getPrimaryId()) + "&" + to_string(field_index);
    
    //コストを減らす
    player[set_chara->getBelongingIndex()]->consumePower(set_chara->getPlaceCost());
    addPowerEffect(0.0f);
    
    resetAllCharaValue();	//戦闘力リセット
    calcCombinationValue();	//コンボによる攻撃力上昇（追加するキャラを含まない）
    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師などのパッシブ）
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
    set_chara_data += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
    
    
    /***置いたキャラによるコンボの計算***/
    Chara *rightC = nullptr;
    Chara *leftC = nullptr;
    
    
    for(Chara *chara : CharaList::getOneSide(set_chara->getBelongingIndex())){
        if(chara->getFieldIndex()>1){//フィールドのキャラのみ
            if(chara->getFieldIndex()==set_chara->leftCharaFieldIndex()){//左側のキャラがいるとき
                //HMSLOG("%d vs %d", set_chara->getCombinationL(), all_chara_list[turn_index].at(i)->getCombinationR());
                if(set_chara->getCombinationL()==chara->getCombinationR() && set_chara->getCombinationL()!=0){//コンボマークが一致した時、コンボカラー０でないとき
                    HMSLOG("左コンボ！！");
                    leftC = chara;
                    
                    set_chara->combination_count += set_chara->getCombinationL() * 10;//コンボ追加
                    leftC->combination_count += set_chara->getCombinationL();//コンボ追加
                }
            }
        }
    }
   
    resetAllCharaValue();	//戦闘力リセット
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師・アクティブスキルによるバッファ効果含む）
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算

    if(leftC!=nullptr){
        combination_dataL += to_string(leftC->getPrimaryId()) +"&"+to_string(set_chara->getPrimaryId());
        combination_dataL += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
    }
    
    for(Chara *chara : CharaList::getOneSide(set_chara->getBelongingIndex())){
        if(chara->getFieldIndex()>1){//フィールドのキャラのみ
            if(chara->getFieldIndex()==set_chara->rightCharaFieldIndex()) {//左側のキャラがいるとき
                //HMSLOG("%d vs %d", set_chara->getCombinationR(), chara->getCombinationL());
                if(set_chara->getCombinationR()==chara->getCombinationL() && set_chara->getCombinationR()!=0) {
                    rightC = chara;
                    HMSLOG("右コンボ！！");
                    
                    set_chara->combination_count += set_chara->getCombinationR();//コンボ追加
                    rightC->combination_count += set_chara->getCombinationR() * 10;//コンボ追加
                }
            }
        }
    }
    resetAllCharaValue();	//戦闘力リセット
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師・アクティブスキルによるバッファ効果含む）
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
    
    if(rightC!=nullptr){
        combination_dataR += to_string(set_chara->getPrimaryId()) +"&"+to_string(rightC->getPrimaryId());
        combination_dataR += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
    }
  
    
    /***置いたキャラによるパッシブスキルの計算***/
    
    //パッシブスキル効果の計算 全員（軍師・アクティブスキルによるバッファ効果含む）
    if(set_chara->getPassiveSkillId()>0){//置いたキャラにパッシブがある時

        //パッシブの追加とデータの保存
        passive_effect_data = passiveSkill->skillLogic(set_chara->getPrimaryId());
        
        resetAllCharaValue();	//戦闘力リセット
        calcCombinationValue();	//コンボによる攻撃力上昇

        //パッシブの追加とデータの保存
        calcPassiveBuffer();
        calcActiveBuffer();//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
        passive_effect_data += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
    }
    
    checkIllegal("placeCharacter()", false);
    placeCharacterEffect(0.0f);//エフェクトの再生へ
}

void DungeonScene::calcActiveBuffer()
{//すべてのキャラのバッファ効果の適用
    //全体
    for(int ii=0; ii<2; ii++){
        for(int j=0; j< player[ii]->whole_status_active.size(); j++){
            //このへんおかしい気がする
            Chara *tempChara;
            for(Chara *chara : CharaList::getAll()){
                if(chara->isAttackAndDefense() || chara->getType()==CHARA_TYPE_SUPPORT){
                    tempChara = chara;
                }
            }
            activeBuffer->bufferLogic(tempChara, player[ii]->whole_status_active.at(j));
        }
    }
    
    //単体
    for(Chara *chara : CharaList::getAll()){
        if(chara->isOnField()){
            for(int k=0; k<chara->chara_status_active.size(); k++){
                activeBuffer->bufferLogic(chara, chara->chara_status_active.at(k));
                if(chara->chara_status_active.at(k).size()>0) HMSLOG("activeBuffer size : %lu", chara->chara_status_active.at(k).size());
            }
        }
    }
}

void DungeonScene::calcPassiveBuffer()
{//すべてのキャラのパッシブ効果の適用
    //全体
    for(int ii=0; ii<2; ii++){
        for(int j=0; j<player[ii]->whole_status_passive.size(); j++){
        
            Chara *skill_use_chara = CharaList::getAtPrimary(player[ii]->whole_status_passive.at(j)["primary"]);
            
            passiveBuffer->bufferLogic(player[ii]->whole_status_passive.at(j)["primary"], skill_use_chara->getPassiveSkillId(), player[ii]->whole_status_passive.at(j)["value"], ii);
        }
    }
    //単体
    for(int ii=0; ii<2; ii++){
        for(Chara *chara : CharaList::getOneSide(ii)){
            for(int k=0; k<chara->chara_status_passive.size(); k++){//かかっているパッシブに
                /*Chara *skill_use_chara=nullptr;
                for(Chara *chara2 : CharaList::getAll()){
                    if(chara2->getPrimaryId()==chara->chara_status_passive.at(k)["primary"]){
                        skill_use_chara = chara2;
                    }
                }*/
                Chara *skill_use_chara = CharaList::getAtPrimary(chara->chara_status_passive.at(k)["primary"]);
                
                if(skill_use_chara!=nullptr){
                    passiveBuffer->bufferLogic(chara->getPrimaryId(),skill_use_chara->getPassiveSkillId(), chara->chara_status_passive.at(k)["value"], ii);//valueにはバッファの変化用値が入る
                }
            }
        }
    }
}

void DungeonScene::calcCombinationValue()
{
    for(Chara *chara : CharaList::getAll()){
        if(chara->isAttackAndDefense()){
            if(chara->isOnField()){
                int combination_color_L = floor(chara->getCombinationCount()/10);
                int combination_color_R = chara->getCombinationCount()%10;
                int combo_magni_add_L = 0;
                int combo_magni_add_R = 0;
                //CCLOG("左：%d  右：%d", combination_color_L, combination_color_R);
                
                for(int k=0; k<chara->chara_status_active.size(); k++){//アクティブ
                    if(chara->chara_status_active.at(k)["state"] == STATUS_TYPE_COMBO_UPDOWN){
                        combo_magni_add_L += chara->chara_status_active.at(k)["value"];//コンボ威力の上下値を計算
                        combo_magni_add_R += chara->chara_status_active.at(k)["value"];//コンボ威力の上下値を計算
                    }
                }
                for(int k=0; k<chara->chara_status_passive.size(); k++){//パッシブ
                    if(chara->chara_status_passive.at(k)["state"] == STATUS_TYPE_COMBO_UPDOWN){
                        combo_magni_add_L += chara->chara_status_passive.at(k)["value"];//コンボ威力の上下値を計算
                        combo_magni_add_R += chara->chara_status_passive.at(k)["value"];//コンボ威力の上下値を計算
                    }
                }
                
                for(int k=0; k<player[chara->getBelongingIndex()]->whole_status_passive.size(); k++){
                    if(player[chara->getBelongingIndex()]->whole_status_passive.at(k)["state"] == STATUS_TYPE_COMBO_UPDOWN){
                        //HMSLOG("コンボ倍率%d",whole_status_passive[ii].at(k)["value"]);
                        combo_magni_add_L += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];//コンボ威力の上下値を計算
                        combo_magni_add_R += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];//コンボ威力の上下値を計算
                    }
                    if(player[chara->getBelongingIndex()]->whole_status_passive.at(k)["state"] == STATUS_TYPE_COMBO_UPDOWN_RED){
                        if(combination_color_L == 1) combo_magni_add_L += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                        if(combination_color_R == 1) combo_magni_add_R += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                    }
                    if(player[chara->getBelongingIndex()]->whole_status_passive.at(k)["state"] == STATUS_TYPE_COMBO_UPDOWN_BLUE){
                        if(combination_color_L == 2) combo_magni_add_L += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                        if(combination_color_R == 2) combo_magni_add_R += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                    }
                    if(player[chara->getBelongingIndex()]->whole_status_passive.at(k)["state"] == STATUS_TYPE_COMBO_UPDOWN_GREEN){
                        if(combination_color_L == 3) combo_magni_add_L += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                        if(combination_color_R == 3) combo_magni_add_R += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                    }
                    if(player[chara->getBelongingIndex()]->whole_status_passive.at(k)["state"] == STATUS_TYPE_COMBO_UPDOWN_DIFFERENT){//左と右のコンボ違う時アップ
                        if(chara->getCombinationL() != chara->getCombinationR()) combo_magni_add_L += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                        if(chara->getCombinationL() != chara->getCombinationR()) combo_magni_add_R += player[chara->getBelongingIndex()]->whole_status_passive.at(k)["value"];
                    }
                }
                float combo_final_L = COMBO_MAGNI * (combo_magni_add_L+100.0f)/ 100.0f;//パーセント値
                float combo_final_R = COMBO_MAGNI * (combo_magni_add_R+100.0f)/ 100.0f;//パーセント値
                
                if(chara->statusCheck(STATUS_TYPE_COMBO_UPDOWN_ZERO)){
                    combo_final_L = 0.0f;
                    combo_final_R = 0.0f;//パーセント値
                }
                
                if(combination_color_L==0) combo_final_L = 0.0;
                if(combination_color_R==0) combo_final_R = 0.0;
                
                //CCLOG("左倍率：%f  右倍率：%f", combo_final_L, combo_final_R);
                chara->combo_value = chara->base_value * (combo_final_L + combo_final_R +100.0f)/ 100.0f;//コンボによる戦闘力増加
                chara->setValue(chara->combo_value);
                
            }else{//手札やデッキ
                chara->combo_value = chara->base_value;
                chara->setValue(chara->combo_value);
            }
        }
    }
    
    comboEffectLayer->combinationThunder(TurnManage::getPlayerIndex());
}

void DungeonScene::reCalcCombination()
{
    for(Chara *chara : CharaList::getAll()){
        chara->combination_count=0;//コンボカウント初期化
    }
    
    for(Chara *chara : CharaList::getAll()){
        if(chara->getFieldIndex()>1){//フィールドにいる武将カードで
            for(Chara *chara2 : CharaList::getOneSide(chara->getBelongingIndex())){
                if(chara->getFieldIndex()+1 == chara2->getFieldIndex()) {//となり同士のカードで
                    if(chara->getCombinationR() == chara2->getCombinationL() && chara->getDeadFlg() == false && chara2->getDeadFlg() == false && chara->getCombinationR() != 0){ //マークが同じ時 かつ　両方生きている時 かつコンボカラー０じゃないとき
                        chara->combination_count += chara->getCombinationR();//右側カウント一桁目
                        chara2->combination_count += chara->getCombinationR() * 10;//左側カウント二桁目
                        break;
                    }
                }
            }
        }
    }
}

void DungeonScene::attack(bool index, bool waiting_flg, int rand_seed_rec)//通常攻撃  攻撃側インデックスと何もしないフラグ
{
    checkIllegal("attack()", true);
    setState("Directioning");
    
    int rand_seed;
    if(rand_seed_rec == -1){
        rand_seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
    }else{
        rand_seed = rand_seed_rec;//相手からもらったシードで
    }
    rand_engine = std::mt19937(rand_seed);//placeCharacter用のランダムシード生成
    
    
    if(index == 1){
        attacker_index=1;
        defender_index=0;
    }else{
        attacker_index=0;
        defender_index=1;
    }
 
    if(wholeStatusCheck(STATUS_TYPE_WAITING_FORCE, attacker_index)) waiting_flg = true;
    
    data_exec_flg = true;
    attack_info.clear();
    
    if(isBossEventBattle()) {
        sendCommand();
    }
    
    //対戦時データ送信
    map<string , Json> send_map;
    send_map.insert(make_pair("player_no", Json(TurnManage::getTurnNo())));
    send_map.insert(make_pair("type", Json("attack")));
    
    if(waiting_flg){
        send_map.insert(make_pair("extra_data", Json("waiting")));
    }else{
        send_map.insert(make_pair("extra_data", Json("normal")));
    }
    send_map.insert(make_pair("battle_data", getBattleData()));
    send_map.insert(make_pair("random", Json(rand_seed)));
    
    if(getBattleMode()=="vs" && data_send_flg==true) {
        send_message_vec.push_back(send_map);//送信待ちベクターに追加
    }else if(getBattleMode()=="solo"){
        SaveData::saveString("battle_data_for_restart", Json(send_map).dump());
    }
    
    
    if(TurnManage::getTurnCount() <= 1 || waiting_flg==true){//1ターン目か時間切れのとき
        
        HMSLOG("一ターン目か時間切れなので攻撃しません");
        
    }else{
        
        HMSLOG("攻撃ですよ");
        HMSLOG("turn_index:%d,wait_index:%d,TurnManage::getPlayerIndex():%d,enemy_index:%d",TurnManage::getTurnIndex(),TurnManage::getWaitIndex(),TurnManage::getPlayerIndex(),TurnManage::getEnemyIndex());//ここに追加してデバッグ
        
        
        //攻撃キャラを判定する
        vector<Chara *> chara_offenses;
        
        for(Chara *chara : CharaList::getOneSide(attacker_index)) {//攻撃側のフィールドのキャラ
            if(chara->getFieldIndex() > 1 && chara->isAlive() && chara->statusCheck(STATUS_TYPE_CANT_ACTION)==false && chara->statusCheck(STATUS_TYPE_ACCUMULATION)== false &&(chara->isAttacker() || chara->statusCheck(STATUS_TYPE_ANTI_ATTACKER))){
                chara_offenses.push_back(chara);
                
                for(int i=0; i<chara->skillCount(STATUS_TYPE_TWICE_ATTACK); i++){
                    chara_offenses.push_back(chara);//２回攻撃の場合はもう一回追加
                }
            }
        }
        
        std::sort(chara_offenses.begin(),chara_offenses.end(),[](Chara *a,Chara *b){//戦闘力の低い順に並べる
            if(a->getValue() == b->getValue()){//戦闘力が同じ時は
                return (a->getFieldIndex() < b->getFieldIndex());//フィールドインデックス低い順
            }else{//普通は
                return (a->getValue() < b->getValue());
            }
        });
        
        for(int k=0; k<chara_offenses.size(); k++){//攻撃低いキャラから攻撃
            map<string,int> temp_data = attackAlone(chara_offenses.at(k));
            if(temp_data.empty() == false) attack_info.push_back(temp_data);
        }
        
        
        //攻撃対象が全滅するかどうかチェックする   全滅する場合、攻撃対象未設定のキャラの攻撃対象をプレイヤーとする
        bool exist_flg = false;//全滅してないフラグ
        Chara *strong_defense = nullptr;//防衛側最高キャラ
        
        for(Chara *chara : CharaList::getOneSide(defender_index)) {//防御側のフィールドのキャラ
            if(chara->getFieldIndex() > 1 && chara->isAlive() && chara->statusCheck(STATUS_TYPE_CANT_ACTION)==false && chara->statusCheck(STATUS_TYPE_CHARM)==false
               && chara->isDefender() && chara->isAlive() && chara->statusCheck(STATUS_TYPE_CANT_ACTION_ESCAPE)==false){//敵側で生きていて行動できる防御キャラ
        
                exist_flg = true;
                if(strong_defense ==nullptr || chara->getValue()>strong_defense->getValue()){
                    strong_defense = chara;
                }
            }
        }
        
        std::sort(attack_info.begin(), attack_info.end(), [](map<string, int> &a, map<string, int> &b){
            bool change = false;
            if(a.count("tag_chara")!=0 && b.count("tag_chara")==0) change = true;
            
            return change;//ターゲットがいるものを前に並び替え（総大将攻撃は最後に行うようにする）
        });
        
        for(map<string,int> &temp_data : attack_info){//攻撃キャラの
            if(temp_data.count("tag_chara")==0){//ターゲットがいない時
                Chara *atk_chara = CharaList::getAtPrimary(temp_data["atk_chara"]);
                
                if(exist_flg == false){//全滅の場合プレイヤーへ攻撃
                    int damage = atk_chara->getValue() * atk_chara->statusUpdown(STATUS_TYPE_PLAYER_ATTACK_UPDOWN)/100.0f;
                    //player[defender_index]->damage(damage);//プレイヤー攻撃の攻撃UPDOWN
                    
                    temp_data["tag_chara"] = CharaList::getLeader(defender_index)->getPrimaryId();
                    temp_data["value"] = damage;
                    
                }else{//全滅してないので敵の高い防御キャラに突撃
                    temp_data["tag_chara"] = strong_defense->getPrimaryId();
                    temp_data["value"] = -1;
                }
            }
        }
        
        for(int i=0; i<attack_info.size(); i++){
            HMSLOG("キャラ%d ターゲット%d バリュ%d HP %d", attack_info.at(i)["atk_chara"], attack_info.at(i)["tag_chara"], attack_info.at(i)["value"], attack_info.at(i)["hp"]);
        }
    }
    
    counter_character_data.clear();
    
    for(map<string,int> temp_data : attack_info){
        Chara *target_chara = CharaList::getAtPrimary(temp_data["tag_chara"]);
        if((target_chara->isAttacker() || target_chara->isDefender()) && target_chara->isAlive() == false){//倒されたキャラ
            for(int k=0; k<target_chara->chara_status_passive.size(); k++){
                if(target_chara->chara_status_passive.at(k)["state"] == STATUS_TYPE_COUNTER && target_chara->statusCheck(STATUS_TYPE_ANTI_COUNTER) == false){//カウンターパッシブを持つキャラクターが死んでいる場合  カウンター使用不可されていないとき
                    
                    string temp = activeSkill->counterPassiveLogic(target_chara, target_chara->chara_status_passive.at(k)["skill_id"], target_chara->chara_status_passive.at(k)["value"], CharaList::getAtPrimary(temp_data["atk_chara"]));
                    counter_character_data.push_back(temp);
                }
            }
        }
    }
    
    resetAllCharaValue();	//戦闘力リセット
    reCalcCombination();	//コンボ再計算
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
    
    checkIllegal("attack()", false);
    
    //順次演出を行う。
    attackEffect(0.0f);
}

map<string,int> DungeonScene::attackAlone(Chara *chara_attack){//このキャラは誰にどう攻撃するか
    
    Chara* chara_target = nullptr;
    map<string,int> temp;//攻撃情報
    temp["atk_chara"] = chara_attack->getPrimaryId();
    
    vector<Chara *> chara_defenses;
    for(Chara *chara : CharaList::getOneSide(defender_index)) {//防御側のフィールドのキャラ
        if(chara->getFieldIndex() > 1 && chara->isAlive() && chara->statusCheck(STATUS_TYPE_CHARM)==false){
            if(chara->statusCheck(STATUS_TYPE_CANT_ACTION)==false && chara->statusCheck(STATUS_TYPE_CANT_ACTION_ESCAPE)==false){//行動不能になっていない、かつ行動不能ガード時に不屈行動不能になったわけでもない
                chara_defenses.push_back(chara);
                
            }
        }
    }
    
    if(chara_attack->statusCheck(STATUS_TYPE_CHARM)){//攻撃キャラがチャーム状態の場合
        temp["tag_chara"] = CharaList::getLeader(chara_attack->getBelongingIndex())->getPrimaryId();
        temp["value"] = chara_attack->getValue() * chara_attack->statusUpdown(STATUS_TYPE_PLAYER_ATTACK_UPDOWN)/100.0f;
        temp["state"] = STATUS_TYPE_CHARM;
        
    }else if(chara_attack->statusCheck(STATUS_TYPE_ANTI_ATTACKER)==true){//対攻撃パッシブを持ったキャラの場合
        for(int i=0; i<chara_defenses.size(); i++) {
            if(chara_defenses.at(i)->isAttacker() && chara_defenses.at(i)->getValue() <= chara_attack->getValue()) {
                
                if(chara_target==nullptr){
                    chara_target = chara_defenses.at(i);
                }else if(chara_target->getValue() < chara_defenses.at(i)->getValue()){//より強い攻撃対象に変更
                    chara_target = chara_defenses.at(i);
                }
            }
        }
        if(chara_target!=nullptr){
            
            temp["tag_chara"] = chara_target->getPrimaryId();
        }else{//攻撃できるキャラがいない
            temp.clear();
        }
    }else{//普通の攻撃キャラの場合
        //攻撃キャラが攻撃できるキャラのうち、一番強い対象を探す
        //ただし、他のキャラが既に攻撃対象としている場合は指定しない

        for(int i=0; i<chara_defenses.size(); i++) {
            if(chara_defenses.at(i)->isDefender() && chara_defenses.at(i)->getValue() <= chara_attack->getValue()) {
                if(chara_target==nullptr){
                    chara_target = chara_defenses.at(i);
                }else if(chara_target->getValue() < chara_defenses.at(i)->getValue()){//より強い攻撃対象に変更
                    chara_target = chara_defenses.at(i);
                }
            }
        }
        if(chara_target!=nullptr){//対象がいる場合
            
            temp["tag_chara"] = chara_target->getPrimaryId();
        }else{//対象がいない場合
            
        }
    }
    
    if(chara_target!=nullptr) {//攻撃するターゲットがいる時
        if(chara_target->statusCheck(STATUS_TYPE_STEEL_SHIELD)){//鉄壁スキル持ち
            temp["state"] = STATUS_TYPE_STEEL_SHIELD;
            temp["value"] = -1;
            for(int i=0; i<chara_target->chara_status_active.size(); i++){
                if(chara_target->chara_status_active.at(i)["state"] == STATUS_TYPE_STEEL_SHIELD){
                    chara_target->chara_status_active.remove(i);//鉄壁を消去
                    break;
                }
            }
        }else if(chara_target->statusCheck(STATUS_TYPE_ESCAPE_GUARD)){//攻撃受けても1ターン行動不能
            map<string,int> temp;
            temp.insert( map<string, int>::value_type("primary", chara_target->getPrimaryId()));
            temp.insert( map<string, int>::value_type("skill_id", chara_target->getPassiveSkillId()));
            temp.insert( map<string, int>::value_type("value", chara_target->getPassiveSkillValue()));
            temp.insert( map<string, int>::value_type("turn", 1));
            temp.insert( map<string, int>::value_type("state", STATUS_TYPE_CANT_ACTION_ESCAPE));
            chara_target->chara_status_active.push_back(temp);
            
            if(chara_target->statusCheck(STATUS_TYPE_CANT_ACTION_GUARD)==false){
                map<string,int> temp;
                temp.insert( map<string, int>::value_type("primary", chara_target->getPrimaryId()));
                temp.insert( map<string, int>::value_type("skill_id", chara_target->getPassiveSkillId()));
                temp.insert( map<string, int>::value_type("value", chara_target->getPassiveSkillValue()));
                temp.insert( map<string, int>::value_type("turn", 3));
                temp.insert( map<string, int>::value_type("state", STATUS_TYPE_CANT_ACTION));
                chara_target->chara_status_active.push_back(temp);
            }
        }else if(chara_target->statusCheck(STATUS_TYPE_RANDOM_GUARD)){//確率で攻撃無効
            int parcent = 0;
            for(int i=0; i<chara_target->chara_status_passive.size(); i++){
                if(chara_target->chara_status_passive.at(i)["state"]==STATUS_TYPE_RANDOM_GUARD){
                    parcent = chara_target->chara_status_passive.at(i)["value"];
                }
            }
            int rand_num = std::uniform_int_distribution<int>(0, 99)(rand_engine);
            if(rand_num <= parcent){//無効した時
                temp["state"] = STATUS_TYPE_RANDOM_GUARD;
                temp["value"] = -1;
            }else{//無効できず
                chara_target ->setDeadFlg(true);
            }
        
        }else{//普通のカード
            chara_target ->setDeadFlg(true);
        }
        
        if(chara_attack->statusCheck(STATUS_TYPE_SPEAR) && (chara_attack->getValue() - chara_target->getValue())>0 && chara_attack->statusCheck(STATUS_TYPE_CHARM)==false && temp["state"]!=STATUS_TYPE_RANDOM_GUARD){//貫通スキル持ち
            HMSLOG("貫通有りで貫通");
            temp["value"] = (chara_attack->getValue() - chara_target->getValue()) * chara_attack->statusUpdown(STATUS_TYPE_PLAYER_ATTACK_UPDOWN)/100.0f;
            
        }
    }
    
    return temp;
}

//開戦演出
void DungeonScene::startBattleEffect(SEL_SCHEDULE after_selector){
    //タップされるか、アクション終了後にセレクタに指定されたメソッドを呼び出す
    startBattleEffectLayer->startBattleEffect(this, after_selector, json_master);
}

bool DungeonScene::supplyCharacter()
{
    int hand_size = HAND_SIZE ;
    bool supply_flg = false;
    
    //デッキから補充する
    for(int ii=0; ii<2; ii++) {
        if(replay_flg==true && TurnManage::getTurnCount()==1) break;//リプレイの１ターン目はなし
        
        if(ii==TurnManage::getTurnIndex() || (TurnManage::getTurnCount()==1 && getBattleMode()=="solo")){
            //自分のターンの最初にのみ引くように・ソロでは最初のターンは両方引く
            
            //チュートリアルのデッキの配布枚数を条件分岐で変更
            if(isTutorial()&&TurnManage::getTurnCount() == 1 && TurnManage::getPlayerIndex()==ii)hand_size = 2 ; //チュートリアル中 自分の1ターン目は2枚
            else if(isTutorial() && TurnManage::getTurnCount() == 1 && TurnManage::getEnemyIndex()==ii)hand_size = 1 ; //チュートリアル中 自分の1ターン目は2枚
            else if (isTutorial() && TurnManage::getTurnCount() == 2)hand_size = 1 ; //チュートリアル中 3ターン目は自分、相手共に以降1枚ずつ
            else if (isTutorial() && TurnManage::getTurnCount() == 3)hand_size = 1 ; //チュートリアル中 3ターン目以降1枚ずつ
            else if (isTutorial() && TurnManage::getTurnCount() >= 4)hand_size = 0 ; //チュートリアル中 3ターン目以降1枚ずつ
            
            vector<bool> being_hand;
            
            for(int i=0; i<hand_size; i++){
                being_hand.push_back(false);//まずは全てにfalseを入れる
            }
            
            for(Chara *chara : CharaList::getOneSide(ii)){
                if(chara->isInHand()){
                    being_hand.at(chara->getHandIndex())=true;//手札にカードがある場所をtrueに
                }
            }
            
            if(wholeStatusCheck(STATUS_TYPE_DRAW_CONFINE,ii) && TurnManage::getNowTurn()!=1){//手札制限スキル用・１ターン目は効果なし
                int draw_count = wholeStatusValue(STATUS_TYPE_DRAW_CONFINE, ii, "min");
                
                for(int i=0; i<being_hand.size(); i++){
                    if(draw_count<=0) being_hand.at(i)=true;//手札制限で引かないようにする
                    if(being_hand.at(i)==false){
                        draw_count--;
                    }
                }
            }
            
            for(int i=0; i<hand_size; i++){
                if(being_hand.at(i)==false && deck[ii].size() > 0){//手札の空いている場所かつ、デッキが残っている時
                    supply_flg = true;
                    Chara *chara = deck[ii].at(0);//デッキシャッフルはinitializeでやったので、一番最初を取る
                    deck[ii].erase(deck[ii].begin());//デッキから取り除いて
                    chara->setHandIndex(i);//手札として
                    chara->reverse_flg = true;
                    cardLayer->reorderChild(chara, 1);//一旦前面に
                }
            }
        }
    }
    
    checkIllegal("supplyCharacter()", false);
    
    
    //デッキから補充するアニメーション
    for(Chara *chara : CharaList::getAll()){
        if(chara->isInHand() && chara->getPosition() != fieldLayer->hand_position_list[chara->getBelongingIndex()].at(chara->hand_index)) {
            //HMSLOG("カード配布");
            
            DelayTime *delay = DelayTime::create(0.1f*chara->hand_index);
            MoveTo *move1 = MoveTo::create(0.35f,fieldLayer->hand_position_list[chara->getBelongingIndex()].at(chara->getHandIndex()));
            EaseOut *ease = EaseOut::create(move1, 2.5f);
            DelayTime *delay2 = DelayTime::create(0.12f+0.03*chara->hand_index);
            int test = TurnManage::getPlayerIndex();
            
            CallFunc *func = CallFunc::create([&,chara,test,this](){
                
                if(chara->getBelongingIndex() == test){
                    chara->cardReverse(test, false);//表にする
                    chara->showValue();
                    this->cardLayer->reorderChild(chara, 0);//後面に移動
                }
            });
            
            chara->runAction(Sequence::create(delay,ease,delay2,func, nullptr));
            
        }else if(chara->isInHand()){
            
            int test = TurnManage::getPlayerIndex();
            CallFunc *func = CallFunc::create([&,test,chara,this](){
                
                if(chara->getBelongingIndex() == test){
                    chara->cardReverse(test, false);//表にする
                    this->cardLayer->reorderChild(chara, 0);//後面に移動
                }
            });
            
            chara->runAction(Sequence::create(DelayTime::create(1.0f),func, nullptr));
            
        }
    }
    
    data_exec_flg = false;
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/bash1.mp3");
    
    return supply_flg;
}


void DungeonScene::changeTurn(float dt)
{
    
    if(isBossEventBattle() && get_damage_data_flg==false) {
        get_damage_data_flg = true;
        getDamageData();
        return;
    }
    get_damage_data_flg = false;
    
    skill_confirm_layer->close();//スキル使用確認画面をとじる
    
    if(replay_flg==true){
        setState("Operationing");
    }
    
    //対戦時は利用しない
    if(getBattleMode()=="vs") {
        return;
    }
    
    if(game_end_flg == true) return;
    
    cpu_moving = true;//ターン切り替え中はCPUを動かさない
    
    TurnManage::setTurnCount(TurnManage::getTurnCount() + 1);
    HMSLOG("!!!!!!ターンカウント%d",TurnManage::getTurnCount());
    
    //ターンの切り替え
    TurnManage::changeTurnIndex();
    
    //ターン変更アニメーション
    changeTurnEffect(0.0f);
}

void DungeonScene::setTurn(int turn_no)
{
    if(game_end_flg == true) return;
    
    CCLOG("セットターン");
   
    turn_change_wait = false;//ターン変更メッセージが来ているフラグ。ターン変更したらfalseに
    
    TurnManage::setTurnCount(TurnManage::getTurnCount()+1);
    HMSLOG("!!!!!!ターンカウント%d",TurnManage::getTurnCount());
    
    //ターンの切り替え
    TurnManage::setTurnIndex(turn_no-1);
    
    //ターン変更アニメーション
    changeTurnEffect(0.0f);

}

void DungeonScene::changeTurnEffect(float dt)
{
    dialogLayer->changeTurnDialog(player[TurnManage::getTurnIndex()]->getName(), isMyTurn());//~~のターンですの表示
    
    //メモリのキャッシュクリア
    if(TurnManage::getTurnCount()%10==2) Director::getInstance()->purgeCachedData();
    
    this->runAction(Sequence::create(DelayTime::create(1.5f), CallFunc::create([this](){
        addPower(0.0f);
        addPowerEffect(0.0f);
    }), NULL));
}



void DungeonScene::addPower(float dt)
{
    if(TurnManage::getTurnCount() <= 2){
        player[TurnManage::getTurnIndex()]->generatePower(player[TurnManage::getTurnIndex()]->getStartAddPower() - player[TurnManage::getTurnIndex()]->getPowerDefault() + player[TurnManage::getTurnIndex()]->getPower());//初ターン増加士気
    }else{
        player[TurnManage::getTurnIndex()]->generatePower(player[TurnManage::getTurnIndex()]->getPower());//士気追加
    }
    
    turn_skill_data_active.clear();
    turn_skill_data_passive.clear();
    turn_skill_data_hand.clear();//演出ストリングリセット
    
    for(Chara *chara : CharaList::getOneSide(TurnManage::getTurnIndex())){
        if(chara->getFieldIndex()>1 && chara->isAlive()){
            for(auto &buffer : chara->chara_status_active){
                
                string temp ="";
                temp = activeBuffer->turnBufferLogic(chara, buffer);//ターン開始時に効果のある全体パッシブ効果
                if(temp != ""){//ターン開始時に効果のある物の場合
                    //HMSLOG("ターン開始時発動パッシブ有り");
                    resetAllCharaValue();	//戦闘力リセット
                    calcCombinationValue();	//コンボによる攻撃力上昇
                    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師などのパッシブ）
                    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
                    temp += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
                    
                    turn_skill_data_active.push_back(temp);//演出stringの保存
                }
            }
        }
    }
    
    for(int i=0; i<player[TurnManage::getTurnIndex()]->whole_status_passive.size(); i++){
        
        string temp ="";
        temp = passiveBuffer->turnBufferLogic(player[TurnManage::getTurnIndex()]->whole_status_passive.at(i));//ターン開始時に効果のある全体パッシブ効果
        if(temp != ""){//ターン開始時に効果のある物の場合
            //HMSLOG("ターン開始時発動パッシブ有り");
            resetAllCharaValue();	//戦闘力リセット
            calcCombinationValue();	//コンボによる攻撃力上昇
            calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師などのパッシブ）
            calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
            temp += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
            
            turn_skill_data_passive.push_back(temp);//演出stringの保存
        }
    }
    for(Chara *chara : CharaList::getOneSide(TurnManage::getTurnIndex())){
        if(chara->getFieldIndex()>1 && chara->isAlive()){
            for(auto &buffer : chara->chara_status_passive){
                
                string temp ="";
                temp = passiveBuffer->turnBufferLogic(buffer);//ターン開始時に効果のある全体パッシブ効果
                if(temp != ""){//ターン開始時に効果のある物の場合
                    //HMSLOG("ターン開始時発動パッシブ有り");
                    resetAllCharaValue();	//戦闘力リセット
                    calcCombinationValue();	//コンボによる攻撃力上昇
                    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師などのパッシブ）
                    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
                    temp += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
                    
                    turn_skill_data_passive.push_back(temp);//演出stringの保存
                }
            }
        }
    }
    
    //手札発動パッシブ
    for(Chara *used_chara : CharaList::getOneSide(TurnManage::getTurnIndex())){//ターン側のキャラで
        if(used_chara->getHandIndex()>=0 && used_chara->getHandIndex()<=5){//手札にあるキャラ（フレンド除く）
            string effect_string = passiveBuffer->turnHandBufferLogic(used_chara);//手札発動パッシブ
            
            if(effect_string != ""){//スキル持ちの時
                
                resetAllCharaValue();	//戦闘力リセット
                calcCombinationValue();	//コンボによる攻撃力上昇
                calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師などのパッシブ）
                calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
                effect_string += beforeAfterCharaValue();//ステータスバフを計算し、算出されたカードの戦闘力をスキルデータに追加
                
                turn_skill_data_hand.push_back(effect_string);//演出stringの保存
            }
        }
    }
    
    this->scheduleOnce(schedule_selector(DungeonScene::turnBufferEffect), 0.0f);
}

void DungeonScene::turnBufferEffect(float dt){//毎ターン効果のパッシブのエフェクト
    if(turn_skill_data_active.size() > 0){
        activeSkillEffectLayer->turnSkillEffect(turn_skill_data_active.front(),TurnManage::getTurnIndex());
        turn_skill_data_active.erase(turn_skill_data_active.begin());//先頭要素削除
        
    }else if(turn_skill_data_passive.size() > 0){
        passiveSkillEffectLayer->turnSkillEffect(turn_skill_data_passive.front(),TurnManage::getTurnIndex());
        turn_skill_data_passive.erase(turn_skill_data_passive.begin());//先頭要素削除
        
    }else if(turn_skill_data_hand.size() > 0){//手札発動スキル
        passiveSkillEffectLayer->turnSkillEffect(turn_skill_data_hand.front(),TurnManage::getTurnIndex());
        turn_skill_data_hand.erase(turn_skill_data_hand.begin());//先頭要素削除
        
    }else{
        bufferMinusTurn();
    }
}

void DungeonScene::bufferMinusTurn(){
    
    bool update_flg = false;
    
    HMSLOG("アクティブ全体バッファマイナス");
    //全体バッファマイナスターン
    for(int ii=0; ii<2; ii++){
        for(int i=0; i<player[ii]->whole_status_active.size(); i++){
            player[ii]->whole_status_active.at(i)["turn"]--;
        }
    }
    
    for(int ii=0; ii<2; ii++){
        auto itr = player[ii]->whole_status_active.begin();
        while (itr != player[ii]->whole_status_active.end()) {
            if((*itr)["turn"] <= 0){//ターンが0になったなら消す
                HMSLOG("アクティブ全体バッファのターンが０になった");
                
                for(int j=0; j<uiLayer->wholeBufferIcon[ii].size(); j++){
                    if((*itr)["primary"]==uiLayer->wholeBufferIcon[ii].at(j).first){
                        uiLayer->wholeBufferIcon[ii].at(j).second->removeFromParentAndCleanup(true);
                        uiLayer->wholeBufferIcon[ii].erase(uiLayer->wholeBufferIcon[ii].begin() + j);//アイコンひとつ削除
                        break;
                    }
                }
                player[ii]->whole_status_active.erase(itr);
            }else{
                itr++;
            }
        }
    }
    uiLayer->resetIconPosition();
    
    
    //キャラバッファマイナスターン
    for(Chara *chara : CharaList::getAll()){
        for(int j=0; j<chara->chara_status_active.size(); j++){
            map<string, int> temp_map = chara->chara_status_active.at(j);
            temp_map["turn"] --;
            chara->chara_status_active.insert(temp_map, j);
            chara->chara_status_active.erase(chara->chara_status_active.begin() + j + 1);
            update_flg =true;
        }
    }
    
    std::function<void()> test = [&](){//ターン0のアクティブスキルを消す再帰ラムダ
        for(Chara *chara : CharaList::getAll()){
            for(int j=0; j<chara->chara_status_active.size(); j++){
                if(chara->chara_status_active.at(j)["turn"] <= 0){
                    chara->chara_status_active.erase(chara->chara_status_active.begin()+j);
                    test();//消す物があったらもう一度実行
                    HMSLOG("アクティブスキル消滅");
                }
            }
        }
    };
    test();
    
    resetAllCharaValue();	//戦闘力リセット
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師などのパッシブ）
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算

    for(Chara *chara : CharaList::getAll()){//戦闘力更新
        if(chara->isOnField()) update_flg = chara->showValue(chara->value,0.1f);
    }
    
    for(int ii=0; ii<2; ii++){//総大将の上昇士気の更新
        CharaList::getLeader(ii)->showValue(player[ii]->getPower());
    }
    
    if(update_flg == true){
        this->scheduleOnce(schedule_selector(DungeonScene::turnMinusSupport), 0.11f);
    }else{
        this->scheduleOnce(schedule_selector(DungeonScene::turnMinusSupport), 0.0f);
    }
}


void DungeonScene::turnMinusSupport(float dt){//軍師のターン減少と消滅
    for(Chara *chara : CharaList::getAll()){
        chara->showCardParam();
    }
    
    //軍師マイナスターン
    vector<Chara *> delete_chara;
    
    for(Chara *chara : CharaList::getOneSide(TurnManage::getTurnIndex())){
        if(chara->getType()==CHARA_TYPE_SUPPORT && chara->isOnField()){//フィールドにいる軍師
            chara->setValue(chara->getValue()-1);
            
            if(chara->getValue()<=0){//ターンが0になったら消す
                HMSLOG("軍師のターンが０になった");
                
                for(int k=0; k<2; k++){
                    auto itr = player[k]->whole_status_passive.begin();
                    while(itr != player[k]->whole_status_passive.end()){
                        if(chara->getPrimaryId()==(*itr)["primary"]){
                            player[k]->whole_status_passive.erase(itr);//効果の削除
                        }else{
                            itr++;
                        }
                    }
                    
                    uiLayer->removePassiveIcon(chara->getPrimaryId());//アイコンの削除
                }
                
                delete_chara.push_back(chara);
            }else{
                //数値の更新
                HMSLOG("軍師バリュー%d",chara->getValue());
                chara->showValue(chara->getValue());
            }
        }
    }
    
    //軍師の削除
    for(int i=0; i<delete_chara.size(); i++){
        delete_chara.at(i)->setDeadFlg(true);
        delete_chara.at(i)->erase();
    }
    
    //次のターン死亡のアクティブスキル
    /*bool next_dead_flg = true;
    while(next_dead_flg){
        next_dead_flg=false;
        Chara *next_dead_chara = nullptr;
        for(Chara *chara : CharaList::getAll()){
            for(int j=0; j<chara->chara_status_active.size(); j++){
                if(chara->chara_status_active.at(j)["state"] == STATUS_TYPE_NEXT_TURN_DEAD){//次のターン死亡異常があり、かけたキャラ側のターンの時
                    
                    Chara *chara2 = CharaList::getAtPrimary(chara->chara_status_active.at(j)["primary"]);
                    
                    
                    if(chara2 != nullptr && chara2->getBelongingIndex() == TurnManage::getTurnIndex()){
                        next_dead_chara = chara;
                        next_dead_flg=true;
                    }
                }
            }
        }
       
        if(next_dead_chara != nullptr){
            next_dead_chara->setDeadFlg(true);
            next_dead_chara->erase();
        }
    }*/
    for(Chara *chara : CharaList::getAll()){
        for(int j=0; j<chara->chara_status_active.size(); j++){
            if(chara->chara_status_active.at(j)["state"] == STATUS_TYPE_NEXT_TURN_DEAD && chara->chara_status_active.at(j)["turn"]==1){//次のターン死亡異常があ
                chara->setDeadFlg(true);
                chara->erase();
                break;
                //Chara *chara2 = CharaList::getAtPrimary(chara->chara_status_active.at(j)["primary"]);
                
                
                //if(chara2 != nullptr && chara2->getBelongingIndex() == TurnManage::getTurnIndex()){
                //    next_dead_chara = chara;
                //    next_dead_flg=true;
                //}
            }
        }
    }
    
    
    
    
    reCalcCombination();    //コンボ再計算
    resetAllCharaValue();	//戦闘力リセット
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師・アクティブスキルによるバッファ効果含む）
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
    
    
    //戦闘力数値の更新
    float delay_time =0.0f;
    if(valueShowUpdate(0.0f) == true) delay_time += 0.3f;
    
    if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount() == 1 ) {//チュートリアル用 自分の最初のターン
        if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_2)==false) {//個別条件
            //ナビキャラの登場後バトル開始(開戦エフェクトは無し)
            tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_2);
            tutorialNaviLayer->setScheduler(this, nullptr);
            uiLayer->changeAttackButton("attack_cant");
        }
    }
    
    if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount()==2) {//共通条件
        if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_7)==false) {//個別条件
            //ナビキャラの登場後バトル開始(開戦エフェクトは無し)
            tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_7);
            SEL_SCHEDULE s = schedule_selector(DungeonScene::changeTurnEnd);
            tutorialNaviLayer->setScheduler(this, s);
            
        }
    }else if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount()==3) {//共通条件
        if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_10)==false) {//個別条件
            //ナビキャラの登場後バトル開始(開戦エフェクトは無し)
            tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_10);
            SEL_SCHEDULE s = schedule_selector(DungeonScene::changeTurnEnd);
            tutorialNaviLayer->setScheduler(this, s);
            uiLayer->changeAttackButton("attack");
            uiLayer->changeAttackButton("attack_cant");
        }
    }else if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount()==5) {//共通条件
        if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_15)==false) {//個別条件
            //ナビキャラの登場後バトル開始(開戦エフェクトは無し)
            tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_15);
            SEL_SCHEDULE s = schedule_selector(DungeonScene::changeTurnEnd);
            tutorialNaviLayer->setScheduler(this, s);
            uiLayer->changeAttackButton("attack");
            uiLayer->changeAttackButton("attack_cant");
        }
    }else{
        runAction(Sequence::create(DelayTime::create(0.4f),CallFunc::create([this](){
            this->gameoverJudge();//ゲームオーバー判定　相打ちからコンテニューした時用
        }), NULL));
        this->scheduleOnce(schedule_selector(DungeonScene::changeTurnEnd), delay_time+0.5f);
    }
    
    
    this->runAction(Sequence::create(DelayTime::create(0.45f),CallFunc::create([&](){//X秒後に終了判定
        for(Chara *chara : CharaList::getAll()){
            chara->showCardParam();
        }
    }), nullptr));
    
}

void DungeonScene::changeTurnEnd(float dt){
    
    //デッキから手札補充
    bool supply_flg = supplyCharacter();
    float supply_delay = 0.0f;
    if(supply_flg == true) supply_delay = 1.4f;
    
    //アクティブスキル使用可能回数の初期化
    HMSLOG("アクティブスキル使用可能回数,%d",SaveData::loadInt("active_skill_limit"));
    for(Chara *chara : CharaList::getAll()){
        if(chara->active_skill_able_count != -1){//-1以外ならリセット
            chara->active_skill_able_count = SaveData::loadInt("active_skill_limit");
        }
        if(isTutorial()) chara->active_skill_able_count = 10;//チュートリアルは例外で・・・
    }
    
    for(Chara *chara : CharaList::getAll()){//スキル１ターンに１回縛りのリセット。現在はCPU用
        chara->used_skill_flg = false;
    }

    this->scheduleOnce(schedule_selector(DungeonScene::postLogicSetStatusPlace), 0.1f+supply_delay);
    
    runAction(Sequence::create(DelayTime::create(0.0f+supply_delay),CallFunc::create([this](){
        first_turn_set_end = true;//最初のセットターン終了
        cpu_moving = false;
        HMSLOG("セットターン終了");
    }), nullptr));
    
    checkIllegal("changeTurnEnd()", false);
}


bool DungeonScene::valueShowUpdate(float delay){
    
    bool value_delay_flg = false;
    for(Chara *chara : CharaList::getAll()){
        if(chara->isOnField() || chara->isInHand() || chara->isFriend()){
            value_delay_flg = chara->showValue(chara->value, delay);
        }
    }

    for(int ii=0; ii<2; ii++){//総大将の上昇士気の更新
        CharaList::getLeader(ii)->showValue(player[ii]->getPower());
    }
    
    return value_delay_flg;
}

void DungeonScene::placeCharacterEffect(float dt)
{
    //戦闘力数値の更新
    bool damageDelayFlg =false;
    
    if(set_chara_data!="") {
        HmsVector<string> set_value_data = split(set_chara_data, "@");//value情報を分割
        
        HmsVector<string> data_list = split(set_value_data.at(0), ",");
        for(int i=0; i<data_list.size(); i++) {
            HmsVector<string> data_list2 = split(data_list.at(i), "&");
            int id = fromString<int>(data_list2.at(0));
            int field_index = fromString<int>(data_list2.at(1));
            
            Chara *chara = CharaList::getAtPrimary(id);
            
            chara->showCardParam();
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/bash1.mp3");
            
            //移動
            Point target_position = fieldLayer->field_position_list[set_chara->getBelongingIndex()].at(field_index);
            MoveTo *move = MoveTo::create(0.3f, target_position);
            auto scale = ScaleTo::create(0.3, 1.0f);
            auto spawn = Spawn::create(move,scale, NULL);
            
            CallFunc *call;
            if (SaveData::loadBool("effect")) {//エフェクト有りの時
                
                call = CallFunc::create([this,chara](){
                    //煙エフェクト
                    ParticleSystemQuad* ptcle = ParticleSystemQuad::create("download/Effect/Particle/card_set.plist");
                    ptcle->setAutoRemoveOnFinish(true);
                    ptcle->setScale(ratio);
                    Point test = fieldLayer->field_position_list[chara->getBelongingIndex()].at(chara->getFieldIndex());
                    ptcle->setPosition(test.x,test.y - 50*ratio);
                    this->uiEffectLayer->addChild(ptcle, 10);
                });
            }else{
                call = CallFunc::create([](){});
            }
            
            chara->runAction(Sequence::create(spawn, call, NULL));
            HMSLOG("プレイスキャラエフェクト　キャラ：%d フィールドインデックス：%d",chara->getCharaId(),field_index);
            
            
            string set_value_string = set_value_data.at(1);
            
            if(set_value_string!=""){
                HMSLOG("ステータス変化%s",set_value_string.c_str());
                
                HmsVector<string> prim_value_list= split(set_value_string,",");
                HmsVector<int> chara_primary;
                HmsVector<int> chara_new_value;
                
                for(int n=0;n<prim_value_list.size();n++){
                    HmsVector<string> temp =split(prim_value_list.at(n),"&");
                    chara_primary.push_back(stoi(temp.at(0)));
                    chara_new_value.push_back(stoi(temp.at(1)));
                }
                
                if(combination_dataL=="" && combination_dataR==""){
                    for(int m=0;m<chara_primary.size();m++){
                        for(Chara *chara2 : CharaList::getAll()){
                            if(chara2->getPrimaryId() == chara_primary.at(m)){
                                chara2->showValue(chara_new_value.at(m), 0.2f);//戦闘力の更新
                            }
                        }
                    }
                }
            }
            
            if(isTutorial() && !isTutorialNaviActive()) {//共通条件
                if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_3)==false && isMyTurn() && TurnManage::getTurnCount()==1) {//個別条件
                    tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_3);
                    tutorialNaviLayer->setScheduler(this, nullptr);
                } else if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_4)==false && isMyTurn() && TurnManage::getTurnCount()==1) {//個別条件
                    tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_4);
                    tutorialNaviLayer->setScheduler(this, nullptr);
                    
                    //攻撃ボタンを有効にする
                    uiLayer->attack_button_compel_false = false;
                    uiLayer->changeAttackButton("attack") ;
                    uiLayer->changeAttackButton("attack_encourage") ;
                    
                }
            }
        }
    }
    
    //攻撃対象を表示
    if(damageDelayFlg==true) {
        this->scheduleOnce(schedule_selector(DungeonScene::combinationCharacterEffectL), 0.5f);
    }else{
        this->scheduleOnce(schedule_selector(DungeonScene::combinationCharacterEffectL), 0.1f);
    }
}

void DungeonScene::combinationCharacterEffectL(float dt)
{
    if(combination_dataL!=""){
        CCLOG("combination_dataL : %s",combination_dataL.c_str());
        comboEffectLayer->combinationEffect(combination_dataL, set_chara->getBelongingIndex(), 1);//コンボエフェクトと戦闘力数値更新
    }else{
        this->scheduleOnce(schedule_selector(DungeonScene::combinationCharacterEffectR), 0.1f);
    }
}
void DungeonScene::combinationCharacterEffectR(float dt)
{
    if(combination_dataR!=""){
        CCLOG("combination_dataR : %s",combination_dataR.c_str());
        comboEffectLayer->combinationEffect(combination_dataR, set_chara->getBelongingIndex(), 2);//コンボエフェクトと戦闘力数値更新
    }else{
        this->scheduleOnce(schedule_selector(DungeonScene::passiveSkillEffect), 0.1f);
    }
}

void DungeonScene::passiveSkillEffect(float dt)
{
    if(passive_effect_data!=""){//パッシブスキルデータがある時
        passiveSkillEffectLayer->skillEffect(passive_effect_data);//パッシブエフェクト再生
        
    }else{
        for(Chara *chara : CharaList::getAll()){
            chara->showCardParam();
        }
        
        if(isTutorial() && !isTutorialNaviActive()) {//共通条件
            if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_11)==false && isMyTurn() && TurnManage::getTurnCount()==3) {//個別条件
                SEL_SCHEDULE after_selector = schedule_selector(DungeonScene::tutorialNaviSkill);
                tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_11);
                tutorialNaviLayer->setScheduler(this, after_selector);
            }
        }
        
        uiLayer->resetIconPosition();
        
        this->scheduleOnce(schedule_selector(DungeonScene::postLogicCleanChara), 0.01f);
        this->scheduleOnce(schedule_selector(DungeonScene::postLogicSetStatusPlace), 0.1f);
        this->scheduleOnce(schedule_selector(DungeonScene::finishRivalEffect), 0.2f);
    }
}

void DungeonScene::attackEffect(float dt)
{
    float attack_interval = 0.34f;//攻撃間隔秒
    for(int i=0; i<attack_info.size(); i++) {//攻撃エフェクトを順番に再生
        DelayTime *delay = DelayTime::create(attack_interval*i);
        CallFunc *func1 = CallFunc::create([this, i]()
                                           {
                                               this->attackEffect2(i);
                                           });//ラムダ式
        Sequence *action = Sequence::create(delay, func1, nullptr);
        this->runAction(action);
    }
    
    
    float delay = attack_interval * attack_info.size();
    if(counter_character_data.empty()==false){
        delay += 1.0f;
    }else{
        delay += 0.3f;
    }
    
    this->scheduleOnce(schedule_selector(DungeonScene::counterEffect), delay);//カウンターのエフェクトを再生
}

void DungeonScene::counterEffect(float dt){
    
    if(counter_character_data.empty() == false/* && player[TurnManage::getWaitIndex()]->getHp()>0*/){
        string counter_data = counter_character_data.front();
        counter_character_data.erase(counter_character_data.begin());
        
        passiveSkillEffectLayer->counterEffect(counter_data);
        
    }else{
        if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount()==2) {//共通条件
            if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_9)==false) {//個別条件
                tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_9);
                SEL_SCHEDULE s = schedule_selector(DungeonScene::changeTurn);
                tutorialNaviLayer->setScheduler(this, s);
            }
        }else if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount()==3) {//共通条件
            if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_13)==false) {//個別条件
                tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_13);
                SEL_SCHEDULE s = schedule_selector(DungeonScene::changeTurn);
                tutorialNaviLayer->setScheduler(this, s);
                
                //攻撃ボタンを無効にする（以後、攻撃ボタンは押させない)
                uiLayer->changeAttackButton("attack_cant");
                uiLayer->attack_button_compel_false = true;
            }
        }else if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount()==4) {//共通条件
            if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_14)==false) {//個別条件
                tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_14);
                SEL_SCHEDULE s = schedule_selector(DungeonScene::changeTurn);
                tutorialNaviLayer->setScheduler(this, s);
            }
        }else{
            
            resetAllCharaValue();	//戦闘力リセット
            calcCombinationValue();	//コンボによる攻撃力上昇
            calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師・アクティブスキルによるバッファ効果含む）
            calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
            
            finishRivalEffect();
            valueShowUpdate(0.0f);
            setState("Waiting");
            
            if(game_end_flg == false){//ゲームオーバー判定
                this->scheduleOnce(schedule_selector(DungeonScene::changeTurn), 0.3f);
            }
        }
    }
}

void DungeonScene::attackEffect2(int index)
{
    //攻撃側が白く点滅
    Chara *atk_chara = CharaList::getAtPrimary(attack_info.at(index)["atk_chara"]);
    
    atk_chara->displayWhiteBlink(0.4f);
    
    if(attack_info.at(index)["state"] == STATUS_TYPE_CHARM){//誘惑持ちはハートマーク出す
        //パーティクル出力
        ParticleSystemQuad* temptation = ParticleSystemQuad::create("download/Effect/Particle/temptation.plist");
        temptation->setAutoRemoveOnFinish(true);
        temptation->setPosition(atk_chara->getPosition());
        temptation->setScale(ratio);
        effectLayer->addChild(temptation, 1);
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/temptation.mp3");
        
    }
    
    Point now_point = fieldLayer->field_position_list[atk_chara->getBelongingIndex()].at(atk_chara->getFieldIndex());
    
    int diff = 30*ratio;//移動距離
    if(attacker_index == TurnManage::getEnemyIndex()) diff = -diff;
    
    //上がる、止まる、下がる
    MoveTo *move0;
    MoveTo *move1;
    if(attack_info.at(index).count("tag_chara")>0 && attack_info.at(index)["tag_chara"] == CharaList::getLeader(attacker_index)->getPrimaryId()){//誘惑などで味方総大将を攻撃
        move0 = MoveTo::create(0.12f, Point(now_point.x, now_point.y+diff/7));
        move1 = MoveTo::create(0.10f, Point(now_point.x, now_point.y-diff));
    }else{
        move0 = MoveTo::create(0.12f, Point(now_point.x, now_point.y-diff/7));
        move1 = MoveTo::create(0.10f, Point(now_point.x, now_point.y+diff));
    }
    auto ease0 = EaseOut::create(move0, 1.7f);
    auto ease1 = EaseIn::create(move1,1.7f);
    
    CallFunc *func1 = CallFunc::create([this, index]()
                                       {
                                           this->attackEffect3(index);//攻撃エフェクト
                                       });//ラムダ式
    DelayTime *delay = DelayTime::create(0.1f);
    CallFunc *func2 = CallFunc::create([this, index]()
                                       {
                                           this->attackEffect4(index);//ダメージエフェクト
                                       });//ラムダ式
    DelayTime *delay2 = DelayTime::create(0.1f);
    float back_time = 0.25f;
    if(atk_chara->statusCheck(STATUS_TYPE_TWICE_ATTACK)) back_time = 0.1f;
    MoveTo *move2 = MoveTo::create(back_time, now_point);
    
    Sequence *action = Sequence::create(ease0, ease1, func1, delay2, func2, delay, move2, nullptr);
    atk_chara->runAction(action);
}

void DungeonScene::attackEffect3(int index)
{
    //防御側
    int value = 0;
    
    Chara *target_chara = CharaList::getAtPrimary(attack_info.at(index)["tag_chara"]);
    if(target_chara!=nullptr) value = attack_info.at(index)["value"];
    
    Chara *attacked_chara = CharaList::getAtPrimary(attack_info.at(index)["atk_chara"]);
    
	if(value != -1) {//攻撃で普通に倒す時
		
		//パーティクル出力
		string effect_filename = "zan1.plist";
		if(index==1) {
			effect_filename = "zan2.plist";
		} else if(index==2) {
			effect_filename = "zan3.plist";
		} else if(index==3) {
			effect_filename = "zan4.plist";
		} else if(index==4) {
			effect_filename = "zan1.plist";
		} else if(index==5) {
			effect_filename = "zan2.plist";
		} else if(index==6) {
			effect_filename = "zan3.plist";
		}
		
            if (SaveData::loadBool("effect")) {//エフェクト有りの時
                ParticleSystemQuad* particle = ParticleSystemQuad::create("download/Effect/Particle/" + effect_filename);
                particle->setAutoRemoveOnFinish(true);
                particle->setPosition(target_chara->getPosition());
                
                if(attacked_chara->getValue()>=300){
                    particle->setScale(ratio*1.45f);
                }else if(attacked_chara->getValue()>=200){
                    particle->setScale(ratio*1.30f);
                }else if(attacked_chara->getValue()>=100){
                    particle->setScale(ratio*1.15f);
                }else if(attacked_chara->getValue()>=50){
                    particle->setScale(ratio*1.0f);
                }else{
                    particle->setScale(ratio*0.9f);
                }
                
                
                if(attacked_chara->statusCheck(STATUS_TYPE_CHARM)){//恋の病
                    particle->setStartColor(Color4F((126.0f/255.0f),(0.0f/255.0f),(67.0f/255.0f),1));
                    particle->setEndColor(Color4F((255.0f/255.0f),(20.0f/255.0f),(148.0f/255.0f),1));
                }else if(attacked_chara->statusCheck(STATUS_TYPE_ANTI_ATTACKER)){//迎撃持ち
                    particle->setStartColor(Color4F((29.0f/255.0f),(32.0f/255.0f),(136.0f/255.0f),1));
                    particle->setEndColor(Color4F((255.0f/255.0f),(255.0f/255.0f),(0.0f/255.0f),1));
                    //particle->setStartColor(Color4F::BLUE);
                }else if(attacked_chara->statusCheck(STATUS_TYPE_SPEAR)){//貫通持ち
                    particle->setStartColor(Color4F((29.0f/255.0f),(32.0f/255.0f),(136.0f/255.0f),1));
                    particle->setEndColor(Color4F(0,(160.0f/255.0f),(233.0f/255.0f),1));
                    //particle->setStartColor(Color4F::BLUE);
                }
                
                
                effectLayer->addChild(particle, 2);
                
                ParticleSystemQuad* particle2 = ParticleSystemQuad::create("download/Effect/Particle/defense.plist");//火花エフェクト
                particle2->setAutoRemoveOnFinish(true);
                particle2->setScale(0.4f*ratio);
                particle2->setPosition(target_chara->getPosition());
                effectLayer->addChild(particle2, 1);
                
            }
            ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
            hit1->setAutoRemoveOnFinish(true);
            hit1->setPosition(target_chara->getPosition());
            hit1->setScale(ratio);
            effectLayer->addChild(hit1, 1);
            
            
            //振動
            MoveTo* move1 = MoveTo::create(0.01f, Point(5, 0));
            MoveTo* move2 = MoveTo::create(0.01f, Point(-5, 0));
            MoveTo* move3 = MoveTo::create(0.01f, Point(0, 0));
            DelayTime* delay_t = DelayTime::create(0.05f);
            
            auto delay = DelayTime::create(0.5f);
            auto call = CallFunc::create([this,target_chara](){
                if((target_chara->statusCheck(STATUS_TYPE_COUNTER)==false || target_chara->statusCheck(STATUS_TYPE_ANTI_COUNTER)==true) && target_chara->getType()!=CHARA_TYPE_LEADER){//カウンター持ちとリーダー以外消去エフェクト
                    this->eraseCharacterEffect(target_chara);
                    HMSLOG("イレースキャラクタエフェクト");
                }
            });
            
            ActionInterval* action = Sequence::create(move1, delay_t, move2, delay_t, move3,delay,call, nullptr);
            this->runAction(action);
        
        
        if(target_chara->statusCheck(STATUS_TYPE_ESCAPE_GUARD)){//倒されても行動不能になるパッシブ
            target_chara->showSpriteField();
        }
        
        //防御側が赤く点滅
        target_chara->displayRedBlink(0.5f);
        
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/slash2.mp3");

        
    }else{//防御キャラにはじかれる場合
        if(attacked_chara->statusCheck(STATUS_TYPE_DEFENSE_BREAK)){//攻撃で防御キャラ戦闘力ダウンを持っている時
            map<string,int> temp;
            temp.insert( map<string, int>::value_type("primary", target_chara->getPrimaryId()));
            temp.insert( map<string, int>::value_type("skill_id", 4));//戦闘力ダウン
            temp.insert( map<string, int>::value_type("value", attacked_chara->getPassiveSkillValue()));
            temp.insert( map<string, int>::value_type("turn", 3));
            target_chara->chara_status_active.push_back(temp);
        }
        
        
        int random = (arc4random()%10 - 5)*3;
        int random2 = (arc4random()%10 - 5)*3;
        
        if(attack_info.at(index)["state"]==STATUS_TYPE_RANDOM_GUARD){//ランダムガード成功時
            activeSkillEffectLayer->missEffect(fieldLayer->field_position_list[target_chara->getBelongingIndex()].at(target_chara->getFieldIndex()));

            Point now_point = fieldLayer->field_position_list[target_chara->getBelongingIndex()].at(target_chara->getFieldIndex());
            int diff = -20*ratio;//移動距離
            if(attacker_index == TurnManage::getEnemyIndex()) diff = -diff;
            
            auto move1 = MoveTo::create(0.10f, Point(now_point.x, now_point.y-diff));
            auto ease1 = EaseIn::create(move1,1.7f);
            DelayTime *delay2 = DelayTime::create(0.05f);
            MoveTo *move2 = MoveTo::create(0.15f, now_point);
            
            Sequence *action = Sequence::create(ease1, delay2, move2, nullptr);
            target_chara->runAction(action);

            /*Sprite *temp = Sprite::create("init/Dungeon/design/ef_shield.png");//盾画像
            temp->setPosition(target_chara->getPosition().x, target_chara->getPosition().y);
            temp->setScale(ratio);
            temp->setOpacity(0);
            effectLayer->addChild(temp,1);
            auto fade1 = FadeTo::create(0.1f,210);
            auto delay = DelayTime::create(0.3f);
            auto fade2 = FadeOut::create(0.1f);
            auto remove = RemoveSelf::create();
            temp->runAction(Sequence::create(fade1,delay,fade2,remove, NULL));*/
            
        }else{
            
            if (SaveData::loadBool("effect")) {//エフェクト有りの時
                
                ParticleSystemQuad* particle = ParticleSystemQuad::create("download/Effect/Particle/defense.plist");//火花エフェクト
                particle->setAutoRemoveOnFinish(true);
                particle->setScale(0.7f*ratio);
                particle->setPosition(target_chara->getPosition().x + random*ratio, target_chara->getPosition().y + random2*ratio);
                if(attacked_chara->statusCheck(STATUS_TYPE_DEFENSE_BREAK)){//攻撃で防御キャラ戦闘力ダウンを持っている時
                    particle->setEndColor(Color4F::BLUE);
                    particle->setStartColor(Color4F::BLUE);
                    particle->setEndSize(40.0f);
                    particle->setEndSizeVar(20.0f);
                    particle->setGravity(Vec2(0,0));
                    particle->setScale(0.6f*ratio);
                }
                effectLayer->addChild(particle, 3);
                
                ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");//ヒットエフェクト
                hit1->setScale(ratio);
                hit1->setAutoRemoveOnFinish(true);
                hit1->setPosition(target_chara->getPosition().x + random*ratio, target_chara->getPosition().y + random2*ratio);
                effectLayer->addChild(hit1, 2);
                
                
                Effect *barri = Effect::create();//バリアエフェクト
                barri->setData(this, "download/Effect/Sprite/barrier", 27, Point(target_chara->getPosition().x,target_chara->getPosition().y), 0.8f, 1);
                
                //SE
                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/kantu.mp3");
            }
            
            Sprite *temp;
            if(attack_info.at(index)["state"] == STATUS_TYPE_STEEL_SHIELD){
                temp = Sprite::create("download/Dungeon/iron_shield.png");//盾画像
            }else{
                temp = Sprite::create("init/Dungeon/design/ef_shield.png");//盾画像
            }
            temp->setPosition(target_chara->getPosition().x, target_chara->getPosition().y);
            temp->setScale(ratio);
            temp->setOpacity(0);
            
            if(attack_info.at(index)["state"] == STATUS_TYPE_STEEL_SHIELD){
                Sprite *temp2 = Sprite::create("download/Dungeon/iron_shield2.png");//光回転
                temp2->setPosition(target_chara->getPosition().x, target_chara->getPosition().y);
                temp2->setScale(ratio*0.8f);
                temp2->setOpacity(0);
                effectLayer->addChild(temp2,0);
                auto fade1 = FadeTo::create(0.1f,230);
                auto delay = DelayTime::create(0.15f);
                auto fade2 = FadeOut::create(0.15f);
                auto remove = RemoveSelf::create();
                temp2->runAction(Spawn::create(Sequence::create(fade1,delay,fade2,remove, NULL),RotateTo::create(0.4, 50), NULL) );
                
                
                Sprite *temp3 = Sprite::create("download/Dungeon/iron_shield3.png");//盾画像
                temp3->setPosition(target_chara->getPosition().x, target_chara->getPosition().y);
                temp3->setScale(ratio*0.7f);
                temp3->setOpacity(0);
                effectLayer->addChild(temp3,1);
                auto scale1 = ScaleTo::create(0.4f, 1.8f);
                auto sign = EaseSineInOut::create(scale1);
                auto remove2 = RemoveSelf::create();
                temp3->runAction(Sequence::create(sign,remove2 ,NULL));
            }
            
            effectLayer->addChild(temp,1);
            auto fade1 = FadeTo::create(0.1f,210);
            auto delay = DelayTime::create(0.3f);
            auto fade2 = FadeOut::create(0.1f);
            auto remove = RemoveSelf::create();
            temp->runAction(Sequence::create(fade1,delay,fade2,remove, NULL));
            
            //振動
            MoveTo* move1 = MoveTo::create(0.01f, Point(5, 0));
            MoveTo* move2 = MoveTo::create(0.01f, Point(-5, 0));
            MoveTo* move3 = MoveTo::create(0.01f, Point(0, 0));
            DelayTime* delay_t = DelayTime::create(0.05f);
            ActionInterval* action = Sequence::create(move1, delay_t, move2, delay_t, move3, nullptr);
            this->runAction(action);
        }
    }
}

void DungeonScene::attackEffect4(int index)
{
    Chara *chara_temp = CharaList::getAtPrimary(attack_info.at(index)["atk_chara"]);
    
    if(chara_temp->statusCheck(STATUS_TYPE_SPEAR) && attack_info.at(index)["value"]>0 && chara_temp->statusCheck(STATUS_TYPE_CHARM)==false) {//貫通持ちの場合
        player[defender_index]->damage(attack_info.at(index)["value"]);
        CharaList::getLeader(defender_index)->displayDamage(attack_info.at(index)["value"]);
        uiLayer->updatePlayerInfo(player[defender_index]->getHp(), player[defender_index]->getMaxHp(), defender_index);
        
        if(chara_temp->statusCheck(STATUS_TYPE_ATTACK_MORALE_GAIN)){//総大将攻撃で士気を得る
            int power_up_num = 0;
            for(int i=0; i<chara_temp->chara_status_passive.size(); i++){
                if(chara_temp->chara_status_passive.at(i)["state"] == STATUS_TYPE_ATTACK_MORALE_GAIN){
                    power_up_num = chara_temp->chara_status_passive.at(i)["value"];
                    break;
                }
            }
            
            player[attacker_index]->generatePower(power_up_num);
            uiLayer->updatePower(player[attacker_index]->getNowPower(), attacker_index);
            
            if(SaveData::loadBool("effect")) {//パーティクル出力
                ParticleSystemQuad* resorb = ParticleSystemQuad::create("download/Effect/Particle/resorb.plist");
                resorb->setAutoRemoveOnFinish(true);
                resorb->setPosition(convertFromDisplaySize(uiLayer->power_point[attacker_index], 0.5, 0.5));
                resorb->setScale(ratio*3/4);
                effectLayer->addChild(resorb, 2);
            }
        }
        return;
    }
    
    //防御側
    if(attack_info.at(index)["tag_chara"] == CharaList::getLeader(defender_index)->getPrimaryId()){//普通に敵総大将を攻撃
        player[defender_index]->damage(attack_info.at(index)["value"]);
        CharaList::getLeader(defender_index)->displayDamage(attack_info.at(index)["value"]);
        uiLayer->updatePlayerInfo(player[defender_index]->getHp(), player[defender_index]->getMaxHp(), defender_index);
        
        if(chara_temp->statusCheck(STATUS_TYPE_ATTACK_MORALE_GAIN)){//総大将攻撃で士気を得る
            int power_up_num = 0;
            for(int i=0; i<chara_temp->chara_status_passive.size(); i++){
                if(chara_temp->chara_status_passive.at(i)["state"] == STATUS_TYPE_ATTACK_MORALE_GAIN){
                    power_up_num = chara_temp->chara_status_passive.at(i)["value"];
                    break;
                }
            }
            
            player[attacker_index]->generatePower(power_up_num);
            uiLayer->updatePower(player[attacker_index]->getNowPower(), attacker_index);
            
            if(SaveData::loadBool("effect")) {//パーティクル出力
                ParticleSystemQuad* resorb = ParticleSystemQuad::create("download/Effect/Particle/resorb.plist");
                resorb->setAutoRemoveOnFinish(true);
                resorb->setPosition(convertFromDisplaySize(uiLayer->power_point[attacker_index], 0.5, 0.5));
                resorb->setScale(ratio*3/4);
                effectLayer->addChild(resorb, 2);
            }
        }
        
    }else if(attack_info.at(index)["tag_chara"] == CharaList::getLeader(attacker_index)->getPrimaryId()){//誘惑などで味方総大将を攻撃
        player[attacker_index]->damage(attack_info.at(index)["value"]);
        CharaList::getLeader(attacker_index)->displayDamage(attack_info.at(index)["value"]);
        uiLayer->updatePlayerInfo(player[attacker_index]->getHp(), player[attacker_index]->getMaxHp(), attacker_index);
    }
    
}

void DungeonScene::playerHpUpdate(int index, int damage, int playerHp)//回復のときはdamageにマイナス値
{
    CharaList::getLeader(index)->displayDamage(damage);
	uiLayer->updatePlayerInfo(playerHp, player[index]->getMaxHp(), index);
	
}

void DungeonScene::eraseCharacterEffect(Chara *chara){//デッドフラグのあるキャラを消すエフェクトと処理
    
    if(chara->getDeadFlg() == true){
        
        chara->erase();
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/erase.mp3");
    }
}

void DungeonScene::addPowerEffect(float dt)
{
    //パワー表示を更新
    uiLayer->updatePower(player[TurnManage::getTurnIndex()]->getNowPower(), TurnManage::getTurnIndex());
    
    //パーティクル出力
    ParticleSystemQuad* white_flash = ParticleSystemQuad::create("download/Effect/Particle/white_flash.plist");
    white_flash->setAutoRemoveOnFinish(true);
    white_flash->setPosition(convertFromDisplaySize(uiLayer->power_point[TurnManage::getTurnIndex()], 0.5, 0.5));
	white_flash->setScale(ratio);
    effectLayer->addChild(white_flash, 1);
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/shake_shield.mp3");
    
}

void DungeonScene::selectAbleEffect(bool able){
    vector<Point> white_filter_position;
    
    //選択できるキャラを色づけ スキルの使用可能エフェクト 攻撃ボタンが押せるか
    for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())){
        
        if(able == false){
            continue;
        }
        
        if(isCharacterPlacable() == false) {
            chara->hideSkillAble();
            continue;
        }
        
        if(chara->skillAble()) {//フィールド上のスキル使えるキャラ
            chara->showSkillAble();
        } else if(chara->isOnField() && chara->skillAble()==false){//フィールド上のスキル使えないキャラ
            chara->hideSkillAble();
        } else if(chara->hand_index >=0 && chara->moveAble()) {//手札で出せるキャラ
            white_filter_position.push_back(chara->getPosition());
        } else {
           
        }
    }
    
    cardLayer->whiteFilterCreate(white_filter_position);
    
    //敵側
    for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())) {
        chara->hideYellowBlink();
    }
    
    for(int i=0; i<2; i++){//総大将スキル
        if(CharaList::getLeader(i)->skillAble()){
            CharaList::getLeader(i)->showSkillAble();
        }else{
            CharaList::getLeader(i)->hideSkillAble();
        }
    }
    
    if(isMyTurn()){
        //uiLayer->enableAttackButton() ;
    }else{
        uiLayer->changeAttackButton("attack");
    }
}

void DungeonScene::moveAbleEffect(){//移動できる場所を色付け
    if(isMyTurn()==false) return;
    
    if(tapped_chara->isAttackAndDefense()) {
        for(int i=2; i<=7; i++){//武将を置ける場所に対して
            bool flag = false;
            for(Chara *chara : CharaList::getOneSide(TurnManage::getTurnIndex())){
                if(chara->getFieldIndex()==i || fieldLayer->equalPositionType(tapped_chara, i, TurnManage::getPlayerIndex())==false){
                    //カードの上に置ける場合の処理
                    flag = true;
                }
            }
            
            if(flag==false){//空白地点の場合
                uiEffectLayer->place_frame_effect(fieldLayer->field_position_list[TurnManage::getTurnIndex()].at(i));
            }
        }
    } else if(tapped_chara->getType()==CHARA_TYPE_SUPPORT){
        for(int i=0; i<=1; i++){//軍師を置ける場所に対して
            bool flag = false;
            for(Chara *chara : CharaList::getOneSide(TurnManage::getTurnIndex())){
                if(chara->getFieldIndex()==i || fieldLayer->equalPositionType(tapped_chara, i, TurnManage::getPlayerIndex())==false){
                    //カードの上に置ける場合の処理
                    flag=true;
                }
            }
            
            if(flag==false){//空白地点の場合
                uiEffectLayer->place_frame_effect_support(fieldLayer->field_position_list[TurnManage::getTurnIndex()].at(i));
            }
        }
    }
}

//キャラクタ死亡後処理
void DungeonScene::postLogicCleanChara(float dt)
{
    //HP0のキャラを判定する
    for(Chara * chara : CharaList::getAll()){
        if(chara->getDeadFlg()) {
            chara->erase();
        }
    }
    
    reCalcCombination();//コンボ再計算
    
    resetAllCharaValue();	//戦闘力リセット
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算 全員（軍師・アクティブスキルによるバッファ効果含む）
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
    
    for(Chara * chara : CharaList::getAll()){
        if((chara->isAttackAndDefense() || chara->getType()==CHARA_TYPE_SUPPORT) && chara->isOnField()){
            chara->showValue(chara->getValue());//戦闘力数値更新
            chara->showCardParam();
        }
    }
    
    checkIllegal("postLogicCleanChara()", false);
}

void DungeonScene::postLogicSetStatusPlace(float dt)
{
    if(isMyTurn()) {
        data_send_flg = true;
        data_received.clear();
    }
    
    createStatusCheckSum();//チェックサムの計算
    HMSLOG("ポストロジックのチェックサム：%d", status_check_sum);
    checkIllegal("postLogicSetStatusPlace()", false);
    
    fieldLayer->fieldStateEffect();
    if(game_end_flg){
        setState("Waiting");
    }else{
        setState("Operationing");
    }
    if(isTutorial()==false) uiLayer->changeAttackButton("attack");
    selectAbleEffect();
}

//自分のターンかどうか
bool DungeonScene::isMyTurn()
{
    if(TurnManage::getPlayerNo()==TurnManage::getTurnNo()){
        return true;
    }else{
        return false;
    }
}

//自分のターンで、かつキャラ配置時かどうか
bool DungeonScene::isCharacterPlacable()
{
    return (isMyTurn() && getState()=="Operationing");
}

//タップしたキャラで置き換える
void DungeonScene::replaceChara(Chara *new_chara, Chara*old_chara, int rand_seed) {
    checkIllegal("replaceChara()", true);
    
    HMSLOG("リプレイスキャラ：%d, インデックス：%d",new_chara->getCharaId(),replace_field_index);
    
    int field_index = old_chara->getFieldIndex();//置き換え場所保存
    
    //古いキャラを消す
    old_chara->setDeadFlg(true);//死亡扱いに
    old_chara->erase();
    
    
    checkIllegal("replaceChara()", false);
    
    //キャラを配置する
    placeCharacter(new_chara, field_index, rand_seed);
    resetTapInfo();
}


//タップしたキャラを元の位置に戻す
void DungeonScene::restoreChara(Chara *chara)
{
    setState("Directioning");
    
    if(chara!=nullptr && (chara->isInHand()||chara->isFriend())){
        
        MoveTo *move;
        if(chara->isFriend()){//フレンドカードの時
            //配置しないときは元に戻す
            move = MoveTo::create(0.23f, fieldLayer->friend_position_list[chara->getBelongingIndex()]);
        }else{//その他
            move = MoveTo::create(0.23f, fieldLayer->hand_position_list[chara->getBelongingIndex()].at(chara->getHandIndex()));
        }
        auto ease = EaseOut::create(move, 2);
        
        auto scale = ScaleTo::create(0.1f, 1.0f);
        
        CallFunc *expire = CallFunc::create([&,this](){
            this->setState("Operationing");
            //resetTapInfo();
            if(this->turn_change_wait == false) selectAbleEffect();
        });
        
        
        chara->runAction(Sequence::create(Spawn::create(ease, scale, NULL), expire, NULL));
        
    }
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/shake_clock.mp3");
    
}

//タップ情報をリセットする
void DungeonScene::resetTapInfo()
{
    
    for(Chara *chara : CharaList::getAll()){
        if(skill_confirm_layer->visible_flg == false) chara->charaNameTab(false);
        
        if(tapped_chara != nullptr && tapped_chara->getPrimaryId() == chara->getPrimaryId()){
            cardLayer->reorderChild(chara, 1);//少し前
            HMSLOG("前へ");
        }else{
            cardLayer->reorderChild(chara, 0);//後ろへ
        }
    }
    
    tapped_chara = nullptr;
    tapped_id = 0;
    
    this->unschedule(schedule_selector(DungeonScene::longTapCounter));
    tap_time = 0;
    
}


//状態変更を行う
void DungeonScene::setState(string new_state)
{
    HMSLOG("state %s -> %s", state.c_str(), new_state.c_str());
    state = new_state;
}

string DungeonScene::getState(){
    return state;
}

void DungeonScene::iconChangeOrder(float dt){//一定時間ごとにステータスアイコン切り替え
    for(Chara *chara : CharaList::getAll()){
        if(chara->isOnField()){
            chara->updateStatusIcon();//キャラクターの
        }
    }
    
    uiLayer->updateBufferIcon();//全体の
    
}

bool DungeonScene::gameoverJudge(string over_state)
{
    if(replay_flg==true) return false;
    if(winner_string!="") return false;//既に終了していたら通らない
    
    if(over_state == "surrender"){//敵が降参してきた時
        player[TurnManage::getEnemyIndex()]->setHp(0);
        winner_string = "player";
        game_end_flg = true;
        
    }else if(over_state == "give_up"){//自分が降参した時
        player[TurnManage::getPlayerIndex()]->setHp(0);
        winner_string = "enemy";
        game_end_flg = true;
        
    }else if(over_state == "time_up"){//タイムアップの時
        if(player[TurnManage::getPlayerIndex()]->getHp() > player[TurnManage::getEnemyIndex()]->getHp()){//自分のがHP高い時
            winner_string = "player";
            player[TurnManage::getEnemyIndex()]->setHp(0);
        }else if(player[TurnManage::getPlayerIndex()]->getHp() < player[TurnManage::getEnemyIndex()]->getHp()){//相手のがHP高い時
            winner_string = "enemy";
            player[TurnManage::getPlayerIndex()]->setHp(0);
        }else{//HPが同じ時
            if((json_master["ahead_no"].int_value() - 1) == TurnManage::getPlayerIndex()){
                player[TurnManage::getEnemyIndex()]->setHp(0);
                winner_string = "player";
            }else{
                player[TurnManage::getPlayerIndex()]->setHp(0);
                winner_string = "enemy";
            }
        }
        game_end_flg = true;
        
    }else if(isBossEventBattle() && over_state=="escape") {//ボスが逃げた時
        game_end_flg = true;
        winner_string = "escape";
        
    }else{//総大将のHPが０になった時
        
        if(player[TurnManage::getPlayerIndex()]->getHp()<=0 && player[TurnManage::getEnemyIndex()]->getHp()<=0){//相打ち
            game_end_flg = true;
            if(isMyTurn()){//ターン取ってる方が勝利
                winner_string = "player";
            }else{
                winner_string = "enemy";
            }
            sendCommand();
        }else if(player[TurnManage::getPlayerIndex()]->getHp()<=0){//自分死亡
            game_end_flg = true;
            winner_string = "enemy";
            sendCommand();
        }else if(player[TurnManage::getEnemyIndex()]->getHp()<=0){//相手死亡
            game_end_flg = true;
            winner_string = "player";
            sendCommand();
        }
    }
    
    
    //ゲーム終了判定
    if(game_end_flg == true){
        
        //コンティニューの有無
        if(over_state=="" && winner_string == "enemy" && json_master["reborn_able"].int_value()==1 /*&& fromString<int>(SaveData::loadString("jewel"))>=json_master["reborn_jewel"].int_value()*/){
            auto continue_layer = ContinueLayer::create();
            addChild(continue_layer,100);
            continue_layer->showDetail();
            
        }else{
            goResult();
        }            
        
    }else{
        return false;
    }
    

    return true;
}

void DungeonScene::goResult(){
    CCLOG("goResult");
    //スケジュールの停止
    unscheduleAllCallbacks();
    
    //リザルト用の透明モーダルレイヤー作成
    auto resultModalLayer = ResultModalLayer::create();
    addChild(resultModalLayer, ZORDER_RESULT_MODAL_LAYER);
    
    this->unschedule(schedule_selector(DungeonScene::sendMessageOthers));
    
    //リザルト画面の表示
    bool winner_flg;
    if(winner_string == "enemy") winner_flg = false;
    else winner_flg = true;
    if(winner_string == "escape") winner_flg = false;
    
    if(isRankingEventBattle()) resultLayer = (ResultLayer *)RankingEventResultLayer::create(winner_flg);
    else if(isChallengeEventBattle()) resultLayer = (ResultLayer *)ChallengeEventResultLayer::create(winner_flg);
    else if(isBossEventBattle()) resultLayer = (ResultLayer *)BossEventResultLayer::create(winner_flg, winner_string);
    else if(isTournamentEventBattle()) resultLayer = (ResultLayer *)TournamentEventResultLayer::create(winner_flg);
    else resultLayer = ResultLayer::create(winner_flg);
    
    
    addChild(resultLayer, ZORDER_RESULT_LAYER);
}

void DungeonScene::continueReborn(){//コンティニューで復活
    game_end_flg = false;
    int jewel_num = fromString<int>(SaveData::loadString("jewel")) - json_master["reborn_jewel"].int_value();//石を減らす（ローカル側）
    SaveData::saveString("jewel", to_string(jewel_num));
    
    player[TurnManage::getPlayerIndex()]->setHp(player[TurnManage::getPlayerIndex()]->getMaxHp());
    uiLayer->updatePlayerInfo(player[TurnManage::getPlayerIndex()]->getMaxHp(), player[TurnManage::getPlayerIndex()]->getMaxHp(), TurnManage::getPlayerIndex());
    
    winner_string="";
    CharaList::getLeader(TurnManage::getPlayerIndex())->rebornLeader();
    setState("Directioning");
    
}

string DungeonScene::getBattleMode()
{
    string mode = "solo";
    if(SaveData::loadString("battle_mode")=="vs") mode = "vs";

    return mode;
}

bool DungeonScene::isEventBattle(){
    bool isEvent = true ;
    string npc_id_str = SaveData::loadString("npc_battle_event_rank_list_id") ;
    if(npc_id_str=="0" || npc_id_str==""){
        isEvent = false ;
    }
    return isEvent;
}

bool DungeonScene::isRankingEventBattle(){
    bool is_ranking_event = true ;
    string stage_id_str = SaveData::loadString("ranking_event_stage_id") ;
    if(stage_id_str=="0" || stage_id_str==""){
        is_ranking_event = false ;
    }
    return is_ranking_event;
}

bool DungeonScene::isBossEventBattle(){
    bool is_boss_event = true ;
    string stage_id_str = SaveData::loadString("boss_event_battle_log_id") ;
    if(stage_id_str=="0" || stage_id_str==""){
        is_boss_event = false ;
    }
    return is_boss_event;
}

bool DungeonScene::isChallengeEventBattle(){
    bool is_challenge_event = true ;
    string rank_list_id_str = SaveData::loadString("challenge_event_stage_id") ;
    if(rank_list_id_str=="0" || rank_list_id_str==""){
        is_challenge_event = false ;
    }
    return is_challenge_event;
}

bool DungeonScene::isTournamentEventBattle(){
    bool is_tournament_event = true ;
    string tournament_event_list_id = SaveData::loadString("tournament_event_list_id") ;
    if(tournament_event_list_id=="0" || tournament_event_list_id==""){
        is_tournament_event = false ;
    }
    return is_tournament_event;
}

bool DungeonScene::isTutorialNaviActive()
{
    return (this->getChildByTag(TUTORIAL_TIP_LAYER_TAG_ID)!=nullptr);
}

bool DungeonScene::isTutorial()
{
    if(SaveData::loadInt("tutorialStatus") == 1){
        return true ;
    } else {
        return false;
    }
}

bool DungeonScene::isQuestBattle()
{
    //クエストかどうか判定
    if (fromString<int>(SaveData::loadString("quest_stage_id")) > 0) {
        return true;
    }
    else {
        return false;
    }
}

//チュートリアル用
void DungeonScene::tutorialNaviSkill(float dt)
{
    tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_12);
    tutorialNaviLayer->setScheduler(this, nullptr);
    
    //攻撃ボタンを有効にする
    uiLayer->attack_button_compel_false = false;
    uiLayer->changeAttackButton("attack_encourage");
}

//通信用データ作成
Json DungeonScene::getBattleData()
{
    
    map<string, Json> all_in_data;
    for(int ii=0; ii<2; ii++) {
        
        string deck_data = "";
        for(Chara *chara : deck[ii]) {
            if(deck_data!="") deck_data += ",";
            deck_data += to_string(chara->getPrimaryId());
        }
        
        Json whole_status_active_json = Json(player[ii]->whole_status_active);
        Json whole_status_passive_json = Json(player[ii]->whole_status_passive);
        
        all_in_data.insert(map<string, Json>::value_type("pow"+to_string(ii), Json(player[ii]->getNowPower())));
        all_in_data.insert(map<string, Json>::value_type("hp"+to_string(ii), Json(player[ii]->getHp())));
        all_in_data.insert(map<string, Json>::value_type("m_hp"+to_string(ii), Json(player[ii]->getMaxHp())));
        all_in_data.insert(map<string, Json>::value_type("deck"+to_string(ii), Json(deck_data)));
        all_in_data.insert(map<string, Json>::value_type("wle_act"+to_string(ii), whole_status_active_json));
        all_in_data.insert(map<string, Json>::value_type("wle_pas"+to_string(ii), whole_status_passive_json));
    }
    
    vector<Json> chara_vec;
    for(int i=0; i<CharaList::chara_list.size(); i++){
        chara_vec.push_back(CharaList::chara_list.at(i)->exportData());
    }
    
    all_in_data.insert(map<string, Json>::value_type("all_cha", Json(chara_vec)));
    all_in_data.insert(map<string, Json>::value_type("t_cnt", Json(TurnManage::getTurnCount())));//ターンカウント
    all_in_data.insert(map<string, Json>::value_type("che", Json(status_check_sum)));//チェックサム
    
    if(getBattleMode()=="solo") {
        all_in_data.insert(map<string, Json>::value_type("param_nodamage_flg",Json(param_nodamage_flg)));
        all_in_data.insert(map<string, Json>::value_type("param_noskill_flg",Json(param_noskill_flg)));
        all_in_data.insert(map<string, Json>::value_type("score_param_damage",Json(score_param_damage)));
        all_in_data.insert(map<string, Json>::value_type("score_param_chara_count",Json(score_param_chara_count)));
        all_in_data.insert(map<string, Json>::value_type("score_param_skill_count",Json(score_param_skill_count)));
        all_in_data.insert(map<string, Json>::value_type("score_param_overkill",Json(score_param_overkill)));
    }
    
   //チェックサムデバッグ用
    int sum = 0;
    sum += TurnManage::getTurnCount();
    for(int ii=0; ii<2; ii++){
        sum += player[ii]->getHp();
        sum += player[ii]->getNowPower();
        for(Chara *chara : CharaList::getOneSide(ii)){
            sum += chara->getValue();
        }
    }
    //HMSLOG("ゲットデータ時チェックサム：%d , パブリック変数：%d",sum, status_check_sum);
    return Json(all_in_data);//Jsonに変換したもの
}

void DungeonScene::setBattleData(Json json_data)
{
    //HMSLOG("json_string=%s", json_string.c_str());むー
    

    map<string, Json> json = json_data.object_items();
    
    for(int ii=0; ii<2; ii++) {
        CCLOG("ジェイソンセット：%d", json["hp"+to_string(ii)].int_value());
        player[ii]->setNowPower(json["pow"+to_string(ii)].int_value());
        if(isBossEventBattle() && ii==1) {
            //ボスイベントの敵はサーバから最新のHPを受け取っているので復帰させない。
        } else {
            player[ii]->setHp(json["hp"+to_string(ii)].int_value());
        }
        player[ii]->setMaxHp(json["m_hp"+to_string(ii)].int_value());
        
        
        //deck更新
        deck[ii].clear();
        HmsVector<string> deck_data_list = split(json["deck"+to_string(ii)].string_value(), ",");
        for(int i=0; i<deck_data_list.size(); i++) {
            deck[ii].push_back( CharaList::getAtPrimary(fromString<int>(deck_data_list.at(i))));
        }
        
        //全体スキルの更新
        player[ii]->whole_status_active.clear();
        for(Json temp_map : Json(json["wle_act"+to_string(ii)]).array_items()){
            map<string, Json> aaa = temp_map.object_items();
            map<string, int>  temp;;
            auto itr = aaa.begin();
            for(itr = aaa.begin(); itr!=aaa.end(); itr++){
                temp.insert(map<string, int>::value_type(itr->first, itr->second.int_value()));
            }
            player[ii]->whole_status_active.push_back(temp);
        }
        
        player[ii]->whole_status_passive.clear();
        for(Json temp_map : Json(json["wle_pas"+to_string(ii)]).array_items()){
            map<string, Json> aaa = temp_map.object_items();
            map<string, int>  temp;;
            auto itr = aaa.begin();
            for(itr = aaa.begin(); itr!=aaa.end(); itr++){
                temp.insert(map<string, int>::value_type(itr->first, itr->second.int_value()));
            }
            player[ii]->whole_status_passive.push_back(temp);
        }
    }
    
    //キャラデータを更新する
    vector<Json> vec_charas = json["all_cha"].array_items();
    for(int i=0; i<vec_charas.size(); i++){
        map<string, Json> map_chara = vec_charas.at(i).object_items();
        
        Chara *same_chara = CharaList::getAtPrimary(i);
        
        if(same_chara!=nullptr){//オールキャラリストに同じプライマリがある場合は
            same_chara->importData(vec_charas.at(i));//a上書き
            CCLOG("データの上書き%d",i);
            
        }else{
            CCLOG("all_chara_listの中に無かったので作成します");
            Chara *chara = Chara::create();
            cardLayer->addChild(chara);
            chara->importData(vec_charas.at(i));//データをインポート
            CharaList::chara_list.insert(chara);//キャラリストに加え
            chara->initialize();//初期化
        }
    }
    
    TurnManage::setTurnCount(json["t_cnt"].int_value());
    int check_data = json["che"].int_value();
    HMSLOG("セットデータのチェックサム：%d",check_data);
    
    if(getBattleMode()=="solo") {
        param_nodamage_flg = json["param_nodamage_flg"].int_value();
        param_noskill_flg = json["param_noskill_flg"].int_value();
        score_param_damage = json["score_param_damage"].int_value();
        score_param_chara_count = json["score_param_chara_count"].int_value();
        score_param_skill_count = json["score_param_skill_count"].int_value();
        score_param_overkill = json["score_param_overkill"].int_value();
    }
    
    HMSLOG("setBattleData() finish");
}

void DungeonScene::printBattleData()
{
    for(int ii=0; ii<2; ii++) {
        string player_no = to_string(ii+1);
        string header = "###p" + player_no + ":";
        string deck_data = "";
        string chara_list_data = "";
        string chara_data = "";
        for(int i=0; i<deck[ii].size(); i++) {
            Chara *chara = deck[ii].at(i);
            //int primary_id = chara->getPrimaryId();
            if(deck_data!="") deck_data += ",";
            deck_data += to_string(chara->getPrimaryId());
        }
        for(Chara *chara : CharaList::getOneSide(ii)){
            //int primary_id = chara->getPrimaryId();
            if(chara_list_data!="") chara_list_data += ",";
            chara_list_data += to_string(chara->getPrimaryId());
            
            if(chara_data!="") chara_data += "\n";
            chara_data += header + chara->exportData().dump();
        }
        
        HMSLOG("%s", (header + "power=" + to_string(player[ii]->getPower())).c_str());
        HMSLOG("%s", (header + "player_hp=" + to_string(player[ii]->getHp())).c_str());
        HMSLOG("%s", (header + "deck_data=" + deck_data).c_str());
        HMSLOG("%s", (header + "chara_list_data=" + chara_list_data).c_str());
        //HMSLOG("%s", (header + "chara_data=\n" + chara_data).c_str());
    }
    HMSLOG("printBattleData finish");
}

void DungeonScene::checkIllegal(string text, bool check_flg)
{
    if(player[TurnManage::getPlayerIndex()]->getHp()<=0||player[TurnManage::getEnemyIndex()]->getHp()<=0) return;//ゲームオーバー後はチェックしない
    //if(isTutorial()) return;//チュートリアルではチェックしない
    
    if(check_flg==true) {//データの変化がないかどうかをチェック。各アクションの開始時に使用。
        string data = getBattleData().dump();

        if(battle_data_for_check!="" && data!=battle_data_for_check) {
            CCLOG("イリーガル３");
            //string err;
            //Json json_new = Json::parse(data, err);
            //Json json_old = Json::parse(battle_data_for_check, err);
            HMSLOG("###checkIllegal[%s]:NG", text.c_str());
            HMSLOG("%s", data.c_str());
            HMSLOG("%s", battle_data_for_check.c_str());
            int illegal_count = SaveData::loadInt("ic");
            illegal_count++;
            SaveData::saveInt("ic", illegal_count);
            
            string checksum = HmsCrypto::makeCheckSum("hms" + toString(illegal_count));
            SaveData::saveString("ics", checksum);
            
            if(replay_flg==false) {
                string user_master_id = SaveData::loadString("user_master_id");
                string url = HOST;
                url += "battle/illegal.php";
                string body = "user_master_id=" + user_master_id;
                body += "&data_new=" + URLEncode(data);
                body += "&data_old=" + URLEncode(battle_data_for_check);
                auto selector = httpresponse_selector(DungeonScene::notifyIllegalCallback);
                HttpConnect::sendHttpRequest(this, url, selector, body);
            }
        } else {
            HMSLOG("###checkIllegal[%s]:OK", text.c_str());
        }
    } else {//チェック用のデータ保存。各アクションの終了時に使用。
        battle_data_for_check = getBattleData().dump();
        HMSLOG("###checkIllegal[%s]:data save", text.c_str());
    }
}

void DungeonScene::notifyIllegalCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    string str = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("response:%s", str.c_str());
    //通信エラー以外
    if(str != "not connect" && str != "response NG") {

    }
}

//ボスイベント用
void DungeonScene::sendCommand()
{
    if(damage_list.size()>0) {
        string damage_str = "";
        string delim = "";
        for(int i=0; i<damage_list.size(); i++) {
            damage_str += delim + toString(damage_list.at(i));
            delim = ":";
        }
        string command = "damage_" + SaveData::loadString("user_master_id") + "_" + damage_str + "_" + to_string(player[0]->getHp());
        
        string url = HOST;
        url += "map/send_command_data.php?";
        url += "user_master_id=" + SaveData::loadString("user_master_id");
        url += "&boss_event_stage_log_id=" + SaveData::loadString("boss_event_stage_log_id");
        url += "&command=" + command;
        
        HMSLOG("url=%s", url.c_str());
        
        HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
            if(response->isSucceed()){
                sendCommandDataCallback(sender, response);
            }else{
                this->runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this](){//接続失敗したら時間おいて再送信
                    sendCommand();
                }), NULL));
            }
        });
        
        damage_list.clear();
    }
}

void DungeonScene::sendCommandDataCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    string str = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("response:%s", str.c_str());
    
}

void DungeonScene::getDamageData()
{
    string url = HOST;
    url += "map/get_damage_data.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + toString(boss_event_stage_log_id);
    url += "&boss_event_stage_command_history_id=" + toString(boss_event_stage_command_history_id);
    url += "&enemy_user_master_id=" + toString(boss_event_enemy_user_master_id);

    HMSLOG("url=%s", url.c_str());
    
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        if(response->isSucceed()){
            getDamageDataCallback(sender, response);
        }else{
            this->runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this](){//接続失敗したら時間おいて再送信
                getDamageData();
            }), NULL));
        }
    });
}

void DungeonScene::getDamageDataCallback(HttpClient *sender, HttpResponse *response)
{
    //結果を設定
    string str = HttpConnectHms::getHttpResponse(this, sender, response);
    HMSLOG("response:%s", str.c_str());
    
    string err;
    Json json = Json::parse(str, err);
    
    //応援データ
    HmsVector<string> support_user_name_list_new;
    int support_user_count = json["support_user_count"].int_value();
    for(int i=0; i<support_user_count; i++) {
        string user_name = json["support_user_list"][toString(i)]["user_name"].string_value();
        support_user_name_list_new.insert(user_name);
    }
    support_user_name_list = support_user_name_list_new;
    support_info_layer->setBossEventSupportInfo(support_user_name_list);
    
    //仲間のダメージデータ
    boss_event_stage_command_history_id = fromString<long>(json["boss_event_stage_command_history_id"].string_value());
    int data_count = json["data_count"].int_value();
    float user_cut_in_time = 1.6f;//カットイン再生時間
    
    for(int i=0; i<data_count; i++) {
        string user_name = json["data_list"][toString(i)]["user_name"].string_value();
        string damage_str = json["data_list"][toString(i)]["damage"].string_value();
        int chara_id = json["data_list"][toString(i)]["chara_id"].int_value();
        HMSLOG("user_name=%s,chara_id=%d,damage_str=%s", user_name.c_str(), chara_id, damage_str.c_str());
        
        HmsVector<string> damage_str_list = split(damage_str, ":");
        int total_damage = 0;
        for(int j=0; j<damage_str_list.size(); j++) {
            total_damage += fromString<int>(damage_str_list.at(j));
        }
        HMSLOG("total_damage=%d", total_damage);
        
        float delay_time = user_cut_in_time * i;
        DelayTime *delay1 = DelayTime::create(delay_time);
        DelayTime *delay2 = DelayTime::create(1.5f);
        CallFunc *func1 = CallFunc::create([this, chara_id, user_name, total_damage, user_cut_in_time, i]()
                                           {
                                               //#########カットイン演出
                                               UserCutInLayer* user_cut_in_layer = UserCutInLayer::create();
                                               user_cut_in_layer->setData(chara_id, user_name, total_damage, user_cut_in_time);
                                               this->addChild(user_cut_in_layer, 100);
                                           });//ラムダ式
        CallFunc *func2 = CallFunc::create([this, total_damage, i]()
                                           {
                                               //#########攻撃演出
                                               int enemy_index = TurnManage::getEnemyIndex();
                                               Chara *target_chara = CharaList::getLeader(enemy_index);
                                               
                                               int index = i % 4;
                                               string effect_filename = "zan1.plist";
                                               if(index==1) {
                                                   effect_filename = "zan2.plist";
                                               } else if(index==2) {
                                                   effect_filename = "zan3.plist";
                                               } else if(index==3) {
                                                   effect_filename = "zan4.plist";
                                               }
                                               
                                               ParticleSystemQuad* particle = ParticleSystemQuad::create("download/Effect/Particle/" + effect_filename);
                                               particle->setAutoRemoveOnFinish(true);
                                               particle->setPosition(target_chara->getPosition());
                                               
                                               if(total_damage>=300){
                                                   particle->setScale(ratio*1.45f);
                                               }else if(total_damage>=200){
                                                   particle->setScale(ratio*1.30f);
                                               }else if(total_damage>=100){
                                                   particle->setScale(ratio*1.15f);
                                               }else if(total_damage>=50){
                                                   particle->setScale(ratio*1.0f);
                                               }else{
                                                   particle->setScale(ratio*0.9f);
                                               }
                                               
                                               effectLayer->addChild(particle, 2);
                                               
                                               ParticleSystemQuad* particle2 = ParticleSystemQuad::create("download/Effect/Particle/defense.plist");//火花エフェクト
                                               particle2->setAutoRemoveOnFinish(true);
                                               particle2->setScale(0.4f*ratio);
                                               particle2->setPosition(target_chara->getPosition());
                                               effectLayer->addChild(particle2, 1);
                                               
                                               ParticleSystemQuad* hit1 = ParticleSystemQuad::create("download/Effect/Particle/hit1.plist");
                                               hit1->setAutoRemoveOnFinish(true);
                                               hit1->setPosition(target_chara->getPosition());
                                               hit1->setScale(ratio);
                                               effectLayer->addChild(hit1, 1);
                                               
                                               //防御側が赤く点滅
                                               target_chara->displayRedBlink(0.5f);
                                               
                                               //SE
                                               CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/slash2.mp3");
                                               
                                               //ダメージ表示
                                               target_chara->displayDamage(total_damage);
                                               
                                               //ダメージ更新
                                               //int hp = player[enemy_index]->getHp();
                                               player[enemy_index]->damage(total_damage, false);
                                               uiLayer->updatePlayerInfo(player[enemy_index]->getHp(), player[enemy_index]->getMaxHp(), enemy_index);
                                           });//ラムダ式
        this->runAction(Sequence::create(delay1, func1, delay2, func2, NULL));
    }
    
    //仲間のカットイン表示後、ターン切り替え
    //ターンの切り替え
    this->scheduleOnce(schedule_selector(DungeonScene::changeTurn), user_cut_in_time*data_count);
    
}

//通信時対戦相手の演出終了
void DungeonScene::finishRivalEffect(float dt)
{
    HMSLOG("finishRivalEffect()");
    this->data_exec_flg = false;
    this->runAction(Sequence::create(DelayTime::create(0.35f),
                                     CallFunc::create([this](){this->cpu_moving = false;
        
      
    
    }),nullptr)
                    );
    if(data_recovery_flg==false && getBattleMode()=="vs") {
        data_recovery_flg = true;
        setTurn(0);
    }
}

//正常運転可能かどうか
string DungeonScene::getVsState()
{
    if(popup_restart_flg==true){
        CCLOG("getVsState() :: popup_restart_flg==true");
        return "restart";
    }
    if(initialize_effect_flg==false){
        CCLOG("getVsState() :: initialize_effect_flg==false");
        return "initialize_effect";
    }
    if(data_recovery_flg==false){
        CCLOG("getVsState() :: data_recovery_flg==false");
        return "regist";
    }
    if(first_turn_set_end==false){
        CCLOG("getVsState() :: first_turn_set_end==false");
        return "set_turn";
    }
    
    return "ok";
}

//バックグラウンドからの復旧後、タイトルシーンへ
void DungeonScene::popupRestartMessage()
{
    if(popup_restart_flg==true) return;
    
    popup_restart_flg = true;

    auto layer = PopupOk::create();
    layer->setPopup("バトルが中断されたため、\n一度タイトルシーンに戻ります", [this](){
        //タイトルシーンへ
        auto scene = TitleScene::createScene( );
        auto director = Director::getInstance();
        director->replaceScene(scene);
    });
    this->addChild(layer, 1000000);//一番上
}

void DungeonScene::sendMessage()//コマンド類のメッセージ送信。送る順番が正しくなるようスタックしてあるものを順番に送信
{
    if(sending_message_flg == true) return;//現在送信中の時はリターン
    if(send_message_vec.size()==0) return;//送るメッセージがない時はリターン
    sending_message_flg = true;//現在送信中
    
    if(send_message_vec.size()>1){
        CCLOG("たまっているメッセージ送信:%dこ",(int)send_message_vec.size());
        for(auto message_map : send_message_vec){
            CCLOG("メッセージタイプ：%s", message_map["type"].dump().c_str());
        }
    }
    
    //ストックしてあるメッセージの最初を送信
    string  message = Json(send_message_vec.front()).dump();
    send_message_vec.erase(send_message_vec.begin());
    
    
    string url = HOST;
    url += "battle/execute_command.php";
    string body = "";
    body += "user_master_id=" + SaveData::loadString("user_master_id");
    body += "&battle_list_id=" + toString(battle_list_id);
    body += "&player_no=" + toString(TurnManage::getPlayerNo());
    body += "&turn_no=" + toString(TurnManage::getTurnNo());
    body += "&last_battle_log_id=" + toString(last_battle_log_id);
    if(isTournamentEventBattle()) body += "&battle_mode=Tournament";
    body += "&data=" + URLEncode2(message);
    
    //送信に対してのコールバック
    HttpConnectHms::sendHttpRequest(url, [this,message](HttpClient *client, HttpResponse *response){
        sending_message_flg = false;//送信処理終了
        
        if(response->isSucceed()){//送信成功
            
            std::vector<char> *buffer = response->getResponseData();
            std::string str = std::string("");
            for(int i=0; i<buffer->size(); i++) {
                str.append(1, (*buffer)[i]);
            }
            
            if(str!="") {
                onMessage(str);
            }
            
            if(send_message_vec.size()>0){
                CCLOG("データ追加送信");
                sendMessage();
            }
            
        }else{
            CCLOG("execute送信失敗");
            
            string err;
            Json json_message = Json::parse(message, err);
            map<string, Json> json_map = json_message.object_items();
            
            send_message_vec.insert(send_message_vec.begin(), json_map);//データ送信の初めに入れ直し、再送信
            sendMessage();//空のベクタをいれて送信し直し
        }
    }, body);
}

void DungeonScene::sendMessageOthers(float dt)//対戦時に２秒ごとに定期的に送られるメッセージ
{
    
    map<string , Json> send_map;
    send_map.insert(make_pair("type", Json("others")));
    send_map.insert(make_pair("player_no", Json(TurnManage::getPlayerNo())));
    send_map.insert(make_pair("turn_no", Json(TurnManage::getTurnNo())));
    
    string message = Json(send_map).dump();
    
    string url = HOST;
    url += "battle/execute_command.php";
    string body = "";
    body += "user_master_id=" + SaveData::loadString("user_master_id");
    body += "&battle_list_id=" + toString(battle_list_id);
    body += "&player_no=" + toString(TurnManage::getPlayerNo());
    body += "&turn_no=" + toString(TurnManage::getTurnNo());
    body += "&last_battle_log_id=" + toString(last_battle_log_id);
    if(isTournamentEventBattle()) body += "&battle_mode=Tournament";
    body += "&data=" + URLEncode2(message);
    
    //送信に対してのコールバック
    HttpConnectHms::sendHttpRequest(url, [this,message](HttpClient *client, HttpResponse *response){
        if(response->isSucceed()){
            string message = HttpConnectHms::getHttpResponse(this, client, response);
            
            if(message!="") {
                onMessage(message);
            }
            
        }else{
            CCLOG("execute送信失敗（others）");
            sendMessageOthers(0.0f);//再送信
        }
    }, body);
}

string DungeonScene::URLEncode(string input)
{
    const unsigned char chars[] = "0123456789ABCDEF";
    
    unsigned char *output = new unsigned char[input.size()*3 + 1 ];
    unsigned char *in = (unsigned char*)input.c_str();
    unsigned char* out = output;
    
    for( ; *in!='\0' ; in++ ){
        if ((*in >= 'A' && *in <= 'Z') || (*in >= 'a' && *in <= 'z') || (*in >= '0' && *in <= '9') ||
            *in == '=' || *in == '-' || *in == '_' || *in == '.' || *in == '~') {
            *out++ = (char)*in;
        } else {
            *out++ = '%';
            *out++ = chars[*in >> 4];
            *out++ = chars[*in & 0x0f];
        }
    }
    *out = '\0';
    
    std::string sout = (char*)output;
    delete output;
    
    return sout;
}
string DungeonScene::URLEncode2(string &str)
{
    std::ostringstream os;
    
    for (int i = 0; i < str.size(); i++) {
        char c = str[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            os << c;
        }else if(c == ' '){
            
        } else {
            char s[4];
            snprintf(s, sizeof(s), "%%%02x", c & 0xff);
            os << s;
        }
    }
    
    return os.str();
}


void DungeonScene::onMessage(string message) {
    CCLOG("受信：%s",message.c_str());
    
    string err;
    Json json_message = Json::parse(message, err);
    map<string, Json> json_map = json_message.object_items();
    
    
    if(json_map["type"].string_value()=="surrender") {
        gameoverJudge("surrender");
        return;
        
    }else if(json_map["type"].string_value()=="time_up") {
        gameoverJudge("time_up");
        return;
        
    }else if(json_map["type"].string_value()=="status" || json_map["type"].string_value()=="change_turn") {
        if(getVsState()!="ok") return;
        
        //残り時間と接続状態の更新
        if(json_map["type"].string_value()=="status") {
            finish_rest_time = json_map["finish_rest_time"].int_value();
            auto delay = DelayTime::create(1.0f);
            auto func = CallFunc::create([this](){
                if(this->finish_rest_time > 0)this->finish_rest_time --;
            });
            auto repeat = RepeatForever::create(Sequence::create(delay,func, NULL));
            stopActionByTag(9989);
            repeat->setTag(9989);
            runAction(repeat);
            
            
            last_time[0] = json_map["last_time0"].int_value();
            last_time[1] = json_map["last_time1"].int_value();
            connection_flg[0] = json_map["connection0"].int_value();
            connection_flg[1] = json_map["connection1"].int_value();
        }
        
        //スタンプの表示
        if(json_map["stamp_data"].is_number()){
            CCLOG("スタンプ:%d",json_map["stamp_data"].int_value());
            
            int stamp_int = json_map["stamp_data"].int_value();
            int count = 0;
            
            while(true){
                int temp = stamp_int % 100;//100で割った余り
                runAction(Sequence::create(DelayTime::create(1.0f*count),CallFunc::create([this,temp](){//１秒ごとに順番に再生
                    uiLayer->showStamp(temp+1, TurnManage::getEnemyIndex());
                }), NULL));
                stamp_int = (floor)(stamp_int/100.0f);
                count++;
                if(stamp_int == 0) break;
            }
        }

    
        //ターン切り替え
        int now_turn_no = json_map["turn_no"].int_value();
        
        string err;
        Json json = Json::parse(message, err);
        
        //まずセットターン、その後にplaceなどのdata_receivedがとおる
        if(TurnManage::getTurnNo()!=now_turn_no && data_exec_flg==false && data_received.size()==0) {//ターン変更メッセージ
            //HMSLOG("セットターン：%d",first_turn_flg);
            data_exec_flg = true;
            setTurn(now_turn_no);
            
        }else if(TurnManage::getTurnNo()!=now_turn_no){
            HMSLOG("ターンチェンジウェイト");
            turn_change_wait = true;//ターン変更メッセージが来ているフラグ。ターン変更したらfalseに
        }
        
    }else if(json_map["type"].string_value()=="regist") {
        
        data_recovery_flg = true;
        last_battle_log_id = json_map["battle_log_id"].int_value();
        
        if(last_battle_log_id == 0){
            setTurn(json_map["turn_no"].int_value());
        }else{
            first_turn_set_end = true;
        }
        
    } else if(json_map["type"].string_value()=="place_character" || json_map["type"].string_value()=="use_skill" || json_map["type"].string_value()=="attack"){//コマンド類の場合
        
        CCLOG("バトルデータ：：%d",(int)json_map["battle_data"].dump().size());
        CCLOG("コマンドナンバー：%d",json_map["battle_log_id"].int_value());
        if(last_battle_log_id < json_map["battle_log_id"].int_value() && (int)json_map["battle_data"].dump().size() > 10){//同じバトルログは実行しない・バトルデータが無いものは実行しない
            data_received.insert(message);
            CCLOG("%sのコマンド追加",json_map["type"].string_value().c_str());
            //last_battle_log_id = json_map["battle_log_id"].int_value();
        }
        
        if(json_map["battle_log_id"].int_value()>0) {
            last_battle_log_id = json_map["battle_log_id"].int_value();
        }
        
    }else{
        CCLOG("その他のコマンド？%s",message.c_str());
    }
}


//タップ処理
bool DungeonScene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(replay_flg==true) return true;
    if(getVsState()!="ok" && getBattleMode()=="vs") return true;
    if(tapped_chara) return true;
    
	move_distance = 0;
	long_tap_check = false;
	
	Vec2 tap_position = touch->getLocation();
    tapped_chara = nullptr;
    
    //カードの上をタップした時はtapped_charaにキャラを入れる
    for(Chara *chara : CharaList::getAll()){
        if((fieldLayer->getPositionIndex(tap_position, "field", chara->getBelongingIndex()) ==  chara->getFieldIndex() && chara->isOnField())
           || (fieldLayer->getPositionIndex(tap_position, "hand", chara->getBelongingIndex()) == chara->getHandIndex() && chara->isInHand())
           || (fieldLayer->getPositionIndex(tap_position, "leader", chara->getBelongingIndex()) == 1 && chara->getType() == CHARA_TYPE_LEADER)
           || (fieldLayer->getPositionIndex(tap_position, "friend", chara->getBelongingIndex()) == 1 && chara->isFriend())){
            tapped_chara = chara;
            tapped_id = touch->getID();
            CCLOG("タップID:%d",tapped_id);
        }
    }
    
    if(tapped_chara != nullptr){//キャラクターをタップした時
        HMSLOG("このカードは ID %d,primary %d,type %d,active %d,passive %d,value %d,combo_value %d,base_value %dです",tapped_chara->getCharaId(),tapped_chara->getPrimaryId(),tapped_chara->getType(),tapped_chara->getActiveSkillId(),tapped_chara->getPassiveSkillId(),tapped_chara->getValue(),tapped_chara->combo_value,tapped_chara->base_value);
        
        cardLayer->reorderChild(tapped_chara, 1000);//前面へ
        
        if(tapped_chara->isMyChara()){//自分のカードの場合
            tapped_chara->charaNameTab(true);//キャラの名前表示
            
            if(isCharacterPlacable()){//自分のターンでキャラ配置可能な時
                
                if(tapped_chara->skillAble() && tapped_chara->isMyChara()){//スキル使用可能なキャラは
                    skill_confirm_layer->showDetail(tapped_chara);
                    
                }else if(tapped_chara->getHandIndex()>=0 && tapped_chara->moveAble()){//自分の手札かフレンドの出すことのできるキャラ
                    
                    tapped_chara->scaleUp(true);
                    
                    if(tapped_chara->isFriend()==false){
                        tapped_revision = tap_position - fieldLayer->hand_position_list[TurnManage::getPlayerIndex()].at(tapped_chara->hand_index);
                    }else{//フレンドの場合
                        tapped_revision = tap_position - fieldLayer->friend_position_list[TurnManage::getPlayerIndex()];
                    }
                    
                    
                    moveAbleEffect();//どこに移動できるかのエフェクト
                    selectAbleEffect(false);//選択可能キャラエフェクト消去
                    //SE
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/shake_clock.mp3");
                    
                    //タップ時間のカウントスタート
                    tap_time = 0;
                    this->schedule(schedule_selector(DungeonScene::longTapCounter), 0.1f);
                    
                }else{//スキル使えないし出せないキャラ
                    
                    //タップ時間のカウントスタート
                    tap_time = 0;
                    this->schedule(schedule_selector(DungeonScene::longTapCounter), 0.1f);
                }
            }else{//敵のターンもしくは演出中
                //タップ時間のカウントスタート
                tap_time = 0;
                this->schedule(schedule_selector(DungeonScene::longTapCounter), 0.1f);
            }
            
        }else{//敵のカードの場合
            if(tapped_chara->card_state == "open"){//表のカードに対して
                
                //タップ時間のカウントスタート
                tap_time = 0;
                this->schedule(schedule_selector(DungeonScene::longTapCounter), 0.1f);
                tapped_chara->charaNameTab(true);//キャラの名前表示
            }
        }
    }

	return true;
}

void DungeonScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(replay_flg==true) return;
    if(turn_change_wait == true) return;
    if(getVsState()!="ok" && getBattleMode()=="vs") return;
    if(touch->getID() != tapped_id) return;
    
	move_distance = calculateDistance(touch->getStartLocation(), touch->getLocation());

	//スキル詳細を出している場合、スキル詳細の除去とキャラを移動可能にする。
	if(move_distance>=30*ratio){
        skillCheckLayer->close();
	}

	//タップしたキャラがいる場合は、キャラを動かす。
	if(isCharacterPlacable() && tapped_chara!=nullptr && tapped_chara->moveAble() && tapped_chara->isMyChara()) {
		Vec2 tap_position = touch->getLocation();
		//HMSLOG("tap moved at %f,%f", tap_position.x, tap_position.y);
		
		tapped_chara->setPosition(tap_position-tapped_revision);
		
		//フィールド上に来た場合は表示を切り替える
		int field_index = fieldLayer->getPositionIndex(tap_position-tapped_revision, "field", TurnManage::getPlayerIndex());
		if(field_index==-1) {
			fieldLayer->cardSelectImage->setVisible(false);
		} else {
			
			if(fieldLayer->equalPositionType(tapped_chara, field_index, TurnManage::getPlayerIndex())){//下のフィールドがキャラタイプと一致する時
                //フィールド位置に応じてスプライトの縦横比を調整
                if (field_index <= 1) {//軍師フィールド
                    fieldLayer->cardSelectImage->setScale(0.34f*ratio,0.315f*ratio);
                }else{//攻撃防御フィールド
                    fieldLayer->cardSelectImage->setScale(0.315f*ratio);
                }
				fieldLayer->cardSelectImage->setPosition(fieldLayer->field_position_list[TurnManage::getPlayerIndex()].at(field_index));
				fieldLayer->cardSelectImage->setVisible(true);
				
			}
			//SE
			CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/exp.mp3");
		}
	}
}

void DungeonScene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(touch->getID() != tapped_id) return;
    
    //スキル詳細が表示されている場合、スキル詳細を除去する。
    skillCheckLayer->close();
    
    
    if(getBattleMode()=="vs" && getVsState()!="ok") return;
    
	move_distance = 0;
    
	Vec2 tap_position = touch->getLocation();
    
	fieldLayer->cardSelectImage->setVisible(false);//カードがそこに置ける時に表示する画像非表示
	uiEffectLayer->reset_place_frame_effect();//移動できる場所表示エフェクト非表示
	
    
    //カードの上をタップした時はtapped_charaにキャラを入れる
    
    
    //手を離した時に下にあるキャラ
    Chara *detach_chara = nullptr;
    for(Chara *chara : CharaList::getAll()){
        if((fieldLayer->getPositionIndex(tap_position, "field", chara->getBelongingIndex()) ==  chara->getFieldIndex() && chara->isOnField())
           || (fieldLayer->getPositionIndex(tap_position, "hand", chara->getBelongingIndex()) == chara->getHandIndex() && chara->isInHand())
           || (fieldLayer->getPositionIndex(tap_position, "leader", chara->getBelongingIndex()) == 1 && chara->getType() == CHARA_TYPE_LEADER)
           || (fieldLayer->getPositionIndex(tap_position, "friend", chara->getBelongingIndex()) == 1 && chara->isFriend())){
            detach_chara = chara;
        }
    }
    
    
    if(tapped_chara!=nullptr){
        
        if(isCharacterPlacable() && tapped_chara->moveAble() && tapped_chara->isMyChara()) {//手札から移動中のキャラを離した時 自分のキャラの時
            
            int field_index = fieldLayer->getPositionIndex(tap_position-tapped_revision, "field", TurnManage::getPlayerIndex());//タップした場所のfield
            
            if(fieldLayer->equalPositionType(tapped_chara,field_index,TurnManage::getPlayerIndex())){//タップしたキャラを置くことができる場所で
                
                if(fieldLayer->getCharaFieldIndex(field_index, TurnManage::getPlayerIndex())!=nullptr){//既にキャラがいる時
                    //キャラを置き換える場合は、確認画面を表示する
                    if(!isTutorial()){ //チュートリアルの場合キャラ交換無し
                        Chara *replace_chara = fieldLayer->getCharaFieldIndex(field_index, TurnManage::getPlayerIndex());
                        ReplaceConfirmLayer *replace_confirm_layer = ReplaceConfirmLayer::create();
                        addChild(replace_confirm_layer,10);
                        replace_confirm_layer->setData(replace_chara, tapped_chara);
                    }else{
                        restoreChara(tapped_chara);
                    }
                }else{
                    //キャラを配置する
                    placeCharacter(tapped_chara, field_index);
                }
                
            }else{//それ以外の時
                restoreChara(tapped_chara);//キャラを元の位置に戻す
            }
        }
        
    }else{//キャラをタップしていない時
        /*if(detach_chara!=nullptr && detach_chara->card_state == "open"){//表のカードに対して
            detach_chara->mascotScale();
        }*/
    }
    
	//タップ情報をクリアする
    resetTapInfo();
	
}

void DungeonScene::longTapCounter(float dt)
{
	tap_time += dt;
	
	//HMSLOG("%f,%f", tap_time, move_distance);
    if(tap_time>0.08f && move_distance<16*ratio && !long_tap_check) {//ロングタップした場合
        if(tapped_chara->statusCheck(STATUS_TYPE_DARKNESS)==true && tapped_chara->isAttackAndDefense() && tapped_chara->getBelongingIndex() == TurnManage::getEnemyIndex()){//ダークネスされている敵フィールドの武将キャラ
            
        }else{//ダークネスされていない時
            
            skillCheckLayer->setData(tapped_chara, TurnManage::getPlayerIndex());
            long_tap_check = true;
        }
    }
    
    if(move_distance>20*ratio) {//スワイプで指定以上の距離を移動した場合、ロングタップ解除する
		this->unschedule(schedule_selector(DungeonScene::longTapCounter));
    }
	return;
}

void DungeonScene::attackButtonClicked(){//攻撃ボタンが押されたとき
    if(replay_flg==true) return;
    
    HMSLOG("アタックボタンのクリック");
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/shake_clock.mp3");
    
    for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())) {
        chara->hideYellowBlink();
    }
    
    //攻撃ボタンを無効にする
    uiLayer->changeAttackButton("attack_cant");
    
    if(isTutorial() && !isTutorialNaviActive() && TurnManage::getTurnCount() == 1) {//共通条件
        if(TutorialNaviLayer::checkCommentComplete(TutorialNaviLayer::CommentIndex::COMMENT_5)==false) {//個別条件
            //ナビキャラの登場後バトル開始(開戦エフェクトは無し)
            tutorialNaviLayer->setup(this, TutorialNaviLayer::CommentIndex::COMMENT_5);
            SEL_SCHEDULE s = schedule_selector(DungeonScene::tutorialAttack);
            tutorialNaviLayer->setScheduler(this, s);
            
            //攻撃ボタンを次の軍師カードが出されるまで無効にする
            uiLayer->attack_button_compel_false = true;;
        }
    }else{
        attack(TurnManage::getPlayerIndex());
    }
}
void DungeonScene::tutorialAttack(float index){
    attack(TurnManage::getTurnIndex());
}

void DungeonScene::updateSendMessage(float dt){
    if(getBattleMode()=="vs") sendMessage();//対戦時、ストックしてあるメッセージがあれば送信
}

//スケジューラー0.3秒ごと
void DungeonScene::updateOne(float dt)
{
    if(replay_flg==true) return;
    if(getBattleMode()=="vs" && getVsState()!="ok") return;
    
    if(isBossEventBattle() && boss_event_limit_time > 0) {
        time_t now_time = getNowTime();
        boss_event_rest_time = boss_event_limit_time - now_time;
        HMSLOG("boss_event_rest_time=%d", boss_event_rest_time);
        if(boss_event_rest_time<=0) boss_event_rest_time = 0;
        
        uiLayer->updateBossEventRestTime((int)boss_event_rest_time);
        
        if(boss_event_rest_time<=0) gameoverJudge("escape");
    }
    
    if(getBattleMode()=="vs") {
        if(SaveData::loadBool("background_flg")==true) {
            popupRestartMessage();
            return;
        }
        
        if(isTournamentEventBattle()){
            string second = to_string(finish_rest_time%60);
            if(second.size()==1) second = "0"+second;//１桁しかない時は０を追加
            string time_min = to_string(int(floor(finish_rest_time/60.0f)))+":"+second;
            uiLayer->rest_time_label->setString(time_min);
            uiLayer->rest_time_label->setVisible(true);
            
            if(int(floor(finish_rest_time/60.0f)) == 0){//１分切ったら赤文字に
                uiLayer->rest_time_label->setColor(Color3B(255,60,32));
            }else{
                uiLayer->rest_time_label->setColor(Color3B::WHITE);
            }
        }
        
        int time_list[2];
        time_list[0] = last_time[0];
        time_list[1] = last_time[1];
        bool check_flg = true;//checkWebSocket();
        uiLayer->setTimer(time_list, check_flg);
        if(check_flg==true) {
            if(this->isMyTurn() && last_time[TurnManage::getPlayerIndex()]>=1 && last_time[TurnManage::getPlayerIndex()]<=2){
                HMSLOG("ここからコマンド不可:%d",last_time[TurnManage::getPlayerIndex()]);
                turn_change_wait=true;
                selectAbleEffect(false);
                if(tapped_chara!=nullptr) restoreChara(tapped_chara);
                uiEffectLayer->reset_place_frame_effect();//移動できる場所表示エフェクト非表示
            }else if(this->isMyTurn() && last_time[TurnManage::getPlayerIndex()]>=3 && last_time[TurnManage::getPlayerIndex()]<=15){
                turn_change_wait=false;
                
            }
            
            if(last_time[TurnManage::getTurnIndex()]>0) last_time[TurnManage::getTurnIndex()]--;
        }
        
        //残り時間カウントの表示
        if(last_time[TurnManage::getPlayerIndex()]>90 || getState() == "Directioning" || isMyTurn()==false){//その時間以外かアタックボタンを押した後か敵のターンは消す
            for(int i=0; i<4; i++) count_timer_flg[i] = false;
            nume->setVisible(false);
        }else if((last_time[TurnManage::getPlayerIndex()] == 22 || last_time[TurnManage::getPlayerIndex()] == 21|| last_time[TurnManage::getPlayerIndex()] == 20) && count_timer_flg[3] ==false){
            count_timer_flg[3] = true;
            nume->setVisible(true);
            nume->setScale(0.6f);
            nume->setValue(3, "");
            auto fadein = FadeIn::create(0.1f);
            auto scale1 = ScaleTo::create(0.1f, 0.85f);
            auto ease = EaseBounceOut::create(scale1);
            auto scale2 = ScaleTo::create(0.1f, 0.7f);
            auto scale =ScaleTo::create(1.0f, 0.85f);
            nume->runAction(Sequence::create(fadein,ease,scale2,scale, NULL));
            
        }else if((last_time[TurnManage::getPlayerIndex()] == 17 || last_time[TurnManage::getPlayerIndex()] == 16|| last_time[TurnManage::getPlayerIndex()] == 15) && count_timer_flg[2] ==false){
            count_timer_flg[2] = true;
            nume->setVisible(true);
            nume->setScale(0.6f);
            nume->setValue(2, "");
            auto fadein = FadeIn::create(0.1f);
            auto scale1 = ScaleTo::create(0.1f, 0.85f);
            auto ease = EaseBounceOut::create(scale1);
            auto scale2 = ScaleTo::create(0.1f, 0.7f);
            auto scale =ScaleTo::create(1.0f, 0.85f);
            nume->runAction(Sequence::create(fadein,ease,scale2,scale, NULL));

        }else if((last_time[TurnManage::getPlayerIndex()] == 12 || last_time[TurnManage::getPlayerIndex()] == 11|| last_time[TurnManage::getPlayerIndex()] == 10) && count_timer_flg[1] ==false){
            count_timer_flg[1] = true;
            nume->setVisible(true);
            nume->setScale(0.6f);
            nume->setValue(1, "");
            auto fadein = FadeIn::create(0.1f);
            auto scale1 = ScaleTo::create(0.1f, 0.85f);
            auto ease = EaseBounceOut::create(scale1);
            auto scale2 = ScaleTo::create(0.1f, 0.7f);
            auto scale =ScaleTo::create(1.0f, 0.85f);
            nume->runAction(Sequence::create(fadein,ease,scale2,scale, NULL));
            
        }else if((last_time[TurnManage::getPlayerIndex()] == 7 || last_time[TurnManage::getPlayerIndex()] == 6|| last_time[TurnManage::getPlayerIndex()] == 5) && count_timer_flg[0] ==false){
            count_timer_flg[0] = true;
            nume->setVisible(true);
            nume->setScale(0.6f);
            nume->setValue(0, "");
            auto fadein = FadeIn::create(0.1f);
            auto scale1 = ScaleTo::create(0.1f, 0.85f);
            auto ease = EaseBounceOut::create(scale1);
            auto scale2 = ScaleTo::create(0.1f, 0.7f);
            auto scale =ScaleTo::create(1.0f, 0.85f);
            
            auto delay = DelayTime::create(0.5f);
            auto fadeout = FadeOut::create(0.1f);
            nume->runAction(Sequence::create(fadein,ease,scale2,scale,delay,fadeout, NULL));
    
        }else if(last_time[TurnManage::getPlayerIndex()]>90){
            for(int i=0; i<4; i++) count_timer_flg[i] = false;
            nume->setVisible(false);
        }
        //HMSLOG("ラストタイム:%d",last_time[TurnManage::getPlayerIndex()]);
        
    }
	
	//使用できる士気コストで何も出来ない時、攻撃ボタンをアクションで動かす
	if(isMyTurn()) {
		bool no_cost_flg = true;//現在のコストで行動できるかどうか判断用
        for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())) {
			if(chara->isOnField()) {//フィールドにいるキャラ
				if(chara->getActiveSkillId()>0 && chara->getActiveSkillCost()<=player[TurnManage::getPlayerIndex()]->getNowPower()) {//スキルを発動できるコストがあるか
					no_cost_flg = false;
					break;
				}
			}
			if(chara->getHandIndex()>=0) {//手札のキャラ・フレンド
				if(chara->getPlaceCost()<=player[TurnManage::getPlayerIndex()]->getNowPower()) {//配置できるか
					no_cost_flg = false;
					break;
				}
			}
		}
		//リーダーのスキルを使用するコストがあるか
        if(CharaList::getLeader(TurnManage::getPlayerIndex())->getActiveSkillId()>0 && CharaList::getLeader(TurnManage::getPlayerIndex())->getActiveSkillCost()<=player[TurnManage::getPlayerIndex()]->getNowPower()) no_cost_flg = false;
		//フレンドの配置できるコストがあるか
		if(CharaList::getFriend(TurnManage::getPlayerIndex()) != nullptr) {
			if(CharaList::getFriend(TurnManage::getPlayerIndex())->getActiveSkillId()>0 && CharaList::getFriend(TurnManage::getPlayerIndex())->getActiveSkillCost()<=player[TurnManage::getPlayerIndex()]->getNowPower()) no_cost_flg = false;
		}
	}
	
    //対戦時相手確認
    if(getBattleMode()=="vs") {
        //最初にオンライン対戦を始めたときにくるメッセージを受け取った時にセットターンをするロジックを書く　それの後にアタックとかをいれて最後にチェンジ
        if(data_exec_flg==false && data_received.size()>0) {
            
            data_send_flg = false;//通信相手にデータを送るかどうか→送らない
            runBattleData(data_received.at(0).c_str());//バトルデータ実行
            
            data_received.remove(0);
            
        }
        //対戦時は敵の思考ルーチンには遷移してはならないため、ここでreturnする
        return;
    }
    
    
    //敵の思考ルーチン
    bool passFlg =false;
    
    if(!isMyTurn() && getState()=="Operationing" && passFlg==false && cpu_moving == false) {
        HMSLOG("敵の思考ルーチン開始");
        vector<Chara *> able_chara;//手札の中でコスト的に出せるキャラ
        vector<float> able_chara_priority;//それの優先度
        
        for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())){
            if(chara->isInHand() && chara->getPlaceCost() <= player[TurnManage::getEnemyIndex()]->getNowPower() && chara->getType() !=CHARA_TYPE_LEADER){
                //手札にあってコスト的に出せる  キャラをable_charaとする
                able_chara.push_back(chara);
                able_chara_priority.push_back(chara->place_priority);//配置優先度
            }
        }
        for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())){
            if(chara->getType() == CHARA_TYPE_LEADER && chara->skillAble() && chara->used_skill_flg == false){//リーダーでスキル使える キャラをable_charaとする    スキルは一回使ったらそのターン使用不可
                able_chara.push_back(chara);
                able_chara_priority.push_back(chara->skill_priority);//スキル優先度
            }
        }
        for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())){
            if(chara->isOnField() && chara->getType() !=CHARA_TYPE_LEADER && chara->skillAble() && chara->used_skill_flg == false){//フィールドにあってスキル使える キャラをable_charaとする    スキルは一回使ったらそのターン使用不可
                able_chara.push_back(chara);
                able_chara_priority.push_back(chara->skill_priority);//スキル優先度
            }
        }
        
        
        if(able_chara.size()==0){
            passFlg=true;
            HMSLOG("敵：コストが足りないのでパスします");
        } else {
            vector<int> able_field_index;//味方がいない置けるフィールドインデックス
            vector<int> able_support_index;//味方がいない置ける軍師フィールドインデックス
            
            /****優先度調整フェーズ****/
            for(int i=2; i<=7; i++){
                bool existence=false;
                
                for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())){
                    if(chara->field_index == i){//軍師フィールドに置いてある
                        existence = true;
                        break;
                    }
                }
                if(fieldLayer->placeAbleField(i, TurnManage::getEnemyIndex())==false){//封鎖されている
                    existence = true;
                }
                
                if(existence==false) able_field_index.push_back(i);//何もフィールドに置いていない
            }
            
            for(int i=0; i<=1; i++){
                bool existence=false;
                
                for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())){
                    if(chara->field_index == i){//軍師フィールドに置いてある
                        existence = true;
                        break;
                    }
                }
                if(fieldLayer->placeAbleField(i, TurnManage::getEnemyIndex())==false){//封鎖されている
                    existence = true;
                }
                
                if(existence==false) able_support_index.push_back(i);//何も軍師フィールドに置いていない
            }

            
            Chara *strong = nullptr;
            Chara *weak = nullptr;
            if(able_field_index.size() == 0){//武将フィールドが全て埋まっているときは
                HMSLOG("フィールドが全て埋まっている");
                for(int i=0; i<able_chara.size(); i++){//出せるカードで
                    if(able_chara.at(i)->isInHand() && able_chara.at(i)->isAttackAndDefense()){//手札にある武将で
                        if(strong == nullptr || strong->base_value < able_chara.at(i)->base_value){//戦闘力が最高の物
                            strong = able_chara.at(i);
                        }
                    }
                }
                
                for(Chara *chara : CharaList::getOneSide(TurnManage::getEnemyIndex())){
                    if(chara->field_index > 1){//フィールドにある武将で
                        if(weak == nullptr || weak->base_value > chara->base_value){//戦闘力が最低の物
                            bool cant_place_flg = false;
                            for(Chara *forward_chara : CharaList::getOneSide(TurnManage::getPlayerIndex())){
                                if(chara->field_index == 9-forward_chara->getFieldIndex()){//前方のキャラの時
                                    if(forward_chara->statusCheck(STATUS_TYPE_PLACE_FORBIDDEN_FORWARD)){//前方に前方配置不可の敵
                                        cant_place_flg = true;
                                    }
                                    break;
                                }
                            }
                            if(cant_place_flg == false) weak = chara;
                        }
                    }
                }
                
                //フィールドの弱いカードを手札の最強カードに置き換えられる時
                if(strong!=nullptr && weak!=nullptr && strong->base_value > weak->base_value){
                    HMSLOG("敵：カードの置き換えを");
                   
                    for(int i=0; i<able_chara.size(); i++){
                        if(able_chara.at(i)->isInHand() && able_chara.at(i)->isAttackAndDefense()){
                            if(able_chara.at(i)->getPrimaryId() == strong->getPrimaryId()){//出せる軍師もいない場合、強いカードだけ２倍
                               
                                able_chara_priority.at(i) = able_chara_priority.at(i)*2;
                            }else{
                                able_chara_priority.at(i) = 0;//出せる軍師がいたり、その他はゼロ
                            }
                        }
                    }
                }else{//置き換えない時
                    HMSLOG("敵：やっぱやめ");
                    for(int i=0; i<able_chara.size(); i++){
                        if(able_chara.at(i)->isInHand() && able_chara.at(i)->isAttackAndDefense()){
                            able_chara_priority.at(i) = 0;//手札の武将全てゼロ
                        }
                    }
                }
            }
            
            
            if(able_support_index.size() == 0){//軍師フィールドが全て埋まっているときは
                for(int i=0; i<able_chara.size(); i++){
                    if(able_chara.at(i)->isInHand() && (able_chara.at(i)->getType()==CHARA_TYPE_SUPPORT)){//手札にある軍師カードは出さない
                        able_chara_priority.at(i) = 0.0f;
                    }
                }
            }
            
            while (true) {
                
                /****抽選フェーズ****/
                float all_priority = 0.0f;
                for(int i=0; i<able_chara_priority.size(); i++){
                    all_priority += able_chara_priority.at(i);
                    HMSLOG("抽選：：：id:%d,  優先度:%f",able_chara.at(i)->getPrimaryId(),able_chara_priority.at(i));
                }
                
                
                int lucky_index;//当たりのindex
                
                if(all_priority >20){//合計優先度が20より大きい時
                    float rand_pri = random(0, (int)floor(all_priority));//合計値中でランダム
                    HMSLOG("合計優先度：%f , 抽選数値：%f",all_priority,rand_pri);
                    for(int i=0; i<able_chara_priority.size(); i++){
                        float sum = 0.0f;
                        for(int j=0; j<=i; j++){
                            sum += able_chara_priority.at(j);
                        }
                        HMSLOG("SUM:%f , id:%d",sum,able_chara.at(i)->getPrimaryId());
                        if(rand_pri <= sum && able_chara_priority.at(i)>0){
                            lucky_index = i;
                            break;
                        }
                    }
                }else{
                    HMSLOG("合計優先度が20に満たない");
                    float rand_20 = random(0, 20);//２０まででランダム
                    HMSLOG("合計優先度：%f , 抽選数値：%f",all_priority,rand_20);
                    if(rand_20 > all_priority){//
                        for(int i=0; i<able_chara_priority.size(); i++){
                            //able_chara_priority.at(i) = 0;
                            all_priority = 0;//何もしない
                        }
                    }else{
                        for(int i=0; i<able_chara_priority.size(); i++){
                            float sum = 0.0f;
                            for(int j=0; j<=i; j++){
                                sum += able_chara_priority.at(j);
                            }
                            HMSLOG("SUM:%f , id:%d",sum,able_chara.at(i)->getPrimaryId());
                            if(rand_20 <= sum && able_chara_priority.at(i)>0){
                                lucky_index = i;
                                break;
                            }
                        }
                    }
                }
                
                /****実行フェーズ****/
                HMSLOG("実行フェーズ");
                if(all_priority >0){
                    Chara *lucky_chara = able_chara.at(lucky_index);
                    HMSLOG("test%d",lucky_index);
                    if(able_chara.at(lucky_index)->getType() == CHARA_TYPE_LEADER || able_chara.at(lucky_index)->isOnField()){//アクティブスキル使用
                        HMSLOG("敵：アクティブスキルを使用します");
                        
                        bool skill_use_able;
                        
                        vector<Chara *> target_charas;
                        HmsVector<string> target_types = split(lucky_chara->getActiveSkillTarget(), "#");
                        if(target_types.size() >= 4 && target_types.at(3) == "select"){
                            target_charas = activeSkill->getTargetCharas(lucky_chara, CharaList::getAll().getVector(), target_types.at(0));//対象のキャラ
                            if(target_charas.size()==0){//対象がいない場合
                                skill_use_able = useSkill(lucky_chara->getPrimaryId(),"");//エラー表示
                            }else{//対象がいる時ランダムの相手に発動
                                int random_index = random(0, (int)target_charas.size()-1);
                                skill_use_able = useSkill(lucky_chara->getPrimaryId(),to_string(target_charas.at(random_index)->getPrimaryId()) + "&1");
                            }
                            
                        }else{
                            skill_use_able = useSkill(lucky_chara->getPrimaryId(),"");
                        }
                        
                        
                        
                        
                        if(skill_use_able == true){
                            cpu_moving = true;
                            able_chara.at(lucky_index)->used_skill_flg = true;//一回のみ
                        }else{
                            able_chara_priority.at(lucky_index) = 0.0f;//使えないアクティブスキルは除外し、再抽選
                            HMSLOG("敵：このアクティブ使えないからやめた。");
                            continue;
                        }
                        
                    }else if(lucky_chara->isInHand() && lucky_chara->isAttackAndDefense()){//手札から武将召喚
                        if(able_field_index.size() == 0 && strong->getPrimaryId() == lucky_chara->getPrimaryId()){//置き換えの場合
                            
                            cpu_moving = true;
                            
                            HMSLOG("敵：カードの置き換えをします.ストロング:%d ,ウィーク:%d,ラッキー:%d,リプレイスindex:%d",strong->getCharaId(), weak->getCharaId(),lucky_chara->getCharaId(), replace_field_index);
                            this->runAction(Sequence::create(DelayTime::create(0.35f),
                                                             CallFunc::create([this, lucky_chara, weak](){
                                //this->replace_field_index = weak->getFieldIndex();
                                this->replaceChara(lucky_chara, weak);
                                HMSLOG("ラッキー:%d ",lucky_chara->getCharaId());}),nullptr));
                            
                        }else{//普通に配置する場合
                            
                            HMSLOG("敵：武将カードを配置します プライマリ:%d バリュー:%d タイプ:%d　配置:%s",lucky_chara->getPrimaryId(),lucky_chara->getValue(),lucky_chara->getType(),lucky_chara->place_type.c_str());
                            int setting_field;
                            string place_type_temp = lucky_chara->place_type;
                            while(true){
                                
                                if(place_type_temp == "random"){
                                    HMSLOG("ランダムな配置にします");
                                    random_shuffle(able_field_index.begin(), able_field_index.end());
                                    setting_field = able_field_index.at(0);
                                    
                                }else if(place_type_temp == "combo"){
                                    random_shuffle(able_field_index.begin(), able_field_index.end());
                                    HMSLOG("コンボを考えた配置にします");
                                    int bigger_combo = 0;//最大コンボ
                                    int bigger_combo_index = 0;//最大コンボの時のable_field_indexの場所
                                    for(int i=0; i<able_field_index.size(); i++){
                                        int temp = 0;
                                        if(able_field_index.at(i) >= 3 && fieldLayer->getCharaFieldIndex(able_field_index.at(i)-1, TurnManage::getEnemyIndex())!=nullptr && fieldLayer->getCharaFieldIndex(able_field_index.at(i)-1, TurnManage::getEnemyIndex())->getCombinationR() == lucky_chara->getCombinationL() && lucky_chara->getCombinationL() > 0){
                                            //インデックス３以上で、そこ-1のインデックスにキャラがいて、そのキャラとコンボが一致する時
                                            temp++;
                                        }
                                        if(able_field_index.at(i) <= 6 && fieldLayer->getCharaFieldIndex(able_field_index.at(i)+1, TurnManage::getEnemyIndex())!=nullptr && fieldLayer->getCharaFieldIndex(able_field_index.at(i)+1, TurnManage::getEnemyIndex())->getCombinationL() == lucky_chara->getCombinationR() && lucky_chara->getCombinationR() > 0){
                                            //インデックス６以下で、そこ+1のインデックスにキャラがいて、そのキャラとコンボが一致する時
                                            temp++;
                                        }
                                        if(bigger_combo <= temp){
                                            bigger_combo = temp;
                                            bigger_combo_index = i;
                                        }
                                    }
                                    if(bigger_combo == 0){//どこもコンボできねぇ時は隣にだれもいない空白地点になるべく置く
                                        HMSLOG("どこもコンボできねぇってよ");
                                        int blank_sum = 0;//最大空白地点数
                                        for(int i=0; i<able_field_index.size(); i++){
                                            int temp = 0;
                                            if(able_field_index.at(i) >= 3 && fieldLayer->getCharaFieldIndex(able_field_index.at(i)-1, TurnManage::getEnemyIndex())==nullptr){//インデックス３以上で、そこー１のインデックスにキャラがいない時
                                                temp = temp+2;;
                                            }
                                            if(able_field_index.at(i) <= 6 && fieldLayer->getCharaFieldIndex(able_field_index.at(i)+1, TurnManage::getEnemyIndex())==nullptr){//インデックス6以下で、そこ+１のインデックスにキャラがいない時
                                                temp = temp+2;;
                                            }
                                            if(able_field_index.at(i) == 2 || able_field_index.at(i) == 7){
                                                temp++;
                                            }
                                            if(blank_sum <= temp){
                                                blank_sum = temp;
                                                bigger_combo_index = i;
                                            }
                                            
                                        }
                                    }
                                    
                                    setting_field = able_field_index.at(bigger_combo_index);
                                    
                                }else if(place_type_temp == "forward"){//敵の前に出す
                                    HMSLOG("敵キャラの前に配置にします");
                                    int temp = 0;
                                    bool forward_flg = false;
                                    random_shuffle(able_field_index.begin(), able_field_index.end());
                                    for(int i=0; i<able_field_index.size(); i++){
                                        for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())){
                                            if(9 - able_field_index.at(i) == chara->getFieldIndex() && chara->getFieldIndex()>=2){//反対側にキャラがいるなら
                                                temp = i;
                                                forward_flg =true;
                                            }
                                        }
                                    }
                                    if(forward_flg == false){//敵の前に置けない場合だったら
                                        place_type_temp = "combo";
                                        continue;
                                    }
                                    setting_field = able_field_index.at(temp);
                                    
                                }else if(place_type_temp == "forward_atk"){//敵の攻撃武将の前に出す
                                    HMSLOG("敵攻撃武将の前に配置にします");
                                    int temp = 0;
                                    bool forward_flg = false;
                                    random_shuffle(able_field_index.begin(), able_field_index.end());
                                    for(int i=0; i<able_field_index.size(); i++){
                                        for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())){
                                            if(9 - able_field_index.at(i) == chara->getFieldIndex() && chara->getFieldIndex()>=2 && chara->isAttacker()){//反対側にキャラがいるなら
                                                temp = i;
                                                forward_flg =true;
                                            }
                                        }
                                    }
                                    if(forward_flg == false){//敵の前に置けない場合だったら
                                        place_type_temp = "combo";
                                        continue;
                                    }
                                    setting_field = able_field_index.at(temp);
                                    
                                }else if(place_type_temp == "forward_def"){//敵の防御武将の前に出す
                                    HMSLOG("敵防御武将の前に配置にします");
                                    int temp = 0;
                                    bool forward_flg = false;
                                    random_shuffle(able_field_index.begin(), able_field_index.end());
                                    for(int i=0; i<able_field_index.size(); i++){
                                        for(Chara *chara : CharaList::getOneSide(TurnManage::getPlayerIndex())){
                                            if(9 - able_field_index.at(i) == chara->getFieldIndex() && chara->getFieldIndex()>=2 && chara->isDefender()){//反対側にキャラがいるなら
                                                temp = i;
                                                forward_flg =true;
                                            }
                                        }
                                    }
                                    if(forward_flg == false){//敵の前に置けない場合だったら
                                        place_type_temp = "combo";
                                        continue;
                                    }
                                    setting_field = able_field_index.at(temp);
                                    
                                }else{
                                    random_shuffle(able_field_index.begin(), able_field_index.end());
                                    setting_field = able_field_index.at(0);
                                }
                                
                                break;
                            }
                            cpu_moving = true;
                            this->runAction(Sequence::create(DelayTime::create(0.35f),
                                                             CallFunc::create([this,lucky_chara,setting_field](){this->placeCharacter(lucky_chara, setting_field);}),nullptr));
                        }
                        
                    }else if(lucky_chara->isInHand() && lucky_chara->getType() == CHARA_TYPE_SUPPORT){//手札から軍師召喚
                        HMSLOG("敵：軍師カードを配置します");
                        random_shuffle(able_support_index.begin(), able_support_index.end());
                        
                        int temp_field = able_support_index.at(0);
                        cpu_moving = true;
                        this->runAction(Sequence::create(DelayTime::create(0.35f),
                                                         CallFunc::create([this,lucky_chara,temp_field](){this->placeCharacter(lucky_chara, temp_field);}),nullptr)
                                        );
                        
                    }else{
                        HMSLOG("おかしいですよっと");
                    }
                }else{
                    passFlg=true;
                }
                
                break;
            }
        }
    }
    if(!isMyTurn() && getState()=="Operationing" && passFlg==true) {
        //状態を変更
        HMSLOG("敵のターンのattack");
        cpu_moving = true;
        
        this->runAction(Sequence::create(DelayTime::create(0.40f),
                                         CallFunc::create([this](){this->attack(TurnManage::getEnemyIndex());}),nullptr)
        );
    }
}

void DungeonScene::runBattleData(string battle_data){//バトルデータの実行
    
    HMSLOG("実行バトルデータ=%s", battle_data.c_str());
    string err;
    Json json = Json::parse(battle_data.c_str(), err);
    
    int rand_seed = json["random"].int_value();
    
    CCLOG("全体データの更新");
    setBattleData(json["battle_data"]);
    
    CCLOG("更新後データ%s",getBattleData().dump().c_str());
    checkIllegal("OnlineDataReceive", false);
    
    resetAllCharaValue();	//戦闘力リセット
    reCalcCombination();	//コンボ再計算
    calcCombinationValue();	//コンボによる攻撃力上昇
    calcPassiveBuffer();	//パッシブスキル効果の計算
    calcActiveBuffer();		//ステータスバッファ（アクティブスキルによる攻撃力上昇等）効果の計算
    
    
    updateFieldLook();
    
    if(json["type"].string_value()=="place_character") {
        set_chara = CharaList::getAtPrimary(json["primary_id"].int_value());
        
        int field_index = fromString<int>(json["extra_data"].string_value());
        
        if(fieldLayer->getCharaFieldIndex(field_index, json["player_no"].int_value()-1) != nullptr){//そのフィールドインデックスにキャラがある場合
            replaceChara(set_chara, fieldLayer->getCharaFieldIndex(field_index, json["player_no"].int_value()-1), rand_seed);//置き換え
        }else{
            placeCharacter(set_chara, field_index, rand_seed);
        }
        
    } else if(json["type"].string_value()=="attack") {
        HMSLOG("VS用　敵のターンの攻撃");
        
        //TurnManage::setTurnIndex(json["player_no"].int_value()-1);//敵の攻撃の時はターンを自分にまわす？
       
        if(json["extra_data"].string_value() == "waiting"){//攻撃しない時
            attack(json["player_no"].int_value()-1,true, rand_seed);
        }else{
            attack(json["player_no"].int_value()-1,false, rand_seed);
        }
        
    } else if(json["type"].string_value()=="use_skill") {
        useSkill(json["primary_id"].int_value(), json["extra_data"].string_value(), rand_seed);
        
    } else if(json["type"].string_value()=="initialize") {//カード配られる前状態
        scheduleOnce(schedule_selector(DungeonScene::changeTurn), 2.0f);
    }
}


void DungeonScene::updateFieldLook(){
    //プレイヤーのHP表示を更新する
    for(int ii=0; ii<2; ii++) uiLayer->updatePlayerInfo(player[ii]->getHp(), player[ii]->getMaxHp(), ii);
    
    //パワー表示も更新
    for(int ii=0; ii<2; ii++) {
        uiLayer->updatePower(player[ii]->getNowPower(), ii);
    }
    
    
    for(Chara *chara : CharaList::getAll()){
        chara->comboSpriteUpdate();//コンボを正しく
        chara->setTypeSprite();//タイプ画像を正しく
    }
    
    //フィールド上のキャラを正しい位置に設置する。
    for(Chara *chara : CharaList::getAll()){
        
        if(chara->getType() == CHARA_TYPE_LEADER) continue;//総大将は初期配置から絶対に動かさないため、除外する
        
        if(chara->isOnField()) {//フィールドに移動
            Point target_position = fieldLayer->field_position_list[chara->getBelongingIndex()].at(chara->getFieldIndex());
            chara->setPosition(target_position);
            chara->showCardParam();
            chara->showValue();
        }else if(chara->isInHand()) {//手札に移動
            Point target_position = fieldLayer->hand_position_list[chara->getBelongingIndex()].at(chara->getHandIndex());
            chara->setPosition(target_position);
            chara->showCardParam();
        }else if(chara->isFriend()) {//フレンド
            Point target_position = fieldLayer->friend_position_list[chara->getBelongingIndex()];
            chara->setPosition(target_position);
            chara->showCardParam();
        }else if(!chara->isInHand() && !chara->isOnField()) {//手札にもフィールドにもない場合はデッキに戻す
            Point target_position = fieldLayer->deck_point[chara->getBelongingIndex()];
            chara->setPosition(target_position);
            chara->showCardParam();
        }
    }
    for(Chara *chara : CharaList::getAll()){
        if(chara->isOnField()){
            chara->showValue();
        }
    }
    uiLayer->resetShowAllIcon();
}

bool DungeonScene::wholeStatusCheck(int state, int index){
    for(int i=0; i<player[index]->whole_status_active.size(); i++){
        if(player[index]->whole_status_active.at(i)["state"] == state){
            return true;
        }
    }
    for(int i=0; i<player[index]->whole_status_passive.size(); i++){
        if(player[index]->whole_status_passive.at(i)["state"] == state){
            return true;
        }
    }
    return false;
}

int DungeonScene::wholeStatusValue(int state, int index, string return_type){
    
    vector<int> values;
    
    for(int i=0; i<player[index]->whole_status_active.size(); i++){
        if(player[index]->whole_status_active.at(i)["state"] == state){
            values.push_back(player[index]->whole_status_active.at(i)["value"]);
        }
    }
    
    for(int i=0; i<player[index]->whole_status_passive.size(); i++){
        if(player[index]->whole_status_passive.at(i)["state"] == state){
            values.push_back(player[index]->whole_status_passive.at(i)["value"]);
        }
    }
    
    if(values.empty()) return 0;
    if(return_type == "max") return *max_element(values.begin(), values.end());
    if(return_type == "min") return *min_element(values.begin(), values.end());
    if(return_type == "sum") return std::accumulate(values.begin(), values.end(), 0);
    
    return 0;
}

int DungeonScene::wholeStatusValueMultiply(int state, int index){//回復妨害スキルカット量
    float cut_par = 0.0f;
    for(int i=0; i<player[index]->whole_status_active.size(); i++){
        if(player[index]->whole_status_active.at(i)["state"] == state){
            
            cut_par = 100.0f-(100.0f-cut_par)*(100-player[index]->whole_status_active.at(i)["value"])/100.0f;
        }
    }
    for(int i=0; i<player[index]->whole_status_passive.size(); i++){
        if(player[index]->whole_status_passive.at(i)["state"] == state){
            cut_par = 100.0f-(100.0f-cut_par)*(100-player[index]->whole_status_passive.at(i)["value"])/100.0f;
        }
    }
    return cut_par;
}

void DungeonScene::createStatusCheckSum(){
    //お互いのHP、士気、ターンカウント、all_chara_listのすべてのvalueを足した値をチェックサムとします
    int sum = 0;
    sum += TurnManage::getTurnCount();
    for(int ii=0; ii<2; ii++){
        sum += player[ii]->getHp();
        sum += player[ii]->getNowPower();
    }
    
    //int value_sum = 0;
    for(Chara *chara : CharaList::getAll()){
        if(chara->getValue()!=chara->getBaseValue()){
            sum += chara->getValue();
        }
    }
    
    //sum = sum * value_sum;
    
    status_check_sum = sum;
    //status_check_sum = 0;
}

