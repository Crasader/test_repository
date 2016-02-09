#include "TournamentRecordLayer.h"

bool TournamentRecordLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TournamentRecordLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TournamentRecordLayer::onEnter()
{
    Layer::onEnter();
    
    //レコードデータの取得
    getRecordDataRequest();
}

void TournamentRecordLayer::getRecordDataRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "event/tournament_event/tournament_record.php";
    auto selector = httpresponse_selector(TournamentRecordLayer::recordDataCallback);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentRecordLayer::recordDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    _record_data = Json::parse(result, err);
    _replay_data_list = _record_data["replay_data_list"].object_items();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("トーナメント戦績", "TournamentEventTop", this);
    
    //データの設定
    createDataList();
    
    //レコードの作成、表示
    createRecordLabelList();
    
    //リプレイデータの作成、表示
    showReplayList();
}

void TournamentRecordLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TournamentRecordLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentRecordLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentRecordLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TournamentRecordLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentRecordLayer::createDataList()
{
    _vec_record_data.push_back(toString<int>(_record_data["entry_count"].int_value()));
    _vec_record_data.push_back(toString<int>(_record_data["victory_count"].int_value()));
    _vec_record_data.push_back(toString<int>(_record_data["total_count"].int_value()));
    _vec_record_data.push_back(toString<int>(_record_data["win_count"].int_value()));
}

void TournamentRecordLayer::createRecordLabelList()
{
    //背景
    auto bg = Sprite::create("init/Home/Event/TournamentEvent/bk_record.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 608), 0.5, 0.5));
    this->addChild(bg, Z_Bg_Record, T_Bg_Record);
    
    string vec_record_item[] = {"参加トーナメント数", "優勝回数", "対戦総数", "勝利数"};
    int distance = 47;
    for (int i = 0; i < (sizeof(vec_record_item) / sizeof(vec_record_item[0])); i++) {
        
        //戦績項目の表示
        string item_str = vec_record_item[i];
        auto record_item_label = HMSFontUtil::createWithTTFBySystemFontArgument(item_str, FONT_NAME_SYSTEM , 30);
        record_item_label->setAnchorPoint(Point(0, 0.5));
        record_item_label->setPosition(Point(50, 184 - (distance * i)));
        record_item_label->enableOutline(Color4B(0, 0, 0, 255), 2);
        bg->addChild(record_item_label);
        
        //戦績データの表示
        string data_str = _vec_record_data[i] + "回";
        auto record_data_label = HMSFontUtil::createWithTTFBySystemFontArgument(data_str, FONT_NAME_SYSTEM , 30);
        record_data_label->setAnchorPoint(Point(1.0, 0.5));
        record_data_label->setPosition(Point(560, 184 - (distance * i)));
        record_data_label->enableOutline(Color4B(0, 0, 0, 255), 2);
        bg->addChild(record_data_label);
    }
}

void TournamentRecordLayer::showReplayList()
{
    //背景
    auto bg = Sprite::create("init/Home/Event/TournamentEvent/bk_replay.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 230), 0.5, 0.5));
    this->addChild(bg, Z_Bg_Record, T_Bg_Record);
    
    //リプレイデータの表示
    int data_count = countJsonData(_replay_data_list);
    if (data_count > 0) {
        
        auto normal_scene = (NormalScene *)this->getParent();
        for (int i = 0; i < data_count; i++) {
            string iStr = toString<int>(i);
            auto replay_sp = ReplayListSprite::create(_replay_data_list[iStr].object_items(), normal_scene);
            replay_sp->setScale(0.9);
            replay_sp->setPosition(Point(bg->getContentSize().width / 2, bg->getContentSize().height - 104 - (i * 110)));
            bg->addChild(replay_sp);
        }
    }
    else {
        //リプレイデータがない場合
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument("最新のリプレイはありません", FONT_NAME_SYSTEM , 33);
        label->setPosition(Point(bg->getContentSize().width / 2, bg->getContentSize().height / 2 + 40));
        label->enableOutline(Color4B(0, 0, 0, 255), 1);
        bg->addChild(label);
    }
}

bool TournamentRecordLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    
    return true;
}
void TournamentRecordLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TournamentRecordLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TournamentRecordLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}