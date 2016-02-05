#include "TeamListLayerBase.h"
#include "SaveData.h"
#include "HmsUtilityFunction.h"
#include "TeamListSprite.h"

bool TeamListLayerBase::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamListLayerBase::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}


void TeamListLayerBase::onEnter()
{
    Layer::onEnter();
    
//    if(_use_name=="search") {

//    } else if(_use_name=="app") {
//        //ページタイトルの作成
//        HeaderLayer::setupTitleBar("申請中チーム一覧", "TeamNoBelong", this);
//        
//        string team_search_result = SaveData::loadString("team_app_result");
//        string err;
//        Json json = Json::parse(team_search_result, err);
//        _json_team_list = json["team_list"].object_items();
//        SaveData::saveString("team_app_result", "");
//    } else if(_use_name=="soli") {
//        //ページタイトルの作成
//        HeaderLayer::setupTitleBar("勧誘中チーム一覧", "TeamNoBelong", this);
//        
//        string team_search_result = SaveData::loadString("team_soli_result");
//        string err;
//        Json json = Json::parse(team_search_result, err);
//        _json_team_list = json["team_list"].object_items();
//        SaveData::saveString("team_soli_result", "");
//    }
    setHeaderSetting();
    setSpriteTouchAction();
    
    cellSize = 130 * ratio;
    
    GetTeamListData();

}

void TeamListLayerBase::GetTeamListData(){
    string url = HOST;
    url += "team/team_search.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    
    auto selector = httpresponse_selector(TeamListLayerBase::httpCallBack);
    HttpConnect::sendHttpRequest(this, url, selector);
    
    
}

void TeamListLayerBase::httpCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    teamListData = json["team_list"].object_items();
    
    
    
    //リストの生成
    if(countJsonData(this->teamListData)>0){
        
        createListLayer();
        
    } else {
        //ない場合
        string path = "init/Home/Popup/popup_400.png";
        Sprite* errorBackground = Sprite::create(path);
        errorBackground->setScale(ratio);
        errorBackground->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(errorBackground, Z_ScrollView, T_ScrollView);
        
        Label* errorLabel = HMSFontUtil::createWithTTFBySystemFontArgument("表示できるチームがありません", FONT_NAME_SYSTEM, 29);
        //        label2->setScale(ratio);
        errorLabel->setAnchorPoint(Point(0.5, 0.5));
        errorLabel->setPosition(Point(errorBackground->getContentSize().width/2, errorBackground->getContentSize().height/2));
        errorLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        errorBackground->addChild(errorLabel);
        
    }
}

void TeamListLayerBase::createListLayer(){
    
    createPositionIndex();
    //リストを作成
    createScrollView();
    
    setScrollViewData();
}

void TeamListLayerBase::createScrollView()
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


void TeamListLayerBase::setScrollViewData()
{
    //スクロールビューのデータの設定
    ScrollView* scrollView = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scrollView->setContentSize(Size(_contentSize.width, cellSize * teamListSprites.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scrollView->setContentOffset(Point(0, 0 - ((cellSize * teamListSprites.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* TeamListLayerBase::createList()
{
    //スクロールビューの中のレイヤー設定
    Layer* layer = Layer::create();
    layer->setTag(T_ScrollLayer);
    
    //交換キャラリストの一覧の作成
    int dataCount = countJsonData(teamListData);
    for (int i = 0; i < dataCount; i++){
        string iStr = to_string(i);
        
        //交換キャラ画像の作成
        teamListSprites.push_back(TeamListSprite::create(teamListData[iStr], listTouchAction));
        teamListSprites[i]->setScale(ratio);
        layer->addChild(teamListSprites[i]);
    }
    
    //表示
    for (int i = 0; i < dataCount; i++) {
        teamListSprites[i]->setPosition(teamListSpritePositionList[i]);
    }
    
    return layer;
}

void TeamListLayerBase::createPositionIndex()
{
    //ポジションインデックスの作成
    int dataCount = countJsonData(teamListData);
    for (int i = 0; i < dataCount; i++) {
        teamListSpritePositionList.push_back(Point(320 * ratio, (cellSize / 2) + (cellSize * ((dataCount - 1) - i))));
    }
}

void TeamListLayerBase::refreshList(){
    removeChildByTag(T_ScrollView);
    teamListSprites.clear();
    teamListSpritePositionList.clear();
    GetTeamListData();
}

void TeamListLayerBase::InitEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TeamListLayerBase::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamListLayerBase::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamListLayerBase::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamListLayerBase::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool TeamListLayerBase::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void TeamListLayerBase::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamListLayerBase::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamListLayerBase::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

