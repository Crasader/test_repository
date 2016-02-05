#include "PresentMenuLayer.h"

PresentMenuLayer::PresentMenuLayer()
:_offset(0)
,_scroll_layer_lower_end_flg(false)
{
}

bool PresentMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PresentMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void PresentMenuLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(PresentMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PresentMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PresentMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PresentMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void PresentMenuLayer::createReceiveAllButton()
{
    //まとめて受け取るボタンの作成
    auto receive_all_off = Sprite::create("init/Present/bt_receive_all.png");
    auto receive_all_on  = Sprite::create("init/Present/bt_receive_all.png");
    receive_all_on->setColor(Color3B(180, 180, 180));
    
    auto receive_all_btn = MenuItemSprite::create(receive_all_off, receive_all_on, [this](Ref* sender)
    {
        //まとめて受け取る処理
        presentReceiveAllRequest();
    });
    receive_all_btn->setScale(ratio);
    receive_all_btn->setAnchorPoint(Point(1.0, 0.5));
    auto Menu = Menu::create(receive_all_btn, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(640, 789), 0.5, 1));
    this->addChild(Menu, ORDER_PRESENT_ALL, TAG_RECEIVE_ALL_BUTTON);

}

void PresentMenuLayer::getPresentDataRequest()
{
    //プレゼントデータ取得
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "present/get_present_info.php";
    auto selector = httpresponse_selector(PresentMenuLayer::presentDateCallback);
    string body = "user_master_id="+user_master_id;
    body += "&offset=" + toString<int>(_offset) ;
    body += "&limit=" + toString<int>(kListCellMaxCountInPage) ;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void PresentMenuLayer::presentDateCallback(HttpClient *sender, HttpResponse *response)
{
    //プレゼントデータ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s",result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    Json user_present_list = json["user_present_list"].object_items();
    int present_count_all = json["present_count_all"].int_value() ;
    
    int present_count_now_page = json["present_count_now_page"].int_value() ;

    bool has_page_next = json["has_page_next"].bool_value() ;
    bool has_page_prev = json["has_page_prev"].bool_value() ;
    int offset = json["offset"].int_value() ;
    
    recreatePager(present_count_all, offset, has_page_next, has_page_prev) ;
    
    //ユーザーデフォルトの更新
    SaveData::saveString("user_present_list", user_present_list.dump());
    SaveData::saveInt("present_count", present_count_all);
    
    //ページの基準位置を保存
    _offset = offset ;
    
    //データの数をメンバ変数に設定
    _present_count_now_page = present_count_now_page;
    
    //スプライトの作成
    initSprite(user_present_list);
    
    //データの数により処理分け
    showSelectPage();
}

void PresentMenuLayer::recreatePager(int present_cont_all, int offset, bool has_page_next, bool has_page_prev)
{
    this->removeChildByTag(TAG_PAGER_BACK) ;
    
    int now_page_num = offset / kListCellMaxCountInPage + 1;
    int max_page_num = (present_cont_all-1) / kListCellMaxCountInPage + 1;
    
    if(max_page_num > 1){
        Sprite *pager_back = Sprite::create("init/Present/bk_page.png") ;
        pager_back->setScale(ratio);
        pager_back->setAnchorPoint(Point(0.5, 0)) ;
        pager_back->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0), 0.5, 0));
        this->addChild(pager_back, ORDER_PAGER_BACK, TAG_PAGER_BACK) ;
        
        Label *page_num = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(now_page_num) + "/" + toString<int>(max_page_num), FONT_NAME_SYSTEM, 30);
        page_num->setHorizontalAlignment(TextHAlignment::CENTER);
        page_num->setAnchorPoint(Point(0.5, 0.5)) ;
        page_num->setPosition(pager_back->getContentSize().width/2, 131);
        pager_back->addChild(page_num, 0, TAG_PAGE_NUM);

        if(has_page_next) {
            ui::Button *button = ui::Button::create("init/Present/bt_next.png") ;
            button->addTouchEventListener(getFunctionByArrowTag(TAG_RIGHT_ARROW)) ;
            button->setPosition(Point(DISPLAY_WIDTH-126, 131));
            pager_back->addChild(button, 0, TAG_RIGHT_ARROW) ;
        }
        
        if(has_page_prev) {
            ui::Button *button = ui::Button::create("init/Present/bt_before.png") ;
            button->addTouchEventListener(getFunctionByArrowTag(TAG_LEFT_ARROW)) ;
            button->setPosition(Point(126, 131));
            pager_back->addChild(button, 0, TAG_LEFT_ARROW) ;
        }
        
    }
    

}

