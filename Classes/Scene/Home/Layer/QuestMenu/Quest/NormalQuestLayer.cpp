/* クエスト選択メニュー */
#include "NormalQuestLayer.h"

bool NormalQuestLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("NormalQuestLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void NormalQuestLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("中華統一戦記", "QuestMenu", this, HeaderLayer::TitleBarType::WIDTH_390);
    
    //クエストデータ取得通信
    getQuestDataRequest();
}

void NormalQuestLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(NormalQuestLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(NormalQuestLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(NormalQuestLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(NormalQuestLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void NormalQuestLayer::getQuestDataRequest()
{
    //クエストデータ取得リクエスト
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "quest/get_quest_info.php";
    auto selector = httpresponse_selector(NormalQuestLayer::getQuestDataRequestCallback);
    string body = "user_master_id="+user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void NormalQuestLayer::getQuestDataRequestCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("result:%s",result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    challenge_event_flg = json["challenge_event_flg"].bool_value();
    
    if (countJsonData(json["quest_data"].object_items()) > 0)
    {
        //データの取得
        _quest_data = json["quest_data"].object_items();
        
        //レベル別にデータ分解
        setupLevelData();
        
        //画像の作成
        createQuestSprite();
    }
}

void NormalQuestLayer::setupLevelData()
{
    //クエストデータの分解
    for(int i = 0; i<countJsonData(_quest_data); i++) {
        string iStr = toString<int>(i);
        _quest_level_data.push_back(_quest_data[iStr].object_items());
    }
    
    //クエストレベルを基準にソート
    sort(_quest_level_data.begin(), _quest_level_data.end(), [](const Json &json1, const Json &json2){ return fromString<int>(json1["quest_level"].string_value()) > fromString<int>(json2["quest_level"].string_value());});
}

void NormalQuestLayer::createQuestSprite()
{
    //クエストリストの作成
    for (int i = 0; i < (int)_quest_level_data.size(); i++)
    {
        _quest_sp.push_back((Sprite *)QuestSprite::create(_quest_level_data[i].object_items()));
        _quest_sp[i]->setTag(i);
    }
    
    auto func = [this](int tag){
        changePage(tag);
    };
    
//    sort(_quest_sp.begin(), _quest_sp.end(), [](const QuestSprite* sp1, const QuestSprite* sp2){
//        return fromString<int>(sp1->getQuestLevel()) > fromString<int>(sp2->getQuestLevel());
//    });
    
    //スクロールレイヤーの作成
    auto scroll_layer = HmsScrollLayer::create(160 * ratio, _quest_sp, func);
    this->addChild(scroll_layer, 1, SCROLLLAYER_TAG);
    
    //試練ボタン
    if(challenge_event_flg==true) {
        auto challenge_btn_off = Sprite::create("init/Home/Event/ChallengeEvent/bt_challenge.png");
        auto challenge_btn_on  = Sprite::create("init/Home/Event/ChallengeEvent/bt_challenge.png");
        challenge_btn_on->setColor(Color3B(180, 180, 180));
        
        auto challenge_btn = MenuItemSprite::create(challenge_btn_off, challenge_btn_on, CC_CALLBACK_1(NormalQuestLayer::toChallengePage, this));
        challenge_btn->setScale(ratio);
        challenge_btn->setTag(BATTLE_HELP_BUTTON_TAG);
        challenge_btn->setAnchorPoint(Point(0.5, 1));
        auto Menu3 = Menu::create(challenge_btn, nullptr);
        Menu3->setPosition(convertFromDisplaySize(Point(512, 838), 0.5, 1));
        this->addChild(Menu3, 5);
        
        auto img= Sprite::create("init/Home/Event/ChallengeEvent/ordeal_challenge.png");
        img->setPosition(convertFromDisplaySize(Point(492, 879), 0.5, 1));
        img->setScale(ratio);
        img->setGlobalZOrder(1);
        this->addChild(img, Z_Challenge_Sp, T_Challenge_Sp);
        
        //表示して削除するアクション
        ScaleTo *scaleTo_big = ScaleTo::create(2.0f, ratio) ;
        EaseElasticOut *scale_ease_out = EaseElasticOut::create(scaleTo_big);
        
        ScaleTo *scaleTo_small = ScaleTo::create(0.7f, 0) ;
        EaseBackIn *scale_back_in = EaseBackIn::create(scaleTo_small);
        
        Sequence* seq = Sequence::create(DelayTime::create(1.0f), scale_ease_out, scale_back_in, RemoveSelf::create(), nullptr) ;
        img->runAction(seq) ;
    }
}

void NormalQuestLayer::changePage(int tag)
{
    //タップしたスプライトの取得
    auto quest_sp = (QuestSprite *)this->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(tag);
    
    //ステージ説明のデータを保存
    SaveData::saveString("quest_explain_data", _quest_level_data[tag].dump().c_str());
    
    //小ステージデータの保存
    SaveData::saveString("quest_stage_data", quest_sp->_quest_stage_data.dump());
    
    //小ステージへのページ遷移
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("NormalQuestStage");
}

void NormalQuestLayer::toChallengePage(Ref* sender)
{
    //小ステージへのページ遷移
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("ChallengeEventMenu");
}

bool NormalQuestLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touchBegan! NormalQuestLayer!");
    
    return true;
}
void NormalQuestLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void NormalQuestLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

