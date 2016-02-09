#include "TournamentEventConfirmLayer.h"

TournamentEventConfirmLayer::TournamentEventConfirmLayer()
:_confirm_status(Entry)
{
}

TournamentEventConfirmLayer* TournamentEventConfirmLayer::create(string page_name)
{
    TournamentEventConfirmLayer *pRet = new TournamentEventConfirmLayer();
    
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

bool TournamentEventConfirmLayer::init(string page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TournamentEventConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    //ページ状態の初期設定
    if (page_name == "TournamentEventConfirmEntry") _confirm_status = Entry;
    else if (page_name == "TournamentEventConfirmCancel") _confirm_status = Cancel;
    
    return true;
}

void TournamentEventConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    if (_confirm_status == Entry) HeaderLayer::setupTitleBar("トーナメント参加確認", "TournamentEventDeckChoice", this);
    else if (_confirm_status == Cancel) HeaderLayer::setupTitleBar("参加キャンセル", "TournamentEventTop", this);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //トーナメント情報取得通信処理
    httpRequestForEventInfo();
}

void TournamentEventConfirmLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TournamentEventConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentEventConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentEventConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TournamentEventConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentEventConfirmLayer::httpRequestForEventInfo()
{
    //トーナメント情報取得処理
    string user_master_id = SaveData::loadString("user_master_id");
    string status_text = "";
    if (_confirm_status == Entry) status_text = "Entry";
    else if (_confirm_status == Cancel) status_text = "Cancel";
    
    string url = HOST;
    url += "event/tournament_event/get_tournament_confirm_info.php";
    auto selector = httpresponse_selector(TournamentEventConfirmLayer::onHttpRequestCompleted);
    string body = "user_master_id=" + user_master_id + "&tournament_event_list_id=" + SaveData::loadString("tournament_event_list_id") + "&deck_no=" + SaveData::loadString("tournament_event_deck_no") + "&status_text=" + status_text;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEventConfirmLayer::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    //トーナメント情報コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //トーナメント情報の取得
    _tournament_data = json["tournament_list_data"].object_items();
    
    if (countJsonData(_tournament_data))
    {
        //選択したデータの設定
        _tournament_event_list_id = _tournament_data["0"]["tournament_event_list_id"].string_value();
        _deck_no = _tournament_data["0"]["deck_no"].string_value();
        _item_type = _tournament_data["0"]["item_type"].string_value();
        _count = _tournament_data["0"]["count"].string_value();
        
        //トーナメント情報画像の作成
        createTournamentInfoSprite();
        
        //トーナメント参加ボタンの作成
        createTournamentJoinButton();
        
        if (_confirm_status == Entry) {
            
            //宝玉消費警告画像の表示
            showJewelConsumeSprite();
        }
        else if (_confirm_status == Cancel) {
            
            //キャンセル文字列の作成
            createCancelLabel();
            
        }
    }
    else {
        
        //トーナメント情報取得失敗時
        auto normal_scene = (NormalScene *)this->getParent();
        auto func = [normal_scene](){
            normal_scene->changePage("TournamentEventTop");
        };
        
        string comment = "トーナメントの情報が\n取得できませんでした。\nイベントのトップに戻ります。";
        auto popup_layer = PopupOkButtonLayer::create(func, comment);
        normal_scene->addChild(popup_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    }
}

void TournamentEventConfirmLayer::createTournamentInfoSprite()
{
    //トーナメント情報の背景
    TournamentEventConfirmSprite::Status status = TournamentEventConfirmSprite::Entry;
    if (_confirm_status == Cancel) status = TournamentEventConfirmSprite::Cancel;
    
    _info_bg = (Sprite *)TournamentEventConfirmSprite::create(_tournament_data["0"], status);
    _info_bg->setScale(ratio);
    _info_bg->setPosition(convertFromDisplaySize(Point(320, 530), 0.5, 0.5));
    this->addChild(_info_bg, Z_Tournament_Info, T_Tournament_Info);
}

void TournamentEventConfirmLayer::createTournamentJoinButton()
{
    //トーナメントに参加するボタン
    string image_path = "init/Home/Event/TournamentEvent/bt_join_tournament.png";
    if (_confirm_status == Cancel) image_path = "init/Home/Event/TournamentEvent/bt_tournament_cancel.png";
    
    auto entry_off = Sprite::create(image_path);
    auto entry_on  = Sprite::create(image_path);
    entry_on->setColor(Color3B(180, 180, 180));
    
    auto entry_btn = MenuItemSprite::create(entry_off, entry_on,[this](Ref* sender){
        
        switch (_confirm_status) {
            case Entry:
            {
                //トーナメント参加通信処理
                httpRequestForTournamentEntry();
            }break;
            case Cancel:
            {
                //トーナメントキャンセル通信処理
                httpRequestForTournamentCancel();
            }break;
            default: break;
        }
    });
    entry_btn->setTag(T_Entry_MenuItem);
    entry_btn->setScale(ratio);
    
    auto menu = Menu::create(entry_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 230), 0.5, 0.5));
    this->addChild(menu, Z_Entry_Menu, T_Entry_Menu);
}

