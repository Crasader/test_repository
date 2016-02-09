#include "MypageLayer.h"

MypageLayer::MypageLayer()
:_multi_event_list(nullptr)
,_login_present_list(nullptr)
,_mypage_info(nullptr)
,_now_center_slide_banner_index(0)
,_tournament_data_list(nullptr)
,_popup_status_queue()
,_touch_disable(false)
{
}

bool MypageLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("MypageLayer::init()");
    
    ratio = getVisibleRatio();
    
    
    return true;
}

void MypageLayer::onEnter()
{
    Layer::onEnter();
    
    this->setName("MypageLayer") ;
    
    //イベントリスナーの作成
    createEventListener();
    
    //CardGalleryLayerの追加
    createCardGalleryLayer() ;
    
    //ボタンの作成
    createButton();
    
    //ビギナーボタンの表示
    showBeginnerButton();
    
    //イベントバナーの作成
    createEventBanner();
    
    //バナーの表示位置を示す点を表示
    if(_slide_banner.size()>=2)updateBannerDot() ;
    
    //バナーを初期位置、初期不可視にする
    initBannerPosition(SwipeDirection::LEFT) ;
    initBannerVisible(SwipeDirection::LEFT) ;
    
    //バナーが二つ以上ある場合はバナースワイプを定期的に行う(最初のバナーが表示されている時は6.0f後に切り替える)
    this->schedule(schedule_selector(MypageLayer::updateAutoSwipeBanner), 6.0f);
    
    //ログインボーナス・お知らせ・NEWマーク表示のチェックと表示処理
    mypageRequest();
}

void MypageLayer::updateBannerDot()
{
    if(_slide_banner.size()>0){
        
        //初期化
        if(_banner_dot.size()==0){
            float distance = 60 ;
            float index_0_x = DISPLAY_WIDTH/2 - (_slide_banner.size()-1) * distance / 2;
            for(int i=0; i<_slide_banner.size(); i++){
                //初期化
                Sprite *sprite = Sprite::create("init/Home/Common/circle_black.png") ;
                sprite->setPosition(convertFromDisplaySize(Point(index_0_x+distance*i, 110), 0.5, 0.5));
                sprite->setScale(ratio) ;
                this->addChild(sprite, 1) ;
                _banner_dot.push_back(sprite);
            }
        }
        
        //現在表示されているバナーの位置を赤く表示
        for(int i=0; i<_banner_dot.size(); i++){
            if(i==_now_center_slide_banner_index){
                _banner_dot[i]->setTexture("init/Home/Common/circle_red.png") ;
            }else{
                _banner_dot[i]->setTexture("init/Home/Common/circle_black.png") ;
            }
        }
    }
}

void MypageLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(MypageLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MypageLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MypageLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void MypageLayer::createCardGalleryLayer()
{
    //ギャラリーレイヤーの作成
    CardGalleryLayer *layer = CardGalleryLayer::create() ;
    addChild(layer, 1) ;
}

void MypageLayer::createButton()
{
    //チームトップ
    auto team_off = Sprite::create("init/Home/Mypage/bt_mypage_team1.png");
    auto team_on = Sprite::create("init/Home/Mypage/bt_mypage_team1.png");
    team_on->setColor(Color3B(180, 180, 180));
    
//    //textureを0.7秒おきに変更する
//    RepeatForever* repeat_texture_change_team_off = RepeatForever::create(Sequence::create(
//                           TargetedAction::create(team_off, ChangeTexture::create("init/Home/Mypage/bt_mypage_team2.png")),
//                           DelayTime::create(0.7),
//                           TargetedAction::create(team_off, ChangeTexture::create("init/Home/Mypage/bt_mypage_team1.png")),
//                           DelayTime::create(0.7),
//                           nullptr)) ;
    
    //textureを0.7秒おきに変更する
    RepeatForever* repeat_texture_change_team_off = RepeatForever::create(Sequence::create(
                           ChangeTexture::create("init/Home/Mypage/bt_mypage_team2.png"),
                           DelayTime::create(0.7),
                           ChangeTexture::create("init/Home/Mypage/bt_mypage_team1.png"),
                           DelayTime::create(0.7),
                           nullptr)) ;
    
    team_off->runAction(repeat_texture_change_team_off) ;
    
    
    auto team_btn = MenuItemSprite::create(team_off, team_on, [this](Ref* sender)
                                           {
                                               auto normal_scene = (NormalScene *)this->getParent();
                                               //ボスイベントへ
                                               if(SaveData::loadInt("team_id")>0) {
                                                   normal_scene->changePage("TeamTop");
                                               } else {
                                                   normal_scene->changePage("TeamNoBelong");
                                               }
                                           });
    team_btn->setScale(ratio*0.94);
    auto menu_team = Menu::create(team_btn, nullptr);
    menu_team->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*9/12, 750), 0.5, 1));
    this->addChild(menu_team, ZORDER_BUTTON);
    
    //メニューボタン
    auto menu_off = Sprite::create("init/Home/Mypage/bt_mypage_menu.png");
    auto menu_on = Sprite::create("init/Home/Mypage/bt_mypage_menu.png");
    menu_on->setColor(Color3B(180, 180, 180));
    
    auto menu_btn = MenuItemSprite::create(menu_off, menu_on, [this](Ref* sender)
                                           {
                                               auto normal_scene = (NormalScene *)this->getParent();
                                               normal_scene->changePage("SubMenu");
                                           });
    menu_btn->setScale(ratio*0.94);
    auto menu_menu = Menu::create(menu_btn, nullptr);
    menu_menu->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*11/12, 750), 0.5, 1));
    this->addChild(menu_menu, ZORDER_BUTTON);
    
    //お知らせボタン
    auto info_off = Sprite::create("init/Home/Mypage/bt_mypage_base.png");
    Sprite* info_part1 = Sprite::create("init/Home/Mypage/bt_mypage_icon5.png");
    info_part1->setAnchorPoint(Point(0,0));
    info_part1->setPosition(Point(0,0));
    info_off->addChild(info_part1);
    
    Sprite* info_part2 = Sprite::create("init/Home/Mypage/bt_mypage_icon6.png");
    info_part2->setAnchorPoint(Point(0.5,0.5));
    info_part2->setPosition(Point(info_part2->getContentSize() / 2));
    info_off->addChild(info_part2);
    
    //アクション
    Sequence* info_seq1 = Sequence::create(ScaleTo::create(1.0f, 1.1f), ScaleTo::create(1.0f, 1.0f), nullptr);
    Sequence* info_seq2 = Sequence::create(MoveTo::create(1.0f, Vec2(info_part2->getContentSize().width / 2 + 5, info_part2->getContentSize().height / 2 + 10)), MoveTo::create(1.0f, Vec2(info_part2->getContentSize() / 2)), nullptr);
    Spawn* sum_seq = Spawn::create(info_seq1, info_seq2, nullptr);
    RepeatForever* info_repeart_foever = RepeatForever::create(sum_seq);
    info_part2->runAction(info_repeart_foever);
    
    Sprite* info_part3 = Sprite::create("init/Home/Mypage/bt_mypage_name3.png");
    info_part3->setAnchorPoint(Point(0,0));
    info_part3->setPosition(Point(0,0));
    info_off->addChild(info_part3);
    
    auto info_on  = Sprite::create("init/Home/Mypage/bt_mypage_announce.png");
    info_on->setColor(Color3B(180, 180, 180));
    
    auto info_btn = MenuItemSprite::create(info_off, info_on, [this](Ref* sender)
                                           {
                                               this->removeChildByTag(T_INFO_LABEL_TAG);
                                               this->removeChildByTag(T_RED_CIRCLE_INFO_TAG);
                                               SaveData::saveInt("unread_info_count", 0);
                                               
                                               auto normal_scene = (NormalScene *)this->getParent();
                                               string url = HOST;
                                               url += "user/index.php?p=page__info&user_master_id=" + SaveData::loadString("user_master_id");
                                               auto webview_layer = WebViewLoadLayer::create(url, "お知らせ");
                                               normal_scene->addChild(webview_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
                                           });
    info_btn->setScale(ratio*0.94);
    auto menu_info = Menu::create(info_btn, nullptr);
    menu_info->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*1/12, 750), 0.5, 1));
    this->addChild(menu_info, ZORDER_BUTTON);
    
    //プレゼントボタン
    auto present_off = Sprite::create("init/Home/Mypage/bt_mypage_base.png");
    
    Sprite* present_part1 = Sprite::create("init/Home/Mypage/bt_mypage_icon1.png");
    present_part1->setAnchorPoint(Point(0.5,0));
    present_part1->setPosition(Point(present_off->getContentSize().width / 2, 0));
    present_off->addChild(present_part1);
    
    //プレゼントが存在する場合、揺れ動かす。
    int present_count = SaveData::loadInt("present_count");
    if (present_count > 0) {
        //横の動き※ミミック風
        //		RotateBy* rotate1 = RotateBy::create(0.07f, 10);
        //		RotateTo* rotate2 = RotateTo::create(0.07f, 350);
        //		RotateBy* rotate3 = RotateBy::create(0.07f, 20);
        //		Sequence* present_seq = Sequence::create(DelayTime::create(2.0f), rotate1, rotate2, rotate3, rotate2, rotate1, nullptr);
        //		RepeatForever* present_repeart_foever = RepeatForever::create(present_seq);
        //		present_part1->runAction(present_repeart_foever);
        
        //縦の動き※飛び跳ねてる
        JumpBy* jump = JumpBy::create(0.5f, Point(0, 0), 20.0f, 1);
        Sequence* seq1 = Sequence::create(DelayTime::create(0.5f), ScaleTo::create(0.05f, 1.0f, 0.9f), ScaleTo::create(0.05f, 1.0f, 1.0f), nullptr);
        Spawn* spawn = Spawn::create(DelayTime::create(1.0f), jump, seq1, nullptr);
        RepeatForever* present_repeart_foever = RepeatForever::create(spawn);
        present_part1->runAction(present_repeart_foever);
    }
    
    Sprite* present_part2 = Sprite::create("init/Home/Mypage/bt_mypage_name1.png");
    present_part2->setAnchorPoint(Point(0,0));
    present_part2->setPosition(Point(0,0));
    present_off->addChild(present_part2);
    
    auto present_on  = Sprite::create("init/Home/Mypage/bt_mypage_present.png");
    present_on->setColor(Color3B(180, 180, 180));
    auto present_btn = MenuItemSprite::create(present_off, present_on, [this](Ref* sender)
                                              {
                                                  auto normal_scene = (NormalScene *)this->getParent();
                                                  normal_scene->changePage("PresentMenu");
                                              });
    present_btn->setScale(ratio*0.94);
    auto menu_present = Menu::create(present_btn, nullptr);
    menu_present->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*3/12, 750), 0.5, 1));
    this->addChild(menu_present, ZORDER_BUTTON);
    
    
    //デイリーミッションボタン
    auto daily_mission_off = Sprite::create("init/Home/DailyMission/bt_mypage_shoukyou1.png");
    auto daily_mission_on = Sprite::create("init/Home/DailyMission/bt_mypage_shoukyou1.png");
    daily_mission_on->setColor(Color3B(180, 180, 180));

    //textureを0.7秒おきに変更する
    RepeatForever* repeat_texture_change_daily_mission_off = RepeatForever::create(Sequence::create(
                           ChangeTexture::create("init/Home/DailyMission/bt_mypage_shoukyou2.png"),
                           DelayTime::create(0.7),
                           ChangeTexture::create("init/Home/DailyMission/bt_mypage_shoukyou1.png"),
                           DelayTime::create(0.7),
                           nullptr)) ;

    daily_mission_off->runAction(repeat_texture_change_daily_mission_off) ;


    auto daily_mission_btn = MenuItemSprite::create(daily_mission_off, daily_mission_on, [this](Ref* sender)
                                           {
                                                    auto normal_scene = (NormalScene *)this->getParent();
                                               
                                                   //デイリーミッションを追加
                                                   auto layer = DailyMissionLayer::create();
                                                   normal_scene->addChild(layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
                                           });
    daily_mission_btn->setScale(ratio*0.94);
    auto menu_daily_mission = Menu::create(daily_mission_btn, nullptr);
    menu_daily_mission->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*7/12, 750), 0.5, 1));
    this->addChild(menu_daily_mission, ZORDER_BUTTON);
}