function<void(Ref *sender, cocos2d::ui::Widget::TouchEventType type)> PresentMenuLayer::getFunctionByArrowTag(int tag){
    
    function<void(Ref *sender, cocos2d::ui::Widget::TouchEventType type)> func ;
    
    func = [this, tag](Ref *sender, cocos2d::ui::Widget::TouchEventType type){
            switch(type)
            {
                case ui::Widget::TouchEventType::BEGAN:{
                    HMSLOG("touchEvent: %s", "BEGAN") ;
                    
                    this->removeChildByTag(1); //スクロールビューを削除
                    
                    //ページャーに乗ってるスプライトを削除
                    Sprite *pager_back = (Sprite *)getChildByTag(TAG_PAGER_BACK) ;
                    if(pager_back){
                        pager_back->removeChildByTag(TAG_PAGE_NUM) ; //現在のページの数
                        pager_back->removeChildByTag(TAG_RIGHT_ARROW) ; //矢印を削除
                        pager_back->removeChildByTag(TAG_LEFT_ARROW) ;//矢印を削除
                    }
                    
                    //タグに応じて処理切り替え
                    if(tag == TAG_LEFT_ARROW) this->_offset -= kListCellMaxCountInPage ;
                    if(tag == TAG_RIGHT_ARROW) this->_offset += kListCellMaxCountInPage ;
                    
                    this->getPresentDataRequest();//データ取得通信
                    
                    break;
                }
                default:
                    break;
            }
        
    } ;
    
    
    return func ;
}


void PresentMenuLayer::showSelectPage()
{
    //データがあるか無いかで処理分け
    if (_present_count_now_page > 0)
    {
        //スクロールビューの作成
        createScrollView();
    }
    else
    {
        //受け取れるプレゼントが無い場合
        auto layer = popupChangePageLayer::create("受け取れるプレゼントはありません。\nホーム画面に戻ります。", "Mypage");
        addChild(layer, 999999);
        
        //未読のプレゼントカウントを0にする
        SaveData::saveInt("present_count", 0);
    }
}

void PresentMenuLayer::initSprite(Json json)
{
    //プレゼントデータを初期化
    presents.clear();
    
    //プレゼント画像の作成
    for (int i = 0; i < _present_count_now_page; i++)
    {
        //jsonデータをメンバ変数に設定
        string i_str = toString<int>(i);
        presents.push_back(PresentSprite::create());
        presents[i]->setupData(json[i_str]);
        presents[i]->parseInsertDate();
        presents[i]->showData();
    }
}

void PresentMenuLayer::createScrollView()
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
    this->addChild(pScrollView, ORDER_SCROLLVIEW, TAG_SCROLLVIEW);
    
    //PresentMenuのレイヤーを戻り値に指定して使う
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _present_count_now_page));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _present_count_now_page) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
    
}

Layer* PresentMenuLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    _cell_size = 130 * ratio;
    auto layer = Layer::create();
    
    //プレゼント画像の配置
    for (int i = _present_count_now_page-1; 0 <= i; i--)
    {
        auto present_sp = presents[presents.size() - 1 -i];
        present_sp->setScale(ratio);
        present_sp->setPosition(Point(320 * ratio, (_cell_size / 2) + (_cell_size * i)));
        layer->addChild(present_sp, 1);
    }

    return layer;
}

void PresentMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナー登録
    initEventListener();
    
    //ヘッダーの作成
    HeaderLayer::setupTitleBar("プレゼント一覧", "Mypage", this);
    
    //まとめて受け取るボタンの作成
    createReceiveAllButton();
    
    //プレゼントデータの取得
    getPresentDataRequest();
}


