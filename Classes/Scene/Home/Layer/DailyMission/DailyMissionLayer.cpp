#include "DailyMissionLayer.h"

DailyMissionLayer::DailyMissionLayer()
:_beginner_guide_complete_flg(false)
//,_rank_name()
{
}

DailyMissionLayer* DailyMissionLayer::create()
{
    DailyMissionLayer *node = new DailyMissionLayer();
    if (node && node->init())
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool DailyMissionLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("DailyMissionLayer::init()");
    
    ratio = getVisibleRatio();
    
    //jsonのdumpをオブジェクトにして取得
    HmsVector<DailyMissionGroupData *> daily_mission_group_data_list = loadData() ;
    
    //背景画像の作成
    createBg(daily_mission_group_data_list);
    
    //デイリーミッションのグループごとのスプライトを作成
    createDailyMissionGroup(daily_mission_group_data_list);
    
    //ボタンの作成
    createButton();
    
    return true;
}

HmsVector<DailyMissionGroupData *> DailyMissionLayer::loadData()
{
    //jsonに保存されているデータからインスタンス生成
    string err ;
    string json_dump_daily_mission_group_data_list = SaveData::loadString("daily_mission_group_data_list") ;
    Json json_daily_mission_group_data_list = Json::parse(json_dump_daily_mission_group_data_list, err);
    
    DailyMissionJsonParser *daily_mission_json_parser = DailyMissionJsonParser::create() ;
    HmsVector<DailyMissionGroupData *> daily_mission_group_data_list = daily_mission_json_parser->parseDailyMissionGroupDataList(json_daily_mission_group_data_list) ;
    
    
//    for(vector<DailyMissionGroupData *>::iterator iterator = daily_mission_group_data_list.begin(); iterator != daily_mission_group_data_list.end(); iterator++){
//        DailyMissionGroupData* data = *iterator ;
//        data->echoData() ;
//    }
    return daily_mission_group_data_list ;
}

void DailyMissionLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //スタンプ表示
    popupAction();
}

//クリアーアニメーションが一つ終了した時に呼ばれるコールバック
void DailyMissionLayer::endClearAnimationCallback(float dt){
    //次のクリアアニメーションがあるか判定
    if(hasNextClearAnimation()){
        //クリアアニメーションを表示する
        showNextClearAnimation() ;
    }
    else if (_beginner_guide_complete_flg) {
        //コンプリートフラブがtrueのときコンプリートレイヤーを表示させる
        showCompleteAnimation();
    }
}


//次の達成アニメーションを呼ぶ
void DailyMissionLayer::showNextClearAnimation(){
    int max_index = _daily_mission_group.size()-1 ;
    float max_scroll_distance = (max_index+1)*MISSION_GROUP_LIST_SIZE_Y - _scroll_sprite->getContentSize().height;
    
    for(vector<DailyMissionGroupSprite *>::iterator iterator = _daily_mission_group.begin(); iterator != _daily_mission_group.end(); iterator++){
        int index = (int)(iterator - _daily_mission_group.begin()) ;
        
        DailyMissionGroupSprite* group_sprite = *iterator ;
        //次のアニメーションがあるグループのアニメーションを再生
        if(group_sprite->hasNextStampAnimation()){
            float scroll_distance = index*MISSION_GROUP_LIST_SIZE_Y ;
            if(scroll_distance>max_scroll_distance)scroll_distance=max_scroll_distance ;
            
            _scroll_sprite->contentOffsetChangeFromOffsetDistance(scroll_distance) ;
            SEL_SCHEDULE s = schedule_selector(DailyMissionLayer::endClearAnimationCallback);
            group_sprite->nextStampAnimation(this,  s) ;
            return ;
        }
    }
}

ActionInterval* DailyMissionLayer::createStampAction(){
    ScaleTo *scaleTo = ScaleTo::create(1.3, 1) ;
    FadeTo *fadeTo = FadeTo::create(1.3, 255) ;
    RotateTo *rotateTo= RotateTo::create(1.3, 0) ;
    Spawn *spawn = Spawn::create(scaleTo, fadeTo, rotateTo, nullptr) ;
    EaseExponentialIn *ease_in = EaseExponentialIn::create(spawn) ;
    
    auto func = CallFunc::create([](){
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/clear_tap.mp3");
    });
    auto seq = Sequence::create(ease_in, func, nullptr);
    
    return seq ;
}

//次のアニメーションがあるか
bool DailyMissionLayer::hasNextClearAnimation(){
    for(vector<DailyMissionGroupSprite *>::iterator iterator = _daily_mission_group.begin(); iterator != _daily_mission_group.end(); iterator++){
        DailyMissionGroupSprite* group_sprite = *iterator ;
        if(group_sprite->hasNextStampAnimation()){
            return true ;
        }
    }
    return false ;
}

