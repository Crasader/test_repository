#include "QuestMenuLayer.h"
#include "PlistAnimationSprite.h"

QuestMenuLayer::QuestMenuLayer()
:_event_open_flg(false)
,_challenger_exists_result()
,_rank_point_up_rate()
,_index_story_button(-1)
,_index_online_button(-1)
,_index_event_button(-1)
,_index_tournament_event_button(-1)
,_index_ranking_event_button(-1)
//,_index_boss_event_button(-1)
,_index_offline_button(-1)
{
}

bool QuestMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("QuestMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void QuestMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //ダンジョンシーンで使用するデータ初期化
    Utility::initDungeonData();
    
    //最終確認画面で使用する敵スプライトデータの初期化
    initBattleUserSpriteData();
    
    //イベントリスナーの作成
    createEventListener();
	
	//クイック対戦の対戦相手存在チェック
	ArenaChallengerCheckRequest();
	
    //タイトル画像の作成
    HeaderLayer::setupTitleBar("出陣", "Mypage", this, HeaderLayer::TitleBarType::WIDTH_215);
    
    //称号一覧、報酬一覧、ヘルプボタンの作成
    createWebViewButton();
    
    //リストスプライトの作成
    createListSprite();
}

void QuestMenuLayer::initBattleUserSpriteData()
{
    //最終確認ページの敵スプライトデータの初期化
    string user_master_id = "" ;
    string friend_id = "";
    string card_image_id = "";
    string user_name = "???" ;
    string level = "0";
    string last_play_date = "";
    string comment = "";
    string rank_name =  "???";
    string battle_point_rate = "1" ;
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"user_master_id\":\"" + user_master_id + "\"," + cr;
    data += "  \"friend_id\":\"" + friend_id + "\"," + cr;
    data += "  \"card_image_id\":\"" + card_image_id+ "\"," + cr;
    data += "  \"user_name\":\"" + user_name + "\"," + cr;
    data += "  \"level\":\"" + level + "\"," + cr;
    data += "  \"last_play_date\":\"" + last_play_date + "\"," + cr;
    data += "  \"comment\":\"" + comment + "\"," + cr;
    data += "  \"rank_name\":\"" + rank_name + "\"," + cr;
    data += "  \"battle_point_rate\":\"" + battle_point_rate + "\"" + cr;
    data += "}";
    HMSLOG("%s", data.c_str()) ;
    
    SaveData::saveString("battle_user_sprite_data", data);
}

void QuestMenuLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(QuestMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(QuestMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(QuestMenuLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void QuestMenuLayer::ArenaChallengerCheckRequest()
{
	//クイック対戦対戦相手存在チェックのリクエスト
	string url = HOST;
	url += "battle/arena_state_check.php";
	string body = "user_master_id="+ SaveData::loadString("user_master_id");
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        ArenaChallengerCheckCallback(sender,response);
    }, body);
}

void QuestMenuLayer::ArenaChallengerCheckCallback(HttpClient *sender, HttpResponse *response)
{
	//クイック対戦対戦相手存在チェックのレスポンス
	string result = HttpConnectHms::getHttpResponse(this, sender, response);
	if (result != "not connect" && result != "response NG"){
		
		string err;
		Json json = Json::parse(result, err);
        
        //オンラインバトルに関する情報の設定
        //挑戦者の存在するか結果を取得
        _challenger_exists_result = json["challenger_exists"].string_value();
        
        //オンラインポイント倍率の取得
        _rank_point_up_rate = json["rank_point_up_rate"].string_value();
        
        
        //オンライン対戦に対戦相手が存在する場合、画像を設定する。
        if(_challenger_exists_result=="challenger_coming"){
            auto challenger_coming = Sprite::create("init/Home/Quest/challenger.png");
            challenger_coming->setScale(0);
            challenger_coming->setPosition(Point(_list_sp[_index_online_button]->getContentSize().width / 2 - 220, _list_sp[_index_online_button]->getContentSize().height - 20));
            
            auto *challenger_coming_scale_up1  = ScaleTo::create(0.2f, 1.0f);
            challenger_coming->runAction(
                                         Sequence::create(
                                                          EaseBounceOut::create(challenger_coming_scale_up1),
                                                          nullptr)
                                         );
            
            auto *challenger_coming_scale_up   = ScaleTo::create(0.8f, 1.0f);
            auto *challenger_coming_scale_down = ScaleTo::create(0.8f, 0.8f);
            RepeatForever *challenger_coming_forever_action = RepeatForever::create(
                                                                                    Sequence::create(
                                                                                                     challenger_coming_scale_up,
                                                                                                     challenger_coming_scale_down,
                                                                                                     nullptr)
                                                                                    ) ;
            challenger_coming->runAction(challenger_coming_forever_action);
            _list_sp[_index_online_button]->addChild(challenger_coming, 3);
        }
        
        //オンライン対戦のポイント倍率があがっている場合のみバナーに文言を表示。
        if(_rank_point_up_rate != "0"){
            auto up_rate = Sprite::create("init/Home/Quest/event_term.png");
            up_rate->setPosition(Point(_list_sp[_index_online_button]->getContentSize().width - 64, 50));
            up_rate->setScale(0);
            _list_sp[_index_online_button]->addChild(up_rate, 3, ACTION_SPRITE_TAG);
            
            //出現時のアクション
            auto *up_rate_scale_up1  = ScaleTo::create(0.05f, 1.0f);
            up_rate->runAction(Sequence::create(EaseIn::create(up_rate_scale_up1,100),nullptr));
            
            //ラベル1
            auto label1 = HMSFontUtil::createWithTTFBySystemFontArgument("称号Pt", FONT_NAME_SYSTEM, 26);
            label1->setHorizontalAlignment(TextHAlignment::CENTER);
            label1->setPosition(Point(up_rate->getContentSize().width / 2 - 5, up_rate->getContentSize().height / 2 + 20));
            label1->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
            
            //ラベル2
            auto label2 = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_point_up_rate + "倍！", FONT_NAME_SYSTEM, 26);
            label2->setHorizontalAlignment(TextHAlignment::CENTER);
            label2->setPosition(Point(up_rate->getContentSize().width / 2 , up_rate->getContentSize().height / 2 - 20));
            label2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
            
            up_rate->addChild(label1, 3, ACTION_SPRITE_TAG);
            up_rate->addChild(label2, 3, ACTION_SPRITE_TAG);
            
            //表示されている間のアクション
            auto *up_rate_scale_up  = ScaleTo::create(0.8f, 1.0f);
            auto *up_rate_scale_down = ScaleTo::create(0.8f, 0.8f);
            RepeatForever *up_rate_forever_action = RepeatForever::create(
                                                                          Sequence::create(up_rate_scale_up,
                                                                                           up_rate_scale_down,
                                                                                           nullptr)
                                                                          ) ;
            up_rate->runAction(up_rate_forever_action);
        }
	}
}

void QuestMenuLayer::createWebViewButton()
{
    //称号一覧
    auto degree_btn_off = Sprite::create("init/Home/Quest/bt_title_list.png");
    auto degree_btn_on  = Sprite::create("init/Home/Quest/bt_title_list.png");
    degree_btn_off->setColor(Color3B(180, 180, 180));
    
    auto degree_btn = MenuItemSprite::create(degree_btn_on, degree_btn_off, CC_CALLBACK_1(QuestMenuLayer::showWebPage, this));
    degree_btn->setScale(ratio);
    degree_btn->setTag(DEGREE_BUTTON_TAG);
    degree_btn->setAnchorPoint(Point(0.5, 1));
    auto Menu1 = Menu::create(degree_btn, nullptr);
    Menu1->setPosition(convertFromDisplaySize(Point(290, 838), 0.5, 1));
    this->addChild(Menu1, 5);
    
    //報酬一覧
    auto reward_btn_off = Sprite::create("init/Home/Quest/bt_reward_list.png");
    auto reward_btn_on = Sprite::create("init/Home/Quest/bt_reward_list.png");
    reward_btn_on->setColor(Color3B(180, 180, 180));
    
    auto reward_btn = MenuItemSprite::create(reward_btn_off, reward_btn_on, CC_CALLBACK_1(QuestMenuLayer::showWebPage, this));
    reward_btn->setScale(ratio);
    reward_btn->setTag(REWARD_BUTTON_TAG);
    reward_btn->setAnchorPoint(Point(0.5, 1));
    auto Menu2 = Menu::create(reward_btn, nullptr);
    Menu2->setPosition(convertFromDisplaySize(Point(430, 838), 0.5, 1));
    this->addChild(Menu2, 5);
    
    //ヘルプボタン
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, CC_CALLBACK_1(QuestMenuLayer::showWebPage, this));
    help_btn->setScale(ratio);
    help_btn->setTag(BATTLE_HELP_BUTTON_TAG);
    help_btn->setAnchorPoint(Point(0.5, 1));
    auto Menu3 = Menu::create(help_btn, nullptr);
    Menu3->setPosition(convertFromDisplaySize(Point(570, 838), 0.5, 1));
    this->addChild(Menu3, 5);
}