bool PresentMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void PresentMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PresentMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{

}
void PresentMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PresentMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    if(_scroll_layer_lower_end_flg == false && view->getContentOffset().y == 0){
        //スクロールレイヤーが下端についた時の処理
        _scroll_layer_lower_end_flg = true ;
        Sprite *pager_back = (Sprite *)getChildByTag(TAG_PAGER_BACK) ;
        if(pager_back){
            MoveTo *move = MoveTo::create(0.5, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, -300), 0.5, 0)) ;
            pager_back->stopAllActions() ;
            pager_back->runAction(move);
        }
    }else if(_scroll_layer_lower_end_flg == true){
        //スクロールレイヤーが下端以外の時の処理
        _scroll_layer_lower_end_flg = false ;
        Sprite *pager_back = (Sprite *)getChildByTag(TAG_PAGER_BACK) ;
        if(pager_back){
            MoveTo *move = MoveTo::create(0.5, convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0), 0.5, 0)) ;
            pager_back->stopAllActions() ;
            pager_back->runAction(move);
        }
    }
}

void PresentMenuLayer::presentReceiveRequest(string primaryId)
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "present/get_navi_present.php";
    auto selector = httpresponse_selector(PresentMenuLayer::presentReceiveCallback);
    string body = "user_master_id=" + user_master_id + "&navi_present_log_id=" + primaryId;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void PresentMenuLayer::presentReceiveCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["comment"].string_value() == "ok")
    {
//        //受け取りプレゼントカウントを1つ減らす
//        SaveData::saveInt("present_count", SaveData::loadInt("present_count") - 1);
        
        //ヘッダー情報更新処理
        auto header_layer = (HeaderLayer *)this->getParent()->getChildByTag(HEADER_LAYER_TAG);
        
        if (json["receive_present_list"]["0"]["item_type"].string_value() == "gold")
        {
            //資金更新処理
            auto gold_label = (Label *)header_layer->getChildByTag(GOLD_LABEL_TAG);
            int new_gold = fromString<int>(SaveData::loadString("gold")) + (fromString<int>(json["receive_present_list"]["0"]["count"].string_value()));
            gold_label->setString(toString<int>(new_gold));
            SaveData::saveString("gold", toString<int>(new_gold));
        }
        else if (json["receive_present_list"]["0"]["item_type"].string_value() == "jewel")
        {
            //宝玉更新処理
            auto jewel_label = (Label *)header_layer->getChildByTag(JEWEL_LABEL_TAG);
            int new_jewel = fromString<int>(SaveData::loadString("jewel")) + (fromString<int>(json["receive_present_list"]["0"]["count"].string_value()));
            jewel_label->setString(toString<int>(new_jewel));
            SaveData::saveString("jewel", toString<int>(new_jewel));
        }
        else if (json["receive_present_list"]["0"]["item_type"].string_value() == "exchange_ticket")
        {
            //交換チケット更新処理
            int new_exchange_ticket = fromString<int>(SaveData::loadString("exchange_ticket")) + (fromString<int>(json["receive_present_list"]["0"]["count"].string_value()));
            SaveData::saveString("exchange_ticket", toString<int>(new_exchange_ticket));
        }
        
        //受け取った処理
        auto ok_func = [this]()
        {
            this->removeChildByTag(1); //スクロールビューを削除
            
            //再度データを取得して表示
            this->getPresentDataRequest();
        };
        
        auto popup_layer = PopupOkButtonLayer::create(ok_func, "プレゼントを受け取りました。");
        addChild(popup_layer, 9999999);
        
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/money.mp3");
        
    }
    else
    {
        //受け取り失敗
        auto ok_func = [](){}; //何もしない閉じるだけ
        auto popup_layer = PopupOkButtonLayer::create(ok_func, json["comment"].string_value());
        addChild(popup_layer, 999999);
    }
}

