#include "RankingEventTopLayer.h"

RankingEventTopLayer::RankingEventTopLayer()
{
}

bool RankingEventTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("RankingEventTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void RankingEventTopLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("国士無双イベント", "QuestMenu", this, HeaderLayer::TitleBarType::WIDTH_390);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //イベント情報の取得要求
    httpRequestForEventInfo() ;
    
}

void RankingEventTopLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(RankingEventTopLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(RankingEventTopLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(RankingEventTopLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(RankingEventTopLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void RankingEventTopLayer::createEndImage()
{
    Sprite *test_end_image = Sprite::create("init/Home/Event/RankingEvent/bk_record_gray.png");
    test_end_image->setPosition(convertFromDisplaySize(Point(320, 422), 0.5, 0.5)) ;
    test_end_image->setScale(ratio) ;
    this->addChild(test_end_image) ;
}

void RankingEventTopLayer::createScoreBoard()
{
    Sprite *record_back = Sprite::create("init/Home/Event/RankingEvent/bk_record.png");
    record_back->setPosition(convertFromDisplaySize(Point(320, 422), 0.5, 0.5)) ;
    record_back->setScale(ratio) ;
    this->addChild(record_back) ;
    
    //ランキングに関する固定の文字を配置
    Label *label_rank_text1 = HMSFontUtil::createWithTTFBySystemFontArgument("順位", FONT_NAME_SYSTEM, 30);
    label_rank_text1->setPosition(Point(50,130)) ;
    label_rank_text1->setAnchorPoint(Point(0, 0.5));
    record_back->addChild(label_rank_text1) ;
    Label *label_rank_text2 = HMSFontUtil::createWithTTFBySystemFontArgument("位", FONT_NAME_SYSTEM, 30);
    label_rank_text2->setPosition(Point(300,130)) ;
    label_rank_text2->setAnchorPoint(Point(0, 0.5));
    record_back->addChild(label_rank_text2) ;
    
    //ランキングの表示
    string ranking_str = _ranking_event_score_data["rank"].string_value() ;
    if(ranking_str=="")ranking_str = "集計中" ;
    Label *ranking = HMSFontUtil::createWithTTFBySystemFontArgument(ranking_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(ranking_str, 38, 190));
    ranking->setPosition(Point(290,130)) ;
    ranking->setAnchorPoint(Point(1, 0.5));
    record_back->addChild(ranking) ;
    
    //スコアに関する固定の文字を配置
    Label *label_score_text1 = HMSFontUtil::createWithTTFBySystemFontArgument("スコア", FONT_NAME_SYSTEM, 30);
    label_score_text1->setPosition(Point(50,85)) ;
    label_score_text1->setAnchorPoint(Point(0, 0.5));
    record_back->addChild(label_score_text1) ;
    Label *label_score_text2 = HMSFontUtil::createWithTTFBySystemFontArgument("Pt", FONT_NAME_SYSTEM, 30);
    label_score_text2->setPosition(Point(300,85)) ;
    label_score_text2->setAnchorPoint(Point(0, 0.5));
    record_back->addChild(label_score_text2) ;
    
    //スコアの表示
    string score_str = _ranking_event_score_data["point"].string_value() ;
    score_str = commaSeparatedString(score_str) ;
    if(score_str=="")score_str = "集計中" ;
    Label *score = HMSFontUtil::createWithTTFBySystemFontArgument(score_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(score_str, 38, 190));
    score->setPosition(Point(290,85)) ;
    score->setAnchorPoint(Point(1, 0.5));
    record_back->addChild(score) ;
    
    //最終更新時刻の表示
    string update_date_str = createUpdateTime(_ranking_event_score_data["update_date"].string_value()) ;
    if(update_date_str!=""){
        Label *update_date = HMSFontUtil::createWithTTFBySystemFontArgument(update_date_str, FONT_NAME_SYSTEM, 20);
        update_date->setPosition(Point(record_back->getContentSize().width/2, 45)) ;
        record_back->addChild(update_date) ;
    }
}

string RankingEventTopLayer::commaSeparatedString(string score){
    int length = (int)score.length() ;
    int roop_count = (length-1)/3 ;
    for(int i=0; i<roop_count; i++){
        score.insert(length-(i+1)*3, ",") ;
    }
    return score ;
}

//最終更新時間の文字列を整形
string RankingEventTopLayer::createUpdateTime(string update_date)
{
    string date_string = "" ;

    if (update_date != "") {
        string month  = update_date.substr(5, 2);
        string day    = update_date.substr(8, 2);
        string hour   = update_date.substr(11, 2);
        string minute = update_date.substr(14, 2);

        if (month.substr(0, 1) == "0"){
            //一桁の場合
            month = month.at(1);
        }
        
        if (day.substr(0, 1) == "0"){
            //一桁の場合
            day = day.at(1);
        }
        HMSLOG("month = %s, day = %s, hour = %s, minute = %s", month.c_str(), day.c_str(), hour.c_str(), minute.c_str());
        date_string = "最終更新時刻: " + month + "月" + day + "日" + " " + hour + ":" + minute ;
    }
    
    return date_string;
}

void RankingEventTopLayer::createRewardListButton()
{
    //イベント報酬一覧ボタン
    auto detail_off = Sprite::create("init/Home/Event/RankingEvent/bt_reward.png");
    auto detail_on  = Sprite::create("init/Home/Event/RankingEvent/bt_reward.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string ranking_event_id = _ranking_event_data["0"]["ranking_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__ranking_event_reward_list&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + ranking_event_id + "&level=1&is_rank=0";
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント報酬一覧");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(74, 169), 0.5, 0.5));
    this->addChild(menu, O_button);
}

void RankingEventTopLayer::createHelpButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Event/RankingEvent/bt_help.png");
    auto detail_on  = Sprite::create("init/Home/Event/RankingEvent/bt_help.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string ranking_event_id = _ranking_event_data["0"]["ranking_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__ranking_event_detail&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + ranking_event_id;
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ヘルプ");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(566, 169), 0.5, 0.5));
    this->addChild(menu, O_button);
}

void RankingEventTopLayer::createFollowInfoButton()
{
    //友達の状況のボタンの作成
    auto detail_off = Sprite::create("init/Home/Event/RankingEvent/bt_follow.png");
    auto detail_on  = Sprite::create("init/Home/Event/RankingEvent/bt_follow.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string ranking_event_id = _ranking_event_data["0"]["ranking_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 
                                                 url += "user/index.php?p=page__ranking_event_follow_info&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + ranking_event_id;
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "フォローの状況");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(566, 304), 0.5, 0.5));
    this->addChild(menu, O_button);
    
}

void RankingEventTopLayer::createRankingButton()
{
    //ランキングボタンの作成
    auto detail_off = Sprite::create("init/Home/Event/RankingEvent/bt_ranking.png");
    auto detail_on  = Sprite::create("init/Home/Event/RankingEvent/bt_ranking.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string ranking_event_id = _ranking_event_data["0"]["ranking_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 
                                                 url += "user/index.php?p=page__ranking_event_ranking&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + ranking_event_id ;
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ランキング");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(74, 304), 0.5, 0.5));
    this->addChild(menu, O_button);
    
}

void RankingEventTopLayer::httpRequestForEventInfo()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/get_ranking_event_info.php";
    auto selector = httpresponse_selector(RankingEventTopLayer::onHttpRequestCompleted);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void RankingEventTopLayer::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //イベントパワーのデータを保存
    string event_power = json["event_power_data"]["event_power"].string_value() ;
    string max_event_power = json["event_power_data"]["max_event_power"].string_value() ;
    string event_power_use_time = json["event_power_data"]["event_power_use_time"].string_value() ;
    string timestamp_event_power_use_time = json["event_power_data"]["timestamp_event_power_use_time"].string_value() ;
    
    _ranking_event_data = json["ranking_event_data"].object_items();
    _ranking_event_score_data = json["ranking_event_score_data"].object_items();
    
    savePowerData(event_power, max_event_power, event_power_use_time, timestamp_event_power_use_time) ;
    
    if (countJsonData(_ranking_event_data)) {
        //---------- イベント未開催時 or イベント結果表示時 ------------
        //ページを作成する
        setPageState();
        createPage();
    } else {
        //---------- イベント未開催時 ------------
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("QuestMenu");
        };
        
        string comment = "現在開催している\nイベントはございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
}

void RankingEventTopLayer::setPageState(){
    bool is_event_open = _ranking_event_data["0"]["event_open_flg"].bool_value() ;
    bool is_result_display = _ranking_event_data["0"]["result_display_flg"].bool_value() ;
    
    if(is_event_open){
        _page_state = PageState::EVENT_OPEN ;
    }else if(is_result_display){
        _page_state = PageState::RESULT_DISPLAY ;
    }
    
}

void RankingEventTopLayer::savePowerData(string event_power, string max_event_power, string event_power_use_time, string timestamp_event_power_use_time) {
    SaveData::saveString("max_event_power", max_event_power); //パワーの最大値
    SaveData::saveString("event_power_use_time", event_power_use_time);// 最終使用時間
    SaveData::saveString("timestamp_event_power_use_time", timestamp_event_power_use_time);// 最終使用時間のタイムスタンプ
    SaveData::saveString("event_power", event_power); //event_powerを保存
}

void RankingEventTopLayer::createPage()
{
    createEventExplainSprite();
    createRewardListButton() ;
    createHelpButton() ;
    createFollowInfoButton();
    createRankingButton();
    
    if(_page_state == PageState::EVENT_OPEN) {
        createEventPower() ;
        createChallengeButton() ;
        createScoreBoard() ;
    }else if(_page_state == PageState::RESULT_DISPLAY){
        createResultButton() ;
        createEndImage() ;
    }
}

void RankingEventTopLayer::createEventPower()
{
    EventPowerSprite *eventPowerSprite = EventPowerSprite::create() ;
    eventPowerSprite->setScale(ratio) ;
    eventPowerSprite->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH, DISPLAY_HEIGHT-HEADER_SIZE+10), 0.5, 1)) ;
    eventPowerSprite->setAnchorPoint(Point(1, 1)) ;
    this->addChild(eventPowerSprite,O_eventPowerSprite_weak,T_eventPowerSprite) ;
}

