#include "BattleFriendChoiceLayer.h"

BattleFriendChoiceLayer* BattleFriendChoiceLayer::create(string page_name)
{
    BattleFriendChoiceLayer *pRet = new BattleFriendChoiceLayer();
    
    if (pRet && pRet->init(page_name))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool BattleFriendChoiceLayer::init(string page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BattleFriendChoiceLayer::init()");
    
    ratio = getVisibleRatio();
    
    //ページ名の取得
    _page_name = page_name;
    
    return true;
}

void BattleFriendChoiceLayer::showTitleBar()
{
    //_page_nameによって戻るページを切り替える
    if (_page_name == "BattleFriendChoiceForOffline"){
        HeaderLayer::setupTitleBar("出陣フレンド選択", "OfflinePage", this);
    }
    else if(_page_name == "BattleFriendChoiceForEvent"){
        HeaderLayer::setupTitleBar("出陣フレンド選択", "EventRankMenu", this);
    }
    else if(_page_name == "BattleFriendChoiceForOnline"){
        HeaderLayer::setupTitleBar("出陣フレンド選択", "BattleDeckChoiceForOnline", this);
    }
    else if(_page_name == "BattleFriendChoiceForQuest"){
        HeaderLayer::setupTitleBar("出陣フレンド選択", "NormalQuestStage", this);
    }
    else if(_page_name == "BattleFriendChoiceForRankingEvent"){
        HeaderLayer::setupTitleBar("出陣フレンド選択", "RankingEventRankMenu", this);
    }
    else if(_page_name == "BattleFriendChoiceForBossEvent"){
        HeaderLayer::setupTitleBar("出陣フレンド選択", "BossEventRankMenu", this);
    }
    else if(_page_name == "BattleFriendChoiceForChallengeEvent"){
        HeaderLayer::setupTitleBar("出陣フレンド選択", "ChallengeEventRankMenu", this);
    }
}

void BattleFriendChoiceLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BattleFriendChoiceLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BattleFriendChoiceLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BattleFriendChoiceLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BattleFriendChoiceLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BattleFriendChoiceLayer::getFollowListRequest()
{
    string user_master_id       = SaveData::loadString("user_master_id");
    string enemy_user_master_id = SaveData::loadString("enemy_user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "battle/get_battle_follow_info.php";
    auto selector = httpresponse_selector(BattleFriendChoiceLayer::FollowListCallback);
    string body = "user_master_id="+user_master_id+"&enemy_user_master_id="+enemy_user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BattleFriendChoiceLayer::FollowListCallback(HttpClient *sender, HttpResponse *response){
    
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    _helper_user_list = json["helper_user_list"].object_items();
    
    //フレンドスプライトの作成
    initFollowSprite();
    
    if (countJsonData(_helper_user_list) > 0)
    {
        //リスト表示の為のスクロールビューを作成
        createScrollView();
    }
    else
    {
        //助っ人が見つからなかった時の処理
        auto popup = popupChangePageLayer::create("通信エラーが発生しました。\n出陣メニューに戻ります。", "QuestMenu");
        addChild(popup, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}

void BattleFriendChoiceLayer::initFollowSprite()
{
    //フレンド画像の初期化
    _helper_cells.clear();
    
    //フレンド画像の作成
    for (int i = 0; i < countJsonData(_helper_user_list); i++)
    {
        string num = toString<int>(i);
        _helper_cells.push_back(HelperSprite::create(_helper_user_list[num].object_items()));
    }
}

//Json BattleFriendChoiceLayer::jsonParseForUserInfoCellSprite(int count)
//{
//    string num = toString<int>(count);
//    
//    string user_master_id    = _helper_user_list[num]["user_master_id"].string_value() ;
//    string friend_id         = _helper_user_list[num]["friend_id"].string_value() ;
//    string card_image_id     = _helper_user_list[num]["friend_card_id"].string_value() ;
//    string user_name         = _helper_user_list[num]["user_name"].string_value() ;
//    string last_play_date    = _helper_user_list[num]["last_play_date"].string_value() ;
//    string comment           = _helper_user_list[num]["comment"].string_value() ;
//    string rank_name         = _helper_user_list[num]["rank_name"].string_value() ;
////    string battle_point_rate = _helper_user_list[num]["battle_point_rate"].string_value() ;
//    
//    string data = "";
//    string cr = "\n";
//    data += "{" + cr;
//    data += "  \"user_master_id\":\"" + user_master_id + "\"," + cr;
//    data += "  \"friend_id\":\"" + friend_id + "\"," + cr;
//    data += "  \"card_image_id\":\"" + card_image_id + "\"," + cr;
//    data += "  \"user_name\":\"" + user_name + "\"," + cr;
//    data += "  \"last_play_date\":\"" + last_play_date + "\"," + cr;
//    data += "  \"comment\":\"" + comment + "\"," + cr;
//    data += "  \"rank_name\":\"" + rank_name + "\"," + cr;
////    data += "  \"battle_point_rate\":\"" + battle_point_rate + "\"" + cr;
//    data += "}";
//    HMSLOG("%s", data.c_str()) ;
//    
//    string err ;
//    Json result = Json::parse(data, err);
//    return result ;
//}

void BattleFriendChoiceLayer::createScrollView()
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
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _helper_cells.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _helper_cells.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* BattleFriendChoiceLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    _cell_size = 138 * ratio;
    auto layer = Layer::create();
    
    int num = 0;
    for (int i = (int)_helper_cells.size()-1; 0 <= i; i--)
    {
        _helper_cells[num]->setScale(ratio);
        _helper_cells[num]->setColor(Color3B(0, 200, 0));
        _helper_cells[num]->setTag(num);
        _helper_cells[num]->setPosition(Point(320 * ratio, (_cell_size / 2) + (_cell_size * i)));
        layer->addChild(_helper_cells[num], 1);
        num++;
    }
    
    return layer;
}

bool BattleFriendChoiceLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_helper_cells.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void BattleFriendChoiceLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BattleFriendChoiceLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (Sprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            
            //詳細表示用の当たり判定
            Rect detail_touch_rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio / 5 , sp->getContentSize().height * ratio);
            //セルの当たり判定
            Rect cell_rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            //キャラ画像をタップで詳細表示
            if (detail_touch_rect.containsPoint(touch->getLocation()))
            {
                //詳細を表示
                showDetail(sp->getTag()) ;
            }
            //セルのどこかをタップしたら次のページへ遷移
            else if (cell_rect.containsPoint(touch->getLocation()))
            {
                changePage(sp->getTag());
                HMSLOG("tag = %d", sp->getTag());
            }
            tag++;
        }
    }
}

void BattleFriendChoiceLayer::showDetail(int tag)
{
    string jsonStr = _helper_cells[tag]->getJson() ;
    string err;
    Json json = Json::parse(jsonStr, err);
    string card_id = json["card_image_id"].string_value() ;
    
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(card_id);
    this->addChild(detail_layer, 999999);
}

void BattleFriendChoiceLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BattleFriendChoiceLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらボタンタッチをfalseにする
    touchButtonFlg = false;
}