void MypageLayer::mypageRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    string password = SaveData::loadString("password");
    
    //ログインボーナスリクエスト
    string url = HOST;
    url += "user/mypage.php";
    //auto selector = httpresponse_selector(MypageLayer::mypageCallback);
    string body = "user_master_id=" + user_master_id + "&password=" + password + "&resource_version=" + SaveData::loadString("resource_version");
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        mypageCallback(sender, response);
    }, body);
    
}

void MypageLayer::mypageCallback(HttpClient *sender, HttpResponse *response)
{
    //ログインボーナスレスポンス
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("%s",result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    //ダウンロードする必要なデータがあった時タイトルに戻す処理
    if(json["needs_download"].bool_value()){
        auto normal_scene = (NormalScene *)this->getParent();
        auto func = [](){
            //タイトルへ
            auto scene = TitleScene::createScene();
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        };
        //ボップアップ表示
        string message = json["message"].string_value() ;
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        normal_scene->addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
        return ;
    }

    //Jsonデータの取得
    _comeback_present_data_list = json["comeback_present_data_list"].object_items();
    _login_present_list = json["present_list"].object_items();
    _mypage_info        = json["mypage_info"].object_items();
    _tournament_data_list = json["tournament_data_list"].object_items();
    int unread_info_count  = json["unread_info_count"].int_value();
    int boss_count = json["boss_count"].int_value();
    
    //出現中ボス数の表示
    //TODO 今後チームイベントは匈奴討伐イベント(boss_event)以外にもイベントを行うかもしれないので、チームのアイコンの上に数だけを表示するのは適さない
    if(boss_count > 0) showRedcircleCount(boss_count, convertFromDisplaySize(Point(465, 792), 0.5, 1)) ;
    
    //未読お知らせの表示
    if (unread_info_count > 0) showRedcircleCount(unread_info_count, convertFromDisplaySize(Point(37, 792), 0.5, 1));
    
    
    //未読プレゼント数の表示
    int present_count = SaveData::loadInt("present_count");
    if (present_count > 0) showRedcircleCount(present_count, convertFromDisplaySize(Point(144, 792), 0.5, 1));
    
    //出陣とガチャのNew表示
    showNewSprite(json);

    //挑戦者ボタンの表示
    showChallengerButton(json);
    
    //初心者パネル処理
    if (SaveData::loadString("beginner_guide_complete_flg") == "N") {
        
        //ユーザーレベルチェック
        checkUserLevel(json);

        //ビギナーコメントの表示
        showBeginnerComment();

    }
    
    //トーナメント次戦残り時間の表示
    if (fromString<int>(_tournament_data_list["wait_start_date_timestamp"].string_value()) > 0){
        showTournamentNextTimeLabel();
        SaveData::saveString("tournament_event_entry", "1");
        SaveData::saveString("tournament_event_list_id", _tournament_data_list["tournament_event_list_id"].string_value());
        SaveData::saveString("tournament_event_deck_no", _tournament_data_list["tournament_event_deck_no"].string_value());
        HMSLOG("######id?%d:%d", fromString<int>(_tournament_data_list["tournament_event_list_id"].string_value()), fromString<int>(_tournament_data_list["tournament_event_deck_no"].string_value()));

    }
    //不参加状態だったらエントリーフラグをリセット
    else {
        SaveData::saveString("tournament_event_entry", "0");
    }
    
    //daily_mission_check_trigerがtureの時だけ条件判定のチェックを行う
    if(SaveData::loadBool("daily_mission_check_triger")){
        SaveData::saveBool("daily_mission_check_triger", false);
        dailyMissionCheckRequest() ;
    }else{
        //ポップアップキューの作成
        createPopupStatusQueue(false);
        
        //ポップアップの表示
        showNextPopupStatus();
    }
        
}

void MypageLayer::dailyMissionCheckRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    //ログインボーナスリクエスト
    string url = HOST;
    url += "user/daily_mission_check.php";
    auto selector = httpresponse_selector(MypageLayer::dailyMissionCheckCallback);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
    
}

void MypageLayer::dailyMissionCheckCallback(HttpClient *sender, HttpResponse *response)
{
    //ログインボーナスレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("%s",result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    //デイリーミッションの状況のダンプを保存
    SaveData::saveString("daily_mission_group_data_list", json["daily_mission_group_data_list"].dump()) ;
    bool display_daily_mission_animation = json["display_daily_mission_animation"].bool_value() ;
    
    //新規でクリアしていたrアニメーション表示フラグを立てる
    SaveData::saveBool("display_daily_mission_animation", display_daily_mission_animation) ;
    
    //ポップアップキューの作成
    createPopupStatusQueue(display_daily_mission_animation);

    //ポップアップの表示
    showNextPopupStatus();
}

void MypageLayer::createPopupStatusQueue(bool display_daily_mission_animation)
{
    //ポップアップキューの作成（カムバックボーナス→ログインボーナス→マイページお知らせ→ビギナー→レビュー）
    if (countJsonData(_comeback_present_data_list) > 0) _popup_status_queue.push(PopupStatus::ComebackBonus);
    if (countJsonData(_login_present_list) > 0) _popup_status_queue.push(PopupStatus::LoginBouns);
    if (countJsonData(_mypage_info) > 0 ) _popup_status_queue.push(PopupStatus::MypageInfo);
    if (SaveData::loadBool("beginner_layer_open_flg")) {
        _popup_status_queue.push(PopupStatus::Beginner);
        SaveData::saveBool("beginner_layer_open_flg", false);
    }
    if (display_daily_mission_animation) _popup_status_queue.push(PopupStatus::DailyMission);
    if (SaveData::loadBool("review_demand")) _popup_status_queue.push(PopupStatus::Review);
}