void TournamentEventConfirmLayer::showJewelConsumeSprite()
{
    string item_path = "";
    string text = "";
    if (_item_type == "jewel") {
        item_path = "init/Home/Event/TournamentEvent/jewel.png";
        text = "宝玉を" + _count + "個消費します";
    }
    else if (_item_type == "gold") {
        item_path = "init/Home/Event/TournamentEvent/coin.png";
        text = "資金を" + _count + "枚消費します";
    }
    
    //エントリー消費アイテムの画像
    auto consume_sp = Sprite::create("init/Home/Event/TournamentEvent/consume.png");
    consume_sp->setScale(ratio);
    consume_sp->setPosition(convertFromDisplaySize(Point(320, 140), 0.5, 0.5));
    this->addChild(consume_sp, Z_Consume, T_Consume);
    
    //参加アイテム画像
    auto item_sp = Sprite::create(item_path);
    item_sp->setPosition(Point(40, consume_sp->getContentSize().height / 2));
    consume_sp->addChild(item_sp);
    
    //文字
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 28, 340));
    label->setPosition(Point(240, consume_sp->getContentSize().height / 2));
    label->enableOutline(Color4B(0, 0, 0, 255), 1);
    label->setColor(Color3B(255, 30, 30));
    consume_sp->addChild(label);
}

void TournamentEventConfirmLayer::createCancelLabel()
{
    //トーナメントキャンセル文字列の作成
    auto bg = Sprite::create("init/Home/Event/TournamentEvent/bk_participating.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 320), 0.5, 0.5));
    this->addChild(bg, Z_Cancel_Bg, T_Cancel_Bg);
    
    string text = "「" + _tournament_data["0"]["name"].string_value() + "」に参加中です";
    auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 33, 600));
    label1->setColor(Color3B(240, 0, 0));
    label1->setPosition(Point(bg->getContentSize() / 2));
    bg->addChild(label1, Z_Cancel_Label1, T_Cancel_Label1);
    
    auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument("※トーナメントに参加で使用したアイテムは返却されます", FONT_NAME_SYSTEM, 24);
    label2->setScale(ratio);
    label2->setPosition(convertFromDisplaySize(Point(320, 140), 0.5, 0.5));
    this->addChild(label2, Z_Cancel_Label1, T_Cancel_Label1);

}

