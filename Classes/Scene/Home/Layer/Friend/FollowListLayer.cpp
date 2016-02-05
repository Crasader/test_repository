#include "FollowListLayer.h"

FollowListLayer* FollowListLayer::create(string page_name)
{
    FollowListLayer *pRet = new FollowListLayer();
    
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

bool FollowListLayer::init(string page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("FollowListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //ページ名の取得
    _page_name = page_name;
    
    return true;
}

void FollowListLayer::showTitleBar()
{
    if (_page_name == "FollowList") HeaderLayer::setupTitleBar("フォローリスト", "FriendMenu", this);
    else if (_page_name == "FollowerList" ) HeaderLayer::setupTitleBar("フォロワーリスト", "FriendMenu", this);
}

void FollowListLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(FollowListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(FollowListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(FollowListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(FollowListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void FollowListLayer::getFollowListRequest(string page_name)
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "friend/get_follow_info.php";
    auto selector = httpresponse_selector(FollowListLayer::FollowListCallback);
    string body = "user_master_id="+user_master_id+"&page_name="+page_name;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void FollowListLayer::FollowListCallback(HttpClient *sender, HttpResponse *response){
    
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    Json user_follow_list = json["user_follow_list"].object_items();
    
    //jsonの数をチェック
    int count = countJsonData(user_follow_list);
    
    //フレンドスプライトの作成
    initFollowSprite(user_follow_list, count);
    
    
    if (follows.size() > 0)
    {
        //リスト表示の為のスクロールビューを作成
        createScrollView();
    }
    else
    {
        //フレンドがいなかった時の処理
        string message = "フォローがありません\nフレンドメニューに戻ります";
        auto popup_layer = popupChangePageLayer::create(message, "FriendMenu");
        addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}

void FollowListLayer::initFollowSprite(Json json, int count)
{
    //フレンド画像の初期化
    follows.clear();
    
    //フレンド画像の作成
    for (int i = 0; i < count; i++)
    {
        follows.push_back(FollowSprite::create(json, i));
    }
}

void FollowListLayer::createScrollView()
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
    pScrollView->setContentSize(Size(_contentSize.width, cell_size * follows.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * follows.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* FollowListLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 130 * ratio;
    auto layer = Layer::create();
    
    for (int i = (int)follows.size()-1; 0 <= i; i--)
    {
        string num = toString<int>((int)follows.size() - 1 -i);
        
        auto bg = follows[follows.size() - 1 -i];
        bg->setScale(ratio);
        bg->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2)));
        layer->addChild(bg, 1);
    }
    
    return layer;
}

bool FollowListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (follows.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void FollowListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void FollowListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
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
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                //ユーザー情報ページ
                SaveData::saveString("to_user_master_id", follows[tag]->getUserMasterId());
                SaveData::saveString("search_friend_id", follows[tag]->getFriendId());
                changeUserDetail();
            }
            tag++;
        }
    }
}
void FollowListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void FollowListLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらボタンタッチをfalseにする
    touchButtonFlg = false;
}

void FollowListLayer::changeUserDetail()
{
    //ページの遷移を行う
    string next_page_name = "UserDetailFromFollowList";
    if (_page_name == "FollowerList") next_page_name = "UserDetailFromFollowerList";
    
    auto normal_scene = (NormalScene *)this->getParent();    
    normal_scene->changePage(next_page_name);
}

void FollowListLayer::onEnter()
{
    Layer::onEnter();

    //タイトルバーの作成
    showTitleBar();
    
    //イベントリスナーの登録
    initEventListener();

    //通信処理
    getFollowListRequest(_page_name);
}