void PresentMenuLayer::presentReceiveAllRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += "present/get_navi_present_all.php";
    auto selector = httpresponse_selector(PresentMenuLayer::presentReceiveAllCallback);
    string body = "user_master_id=" + user_master_id;
    body += "&offset=" + toString<int>(_offset) ;
    body += "&limit=" + toString<int>(kListCellMaxCountInPage) ;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void PresentMenuLayer::presentReceiveAllCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json["comment"].string_value() == "ok")
    {
        //メンバ変数に保存
        Json receive_present_list      = json["receive_present_list"].object_items();//受け取ったプレゼント
        Json fail_receive_present_list = json["fail_receive_present_list"].object_items();//受け取り失敗プレゼント
        
        int add_gold  = 0;
        int add_jewel = 0;
        int add_exchange_ticket = 0;
        
        //受け取った資金と宝玉の数を数える
        int json_count = countJsonData(receive_present_list) ;
        for (int i = 0; i < json_count; i++) {
            if (receive_present_list[toString<int>(i)]["item_type"].string_value() == "gold") {
                add_gold += (fromString<int>(receive_present_list[toString<int>(i)]["count"].string_value()));
            }
            else if (receive_present_list[toString<int>(i)]["item_type"].string_value() == "jewel") {
                add_jewel += (fromString<int>(receive_present_list[toString<int>(i)]["count"].string_value()));
            }
            else if (receive_present_list[toString<int>(i)]["item_type"].string_value() == "exchange_ticket") {
                add_exchange_ticket += (fromString<int>(receive_present_list[toString<int>(i)]["count"].string_value()));
            }
        }
            
        
        //ヘッダー情報更新処理
        auto header_layer = (HeaderLayer *)this->getParent()->getChildByTag(HEADER_LAYER_TAG);
        
        //資金更新処理
        auto gold_label = (Label *)header_layer->getChildByTag(GOLD_LABEL_TAG);
        int new_gold = fromString<int>(SaveData::loadString("gold")) + add_gold;
        gold_label->setString(toString<int>(new_gold));
        SaveData::saveString("gold", toString<int>(new_gold));
        
        //宝玉更新処理
        auto jewel_label = (Label *)header_layer->getChildByTag(JEWEL_LABEL_TAG);
        int new_jewel = fromString<int>(SaveData::loadString("jewel")) + add_jewel;
        jewel_label->setString(toString<int>(new_jewel));
        SaveData::saveString("jewel", toString<int>(new_jewel));
        
        //交換チケット更新処理
        int new_exchange_ticket = fromString<int>(SaveData::loadString("exchange_ticket")) + add_exchange_ticket ;
        SaveData::saveString("exchange_ticket", toString<int>(new_exchange_ticket));
        
        if (countJsonData(fail_receive_present_list) > 0)
        {
            //受け取り失敗あり成功処理
            auto ok_func = [this]()
            {
                this->removeChildByTag(1); //スクロールビューを削除
                
                //再度データを取得して表示
                this->getPresentDataRequest();
            };
            
            auto popup_layer = PopupOkButtonLayer::create(ok_func, "受け取れなかった\nプレゼントがあります。");
            addChild(popup_layer, 9999999);
            
//            //受け取ったプレゼントの数だけプレゼントカウントを引く
//            SaveData::saveInt("present_count", SaveData::loadInt("present_count") - countJsonData(receive_present_list));
        }
        else
        {
            auto ok_func = [this]()
            {
                this->removeChildByTag(1); //スクロールビューを削除
                
                //再度データを取得して表示
                this->getPresentDataRequest();
            };
            
            string receive_present_list_count = toString<int>(countJsonData(receive_present_list));
            auto popup_layer = PopupOkButtonLayer::create(ok_func, receive_present_list_count+"件のプレゼントを\n受け取りました。");
            addChild(popup_layer, 9999999);
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/money.mp3");

        }
    }
    else
    {
        //受け取り失敗
        auto ok_func = [](){}; //何もしない、閉じるだけ
        auto popup_layer = PopupOkButtonLayer::create(ok_func, json["comment"].string_value());
        addChild(popup_layer, 9999999);
    }
}
