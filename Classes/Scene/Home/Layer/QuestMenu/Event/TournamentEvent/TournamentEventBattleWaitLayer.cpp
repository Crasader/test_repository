#include "TournamentEventBattleWaitLayer.h"

TournamentEventBattleWaitLayer::TournamentEventBattleWaitLayer()
:_update_count(0)
,_waiting_text()
,_wait_remain_time(0)
,_open_flg(false)
,_waiting_flg(false)
,_no_data_flg(false)
{
}

TournamentEventBattleWaitLayer::~TournamentEventBattleWaitLayer()
{
    unscheduleAllCallbacks();
}

TournamentEventBattleWaitLayer* TournamentEventBattleWaitLayer::create()
{
    TournamentEventBattleWaitLayer *sprite = new TournamentEventBattleWaitLayer();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEventBattleWaitLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TournamentEventBattleWaitLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TournamentEventBattleWaitLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventLitsener() ;
    
    //ページタイトル
    HeaderLayer::setupTitleBar("トーナメント待機", "TournamentList", this);
    
    //砂時計の画像表示
    createSandglass();
    
    //待機時間文字列の作成
    createWaitingLabel();
    
    //待機通信処理の開始
    scheduleOnce(schedule_selector(TournamentEventBattleWaitLayer::waitBattle), 0);
    
    //スケジューラーの開始
    schedule(schedule_selector(TournamentEventBattleWaitLayer::updateWaitingLabel), 1.0f);
    
    //    //tipsを作成
    //    createTips() ;
    //
    //    //tipsを表示
    //    showTipsAction() ;
}

void TournamentEventBattleWaitLayer::initEventLitsener()
{
	//レイヤーの後ろにタップ処理が伝搬しないように設定する
	listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TournamentEventBattleWaitLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentEventBattleWaitLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentEventBattleWaitLayer::onTouchEnded, this);
    
	//タップイベントの登録
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentEventBattleWaitLayer::waitBattle(float dt)
{
    //対戦待機通信処理（ループ）
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/tournament_event/waiting_tournament_battle.php";
    string body = "user_master_id=" + user_master_id;
    body += "&tournament_event_list_id=" + SaveData::loadString("tournament_event_list_id");
//    auto selector = httpresponse_selector(TournamentEventBattleWaitLayer::waitBattleCallback);
//    HttpConnect::sendHttpRequest(this, url, selector, body);

    HttpConnectHms::sendHttpRequest(url,[this](HttpClient *client, HttpResponse *response){
        waitBattleCallback(client, response);
    },body);
}