void RankingEventTopLayer::createEventExplainSprite()
{
    //イベント説明画像の作成
    string ranking_event_id = _ranking_event_data["0"]["ranking_event_id"].string_value();
    string comment = _ranking_event_data["0"]["comment"].string_value();
    string image_path = "download/EventMenu/RankingEvent/event" + ranking_event_id + "/event_explain_" + ranking_event_id + ".png";
    string end_date = _ranking_event_data["0"]["end_date"].string_value() ;
    
    RankingEventExplainSprite *explain_sp = RankingEventExplainSprite::create(comment, image_path);
    explain_sp->setScale(ratio);
    explain_sp->setPosition(convertFromDisplaySize(Point(320, 638), 0.5, 0.5)); // 80 = ヘッダー下部の戻るボタンの高さ
    this->addChild(explain_sp, O_explain_sp);
    
    //イベント終了時刻画像の作成
    Sprite *event_time = createEventEndDate(end_date);
    event_time->setScale(0.0); //アニメーションで表示して削除するために初期のサイズを0倍にしておく
    event_time->setPosition(Point(explain_sp->getContentSize().width - 64, 50));
    explain_sp->addChild(event_time, O_event_time);
    
    //表示して削除するアクション
    ScaleTo *scaleTo_big = ScaleTo::create(2.0f, 1) ; //2秒かけて1倍に拡大
    EaseElasticOut *scale_ease_out = EaseElasticOut::create(scaleTo_big);
    
    ScaleTo *scaleTo_small = ScaleTo::create(0.7f, 0) ; //0.7秒かけて0倍に
    EaseBackIn *scale_back_in = EaseBackIn::create(scaleTo_small);
    
    Sequence *popup_seq = Sequence::create(DelayTime::create(1.0f), scale_ease_out, DelayTime::create(1.0f), scale_back_in, RemoveSelf::create(), nullptr) ;
    event_time->runAction(popup_seq) ;
}

