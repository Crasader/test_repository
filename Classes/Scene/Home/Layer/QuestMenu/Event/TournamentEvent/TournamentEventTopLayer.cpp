#include "TournamentEventTopLayer.h"

TournamentEventTopLayer::TournamentEventTopLayer()
{
}

TournamentEventTopLayer* TournamentEventTopLayer::create()
{
    TournamentEventTopLayer *sprite = new TournamentEventTopLayer();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEventTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TournamentEventTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TournamentEventTopLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("トーナメントイベント", "QuestMenu", this);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //ヘルプボタンの作成
    createHelpButton();
    
    //トーナメント参加中か通信処理
    httpRequestForEventInfo();
}

void TournamentEventTopLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TournamentEventTopLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentEventTopLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentEventTopLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TournamentEventTopLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentEventTopLayer::createHelpButton()
{
    //ヘルプボタン
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on,[this](Ref* sender){
        
        auto normal_scene = (NormalScene *)this->getParent();
        
        //WebView表示
        string url = HOST;
        url += "user/index.php?&p=page__tournament_help&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベントヘルプ");
        normal_scene->addChild(WebViewLoadLayer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    });
    help_btn->setScale(ratio);
    
    auto menu = Menu::create(help_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, Z_Help_Menu, T_Help_Menu);
}

void TournamentEventTopLayer::createTournamentTitle()
{
    //トーナメントタイトル画像の作成
    auto title_sp = Sprite::create("init/Home/Event/TournamentEvent/maindraw.png");
    title_sp->setScale(ratio);
    title_sp->setPosition(convertFromDisplaySize(Point(320, 700), 0.5, 0.5));
    this->addChild(title_sp, Z_Title_Sprite, T_Title_Sprite);
}

void TournamentEventTopLayer::createTournamentInfoSprite(string image_path)
{
    //トーナメント参加情報画像の生成
    
    //情報の背景
    int position_y = 550;
    if (_tournament_status != Not_Entry) {
        _info_bg = (Sprite *)TournamentEventTopSprite::create(_tournament_data_list, image_path);
        position_y = 570;
    }
    else {
        _info_bg = Sprite::create(image_path);
    }
    _info_bg->setScale(ratio);
    _info_bg->setPosition(convertFromDisplaySize(Point(320, position_y), 0.5, 0.5));
    this->addChild(_info_bg, Z_Info_Sprite, T_Info_Sprite);
}

void TournamentEventTopLayer::createNaviSprite(string navi_comment)
{
    //ナビ画像の作成
    auto navi_sp = Sprite::create("init/Home/Event/TournamentEvent/message_navi1.png");
    navi_sp->setScale(ratio);
    navi_sp->setPosition(convertFromDisplaySize(Point(320, 370), 0.5, 0.5));
    this->addChild(navi_sp, Z_Navi_Sprite, T_Navi_Sprite);
    
    //ナビの言葉
    string navi_text = navi_comment;
    auto navi_label = HMSFontUtil::createWithTTFBySystemFontArgument(navi_text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(navi_text, 28, 450));
    navi_label->setPosition(Point(navi_sp->getContentSize().width / 2 + 50, navi_sp->getContentSize().height / 2 - 10));
    navi_sp->addChild(navi_label, Z_Navi_Label, T_Navi_Label);
}

void TournamentEventTopLayer::createTournamentButton(string image_path)
{
    //トーナメント一覧
    auto list_off = Sprite::create(image_path);
    auto list_on  = Sprite::create(image_path);
    list_on->setColor(Color3B(180, 180, 180));
    
    auto list_btn = MenuItemSprite::create(list_off, list_on,[this](Ref* sender){
        
        switch (_tournament_status) {
            case Not_Entry:
            case Close_Tournament:
            {
                //トーナメント一覧へページ遷移
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("TournamentList");
            }break;
            default:
            {
                //トーナメント対戦待機ページへ
                auto normal_scene = (NormalScene *)this->getParent();
//                normal_scene->changePage("TournamentEventBattleWait");
                normal_scene->changePage("TournamentList");
            }break;
        }
    });
    list_btn->setScale(ratio);
    list_btn->setTag(T_Tournament_List);
    
    auto menu = Menu::create(list_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 260), 0.5, 0.5));
    this->addChild(menu, Z_Tournament_List);
}

void TournamentEventTopLayer::createWebViewButton()
{
    //戦績、対戦表のボタンの作成
    auto recode_off = Sprite::create("init/Home/Event/TournamentEvent/bt_tournament_kos.png");
    auto recode_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_tournament_kos.png");
    recode_on->setColor(Color3B(180, 180, 180));
    
    auto recode_btn = MenuItemSprite::create(recode_off, recode_on,[this](Ref* sender){
        
        //トーナメント戦績の表示
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("TournamentRecord");
        
    });
    recode_btn->setScale(ratio);
    recode_btn->setTag(T_Recode);
    
    //対戦表
    auto bracket_off = Sprite::create("init/Home/Event/TournamentEvent/bt_competition_table.png");
    auto bracket_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_competition_table.png");
    bracket_on->setColor(Color3B(180, 180, 180));
    
    auto bracket_btn = MenuItemSprite::create(bracket_off, bracket_on,[this](Ref* sender){
        //対戦表ページへ遷移
        SaveData::saveString("bracket_type", "display") ;
        SaveData::saveString("bracket_back", "top") ;
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePageFadeAction("TournamentEventBracket");
    });
    bracket_btn->setScale(ratio);
    bracket_btn->setTag(T_Bracket);
    
    auto menu2 = Menu::create(recode_btn, bracket_btn, nullptr);
    menu2->alignItemsHorizontallyWithPadding(0);
    menu2->setPosition(convertFromDisplaySize(Point(320, 150), 0.5, 0.5));
    this->addChild(menu2, Z_Bracket);
    
}