bool QuestMenuLayer::isTournamentEventOpen()
{
    bool tournament_event_open = false ;
    
    string err;
    Json multi_event_list = Json::parse(SaveData::loadString("multi_event_list"), err);
    int event_count = countJsonData(multi_event_list);
    
    for(int i=0; i<event_count; i++){
        string num = toString<int>(i);
        string event_type = multi_event_list[num]["event_type"].string_value() ;
        if(event_type == "tournament_event"){
            tournament_event_open = true ;
        }
    }
    
    return tournament_event_open ;
}

bool QuestMenuLayer::isRankingEventOpen()
{
    bool ranking_event_open = false ;
    
    string err;
    Json multi_event_list = Json::parse(SaveData::loadString("multi_event_list"), err);
    int event_count = countJsonData(multi_event_list);
    
    for(int i=0; i<event_count; i++){
        string num = toString<int>(i);
        string event_type = multi_event_list[num]["event_type"].string_value() ;
        if(event_type == "ranking_event"){
            ranking_event_open = true ;
        }
    }
    
    return ranking_event_open ;
}

//bool QuestMenuLayer::isBossEventOpen()
//{
//    bool boss_event_open = false ;
//    
//    string err;
//    Json multi_event_list = Json::parse(SaveData::loadString("multi_event_list"), err);
//    int event_count = countJsonData(multi_event_list);
//    
//    for(int i=0; i<event_count; i++){
//        string num = toString<int>(i);
//        string event_type = multi_event_list[num]["event_type"].string_value() ;
//        if(event_type == "boss_event"){
//            boss_event_open = true ;
//        }
//    }
//    
//    return boss_event_open ;
//}

void QuestMenuLayer::createListSprite()
{
    /* クエスト（中華統一）、オンライン、通常イベント、トーナメント、国士無双、ボスイベント、オフラインの順番 */

    //クエスト（中華統一戦記）
    _index_story_button = _image_pass.size();
    _image_pass.push_back("init/Home/Quest/bt_story.png");
    
    //オンラインバトル
    _index_online_button = _image_pass.size();
    _image_pass.push_back("init/Home/Quest/bt_online.png");
    
    //通常イベント
    _index_event_button = _image_pass.size();
    _image_pass.push_back("init/Home/Event/NpcBattleEvent/bt_event.png");
    
    //トーナメントイベント
    if(isTournamentEventOpen()) {
        _index_tournament_event_button = _image_pass.size();
        _image_pass.push_back("init/Home/Event/TournamentEvent/bt_tournament.png");
    }
    //国士無双イベント
    if(isRankingEventOpen()) {
        _index_ranking_event_button = _image_pass.size();
        _image_pass.push_back("init/Home/Event/RankingEvent/bt_contest.png");
    }
    
//    //ボスイベント
//    if(isBossEventOpen()) {
//        _index_boss_event_button = _image_pass.size();
//        _image_pass.push_back("init/Home/Event/BossEvent/bt_contest.png");
//    }

    //オフラインバトル
    _index_offline_button = _image_pass.size();
    _image_pass.push_back("init/Home/Quest/bt_offline.png");
    
    for (int i = 0; i < _image_pass.size(); i++)
    {
        if (_image_pass[i] != "")
        {
            _list_sp.push_back(Sprite::create(_image_pass[i]));
            _list_sp[i]->setTag(i + 1);
        }
    }
    
    //新しいクエスト・イベントがある場合、NEWを表示する。
	if(SaveData::loadBool("ui_notice_quest"))addNewImg(_index_story_button);
	if(SaveData::loadBool("ui_notice_event"))addNewImg(_index_event_button);

    
    //リベンジモードの場合
    if(SaveData::loadBool("revenge_mode")==true) {
        auto revenge_sprite = PlistAnimationSprite::setData((Layer *)_list_sp[_index_online_button], "download/Effect/Sprite/ef_revenge2", 0, 0.04f, 10000);
        revenge_sprite->setPosition(Point(_list_sp[_index_online_button]->getContentSize().width/2, _list_sp[_index_online_button]->getContentSize().height/2 + 60));
        //revenge_sprite->setScale(ratio);
        BlendFunc blend;//加算合成
        blend.src = GL_SRC_ALPHA;
        blend.dst = GL_ONE;
        revenge_sprite->setBlendFunc(blend);

        auto revenge_sprite2 = Sprite::create("download/Effect/Sprite/revenge.png");
        //revenge_sprite2->setScale(ratio);
        revenge_sprite2->setPosition(Point(_list_sp[_index_online_button]->getContentSize().width/2, _list_sp[_index_online_button]->getContentSize().height/2 + 60));
        _list_sp[_index_online_button]->addChild(revenge_sprite2, 10000-1);
    }
	
    //画像タッチ時に実行されるメソッドの設定
    auto func = [this](int tag){
        changePage(tag);
    };
    
    //スクロールビューの作成
    auto scroll_layer = HmsScrollLayer::create(165 * ratio, _list_sp, func);
    this->addChild(scroll_layer, 1, SCROLLLAYER_TAG);
    
//	//イベント未開催時タッチをさせなくする
//    if (_event_open_flg == false)
//    {
//        scroll_layer->disableSpriteTouch(EVENT_BUTTON_TAG);
//    }
}

