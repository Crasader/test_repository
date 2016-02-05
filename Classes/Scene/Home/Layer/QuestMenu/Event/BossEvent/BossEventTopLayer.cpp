#include "BossEventTopLayer.h"

BossEventTopLayer::BossEventTopLayer()
{
}

bool BossEventTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BossEventTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void BossEventTopLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("匈奴討伐イベント", "TeamTop", this, HeaderLayer::TitleBarType::WIDTH_466);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //イベント情報の取得要求
    httpRequestForEventInfo() ;
    
}

void BossEventTopLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BossEventTopLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BossEventTopLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BossEventTopLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BossEventTopLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void BossEventTopLayer::createRecordTeam()
{
    Sprite *record_back = Sprite::create("init/Home/Event/BossEvent/record_team.png");
    record_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 472), 0.5, 0.5)) ;
    record_back->setScale(ratio) ;
    this->addChild(record_back) ;
    
    //ランキングの表示
    string ranking_str = _boss_event_score_data_team["rank"].string_value() ;
    if(ranking_str=="")ranking_str = "集計中" ;
    else ranking_str += "位" ;
    Label *ranking = HMSFontUtil::createWithTTFBySystemFontArgument(ranking_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(ranking_str, 25, 150));
    ranking->setHorizontalAlignment(TextHAlignment::RIGHT);
    ranking->setPosition(Point(380,79)) ;
    ranking->setAnchorPoint(Point(1, 0.5));
    record_back->addChild(ranking) ;
    
    //スコアの表示
    string score_str = _boss_event_score_data_team["point"].string_value() ;
    score_str = commaSeparatedString(score_str) ;
    if(score_str=="")score_str = "集計中" ;
    else score_str += "Pt" ;
    Label *score = HMSFontUtil::createWithTTFBySystemFontArgument(score_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(score_str, 25, 240));
    score->setHorizontalAlignment(TextHAlignment::RIGHT);
    score->setPosition(Point(380,43)) ;
    score->setAnchorPoint(Point(1, 0.5));
    record_back->addChild(score) ;
    
//    //最終更新時刻の表示
//    string update_date_str = createUpdateTime(_boss_event_score_data_team["update_date"].string_value()) ;
//    if(update_date_str!=""){
//        Label *update_date = HMSFontUtil::createWithTTFBySystemFontArgument(update_date_str, FONT_NAME_SYSTEM, 20);
//        update_date->setPosition(Point(record_back->getContentSize().width/2, 45)) ;
//        record_back->addChild(update_date) ;
//    }
}

void BossEventTopLayer::createRecordPersonal()
{
    Sprite *record_back = Sprite::create("init/Home/Event/BossEvent/record_personal.png");
    record_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 321), 0.5, 0.5)) ;
    record_back->setScale(ratio) ;
    this->addChild(record_back) ;
    
    //ランキングの表示
    string ranking_str = _boss_event_score_data_personal["rank"].string_value() ;
    if(ranking_str=="")ranking_str = "集計中" ;
    else ranking_str += "位" ;
    Label *ranking = HMSFontUtil::createWithTTFBySystemFontArgument(ranking_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(ranking_str, 25, 150));
    ranking->setHorizontalAlignment(TextHAlignment::RIGHT);
    ranking->setPosition(Point(380,79)) ;
    ranking->setAnchorPoint(Point(1, 0.5));
    record_back->addChild(ranking) ;
    
    //スコアの表示
    string score_str = _boss_event_score_data_personal["point"].string_value() ;
    score_str = commaSeparatedString(score_str) ;
    if(score_str=="")score_str = "集計中" ;
    else score_str += "Pt" ;
    Label *score = HMSFontUtil::createWithTTFBySystemFontArgument(score_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(score_str, 25, 240));
    score->setHorizontalAlignment(TextHAlignment::RIGHT);
    score->setPosition(Point(380,43)) ;
    score->setAnchorPoint(Point(1, 0.5));
    record_back->addChild(score) ;
    
//    //最終更新時刻の表示
//    string update_date_str = createUpdateTime(_boss_event_score_data_personal["update_date"].string_value()) ;
//    if(update_date_str!=""){
//        Label *update_date = HMSFontUtil::createWithTTFBySystemFontArgument(update_date_str, FONT_NAME_SYSTEM, 20);
//        update_date->setPosition(Point(record_back->getContentSize().width/2, 45)) ;
//        record_back->addChild(update_date) ;
//    }
}


string BossEventTopLayer::commaSeparatedString(string score){
    int length = (int)score.length() ;
    int roop_count = (length-1)/3 ;
    for(int i=0; i<roop_count; i++){
        score.insert(length-(i+1)*3, ",") ;
    }
    return score ;
}

//最終更新時間の文字列を整形
string BossEventTopLayer::createUpdateTime(string update_date)
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