void BattleFriendChoiceLayer::saveUseDungeonData(Json json)
{
    //Dungeonシーンで使用するフレンドIDや、Dungeonシーンのリザルトで表示するフレンド情報を保存する
    
    //jsonを解析
    bool follow_flg = json["follow_flg"].bool_value() ;
    string choice_friend_id = json["friend_id"].string_value() ;
    string choice_friend_level = json["level"].string_value() ;
    string choice_friend_rank_name = json["rank_name"].string_value() ;
    string choice_friend_user_name = json["user_name"].string_value() ;
    string choice_friend_comment = json["comment"].string_value() ;
    string choice_friend_card_image_id = json["card_image_id"].string_value() ;
    
    //UserDefaultに保存
    SaveData::saveBool("follow_flg", follow_flg);
    SaveData::saveString("choice_friend_id", choice_friend_id);
    SaveData::saveString("choice_friend_level", choice_friend_level);
    SaveData::saveString("choice_friend_rank_name",choice_friend_rank_name);
    SaveData::saveString("choice_friend_user_name",choice_friend_user_name);
    SaveData::saveString("choice_friend_comment",choice_friend_comment);
    SaveData::saveString("choice_friend_card_image_id",choice_friend_card_image_id) ;
}

void BattleFriendChoiceLayer::changePage(int tag)
{
    string jsonStr = _helper_cells[tag]->getJson() ;
    
    //デッキ編成画面でスプライトを生成するためのjson文字列を保存
    SaveData::saveString("helper_user_sprite_data", jsonStr);
    HMSLOG("helper_user_sprite_data = %s", SaveData::loadString("helper_user_sprite_data").c_str());
    
    //ダンジョンシーンで使用するデータを保存
    string err;
    Json json = Json::parse(jsonStr, err);
    
    
    //ダンジョンシーンで使用するデータを保存
    saveUseDungeonData(json)  ;
    
    //ページの遷移を行う
    auto normal_scene = (NormalScene *)this->getParent();
    
    //_page_nameによって遷移先を切り替える
    if(_page_name == "BattleFriendChoiceForEvent"){
        normal_scene->changePage("BattleDeckChoiceForEvent");
    }
    else if(_page_name == "BattleFriendChoiceForOffline") {
        normal_scene->changePage("BattleDeckChoiceForOffline");
    }
    else if(_page_name == "BattleFriendChoiceForQuest") {
        normal_scene->changePage("BattleDeckChoiceForQuest");
    }
    else if(_page_name == "BattleFriendChoiceForRankingEvent"){
        normal_scene->changePage("BattleDeckChoiceForRankingEvent");
    }
    else if(_page_name == "BattleFriendChoiceForBossEvent"){
        normal_scene->changePage("BattleDeckChoiceForBossEvent");
    }
    else if(_page_name == "BattleFriendChoiceForChallengeEvent"){
        normal_scene->changePage("BattleDeckChoiceForChallengeEvent");
    }
}

void BattleFriendChoiceLayer::onEnter()
{
    Layer::onEnter();

    //タイトルバーの作成
    showTitleBar();
    
    //イベントリスナーの登録
    initEventListener();

    //通信処理
    getFollowListRequest();
}