void DailyMissionLayer::showCompleteAnimation()
{
//    //完全達成アニメーションの表示
//    auto normal_scene = (NormalScene *)this->getParent();
//    
//    string guide_id_str = toString<int>(COMPLETE_ID - 1);
//    string reward_item_type  = json["beginner_guide_list"][guide_id_str]["item_type"].string_value() ;
//    string reward_related_id = json["beginner_guide_list"][guide_id_str]["related_primary_id"].string_value() ;
//    string comment           = json["beginner_guide_list"][guide_id_str]["clear_comment"].string_value();
//    string count             = json["beginner_guide_list"][guide_id_str]["count"].string_value();
//    string chara_name        = json["beginner_guide_list"][guide_id_str]["chara_name"].string_value();
//    
//    auto layer = DailyMissionClearAnimLayer::create(reward_item_type, reward_related_id, comment, count, chara_name, true);
//    normal_scene->addChild(layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
//    
//    //閉じられたときによばれる関数の指定
//    SEL_SCHEDULE s = schedule_selector(DailyMissionLayer::deleteAction);
//    layer->setSchedulerForClose(this, s);
}

void DailyMissionLayer::changeChallengePage(string change_page_name)
{
    //チャレンジボタンのページ遷移
    auto normal_scene = (NormalScene *)this->getParent();
    if(change_page_name == ""){
        change_page_name = "Mypage";
    }
    
    //ページ遷移
    normal_scene->changePage(change_page_name);
    
    //閉じるアクション
    deleteAction(0.0f);
}

void DailyMissionLayer::showAllNewClearStamp(){
    for(vector<DailyMissionGroupSprite *>::iterator iterator = _daily_mission_group.begin(); iterator != _daily_mission_group.end(); iterator++){
        DailyMissionGroupSprite* group_sprite = *iterator ;
        group_sprite->showAllNewClearStamp() ;
    }
}

void DailyMissionLayer::showAllOldClearStamp(){
    for(vector<DailyMissionGroupSprite *>::iterator iterator = _daily_mission_group.begin(); iterator != _daily_mission_group.end(); iterator++){
        DailyMissionGroupSprite* group_sprite = *iterator ;
        group_sprite->showAllOldClearStamp() ;
    }
}

void DailyMissionLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(DailyMissionLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DailyMissionLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DailyMissionLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DailyMissionLayer::createBg(HmsVector<DailyMissionGroupData *> daily_mission_group_data_list)
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    this->addChild(shadow, O_SHADOW);
    
    int list_size = daily_mission_group_data_list.size() ;
    //背景
//    _bg = Sprite::create("init/Home/DailyMission/beginner_bk1.png");
    _bg = ui::Scale9Sprite::create("init/Home/DailyMission/beginner_bk1.png", Rect(0, 0, 640, 740), Rect(30, 30, 580, 680));
    if(list_size>=3){
        _bg->setContentSize(Size(640, 740)) ;
    }else{
        _bg->setContentSize(Size(640, list_size*MISSION_GROUP_LIST_SIZE_Y+20)) ;
    }
    _bg->setScale(ratio) ;
    _bg->setPosition(Point(_contentSize.width / 2, - (_bg->getContentSize().height * ratio)));
    this->addChild(_bg, O_BACK, T_BACK);
    
    //背景上部
    Sprite *bg_top = Sprite::create("init/Home/DailyMission/beginner_bk1_top.png") ;
    bg_top->setAnchorPoint(Point(0.5, 0));
    bg_top->setPosition(Point(_bg->getContentSize().width/2, _bg->getContentSize().height));
    _bg->addChild(bg_top, O_BACK_TOP, T_BACK_TOP);
}

void DailyMissionLayer::createButton()
{
    //閉じるボタンの作成
    auto close_off = Sprite::create("init/Home/Common/bt1.png");
    auto close_on  = Sprite::create("init/Home/Common/bt1.png");
    close_on->setColor(Color3B(180, 180, 180));
    
    auto text_off = HMSFontUtil::createWithTTFBySystemFontArgument("閉じる", FONT_NAME_SYSTEM, 30);
    text_off->setPosition(Point(close_off->getContentSize() / 2));
    close_off->addChild(text_off, 1);
    
    auto text_on = HMSFontUtil::createWithTTFBySystemFontArgument("閉じる", FONT_NAME_SYSTEM, 30);
    text_on->setPosition(Point(close_on->getContentSize() / 2));
    close_on->addChild(text_on, 1);
    
    auto close_btn = MenuItemSprite::create(close_off, close_on, [this](Ref* sender)
                                         {
                                             //ポップアップ閉じる
                                             deleteAction(0.0f);
                                         });
    close_btn->setAnchorPoint(Point(0.5, 1));
    
    auto menu = Menu::create(close_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width/2, -0));
    _bg->addChild(menu, O_CLOSE);
}