void TournamentEventTopLayer::createNextBattleTimeSprite()
{
    //次の対戦時刻までの画像の表示
    auto normal_scene = (Layer *)this->getParent();
    auto remain_time_sp = TournamentRemainTimeSprite::create(fromString<int>(_tournament_data_list["total_battle_number"].string_value()), fromString<int>(_tournament_data_list["now_battle_number"].string_value()), normal_scene);
    remain_time_sp->setScale(0.62 * ratio);
    remain_time_sp->setPosition(convertFromDisplaySize(Point(550, 480), 0.5, 0.5));
    this->addChild(remain_time_sp, Z_Tournament_Time, T_Tournament_Time);
}

void TournamentEventTopLayer::httpRequestForEventInfo()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/tournament_event/get_user_entry_tournament_info.php";
    auto selector = httpresponse_selector(TournamentEventTopLayer::onHttpRequestCompleted);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEventTopLayer::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //トーナメント参加状態とナビの文言の取得
    _tournament_status = (TournamentStatus)json["tournament_status"].int_value();
    string navi_comment = json["navi_comment"].string_value();
    
    //トーナメント情報
    _tournament_data_list = json["tournament_data_list"].object_items();
    
    //トーナメント情報の取得
    if (_tournament_data_list["tournament_event_list_id"].string_value() != ""){
        setCallbackTournamentData();
    }
    
    string info_image_path = "";
    string list_image_path = "";
    
    switch (_tournament_status) {
        case Not_Entry:
        {
            //イベント開催中、且つ、未エントリー
            info_image_path = "init/Home/Event/TournamentEvent/bk_tournament_insession.png";
            list_image_path = "init/Home/Event/TournamentEvent/bt_tournament_list.png";
        }break;
        case Wait_Bracket:
        {
            //エントリー済み、且つ、トーナメント作成待ち状態
            info_image_path = "init/Home/Event/TournamentEvent/bk_tournament5.png";
            list_image_path = "init/Home/Event/TournamentEvent/bt_tournament_preparation.png";
        }break;
        case Wait_Tournamnt_Start:
        {
            //エントリー済み、且つ、トーナメント開催待ち状態
            info_image_path = "init/Home/Event/TournamentEvent/bk_tournament5.png";
            list_image_path = "init/Home/Event/TournamentEvent/bt_tournament_preparation.png";
        }break;
        case Open_Tournament:
        {
            //エントリー済み、且つ、トーナメント開催中状態
            info_image_path = "init/Home/Event/TournamentEvent/bk_tournament5.png";
            list_image_path = "init/Home/Event/TournamentEvent/bt_join_tournament.png";
        }break;
        case Battle_Tournament:
        {
            //エントリー済み、且つ、トーナメント対戦中状態
            info_image_path = "init/Home/Event/TournamentEvent/bk_tournament5.png";
            list_image_path = "init/Home/Event/TournamentEvent/bt_join_tournament.png";
        }break;
        case Close_Tournament:
        {
            //エントリー済み、且つ、トーナメント終了状態
            info_image_path = "init/Home/Event/TournamentEvent/bk_tournament5.png";
            list_image_path = "init/Home/Event/TournamentEvent/bt_tournament_list.png";
        }break;
        default: break;
    }
    
    //表示処理
    if (info_image_path != "" && list_image_path != "")
    {
        //トーナメント情報画像の作成
        createTournamentInfoSprite(info_image_path);
        
        //トーナメント一覧ボタン
        createTournamentButton(list_image_path);
        
        //ナビ画像の作成
        createNaviSprite(navi_comment);
        
        //ナビの目の瞬き
        schedule(schedule_selector(TournamentEventTopLayer::blinkEyes), 3.0f);
        
        //戦績、報酬一覧のボタン
        createWebViewButton();
    }
    else {
        
        //イベント未開催
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("QuestMenu");
        };
        
        string comment = "現在開催している\nイベントはございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
    }
}

void TournamentEventTopLayer::setCallbackTournamentData()
{
    //トーナメントデータの設定
    SaveData::saveString("tournament_event_list_id", _tournament_data_list["tournament_event_list_id"].string_value());
    
    if (_tournament_data_list["wait_start_date_timestamp"].string_value() != "0") {
        
        //現在時刻のタイムスタンプの取得
        time_t now_time;
        time(&now_time);
        time_t now_timestamp = now_time - (time_t)SaveData::loadInt("server_diff_time");
        time_t wait_start_date_timestamp = fromString<time_t>(_tournament_data_list["wait_start_date_timestamp"].string_value());
        SaveData::saveString("tournament_event_entry", "1");
        
        //現在時刻を過ぎていない場合
        if (now_timestamp < wait_start_date_timestamp) {
            
            NormalScene::_tournament_next_battle_time = wait_start_date_timestamp;
            
            //対戦開始までの画像の表示
            createNextBattleTimeSprite();
        }
    }
}

void TournamentEventTopLayer::blinkEyes(float dt)
{
    
    //貂蝉の目の瞬き
    auto message_bg = (Sprite *)this->getChildByTag(T_Navi_Sprite);
    if (message_bg) {
        
        auto close = CallFunc::create([message_bg](){
            message_bg->setTexture("init/Home/Event/TournamentEvent/message_navi2.png");
        });
        
        auto open = CallFunc::create([message_bg](){
            message_bg->setTexture("init/Home/Event/TournamentEvent/message_navi1.png");
        });
        
        runAction(Sequence::create(close, DelayTime::create(0.2f), open, nullptr));
    }
}

bool TournamentEventTopLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TournamentEventTopLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TournamentEventTopLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void TournamentEventTopLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
