#include "BeginnerGuideLayer.h"

BeginnerGuideLayer::BeginnerGuideLayer()
:_active_guide_detail_id(0)
,_next_animation_guide_index(0)
,_beginner_guide_complete_flg(false)
//,_rank_name()
{
}

BeginnerGuideLayer* BeginnerGuideLayer::create()
{
    BeginnerGuideLayer *node = new BeginnerGuideLayer();
    if (node && node->init())
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool BeginnerGuideLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BeginnerGuideLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}
void BeginnerGuideLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //背景画像の作成
    createBg();
    
    //ボタンの作成
    createButton();
    
    //スタンプ画像を作成
    createStamp() ;
    
    //「初心の手引き」のデータ更新や取得のリクエストを送信
    getBeginnerGuideDataRequest() ;
}

void BeginnerGuideLayer::getBeginnerGuideDataRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    //リクエスト
    string url = HOST;
    url += "beginner/check_beginner_guide_clear.php";
    auto selector = httpresponse_selector(BeginnerGuideLayer::getBeginnerGuideDataCallback);
    
    string body = "user_master_id="+user_master_id ;
    
    if(SaveData::loadBool("beginner_guide_clear_flg_1"))body += "&f1=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_2"))body += "&f2=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_3"))body += "&f3=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_4"))body += "&f4=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_5"))body += "&f5=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_6"))body += "&f6=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_7"))body += "&f7=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_8"))body += "&f8=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_9"))body += "&f9=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_10"))body += "&f10=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_11"))body += "&f11=true";
    if(SaveData::loadBool("beginner_guide_clear_flg_12"))body += "&f12=true";
    
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BeginnerGuideLayer::getBeginnerGuideDataCallback(HttpClient *sender, HttpResponse *response)
{
    //レスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s",result.c_str());
    
    string err;
    json = Json::parse(result, err);
    
    //スタンプの背景の作成
    createPanels() ;
    
    //スタンプを表示し、アニメーションを表示するデータを設定する
    showStampAndSetAnimationData() ;
    
    //詳細の表示
    createGuideDetail() ;
    
    //ポップアップアクション
    popupAction();
    
}

void BeginnerGuideLayer::updateActiveGuideDetail(int beginner_guide_id){
    //アクティブな詳細のidを変更
    _active_guide_detail_id = beginner_guide_id ;
    
    //アクティブな項目を表示する
    showActiveAction() ;
    showGuideDetailAction() ;
}