void TournamentEventBattleWaitLayer::waitBattleCallback(HttpClient *sender, HttpResponse *response)
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
                scheduleOnce(schedule_selector(TournamentEventBattleWaitLayer::waitBattle), 1.0f);
                
                if (_no_data_flg == false) {
                    _no_data_flg = true;
                    
                    //キャンセルボタンの作成表示
                    createCancelButton();
                }
                
            }break;
            case Next_Waiting:
            {
                if (_waiting_flg == false) {
                    _waiting_flg = true;
                    
                }
                //現在時刻の取得
                time_t now_time;
                time(&now_time);
                now_time = now_time - SaveData::loadInt("server_diff_time");
                
                _wait_remain_time = fromString<time_t>(json["wait_start_date_timestamp"].string_value()) - now_time;
                
                //次戦対戦待ち状態
                scheduleOnce(schedule_selector(TournamentEventBattleWaitLayer::waitBattle), 1.0f);
                
            }break;
            case Open_Waiting:
            {
                //トーナメント開催時間、待ち状態
                if (_open_flg == false) {
                    _open_flg = true;
                    
                    //現在時刻の取得
                    time_t now_time;
                    time(&now_time);
                    now_time = now_time - SaveData::loadInt("server_diff_time");
                    
                    _wait_remain_time = fromString<time_t>(json["wait_end_date_timestamp"].string_value()) - now_time;
                }
                
                //再起処理
                scheduleOnce(schedule_selector(TournamentEventBattleWaitLayer::waitBattle), 1.0f);
                
            }break;
            case Win_Default:
            case Exception:
            case Already_Finished:

            {
                //ポップアップ処理
                auto func = [&](){
                    auto normal_scene = (NormalScene *)this->getParent();
                    normal_scene->changePage("TournamentEventTop");
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
                
                //対戦相手が見つかった時、トーナメント開始！
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

void TournamentEventBattleWaitLayer::createSandglass()
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

void TournamentEventBattleWaitLayer::createWaitingLabel()
{
    auto waiting_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 40);
    waiting_label->setPosition(convertFromDisplaySize(Point(320, 330), 0.5, 0.5));
    waiting_label->setScale(ratio);
    waiting_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(waiting_label, Z_Waiting_Label, T_Waiting_Label);
    
    auto remain_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 50);
    remain_label->setScale(ratio);
    remain_label->setColor(Color3B(255, 255, 80));
    remain_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    remain_label->setPosition(convertFromDisplaySize(Point(320, 230), 0.5, 0.5));
    this->addChild(remain_label, Z_Waiting_Time_Label, T_Waiting_Time_Label);
    
    //注意文
    auto attention_label = HMSFontUtil::createWithTTFBySystemFontArgument("※対戦開始日時より1分後自動的にバトルが開始されます", FONT_NAME_SYSTEM, 23);
    attention_label->setPosition(convertFromDisplaySize(Point(320, 140), 0.5, 0.5));
    attention_label->setScale(ratio);
    attention_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(attention_label);
}

void TournamentEventBattleWaitLayer::createCancelButton()
{
    //キャンセルボタンの作成
    auto cancel_btn_off = Sprite::create("init/Home/Event/TournamentEvent/bt_tournament_cancel.png");
    auto cancel_btn_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_tournament_cancel.png");
    cancel_btn_on->setColor(Color3B(180, 180, 180));
    
    auto cancel_btn = MenuItemSprite::create(cancel_btn_off, cancel_btn_on,[this](Ref* sender){
        
        //スケジューラーのストップ
        unscheduleAllCallbacks();
        
        //トーナメントキャンセルページ遷移
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("TournamentEventConfirmCancel");
    
    });
    cancel_btn->setScale(ratio);
    cancel_btn->setTag(T_Cancel_MenuItem);
    
    auto menu = Menu::create(cancel_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 230), 0.5, 0.5));
    this->addChild(menu, Z_Cancel_Menu, T_Cancel_Menu);
}

void TournamentEventBattleWaitLayer::removeCancelButton()
{
    //キャンセルボタンを閉じる
    auto cancel_btn = (MenuItem *)this->getChildByTag(T_Cancel_Menu)->getChildByTag(T_Cancel_MenuItem);
    auto scale = ScaleTo::create(0.2f, 0);
    cancel_btn->runAction(Sequence::create(scale, RemoveSelf::create(true), nullptr));
}

