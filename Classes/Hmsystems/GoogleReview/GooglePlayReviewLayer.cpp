#include "GooglePlayReviewLayer.h"

GooglePlayReviewLayer::GooglePlayReviewLayer()
:_state(State::ASC_STARS)
,_enable_touch_flg(false)
{
}

GooglePlayReviewLayer* GooglePlayReviewLayer::create()
{
    GooglePlayReviewLayer *node = new GooglePlayReviewLayer();
    if (node && node->init())
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool GooglePlayReviewLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("GooglePlayReviewLayer::init()");
    
    ratio = getVisibleRatio();
    
    _user_master_id = SaveData::loadString("user_master_id");
    
    return true;
}

void GooglePlayReviewLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //背景画像の作成
    createBg();
    
    //星マークの生成
    createStars();
    
    //ボタンの作成
    createButton();
    
    //ラベルの作成
    createLabel() ;
    
    //ポップアップアクション
    popupAction();
}


void GooglePlayReviewLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GooglePlayReviewLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GooglePlayReviewLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GooglePlayReviewLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(GooglePlayReviewLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GooglePlayReviewLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Mypage/review_back.png");
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, O_Popup ,T_Popup);
}

void GooglePlayReviewLayer::createButton()
{
    //ボタンの型の作成
    Rect button_rect = Rect (0, 0, 270, 80) ;
    
    //左のボタンの作成
    Sprite *left_button = Sprite::create() ;
    left_button->setTextureRect(button_rect) ;
    left_button->setOpacity(0) ;
    left_button->setAnchorPoint(Point(0,0)) ;
    
    Sprite *left_button_on = Sprite::create() ;
    left_button_on->setTextureRect(button_rect) ;
    left_button_on->setOpacity(40) ;
    left_button_on->setColor(Color3B::BLUE) ;
    
    auto left_button_menu_item = MenuItemSprite::create(left_button, left_button_on,  CC_CALLBACK_1(GooglePlayReviewLayer::onLeftButton, this)) ;
    
    auto left_menu = Menu::create(left_button_menu_item, nullptr);
    left_menu->setPosition(Point(_bg->getContentSize().width / 4, 46));
    _bg->addChild(left_menu, 1);
    
    
    //右のボタンの作成
    Sprite *right_button = Sprite::create() ;
    right_button->setTextureRect(button_rect) ;
    right_button->setOpacity(0) ;
    right_button->setAnchorPoint(Point(0,0)) ;
    
    Sprite *right_button_on = Sprite::create() ;
    right_button_on->setTextureRect(button_rect) ;
    right_button_on->setOpacity(40) ;
    right_button_on->setColor(Color3B::BLUE) ;
    
    auto right_menu_item = MenuItemSprite::create(right_button, right_button_on, CC_CALLBACK_1(GooglePlayReviewLayer::onRightButton, this)) ;
    
    auto right_menu = Menu::create(right_menu_item, nullptr);
    right_menu->setPosition(Point(_bg->getContentSize().width * 3 / 4, 46));
    _bg->addChild(right_menu, 1);
}

void GooglePlayReviewLayer::createLabel(){
    _left_button_label = HMSFontUtil::createWithTTFBySystemFontArgument("", "", 34);
    _left_button_label->setHorizontalAlignment(TextHAlignment::CENTER);
    _left_button_label->setTextColor(Color4B::BLACK);
    _left_button_label->setVerticalAlignment(TextVAlignment::CENTER);
    _left_button_label->setPosition(Point(_bg->getContentSize().width / 4, 46));
    _bg->addChild(_left_button_label, O_Star) ;
    
    _right_button_label = HMSFontUtil::createWithTTFBySystemFontArgument("", "", 34);
    _right_button_label->setHorizontalAlignment(TextHAlignment::CENTER);
    _right_button_label->setTextColor(Color4B::BLACK);
    _right_button_label->setVerticalAlignment(TextVAlignment::CENTER);
    _right_button_label->setPosition(Point(_bg->getContentSize().width * 3 / 4, 46));
    _bg->addChild(_right_button_label, O_Star) ;
    
    _title_label = HMSFontUtil::createWithTTFBySystemFontArgument("", "", 30);
    _title_label->setHorizontalAlignment(TextHAlignment::CENTER);
    _title_label->setTextColor(Color4B::BLACK);
    _title_label->setVerticalAlignment(TextVAlignment::CENTER);
    _title_label->setPosition(Point(279,234));
    _bg->addChild(_title_label, O_Star) ;
    
    _comment_label = HMSFontUtil::createWithTTFBySystemFontArgument("", "", 24);
    _comment_label->setHorizontalAlignment(TextHAlignment::CENTER);
    _comment_label->setTextColor(Color4B::BLACK);
    _comment_label->setAnchorPoint(Point(0.5,1)) ;
    _comment_label->setVerticalAlignment(TextVAlignment::TOP);
    _comment_label->setDimensions(520, 410);
    _comment_label->setPosition(279,196);
    _bg->addChild(_comment_label, O_Star) ;
    
    //ラベルの更新
    updateLabel() ;
}

