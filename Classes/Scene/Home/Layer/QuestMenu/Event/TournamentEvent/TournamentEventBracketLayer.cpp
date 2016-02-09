#include "TournamentEventBracketLayer.h"

TournamentEventBracketLayer::TournamentEventBracketLayer()
:_tap_enable(true)
,_flg_pinch_in_out_mode(false)
{
    
}

bool TournamentEventBracketLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TournamentEventBracketLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TournamentEventBracketLayer::onEnter()
{
    Layer::onEnter();
    
    if(!isNowBattle()){
        //ページタイトルの作成
        if(SaveData::loadString("bracket_back")=="entry"){
        HeaderLayer::setupTitleBar("対戦表", "TournamentEntryList", this, HeaderLayer::TitleBarType::WIDTH_466, HeaderLayer::TitleBarPosition::ALIGN_LAYER_TOP);
        }else{
            HeaderLayer::setupTitleBar("対戦表", "TournamentEventTop", this, HeaderLayer::TitleBarType::WIDTH_466, HeaderLayer::TitleBarPosition::ALIGN_LAYER_TOP);
        }
    }
    
    //開開催しているトーナメントの一覧を取得
    getTournamentEventInfoRequest();
}

void TournamentEventBracketLayer::initArrowButtons()
{
    this->addChild(createButtonByTag(T_ARROW_LEFT), Z_ARROW, T_ARROW_LEFT) ;
    this->addChild(createButtonByTag(T_ARROW_BELOW), Z_ARROW,T_ARROW_BELOW) ;
    this->addChild(createButtonByTag(T_ARROW_UPPER_RIGHT), Z_ARROW, T_ARROW_UPPER_RIGHT) ;
    this->addChild(createButtonByTag(T_ARROW_LOWER_RIGHT), Z_ARROW, T_ARROW_LOWER_RIGHT) ;
    this->addChild(createButtonByTag(T_ARROW_UP), Z_ARROW, T_ARROW_UP) ;
}

Action *TournamentEventBracketLayer::createActionByTag(int tag)
{
    float rotate = getRotateByArrowTag(tag) ;
    float x = 7 * cos(CC_DEGREES_TO_RADIANS(rotate)) ;
    float y = -1 * 7 * sin(CC_DEGREES_TO_RADIANS(rotate)) ;
    
    Point point = getPointByArrowTag(tag) ;
    MoveTo *move = MoveTo::create(0.5f, point+Point(x,y)*ratio) ;
    ActionEase *ease_in = EaseExponentialIn::create(move);
    MoveTo *move_back = MoveTo::create(0.5f, point) ;
    ActionEase *ease_back = EaseExponentialOut::create(move_back);
    RepeatForever *repeat = RepeatForever::create(Sequence::create(ease_in, ease_back, nullptr)) ;
    return repeat ;
}

ui::Button *TournamentEventBracketLayer::createButtonByTag(int tag)
{
    float rotate = getRotateByArrowTag(tag) ;
    Point point = getPointByArrowTag(tag) ;
    
    auto button = ui::Button::create("init/Home/Quest/arrow_right_alfa_big.png") ;
    button->setRotation(rotate) ;
    button->setPosition(point) ;
    button->addTouchEventListener(CC_CALLBACK_2(TournamentEventBracketLayer::touchEvent, this)) ;
    button->setScale(ratio) ;
    return button ;
}

Point TournamentEventBracketLayer::getPointByArrowTag(int tag)
{
    Point arrow_position ;
    if(tag == T_ARROW_LEFT)arrow_position = convertFromDisplaySize(Point(50, DISPLAY_HEIGHT/2), 0.5, 0.5) ;
    else if(tag == T_ARROW_BELOW) arrow_position = convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 50), 0.5, 0.0) ;
    else if(tag == T_ARROW_UPPER_RIGHT) arrow_position = convertFromDisplaySize(Point(DISPLAY_WIDTH-50, DISPLAY_HEIGHT-50-160), 0.5, 1.0) ;
    else if(tag == T_ARROW_LOWER_RIGHT) arrow_position = convertFromDisplaySize(Point(DISPLAY_WIDTH-50, 50), 0.5, 0.0) ;
    else if(tag == T_ARROW_UP) arrow_position = convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT-50-160), 0.5, 1.0) ;
    return arrow_position ;
}