//クリアーアニメーションが一つ終了した時に呼ばれるコールバック
void BeginnerGuideLayer::endClearAnimationCallback(float dt){
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
void BeginnerGuideLayer::showNextClearAnimation(){
    //タップ禁止レイヤー生成
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->addTapDisableLayer() ;
    
    int guide_id = _animation_guide_ids[_next_animation_guide_index] ;
    
    //アニメーションが再生される最初の値を設定する
    _stamp[guide_id-1]->setScale(4) ;
    _stamp[guide_id-1]->setRotation(20) ;
    
    //詳細の更新
    updateActiveGuideDetail(guide_id);
    
    //スタンプが押されるアニメーション
    ActionInterval *stamp_action = createStampAction() ;
    
    auto next_animation_func = CallFunc::create([this, guide_id, normal_scene](){
        
        //タップ禁止レイヤー削除
        normal_scene->removeTapDisableLayer() ;
        
        //アニメーションを再生するための情報の取得
        string guide_id_str      = toString<int>(guide_id - 1) ;
        string reward_item_type  = json["beginner_guide_list"][guide_id_str]["item_type"].string_value() ;
        string reward_related_id = json["beginner_guide_list"][guide_id_str]["related_primary_id"].string_value() ;
        string comment           = json["beginner_guide_list"][guide_id_str]["clear_comment"].string_value();
        string count             = json["beginner_guide_list"][guide_id_str]["count"].string_value();
        string chara_name        = json["beginner_guide_list"][guide_id_str]["chara_name"].string_value();
//        bool all_complate_anim_flg = json["beginner_guide_complete_flg"].bool_value();
    
        //別レイヤーでアニメーションを再生（通常の達成レイヤーの表示）
        auto layer = BeginnerGuideClearAnimLayer::create(reward_item_type, reward_related_id, comment, count, chara_name, false);
        normal_scene->addChild(layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
        
        //閉じられたときによばれる関数の指定
        SEL_SCHEDULE s = schedule_selector(BeginnerGuideLayer::endClearAnimationCallback);
        layer->setSchedulerForClose(this, s);
        
        //indexを進める
        _next_animation_guide_index++ ;
    });
    
    
    _stamp[guide_id-1]->runAction(Sequence::create(DelayTime::create(1.0), stamp_action, DelayTime::create(1.0), next_animation_func, nullptr)) ;
}

ActionInterval* BeginnerGuideLayer::createStampAction(){
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
bool BeginnerGuideLayer::hasNextClearAnimation(){
    if(_animation_guide_ids.size() <= _next_animation_guide_index){
        return false;
    }else{
        return true;
    }
}

void BeginnerGuideLayer::showCompleteAnimation()
{
    //完全達成アニメーションの表示
    auto normal_scene = (NormalScene *)this->getParent();
    
    string guide_id_str = toString<int>(COMPLETE_ID - 1);
    string reward_item_type  = json["beginner_guide_list"][guide_id_str]["item_type"].string_value() ;
    string reward_related_id = json["beginner_guide_list"][guide_id_str]["related_primary_id"].string_value() ;
    string comment           = json["beginner_guide_list"][guide_id_str]["clear_comment"].string_value();
    string count             = json["beginner_guide_list"][guide_id_str]["count"].string_value();
    string chara_name        = json["beginner_guide_list"][guide_id_str]["chara_name"].string_value();
    
    auto layer = BeginnerGuideClearAnimLayer::create(reward_item_type, reward_related_id, comment, count, chara_name, true);
    normal_scene->addChild(layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    
    //閉じられたときによばれる関数の指定
    SEL_SCHEDULE s = schedule_selector(BeginnerGuideLayer::deleteAction);
    layer->setSchedulerForClose(this, s);
}

void BeginnerGuideLayer::showActiveAction(){
    
    if(_active_guide_detail_id!=0){
        int index = _active_guide_detail_id - 1;
        //アニメーション生成
        ScaleTo *scale_to_big = ScaleTo::create(0.1, 1.05) ;
        ScaleTo *scale_to_small = ScaleTo::create(0.2f, 1) ;
        EaseElasticOut *scale_ease_out = EaseElasticOut::create(scale_to_small);
        
        Sequence *popup = Sequence::createWithTwoActions(scale_to_big, scale_ease_out) ;
        _panel[index]->runAction(popup) ;
        
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/select_menu.mp3");
    }
    
    /*
    if(_active_guide_detail_id!=0){
        
        //左から2番目,上から2番目の位置の中心
        Point center_1_1 = Point(312, 340) ;
        //スタンプ間の距離(中心同士)
        Point distance = Point(188, 125) ;
        
        //0番目の中心座標を計算
        float stamp_pos_x_0 = center_1_1.x - distance.x;
        float stamp_pos_y_0 = center_1_1.y + distance.y;
        
        //位置を決める
        int index =  _active_guide_detail_id - 1;
        int column = index % 3 ;
        int row = index / 3 ;
        
        //アクティブを示す画像の配置
        _active_sprite->setVisible(true) ;
        _active_sprite->setPosition(Point(stamp_pos_x_0 + (column * distance.x), stamp_pos_y_0 - (row * distance.y)));
    }
     */
}
void BeginnerGuideLayer::showGuideDetailAction(){
    
    //詳細をひっこめるアクション
    MoveTo *move_hide = MoveTo::create(0.1, Point(0,0)) ;
    FadeTo *fadeTo_hide = FadeTo::create(0.1, 0) ;
    Spawn *spawn_hide = Spawn::createWithTwoActions(move_hide,fadeTo_hide) ;
    
    //詳細の内容を変更する関数
    auto content_change_func = CallFunc::create([this](){
        
        if(SaveData::loadBool(("beginner_guide_clear_flg_"+toString<int>(_active_guide_detail_id)).c_str())){
            //クリア済みの時
            _challenge_btn->setVisible(false) ;
            _disable_challenge_btn->setVisible(true) ;
            _detail_cleared_mark->setVisible(true) ;
        }else{
            //未クリアの時
            _challenge_btn->setVisible(true) ;
            _disable_challenge_btn->setVisible(false) ;
            _detail_cleared_mark->setVisible(false) ;
        }
        
        if(_active_guide_detail_id!=0){
            //詳細に表示するテキストを更新する
            string index_str = toString<int>(_active_guide_detail_id-1) ;
            string detail_str = json["beginner_guide_list"][index_str]["detail"].string_value() ;
            _guide_detail_label->setString(detail_str) ;
            _guide_detail_label->setSystemFontSize(Utility::calcFlexibleFontSize(detail_str, 37, 560)) ;
            _reward_count_label->setString("× " + json["beginner_guide_list"][index_str]["count"].string_value()) ;
            
            string item_type = json["beginner_guide_list"][index_str]["item_type"].string_value() ;
            //プレゼント画像毎で切り替え
            if (item_type == "chara")
            {
                _reward_prite->setTexture("init/Home/Beginner/panel_reward3.png");
                
                //カード画像のパスを取得
                string card_id = json["beginner_guide_list"][index_str]["related_primary_id"].string_value() ;
                string card_pass = "download/Chara/all100/" + card_id + ".png";
                
                _reward_prite->setTexture(card_pass) ;
                _reward_prite->setScale(0.6) ; //カード画像を0.6倍に
            }
            else if (item_type == "gold")
            {
                _reward_prite->setTexture("init/Home/Beginner/reward1.png");
                _reward_prite->setScale(1) ;
            }
            else if (item_type == "jewel")
            {
                _reward_prite->setTexture("init/Home/Beginner/reward2.png");
                _reward_prite->setScale(1) ;
            }
                
        }else{
            //表示するメッセージの作成
            if(json["new_clear_data_list"]["count"].int_value() >= 1){
                _guide_detail_label->setString("達成おめでとう！\nプレゼント一覧から報酬を受け取ってね♪" ) ;
            } else {
                _guide_detail_label->setString("タップして選んでね。" );
            }
        }
    });
    
    //位置を変えるアクション
    MoveTo *move_start_place = MoveTo::create(0, Point(_bg->getContentSize().width, 0)) ;
    
    //出現するアクション
    MoveTo *move_appear = MoveTo::create(0.2, Point(_bg->getContentSize().width/2, 0)) ;
    EaseExponentialOut *ease_out_appear = EaseExponentialOut::create(move_appear);
    FadeTo *fadeTo_appear = FadeTo::create(0.1, 255) ;
    Spawn *spawn_appear = Spawn::createWithTwoActions(ease_out_appear, fadeTo_appear) ;
    
    _guide_detail_sprite->runAction(Sequence::create(spawn_hide, content_change_func, move_start_place, spawn_appear, nullptr)) ;
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/slide_detail.mp3");
}


void BeginnerGuideLayer::createGuideDetail(){
    
    //仮の詳細背景
    _guide_detail_sprite = Sprite::create("init/Home/Beginner/beginner_bk2.png");
    _guide_detail_sprite->setAnchorPoint(Point(0.5, 1));
    _guide_detail_sprite->setPosition(Point(_bg->getContentSize().width/2, 0));
    _guide_detail_sprite->setOpacity(0) ;
    _guide_detail_sprite->setCascadeOpacityEnabled(true) ;
    _bg->addChild(_guide_detail_sprite, O_Guide_Detail) ;
    
    //任務の詳細を表示するラベル
    _guide_detail_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 37);
    _guide_detail_label->setHorizontalAlignment(TextHAlignment::LEFT);
    _guide_detail_label->setPosition(Point(_bg->getContentSize().width/2, 143));
    _guide_detail_sprite->addChild(_guide_detail_label, 2);
    
    //報酬の個数を表示するラベル
    _reward_count_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 30);
    _reward_count_label->setHorizontalAlignment(TextHAlignment::LEFT);
    _reward_count_label->setPosition(206, 70);
    _reward_count_label->setAnchorPoint(Point(0,0.5)) ;
    _guide_detail_sprite->addChild(_reward_count_label, 2);
    
    //報酬の画像を表示するスプライト
    _reward_prite = Sprite::create() ;
    _reward_prite->setPosition(162, 70);
    _guide_detail_sprite->addChild(_reward_prite, 2);
    
    
    /*
    //アクティブな詳細の位置を示すスプライトの生成
    _active_sprite = Sprite::create("init/Dungeon/design/battle_skill_select.png") ;
    _active_sprite->setScale(1.2) ;
    _active_sprite->setVisible(false) ;
    _bg->addChild(_active_sprite, O_Active) ;
     */
    
    //クリア済みの判子
    
    //無効なチャレンジボタン(任務終了済みのときに表示)
    _disable_challenge_btn = Sprite::create("init/Home/Beginner/bt_challenge.png");
    _disable_challenge_btn->setPosition(Point(470, 68));
    _disable_challenge_btn->setColor(Color3B(180, 180, 180)) ;
    _guide_detail_sprite->addChild(_disable_challenge_btn, 1);
    
    //クリア済みの時に表示する判子
    _detail_cleared_mark = Sprite::create("init/Home/Beginner/clear.png");
    _detail_cleared_mark->setPosition(Point(545, 145));
    _guide_detail_sprite->addChild(_detail_cleared_mark, 3);
    
    //チャレンジ（ページ遷移ボタン）ボタン仮
    auto challenge_off = Sprite::create("init/Home/Beginner/bt_challenge.png");
    auto challenge_on  = Sprite::create("init/Home/Beginner/bt_challenge.png");
    challenge_on->setColor(Color3B(180, 180, 180));
    
    _challenge_btn = MenuItemSprite::create(challenge_off, challenge_on, [this](Ref* sender)
                                            {
                                                //ボタンをタッチさせなくする
                                                auto meunItem = (MenuItemSprite *)sender;
                                                meunItem->setEnabled(false);
                                                
                                                //ページ遷移
                                                changeChallengePage();
                                                
                                            });
    _challenge_btn->setAnchorPoint(Point(0.5, 0.5));
    
    auto menu = Menu::create(_challenge_btn, nullptr);
    menu->setPosition(Point(470, 68));
    _guide_detail_sprite->addChild(menu, 1);
    
    /*
    //クリア状況を見て最初に表示する詳細を計算する(クリア済みのものの詳細は開いたときデフォルトでは表示しない)
    int initial_guide_detail_id = calcInitialGuideDetailId() ;
    updateActiveGuideDetail(initial_guide_detail_id);
     */
}

int BeginnerGuideLayer::calcInitialGuideDetailId(){
    //クリア済みのidではない初めて現れるidを結果として返す
    int result_id = 1 ;
    for (int i = 0; i < MAX_STAMP_COUNT; i++) {
        string i_str = toString<int>(i+1) ;
        if(SaveData::loadBool(("beginner_guide_clear_flg_"+i_str).c_str())){
        }else{
            result_id = i+1 ;
            break ;
        }
    }
    return result_id ;
}

void BeginnerGuideLayer::createPanels(){
    
    //左から2番目,上から2番目の位置の中心
    Point center_1_1 = Point(312, 340) ;
    //スタンプ間の距離(中心同士)
    Point distance = Point(188, 125) ;
    
    
    //0番目の中心座標を計算
    float stamp_pos_x_0 = center_1_1.x - distance.x;
    float stamp_pos_y_0 = center_1_1.y + distance.y;
    
    for (int i = 0; i < MAX_STAMP_COUNT; i++) {
        string index_string = toString<int>(i) ;
        
        //プレゼント画像の背景の作成
        string item_type = json["beginner_guide_list"][index_string]["item_type"].string_value() ;
        
        Sprite *panel = Sprite::create("init/Home/Beginner/panel.png");
        _panel.push_back(panel) ;
        
        //パネル左上の小さい画像
        Sprite *item_thumb = Sprite::create() ;
        
        //プレゼント画像毎で切り替え
        if (item_type == "chara")
        {
            item_thumb->setTexture("init/Home/Beginner/panel_reward3.png");
            
            //カード画像をパネルの小さい画像の左上に重ねる
            string card_id = json["beginner_guide_list"][index_string]["related_primary_id"].string_value() ;
            string card_pass = "download/Chara/all100/" + card_id + ".png";
            Sprite *card = Sprite::create(card_pass) ;
            card->setScale(0.45) ; //カード画像を0.45倍に
            card->setPosition(item_thumb->getContentSize()/2) ; //カード画像を0.5倍に
            
            item_thumb->addChild(card);
        }
        else if (item_type == "gold")
        {
            item_thumb->setTexture("init/Home/Beginner/panel_reward1.png");
        }
        else if (item_type == "jewel")
        {
            item_thumb->setTexture("init/Home/Beginner/panel_reward2.png");
        }
        
        item_thumb->setPosition(Point(15,_panel[i]->getContentSize().height-15)) ;
        _panel[i]->addChild(item_thumb) ;
        
        //表示位置の行と列を調べる
        int column = i % 3 ;
        int row = i / 3 ;
        
        _panel[i]->setPosition(Point(stamp_pos_x_0 + (column * distance.x), stamp_pos_y_0 - (row * distance.y)));
        _bg->addChild(_panel[i],O_Stamp_Back) ;
        
        //パネル上の任務の概要ラベル
        Sprite *panel_detail = Sprite::create("init/Home/Beginner/panel_index"+toString<int>(i+1)+".png") ;
        panel_detail->setPosition(_panel[i]->getContentSize()/2) ;
        _panel[i]->addChild(panel_detail)  ;
    }
}

void BeginnerGuideLayer::createStamp(){
    //左から2番目,上から2番目の位置の中心
    Point center_1_1 = Point(312, 340) ;
    //スタンプ間の距離(中心同士)
    Point distance = Point(188, 125) ;
    
    //0番目の中心座標を計算
    float stamp_pos_x_0 = center_1_1.x - distance.x;
    float stamp_pos_y_0 = center_1_1.y + distance.y;
    
    for (int i = 0; i < MAX_STAMP_COUNT; i++) {
        int column = i % 3 ;
        int row = i / 3 ;
        Sprite *stamp = Sprite::create("init/Home/Beginner/clear.png") ;
        stamp->setPosition(Point(stamp_pos_x_0 + (column * distance.x), stamp_pos_y_0 - (row * distance.y)));
        stamp->setOpacity(0) ; //透過度を0にして見えないようにしておく
        _bg->addChild(stamp,O_Stamp);
        _stamp.push_back(stamp) ;
    }
}

//クリアー状況のスタンプを作成
void BeginnerGuideLayer::showStampAndSetAnimationData() {
    int new_clear_count = json["new_clear_data_list"]["count"].int_value() ;
    HMSLOG("新規のクリア項目数:%d", new_clear_count) ;
    
    for (int i = 0; i < MAX_STAMP_COUNT; i++) {
        string id_str = toString<int>(i+1) ;
        
        bool new_clear = json["new_clear_data_list"]["new_clear_flg_" + id_str].bool_value() ;
        if(new_clear){
            _animation_guide_ids.push_back(i+1) ;
        }else if(SaveData::loadBool(("beginner_guide_clear_flg_"+id_str).c_str())){
            //「初心の手引き」既にクリアしていた項目の場合そのまま表示する
            _stamp[i]->setOpacity(255) ;
        }else{
            //クリアしてないよ
        }
    }
    
    //コンプリートフラグの取得
    _beginner_guide_complete_flg = json["beginner_guide_complete_flg"].bool_value();
    
    //未受け取りプレゼント数の更新
    auto mypage_layer = (MypageLayer *)this->getParent()->getChildByTag(MAIN_LAYER_TAG);
    if (mypage_layer)
    {
        int add_count = (int)_animation_guide_ids.size();
        if (_beginner_guide_complete_flg)
        {
            //コンプリート報酬はnew_clear_data_listに含まれないのでここで足す
            add_count++;
        }
        mypage_layer->updatePresentCount(add_count);
    }
}

void BeginnerGuideLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BeginnerGuideLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BeginnerGuideLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BeginnerGuideLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BeginnerGuideLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Beginner/beginner_bk1.png");
    _bg->setScale(ratio) ;
    _bg->setAnchorPoint(Point(0.5, 0)) ;
    _bg->setPosition(Point(_contentSize.width / 2, - (_bg->getContentSize().height * ratio)));
//    _bg->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, -DISPLAY_HEIGHT/2), 0.5, 0.5));
    this->addChild(_bg, O_Popup ,T_Popup);
}

void BeginnerGuideLayer::createButton()
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
    menu->setPosition(Point(_bg->getContentSize().width/2, -210));
    _bg->addChild(menu, 1);
}