void GooglePlayReviewLayer::updateLabel(){
    string title; //題目
    string comment ; //内容
    string left_button ; //左ボタンのラベル
    string right_button ; //右ボタンのラベル
    
    if(_state == ASC_STARS){
        title = "ご感想をお聞かせください。" ;
        comment= "三国志バトルウォーズのご感想をお聞かせください。星をタップして評価してください。" ;
        left_button = "興味なし";
        right_button = "後にする" ;
    }else if(_state == GOOGLE_REVIEW){
        title = "喜んでいただけたら幸いです" ;
        comment= "[続ける]をタップして、このアプリの評価をGoogle Play Storeで共有してください。" ;
        left_button = "興味なし";
        right_button = "続ける" ;
    }else if(_state == HMS_REVIEW){
        title = "ご協力ありがとうございます。" ;
        comment = "三国志バトルウォーズは、お客様のご意見を取り入れて改善に努めてまいります。" ;
        left_button = "興味なし";
        right_button = "評価を送信" ; //(実際にこのボタンを押すまえに、評価の送信は既に終わっている)
    }
    
    _title_label->setString(title) ;
    _comment_label->setString(comment) ;
    _left_button_label->setString(left_button) ;
    _right_button_label->setString(right_button) ;
}

void GooglePlayReviewLayer::onLeftButton(Ref* sender){
    if(_state == ASC_STARS|| _state == GOOGLE_REVIEW || _state == HMS_REVIEW){ //興味無しボタン
        //レビュー要求のフラグをfalseにする
        SaveData::saveBool("review_demand", false);
        
        //ユーザーに不快感を与えないように、今後評価をさせない
        userRefuseReviewRequest() ;
    }
}

void GooglePlayReviewLayer::userRefuseReviewRequest()
{
    //不正アクセスチェックリクエスト
    string url = HOST;
    url += "review/review_refuse.php";
    auto selector = httpresponse_selector(GooglePlayReviewLayer::userRefuseReviewCallback);
    string body = "user_master_id=" + _user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void GooglePlayReviewLayer::userRefuseReviewCallback(HttpClient *sender, HttpResponse *response)
{
    //不正アクセスチェックレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    //ポップアップ閉じる
    deleteAction();
}

void GooglePlayReviewLayer::onRightButton(Ref* sender){
    //レビュー要求のフラグをfalseにする
    SaveData::saveBool("review_demand", false);
    
    if(_state == ASC_STARS){ //後にするボタン
        int add_next_ask_day = 1;
        userReviewResultRequest(0, add_next_ask_day) ;
    }else if(_state == GOOGLE_REVIEW){
        //レビューに飛ばす
        string url ;
        if(RAKUTEN_FLG == 0){
            url = "https://play.google.com/store/apps/details?id=jp.pkga.war&hl=ja";
        }else if(RAKUTEN_FLG == 1){
            url = "https://apps.rakuten.co.jp/detail/?appId=93";
        }
        
        NativeBridge::openURL(url.c_str());
        
        //ポップアップ閉じる
        deleteAction();
        
    }else if(_state == HMS_REVIEW){
        //ポップアップ閉じる
        deleteAction();
    }
}


void GooglePlayReviewLayer::createStars(){
    
    //中心位置とスタンプ間の距離(中心同士)を定義
    Point center = Point(279, 340) ;
    Point distance = Point(95, 0);
    
    //0番目の中心座標を計算
    float stamp_pos_x_0 = center.x - distance.x * 2 ;
    float stamp_pos_y_0 = center.y ;
    
    for (int i = 0; i < 5; i++) {
        Sprite *star = Sprite::create("init/Home/Mypage/review_star2.png") ;
        star->setPosition(Point(stamp_pos_x_0 + (i * distance.x), stamp_pos_y_0));
        _bg->addChild(star,O_Star) ;
        _stars.push_back(star) ;
    }
    
}

void GooglePlayReviewLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3");
}

void GooglePlayReviewLayer::deleteAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == T_Popup)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    
    //SE
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
}

bool GooglePlayReviewLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch GooglePlayReviewLayer!");
    if(_enable_touch_flg)return true;
    
    for(int i=0 ; i<5 ; i++){
        if(Utility::containsPoint(_stars[i], touch->getLocation())) {
            _touch_begin_star_index = i ;
            break ;
        }
    }
    
    return true;
}
void GooglePlayReviewLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void GooglePlayReviewLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_enable_touch_flg)return ;
    
    for(int i=0 ; i<5 ; i++){
        if(Utility::containsPoint(_stars[i], touch->getLocation())) {
            if(_touch_begin_star_index == i){ //開始時にタップしていた星と、終了時にタップしていた星が一致している場合、タップをした判定をする
                onTouchStar(i) ;
            }
            break ;
        }
    }
    
}
void GooglePlayReviewLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}


void GooglePlayReviewLayer::onTouchStar(int index){
    _enable_touch_flg = true;
    int review_point = index + 1 ;
    for(int i=0 ; i<review_point ; i++){
        _stars[i]->setTexture("init/Home/Mypage/review_star.png") ;
    }
    
    if(review_point >= 4){
        _state = State::GOOGLE_REVIEW;
    }else if(review_point < 4) {
        _state = State::HMS_REVIEW;
    }
    
    //次にレビューを求める日までの日数
    int add_next_ask_day = 1000 ;
    userReviewResultRequest(review_point, add_next_ask_day) ;
}

void GooglePlayReviewLayer::userReviewResultRequest(int review_point, int add_next_ask_day)
{
    //不正アクセスチェックリクエスト
    string url = HOST;
    url += "review/send_review.php";
    auto selector = httpresponse_selector(GooglePlayReviewLayer::userReviewResultCallback);
    string body = "user_master_id=" + _user_master_id + "&star_count=" + toString<int>(review_point) + "&add_next_ask_day=" + toString<int>(add_next_ask_day);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void GooglePlayReviewLayer::userReviewResultCallback(HttpClient *sender, HttpResponse *response)
{
    //不正アクセスチェックレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    if(_state == State::GOOGLE_REVIEW || _state == State::HMS_REVIEW){
        //テキストを更新する
        updateLabel() ;
    }else{
        //ポップアップ閉じる
        deleteAction();
    }
}