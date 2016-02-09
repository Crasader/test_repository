
#include "NormalScene.h"
#include "TurnManage.h"
#include "PlistAnimationSprite.h"
#include "HttpConnectHms.h"
#include "PopupOk.h"

ResultLayer::ResultLayer()
:_check_count(0)
,_follow_flg(false)
,_follow_target_friend_id(0)
,_showed_booty_popup_flg(false)
{
}

ResultLayer* ResultLayer::create(bool winner_flg, string from_page_name)
{
    ResultLayer *node = new ResultLayer();
    if (node && node->init(winner_flg, from_page_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool ResultLayer::init(bool winner_flg, string from_page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //メンバ変数に登録
    _winner_flg = winner_flg;
    
    //シーンきりかえフラグ初期化
    _change_scened_flg = false;
    _from_page_name = from_page_name;
    
    return true;
}

void ResultLayer::onEnter()
{
    Layer::onEnter();
    
    
    //チュートリアルステータスの変更
    if(((DungeonScene *)this->getParent())->isTutorial()){
        SaveData::saveInt("tutorialStatus", 2);
    }
    
    //リザルトデータ通信処理
    getPointDataRequest();
}

void ResultLayer::getPointDataRequest()
{
    string user_master_id                = SaveData::loadString("user_master_id");
    string enemy_user_master_id          = SaveData::loadString("enemy_user_master_id");
    string npc_battle_event_rank_list_id = SaveData::loadString("npc_battle_event_rank_list_id");
    string choice_friend_id              = SaveData::loadString("choice_friend_id");
    int battle_list_id                   = SaveData::loadInt("battle_list_id");
    string quest_stage_id                = SaveData::loadString("quest_stage_id");
    string result_log_id                 = SaveData::loadString("result_log_id");
	int online_battle_surrender_flg      = SaveData::loadInt("OnlineBattleSurrenderFlg");
    string tutorial_status               = toString<int>(SaveData::loadInt("tutorialStatus"));
    
    auto dungeon_scene = (DungeonScene *)this->getParent();
    
    string param_turn = "";
    string param_nodamage_flg = "";
    string param_noskill_flg = "";
    string param_resthp = "";
    string achi_params = "";
    string check_sum = "";
    
    if (_from_page_name == "DungeonScene")
    {
        param_turn = toString<int>(TurnManage::getTurnCount());
        param_nodamage_flg = toString<int>(dungeon_scene->param_nodamage_flg);
        param_noskill_flg = toString<int>(dungeon_scene->param_noskill_flg);
        param_resthp = toString<int>(dungeon_scene->player[0]->getHp());
        achi_params = "turn=" + param_turn + "&nodamage_flg=" + param_nodamage_flg + "&noskill_flg=" + param_noskill_flg + "&resthp=" + param_resthp  ;
        check_sum = HmsCrypto::md5encode(param_turn+"_"+param_nodamage_flg+"_"+param_noskill_flg+"_"+param_resthp) ; //[turn]_[nodamage_flg]_[noskill_flg]_[param_resthp]の順
    }

    //リザルトデータリクエスト
    string url = HOST;
    url += "battle/battle_result.php";
    string body = "user_master_id="+user_master_id;
    body += "&enemy_user_master_id="+enemy_user_master_id;
    body += "&winner_flg="+toString<int>(_winner_flg);
    body += "&npc_battle_event_rank_list_id="+npc_battle_event_rank_list_id;
    body += "&choice_friend_id="+choice_friend_id;
    body += "&battle_list_id="+toString<int>(battle_list_id);
    body += "&quest_stage_id="+quest_stage_id;
    body += "&result_log_id="+result_log_id;
    body += "&surrender_flg="+toString<int>(online_battle_surrender_flg);
    body += "&tutorial_status="+tutorial_status;
    body += "&" + achi_params;
    body += "&chk_sum=" + check_sum ;
    HttpConnectHms::sendHttpRequest(url,[this](HttpClient *client, HttpResponse *response){
        getPointDataCallback(client, response);
    },body);
}

void ResultLayer::getPointDataCallback(HttpClient *client, HttpResponse *response)
{
    if(response->isSucceed()==false){//通信失敗したら
        CCLOG("通信失敗");
        runAction(Sequence::create(DelayTime::create(1.5f),CallFunc::create([this](){//少し時間をおき
            
            auto popup = PopupOk::create();
            popup->setPopup("タイムアウトしました。再送信します。",[this](){//ポップアップ出して再送信
                getPointDataRequest();//再送信
            });
            addChild(popup,1000000);
            
        }), NULL));
        
        return;
    }
    
    //リザルトデータレスポンス
    string result = HttpConnectHms::getHttpResponse(this, client, response);
                           
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("%s",result.c_str());
    
    string err;
    json = Json::parse(result, err);
    
	Json reward_item_list = json["item_list_data"].object_items();
    _item_count = countJsonData(reward_item_list);
    
    //再開用データをクリア
    SaveData::saveString("battle_data_for_restart", "");
    
    //リベンジモード突入かどうか
    _revenge_mode = json["revenge_mode"].bool_value();
    if(_revenge_mode==true) {
        SaveData::saveBool("revenge_mode", true);
    } else {
        SaveData::saveBool("revenge_mode", false);
    }
    
    //リベンジ成功したかどうか
    _revenge_success_flg = json["revenge_success_flg"].bool_value();
    
    //ボスイベント新規発見かどうか
    _boss_event_flg = json["boss_event_flg"].bool_value();
    
    //フォロー対象の情報を格納
    if(json["follow_target_data"]["follow_flg"].string_value() == "true") _follow_flg = true ;
    _follow_target_friend_id = json["follow_target_data"]["friend_id"].int_value() ;
    _follow_target_rank_name = json["follow_target_data"]["rank_name"].string_value();
    _follow_target_level = json["follow_target_data"]["level"].string_value();
	_follow_target_user_name = json["follow_target_data"]["user_name"].string_value();
    _follow_target_comment = json["follow_target_data"]["comment"].string_value();
    _follow_target_card_image_id = json["follow_target_data"]["card_image_id"].string_value();
	
	//条件達成を一定数集めたときの報酬を取得しているか保存 (マイページでアニメを再生するときに利用予定)
	SaveData::saveBool("achievement_reward_flg", json["achievement_reward_flg"].bool_value());
	
	//集めた星の数を保存
	int sum_reward_flg = json["sum_reward_flg"].int_value() ;
	if(sum_reward_flg != 0)SaveData::saveInt("sum_reward_flg", sum_reward_flg);
    
    
    
    //リザルト結果をユーザーデータに更新
    SaveData::saveString("level", json["after_user_data_list"]["level"].string_value());
    SaveData::saveString("rank_point", json["after_user_data_list"]["rank_point"].string_value());
    SaveData::saveString("total_rank_point", json["after_user_data_list"]["total_rank_point"].string_value());
    SaveData::saveString("need_rank_point", json["after_user_data_list"]["need_rank_point"].string_value());
    SaveData::saveString("reward_point", json["after_user_data_list"]["reward_point"].string_value());
    SaveData::saveString("total_reward_point", json["after_user_data_list"]["total_reward_point"].string_value());
    SaveData::saveString("need_reward_point", json["after_user_data_list"]["need_reward_point"].string_value());
    SaveData::saveString("rank_name", json["after_user_data_list"]["rank_name"].string_value());
    SaveData::saveString("gold", json["after_user_data_list"]["gold"].string_value());
    SaveData::saveString("power", json["after_user_data_list"]["power"].string_value());
    SaveData::saveString("max_power", json["after_user_data_list"]["max_power"].string_value());
    SaveData::saveString("power_use_time", json["after_user_data_list"]["power_use_time"].string_value());
    SaveData::saveString("timestamp_power_use_time", json["after_user_data_list"]["timestamp_power_use_time"].string_value());
    SaveData::saveString("total_rank", json["after_user_data_list"]["total_rank"].string_value());
    
    
    //プレゼントカウントの更新
    int total_present_item_count = json["present_count_data"].int_value();
    SaveData::saveInt("present_count", SaveData::loadInt("present_count") + total_present_item_count);
    
    
    if(_from_page_name == "DungeonScene"){
        
        //ポップアップ画像の作成（透明）
        createPopup();
    	
    	//ポップアップ表示順序(表示されるポップアップを順番に入れてる配列)を作成
        _now_popup_queue_index = 0 ;
        
        int npc_battle_event_rank_list_id = fromString<int>(SaveData::loadString("npc_battle_event_rank_list_id"));
        int quest_stage_id                = fromString<int>(SaveData::loadString("quest_stage_id"));
        
        _popup_types_queue.insert(PopupType::POPUP_POINT) ;
        
        if (_from_page_name != "DungeonScene") return;
        
        if (_item_count > 0) {
            _popup_types_queue.insert(PopupType::POPUP_BOOTY) ;
        }
        
        if(quest_stage_id > 0 || npc_battle_event_rank_list_id > 0) {
            _popup_types_queue.insert(PopupType::POPUP_ACHIEVEMENT) ;
        }
        
        if(!_follow_flg && _follow_target_friend_id != 0) {
            _popup_types_queue.insert(PopupType::POPUP_FRIENT) ;
        }
    }
    
    
    //報酬ポイントがランクアップしたか調べる
    int before_reward_point_list_id = fromString<int>(json["before_user_data_list"]["reward_point_list_id"].string_value());
    int after_reward_point_list_id  = fromString<int>(json["after_user_data_list"]["reward_point_list_id"].string_value());
    
    if (before_reward_point_list_id < after_reward_point_list_id) {
        //体力全回復
        SaveData::saveString("power", json["after_user_data_list"]["power"].string_value());
        string max_power = json["after_user_data_list"]["max_power"].string_value() ;
        SaveData::saveString("max_power", max_power);
        SaveData::saveString("power_use_time", json["after_user_data_list"]["power_use_time"].string_value());
        SaveData::saveString("timestamp_power_use_time", json["after_user_data_list"]["timestamp_power_use_time"].string_value());
    }
    
    
    //初心者の手引きをクリアしているか調べる
    checkClearBiginnerGuide() ;
    
    
    if(_from_page_name == "DungeonScene"){
        //結果文字画像の表示
        scheduleOnce(schedule_selector(ResultLayer::showResultLabel), 1.0f);
    }else if(_from_page_name == "TitleScene"){
        //Normalシーンへ遷移
        changeScene() ;
    }
    
}


void ResultLayer::createPopup()
{
    //ポップアップ画像の作成
    _titleSprite = Sprite::create("init/Dungeon/result/title_result.png");
    _titleSprite->setScale(ratio);
    _titleSprite->setOpacity(0);
    _titleSprite->setPosition(convertFromDisplaySize(Point(320, 820), 0.5, 0.5));
    addChild(_titleSprite, 2);

    if(SaveData::loadString("battle_mode")=="vs"){
        //オンラインの場合のみ経験値とランクを表示する
        _pointPopup = PointPopupSprite::create(PointPopupSprite::PopupType::TYPE_ALL_POINT, json);
    }else{
        //経験値のみを表示する
        _pointPopup = PointPopupSprite::create(PointPopupSprite::PopupType::TYPE_NO_RANK_POINT, json);
    }
    _pointPopup->setScale(ratio);
    _pointPopup->setOpacity(0);
    _pointPopup->setPosition(convertFromDisplaySize(Point(320, 350), 0.5, 0.5));
    addChild(_pointPopup, 2);

    _bootyPopup = BootyPopupSprite::create("init/Dungeon/result/popup_booty2.png", json);
    _bootyPopup->setScale(ratio);
    _bootyPopup->setOpacity(180);
    _bootyPopup->setPosition(convertFromDisplaySize(Point(320 + _contentSize.width, 380), 0.5, 0.5));
    addChild(_bootyPopup, 2);
	
    //子ノードのにも適用させる処理
    _setCascadeOpacityEnabled(_pointPopup, true);
    _setCascadeOpacityEnabled(_bootyPopup, true);

    //フォロー対象のユーザーがすでにフレンドでなかった場合フレンド登録するスプライトを生成する
	//フォロー対象のユーザーはオンラインでは対戦相手、オフラインでは協力者になる。
    if(!_follow_flg && _follow_target_friend_id != 0){
        
        string friend_id = toString<int>(_follow_target_friend_id) ;
        string rank_name =_follow_target_rank_name;
        string level =_follow_target_level;
        string user_name =_follow_target_user_name;
        string comment =_follow_target_comment;
        string card_image_id =_follow_target_card_image_id ;
        
        _userDetailSprite = FriendUserDetailSprite::create(friend_id, rank_name, user_name, level, comment, card_image_id) ;
        _userDetailSprite->setScale(ratio) ;
        _userDetailSprite->setOpacity(180) ;
        _userDetailSprite->setPosition(convertFromDisplaySize(Point(320 + _contentSize.width, 380), 0.5, 0.5));
        addChild(_userDetailSprite, 2) ;
        _setCascadeOpacityEnabled(_userDetailSprite, true);
    }
}

void ResultLayer::checkClearBiginnerGuide()
{
	if(json["beginner_guide_data_list"]["beginner_guide_clear_flg_4"].bool_value() && SaveData::loadBool("beginner_guide_clear_flg_4") == false){
        //初心の手引きクリアーフラグ4 レベルを3まで上げる(battle_result.php側で判断)
        SaveData::saveBool("beginner_guide_clear_flg_4", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
	}
	
    if(json["beginner_guide_data_list"]["beginner_guide_clear_flg_5"].bool_value() && SaveData::loadBool("beginner_guide_clear_flg_5") == false){
        //初心の手引きクリアーフラグ5 レベルを8まで上げる(battle_result.php側で判断)
        SaveData::saveBool("beginner_guide_clear_flg_5", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
    }
    
    if(json["beginner_guide_data_list"]["beginner_guide_clear_flg_6"].bool_value() && SaveData::loadBool("beginner_guide_clear_flg_6") == false){
        //初心の手引きクリアーフラグ6レベルを13まで上げる(battle_result.php側で判断)
        SaveData::saveBool("beginner_guide_clear_flg_6", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
    }
    
    if(_winner_flg && fromString<int>(SaveData::loadString("event_rank_level")) == 1 && SaveData::loadBool("beginner_guide_clear_flg_7") == false){
        //初心の手引きクリアーフラグ7 イベント初級に勝利する
        SaveData::saveBool("beginner_guide_clear_flg_7", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
    }
    
    if(_winner_flg && fromString<int>(SaveData::loadString("event_rank_level")) == 2 && SaveData::loadBool("beginner_guide_clear_flg_8") == false){
        //初心の手引きクリアーフラグ8 イベント中級に勝利する
        SaveData::saveBool("beginner_guide_clear_flg_8", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
    }
    
    if(_winner_flg && fromString<int>(SaveData::loadString("event_rank_level")) == 3 && SaveData::loadBool("beginner_guide_clear_flg_9") == false){
        //初心の手引きクリアーフラグ9 イベント上級に勝利する
        SaveData::saveBool("beginner_guide_clear_flg_9", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
    }
	
	//オンライン対戦のクイック対戦に参戦している場合
    if(SaveData::loadInt("battle_list_id") > 0 && SaveData::loadString("online_battle_type") == "quick" && SaveData::loadBool("beginner_guide_clear_flg_10") == false){
        //初心の手引きクリアーフラグ10 オンライン対戦のクイック対戦に参加する
        SaveData::saveBool("beginner_guide_clear_flg_10", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
	}
    
    //オンライン対戦のルーム対戦に参戦している場合
    if(SaveData::loadInt("battle_list_id") > 0 && (SaveData::loadString("online_battle_type") == "create_room" || SaveData::loadString("online_battle_type") == "search_room") && SaveData::loadBool("beginner_guide_clear_flg_11") == false){
        //初心の手引きクリアーフラグ11 オンライン対戦のルーム対戦に参加する
        SaveData::saveBool("beginner_guide_clear_flg_11", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
    }
	
	//勝利していてオンラインである場合
	if(_winner_flg && SaveData::loadInt("battle_list_id") > 0 && SaveData::loadBool("beginner_guide_clear_flg_12") == false){
        //初心の手引きクリアーフラグ12 オンライン対戦で勝利する
        SaveData::saveBool("beginner_guide_clear_flg_12", true);
        SaveData::saveBool("beginner_layer_open_flg", true);
	}
}

void ResultLayer::showResultLabel(float dt)
{
    //影の表示
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(shadow, 1);

    
    string effect_url = "";
    float effect_scale = 0.0f;
    
    if (_winner_flg) {
        //勝利時
        auto dungeon_scene = (DungeonScene *)this->getParent();
        float player_hp     = dungeon_scene->player[TurnManage::getPlayerIndex()]->getHp();
        float player_max_hp = dungeon_scene->player[TurnManage::getPlayerIndex()]->getMaxHp();
        HMSLOG("player_hp = %f, player_max_hp = %f", player_hp, player_max_hp);
        
        if(_revenge_success_flg==true)  {
            HMSLOG("リベンジ成功");
            effect_url = "download/Effect/Sprite/ef_revenge4.plist";
            effect_scale = 2.0f;
            
        }else if (player_hp / player_max_hp < 0.2f) {
            HMSLOG("辛勝");
            effect_url = "init/Dungeon/result/plist/win3.plist";
            effect_scale = 1.0f;
        }
        else if (player_hp / player_max_hp > 0.8f) {
            HMSLOG("完全勝利");
            effect_url = "init/Dungeon/result/plist/win.plist";
            effect_scale = 1.0f;
        }
        else {
            HMSLOG("勝利");
            effect_url = "init/Dungeon/result/plist/win2.plist";
            effect_scale = 1.0f;
        }

        SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/08_win.mp3",true);
        
    }else{
        //敗北時
        effect_url = "init/Dungeon/result/plist/lose.plist";
        effect_scale = 1.0f;
        SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/09_lose.mp3",true);
    }
    
    //エフェクトの表示
    /*auto sprite2 = PlistAnimationSprite::setData(this, effect_url, 1, 0.08f, 1000);
    sprite2->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    sprite2->setScale(effect_scale * ratio);
    */
    
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(effect_url);
    
    // 配列に各フレームを格納
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        animFrames.pushBack(frame);
    }
    
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.08f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    _resultLabelSprite = Sprite::createWithSpriteFrame(animFrames.front());
    _resultLabelSprite->setScale(effect_scale * ratio);
    _resultLabelSprite->setPosition(convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
    this->addChild(_resultLabelSprite, 1000);
    
    
    _resultLabelSprite->runAction(Sequence::create(animate, DelayTime::create(0.4f), MoveTo::create(0.4f, convertFromDisplaySize(Point(320, 520), 0.5, 0.5)),CallFunc::create([this](){
        //獲得ポイントポップアップアクション
        
        if(_titleSprite != nullptr){
            _titleSprite->runAction(FadeIn::create(0.5f));
        }
        
        _pointPopup->runAction(FadeIn::create(0.5f));
        
        //ポップアップ表示後にタッチ可能にする
        createEventListener();
        
        //ポイントポップアップアクション開始
        _pointPopup->scheduleOnce(schedule_selector(PointPopupSprite::gaugeAction), 1.0f);
        
    }), nullptr));
    
}

Menu *ResultLayer::createFriendButtonMenu(){
    
    //メニューの変数を作成
    Menu *menu  ;

    //はいボタンの作成
    auto yes_off = Sprite::create("init/Dungeon/result/bt1.png");
    auto yes_on  = Sprite::create("init/Dungeon/result/bt1.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto menuItem1 = MenuItemSprite::create(yes_off, yes_on, [this, menu](Ref* sender)
                                            {
                                                //SE
                                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");

                                                //友達申請リクエスト送信
                                                demandFollowRequest();
                                            });
    menuItem1->setScale(ratio) ;
    
    
    //ラベル生成
    auto yes_label1 = HMSFontUtil::createWithTTFBySystemFontArgument("フォローする", FONT_NAME_SYSTEM, 24);
    yes_label1->setHorizontalAlignment(TextHAlignment::LEFT);
    yes_label1->setPosition(yes_off->getContentSize()/2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    yes_label1->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    yes_off->addChild(yes_label1) ;
    
    //フォローする
    auto yes_label2 = HMSFontUtil::createWithTTFBySystemFontArgument("フォローする", FONT_NAME_SYSTEM, 24);
    yes_label2->setHorizontalAlignment(TextHAlignment::LEFT);
    yes_label2->setPosition(yes_on->getContentSize()/2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    yes_label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    yes_on->addChild(yes_label2) ;
    
    
    //いいえボタンの作成
    auto no_off = Sprite::create("init/Dungeon/result/bt1_blue.png");
    auto no_on  = Sprite::create("init/Dungeon/result/bt1_blue.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto menuItem2 = MenuItemSprite::create(no_off, no_on, [this](Ref* sender)
                                            {
                                                //SE
                                                CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_no.mp3");

                                                changeScene() ;
                                            });
    menuItem2->setScale(ratio) ;
    
    //ラベル生成
    auto no_label1 = HMSFontUtil::createWithTTFBySystemFontArgument("キャンセル", FONT_NAME_SYSTEM, 24);
    no_label1->setHorizontalAlignment(TextHAlignment::LEFT);
    no_label1->setPosition(no_off->getContentSize()/2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    no_label1->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    no_off->addChild(no_label1) ;
    
    //フォローする
    auto no_label2 = HMSFontUtil::createWithTTFBySystemFontArgument("キャンセル", FONT_NAME_SYSTEM, 24);
    no_label2->setHorizontalAlignment(TextHAlignment::LEFT);
    no_label2->setPosition(no_on->getContentSize()/2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    no_label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    no_on->addChild(no_label2) ;
    
    menu = Menu::create(menuItem1, menuItem2, nullptr);
    menu->alignItemsHorizontallyWithPadding(20*ratio);
    
    return menu ;
}

void ResultLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void ResultLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ResultLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ResultLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ResultLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ResultLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void ResultLayer::showRevengeEffect()
{
    //showEffect(this, "download/Effect/Sprite/ef_revenge1.plist", 2.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
    auto loading_sprite = PlistAnimationSprite::setData(this, "download/Effect/Sprite/ef_revenge1", 1, 0.06f, 10000);
    loading_sprite->setPosition(convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
    loading_sprite->setScale(2.0*ratio);

    DelayTime *delay = DelayTime::create(1.0f);
    CallFunc *expire1 = CallFunc::create([this]()
                                         {
                                             _revenge_effect_finish_flg = true;
                                            if(_from_page_name == "TitleScene"){
                                                this->changeScene();
                                            }
                                         });//ラムダ式
    this->runAction(Sequence::create(delay, expire1, NULL));
}

void ResultLayer::showBossEventEffect()
{
    auto loading_sprite = PlistAnimationSprite::setData(this, "download/Effect/Sprite/ef_boss_event1", 1, 0.06f, 10000);
    loading_sprite->setPosition(convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
    loading_sprite->setScale(2.0*ratio);
    
    DelayTime *delay = DelayTime::create(1.0f);
    CallFunc *expire1 = CallFunc::create([this]()
                                         {
                                             _boss_event_effect_finish_flg = true;
                                             if(_from_page_name == "TitleScene"){
                                                 this->changeScene();
                                             }
                                         });//ラムダ式
    this->runAction(Sequence::create(delay, expire1, NULL));
}

void ResultLayer::changeScene()
{
    if(_revenge_mode==true && SaveData::loadString("online_battle_type")=="quick") {
        if(_revenge_effect_start_flg==false) {
            _revenge_effect_start_flg = true;
            showRevengeEffect();
            return;
        } else if(_revenge_effect_finish_flg==false) {
            return;
        }
    }
    
    if(_boss_event_flg==true) {
        if(_boss_event_effect_start_flg==false) {
            _boss_event_effect_start_flg = true;
            showBossEventEffect();
            return;
        } else if(_boss_event_effect_finish_flg==false) {
            return;
        }
    }
    
    if(!_change_scened_flg){
        _change_scened_flg = true;
        if(SaveData::loadInt("tutorialStatus") == 2){
            //シーンの切り替え
            auto scene = TutorialScene::createScene();
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        }
        else {
            //遷移先ページ名
            string to_page_name = "";
            
            if (_from_page_name == "DungeonScene") {
                if (SaveData::loadString("npc_battle_event_rank_list_id") != "0") {
                    to_page_name = "EventMenu";
                }
                else if (SaveData::loadString("quest_stage_id") != "0") {
                    to_page_name = "NormalQuest";
                }
            }
            
            //ダンジョンデータの初期化
            Utility::initDungeonData();
            
            //ページ遷移の処理
            auto scene = NormalScene::createScene(to_page_name);
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        }
    }
}

bool ResultLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch ResultLayer!");
    return true;
}
void ResultLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ResultLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
	
	//戦利品表示済み かつ 次の戦利品の表示がまだある場合
	if(_showed_booty_popup_flg && _bootyPopup->hasNextContainer()){
		//戦利品をslideして表示する
		_bootyPopup->slideNextContainer() ;
		return ;
	}
	
    HMSLOG("check_count = %d, reward_item_list = %d", _check_count, _item_count);
	
	
	if(_now_popup_queue_index >= _popup_types_queue.size()){
		//インデックスを超えていたらシーン遷移
		changeScene() ;
		return ;
	}
	
	if (_popup_types_queue[_now_popup_queue_index] == PopupType::POPUP_POINT) {
        //タップされたときゲージのスピードを上昇させる
        _pointPopup->gaugeSpeedUp() ;
        
    } else if(_popup_types_queue[_now_popup_queue_index] == PopupType::POPUP_BOOTY) {
        //タップイベントの削除
        _eventDispatcher->removeEventListener(listener);

        Node *fadeOutTarget = prevPopupNode() ;
        
        //獲得ポイントポップアップのアクション
        auto fade = FadeTo::create(0.2f, 180);
        auto move = MoveTo::create(0.5f, Point(fadeOutTarget->getPosition().x - _contentSize.width * ratio ,fadeOutTarget->getPosition().y));
        auto easeIn = EaseBackIn::create(move);
        
        auto remove = CallFunc::create([this, fadeOutTarget](){
            fadeOutTarget->removeFromParent();
        });
        
        auto func = CallFunc::create([this](){
            //獲得戦利品ポップアップのアクション
            auto move = MoveTo::create(0.5f, Point(_bootyPopup->getPosition().x - _contentSize.width * ratio, _bootyPopup->getPosition().y));
            auto easeOut = EaseBackOut::create(move);
            auto fadeIn  = FadeIn::create(0.2f);
            auto func2 = CallFunc::create([&]()
                                          {
                                              //ポップアップ表示後にタッチ可能にする
                                              createEventListener();
                                          });
            
            _bootyPopup->runAction(Sequence::create(easeOut, fadeIn, func2, nullptr));
            
        });
        fadeOutTarget->runAction(Sequence::create(fade, easeIn, remove, func, nullptr));
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");


        _showed_booty_popup_flg = true;
		
		_now_popup_queue_index++ ;
        
    } else if(_popup_types_queue[_now_popup_queue_index] == PopupType::POPUP_ACHIEVEMENT) {
        //タップイベントの削除
        _eventDispatcher->removeEventListener(listener);
        showResultAchievementLayer() ;
		_now_popup_queue_index++ ;
        
    } else if(_popup_types_queue[_now_popup_queue_index] == PopupType::POPUP_FRIENT) {
        //タップイベントの削除
        _eventDispatcher->removeEventListener(listener);
        //タッチスクリーンのスプライトを削除
        _touchScreenSprite->stopAllActions() ;
        _touchScreenSprite->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr )) ;
        //フレンド詳細のアクション
        showFriendRequestPopupAction() ;
		_now_popup_queue_index++ ;
	}
}

void ResultLayer::showResultAchievementLayer(){
    
    Node *fadeOutTarget = prevPopupNode() ;
    
    //前に出ていたポップアップを画面外に移動
    auto move = MoveTo::create(0.5f, Point(fadeOutTarget->getPosition().x - _contentSize.width * ratio ,fadeOutTarget->getPosition().y));
    auto easeIn = EaseBackIn::create(move);
    
    auto func = CallFunc::create([this](){
        _resultAchievementLayer = createResultAchievementLayer() ;
        _resultAchievementLayer->setPosition(convertFromDisplaySize(Point(320 + _contentSize.width, -200), 0.5, 0.5));
        
        this->addChild(_resultAchievementLayer, 2);
        
        //達成条件のポップアップを表示
        auto move = MoveTo::create(0.5f, Point(0, _resultAchievementLayer->getPosition().y));
        auto easeOut = EaseBackOut::create(move);
        auto fadeIn  = FadeIn::create(0.2f);
        
        _resultAchievementLayer->runAction(Sequence::create(easeOut, fadeIn, CallFunc::create([&](){
            //ポップアップ表示後にタッチ可能にする
            createEventListener();
        }), nullptr));
        
    });
    fadeOutTarget->runAction(Sequence::create(FadeTo::create(0.2f, 180), easeIn, func, nullptr));
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
}

ResultAchievementLayer *ResultLayer::createResultAchievementLayer(){
    int npc_battle_event_rank_list_id = fromString<int>(SaveData::loadString("npc_battle_event_rank_list_id"));
    int quest_stage_id                = fromString<int>(SaveData::loadString("quest_stage_id"));
    
    //ResultAchievementLayerを生成するための引数
    HmsVector<string> achivement_txt_list;
    HmsVector<int> old_achievement_flg_list;
    HmsVector<int> new_achievement_flg_list;
    
    //イベントかクエストかによってjsonを切り替える
    Json json_stage_achievement_list ;
    if(npc_battle_event_rank_list_id>0){
        json_stage_achievement_list = json["event_stage_achievement_list"].object_items();
    }else if(quest_stage_id>0){
        json_stage_achievement_list = json["quest_stage_achievement_list"].object_items();
    }
    
    
    for(int i=0 ; i<3; i++){
        //評価条件の文字列の格納
        string i_str = toString<int>(i);
        achivement_txt_list.insert(json_stage_achievement_list[i_str]["reward_text"].string_value()) ;
        
        //過去にクリアした評価条件がクリアしているか調べ old_achievement_flg_listにinsertする
        if(json_stage_achievement_list["old_achievement_flg"][i_str].bool_value()){
            old_achievement_flg_list.insert(1) ;
        }else{
            old_achievement_flg_list.insert(0) ;
        }
        
        //過去にクリアした評価条件がクリアしているか調べ new_achievement_flg_listにinsertする
        if(json_stage_achievement_list["new_achievement_flg"][i_str].bool_value()){
            new_achievement_flg_list.insert(1) ;
        }else{
            new_achievement_flg_list.insert(0) ;
        }
    }
    
    
    //ResultAchievementLayerの生成と再生
    ResultAchievementLayer *resultAchievementLayer= ResultAchievementLayer::create();
    resultAchievementLayer->setup(achivement_txt_list, old_achievement_flg_list, new_achievement_flg_list);
    
    /*
     //ResultAchievementLayerの生成と再生
     ResultAchievementLayer *resultAchievementLayer= ResultAchievementLayer::create();
     resultAchievementLayer->setup(achivement_txt_list, old_achievement_flg_list, new_achievement_flg_list);
     
     ResultAchievementLayerの使用例
     achivement_txt_list.insert("SR以下のデッキでクリアする");
     achivement_txt_list.insert("3ターン以内にクリアする");
     achivement_txt_list.insert("ノーダメージでクリアする");
     old_achievement_flg_list.insert(1);
     old_achievement_flg_list.insert(0);
     old_achievement_flg_list.insert(0);
     new_achievement_flg_list.insert(1);
     new_achievement_flg_list.insert(1);
     new_achievement_flg_list.insert(1);
     
     resultAchievementLayer = ResultAchievementLayer::create();
     resultAchievementLayer->setup(achivement_txt_list, old_achievement_flg_list, new_achievement_flg_list);
     this->addChild(resultAchievementLayer, 2);
     */
    
    return resultAchievementLayer ;
}

void ResultLayer::showFriendRequestPopupAction(){
    Node* fade_target_node = prevPopupNode() ;
    
    //獲得戦利品ポップアップのアクション
    auto fade = FadeTo::create(0.2f, 180);
    auto move = MoveTo::create(0.5f, Point(fade_target_node->getPosition().x - _contentSize.width * ratio ,fade_target_node->getPosition().y));
    auto easeIn = EaseBackIn::create(move);
    
    auto remove = CallFunc::create([this, fade_target_node](){
        fade_target_node->removeFromParent();
    });
    
    fade_target_node->runAction(Sequence::create(fade, easeIn, remove, CallFunc::create([this](){
        //フレンドのポップアップアクション
        auto move = MoveTo::create(0.5f, Point(_userDetailSprite->getPosition().x - _contentSize.width * ratio, _userDetailSprite->getPosition().y));
        auto easeOut = EaseBackOut::create(move);
        auto fadeIn  = FadeIn::create(0.2f);
        
        _userDetailSprite->runAction(Sequence::create(easeOut, fadeIn, CallFunc::create([this](){
            //userDetailSpriteの下端と画面の下端の中心
            float menu_y = (_userDetailSprite->getPosition().y - _userDetailSprite->getContentSize().height/2*ratio) / 2  ;
            //ボタン生成
            Menu *menu = createFriendButtonMenu() ;
            menu->setPosition(convertFromDisplaySize(Point(320, 0), 0.5, 0)+Point(0, menu_y));
            this->addChild(menu, 2) ;
            
            //ボタンアニメーション
            menu->setOpacity(0) ;
            menu->runAction(FadeIn::create(0.2f)) ;
            _setCascadeOpacityEnabled(menu, true);
        }), nullptr));
        
    }), nullptr));
    
}

void ResultLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}


void ResultLayer::demandFollowRequest()
{
    string url = HOST;
    url += "friend/follow_demand.php";
    auto selector = httpresponse_selector(ResultLayer::demandFollowCallback);
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&to_friend_id=" + toString<int>(_follow_target_friend_id);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void ResultLayer::demandFollowCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["success_flg"].bool_value())
    {
        //初心の手引きクリアーフラグ2 フレンドをフォローする(このフラグが立つ処理は他の場所にもあるので注意)
        if (SaveData::loadBool("beginner_guide_clear_flg_2") == false) {
            SaveData::saveBool("beginner_guide_clear_flg_2", true);
            SaveData::saveBool("beginner_layer_open_flg", true);
        }
		
        //申請成功時
        string message = json["user_name"].string_value() + "さんを\nフォローしました。";
        auto yes_func = [this](){
			//SE
			CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
            changeScene() ;
        };
		
        auto popup_layer = PopupOkButtonLayer::create(yes_func, message);
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
    else
    {
        string comment = json["comment"].string_value() ;
        auto yes_func = [this](){
			//SE
			CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
            changeScene() ;
        };
		
        //申請失敗時
        auto popup_layer = PopupOkButtonLayer::create(yes_func, comment);
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}

Node *ResultLayer::prevPopupNode(){
    
    //	if(_now_popup_queue_index >= _popup_types_queue.size())return Node::create() ;
    //	if(_now_popup_queue_index<=0) return Node::create() ;
    
    Node *prevNode ;
    
    //前回に表示されていたポップアップのタイプを取得する
    PopupType prevPopupType = _popup_types_queue[_now_popup_queue_index-1] ;
    
    if(prevPopupType == PopupType::POPUP_POINT){
        prevNode = _pointPopup ;
    }else if(prevPopupType == PopupType::POPUP_BOOTY){
        prevNode = _bootyPopup;
    }else if(prevPopupType == PopupType::POPUP_ACHIEVEMENT){
        prevNode = _resultAchievementLayer ;
    }
    
    return prevNode ;
}

void ResultLayer::touchStartAction()
{
    //タッチ画像
    _touchScreenSprite = TouchScreenSprite::create();
    _touchScreenSprite->setScale(ratio);
    _touchScreenSprite->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0));
    this->addChild(_touchScreenSprite, ZORDER_TOUCH_SCREAN, TOUCH_SCREAN_TAG);
}