void BossEventTopLayer::createRewardListButton()
{
    //イベント報酬一覧ボタン
    auto detail_off = Sprite::create("init/Home/Event/BossEvent/bt_reward.png");
    auto detail_on  = Sprite::create("init/Home/Event/BossEvent/bt_reward.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string boss_event_id = _boss_event_data["0"]["boss_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__boss_event_reward_list&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + boss_event_id + "&level=1&is_rank=0";
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント報酬一覧");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(112, 171), 0.5, 0.5)) ;
    this->addChild(menu, ZORDER_BUTTON);
}

void BossEventTopLayer::createRankingButton()
{
    //ランキングボタンの作成
    auto detail_off = Sprite::create("init/Home/Event/BossEvent/bt_ranking.png");
    auto detail_on  = Sprite::create("init/Home/Event/BossEvent/bt_ranking.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string boss_event_id = _boss_event_data["0"]["boss_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 
                                                 url += "user/index.php?p=page__boss_event_ranking&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + boss_event_id ;
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ランキング");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(534, 171), 0.5, 0.5)) ;
    this->addChild(menu, ZORDER_BUTTON);
}

void BossEventTopLayer::createHelpButton()
{
    //ヘルプボタン
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [this](Ref* sender) {
                                                 string boss_event_id = _boss_event_data["0"]["boss_event_id"].string_value();
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__boss_event_detail_1&user_id=" + SaveData::loadString("user_master_id") + "&event_id=" + boss_event_id;
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ヘルプ");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                         });
    
    help_btn->setScale(ratio);
    help_btn->setAnchorPoint(Point(0.5, 1));
    auto menu = Menu::create(help_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(570, 838), 0.5, 1));
    this->addChild(menu, 5);
}

void BossEventTopLayer::httpRequestForEventInfo()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/boss_event/get_boss_event_info.php";
    auto selector = httpresponse_selector(BossEventTopLayer::onHttpRequestCompleted);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BossEventTopLayer::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    string error_status = json["error_status"].string_value();
    _boss_event_data = json["boss_event_data"].object_items();
    _boss_event_score_data_team = json["boss_event_score_data"]["team"].object_items();
    _boss_event_score_data_personal = json["boss_event_score_data"]["personal"].object_items();
    _boss_event_stage_data = json["boss_event_stage_data"].object_items();
    _boss_count = countJsonData(_boss_event_stage_data);
    
    _event_count = countJsonData(_boss_event_data);
    
    if (_event_count >= 1)
    {
        //----------- イベント開催時 -------------
        
        //ページを作成する
        createPage();
    }
    else if(error_status == "not_belong")
    {
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("TeamNoBelong");
        };
        
        string comment = "現在チームに所属していません。\nこのイベントに参加するには、\nチームへの所属が必要です。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
    else
    {
        //---------- イベント未開催時 ------------
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("TeamTop");
        };
        
        string comment = "現在開催している\nイベントはございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
}

void BossEventTopLayer::createPage()
{
    createBossEventExplainSprite();
    createButton() ;
    createRewardListButton() ;
    createHelpButton() ;
    createRankingButton();
    
    createRecordTeam() ;
    createRecordPersonal() ;
    
}

void BossEventTopLayer::createBossEventExplainSprite()
{
    //イベント説明画像の作成
    string boss_event_id = _boss_event_data["0"]["boss_event_id"].string_value();
    string comment = _boss_event_data["0"]["comment"].string_value();
    string image_path = "download/EventMenu/BossEvent/event_explain.png";
    string end_date = _boss_event_data["0"]["end_date"].string_value() ;
    
    explain_sp = BossEventExplainSprite::create(comment, image_path);
    explain_sp->setPosition(convertFromDisplaySize(Point(320, 647), 0.5, 0.5)) ;
    explain_sp->setScale(ratio) ;
    this->addChild(explain_sp, 1);
    
    //イベント終了時刻画像の作成
    Sprite *event_time = createEventEndDate(end_date);
    event_time->setScale(0.0); //アニメーションで表示して削除するために初期のサイズを0倍にしておく
    event_time->setPosition(Point(explain_sp->getContentSize().width - 64, 50));
    explain_sp->addChild(event_time, 2);
    
    //表示して削除するアクション
    ScaleTo *scaleTo_big = ScaleTo::create(2.0f, 1) ; //2秒かけて1倍に拡大
    EaseElasticOut *scale_ease_out = EaseElasticOut::create(scaleTo_big);
    
    ScaleTo *scaleTo_small = ScaleTo::create(0.7f, 0) ; //0.7秒かけて0倍に
    EaseBackIn *scale_back_in = EaseBackIn::create(scaleTo_small);
    
    Sequence *popup_seq = Sequence::create(DelayTime::create(1.0f), scale_ease_out, DelayTime::create(1.0f), scale_back_in, RemoveSelf::create(), nullptr) ;
    event_time->runAction(popup_seq) ;
}

Sprite* BossEventTopLayer::createEventEndDate(string end_date)
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

bool BossEventTopLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
    return true;
}
void BossEventTopLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BossEventTopLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BossEventTopLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void BossEventTopLayer::changeBattlePage()
{
    auto normal_scene = (NormalScene *)this->getParent();
    
    //選択したイベント難易度のデータの保存
    SaveData::saveString("boss_event_stage_data", _boss_event_stage_data.dump());
    HMSLOG("boss_event_stage_data = %s", _boss_event_stage_data.dump().c_str());
    
    //選択したイベントのIDを保存
    SaveData::saveString("boss_event_id", _boss_event_data["0"]["boss_event_id"].string_value());
    
    //イベント説明画像のデータの保存
    createEventExplainData();
    
    //難易度選択ページへ遷移
    normal_scene->changePage("BossEventRankMenu");
}

