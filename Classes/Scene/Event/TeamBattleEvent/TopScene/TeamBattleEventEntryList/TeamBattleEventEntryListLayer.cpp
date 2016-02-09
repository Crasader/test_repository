#include "TeamBattleEventEntryListLayer.h"
#include "HeaderLayer.h"

TeamBattleEventEntryListLayer::TeamBattleEventEntryListLayer()
:_cell_size(0)
,_team_battle_entry_interval(0)
{
}

TeamBattleEventEntryListLayer* TeamBattleEventEntryListLayer::create()
{
    TeamBattleEventEntryListLayer *layer = new TeamBattleEventEntryListLayer();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool TeamBattleEventEntryListLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamBattleEventEntryListLayer::init()");
    
    //データの設定
    ratio = getVisibleRatio();
    _cell_size = 350 * ratio;
    
    _team_battle_entry_interval = fromString<time_t>(SaveData::loadString("team_battle_entry_interval"));
    
    return true;
}

void TeamBattleEventEntryListLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チーム対抗戦一覧", "TeamBattleEventTop", this);

    //開催しているチーム対抗戦の一覧を取得
    getTeamBattleEventInfoRequest();
}

void TeamBattleEventEntryListLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TeamBattleEventEntryListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamBattleEventEntryListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamBattleEventEntryListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamBattleEventEntryListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TeamBattleEventEntryListLayer::getTeamBattleEventInfoRequest()
{
    //チーム対抗戦一覧取得php
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/team_battle_event/top_scene/get_team_battle_event_info.php";
    auto selector = httpresponse_selector(TeamBattleEventEntryListLayer::teamBattleEventInfoCallback);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TeamBattleEventEntryListLayer::teamBattleEventInfoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //チーム対抗戦情報の取得
    _team_battle_data_list = json["team_battle_data_list"].object_items();
    
    if (countJsonData(_team_battle_data_list)) {
        
        //チーム対抗戦一覧の作成
        createTeamBattleEntryListSprite();

        //スクロールの作成
        createScrollView();
        
        TeamBattleEventEntryListLayer::updateCheckEntryStartDate(0.0f);
        
        //スケジューラの設定
        schedule(schedule_selector(TeamBattleEventEntryListLayer::updateCheckEntryStartDate), 1.0f);
    }
    else {
        
        //現在開催待ちのチーム対抗戦がない場合
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("TeamBattleEventTop");
        };
        
        string comment = "現在開催待ちの\nチーム対抗戦はございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
    }
}

void TeamBattleEventEntryListLayer::entryFinish(string _team_battle_event_list_id)
{
    //チーム対抗戦参加通信処理
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/team_battle_event/top_scene/team_battle_entry.php";
    auto selector = httpresponse_selector(TeamBattleEventEntryListLayer::entryFinishCallBack);
    string body = "user_master_id=" + user_master_id + "&team_battle_event_list_id=" + _team_battle_event_list_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TeamBattleEventEntryListLayer::entryFinishCallBack(HttpClient *sender, HttpResponse *response)
{
    //チーム対抗戦
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    Json _entry_data_list = json["entry_date_list"].object_items();
    
    auto normal_scene = (NormalScene *)this->getParent();
    function<void()> ok_func;
    //エントリー成功
    if(_entry_data_list["is_entry"].bool_value()){
        ok_func = [normal_scene](){
            normal_scene->changePage("TeamBattleEventTop");
        };
    }
    //エントリー失敗
    else{
        ok_func = [](){/*アクションなし*/};
    }
    auto popup_layer = PopupOkButtonLayer::create(ok_func, _entry_data_list["entry_text"].string_value());
    this->addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
}

void TeamBattleEventEntryListLayer::createTeamBattleEntryListSprite()
{
    //チーム対抗戦一覧の作成
    for (int i = 0; i < countJsonData(_team_battle_data_list); i++) {
        
        string iStr = toString<int>(i);

        //チーム対抗戦画像
        _team_battle_list.push_back(TeamBattleEventEntryListSprite::create(_team_battle_data_list[iStr], this));
        _team_battle_list[i]->setTag(i + 1);
    }
}

void TeamBattleEventEntryListLayer::createScrollView()
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
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _team_battle_list.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _team_battle_list.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* TeamBattleEventEntryListLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    
    //画像の配置
    int num = 0;
    for (int i = _team_battle_list.size()-1; 0 <= i; i--)
    {
        _team_battle_list[num]->setScale(ratio);
        _team_battle_list[num]->setAnchorPoint(Point(0, 0));
        _team_battle_list[num]->setPosition(Point((320 - _team_battle_list[num]->getContentSize().width / 2) * ratio,  (_cell_size * i)));
        layer->addChild(_team_battle_list[num], 1);
        num++;
    }
    
    return layer;
}

void TeamBattleEventEntryListLayer::updateCheckEntryStartDate(float dt)
{
    //現在時刻を端末時間からサーバーの差の時間を引いたものに設定
    time_t now_time;
    time(&now_time);
    now_time = now_time - SaveData::loadInt("server_diff_time");
    
    //エントリーできるかチェック
    for (int i = 0; i < _team_battle_list.size(); i++) {
        
        time_t team_battle_start_date_timestamp = fromString<time_t>(_team_battle_list[i]->getStartDateTimestamp());
        auto entry_btn = (MenuItem *)_team_battle_list[i]->getChildByTag(TeamBattleEventEntryListSprite::T_Entry_Menu)->getChildByTag(TeamBattleEventEntryListSprite::T_Entry_MenuItem);
        
        if ((team_battle_start_date_timestamp - _team_battle_entry_interval) < now_time && now_time <= team_battle_start_date_timestamp) {
            //チーム対抗戦時間内
            if (_team_battle_list[i]->getEntryTimeFlg() == false) {
                entry_btn->unselected();
                _team_battle_list[i]->setEntryTimeFlg(true);
                _team_battle_list[i]->blinkingAction();
            }

            
        }
        else {
            //エントリー中
            HMSLOG("######id?%d:%d,%d", fromString<int>(_team_battle_list[i]->getTeamBattleEventListId()), fromString<int>(SaveData::loadString("team_battle_event_list_id")), fromString<int>(SaveData::loadString("team_battle_event_entry")));
            
            _team_battle_list[i]->checkEntryTime();
            if(_team_battle_list[i]->getTeamBattleEventListId()==SaveData::loadString("team_battle_event_list_id")&&SaveData::loadString("team_battle_event_entry")=="1"){
                
                if (_team_battle_list[i]->getEntryTimeFlg() == false) {
                    entry_btn->unselected();
                    _team_battle_list[i]->setEntryTimeFlg(true);
                    _team_battle_list[i]->blinkingAction();
                }

            }else{
                //チーム対抗戦時間を超えた場合
                if (_team_battle_list[i]->getEntryTimeFlg() == true) {
                    entry_btn->selected();
                    _team_battle_list[i]->setEntryTimeFlg(false);
                    _team_battle_list[i]->blinkingStopAction();
                }
            }
        }
    }
}

bool TeamBattleEventEntryListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){return true;}
void TeamBattleEventEntryListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){}
void TeamBattleEventEntryListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event){}
void TeamBattleEventEntryListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){}
void TeamBattleEventEntryListLayer::scrollViewDidScroll(ScrollView *view){}