Sprite* RankingEventTopLayer::createEventEndDate(string end_date)
{
    Sprite *red_bg = Sprite::create("init/Home/Quest/event_term.png");
    
    if (end_date != "")
    {
        string month  = end_date.substr(5, 2);
        string day    = end_date.substr(8, 2);
        string hour   = end_date.substr(11, 2);
        string minute = end_date.substr(14, 2);
        
        if (month.substr(0, 1) == "0"){
            //一桁の場合
            month = month.at(1);
        }
        if (day.substr(0, 1) == "0"){
            //一桁の場合
            day = day.at(1);
        }
        HMSLOG("month = %s, day = %s, hour = %s, minute = %s", month.c_str(), day.c_str(), hour.c_str(), minute.c_str());
        
        //スラッシュ画像
        auto slash = Sprite::create("init/Home/Quest/slash.png");
        slash->setPosition(Point(red_bg->getContentSize().width / 2 - 6, red_bg->getContentSize().height / 2 + 10));
        red_bg->addChild(slash, 1);
        
        //日付
        auto month_label = Label::createWithBMFont("init/Home/Quest/num_g.fnt", month);
        if (month.size() == 2) {
            //2桁の場合、少し小さく
            month_label->setScale(0.88);
        }
        month_label->setPosition(Point(red_bg->getContentSize().width / 2 - 22, red_bg->getContentSize().height / 2 + 50));
        red_bg->addChild(month_label, 2);
        
        auto day_label = Label::createWithBMFont("init/Home/Quest/num_g.fnt", day);
        if (day.size() == 2) {
            //2桁の場合、少し小さく
            day_label->setScale(0.88);
        }
        day_label->setPosition(Point(red_bg->getContentSize().width / 2 + 22, red_bg->getContentSize().height / 2 + 10));
        red_bg->addChild(day_label, 2);
        
        auto time_label = HMSFontUtil::createWithTTFBySystemFontArgument(hour + ":" + minute + "まで", FONT_NAME_SYSTEM, 22);
        time_label->setPosition(Point(red_bg->getContentSize().width / 2 - 4, 24));
        time_label->setColor(Color3B(240, 240, 240));
        time_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        red_bg->addChild(time_label, 3, END_DATE_LABEL_TAG);
    }
    
    return red_bg;
}

