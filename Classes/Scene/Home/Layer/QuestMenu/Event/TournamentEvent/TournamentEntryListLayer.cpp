#include "TournamentEntryListLayer.h"

TournamentEntryListLayer::TournamentEntryListLayer()
:_cell_size(0)
,_tournament_entry_interval(0)
{
}

TournamentEntryListLayer* TournamentEntryListLayer::create()
{
    TournamentEntryListLayer *sprite = new TournamentEntryListLayer();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEntryListLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TournamentEntryListLayer::init()");
    
    //データの設定
    ratio = getVisibleRatio();
    _cell_size = 350 * ratio;
    
    _tournament_entry_interval = fromString<time_t>(SaveData::loadString("tournament_entry_interval"));
    
    return true;
}

void TournamentEntryListLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("参加トーナメント一覧", "TournamentEventTop", this);
    
    createTournamentListButton();

    
    //開開催しているトーナメントの一覧を取得
    getTournamentEventInfoRequest();
}

void TournamentEntryListLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TournamentEntryListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentEntryListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentEntryListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TournamentEntryListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentEntryListLayer::getTournamentEventInfoRequest()
{
    //トーナメント一覧取得php
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/tournament_event/get_user_entry_tournament_list.php";
    auto selector = httpresponse_selector(TournamentEntryListLayer::tournamentEventInfoCallback);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEntryListLayer::tournamentEventInfoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //トーナメント情報の取得
    _tournament_data_list = json["tournament_data_list"].object_items();
    
    if (countJsonData(_tournament_data_list)) {
        
        //トーナメント一覧の作成
        createTournamentEntryListSprite();
        
        //スクロールの作成
        createScrollView();
        
        //スケジューラの設定
//        schedule(schedule_selector(TournamentEntryListLayer::updateCheckEntryStartDate), 1.0f);
    }
    else {
        
        //現在開催待ちのトーナメントがない場合
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("TournamentEventTop");
        };
        
        string comment = "参加した\nトーナメントはございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
    }
}

void TournamentEntryListLayer::createTournamentEntryListSprite()
{
    //トーナメント一覧の作成
    auto normal_scene = this->getParent();
    int cnt = countJsonData(_tournament_data_list);
    for (int i = 0; i < cnt; i++) {
        
        string iStr = toString<int>(i);

        //トーナメント画像
        _tournament_list.push_back(TournamentEntryListSprite::create(_tournament_data_list[iStr], normal_scene));
        _tournament_list[i]->setTag(i + 1);
    }
}

void TournamentEntryListLayer::createTournamentListButton()
{
    //参加一覧ボタン
    auto entry_list_btn_off = Sprite::create("init/Home/Quest/bt2.png");
    auto entry_list_btn_on  = Sprite::create("init/Home/Quest/bt2.png");
    entry_list_btn_on->setColor(Color3B(180, 180, 180));
    
    auto entry_list_btn = MenuItemSprite::create(entry_list_btn_off, entry_list_btn_on,[this](Ref* sender){
        
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("TournamentList");
    });
    float scaleX = 0.6;
    entry_list_btn->setScale(ratio);
    entry_list_btn->setScaleX(scaleX * ratio);
    
    
    auto menu = Menu::create(entry_list_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    
    auto title_bar_layer = this->getParent()->getChildByTag(TITLE_LAYER_TAG);
    title_bar_layer->addChild(menu, TitleBarLayer::Z_Title_Bar, TitleBarLayer::T_Title_Bar);
    
    
    //文字の表示
    auto entry_list_text_label = HMSFontUtil::createWithTTFBySystemFontArgument("開催中", FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize("開催中", 28, 375));
    entry_list_text_label->setAnchorPoint(Point(0.5, 0.5));
    entry_list_text_label->setPosition(Point(entry_list_btn->getContentSize().width/2, entry_list_btn->getContentSize().height/2));
    entry_list_text_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    entry_list_text_label->cocos2d::Node::setScaleX(1/scaleX);
    
    entry_list_btn->addChild(entry_list_text_label, Z_Popup_Layer, Z_Popup_Layer);

}


void TournamentEntryListLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, 10, SCROLLVIEW_TAG);
    
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _tournament_list.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _tournament_list.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* TournamentEntryListLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    
    //画像の配置
    int num = 0;
    for (int i = _tournament_list.size()-1; 0 <= i; i--)
    {
        _tournament_list[num]->setScale(ratio);
        _tournament_list[num]->setAnchorPoint(Point(0, 0));
        _tournament_list[num]->setPosition(Point((320 - _tournament_list[num]->getContentSize().width / 2) * ratio,  (_cell_size * i)));
        layer->addChild(_tournament_list[num], 1);
        num++;
    }
    
    return layer;
}

void TournamentEntryListLayer::updateCheckEntryStartDate(float dt)
{
    //現在時刻を端末時間からサーバーとの差の時間を引いたものに設定
    time_t now_time;
    time(&now_time);
    now_time = now_time - SaveData::loadInt("server_diff_time");
    
    //エントリーできるかチェック
    for (int i = 0; i < _tournament_list.size(); i++) {
        
        time_t tournament_start_date_timestamp = fromString<time_t>(_tournament_list[i]->getStartDateTimestamp());
        auto entry_btn = (MenuItem *)_tournament_list[i]->getChildByTag(TournamentEntryListSprite::T_Entry_Menu)->getChildByTag(TournamentEntryListSprite::T_Entry_MenuItem);
        
        if ((tournament_start_date_timestamp - _tournament_entry_interval) < now_time && now_time <= tournament_start_date_timestamp && _tournament_list[i]->getConditionMatchFlg()) {
            //トーナメント時間内
            if (_tournament_list[i]->getEntryTimeFlg() == false) {
                entry_btn->unselected();
                _tournament_list[i]->setEntryTimeFlg(true);
                _tournament_list[i]->blinkingAction();
            }

            
        }
        else {
            //エントリー中
            HMSLOG("######id?%d:%d,%d", fromString<int>(_tournament_list[i]->getTournamentEventListId()), fromString<int>(SaveData::loadString("tournament_event_list_id")), fromString<int>(SaveData::loadString("tournament_event_entry")));
            if(_tournament_list[i]->getTournamentEventListId()==SaveData::loadString("tournament_event_list_id")&&SaveData::loadString("tournament_event_entry")=="1"){
                
                if (_tournament_list[i]->getEntryTimeFlg() == false) {
                    entry_btn->unselected();
                    _tournament_list[i]->setEntryTimeFlg(true);
                    _tournament_list[i]->blinkingAction();
                }

            }else{
                //トーナメント時間を超えた場合
                if (_tournament_list[i]->getEntryTimeFlg() == true) {
                    entry_btn->selected();
                    _tournament_list[i]->setEntryTimeFlg(false);
                    _tournament_list[i]->blinkingStopAction();
                }
            }
        }
    }
}

bool TournamentEntryListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void TournamentEntryListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TournamentEntryListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TournamentEntryListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void TournamentEntryListLayer::scrollViewDidScroll(ScrollView *view) {
    
}