void MypageLayer::showNextPopupStatus()
{
    //キューを取り出し、ポップアップ表示処理
    if (_popup_status_queue.size() > 0) {
        
        PopupStatus status = _popup_status_queue.front();
        
        auto normal_scene = (NormalScene *)this->getParent();
        switch (status) {
            case ComebackBonus:
            {
                //カムバックキャンペーン
                auto comeback_layer = ComebackCampaignLayer::create(_comeback_present_data_list);
                normal_scene->addChild(comeback_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
                
                //プレゼントの数の更新
                int count = countJsonData(_comeback_present_data_list);
                updatePresentCount(count);
                
            } break;
            case LoginBouns:
            {
                //ログインボーナスポップアップの表示
                auto login_bouns_popup = LoginBounsPopupLayer::create(_login_present_list);
                normal_scene->addChild(login_bouns_popup, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
                
                //プレゼントの数の更新
                int count = countJsonData(_login_present_list);
                updatePresentCount(count);
                
            } break;
            case MypageInfo:
            {
                //マイページお知らせレイヤー（新規ガチャ画像など）の表示
                string image_path = _mypage_info["0"]["image_path"].string_value();
                if (image_path != "") {
                    auto layer = MypageInfoLayer::create(image_path);
                    normal_scene->addChild(layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
                }
            } break;
            case Beginner:
            {
                //ビギナーレイヤーの表示
                checkOpenCheckBeginnerGuideLayer();
            } break;
            case DailyMission:
            {
                //デイリーミッションの表示
                checkOpenCheckDailyMissionLayer();
            } break;
            case Review:
            {
                //レビューの表示
                afterLoginBonusCheck();
            } break;
            default:break;
        }
        
        _popup_status_queue.pop(); //先頭を削除
    }
}

void MypageLayer::afterLoginBonusCheck()
{
    //androidのみレビューの処理
    auto normal_scene = (NormalScene *)this->getParent();
    
    //レビューをしてもらうウィンドウを表示
    auto layer = GooglePlayReviewLayer::create();
    normal_scene->addChild(layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
}

void MypageLayer::showRedcircleCount(int count, Point p)
{
    auto bg_red = Sprite::create("init/Home/Mypage/bt_mypage_red_circle.png");
    bg_red->setScale(ratio);
    bg_red->setPosition(p);
    this->addChild(bg_red, ZORDER_BUTTON + 1, T_RED_CIRCLE_INFO_TAG);
    
    string str_count = toString<int>(count) ;
    auto num_label = HMSFontUtil::createWithTTFBySystemFontArgument(str_count, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(str_count, 24, 50));
    num_label->setPosition(Point(bg_red->getContentSize() / 2));
    bg_red->addChild(num_label, ZORDER_BUTTON + 2, T_INFO_LABEL_TAG);
}

void MypageLayer::createEventBanner()
{
    string err;
    _multi_event_list = Json::parse(SaveData::loadString("multi_event_list"), err);
    
    //イベントの数を取得
    int event_count = countJsonData(_multi_event_list);
    
    for(int i=0 ; i < event_count ; i++){
        string str_index = toString<int>(i) ;
        int tag = eventTypeToBannerTag(_multi_event_list[str_index]["event_type"].string_value()) ;
        if(tag == -1){
            HMSLOG("unknown event_type") ;
            continue ;
        }
        string image_path = _multi_event_list[str_index]["image_path"].string_value() ;
        string end_date = _multi_event_list[str_index]["end_date"].string_value() ;
        
        if(checkEndDate(end_date)){
            //バナーのスプライトを作成
            BannerSprite *banner_sprite = BannerSprite::create(image_path) ;
            banner_sprite->setScale(ratio) ;
            banner_sprite->setTag(tag) ;
            this->addChild(banner_sprite, 1) ;
            _slide_banner.push_back(banner_sprite);
            
            //バナー固有の値を設定
            if(tag == T_MUTUAL_LINK_BANNER){
                banner_sprite->setMutualLinkUrl(_multi_event_list[str_index]["url"].string_value()) ;
            }else if(tag == T_CHALLENGE_EVENT_BANNER){
                banner_sprite->addDisplayEndDate(end_date) ;
            }else if(tag == T_CHANGE_PAGE_BANNER){
                banner_sprite->setChangePageName(_multi_event_list[str_index]["change_page_name"].string_value()) ;
            }
        }
    }
}

bool MypageLayer::checkEndDate(string end_date){
    if(end_date=="") return true ; //end_dateの値が無い場合は通す
    
    //終了時間を取得
    struct tm tm;
    strptime(end_date.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
    time_t _end_time = mktime(&tm);
    
    time_t now_time;
    time(&now_time);
    
    //端末時刻とサーバー時間の差を加える
    now_time = now_time - SaveData::loadInt("server_diff_time");
    
    //終了までの残り時間
    time_t remain_time = _end_time - now_time;
    
    if(remain_time < 0){
        return false ;
    }else{
        return true ;
    }
}


int MypageLayer::eventTypeToBannerTag(string event_type)
{
    int tag = -1;
    if(event_type == "npc_battle_event"){
        tag = T_NPC_BATTLE_EVENT_BANNER ;
    }else if(event_type == "ranking_event"){
        tag = T_RANKING_EVENT_BANNER ;
    }else if(event_type == "boss_event"){
        tag = T_BOSS_EVENT_BANNER ;
    }else if(event_type == "challenge_event") {
        tag = T_CHALLENGE_EVENT_BANNER ;
    }else if(event_type == "tournament_event") {
        tag = T_TOURNAMENT_EVENT_BANNER ;
    }else if(event_type == "mutual_link") {
        tag = T_MUTUAL_LINK_BANNER ;
    }else if(event_type == "change_page_banner") {
        tag = T_CHANGE_PAGE_BANNER ;
    }

    if(tag == -1){
        HMSLOG("MypageLayer::eventTypeToBannerTag() event_typeからタグが取得出来ない event_type:%s", event_type.c_str()) ;
    }
    
    return tag ;
}

void MypageLayer::initBannerVisible(SwipeDirection swipe_direction){
    
    //全てのバナーを不可視にする
    
    for_each(_slide_banner.begin(), _slide_banner.end(), [](BannerSprite *banner){
        banner->setVisible(false) ;
    }) ;
             
    if(_slide_banner.size()>0){
        auto center_banner = _slide_banner.at(_now_center_slide_banner_index);
        center_banner->setVisible(true);
    }
    
    if(_slide_banner.size()>=2){
        if(SwipeDirection::LEFT == swipe_direction){
            int right_banner_index = calcRightBannerTagIndex(_now_center_slide_banner_index) ;
            auto right_banner = _slide_banner.at(right_banner_index) ;
            right_banner->setVisible(true);
        }
        
        if(SwipeDirection::RIGHT == swipe_direction){
            int left_banner_index = calcLeftBannerTagIndex(_now_center_slide_banner_index) ;
            auto left_banner = _slide_banner.at(left_banner_index) ;
            left_banner->setVisible(true);
        }
    }
    
}

void MypageLayer::initBannerPosition(SwipeDirection swipe_direction)
{
    //バナーの位置調整
    
    //全て見えない位置にバナーを移動する
    for_each(_slide_banner.begin(), _slide_banner.end(), [](BannerSprite *banner){
        Point out_point = convertFromDisplaySize(Point(DISPLAY_WIDTH/2 + DISPLAY_WIDTH*5/2, 190), 0.5, 0.5);
        banner->setPosition(out_point) ;
    }) ;
    
    
    //中心の位置に合わせる
    if(_slide_banner.size()>0){
        Point center_point = convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 190), 0.5, 0.5);
        auto center_banner = _slide_banner.at(_now_center_slide_banner_index);
        center_banner->setPosition(center_point) ;
    }
    
    if(_slide_banner.size()>=2){
        //右のバナー
        if(SwipeDirection::LEFT == swipe_direction){
            Point right_point = convertFromDisplaySize(Point(DISPLAY_WIDTH/2 + DISPLAY_WIDTH, 190), 0.5, 0.5);
            int right_banner_index = calcRightBannerTagIndex(_now_center_slide_banner_index) ;
            auto right_banner = _slide_banner.at(right_banner_index) ;
            right_banner->setPosition(right_point) ;
        }
        
        //左のバナー
        if(SwipeDirection::RIGHT == swipe_direction){
            Point left_point = convertFromDisplaySize(Point(DISPLAY_WIDTH/2 - DISPLAY_WIDTH, 190), 0.5, 0.5);
            int left_banner_index = calcLeftBannerTagIndex(_now_center_slide_banner_index) ;
            auto left_banner = _slide_banner.at(left_banner_index) ;
            left_banner->setPosition(left_point) ;
        }
    }
    
}

bool MypageLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    if(_slide_banner.size()>=2){
        if(_touch_disable)return false ;
        
        if(containBanner(touch->getLocation())){
            this->unschedule(schedule_selector(MypageLayer::updateAutoSwipeBanner));
            return true ;
        }else{
            return false ;
        }
    }
    
    return true ;
}

void MypageLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    if(_slide_banner.size()>=2){
        float prev_diff_x = touch->getPreviousLocation().x - touch->getStartLocation().x ;
        float diff_x = touch->getLocation().x - touch->getStartLocation().x ;
        
        //直前の初期位置との差のx成分と、今回の初期位置との差のx成分の、符号の向きが変わる時。ないしは、最初のonTouchMovedが呼ばれた時 (prev_diff_x==0)
        //バナーの位置・可視不可視を調整する
        if((prev_diff_x>0 && diff_x<0) || (prev_diff_x<0 && diff_x>0) || prev_diff_x == 0){
            if(diff_x<0) {
                initBannerPosition(SwipeDirection::LEFT) ;
                initBannerVisible(SwipeDirection::LEFT) ;
            } else {
                initBannerPosition(SwipeDirection::RIGHT) ;
                initBannerVisible(SwipeDirection::RIGHT) ;
            }
        }
        
        if((touch->getStartLocation()-touch->getLocation()).length() > 10){
            _touch_disable = true;
            for_each(_slide_banner.begin(), _slide_banner.end(), [&touch](BannerSprite *banner){
                banner->setPosition(Point(banner->getPosition().x + touch->getDelta().x, banner->getPosition().y)) ;
            }) ;
        }
    }
}

void MypageLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if(_slide_banner.size()>=2){
        if(_touch_disable) {
            float diff = touch->getLocation().x - touch->getStartLocation().x ;
            if( diff < 0 ){
                //左にスワイプ
                swipeBanner(SwipeDirection::LEFT) ;
            }else if( 0 <= diff ){
                //右にスワイプ
                swipeBanner(SwipeDirection::RIGHT) ;
            }
            
            return;
        }
    }
    
    //イベントバナーの取得
    
    for_each(_slide_banner.begin(), _slide_banner.end(), [&touch, this](BannerSprite *banner){
        int tag = banner->getTag() ;
        Rect rect = banner->getBoundingBox();
        if (rect.containsPoint(touch->getLocation())) {
            if (tag == T_NPC_BATTLE_EVENT_BANNER){
                //イベントメニューページへ
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("EventMenu");
            } else if (tag == T_RANKING_EVENT_BANNER){
                //イベントメニューページへ
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("RankingEventTop");
            } else if (tag == T_BOSS_EVENT_BANNER){
                //イベントメニューページへ
                auto normal_scene = (NormalScene *)this->getParent();
                //ボスイベントへ
                if(SaveData::loadInt("team_id")>0) {
                    normal_scene->changePage("TeamTop");
                } else {
                    normal_scene->changePage("TeamNoBelong");
                }
            } else if (tag == T_CHALLENGE_EVENT_BANNER){
                //イベントメニューページへ
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("ChallengeEventMenu");
            } else if (tag == T_TOURNAMENT_EVENT_BANNER){
                //イベントメニューページへ
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("TournamentEventTop");
            } else if (tag == T_MUTUAL_LINK_BANNER){
                //相互リンクURLをブラウザで開く
                string mutual_link_url = banner->getMutualLinkUrl() ;
                NativeBridge::openURL(mutual_link_url.c_str());
            } else if(tag == T_GACHA_BANNER){
                //最初にプレミアムガチャを表示させるタグの設定
                SaveData::saveInt("gacha_tab", 2);
                
                //ページ遷移
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("GachaMenu");
            } else if (tag == T_BBS_BANNER){
                //掲示板へ
                string url = HOST;
                url += "user/index.php?&p=page__board_top&user_id=" + SaveData::loadString("user_master_id");
                auto WebViewLoadLayer = WebViewLoadLayer::create(url, "掲示板");
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->addChild(WebViewLoadLayer, 9);
            } else if (tag == T_CHANGE_PAGE_BANNER){
                //change_scene_nameに応じて遷移先を切り替える
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage(banner->getChangePageName());
            }
    }}) ;
}