bool RankingEventTopLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タップ位置がeventPowerSpriteの範囲内であればZOrderを手前に変更する
    auto eventPowerSprite = (EventPowerSprite *)this->getChildByTag(T_eventPowerSprite);
    if(eventPowerSprite && Utility::containsPoint(eventPowerSprite, touch->getLocation())){
        eventPowerSprite->setLocalZOrder(O_eventPowerSprite_strong) ;
    }
    
    return true;
}
void RankingEventTopLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void RankingEventTopLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タップが終了したときeventPowerSpriteのZOrderを奥に変更する
    auto eventPowerSprite = (EventPowerSprite *)this->getChildByTag(T_eventPowerSprite);
    if(eventPowerSprite){
        eventPowerSprite->setLocalZOrder(O_eventPowerSprite_weak) ;
    }
}
void RankingEventTopLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void RankingEventTopLayer::changePage()
{
    auto normal_scene = (NormalScene *)this->getParent();
    
    //選択したイベント難易度のデータの保存
    SaveData::saveString("ranking_event_stage_data", _ranking_event_data["0"]["ranking_event_stage"].dump());
    HMSLOG("ranking_event_stage_data = %s", SaveData::loadString("ranking_event_stage_data").c_str());
    
    //選択したイベントのIDを保存
    SaveData::saveString("ranking_event_id", _ranking_event_data["0"]["ranking_event_id"].string_value());
    
    //イベント説明画像のデータの保存
    createEventExplainData();
    
    //難易度選択ページへ遷移
    normal_scene->changePage("RankingEventRankMenu");
}

void RankingEventTopLayer::createResultButton()
{
    auto button_off = Sprite::create("init/Home/Event/RankingEvent/bt_event_result.png");
    auto button_on  = Sprite::create("init/Home/Event/RankingEvent/bt_event_result.png");
    button_on->setColor(Color3B(180, 180, 180));
    
    auto challenge_sprite = MenuItemSprite::create(button_off, button_on, [this](Ref* sender)
                                             {
                                                 string ranking_event_id = _ranking_event_data["0"]["ranking_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__ranking_event_result&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + ranking_event_id;
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント結果");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    challenge_sprite->setPosition(convertFromDisplaySize(Point(320, 218), 0.5, 0.5)) ;
    challenge_sprite->setScale(ratio) ;
    
    auto menu = Menu::create(challenge_sprite, nullptr);
    menu->setPosition(Point(0,0));
    
    this->addChild(menu) ;
}

void RankingEventTopLayer::createChallengeButton()
{
    auto button_off = Sprite::create("init/Home/Event/RankingEvent/bt_challenge.png");
    auto button_on  = Sprite::create("init/Home/Event/RankingEvent/bt_challenge.png");
    button_on->setColor(Color3B(180, 180, 180));
    
    auto challenge_sprite = MenuItemSprite::create(button_off, button_on, [this](Ref* sender)
                                         {
                                             //進む
                                             changePage() ;
                                         });
    challenge_sprite->setPosition(convertFromDisplaySize(Point(320, 218), 0.5, 0.5)) ;
    challenge_sprite->setScale(ratio) ;
    
    auto menu = Menu::create(challenge_sprite, nullptr);
    menu->setPosition(Point(0,0));
    
    this->addChild(menu) ;
}

void RankingEventTopLayer::createEventExplainData()
{
    
    //イベント説明画像のデータの作成
    string ranking_event_id = _ranking_event_data["0"]["ranking_event_id"].string_value();
    string name                = _ranking_event_data["0"]["name"].string_value();
    string comment             = _ranking_event_data["0"]["comment"].string_value();
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"ranking_event_id\":\"" + ranking_event_id+ "\"," + cr;
    data += "  \"name\":\"" + name + "\"," + cr;
    data += "  \"comment\":\"" + comment + "\"" + cr;
    data += "}";
    HMSLOG("ranking_event_explain_data = %s", data.c_str()) ;
    
    SaveData::saveString("ranking_event_explain_data", data);
}

