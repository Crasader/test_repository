#include "TeamBattleEventItemExchangeLayer.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HttpConnect.h"
#include "json11.hpp"
#include "HeaderLayer.h"

bool TeamBattleEventItemExchangeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamBattleEventItemExchangeLayer::init()");
    
    ratio = getVisibleRatio();
    _cell_size = 156 * ratio;
    _event_item = fromString<int>(SaveData::loadString("team_battle_event_item"));
    
    return true;
}

void TeamBattleEventItemExchangeLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("イベントアイテム交換所", "ShopMenu", this);
    
    //イベントアイテム所持数
    createEventItem();
    
    //交換アイテムデータの取得
    getExchangeItemInfoRequest();
}

void TeamBattleEventItemExchangeLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TeamBattleEventItemExchangeLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamBattleEventItemExchangeLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamBattleEventItemExchangeLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamBattleEventItemExchangeLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TeamBattleEventItemExchangeLayer::createEventItem()
{
    //所持イベントアイテム数表示
    auto bg = Sprite::create("init/CharaMenu/revision/chara_exchange/ticket_bk.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(bg, 1, T_Event_Item_Bg);
    
    auto team_battle_event_item_count = HMSFontUtil::createWithTTFBySystemFontArgument(toString(_event_item), FONT_NAME_SYSTEM , Utility::calcFlexibleFontSize(toString(_event_item), 36, 100));
    team_battle_event_item_count->setAnchorPoint(Point(1.0, 0.5));
    team_battle_event_item_count->setPosition(Point(bg->getContentSize().width - 20, bg->getContentSize().height / 2));
    bg->addChild(team_battle_event_item_count, 1, T_Event_Item_Label);
}

void TeamBattleEventItemExchangeLayer::getExchangeItemInfoRequest()
{
    //交換キャラデータの取得
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/team_battle_event/top_scene/get_exchange_reward_info.php";
    auto selector = httpresponse_selector(TeamBattleEventItemExchangeLayer::exchangeItemInfoCallback);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TeamBattleEventItemExchangeLayer::exchangeItemInfoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //交換キャラの情報の取得
    _exchange_item_data_list = json["exchange_item"].object_items();
    
    if (countJsonData(_exchange_item_data_list)) {
        
        //ポジションインデックス
        createPositionIndex();
        
        //交換アイテムリスト作成
        createScrollView();
        
        setScrollViewData();
        
        //交換済みのチェック
        checkExchangedItemAndShortage();
    }
    else {
        
        //交換アイテムがない場合
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("ShopMenu");
        };
        
        string comment = "現在交換できるアイテムは\nございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
    }
    
}

void TeamBattleEventItemExchangeLayer::checkExchangedItemAndShortage()
{
    //交換済み又は所持イベントアイテムが足りているかチェックする
    for (int i = 0; i < _exchange_item_sp.size(); i++) {
        string iStr = toString<int>(i);
        
        //交換済み
        if (_exchange_item_sp[i]->getExchangeFlg()) _exchange_item_sp[i]->setExchangedItem();
        
        //所持イベントアイテム数チェック
        if (fromString<int>(_exchange_item_sp[i]->getNeedExchangeItem()) > _event_item) _exchange_item_sp[i]->setUnselectedExchangeButton();
    }
}

void TeamBattleEventItemExchangeLayer::createPositionIndex()
{
    //ポジションインデックスの作成
    int data_count = countJsonData(_exchange_item_data_list);
    for (int i = 0; i < data_count; i++) {
        _vec_position_index.push_back(Point(320 * ratio, (_cell_size / 2) + (_cell_size * ((data_count - 1) - i))));
    }
}

void TeamBattleEventItemExchangeLayer::createScrollView()
{
    //スクロールビューの作成
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, Z_ScrollView, T_ScrollView);
    
    //スクロールの中身を作成
    pScrollView->setContainer(createScrollList());
}

void TeamBattleEventItemExchangeLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, _cell_size * _exchange_item_sp.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - ((_cell_size * _exchange_item_sp.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* TeamBattleEventItemExchangeLayer::createScrollList()
{
    //スクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    layer->setTag(T_ScrollLayer);
    
    //交換キャラリストの一覧の作成
    int data_count = countJsonData(_exchange_item_data_list);
    for (int i = 0; i < data_count; i++)
    {
        string iStr = toString<int>(i);
        
        //交換キャラ画像の作成
        _exchange_item_sp.push_back(TeamBattleEventItemExchangeSprite::create(_exchange_item_data_list[iStr], this));
        _exchange_item_sp[i]->setScale(ratio);
        layer->addChild(_exchange_item_sp[i]);
    }
    
    //並び替え
    sortExchangeItem();
    
    //表示
    for (int i = 0; i < data_count; i++) {
        _exchange_item_sp[i]->setPosition(_vec_position_index[i]);
    }
    
    return layer;
}

void TeamBattleEventItemExchangeLayer::sortExchangeItem()
{
    //交換済みが下にくるように並び替え
    sort(_exchange_item_sp.begin(), _exchange_item_sp.end(), [](const TeamBattleEventItemExchangeSprite* item1, const TeamBattleEventItemExchangeSprite* item2){
        return (item2->getExchangeFlg());
    });
}

void TeamBattleEventItemExchangeLayer::exchangeRequest(string reward_exchange_id)
{
    string url = HOST;
    url += "event/team_battle_event/top_scene/reward_exchange_item.php";
    string body = "user_master_id=" + SaveData::loadString("user_master_id");
    body += "&reward_exchange_id=" + reward_exchange_id;
    auto selector = httpresponse_selector(TeamBattleEventItemExchangeLayer::exchangeRequestCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TeamBattleEventItemExchangeLayer::exchangeRequestCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //共通処理
    function<void()> func = [this, json](){/*アクションなし*/}; //ポップアップでコメント出した後の動き
    string after_event_item = json["after_event_item"].string_value();
    _event_item = fromString<int>(after_event_item);
    SaveData::saveString("team_battle_event_item", after_event_item);
    //チケット数表示の更新
    auto exchange_ticket_label = (Label *)this->getChildByTag(T_Event_Item_Bg)->getChildByTag(T_Event_Item_Label);
    exchange_ticket_label->setString(after_event_item);
    
    //交換成功
    if(json["exchange_flg"].bool_value()){
        //プレゼント数の更新（+1する）
        int present_count = SaveData::loadInt("present_count") + 1;
        SaveData::saveInt("present_count", present_count);
        
        //今回獲得した報酬の情報を更新
        for (int i = 0; i < _exchange_item_sp.size(); i++) {
            if (_exchange_item_sp[i]->getPrimaryId() == json["reward_exchange_id"].string_value()) {
                //今回獲得した報酬の残り交換回数を更新
                _exchange_item_sp[i]->exchangeCountRemainDecrement();
                
                //今回獲得した報酬の残り交換回数が０だったら交換済みにして順番を並び替える
                if (json["is_exchange_finish"].bool_value()){
                    //交換済みに変更
                    _exchange_item_sp[i]->setExchangeFlg(true);
                    _exchange_item_sp[i]->setExchangedItem();
                    //順番を並び替える
                    func = [this, json](){
                        //データの並び替え（交換済みを下にする）
                        this->sortExchangeItem();
                        
                        //表示位置の変更アクション
                        this->actionUpdatePage();
                    };
                }
            }
            
            //所持イベントアイテム数チェック
            if (fromString<int>(_exchange_item_sp[i]->getNeedExchangeItem()) > _event_item) _exchange_item_sp[i]->setUnselectedExchangeButton();
        }
    }
    
    string message =  json["exchange_text"].string_value();
    auto popup_layer = PopupOkButtonLayer::create(func, message);
    this->addChild(popup_layer, 999999);
}

void TeamBattleEventItemExchangeLayer::actionUpdatePage()
{
    //ページ更新の際のアクション
    for (int i = 0; i < _exchange_item_sp.size(); i++)
    {
        auto move = MoveTo::create(0.3f, _vec_position_index[i]);
        _exchange_item_sp[i]->runAction(move);
    }
}

bool TeamBattleEventItemExchangeLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void TeamBattleEventItemExchangeLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamBattleEventItemExchangeLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamBattleEventItemExchangeLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamBattleEventItemExchangeLayer::scrollViewDidScroll(ScrollView *view)
{
}