void MypageLayer::showBeginnerButton()
{
    if (SaveData::loadString("beginner_guide_complete_flg") == "N")
    {
        //大喬の館 表示ボタン
        auto beginner_off = Sprite::create("init/Home/Beginner/bt_mypage_beginner1.png");
        auto beginner_on  = Sprite::create("init/Home/Beginner/bt_mypage_beginner1.png");
        beginner_on->setColor(Color3B(180, 180, 180));
        
        auto beginner_btn = MenuItemSprite::create(beginner_off, beginner_on, [this](Ref* sender)
                                                   {
                                                       auto normal_scene = (NormalScene *)this->getParent();
                                                       
                                                       //初心の手引き表示
                                                       auto layer = BeginnerGuideLayer::create();
                                                       normal_scene->addChild(layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
                                                   });
        beginner_btn->setScale(ratio*0.94);
        beginner_btn->setTag(T_Beginner_MenuItem);
        
        auto menu = Menu::create(beginner_btn, nullptr);
        menu->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*5/12, 750), 0.5, 1));
        this->addChild(menu, Z_Beginner_Menu, T_Beginner_Menu);
        
        //ビギナーボタンの瞬き
        this->schedule(schedule_selector(MypageLayer::blinkingEyes), 3.0f);
    }
    else
    {
        //通常、ランキングボタン
        auto ranking_off = Sprite::create("init/Home/Mypage/bt_mypage_ranking.png");
        
        auto ranking_on = Sprite::create("init/Home/Mypage/bt_mypage_ranking.png");
        ranking_on->setColor(Color3B(180, 180, 180));
        auto ranking_btn = MenuItemSprite::create(ranking_off, ranking_on, [this](Ref* sender)
                                                  {
                                                      
                                                      auto normal_scene = (NormalScene *)this->getParent();
                                                      string url = HOST;
                                                      url += "webview/ranking.php?type=0";
                                                      url += "&user_master_id=" + SaveData::loadString("user_master_id");
                                                      auto webview_layer = WebViewLoadLayer::create(url, "ランキング");
                                                      normal_scene->addChild(webview_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
                                                  });
        ranking_btn->setScale(ratio*0.94);
        auto menu = Menu::create(ranking_btn, nullptr);
        menu->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*5/12, 750), 0.5, 1));
        this->addChild(menu, ZORDER_BUTTON);
        
        //パーティクル出力
        ParticleSystemQuad* rank_particle = ParticleSystemQuad::create("download/Effect/Particle/rank_kirakira.plist");
        rank_particle->setAutoRemoveOnFinish(true);
        rank_particle->setPosition(ranking_btn->getContentSize()/2) ;
        ranking_btn->addChild(rank_particle, 100);
        
    }
}