float TournamentEventBracketLayer::getRotateByArrowTag(int tag)
{
    float rotate ;
    if(tag == T_ARROW_LEFT)rotate = 180;
    else if(tag == T_ARROW_BELOW) rotate = 90 ;
    else if(tag == T_ARROW_UPPER_RIGHT) rotate = -45 ;
    else if(tag == T_ARROW_LOWER_RIGHT) rotate = 45 ;
    else if(tag == T_ARROW_UP) rotate = -90;
    return rotate ;
}

void TournamentEventBracketLayer::updateArrowButtons()
{
    updateArrowButtonByTag(T_ARROW_LEFT) ;
    updateArrowButtonByTag(T_ARROW_BELOW) ;
    updateArrowButtonByTag(T_ARROW_UPPER_RIGHT) ;
    updateArrowButtonByTag(T_ARROW_LOWER_RIGHT) ;
    updateArrowButtonByTag(T_ARROW_UP) ;
}

void TournamentEventBracketLayer::updateArrowButtonByTag(int tag)
{
    //矢印に対応する移動方向が存在するか判定する
    BracketMoveInfo::Distination distination ;
    if(tag == T_ARROW_LEFT) distination = BracketMoveInfo::Distination::Up ;
    else if(tag == T_ARROW_BELOW) distination = BracketMoveInfo::Distination::Left ;
    else if(tag == T_ARROW_UPPER_RIGHT) distination = BracketMoveInfo::Distination::LowerRight ;
    else if(tag == T_ARROW_LOWER_RIGHT) distination = BracketMoveInfo::Distination::LowerLeft ;
    else if(tag == T_ARROW_UP) distination = BracketMoveInfo::Distination::Right ;
    
    BracketMoveInfo* bracketMoveInfo = _tournamentEventBracketSprite->getBracketMoveInfo() ;
    ui::Button *button = (ui::Button*) getChildByTag(tag) ;
    button->stopAllActions() ;
    
    if(bracketMoveInfo->hasValidDestination(distination)){
        //移動先がある場合、矢印を明るくする
        button->setTouchEnabled(true) ;
        button->setVisible(true) ;
        
        Action *repeat_action = createActionByTag(tag) ;
        button->runAction(repeat_action) ;
        
    }else{
        //移動先がない場合、矢印を暗くして無効にする
        button->setTouchEnabled(false) ;
        button->setVisible(false) ;
    }
}


void TournamentEventBracketLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchAllAtOnce::create(); //複数対応のリスナー作成
    listener->onTouchesBegan = CC_CALLBACK_2(TournamentEventBracketLayer::onTouchBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(TournamentEventBracketLayer::onTouchMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(TournamentEventBracketLayer::onTouchEnded, this);
    listener->onTouchesCancelled = CC_CALLBACK_2(TournamentEventBracketLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentEventBracketLayer::getTournamentEventInfoRequest()
{
    //トーナメント一覧取得php
    string user_master_id = SaveData::loadString("user_master_id");
    string tournament_event_list_id = SaveData::loadString("tournament_event_list_id");
    
    string url = HOST;
    url += "event/tournament_event/display_bracket_info.php";
    auto selector = httpresponse_selector(TournamentEventBracketLayer::tournamentEventInfoCallback);
    string body = "user_master_id=" + user_master_id + "&tournament_event_list_id=" + tournament_event_list_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEventBracketLayer::tournamentEventInfoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    
    //トーナメント情報の取得
    _bracket_data = Json::parse(result, err);
    
    string error_status = _bracket_data["error_status"].string_value() ;
    if(error_status == "no_entry"){
        //現在開催待ちのトーナメントがない場合
        returnPage("TournamentEventTop", "トーナメント一覧から\nエントリーを行って下さい");
    } else if (error_status == "not_exist_bracket") {
        returnPage("TournamentEventTop", "対戦表を作成中です。\n暫くお待ちください。");
    } else if (strlen(err.c_str()) == 0) {
       
        //トーナメント一覧の作成
        createBracketList();
        
        if(isNowBattle()){
            //バトル時は背景を移動
            Sprite *back = Sprite::create("init/Home/Event/TournamentEvent/back2.png") ;
            back->setScale(ratio) ;
            back->setAnchorPoint(Point(0, 0.5)) ;
            back->setPosition(Point(0, _contentSize.height/2) ) ;
            back->runAction(MoveTo::create(10, Point( _contentSize.width - back->getContentSize().width*ratio, _contentSize.height/2))) ;
            this->addChild(back, Z_BACK) ;
            
            //バトル時は5秒後にダンジョンシーンに遷移
            CallFunc* func = CallFunc::create([this](){
                auto scene = DungeonScene::createScene();
                auto director = Director::getInstance();
                director->replaceScene(scene);
            });
            this->runAction(Sequence::create(DelayTime::create(5), func, nullptr)) ;
        }else{
            //バトルじゃない時は移動用の矢印を表示
            
            //矢印を作成
            initArrowButtons() ;
            //矢印の情報を更新
            updateArrowButtons() ;
        }
        
    } else {
        returnPage("TournamentEventTop", "トーナメント一覧から\nエントリーを行って下さい");
    }
    
    //イベントリスナーの登録
    initEventListener();
    
}

void TournamentEventBracketLayer::returnPage(string page_name, string popup_message){
        //現在開催待ちのトーナメントがない場合
        auto normal_scene = (NormalScene *)this->getParent();
        auto func = [this, page_name, normal_scene](){
            normal_scene->changePage(page_name);
        };
    
        auto popup_layer = PopupOkButtonLayer::create(func, popup_message);
        normal_scene->addChild(popup_layer, Z_POPUP_LAYER, T_POPUP_LAYER);
}

void TournamentEventBracketLayer::createBracketList()
{
    _tournamentEventBracketSprite = TournamentEventBracketSprite::create(isNowBattle(), _bracket_data, Size(_contentSize.width - getVisibleSpaceWidth()*2, _contentSize.height)) ;
    _tournamentEventBracketSprite->setRotation(-90) ;
    _tournamentEventBracketSprite->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5)) ;
    _tournamentEventBracketSprite->setScale(ratio) ;
    this->addChild(_tournamentEventBracketSprite, Z_BRACKET) ;
    
    if(!isNowBattle()){
        //リボンのスプライトを生成
        Sprite *ribbon = Sprite::create("init/Home/Event/TournamentEvent/ribbon.png");
        ribbon->setScale(0.8f*ratio) ;
        ribbon->setAnchorPoint(Point(0,1));
        ribbon->setPosition(convertFromDisplaySize(Point(30, 860), 0.5, 1));
        this->addChild(ribbon,Z_RIBBON) ;
        
        //ラベルをリボンに追加
        string tournament_name_str = _bracket_data["tournament_name"].string_value() ;
        Label *tournament_name = HMSFontUtil::createWithTTFBySystemFontArgument(tournament_name_str, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(tournament_name_str, 40, 460));
        tournament_name->setVerticalAlignment(TextVAlignment::CENTER);
        tournament_name->setColor(Color3B::YELLOW) ;
        tournament_name->setPosition(Point(ribbon->getContentSize().width/2, 64));
        ribbon->addChild(tournament_name) ;
    }

    return ;
}

void TournamentEventBracketLayer::onTouchBegan(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    _touch_begin_bracket_scale = _tournamentEventBracketSprite->getScale() ;
    _flg_pinch_in_out_mode = false ;
    
}
void TournamentEventBracketLayer::onTouchMoved(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event)
{
    if(_tap_enable == false) return  ;
    
    //タップの始点と現在位置が一定距離を超えないときは処理を通さない
    if(touches.size()>0){
        Point diff = touches[0]->getLocation() - touches[0]->getStartLocation() ;
        if(diff.length() < 10){
            return ;
        }
    }
          
    if(touches.size()==2){
        _flg_pinch_in_out_mode = true ;
        Point start_diff = touches[1]->getStartLocation() - touches[0]->getStartLocation() ;
        Point diff = touches[1]->getLocation() - touches[0]->getLocation() ;
        _tournamentEventBracketSprite->setScale(_touch_begin_bracket_scale*diff.length()/start_diff.length());
        
        if(_tournamentEventBracketSprite->getScale() > ratio*3){
            _tournamentEventBracketSprite->setScale(ratio*3) ;
        }else if(_tournamentEventBracketSprite->getScale() < ratio/3){
            _tournamentEventBracketSprite->setScale(ratio/3) ;
        }
    }
    
    if(touches.size()==1 && _flg_pinch_in_out_mode == false){
        Point diff = touches[0]->getLocation() - touches[0]->getStartLocation() ;
        float radian = Point(1,0).getAngle(diff);
        float angle = CC_RADIANS_TO_DEGREES(radian) ;
        
        //スワイプの方向を決定
        TournamentEventBracketSprite::SwipeDirection swipeDirection ;
        if(angle >= 0 && angle < 60){
            swipeDirection = TournamentEventBracketSprite::SwipeDirection::LEFT ;
        }else if(angle >= 60 && angle < 120){
            swipeDirection = TournamentEventBracketSprite::SwipeDirection::BELOW;
        }else if(angle >= 120 && angle < 180){
            swipeDirection = TournamentEventBracketSprite::SwipeDirection::LOWER_RIGHT ;
        }else if(angle >= -180 && angle < -120){
            swipeDirection = TournamentEventBracketSprite::SwipeDirection::UPPER_RIGHT ;
        }else if(angle >= -120 && angle < -60){
            swipeDirection = TournamentEventBracketSprite::SwipeDirection::UP  ;
        }else if(angle >= -60 && angle < 0){
            swipeDirection = TournamentEventBracketSprite::SwipeDirection::LEFT ;
        }
        
        //タップ不可にする。
        _tap_enable = false ;
        //タップ可能にさせる
        auto func_after_move = [this](){
            _tap_enable = true ;
            updateArrowButtons() ;
        };
        _tournamentEventBracketSprite->moveNodeBySwipeDirection(swipeDirection, func_after_move) ;
    }
//    //タップで連動して動く処理
//    std::vector<cocos2d::Touch*>::const_iterator iterator = touches.begin() ;
//    while(iterator != touches.end()){
//        Touch* touch = (Touch*)(*iterator) ;
//        Vec2 delta = touch->getDelta() ;
//        Vec2 replaceXYDelta = Vec2(delta.y, -delta.x) ;
//        _tournamentEventBracketSprite->setAnchorPoint(_tournamentEventBracketSprite->getAnchorPoint() - _tournamentEventBracketSprite->convertPositionToAnchorPoint(replaceXYDelta/_tournamentEventBracketSprite->getScale())) ;
//        iterator++ ;
//    }
//    
//    //anchorPointがx軸方向に0~1の間でなかった場合 その範囲に修正する
//    Point anc1 = _tournamentEventBracketSprite->getAnchorPoint() ;
//    if(anc1.x<0) _tournamentEventBracketSprite->setAnchorPoint(Point(0, anc1.y)) ;
//    if(1<anc1.x) _tournamentEventBracketSprite->setAnchorPoint(Point(1, anc1.y)) ;
//    
//    Point anc2 = _tournamentEventBracketSprite->getAnchorPoint() ;
//    if(anc2.y<0) _tournamentEventBracketSprite->setAnchorPoint(Point(anc2.x, 0)) ;
//    if(1<anc2.y) _tournamentEventBracketSprite->setAnchorPoint(Point(anc2.x, 1)) ;
}

void TournamentEventBracketLayer::touchEvent(Ref *sender, cocos2d::ui::Widget::TouchEventType type)
{
    switch(type)
    {
        case ui::Widget::TouchEventType::BEGAN:{
            HMSLOG("touchEvent: %s", "BEGAN") ;
            int tag = ((ui::Button *)sender)->getTag() ;
            
            TournamentEventBracketSprite::SwipeDirection swipeDirection ;
            if(tag == T_ARROW_LEFT) swipeDirection = TournamentEventBracketSprite::SwipeDirection::LEFT ;
            else if(tag == T_ARROW_BELOW) swipeDirection = TournamentEventBracketSprite::SwipeDirection::BELOW;
            else if(tag == T_ARROW_UPPER_RIGHT) swipeDirection = TournamentEventBracketSprite::SwipeDirection::UPPER_RIGHT ;
            else if(tag == T_ARROW_LOWER_RIGHT) swipeDirection = TournamentEventBracketSprite::SwipeDirection::LOWER_RIGHT ;
            else if(tag == T_ARROW_UP) swipeDirection = TournamentEventBracketSprite::SwipeDirection::UP  ;
            
            //タップ不可にする。
            _tap_enable = false ;
            auto func_after_move = [this](){
                //タップ可能にさせる
                _tap_enable = true ;
                updateArrowButtons() ;
            };
            _tournamentEventBracketSprite->moveNodeBySwipeDirection(swipeDirection, func_after_move) ;
            
            
            _long_tap_swipe_direction = swipeDirection ;
            this->scheduleOnce(schedule_selector(TournamentEventBracketLayer::updateLongTapDetect), 0.5f);
            break;
        }
        case ui::Widget::TouchEventType::MOVED:
            break;
            
        case ui::Widget::TouchEventType::ENDED:
        case ui::Widget::TouchEventType::CANCELED:
        {
            HMSLOG("touchEvent: %s", "ENDED||CANCELED") ;
            this->unschedule(schedule_selector(TournamentEventBracketLayer::updateLongTapDetect));
            this->unschedule(schedule_selector(TournamentEventBracketLayer::updateLongTap));
            break;
        }
        default:
            break;
    }
    
}

void TournamentEventBracketLayer::updateLongTapDetect(float dt){
    //0.5秒以上ボタンに指が乗っている場合ロングタップと見なし、断続的にスクロールする
    this->schedule(schedule_selector(TournamentEventBracketLayer::updateLongTap), 0.1f);
}

void TournamentEventBracketLayer::updateLongTap(float dt){
    
    int tag ;
    if(_long_tap_swipe_direction == TournamentEventBracketSprite::SwipeDirection::UP) tag = T_ARROW_UP;
    else if(_long_tap_swipe_direction == TournamentEventBracketSprite::SwipeDirection::BELOW) tag = T_ARROW_BELOW ;
    else if(_long_tap_swipe_direction == TournamentEventBracketSprite::SwipeDirection::UPPER_RIGHT) tag = T_ARROW_UPPER_RIGHT ;
    else if(_long_tap_swipe_direction == TournamentEventBracketSprite::SwipeDirection::LOWER_RIGHT) tag = T_ARROW_LOWER_RIGHT ;
    else if(_long_tap_swipe_direction == TournamentEventBracketSprite::SwipeDirection::LEFT) tag = T_ARROW_LEFT ;
    
    if(((ui::Button*)getChildByTag(tag))->isTouchEnabled()){
        //タップ不可にする。
        _tap_enable = false ;
        auto func_after_move = [this](){
            //タップ可能にさせる
            _tap_enable = true ;
            updateArrowButtons() ;
        };
        _tournamentEventBracketSprite->moveNodeBySwipeDirection(_long_tap_swipe_direction, func_after_move) ;
    }else{
        this->unschedule(schedule_selector(TournamentEventBracketLayer::updateLongTap));
        this->unschedule(schedule_selector(TournamentEventBracketLayer::updateLongTapDetect));
    }
    
}

void TournamentEventBracketLayer::onTouchEnded(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event)
{
    
}
void TournamentEventBracketLayer::onTouchCancelled(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event *event)
{
    
}

bool TournamentEventBracketLayer::isNowBattle(){
    string bracket_type = SaveData::loadString("bracket_type") ;
    if(bracket_type == "now_battle"){
        return true;
    }else if(bracket_type == "display"){
        return false;
    }else{
        return false;
    }
}
