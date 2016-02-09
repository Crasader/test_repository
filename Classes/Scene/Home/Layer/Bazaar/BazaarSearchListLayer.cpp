#include "BazaarSearchListLayer.h"
#include "button/BazaarListRefreshButton.h"
#include "BazaarSearchListButton.h"
#include "BazaarListLayer.h"

BazaarSearchListLayer::BazaarSearchListLayer(){
    sortRarelity = 0;
    sortName = "";
    sortCost = 0;
    sortCharaType = CharaSprite::CharaType::ALL;
}

bool BazaarSearchListLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarSearchListLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void BazaarSearchListLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("キャラの検索", "BazaarTop", this, HeaderLayer::TitleBarType::WIDTH_324);

    GetBazaarData();
    
}

//最新の出品一覧を取得
void BazaarSearchListLayer::GetBazaarData()
{
    string user_master_id = SaveData::loadString("user_master_id");
    string url = HOST;
    url += "bazaar/get_bazaar_search_list.php";
    auto selector = httpresponse_selector(BazaarSearchListLayer::GetBazaarDataCallBack);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

//最新の出品一覧を取得完了
void BazaarSearchListLayer::GetBazaarDataCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    CCLOG("result = %s", result.c_str());
    
    //通信エラー
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    //ticketCount = stoi(json["ticket_count"].string_value());
    ticketCount = stoi(SaveData::loadString("exchange_ticket"));

    bazaarSeachList = json["search_list"].object_items();
    
    
    //リストの生成
    bazaarList = BazaarListLayer::create(bazaarSeachList);
    this->addChild(bazaarList, Z_SearchList ,T_Search_list);
    
    //バザーのリスト以外の画面を生成
    createButton();
    showExchangeTicketSprite();
}


void BazaarSearchListLayer::showExchangeTicketSprite()
{
    //所持チケット枚数
    Sprite* background = Sprite::create("init/CharaMenu/revision/chara_exchange/ticket_bk.png");
    background->setScale(ratio);
    background->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(background, Z_Exchange_Ticket_BG, T_Exchange_Ticket_BG);
    
    string ticketCountText = toString<int>(ticketCount);
    Label* ticketCountLabel = HMSFontUtil::createWithTTFBySystemFontArgument(ticketCountText, FONT_NAME_SYSTEM , Utility::calcFlexibleFontSize(ticketCountText, 36, 100));
    ticketCountLabel->setAnchorPoint(Point(1.0, 0.5));
    ticketCountLabel->setPosition(Point(background->getContentSize().width - 20, background->getContentSize().height / 2));
    background->addChild(ticketCountLabel, Z_Exchange_Ticket_Label, T_Exchange_Ticket_Label);
}


//出品リスト以外の画面生成
void BazaarSearchListLayer::createButton()
{
    //条件検索
    BazaarSearchListButton* searchButton = BazaarSearchListButton::Create("init/Home/Bazaar/button/bt_condition_search.png", "", this);
    searchButton->setScale(ratio);
    searchButton->setPosition(convertFromDisplaySize(Point(400, 789), 0, 0));
    
    //更新
//    BazaarListRefreshButton* refreshButton = BazaarListRefreshButton::Create("init/Home/Bazaar/button/bt_condition_search.png", "", bazaarList);
//    refreshButton->setPosition(convertFromDisplaySize(Point(400, 789), 0.0, 0.0));
    
    Menu* menu = Menu::create(searchButton, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(0, 0), 0.5, 1));
    this->addChild(menu, Z_Update, T_Update);
}


void BazaarSearchListLayer::exchangeCharaDataRequest(string bazaarLogId)
{
    //交換通信処理
    string url = HOST;
    url += "bazaar/bazaar_exchange.php";
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&bazaar_log_id=" + bazaarLogId;
    auto selector = httpresponse_selector(BazaarSearchListLayer::exchangeCharaDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BazaarSearchListLayer::exchangeCharaDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json exchangeResultJson = Json::parse(result, err);
    
    
    //表示枚数の更新
    if (exchangeResultJson["exchange_flg"].bool_value()) {
        
        string after_exchange_ticket = exchangeResultJson["after_exchange_ticket"].string_value();
        SaveData::saveString("exchange_ticket", after_exchange_ticket);
        //_exchange_ticket = fromString<int>(after_exchange_ticket);
        
        auto exchange_ticket_label = (Label *)this->getChildByTag(T_Exchange_Ticket_BG)->getChildByTag(T_Exchange_Ticket_Label);
        exchange_ticket_label->setString(after_exchange_ticket);
        
        //プレゼント数の更新（+1する）
        int present_count = SaveData::loadInt("present_count") + 1;
        SaveData::saveInt("present_count", present_count);
    }
    
    string message =  exchangeResultJson["exchange_chara_id"].string_value();
    function<void()> func = [](){};
    Layer* popLayer = PopupOkButtonLayer::create(func, exchangeCharaResultMessage(exchangeResultJson["result"].string_value()));
    this->addChild(popLayer, 999999);
    
    refreshList();
}

string BazaarSearchListLayer::exchangeCharaResultMessage(string result){
    if(result == "error"){
        return "不正な値です。\nリストを更新してからやり直して下さい。";
    }else if (result == "no_data"){
        return "他のユーザーが購入済みです。\nリストを更新してからやり直して下さい。";
    }else if (result == "mine"){
        return "自分の出品物は購入できません。";
    }else if (result == "no_ticket"){
        return "チケット数が足りません。";
    }else if (result == "number_of_times_over"){
        return "交換回数制限に達しているため\n本日はもう交換できません。";
    }
    return "購入を完了しました。\n※プレゼントからお受け取りください。";
}

void BazaarSearchListLayer::refreshList(){
    bazaarList->RefreshList(sortName, sortRarelity, sortCharaType, sortCost);
    
}