void MypageLayer::showBeginnerComment()
{
    int beginner_guide_id = 0;
    int clear_count = 0;
    for (int i = 0; i < 12; i++)
    {
        string flg_name = "beginner_guide_clear_flg_" + toString<int>(i + 1);
        if (SaveData::loadBool(flg_name.c_str()) == false)
        {
            //クリアフラグを1から見ていき、最初にまだ未達成のidを取得し表示する
            beginner_guide_id = i;
            break;
        }
        clear_count++;
    }
    
    //全てクリアしている場合は表示しない
    if (clear_count < 12)
    {
        string json_text = SaveData::loadString("beginner_guide_list");
        string err;
        Json json = Json::parse(json_text, err);
        
        string beginner_comment = json[toString<int>(beginner_guide_id)]["comment"].string_value();
        HMSLOG("beginner_guide_id = %d, beginner_comment = %s", beginner_guide_id, beginner_comment.c_str());
        
        //初心の手引きの表示
        auto sp = Sprite::create("init/Home/Beginner/beginner_balloon.png");
        sp->setScale(ratio);
        sp->setAnchorPoint(Point(0.411, 1.0)); //吹き出しの先端をアンカーポイントにする
        sp->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH*5/12, 684), 0.5, 1));
        addChild(sp, Z_Beginner_Ballon, T_Beginner_Ballon);
        
        auto beginner_comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(beginner_comment , FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(beginner_comment, 30, sp->getContentSize().width - 20));
        beginner_comment_label->setAlignment(TextHAlignment::CENTER);
        beginner_comment_label->setPosition(Point(sp->getContentSize().width/2, 25));
        sp->addChild(beginner_comment_label);
        
        //浮遊アクション
        auto move = MoveBy::create(2.0f, Point(0, -20));
        auto ease = EaseSineInOut::create(move);
        
        auto move2 = MoveBy::create(2.0f, Point(0, 20));
        auto ease2 = EaseSineInOut::create(move2);
        
        auto scale = ScaleTo::create(0.3f, 0);
        
        auto repeat = Repeat::create(Sequence::create(ease, ease2, nullptr), 2);
        sp->runAction(Sequence::create(repeat, scale, RemoveSelf::create(true), nullptr));
    }
}