void BossEventTopLayer::changeTeamPage()
{
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("TeamTop");
}

void BossEventTopLayer::createButton()
{
    if(_boss_count>0) {
        Sprite *battle_off = Sprite::create("init/Home/Event/BossEvent/bt_battle.png");
        Sprite *battle_on  = Sprite::create("init/Home/Event/BossEvent/bt_battle.png");
        battle_on->setColor(Color3B(180, 180, 180));
        
        MenuItemSprite *battle_button = MenuItemSprite::create(battle_off, battle_on, [this](Ref* sender)
                                                               {
                                                                   changeBattlePage() ;
                                                               });
        battle_button->setPosition(convertFromDisplaySize(Point(320, 171), 0.5, 0.5)) ;
        battle_button->setScale(ratio) ;
        
        Menu *menu1 = Menu::create(battle_button, nullptr);
        menu1->setPosition(Point(0,0));
        
        this->addChild(menu1, 10000) ;
        
        //ボスの数を表示
        Layer *layer = Layer::create();
        //画像
        Sprite *img = Sprite::create("init/Home/Event/BossEvent/boss_appearance3.png");
        layer->addChild(img);
        
        //テキスト
        if(_boss_count>=100) _boss_count = 99;
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument("ボス" + to_string(_boss_count) + "体出現中", FONT_NAME_SYSTEM, 26);
        label->setHorizontalAlignment(TextHAlignment::CENTER);
        label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        label->setPosition(Point(0, 0));
        layer->addChild(label);
        
        //レイヤー
        layer->setAnchorPoint(Point(0, 0));
        layer->setPosition(Point(110, 66+65));
        battle_button->addChild(layer);
        
        ScaleTo *scale1 = ScaleTo::create(0.5f, 1.1);
        ScaleTo *scale2 = ScaleTo::create(0.5f, 1.0);
        RepeatForever *action =  RepeatForever::create(Sequence::create(scale1, scale2, nullptr));
        layer->runAction(action);
    } else {
        Sprite *img = Sprite::create("init/Home/Event/BossEvent/bt_battle_gray.png");
        img->setScale(ratio);
        img->setPosition(convertFromDisplaySize(Point(320, 171), 0.5, 0.5));
        this->addChild(img);
        
        Sprite* popup = Sprite::create("init/Home/Event/BossEvent/bt_battle_ex.png");
        popup->setScale(ratio);
        popup->setPosition(convertFromDisplaySize(Point(320, 215), 0.5, 0.5));
        this->addChild(popup, ZORDER_BUTTON+100);
        
        //表示して削除するアクション
        ScaleTo *scaleTo_big = ScaleTo::create(2.0f, ratio) ;
        EaseElasticOut *scale_ease_out = EaseElasticOut::create(scaleTo_big);
        
        ScaleTo *scaleTo_small = ScaleTo::create(0.7f, 0) ;
        EaseBackIn *scale_back_in = EaseBackIn::create(scaleTo_small);
        
        Sequence* seq = Sequence::create(DelayTime::create(1.0f), scale_ease_out, scale_back_in, RemoveSelf::create(), nullptr) ;
        popup->runAction(seq) ;
    }


//
//
//    Sprite *team_off = Sprite::create("init/Home/Event/BossEvent/bt_team.png");
//    Sprite *team_on  = Sprite::create("init/Home/Event/BossEvent/bt_team.png");
//    team_on->setColor(Color3B(180, 180, 180));
//    
//    MenuItemSprite *team_button = MenuItemSprite::create(team_off, team_on, [this](Ref* sender)
//                                         {
//                                             changeTeamPage() ;
//                                         });
//    team_button->setPosition(convertFromDisplaySize(Point(71, FOOTER_SIZE+320), 0.5, 0.0)) ;
//    team_button->setAnchorPoint(Point(0.5, 0)) ;
//    team_button->setScale(ratio) ;
//    
//    Menu *menu2 = Menu::create(team_button, nullptr);
//    menu2->setPosition(Point(0,0));
//    
//    this->addChild(menu2) ;
//    
//    
}

void BossEventTopLayer::createEventExplainData()
{
    
    //イベント説明画像のデータの作成
    string boss_event_id = _boss_event_data["0"]["boss_event_id"].string_value();
    string name                = _boss_event_data["0"]["name"].string_value();
    string comment             = _boss_event_data["0"]["comment"].string_value();
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"boss_event_id\":\"" + boss_event_id+ "\"," + cr;
    data += "  \"name\":\"" + name + "\"," + cr;
    data += "  \"comment\":\"" + comment + "\"" + cr;
    data += "}";
    HMSLOG("boss_event_explain_data = %s", data.c_str()) ;
    
    SaveData::saveString("boss_event_explain_data", data);
}