void TournamentEventConfirmLayer::httpRequestForTournamentEntry()
{
    //トーナメント参加通信処理
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/tournament_event/tournament_entry.php";
    auto selector = httpresponse_selector(TournamentEventConfirmLayer::getTournamentEntryCallback);
    string body = "user_master_id=" + user_master_id + "&tournament_event_list_id=" + _tournament_event_list_id + "&deck_no=" + _deck_no;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEventConfirmLayer::getTournamentEntryCallback(HttpClient *sender, HttpResponse *response)
{
    //トーナメント参加コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    _entry_data_list = json["entry_date_list"].object_items();
    
    auto normal_scene = (NormalScene *)this->getParent();
    if (_entry_data_list["entry_flg"].bool_value()) {
        
        //エントリー成功時
        
        //所持アイテム数の更新
        string item_type = _entry_data_list["item_type"].string_value();
        int after_item_count = fromString<int>(_entry_data_list["after_item_count"].string_value());
        
        //宝玉の更新
        if (item_type == "jewel") {
            auto jewel_label = (Label *)this->getParent()->getChildByTag(HEADER_LAYER_TAG)->getChildByTag(JEWEL_LABEL_TAG);
            if (after_item_count >= 0) {
                jewel_label->setString(toString(after_item_count));
            }
        }
        
        //資金の更新
        if (item_type == "gold") {
            auto gold_label = (Label *)this->getParent()->getChildByTag(HEADER_LAYER_TAG)->getChildByTag(GOLD_LABEL_TAG);
            if (after_item_count >= 0) {
                gold_label->setString(toString(after_item_count));
            }
        }
        
        auto ok_func = [normal_scene](){
        
            //待機ページへ行く
            normal_scene->changePage("TournamentEventBattleWait");
        };
        
        //トーナメント参加中フラグ
        SaveData::saveString("tournament_event_entry", "1");
        
        auto popup_layer = PopupOkButtonLayer::create(ok_func, _entry_data_list["entry_text"].string_value());
        normal_scene->addChild(popup_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    }
    else {
    
        //エントリー失敗時
        auto normal_scene = (NormalScene *)this->getParent();
        auto ok_func = [normal_scene](){
            normal_scene->changePage("TournamentEventTop");
        };
        auto popup_layer = PopupOkButtonLayer::create(ok_func, _entry_data_list["entry_text"].string_value());
        normal_scene->addChild(popup_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    }
}

void TournamentEventConfirmLayer::httpRequestForTournamentCancel()
{
    //トーナメントキャンセル通信処理
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/tournament_event/tournament_cancel.php";
    auto selector = httpresponse_selector(TournamentEventConfirmLayer::getTournamentCancelCallback);
    string body = "user_master_id=" + user_master_id + "&tournament_event_list_id=" + _tournament_event_list_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEventConfirmLayer::getTournamentCancelCallback(HttpClient *sender, HttpResponse *response)
{
    //トーナメントキャンセルコールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    _cancel_data_list = json["cancel_data_list"].object_items();
    
    if (_cancel_data_list["cancel_flg"].bool_value()) {
        
        //エントリー成功時
        
        //所持アイテム数の更新
        string item_type = _cancel_data_list["item_type"].string_value();
        int after_item_count = fromString<int>(_cancel_data_list["after_item_count"].string_value());
        
        //宝玉の更新
        if (item_type == "jewel") {
            auto jewel_label = (Label *)this->getParent()->getChildByTag(HEADER_LAYER_TAG)->getChildByTag(JEWEL_LABEL_TAG);
            if (after_item_count >= 0) {
                jewel_label->setString(toString(after_item_count));
            }
        }
        
        //資金の更新
        if (item_type == "gold") {
            auto gold_label = (Label *)this->getParent()->getChildByTag(HEADER_LAYER_TAG)->getChildByTag(GOLD_LABEL_TAG);
            if (after_item_count >= 0) {
                gold_label->setString(toString(after_item_count));
            }
        }
        
        auto normal_scene = (NormalScene *)this->getParent();
        auto ok_func = [normal_scene](){
            
            //イベントトップページへ戻る
            normal_scene->changePage("TournamentEventTop");
        };
        
        //エントリーフラグを消す
        SaveData::saveString("tournament_event_entry", "0");
        
        auto popup_layer = PopupOkButtonLayer::create(ok_func, _cancel_data_list["cancel_text"].string_value());
        normal_scene->addChild(popup_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
    }
}

bool TournamentEventConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TournamentEventConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TournamentEventConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TournamentEventConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
