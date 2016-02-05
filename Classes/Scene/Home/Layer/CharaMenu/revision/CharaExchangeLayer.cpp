#include "CharaExchangeLayer.h"

CharaExchangeLayer::CharaExchangeLayer()
:_cell_size(0)
,_exchange_ticket(0)
{
}

bool CharaExchangeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaExchangeLayer::init()");
    
    ratio = getVisibleRatio();
    
    //初期データの設定
    _exchange_ticket = fromString<int>(SaveData::loadString("exchange_ticket"));
    _cell_size = 156 * ratio;
    
    return true;
}

void CharaExchangeLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("キャラ交換", "CharaMenu", this, HeaderLayer::TitleBarType::WIDTH_324);
    
    //所持チケット枚数の表示
    showExchangeTicketSprite();
    
    //キャラ交換ヘルプ
    createHelpButton();
    
    //交換キャラデータの取得
    getExchangeCharaInfoRequest();
}

void CharaExchangeLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CharaExchangeLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaExchangeLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaExchangeLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaExchangeLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaExchangeLayer::showExchangeTicketSprite()
{
    //所持チケット枚数
    auto bg = Sprite::create("init/CharaMenu/revision/chara_exchange/ticket_bk.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(bg, Z_Exchange_Ticket_Bg, T_Exchange_Ticket_Bg);
    
    string exchange_ticket = toString<int>(_exchange_ticket);
    auto exchange_ticket_label = HMSFontUtil::createWithTTFBySystemFontArgument(exchange_ticket, FONT_NAME_SYSTEM , Utility::calcFlexibleFontSize(exchange_ticket, 36, 100));
    exchange_ticket_label->setAnchorPoint(Point(1.0, 0.5));
    exchange_ticket_label->setPosition(Point(bg->getContentSize().width - 20, bg->getContentSize().height / 2));
    bg->addChild(exchange_ticket_label, Z_Exchange_Ticket_Label, T_Exchange_Ticket_Label);
}

void CharaExchangeLayer::createHelpButton()
{
    //ヘルプボタンの作成
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [this](Ref* sender) {
        
        auto normal_scene = (NormalScene *)this->getParent();
        
        string url = HOST;
        url += "user/index.php?&p=page__chara_exchange_help&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "キャラ交換ヘルプ");
        normal_scene->addChild(WebViewLoadLayer, 9);
    });
    help_btn->setScale(ratio);
    auto menu = Menu::create(help_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(400, 789), 0.5, 1));
    this->addChild(menu, Z_Help_Menu, T_Help_Menu);
}

void CharaExchangeLayer::getExchangeCharaInfoRequest()
{
    //交換キャラデータの取得
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "chara/chara_revision/get_exchange_chara_info.php";
    auto selector = httpresponse_selector(CharaExchangeLayer::exchangeCharaInfoCallback);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void CharaExchangeLayer::exchangeCharaInfoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //交換キャラの情報の取得
    _exchange_chara_data_list = json["exchange_chara"].object_items();
    
    if (countJsonData(_exchange_chara_data_list)) {
        
        //ポジションインデックス
        createPositionIndex();
        
        //交換キャラリスト作成
        createScrollView();
        
        setScrollViewData();
        
        //交換済みのチェック
        checkExchangedCharaAndShortage();
    }
    else {
        
        //交換キャラがいない場合
        auto func = [&](){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("CharaMenu");
        };
        
        string comment = "現在交換できるキャラは\nございません。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        addChild(popup_layer, Z_Popup_Layer, T_Popup_Layer);
        
    }
    
}

void CharaExchangeLayer::createPositionIndex()
{
    //ポジションインデックスの作成
    int data_count = countJsonData(_exchange_chara_data_list);
    for (int i = 0; i < data_count; i++) {
        _vec_position_index.push_back(Point(320 * ratio, (_cell_size / 2) + (_cell_size * ((data_count - 1) - i))));
    }
}

void CharaExchangeLayer::createScrollView()
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