void TournamentEventBattleWaitLayer::createTips()
{
    string tips_list_dump = SaveData::loadString("tips_list");
    string err;
    _tips_json = Json::parse(tips_list_dump, err);

    int json_count = countJsonData(_tips_json);
     
    if(json_count > 0){
     
        //tipsのフレーム構築
        _tips_frame = Sprite::create("init/Home/Quest/Online/tips_bk1.png");
        _tips_frame->setScale(ratio);
        _tips_frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 20), 0.5, 0.5));
        _tips_frame->setAnchorPoint(Point(0.5, 1));
        this->addChild(_tips_frame);

        //tipsのメッセージ表示部を作成 (キャラの上に覆い被さるように前後関係を設定するためにtips_bk1.pngとパーツが異なる)
        Sprite *message_area = Sprite::create("init/Home/Quest/Online/tips_bk2.png");
        message_area->setAnchorPoint(Point(0,0)) ;
        _tips_frame->addChild(message_area,3);

        _tips_title = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 34);
        _tips_title->setPosition(Point(_tips_frame->getContentSize().width/2, 250));
        _tips_frame->addChild(_tips_title, 4);

        _tips_comment = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 28);
        _tips_comment->setAnchorPoint(Point(0.5,1)) ;
        _tips_comment->setVerticalAlignment(TextVAlignment::TOP);
        _tips_comment->setDimensions(520, 160);
        _tips_comment->setPosition(Point(_tips_frame->getContentSize().width/2, 210));
        _tips_frame->addChild(_tips_comment, 4);

        //左のキャラを作成
        _left_chara = Sprite::create();
        _left_chara->setAnchorPoint(Point(0.5, 0));
        _left_chara->setPosition(Point(133, 285));
        _tips_frame->addChild(_left_chara,2) ;

        //右のキャラを作成
        _right_chara = Sprite::create();
        _right_chara->setAnchorPoint(Point(0.5, 0));
        _right_chara->setPosition(Point(487, 285));
        _right_chara->setFlippedX(true) ; //Y軸
        _tips_frame->addChild(_right_chara,2) ;

        //next文字
        _next = HMSFontUtil::createWithTTFBySystemFontArgument("NEXT", FONT_NAME_SYSTEM, 30);
        _next->setHorizontalAlignment(TextHAlignment::RIGHT);
        _next->setVerticalAlignment(TextVAlignment::CENTER);
        _next->setAnchorPoint(Point(1, 1));
        _next->setPosition(Point(_tips_frame->getContentSize().width - 50, 0));
        _tips_frame->addChild(_next) ;

        //三角形
        _next_shape = getTriangleShape(Point(0,1)) ;
        _next_shape->setPosition(Point(_tips_frame->getContentSize().width - 40, 0));
        _tips_frame->addChild(_next_shape) ;
    }
}

void TournamentEventBattleWaitLayer::showTipsAction()
{
    int json_count = countJsonData(_tips_json);
    if(json_count>0){
        
        auto start_func = CallFunc::create([this, json_count](){
            //nextの矢印と文字を見えなくする
            _next->setVisible(false);
            _next_shape->setVisible(false);
        });
        
        //詳細をひっこめるアクション
        MoveTo *move_hide = MoveTo::create(0.1, convertFromDisplaySize(Point(0, DISPLAY_HEIGHT/2 + 20), 0.5, 0.5)) ;
        FadeTo *fadeTo_hide = FadeTo::create(0.1, 0) ;
        Spawn *spawn_hide = Spawn::createWithTwoActions(move_hide,fadeTo_hide) ;
        
        //詳細の内容を変更する関数
        auto content_change_func = CallFunc::create([this, json_count](){
            
                //tipsの表示内容をランダムに決定
                string tips_index = toString<int>(random(0, json_count-1)) ;

                //タイトル
                string tips_title_str = _tips_json[tips_index]["title"].string_value();
            	_tips_title->setString(tips_title_str);
                
                //内容
                string tips_comment_str = _tips_json[tips_index]["comment"].string_value();
                tips_comment_str = Utility::replaceStr(tips_comment_str, "n", "\n"); //nを改行文字に置き換える
                _tips_comment->setString(tips_comment_str);
                
                //左右のキャラの画像を1~14の値でランダムに決定
                int left_chara_no = random(1, 14) ;
                int right_chara_no ;
                while(true){ //left_chara_noとright_cahra_noが重複しない値になるまでループ
                    right_chara_no = random(1, 14) ;
                    if(left_chara_no!=right_chara_no)break;
                }

                _left_chara->setTexture("init/Home/Quest/Online/tips_chara" + toString<int>(left_chara_no) + ".png");
                _right_chara->setTexture("init/Home/Quest/Online/tips_chara" + toString<int>(right_chara_no) + ".png");
        });
        
        //位置を変えるアクション
        MoveTo *move_start_place = MoveTo::create(0, convertFromDisplaySize(Point(DISPLAY_HEIGHT, DISPLAY_HEIGHT/2 + 20), 0.5, 0.5)) ;
        
        //出現するアクション
        MoveTo *move_appear = MoveTo::create(0.2, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 + 20), 0.5, 0.5)) ;
        EaseExponentialOut *ease_out_appear = EaseExponentialOut::create(move_appear);
        FadeTo *fadeTo_appear = FadeTo::create(0.1, 255) ;
        Spawn *spawn_appear = Spawn::createWithTwoActions(ease_out_appear, fadeTo_appear) ;
        
        
        auto end_func = CallFunc::create([this, json_count](){
            //nextの矢印と文字を見えるようにする
            _next->setVisible(true);
            _next_shape->setVisible(true);
        });
        
        
        _tips_frame->runAction(Sequence::create(start_func, spawn_hide, content_change_func, move_start_place, spawn_appear, end_func, nullptr)) ;
    }
}

