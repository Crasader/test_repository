#include "WebViewLoadLayer.h"

WebViewLoadLayer* WebViewLoadLayer::create(string url, string page_title)
{
    WebViewLoadLayer *node = new WebViewLoadLayer();
    if (node && node->init(url, page_title))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool WebViewLoadLayer::init(string url, string page_title)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("WebViewLoadLayer::init()");
    
    ratio = getVisibleRatio();
    
    if(IF_ANDROID && RAKUTEN_FLG){
        //楽天のときのみパラメータを付加する
        url += "&is_rakuten=1";
    }
    
    //メンバ変数に保存
    _url = url;
    _page_title = page_title;
    
    return true;
}

void WebViewLoadLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //背景の作成
    createBg();
    
    //ローディング画面表示
    createLoading();
    
    //タイトルの作成
    createTitle();
    
    //ポップアップアクション
    popupAction();
    
}

void WebViewLoadLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(WebViewLoadLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(WebViewLoadLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(WebViewLoadLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(WebViewLoadLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    _first_load = true;
}

void WebViewLoadLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_800.png");
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, 2 ,POPUP_ACTION_TAG);
}

void WebViewLoadLayer::createLoading(){
    Rect rect = Rect(0, 0, _bg->getContentSize().width - 20, _bg->getContentSize().height - 20);
    _background_black = Sprite::create();
    _background_black->setColor(Color3B::BLACK);
    _background_black->setTextureRect(rect);
    _background_black->setPosition(_bg->getContentSize()/2) ;
//	showLoadingEffect(_background_black, "init/Loading/Sprite/loading.plist",_bg->getContentSize()/2, 1.0f); //ローディング中のアニメを表示
    _bg->addChild(_background_black);
}

void WebViewLoadLayer::createTitle()
{
    //ページのタイトル画像
    _title_bg = Sprite::create("init/Home/Popup/popup_index_s.png");
    _title_bg->setScale(0);
    _title_bg->setPosition(convertFromDisplaySize(Point(320, 900), 0.5, 0.5));
    this->addChild(_title_bg, 3, POPUP_ACTION_TAG);
    
    //ページタイトル
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument(_page_title, FONT_NAME_SYSTEM, 30);
    title_label->setPosition(_title_bg->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    _title_bg->addChild(title_label, 2);
    
    //閉じるボタン
    auto close_off = Sprite::create("init/Home/Popup/bt_close.png");
    auto close_on  = Sprite::create("init/Home/Popup/bt_close.png");
    close_on->setColor(Color3B(180, 180, 180));
    
    auto close_btn = MenuItemSprite::create(close_off, close_on, [&](Ref* pSender)
    {
        //閉じるアクション
        deleteAction();
    });
    close_btn->setScale(0.7);
    
    auto menu = Menu::create(close_btn, nullptr);
    menu->setPosition(Point(_title_bg->getContentSize().width - 30, _title_bg->getContentSize().height / 2));
    _title_bg->addChild(menu, 3);
    
}

void WebViewLoadLayer::createWebView()
{
    //ウェブビューの作成
    auto webView = cocos2d::experimental::ui::WebView::create();
    webView->setAnchorPoint(Point(0.5, 0.5));
    webView->setContentSize(Size(_bg->getContentSize().width - 20, _bg->getContentSize().height - 20));
    webView->setPosition(Point(_bg->getContentSize() / 2));
    webView->loadURL(_url);
    webView->setScalesPageToFit(true);
    _bg->addChild(webView, 1);
    HMSLOG("_url = %s", _url.c_str());
    
    //ページロード開始時のコールバックをラムダ式でセット
    webView->setOnShouldStartLoading([this](experimental::ui::WebView *sender, const std::string &url)->bool{
         //初回ページロード開始時は画面を暗くする(webviewを不可視化する ※ページロード開始前に不可視だと上手く描画されないため、このタイミングで不可視にする)
        if(_first_load){
            sender->setVisible(false);
        }
        return true;
    });
    
    //ページロード完了時のコールバックをラムダ式でセット(webviewを可視化)
    webView->setOnDidFinishLoading([this](experimental::ui::WebView *sender, const std::string &url){
        //初回ページロード完了時にwebviewを可視化する
        if(_first_load){
            sender->setVisible(true);
            _background_black->setVisible(false) ;
            _first_load = false ;
        }
    });
    
    // webviewからアプリを操作するための設定
    webView->setJavascriptInterfaceScheme("ccjs");
    webView->setOnJSCallback(CC_CALLBACK_2(WebViewLoadLayer::onJSCallback, this));
}

void WebViewLoadLayer::onJSCallback(experimental::ui::WebView *sender, const std::string &url){
        if(url.find("ccjs://openUrl", 0) != string::npos){
            string after_scheme_place = eraseSchemePlace("ccjs://openUrl:", url) ;
            
            string http_url = "http://" + after_scheme_place ;
            NativeBridge::openURL(http_url.c_str());
        }else if(url.find("ccjs://friendDemand:", 0) != string::npos){
            string after_scheme_place = eraseSchemePlace("ccjs://friendDemand:", url) ;
            
            //閉じるアクション
            deleteAction();
            
            //友達申請画面に遷移
            SaveData::saveString("search_friend_id", after_scheme_place);
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("UserDetailFromFriendSearch");
        }else if(url.find("ccjs://goBack", 0) != string::npos){
            //閉じるアクション
            deleteAction();
        }else if(url.find("ccjs://goBack", 0) != string::npos){
            //戻る
            sender->goBack() ;
        }else if(url.find("ccjs://goForward", 0) != string::npos){
            //進む
            sender->goForward() ;
        }else if(url.find("ccjs://reload", 0) != string::npos){
            //進む
            sender->reload() ;
        }
}

string WebViewLoadLayer::eraseSchemePlace(string scheme_place, string url)
{
    url.erase(0, scheme_place.size()) ; // オリジナルのスキーム部分 ccjs://の部分を取り除く
    return url ;
}

void WebViewLoadLayer::showLoadingEffect(Node *parent_node, string filename,const Point& center_position, float scale)
{
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename);
    
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
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.06f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    auto sprite = Sprite::createWithSpriteFrame(animFrames.front());
    sprite->setScale(scale);
    sprite->setPosition(center_position);
    parent_node->addChild(sprite, 10000);
    
    //永続的に発生させる
    auto repeat = RepeatForever::create(animate);
    sprite->runAction(repeat) ;
}


void WebViewLoadLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale  = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
    
    auto func = CallFunc::create([this](){
        //webView表示
        createWebView();
    });
    
    //開く音
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3",false);
    
    this->runAction(Sequence::create(DelayTime::create(0.2), func, nullptr));
}

void WebViewLoadLayer::deleteAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == POPUP_ACTION_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    
    //閉じる音
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
    
}

bool WebViewLoadLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("Touch WebViewLoadLayer!");
    return true;
}

void WebViewLoadLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void WebViewLoadLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void WebViewLoadLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}