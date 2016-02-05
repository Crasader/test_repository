#include "EventMenuLayer.h"

bool EventMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("EventMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void EventMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("イベント選択", "QuestMenu", this);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //イベント詳細ボタンの作成
//    createDetailButton();
    
    //イベント情報の取得要求
    httpRequestForEventInfo() ;
}

void EventMenuLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(EventMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(EventMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(EventMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(EventMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void EventMenuLayer::createDetailButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/bt_event_detail.png");
    auto detail_on  = Sprite::create("init/Home/Quest/bt_event_detail.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__event_detail&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント詳細");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, ZORDER_BUTTON);
}

void EventMenuLayer::httpRequestForEventInfo()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/get_event_info.php";
    auto selector = httpresponse_selector(EventMenuLayer::onHttpRequestCompleted);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void EventMenuLayer::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    _event_data = json["event_data"].object_items();
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
        
        string comment = "現在開催している\nイベントはございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
}

void EventMenuLayer::createScrollView()
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
    pScrollView->setContentSize(Size(_contentSize.width, cell_size * _event_sp.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * _event_sp.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* EventMenuLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 200 * ratio;
    auto layer = Layer::create();
    
    int num = 0;
    for (int i = (_event_count - 1); 0 <= i; i--)
    {
        string iStr = toString<int>(num);
        string event_id = _event_data[iStr]["npc_battle_event_id"].string_value();
        
        string image_url = "download/EventMenu/NpcBattleEvent/event" + event_id + "/event_banner_" + event_id + ".png";
        HMSLOG("image_url = %s", image_url.c_str());
        
        
        if (image_url != "") {
            _event_sp.push_back(Sprite::create(image_url));
            _event_sp[num]->setScale(ratio);
            _event_sp[num]->setTag(num);
            _event_sp[num]->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2)));
            layer->addChild(_event_sp[num], 1);
            
            //イベント終了時刻画像の作成
            if(_event_data[iStr]["show_date_flg"].string_value() == "N"){
                auto red_bg = createEventEndDate(_event_data[iStr]["end_date"].string_value());
                red_bg->setScale(0.9);
                red_bg->setPosition(Point(_event_sp[num]->getContentSize().width - 64, 50));
                _event_sp[num]->addChild(red_bg, 1);
            }
            
            //クリア済み画像の表示
            bool clear_flg = _event_data[iStr]["event_list_all_clear_flg"].bool_value();
            if (clear_flg)
            {
                //クリア済みだった場合画像の表示
                auto clear_sp = Sprite::create("init/Home/Quest/clear2.png");
                clear_sp->setPosition(Point(120, _event_sp[num]->getContentSize().height - 44));
                _event_sp[num]->addChild(clear_sp, 1);
            }
        }
        num++;
    }
    
    return layer;
}

Sprite* EventMenuLayer::createEventEndDate(string end_date)
{
    auto red_bg = Sprite::create("init/Home/Quest/event_term.png");
    
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

bool EventMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_event_sp.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void EventMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void EventMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //spriteの絶対座標の値を取得
            auto spWorldPoint = child->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, child->getContentSize().width * ratio, child->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                changePage(child->getTag());
            }
            tag++;
        }
    }
}
void EventMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void EventMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    touchButtonFlg = false;
}

void EventMenuLayer::changePage(int tag)
{
    string iStr = toString<int>(tag);

    auto normal_scene = (NormalScene *)this->getParent();
    
    //選択したイベント難易度のデータの保存
    SaveData::saveString("event_rank_data", _event_data[iStr]["event_rank_list"].dump());
    HMSLOG("event_rank_data = %s", SaveData::loadString("event_rank_data").c_str());
    
    //選択したイベントのIDを保存
    SaveData::saveString("event_id", _event_data[iStr]["npc_battle_event_id"].string_value());
    
    //イベント説明画像のデータの保存
    createEventExplainData(tag);
    
    //難易度選択ページへ遷移
    normal_scene->changePage("EventRankMenu");
}

void EventMenuLayer::createEventExplainData(int tag)
{
    string iStr = toString<int>(tag);
    
    //イベント説明画像のデータの作成
    string npc_battle_event_id = _event_data[iStr]["npc_battle_event_id"].string_value();
    string name                = _event_data[iStr]["name"].string_value();
    string comment             = _event_data[iStr]["comment"].string_value();
    string achieve_count       = toString<int>(_event_data[iStr]["achieve_count"].int_value());
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"npc_battle_event_id\":\"" + npc_battle_event_id+ "\"," + cr;
    data += "  \"name\":\"" + name + "\"," + cr;
    data += "  \"comment\":\"" + comment + "\"," + cr;
    data += "  \"achieve_count\":\"" + achieve_count + "\"" + cr;
    data += "}";
    HMSLOG("event_explain_data = %s", data.c_str()) ;
    
    SaveData::saveString("event_explain_data", data);
}

