#include "WebsocketTestChatList.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "WebsocketTestChatSprite.h"

WebsocketTestChatList* WebsocketTestChatList::create()
{
    WebsocketTestChatList *layer = new WebsocketTestChatList();
    
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool WebsocketTestChatList::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("WebsocketTestChatList::init()");
    
    ratio = getVisibleRatio();
    cellSize = 100 * ratio;
    return true;
}

void WebsocketTestChatList::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    createListLayer();
}

void WebsocketTestChatList::createListLayer(){
    //リストを作成
    createScrollView();
    
    setScrollViewData();
}

void WebsocketTestChatList::createScrollView()
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
    pScrollView->setContainer(createList());
}

void WebsocketTestChatList::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, cellSize * _chatTextList.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    //scroll_view->setContentOffset(Point(0, 0 - ((cellSize * _chatTextList.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
    scroll_view->setContentOffset(Vec2::ZERO);
}

//バザーリストを生成
Layer* WebsocketTestChatList::createList()
{
    //スクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    layer->setTag(T_ScrollLayer);
    
    for_each(_chatTextList.begin(), _chatTextList.end(), [this, layer](ChatTextData* chatTextData){
        WebsocketTestChatSprite* chatSprite = WebsocketTestChatSprite::create(chatTextData->getchatText(), chatTextData->getchatType());
        chatSprite->setScale(ratio);
        layer->addChild(chatSprite);
        _chatSpriteList.push_back(chatSprite);
    });
    
    //表示
    int positionIndex = _chatSpriteList.size() - 1;
    for (int i = 0; i < _chatSpriteList.size(); i++) {
        _chatSpriteList[i]->setPosition(curriculateChatPosition(positionIndex, _chatSpriteList[i]->getIsMine()));
        positionIndex--;
    }
    
    return layer;
}

Vec2 WebsocketTestChatList::curriculateChatPosition(int i, bool isMine){
    float x = this->getContentSize().width - _chatSpriteList[i]->getContentSize().width / 2;
    if(isMine)x = 0 + _chatSpriteList[i]->getContentSize().width / 2;
    
    float y = 50 + cellSize * i;
    return Vec2(x, y);
}


void WebsocketTestChatList::refreshList(){
    removeChildByTag(T_ScrollView);
    _chatSpriteList.clear();
    
    createListLayer();
}

void WebsocketTestChatList::addChatText(string text, bool isMine){
    ChatTextData* chatTextData = ChatTextData::create();
    chatTextData->setchatText(text);
    chatTextData->setchatType(isMine);
    this->addChild(chatTextData);
    _chatTextList.push_back(chatTextData);
    refreshList();
}

void WebsocketTestChatList::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(WebsocketTestChatList::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(WebsocketTestChatList::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(WebsocketTestChatList::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(WebsocketTestChatList::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool WebsocketTestChatList::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void WebsocketTestChatList::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void WebsocketTestChatList::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void WebsocketTestChatList::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