void MypageLayer::checkOpenCheckBeginnerGuideLayer()
{
    //ディレイタイムの作成
    float delayTime = 0.0;
    auto normal_scene = (NormalScene *)this->getParent();
    
    //トランジションでページ遷移したときタイミングがずれる為、ページ遷移0回をシーンチェンジとみなし調整
//    if (normal_scene->getChangePageCount() == 0) delayTime = 3.0f;
    
    auto func = CallFunc::create([this, normal_scene](){
        
        auto layer = BeginnerGuideLayer::create();
        normal_scene->addChild(layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    });
    
    runAction(Sequence::create(DelayTime::create(delayTime), func, nullptr));
}

void MypageLayer::checkOpenCheckDailyMissionLayer()
{
    //ディレイタイムの作成
    float delayTime = 0.0;
    auto normal_scene = (NormalScene *)this->getParent();
    
    //トランジションでページ遷移したときタイミングがずれる為、ページ遷移0回をシーンチェンジとみなし調整
//    if (normal_scene->getChangePageCount() == 0) delayTime = 3.0f;
    
    auto func = CallFunc::create([this, normal_scene](){
        auto layer = DailyMissionLayer::create();
        normal_scene->addChild(layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    });
    
    runAction(Sequence::create(DelayTime::create(delayTime), func, nullptr));
}

void MypageLayer::checkUserLevel(Json json)
{
//    int level = fromString<int>(SaveData::loadString("level"));
    
    if(json["beginner_guide_data_list"]["beginner_guide_clear_flg_4"].bool_value()){
        //初心の手引きクリアーフラグ4 レベルを3まで上げる
        if (SaveData::loadBool("beginner_guide_clear_flg_4") == false) {
            SaveData::saveBool("beginner_layer_open_flg", true);
            SaveData::saveBool("beginner_guide_clear_flg_4", true);
        }
    }
    
    if(json["beginner_guide_data_list"]["beginner_guide_clear_flg_5"].bool_value()){
        //初心の手引きクリアーフラグ5 レベルを8まで上げる
        if (SaveData::loadBool("beginner_guide_clear_flg_5") == false) {
            SaveData::saveBool("beginner_guide_clear_flg_5", true);
            SaveData::saveBool("beginner_layer_open_flg", true);
        }
    }
    
    if(json["beginner_guide_data_list"]["beginner_guide_clear_flg_6"].bool_value()){
        //初心の手引きクリアーフラグ6レベルを13まで上げる
        if (SaveData::loadBool("beginner_guide_clear_flg_6") == false) {
            SaveData::saveBool("beginner_guide_clear_flg_6", true);
            SaveData::saveBool("beginner_layer_open_flg", true);
        }
    }
}

void MypageLayer::blinkingEyes(float dt)
{
    auto menuItem = (MenuItemSprite *)this->getChildByTag(T_Beginner_Menu)->getChildByTag(T_Beginner_MenuItem);
    if (menuItem)
    {
        auto sp = (Sprite *)menuItem->getNormalImage();
        if (sp)
        {
            sp->runAction(Sequence::create(
               ChangeTexture::create("init/Home/Beginner/bt_mypage_beginner2.png"),
               DelayTime::create(0.2f),
               ChangeTexture::create("init/Home/Beginner/bt_mypage_beginner1.png")
               , nullptr));
        }
    }
}

void MypageLayer::showChallengerButton(Json json)
{
    //挑戦者ボタンを表示させるか判定
    if (json["challenger_exists"].bool_value())
    {
        auto footer_layer = this->getParent()->getChildByTag(FOOTER_LAYER_TAG);
        
        if (footer_layer)
        {
            auto challenger_sp = footer_layer->getChildByTag(T_Challenger);
            if (!(challenger_sp))
            {
                //挑戦者現るボタンの作成
                auto challenger_sp = Sprite::create("init/Footer/mypage_challenger.png");
                challenger_sp->setScale(0);
                challenger_sp->setAnchorPoint(Point(0.5, 0));
                challenger_sp->setPosition(Point(convertFromDisplaySize(Point(162, 64), 0.5, 0)));
                footer_layer->addChild(challenger_sp, 10, CHALLENGER_BUTTON_TAG);
                
                auto scale2 = ScaleTo::create(0.8f, 1.0f * ratio);
                auto scale3 = ScaleTo::create(0.8f, 0.8f * ratio);
                
                auto repeat = Repeat::create(Sequence::create(scale2, scale3, nullptr), -1);
                challenger_sp->runAction(Sequence::create(repeat, nullptr));
            }
        }
    }
}

void MypageLayer::updatePresentCount(int add_count)
{
    //未受け取りプレゼント数の更新
    SaveData::saveInt("present_count", SaveData::loadInt("present_count") + add_count);
    int new_present_count = SaveData::loadInt("present_count");
    
    auto red_circle = (Sprite *)this->getChildByTag(T_RED_CIRCLE_TAG);
    if (red_circle)
    {
        //未受け取り件数が既に表示されていた場合、数字のみ変更
        auto present_num_label = (Label *)red_circle->getChildByTag(T_PRESENT_LABEL_TAG);
        if (present_num_label) {
            string str_count = toString<int>(new_present_count) ;
            present_num_label->setString(str_count);
            present_num_label->setSystemFontSize(Utility::calcFlexibleFontSize(str_count, 24, 50));
        }
    }
    else {
        //未読のプレゼント数を表示
        if (new_present_count > 0)
        {
            showRedcircleCount(new_present_count, convertFromDisplaySize(Point(144, 792), 0.5, 1));
        }
    }
}

void MypageLayer::showNewSprite(Json json)
{
    //NEW表示のチェック
    auto normal_scene = (NormalScene *)this->getParent();
    auto footer_layer = (FooterLayer *)normal_scene->getChildByTag(FOOTER_LAYER_TAG);
    if(json["ui_notice_gacha"].bool_value()) { //ガチャ
        SaveData::saveBool("new_img_gacha", true);
        if(!footer_layer->new_img_gacha)footer_layer->addNewImg(3);
    } else if(SaveData::loadBool("new_img_gacha")) {
        if(!footer_layer->new_img_gacha)footer_layer->addNewImg(3);
    }
    if(json["ui_notice_quest"].bool_value() || json["ui_notice_event"].bool_value()){ //クエスト
        if(!footer_layer->new_img_quest){
            SaveData::saveBool("new_img_quest", true);
            footer_layer->addNewImg(1);
        }
        if(json["ui_notice_quest"].bool_value()) SaveData::saveBool("ui_notice_quest", true);
        if(json["ui_notice_event"].bool_value()) SaveData::saveBool("ui_notice_event", true);
    } else if(SaveData::loadBool("new_img_quest")) {
        if(!footer_layer->new_img_quest)footer_layer->addNewImg(1);
    }
}

void MypageLayer::updateAutoSwipeBanner(float dt)
{
    _touch_disable = true;
    this->unschedule(schedule_selector(MypageLayer::updateAutoSwipeBanner));
    initBannerPosition(SwipeDirection::LEFT) ;
    initBannerVisible(SwipeDirection::LEFT) ;
    swipeBanner(SwipeDirection::LEFT) ;
}

void MypageLayer::swipeBanner(SwipeDirection swipe_direction) {
    if(_slide_banner.size()<=1) return ;
    
    //移動時間
    float auto_swipe_time = 0.3f ;
    
    //バナーの位置
    Point center_point = convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 190), 0.5, 0.5);
    Point left_point = convertFromDisplaySize(Point(DISPLAY_WIDTH/2 - DISPLAY_WIDTH, 190), 0.5, 0.5);
    Point right_point = convertFromDisplaySize(Point(DISPLAY_WIDTH/2 + DISPLAY_WIDTH, 190), 0.5, 0.5);
    
    int behind_banner_index ;
    int front_banner_index ;
    Point front_point ;
    Point behind_point ;
    
    if(swipe_direction == SwipeDirection::LEFT){
        behind_point = right_point ;
        front_point = left_point ;
    }else if(swipe_direction == SwipeDirection::RIGHT){
        behind_point = left_point ;
        front_point = right_point;
    }
    
    if(swipe_direction == SwipeDirection::LEFT){
        behind_banner_index = calcRightBannerTagIndex(_now_center_slide_banner_index) ;
        front_banner_index = calcLeftBannerTagIndex(_now_center_slide_banner_index) ;
    }else if(swipe_direction == SwipeDirection::RIGHT){
        behind_banner_index = calcLeftBannerTagIndex(_now_center_slide_banner_index) ;
        front_banner_index = calcRightBannerTagIndex(_now_center_slide_banner_index) ;
    }
 
    auto center_banner = _slide_banner.at(_now_center_slide_banner_index);
    auto behind_banner = _slide_banner.at(behind_banner_index) ;
    auto front_banner = _slide_banner.at(front_banner_index) ;
    
//    behind_banner->setPosition(behind_point);
    
    //中心のバナーを次の場所にした後、手前に戻す
    MoveTo *move_front = MoveTo::create(auto_swipe_time, front_point) ;
    center_banner->runAction(Sequence::create(move_front, nullptr)) ;
    
    //手前のバナーを中心に
    MoveTo *move_center = MoveTo::create(auto_swipe_time, center_point) ;
    behind_banner->runAction(Sequence::create(move_center, nullptr)) ;
    
    //先頭のバナーを手前に
    Place *place_behind3 = Place::create(behind_point) ;
    CallFunc *func = CallFunc::create([this, center_banner, front_banner](){
        _touch_disable = false;
        if(_slide_banner.size()>2){
            front_banner->setVisible(false) ;
        }
        center_banner->setVisible(false) ;
        updateBannerDot() ; //赤丸の更新
    }) ;
    
    if(_slide_banner.size()>2){
        front_banner->runAction(Sequence::create(DelayTime::create(auto_swipe_time), place_behind3, func, nullptr)) ;
    }else{
        front_banner->runAction(Sequence::create(DelayTime::create(auto_swipe_time), func, nullptr)) ;
    }
    
    _now_center_slide_banner_index = behind_banner_index;
    
    this->schedule(schedule_selector(MypageLayer::updateAutoSwipeBanner), 4.0f);
}

