#include "BazaarListLayer.h"

BazaarListLayer::BazaarListLayer(){}

BazaarListLayer* BazaarListLayer::create(Json bazaarSeachList)
{
    BazaarListLayer *layer = new BazaarListLayer();
    
    if (layer && layer->init(bazaarSeachList))
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool BazaarListLayer::init(Json bazaarSeachList)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarListLayer::init()");
    
    ratio = getVisibleRatio();
    cellSize = 156 * ratio;
    this->bazaarSeachList = bazaarSeachList;
    return true;
}


void BazaarListLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    InitEventListener();

    //if(this->bazaarSeachList != nullptr){
    if(countJsonData(this->bazaarSeachList)>0){
        createListLayer();
    } else {
        //出品キャラがいない場合
        string path = "init/Home/Popup/popup_400.png";
        auto _bg = Sprite::create(path);
        _bg->setScale(ratio);
        _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(_bg, Z_ScrollView, T_ScrollView);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("出品されているキャラはいません。", FONT_NAME_SYSTEM, 29);
//        label2->setScale(ratio);
        label2->setAnchorPoint(Point(0.5, 0.5));
        label2->setPosition(Point(_bg->getContentSize().width/2, _bg->getContentSize().height/2));
        label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        _bg->addChild(label2);
        
    }
//    GetBazaarData();
}
//最新の出品一覧を取得
void BazaarListLayer::GetBazaarData()
{
    GetBazaarData("", 0, 0, 0);
}

void BazaarListLayer::GetBazaarData(string name, int rare, int type, int cost)
{
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "bazaar/get_bazaar_search_list.php";
    auto selector = httpresponse_selector(BazaarListLayer::GetBazaarDataCallBack);
    string body = "user_master_id="+user_master_id;
    body += "&condition_rare="+toString(rare);
    body += "&condition_need_cost="+toString(cost);
    body += "&condition_chara_type="+toString(type);
    body += "&chara_name="+name;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

//最新の出品一覧を取得完了
void BazaarListLayer::GetBazaarDataCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    bazaarSeachList = json["search_list"].object_items();
    
    
    if(countJsonData(bazaarSeachList)){
        createListLayer();
    }else{
        //出品キャラがいない場合
        string path = "init/Home/Popup/popup_400.png";
        auto _bg = Sprite::create(path);
        _bg->setScale(ratio);
        _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(_bg, Z_ScrollView, T_ScrollView);
        
        auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("出品されているキャラはいません。", FONT_NAME_SYSTEM, 29);
//        label2->setScale(ratio);
        label2->setAnchorPoint(Point(0.5, 0.5));
        label2->setPosition(Point(_bg->getContentSize().width/2, _bg->getContentSize().height/2));
        label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        _bg->addChild(label2);
    }
    
}

void BazaarListLayer::createListLayer(){
    
    createPositionIndex();
    //リストを作成
    CreateScrollView();
    
    setScrollViewData();
    checkPurcaseShortage();
}

void BazaarListLayer::CreateScrollView()
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
    pScrollView->setContainer(CreateList());
}

void BazaarListLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, cellSize * bazaarCharaSpriteList.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - ((cellSize * bazaarCharaSpriteList.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

//バザーリストを生成
Layer* BazaarListLayer::CreateList()
{
    //スクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    layer->setTag(T_ScrollLayer);
    
    //交換キャラリストの一覧の作成
    int dataCount = countJsonData(bazaarSeachList);
    for (int i = 0; i < dataCount; i++){
        string iStr = toString<int>(i);
        
        //交換キャラ画像の作成
        bazaarCharaSpriteList.push_back(BazaarCharaSprite::create(bazaarSeachList[iStr], this, false));
        bazaarCharaSpriteList[i]->setScale(ratio);
        layer->addChild(bazaarCharaSpriteList[i]);
    }
    
    //並び替え
    //sortExchangeChara();
    
    //表示
    for (int i = 0; i < dataCount; i++) {
        bazaarCharaSpriteList[i]->setPosition(charaSpritePositionList[i]);
    }
    
    return layer;
}

void BazaarListLayer::createPositionIndex()
{
    //ポジションインデックスの作成
    int dataCount = countJsonData(bazaarSeachList);
    for (int i = 0; i < dataCount; i++) {
        charaSpritePositionList.push_back(Point(320 * ratio, (cellSize / 2) + (cellSize * ((dataCount - 1) - i))));
    }
}

void BazaarListLayer::checkPurcaseShortage()
{
    //交換済み又は所持チケットが足りているかチェックする
    for (int i = 0; i < bazaarCharaSpriteList.size(); i++) {
        string iStr = toString<int>(i);
        BazaarCharaSprite* bazaarCharaSprite = bazaarCharaSpriteList[i];
        
        //所持チケット枚数チェック
        if (fromString<int>(bazaarCharaSprite->getPrice()) > fromString<int>(SaveData::loadString("exchange_ticket"))) bazaarCharaSprite->setUnselectedPurchaseButton();
    }
}

void BazaarListLayer::RefreshList(string name, int rare, int type, int cost){
    removeChildByTag(T_ScrollView);
    bazaarCharaSpriteList.clear();
    charaSpritePositionList.clear();
    GetBazaarData(name, rare, type, cost);
}

void BazaarListLayer::InitEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool BazaarListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void BazaarListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