Vector<SpriteFrame*> QuestMenuLayer::createSpriteFrame(string file_name)
{
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(file_name);
    
    // 配列に各フレームを格納
    Vector<SpriteFrame*> sprite_frames;
    sprite_frames.clear();//初期化
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame) break;
        
        sprite_frames.pushBack(frame);
    }
    return sprite_frames;
}

void QuestMenuLayer::setAnimateSpriteFrame(Sprite* _sprite, Vector<SpriteFrame*> sp_frames, float frame_time, float daly_time)
{
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(sp_frames, frame_time);
    auto animate   = cocos2d::Animate::create(animation);
    
    //永続的に発生させる
    Sequence* seq = Sequence::create(animate, nullptr);
    auto repeat = RepeatForever::create(seq);
    
    _sprite->runAction(repeat);
}

void QuestMenuLayer::changePage(int tag)
{
    auto normal_scene = (NormalScene *)this->getParent();

    if (tag == _index_story_button + 1) {
        //ストーリーページへ
        SaveData::saveBool("ui_notice_quest", false);
        normal_scene->changePage("NormalQuest");
    }
    else if (tag == _index_online_button + 1) {
        //オンラインページへ
        normal_scene->changePage("BattleTop");
        SaveData::saveString("battle_mode", "vs");
    }
    else if (tag == _index_event_button + 1) {
        //イベントページへ
        SaveData::saveBool("ui_notice_event", false);
        normal_scene->changePage("EventMenu");
    }
    else if (tag == _index_tournament_event_button + 1) {
        //トーナメントイベントページへ
        normal_scene->changePage("TournamentEventTop");
    }
    else if (tag == _index_ranking_event_button + 1) {
        //国士無双イベントへ
        normal_scene->changePage("RankingEventTop");
    }
//    else if (tag == _index_boss_event_button + 1) {
//        //ボスイベントへ
//        normal_scene->changePage("BossEventTop");
//    }
    else if (tag == _index_offline_button + 1) {
        //オフラインページへ
        normal_scene->changePage("OfflinePage");
    }
}

void QuestMenuLayer::showWebPage(cocos2d::Ref *sender)
{
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();
    
    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == DEGREE_BUTTON_TAG)
    {
        string url = HOST;
        url += "user/index.php?&p=page__battle_degree&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "称号一覧");
        normal_scene->addChild(WebViewLoadLayer, 9);
    }
    else if (tag == REWARD_BUTTON_TAG)
    {
        string url = HOST;
        url += "user/index.php?&p=page__reward_list&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "報酬一覧");
        normal_scene->addChild(WebViewLoadLayer, 9);
    }
    else if (tag == BATTLE_HELP_BUTTON_TAG)
    {
        string url = HOST;
        url += "user/index.php?&p=page__battle_help&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ヘルプ");
        normal_scene->addChild(WebViewLoadLayer, 9);
    }
}

bool QuestMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touchBegan! QuestMenuLayer!");
    
    return true;
}
void QuestMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void QuestMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void QuestMenuLayer::addNewImg(int number){
	CCLOG("number:%d",number);
	auto new_img = Sprite::create("init/Home/Quest/new.png");
	new_img->setScale(0.9f);
	auto repeat = RepeatForever::create(Sequence::create(
														 ScaleTo::create(0.4f, 1.1f),
														 FadeTo::create(0.2f, 170),
														 ScaleTo::create(0.4f, 0.9f),
														 FadeTo::create(0.2f, 255),
														 nullptr
														 )
										);
	new_img->runAction(repeat);
	new_img->setPosition(_list_sp[number]->getContentSize().width / 2 - 120, _list_sp[number]->getContentSize().height / 2 + 55);
	_list_sp[number]->addChild(new_img, 2 , 555);
}