int MypageLayer::calcRightBannerTagIndex(int index){
    int next_banner_tag_index = (index + 1) % _slide_banner.size() ;
    return next_banner_tag_index ;
}

int MypageLayer::calcLeftBannerTagIndex(int index){
    int prev_banner_tag_index = (index - 1) % _slide_banner.size();
    if(prev_banner_tag_index<0)prev_banner_tag_index = prev_banner_tag_index + _slide_banner.size() ;
    return prev_banner_tag_index ;
}

void MypageLayer::showTournamentNextTimeLabel()
{
    //次戦までのトーナメント対戦時間の保存
    NormalScene::_tournament_next_battle_time = fromString<int>(_tournament_data_list["wait_start_date_timestamp"].string_value());
    
    //トーナメントデータの設定
    SaveData::saveString("tournament_event_list_id", _tournament_data_list["tournament_event_list_id"].string_value());
    
    //トーナメント次戦対戦時刻の表示
    auto normal_scene = (NormalScene *)this->getParent();
    int total_battle_number = fromString<int>(_tournament_data_list["total_battle_number"].string_value());
    int now_battle_number = fromString<int>(_tournament_data_list["now_battle_number"].string_value());
    
    auto remain_time_sp = TournamentRemainTimeSprite::create(total_battle_number, now_battle_number, normal_scene);
    remain_time_sp->setScale(ratio);
    remain_time_sp->setPosition(convertFromDisplaySize(Point(530, 574), 0.5, 1.0));
    this->addChild(remain_time_sp, Z_Tournament_Time, T_Tournament_Time);
}

bool MypageLayer::containBanner(Point p){
    if(_slide_banner.size()>0){
        if(_slide_banner[0]->getPosition().y + _slide_banner[0]->getContentSize().height*ratio/2 > p.y && p.y > _slide_banner[0]->getPosition().y - _slide_banner[0]->getContentSize().height*ratio/2 ) {
            return true ;
            
        }
    }
    return false ;
}