void DailyMissionLayer::popupAction()
{
    auto children = this->getChildren();
    
    auto move = MoveTo::create(0.5f, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    auto move_ease_out = EaseBackOut::create(move);
    
    _bg->runAction( Sequence::createWithTwoActions(move_ease_out, CallFunc::create(CC_CALLBACK_0(DailyMissionLayer::showStamp, this))));
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/slide_popup.mp3");
    
}

void DailyMissionLayer::showStamp()
{
    //アニメーション表示フラグがある場合、フラグをfalseにしてアニメーション再生
    if(SaveData::loadBool("display_daily_mission_animation")) {
        SaveData::saveBool("display_daily_mission_animation", false) ;
        
        //クリア済みスタンプを最初に表示
        showAllOldClearStamp() ;
        //次のクリアアニメーションがあるか判定
        if(hasNextClearAnimation()){
            //クリアアニメーションを表示する
            showNextClearAnimation() ;
        }
    }else{
        //クリア済み・新たにクリアしたスタンプを最初に表示
        showAllNewClearStamp() ;
        showAllOldClearStamp() ;
    }
}

void DailyMissionLayer::deleteAction(float dt)
{
    
    auto move = MoveTo::create(0.5f, Point(_contentSize.width / 2, - (_bg->getContentSize().height) * ratio));
    auto move_ease_in = EaseBackIn::create(move);
    
    _bg->runAction(Sequence::create(move_ease_in, nullptr));
    
    auto sound = CallFunc::create([this](){
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/slide_popup.mp3");
    });
    
    auto func = CallFunc::create([this](){
        auto mypage_layer = (MypageLayer *)this->getParent()->getChildByTag(MAIN_LAYER_TAG);
        auto normal_scene = (NormalScene *)mypage_layer->getParent() ;
            if(mypage_layer && normal_scene->getMainName()=="")mypage_layer->showNextPopupStatus();
        
        this->removeFromParent();
    });
    
    runAction(Sequence::create(DelayTime::create(0.3f), sound, DelayTime::create(0.3f), func, nullptr));
}

bool DailyMissionLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch DailyMissionLayer!");
    
    return true;
}
void DailyMissionLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void DailyMissionLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DailyMissionLayer::createDailyMissionGroup(HmsVector<DailyMissionGroupData *> daily_mission_group_data_list) {
    for(vector<DailyMissionGroupData *>::iterator iterator = daily_mission_group_data_list.begin(); iterator != daily_mission_group_data_list.end(); iterator++){
        DailyMissionGroupData* data = *iterator ;
        DailyMissionGroupSprite *sprite = DailyMissionGroupSprite::create(data) ;
        
        //デイリーミッション達成条件に関係するページへの遷移ボタン
        Menu *menu = createGuideDetail(data->getChangePageName()) ;
        menu->setPosition(Point(sprite->getContentSize().width/2, 56)) ;
        sprite->addChild(menu, O_CHALLENGE_BUTTON) ;
        
        _daily_mission_group.push_back(sprite) ;
    }
    
    //SpriteクラスにキャストしたHmsVectorを作る
    HmsVector<Sprite *> daily_mission_groups ;
    for(vector<DailyMissionGroupSprite *>::iterator iterator = _daily_mission_group.begin(); iterator != _daily_mission_group.end(); iterator++){
        DailyMissionGroupSprite* data = *iterator ;
        daily_mission_groups.push_back((Sprite *)data);
    }
    
    //スクロールするリストを作成
    int list_size = daily_mission_group_data_list.size() ;
    Size scroll_view_size  ;
    if(list_size>=3) {
        scroll_view_size = Size(592, 720) ;
    }else{
        scroll_view_size = Size(592, list_size*MISSION_GROUP_LIST_SIZE_Y);
    }
   
    _scroll_sprite = ScrollSprite::create(daily_mission_groups, scroll_view_size, ScrollSprite::ScrollDirection::VERTICAL, false);
    _scroll_sprite->setAnchorPoint(Point(0.5, 1));
    _scroll_sprite->setPosition(Point(_bg->getContentSize().width/2, _bg->getContentSize().height)) ;
    _bg->addChild(_scroll_sprite, O_SCROLL, T_SCROLL) ;
}

Menu* DailyMissionLayer::createGuideDetail(string change_page_name){
    
    //チャレンジボタン
    auto challenge_off = Sprite::create("init/Home/DailyMission/bt_challenge.png");
    auto challenge_on  = Sprite::create("init/Home/DailyMission/bt_challenge.png");
    challenge_on->setColor(Color3B(180, 180, 180));
    
    _challenge_btn = MenuItemSprite::create(challenge_off, challenge_on, [this, change_page_name](Ref* sender)
                                            {
                                                //ボタンをタッチさせなくする
                                                auto meunItem = (MenuItemSprite *)sender;
                                                meunItem->setEnabled(false);
                                                
                                                //ページ遷移
                                                changeChallengePage(change_page_name);
                                                
                                            });
    _challenge_btn->setAnchorPoint(Point(0.5, 0.5));
    
    auto menu = Menu::create(_challenge_btn, nullptr);
    menu->setPosition(Point(0,_bg->getContentSize().height));
    
    return menu ;
}