Layer* CharaExchangeLayer::createScrollList()
{
    //スクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    layer->setTag(T_ScrollLayer);
    
    //交換キャラリストの一覧の作成
    int data_count = countJsonData(_exchange_chara_data_list);
    for (int i = 0; i < data_count; i++)
    {
        string iStr = toString<int>(i);
        
        //交換キャラ画像の作成
        _exchange_chara_sp.push_back(CharaExchangeSprite::create(_exchange_chara_data_list[iStr], this));
        _exchange_chara_sp[i]->setScale(ratio);
        layer->addChild(_exchange_chara_sp[i]);
    }
    
    //並び替え
    sortExchangeChara();
    
    //表示
    for (int i = 0; i < data_count; i++) {
        _exchange_chara_sp[i]->setPosition(_vec_position_index[i]);
    }
    
    return layer;
}

void CharaExchangeLayer::sortExchangeChara()
{
    //交換済みが下にくるように並び替え
    sort(_exchange_chara_sp.begin(), _exchange_chara_sp.end(), [](const CharaExchangeSprite* chara1, const CharaExchangeSprite* chara2){
        return (chara2->getExchangeFlg());
    });
}

void CharaExchangeLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, _cell_size * _exchange_chara_sp.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - ((_cell_size * _exchange_chara_sp.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

void CharaExchangeLayer::checkExchangedCharaAndShortage()
{
    //交換済み又は所持チケットが足りているかチェックする
    for (int i = 0; i < _exchange_chara_sp.size(); i++) {
        string iStr = toString<int>(i);
        
        //交換済み
        if (_exchange_chara_sp[i]->getExchangeFlg()) _exchange_chara_sp[i]->setExchangedChara();
        
        //所持チケット枚数チェック
        if (fromString<int>(_exchange_chara_sp[i]->getNeedExchngeTicket()) > _exchange_ticket) _exchange_chara_sp[i]->setUnselectedExchangeButton();
    }
}

void CharaExchangeLayer::exchangeCharaDataRequest(string exchange_chara_id)
{
    //交換通信処理
    string url = HOST;
    url += "chara/chara_revision/exchange_chara.php";
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&exchange_chara_id=" + exchange_chara_id;
    auto selector = httpresponse_selector(CharaExchangeLayer::exchangeCharaDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void CharaExchangeLayer::exchangeCharaDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    
    //表示枚数の更新
    if (json["exchange_flg"].bool_value()) {
        
        string after_exchange_ticket = json["after_exchange_ticket"].string_value();
        SaveData::saveString("exchange_ticket", after_exchange_ticket);
        _exchange_ticket = fromString<int>(after_exchange_ticket);
        
        auto exchange_ticket_label = (Label *)this->getChildByTag(T_Exchange_Ticket_Bg)->getChildByTag(T_Exchange_Ticket_Label);
        exchange_ticket_label->setString(after_exchange_ticket);
        
        //プレゼント数の更新（+1する）
        int present_count = SaveData::loadInt("present_count") + 1;
        SaveData::saveInt("present_count", present_count);
    }
    
    auto func = [this, json](){
        
        if (json["exchange_flg"].bool_value())
        {
            //交換成功時
            for (int i = 0; i < _exchange_chara_sp.size(); i++) {
                
                if (_exchange_chara_sp[i]->getExchangeCharaId() == json["exchange_chara_id"].string_value()) {
                    
                    //交換済みに変更
                    _exchange_chara_sp[i]->setExchangeFlg(true);
                    _exchange_chara_sp[i]->setExchangedChara();
                    
                }
                
                //所持チケット枚数チェック
                if (fromString<int>(_exchange_chara_sp[i]->getNeedExchngeTicket()) > _exchange_ticket) _exchange_chara_sp[i]->setUnselectedExchangeButton();
                
            }
            
            //データの並び替え（交換済みを下にする）
            sortExchangeChara();
            
            //表示位置の変更アクション
            actionUpdatePage();
        }
    };
    string message =  json["exchange_text"].string_value();
    if (json["exchange_flg"].bool_value()) message += "\n※プレゼントからお受け取りください。";
    auto popup_layer = PopupOkButtonLayer::create(func, message);
    this->addChild(popup_layer, 999999);
}

void CharaExchangeLayer::actionUpdatePage()
{
    //ページ更新の際のアクション
    for (int i = 0; i < _exchange_chara_sp.size(); i++)
    {
        auto move = MoveTo::create(0.3f, _vec_position_index[i]);
        _exchange_chara_sp[i]->runAction(move);
    }
}

bool CharaExchangeLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void CharaExchangeLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void CharaExchangeLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void CharaExchangeLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void CharaExchangeLayer::scrollViewDidScroll(ScrollView *view)
{
}