#include "ChallengeEventMenuLayer.h"

bool ChallengeEventMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ChallengeEventMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void ChallengeEventMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("試練に挑戦", "NormalQuest", this);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //イベント詳細ボタンの作成
//    createDetailButton();
    
    createHelpButton() ;
    
    //イベント情報の取得要求
    httpRequestForChallengeEventInfo() ;
}

void ChallengeEventMenuLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(ChallengeEventMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ChallengeEventMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ChallengeEventMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ChallengeEventMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void ChallengeEventMenuLayer::createDetailButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/bt_ChallengeEvent_detail.png");
    auto detail_on  = Sprite::create("init/Home/Quest/bt_ChallengeEvent_detail.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__ChallengeEvent_detail&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント詳細");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, ZORDER_BUTTON);
}

void ChallengeEventMenuLayer::createHelpButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto detail_on  = Sprite::create("init/Home/Quest/bt_help.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__challenge_event_help&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ヘルプ");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, ZORDER_BUTTON);
}

void ChallengeEventMenuLayer::httpRequestForChallengeEventInfo()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/get_challenge_event_info.php";
    auto selector = httpresponse_selector(ChallengeEventMenuLayer::onHttpRequestCompleted);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void ChallengeEventMenuLayer::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    _event_data = json["challenge_event_data"].object_items();
    _event_count = countJsonData(_event_data);
    
    if (_event_count >= 1)
    {
        //----------- イベント開催時 -------------
        
        //スクロールビューの作成
        createScrollView();
    }
    else
    {
        //---------- イベント未開催時 ------------
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("QuestMenu");
        };
        
        string comment = "挑戦できる試練がありません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
}

void ChallengeEventMenuLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio)- (FOOTER_SIZE * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, 1, SCROLLVIEW_TAG);
    
    //PresentMenuのレイヤーを戻り値に指定して使う
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, cell_size * _banner_sprite.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * _banner_sprite.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* ChallengeEventMenuLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 200 * ratio;
    auto layer = Layer::create();
    
    int num = 0;
    for (int i = (_event_count - 1); 0 <= i; i--)
    {
        string iStr = toString<int>(num);
        string challenge_event_id = _event_data[iStr]["challenge_event_id"].string_value();
        
        string image_url = "download/EventMenu/ChallengeEvent/event" + challenge_event_id + "/event_banner_" + challenge_event_id + ".png";
        HMSLOG("image_url = %s", image_url.c_str());
        BannerSprite *banner = BannerSprite::create(image_url) ;
        banner->addDisplayEndDate(_event_data[iStr]["end_date"].string_value()) ;
        
        if (image_url != "") {
            _banner_sprite.push_back(banner);
            _banner_sprite[num]->setScale(ratio);
            _banner_sprite[num]->setTag(num);
            _banner_sprite[num]->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(_banner_sprite[num], 1);
            
            //クリア済み画像の表示
            bool clear_flg = _event_data[iStr]["all_clear_flg"].bool_value();
            if (clear_flg)
            {
                //クリア済みだった場合画像の表示
                auto clear_sp = Sprite::create("init/Home/Quest/clear2.png");
                clear_sp->setPosition(Point(250, banner->getContentSize().height - 34));
                banner->addChild(clear_sp, 1);
            }
        }
        num++;
    }
    
    return layer;
}

bool ChallengeEventMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *ChallengeEvent)
{
    if (_banner_sprite.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void ChallengeEventMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *ChallengeEvent)
{
}
void ChallengeEventMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *ChallengeEvent)
{
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            BannerSprite *banner_sprite = (BannerSprite*)child ;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = banner_sprite->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, banner_sprite->getContentSize().width * ratio, banner_sprite->getContentSize().height * ratio);
            
            //イベントが終了していない、且つタップ位置がバナーの範囲内のとき
            if (!banner_sprite->getEventEnd() && rect.containsPoint(touch->getLocation()))
            {
                changePage(banner_sprite->getTag());
            }
            tag++;
        }
    }
}
void ChallengeEventMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *ChallengeEvent)
{
}

void ChallengeEventMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    touchButtonFlg = false;
}

void ChallengeEventMenuLayer::changePage(int tag)
{
    string iStr = toString<int>(tag);

    auto normal_scene = (NormalScene *)this->getParent();
    
    //選択したイベント難易度のデータの保存
    SaveData::saveString("challenge_event_rank_data", _event_data[iStr]["challenge_event_stage"].dump());
    HMSLOG("challenge_event_rank_data = %s", SaveData::loadString("challenge_event_rank_data").c_str());
    
    //選択したイベントのIDを保存
    SaveData::saveString("challenge_event_id", _event_data[iStr]["challenge_event_id"].string_value());
    
    //イベント説明画像のデータの保存
    createChallengeEventExplainData(tag);
    
    //難易度選択ページへ遷移
    normal_scene->changePage("ChallengeEventRankMenu");
}

void ChallengeEventMenuLayer::createChallengeEventExplainData(int tag)
{
    string iStr = toString<int>(tag);
    
    //イベント説明画像のデータの作成
    string challenge_event_id  = _event_data[iStr]["challenge_event_id"].string_value();
    string name                = _event_data[iStr]["name"].string_value();
    string comment             = _event_data[iStr]["comment"].string_value();
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"challenge_event_id\":\"" + challenge_event_id + "\"," + cr;
    data += "  \"name\":\"" + name + "\"," + cr;
    data += "  \"comment\":\"" + comment + "\"" + cr;
    data += "}";
    HMSLOG("challenge_event_explain_data = %s", data.c_str()) ;
    
    SaveData::saveString("challenge_event_explain_data", data);
}