void BeginnerGuideLayer::popupAction()
{
    auto children = this->getChildren();
    
    auto move = MoveTo::create(0.5f, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 300), 0.5, 0.5));
    auto move_ease_out = EaseBackOut::create(move);
    
    auto after_func = CallFunc::create([this](){
        //クリア状況を見て最初に表示する詳細を計算する(クリア済みのものの詳細は開いたときデフォルトでは表示しない)
        
        //次のクリアアニメーションがあるか判定
        if(hasNextClearAnimation()){
            
            //クリアアニメーションを表示する
            showNextClearAnimation() ;
        }else{
            int initial_guide_detail_id = calcInitialGuideDetailId() ;
            updateActiveGuideDetail(initial_guide_detail_id);
        }
    });
    
    _bg->runAction(Sequence::createWithTwoActions(move_ease_out, after_func));
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/slide_popup.mp3");
}

void BeginnerGuideLayer::deleteAction(float dt)
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

void BeginnerGuideLayer::changeChallengePage()
{
    //チャレンジボタンのページ遷移
    auto normal_scene = (NormalScene *)this->getParent();
    string page_name = "Mypage";
    
    //遷移先のページ名の取得
    switch (_active_guide_detail_id)
    {
        case Type::Free_Gacha:
            page_name = "GachaMenu";
            SaveData::saveInt("gacha_tab", 1); //無料ガチャのタブを開く
            break;
        case Type::Friend_Follow:     page_name = "FriendMenu"; break;
        case Type::Deck_Organization: page_name = "CharaMenu"; break;
        case Type::Level_Three:       page_name = "NormalQuest"; break;
        case Type::Level_Eight:       page_name = "NormalQuest"; break;
        case Type::Level_Thirteen:    page_name = "NormalQuest"; break;
        case Type::Event_Easy:        page_name = "EventMenu"; break;
        case Type::Event_Normal:      page_name = "EventMenu"; break;
        case Type::Event_Hard:        page_name = "EventMenu"; break;
        case Type::Online_Quick:      page_name = "BattleTop"; break;
        case Type::Online_Room:       page_name = "BattleRoomTop"; break;
        case Type::Online_Win:        page_name = "BattleTop"; break;
            
        default: break;
    }
    
    //ページ遷移
    normal_scene->changePage(page_name);
    
    //閉じるアクション
    deleteAction(0.0f);
}

bool BeginnerGuideLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch BeginnerGuideLayer!");
    
    return true;
}
void BeginnerGuideLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BeginnerGuideLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    for (int i = 0; i < MAX_STAMP_COUNT; i++) {
        //rectにタップ位置が含まれていれば「初心の心得」の詳細を切り替える
        if (Utility::containsPoint(_panel[i], touch->getLocation())){
            updateActiveGuideDetail(i+1) ;
            break;
        }
    }
}