DrawNode* TournamentEventBattleWaitLayer::getTriangleShape(const Point& anchorPoint)
{
    float triangle_width = 20;
    float triangle_height = 30;
    
    //下半分のテスト
    auto shape = DrawNode::create();
    static Vec2 triangle[3];

    triangle[0] = Vec2(0 - anchorPoint.x*triangle_width, 0 - anchorPoint.y*triangle_height);
    triangle[1] = Vec2(0 - anchorPoint.x*triangle_width, triangle_height - anchorPoint.y*triangle_height);
    triangle[2] = Vec2(triangle_width - anchorPoint.x*triangle_width, triangle_height/2 - anchorPoint.y*triangle_height);
    
    static Color4F green(0, 1, 0, 1);
    shape->drawPolygon(triangle, 3, Color4F::WHITE, 1, Color4F::GRAY);
    return shape;
}

void TournamentEventBattleWaitLayer::updateWaitingLabel(float dt)
{
    //待機テキストの更新処理
    if (_waiting_text != "") {
        auto wait_label = (Label *)this->getChildByTag(T_Waiting_Label);
        
        string text = wait_label->getString() + ".";
        if (_update_count % 4 == 0) text = _waiting_text;
        wait_label->setString(text);
        
        _update_count++;
    }
    
    if (_waiting_status == Exception || _waiting_status == Win_Default || _waiting_status == Already_Finished) {
        
        //ポップアップ表示の場合
        auto wait_label = (Label *)this->getChildByTag(T_Waiting_Label);
        wait_label->setString("");
        unschedule(schedule_selector(TournamentEventBattleWaitLayer::updateWaitingLabel));
    }
    
    if (_waiting_status == Open_Waiting || _waiting_status == Next_Waiting) {
        
        //残り時間の更新
        auto wait_time_label = (Label *)this->getChildByTag(T_Waiting_Time_Label);
        
        
        if (_wait_remain_time >= 0) {
            
            
            //残り時間文字列の作成
            string remain_text = "あと";
            
            int minute = (int)_wait_remain_time / 60; //分
            int second = (int)_wait_remain_time % 60; //秒
            
            if (minute > 0) remain_text += toString<int>(minute) + "分";
            else if (second > 0) remain_text += toString<int>(second) + "秒";
            
            wait_time_label->setString(remain_text);
            
            _wait_remain_time--;
        }
        
        if (_no_data_flg == true) {
            _no_data_flg = false;
            
            //キャンセルボタンを閉じる
            removeCancelButton();
        }
    }
}

bool TournamentEventBattleWaitLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch TournamentEventBattleWaitLayer!");
    
//    showTipsAction();
    
    return true;
}
void TournamentEventBattleWaitLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TournamentEventBattleWaitLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}