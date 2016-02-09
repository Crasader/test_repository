#include "BattleDeckChoiceLayer.h"
#include "MapScene.h"

BattleDeckChoiceLayer::BattleDeckChoiceLayer()
:_touch_flg(false)
,_now_deck_no(0)
{
}

BattleDeckChoiceLayer* BattleDeckChoiceLayer::create(string page_name)
{
    BattleDeckChoiceLayer *pRet = new BattleDeckChoiceLayer();
    
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

bool BattleDeckChoiceLayer::init(string page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("BattleDeckChoiceLayer::init()");
    HMSLOG("enemy_user_master_id : %s", SaveData::loadString("enemy_user_master_id").c_str());
    HMSLOG("choice_friend_id : %s", SaveData::loadString("choice_friend_id").c_str());
    HMSLOG("battle_list_id : %d", SaveData::loadInt("battle_list_id"));
    HMSLOG("npc_battle_event_rank_list_id : %s", SaveData::loadString("npc_battle_event_rank_list_id").c_str());
    HMSLOG("ranking_event_stage_id : %s", SaveData::loadString("ranking_event_stage_id").c_str());
    HMSLOG("boss_event_stage_log_id : %s", SaveData::loadString("boss_event_stage_log_id").c_str());
    HMSLOG("challenge_event_stage_id : %s", SaveData::loadString("challenge_event_stage_id").c_str());
    HMSLOG("floor_count : %s", SaveData::loadString("floor_count").c_str());
    HMSLOG("quest_stage_id : %s", SaveData::loadString("quest_stage_id").c_str());
    
    ratio = getVisibleRatio();
    
    //バトルモードの初期化
    SaveData::saveString("battle_mode", "solo");
    
    //ページ名の取得
    _page_name = page_name;
    
    return true;
}

void BattleDeckChoiceLayer::initEventListener()
{
    listener = EventListenerTouchOneByOne::create();
    
    listener->onTouchBegan = CC_CALLBACK_2(BattleDeckChoiceLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BattleDeckChoiceLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BattleDeckChoiceLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BattleDeckChoiceLayer::showTitleBar()
{
    if (_page_name == "BattleDeckChoiceForOffline") {
        HeaderLayer::setupTitleBar("出陣最終確認", "BattleFriendChoiceForOffline", this);
    }
    else if(_page_name == "BattleDeckChoiceForEvent"){
        HeaderLayer::setupTitleBar("出陣最終確認", "BattleFriendChoiceForEvent", this);
    }
    else if(_page_name == "BattleDeckChoiceForOnline"){
        HeaderLayer::setupTitleBar("出陣最終確認", "BattleFriendChoiceForOnline", this);
    }
    else if(_page_name == "BattleDeckChoiceForQuest"){
        HeaderLayer::setupTitleBar("出陣最終確認", "BattleFriendChoiceForQuest", this);
    }
    else if(_page_name == "BattleDeckChoiceForRankingEvent"){
        HeaderLayer::setupTitleBar("出陣最終確認", "BattleFriendChoiceForRankingEvent", this);
    }
    else if(_page_name == "BattleDeckChoiceForBossEvent"){
        HeaderLayer::setupTitleBar("出陣最終確認", "BossEventRankMenu", this);
    }
    else if(_page_name == "BattleDeckChoiceForChallengeEvent"){
        HeaderLayer::setupTitleBar("出陣最終確認", "ChallengeEventRankMenu", this);
    }
}

void BattleDeckChoiceLayer::setupData()
{
    //ユーザーのパーティー情報の取得
    string json_text2 = SaveData::loadString("user_party_list");
    string err2;
    _user_party_list = Json::parse(json_text2, err2);
    
    //現在のデッキ番号の取得
    _now_deck_no = fromString<int>(SaveData::loadString("party_no"));
}

void BattleDeckChoiceLayer::createButton()
{
    //出陣ボタン
    auto btn_off = Sprite::create("init/Home/Common/bt2.png");
    auto btn_on = Sprite::create("init/Home/Common/bt2.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto text_off = HMSFontUtil::createWithTTFBySystemFontArgument("出陣する", FONT_NAME_SYSTEM, 32);
    text_off->setPosition(btn_off->getContentSize() / 2) ;
    text_off->enableOutline(Color4B(0, 0, 0, 255), 2);
    btn_off->addChild(text_off, 2);
    
    auto text_on = HMSFontUtil::createWithTTFBySystemFontArgument("出陣する", FONT_NAME_SYSTEM, 32);
    text_on->setPosition(btn_on->getContentSize() / 2) ;
    text_on->enableOutline(Color4B(0, 0, 0, 255), 2);
    btn_on->addChild(text_on, 2);
    
    auto battle_btn = MenuItemSprite::create(btn_off, btn_on, CC_CALLBACK_1(BattleDeckChoiceLayer::onTouchShutujinButton, this));
    battle_btn->setScale(ratio);
    auto menu = Menu::create(battle_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 130), 0.5, 0.5));
    this->addChild(menu, 1);
}

void BattleDeckChoiceLayer::createEnemySprite()
{
    //バトル相手のユーザー情報
    if (_page_name == "BattleDeckChoiceForQuest")
    {
        //クエスト時、クエストのスプライト作成
        string battleUserjsonText = SaveData::loadString("quest_enemy_sprite_data");
        string err2;
        Json battleUserJson = Json::parse(battleUserjsonText, err2);
        
        auto enemy_sprite = QuestEnemySprite::create(battleUserJson);
        enemy_sprite->setScale(ratio);
        enemy_sprite->setAnchorPoint(Point(0.5, 1));
        enemy_sprite->setPosition(convertFromDisplaySize(Point(320, 750), 0.5, 0.5));
        this->addChild(enemy_sprite, 3);
    }
    else if (_page_name == "BattleDeckChoiceForEvent")
    {
        //イベント敵スプライトの作成
        string battleUserjsonText = SaveData::loadString("event_enemy_sprite_data");
        string err2;
        Json battleUserJson = Json::parse(battleUserjsonText, err2);
        
        auto enemy_sprite = EventEnemySprite::create(battleUserJson);
        enemy_sprite->setScale(ratio);
        enemy_sprite->setAnchorPoint(Point(0.5, 1));
        enemy_sprite->setPosition(convertFromDisplaySize(Point(320, 750), 0.5, 0.5));
        this->addChild(enemy_sprite, 3);
    }
    else if (_page_name == "BattleDeckChoiceForRankingEvent")
    {
        //イベント敵スプライトの作成
        string battleUserjsonText = SaveData::loadString("ranking_event_enemy_sprite_data");
        string err2;
        Json battleUserJson = Json::parse(battleUserjsonText, err2);
        
        auto enemy_sprite = EventEnemySprite::create(battleUserJson);
        enemy_sprite->setScale(ratio);
        enemy_sprite->setAnchorPoint(Point(0.5, 1));
        enemy_sprite->setPosition(convertFromDisplaySize(Point(320, 750), 0.5, 0.5));
        this->addChild(enemy_sprite, 3);
    }
    else if (_page_name == "BattleDeckChoiceForBossEvent")
    {
        //イベント敵スプライトの作成
        string battleUserjsonText = SaveData::loadString("boss_event_enemy_sprite_data");
        string err2;
        Json battleUserJson = Json::parse(battleUserjsonText, err2);
        
        auto enemy_sprite = EventEnemySprite::create(battleUserJson);
        enemy_sprite->setScale(ratio);
        enemy_sprite->setAnchorPoint(Point(0.5, 1));
        enemy_sprite->setPosition(convertFromDisplaySize(Point(320, 750), 0.5, 0.5));
        this->addChild(enemy_sprite, 3);
    }
    else if (_page_name == "BattleDeckChoiceForChallengeEvent")
    {
        //イベント敵スプライトの作成
        string battleUserjsonText = SaveData::loadString("challenge_event_enemy_sprite_data");
        string err2;
        Json battleUserJson = Json::parse(battleUserjsonText, err2);
        
        auto enemy_sprite = EventEnemySprite::create(battleUserJson);
        enemy_sprite->setScale(ratio);
        enemy_sprite->setAnchorPoint(Point(0.5, 1));
        enemy_sprite->setPosition(convertFromDisplaySize(Point(320, 750), 0.5, 0.5));
        this->addChild(enemy_sprite, 3);
    }
    else {
        //オフライン敵スプライトの作成
        string battleUserjsonText = SaveData::loadString("battle_user_sprite_data");
        string err2;
        Json battleUserJson = Json::parse(battleUserjsonText, err2);
        
        auto enemy_sprite = EnemySprite::create(battleUserJson);
        enemy_sprite->setScale(ratio);
        enemy_sprite->setAnchorPoint(Point(0.5, 1));
        enemy_sprite->setPosition(convertFromDisplaySize(Point(320, 750), 0.5, 0.5));
        this->addChild(enemy_sprite, 3);
    }
}

void BattleDeckChoiceLayer::showVsSprite()
{
    //VSの画像
    auto vsSprite = Sprite::create("init/Home/Quest/battle_vs.png");
    vsSprite->setScale(ratio);
    vsSprite->setAnchorPoint(Point(0.5, 0.5));
    vsSprite->setPosition(convertFromDisplaySize(Point(320, 570), 0.5, 0.5));
    this->addChild(vsSprite, 2);
    
    //イフェクト
    ParticleSystemQuad* particle = ParticleSystemQuad::create("init/Home/Quest/vs_fire.plist");
    particle->setPosition(convertFromDisplaySize(Point(320, 520), 0.5, 0.5));
    particle->setScale(ratio) ;
    this->addChild(particle, 1);
}

void BattleDeckChoiceLayer::createDeckSprite()
{
    //デッキ画像の作成
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        _deck_sp.push_back(DeckSprite::create(i));
        _deck_sp[i]->setTag(i + 1);
        _deck_sp[i]->setScale(ratio);
        this->addChild(_deck_sp[i], 1);
        _deck_sp[i]->setVisible(false);
        
        if (i == (_now_deck_no - 1))
        {
            //現在のデッキだけ表示
            _deck_sp[i]->setPosition(convertFromDisplaySize(Point(320, 350), 0.5, 0.5));
            _deck_sp[i]->setVisible(true);
        }
    }
}

void BattleDeckChoiceLayer::createArrowSprite()
{
    //左矢印
    _left_arrow = Sprite::create("init/CharaMenu/arrow_left.png");
    _left_arrow->setScale(ratio);
    _left_arrow->setPosition(convertFromDisplaySize(Point(30, 300), 0.5, 0.5));
    this->addChild(_left_arrow, 2, T_Left_Arrow);
    
    //右矢印
    _right_arrow = Sprite::create("init/CharaMenu/arrow_right.png");
    _right_arrow->setScale(ratio);
    _right_arrow->setPosition(convertFromDisplaySize(Point(610, 300), 0.5, 0.5));
    this->addChild(_right_arrow, 2, T_Right_Arrow);
}

void BattleDeckChoiceLayer::actionArrowSprite()
{
    auto scale   = ScaleTo::create(1.5f, 0.8 * ratio);
    auto scale2  = ScaleTo::create(1.5f, 1.0 * ratio);
    auto repeat  = RepeatForever::create(Sequence::create(scale, scale2, NULL));
    _left_arrow->runAction(repeat);
    _right_arrow->runAction(repeat->clone());
}

void BattleDeckChoiceLayer::showTouchDeckSprite()
{
    //左画像
    _deck_sp[calcArrayNo(Pos::Left, _now_deck_no)]->setVisible(true);
    _deck_sp[calcArrayNo(Pos::Left, _now_deck_no)]->setPosition(convertFromDisplaySize(Point(-320, 350), 0.5, 0.5));
    
    //右画像
    _deck_sp[calcArrayNo(Pos::Right, _now_deck_no)]->setVisible(true);
    _deck_sp[calcArrayNo(Pos::Right, _now_deck_no)]->setPosition(convertFromDisplaySize(Point(960, 350), 0.5, 0.5));
}


int BattleDeckChoiceLayer::calcArrayNo(Pos position, int deck_no)
{
    //デッキ番号から配列上での番号を取得する（配列番号とデッキ番号は1ずれているので注意）
    int array_no = deck_no - 1;
    switch (position)
    {
        case Pos::Left:
        {
            //左側のデッキ番号の取得
            array_no  = deck_no - 2;
            if (array_no < 0) array_no = MAX_DECK_COUNT - 1 ; //左が0より下の場合、デッキNo最大を左に表示
        }break;
        case Pos::Right:
        {
            //右側のデッキ番号の取得
            array_no = deck_no;
            if (array_no >= MAX_DECK_COUNT) array_no = 0; //右が最大より上の場合、デッキNo最小を右に表示
        }break;
        default: break;
            
    }
    return array_no;
}

void BattleDeckChoiceLayer::moveTouchDeckSprite(float delta)
{
    //デッキ画像を移動させる
    for (int i = 0; i < _deck_sp.size(); i++)
    {
        _deck_sp[i]->setPosition(Point(_deck_sp[i]->getPosition().x + delta, _deck_sp[i]->getPosition().y));
    }
}

bool BattleDeckChoiceLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    HMSLOG("onTouchBegan BattleDeckChoiceLayer");
    
    //左右に画像を表示させる
    showTouchDeckSprite();
    
    //矢印の非表示
    _left_arrow->setVisible(false) ;
    _right_arrow->setVisible(false) ;
    
    return true;
}

void BattleDeckChoiceLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    //デッキ画像の移動
    moveTouchDeckSprite(touch->getDelta().x);
}

void BattleDeckChoiceLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    listener->setEnabled(false); //アクション中はタッチ不可にする
    
    if(touch->getStartLocation().x - touch->getLocation().x < - _contentSize.width / 4)
    {
        //右にスワイプ（左が出てくる）
        _now_deck_no--;
        if (_now_deck_no < 1) _now_deck_no = MAX_DECK_COUNT;
        
        //音の設定
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_deck_move.mp3");
    }
    else if (touch->getStartLocation().x - touch->getLocation().x > _contentSize.width / 4)
    {
        //左にスワイプ（右が出てくる）
        _now_deck_no++;
        if (_now_deck_no > MAX_DECK_COUNT) _now_deck_no = 1;
        
        //音の設定
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_deck_move.mp3");
    }
    
    //中心点からずれていれば中心に戻す
    float move_x_distance = _contentSize.width / 2 - _deck_sp[calcArrayNo(Pos::Middle, _now_deck_no)]->getPosition().x;
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        auto move = MoveBy::create(0.3f, Point(move_x_distance, 0));
        auto ease = EaseOut::create(move, 2);
        _deck_sp[i]->runAction(ease);
    }
    
    //onTouchBeginからの移動距離が少ない場合、中心のデッキをスクロールする。（下半分のときだけスクロールする）
    if (abs(touch->getLocation().x - touch->getStartLocation().x) < _contentSize.width/10){
        
        auto spWorldPoint = _deck_sp[_now_deck_no - 1]->convertToWorldSpace(Point::ZERO);
        Rect rect_under = Rect(spWorldPoint.x, spWorldPoint.y, _deck_sp[_now_deck_no - 1]->getContentSize().width * ratio, _deck_sp[_now_deck_no - 1]->getContentSize().height * ratio/2);
        
        if (rect_under.containsPoint(touch->getLocation())){
            _deck_sp[_now_deck_no - 1]->scrollAction(0.0f) ;
        }
    }
    
    auto hide_func = CallFunc::create([this](){
        
        //選択中以外のデッキを見えなくする
        for (int i = 0; i < MAX_DECK_COUNT; i++) {
            
            if (i == calcArrayNo(Pos::Middle, _now_deck_no)) continue;
            _deck_sp[i]->setVisible(false);
        }

        listener->setEnabled(true); //タッチ可能にする

        //矢印の表示
        _left_arrow->setVisible(true) ;
        _right_arrow->setVisible(true) ;
    });
    
    this->runAction(Sequence::create(DelayTime::create(0.3f), hide_func, nullptr));
    
    //選択中デッキNoの更新
    SaveData::saveString("party_no", toString<int>(_now_deck_no));
    CCLOG("deck_no = %s", SaveData::loadString("party_no").c_str());
}

void BattleDeckChoiceLayer::onTouchShutujinButton(Ref* sender){
    
    if(isEventBattle()){
        checkEventTimeRequest();
    } else if(isRankingEventBattle()){
        checkRankingEventTimeRequest();
    } else if(isBossEventBattle()){
        checkBossEventTimeRequest();
    } else {
        changeDungeonScene() ;
    }
}

void BattleDeckChoiceLayer::checkEventTimeRequest() {
    
    string url = HOST;
    
    url += "event/event_open_check.php";
    
    auto selector = httpresponse_selector(BattleDeckChoiceLayer::checkEventTimeCallbabck);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void BattleDeckChoiceLayer::checkEventTimeCallbabck(HttpClient *sender, HttpResponse *response){
	string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー時終了
    if(result=="not connect"||result=="response NG") return;
    
//	HMSLOG("Result %s", result.c_str());
	
	string text = result;
	string err;
	
    if(text=="event_open"){
        changeDungeonScene() ;
    }else{
        
        //未開催時
        auto normal_scene = (NormalScene *)this->getParent();
        
        auto func = [normal_scene](){
            normal_scene->changePage("QuestMenu");
        };
        
        auto popup_layer = PopupOkButtonLayer::create(func,"選択されたイベントは現在未開催です。") ;
        normal_scene->addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
}

void BattleDeckChoiceLayer::checkRankingEventTimeRequest() {
    
    string url = HOST;
    url += "event/ranking_event_open_check.php";
    
    auto selector = httpresponse_selector(BattleDeckChoiceLayer::checkRankingEventTimeCallbabck);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void BattleDeckChoiceLayer::checkRankingEventTimeCallbabck(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー時終了
    if(result=="not connect"||result=="response NG") return;
    
    //	HMSLOG("Result %s", result.c_str());
    
    string text = result;
    string err;
    
    if(text=="event_open"){
        changeDungeonScene() ;
    }else{
        
        //未開催時
        auto normal_scene = (NormalScene *)this->getParent();
        
        auto func = [normal_scene](){
            normal_scene->changePage("QuestMenu");
        };
        
        auto popup_layer = PopupOkButtonLayer::create(func,"選択されたイベントは現在未開催です。") ;
        normal_scene->addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
}
void BattleDeckChoiceLayer::checkBossEventTimeRequest() {
    
    string url = HOST;
    url += "event/boss_event_open_check.php";
    
    auto selector = httpresponse_selector(BattleDeckChoiceLayer::checkBossEventTimeCallbabck);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void BattleDeckChoiceLayer::checkBossEventTimeCallbabck(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー時終了
    if(result=="not connect"||result=="response NG") return;
    
    //	HMSLOG("Result %s", result.c_str());
    
    string text = result;
    string err;
    
    if(text=="event_open"){
        //changeDungeonScene() ;
        //タップ禁止レイヤー生成
        auto normal_scene = (NormalScene *)getParent();
        normal_scene->addTapDisableLayer();
        
        //黒いスプライトを画面いっぱいにかぶせる
        auto spr = Sprite::create();
        spr->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
        spr->setTextureRect(Rect(0, 0, _contentSize.width, _contentSize.height));
        spr->setColor(Color3B::BLACK);
        spr->setGlobalZOrder(1); //zOrderに関係無く強制的に前面に表示するメソッド よっぽどのことが無ければ基本的に使用しない方がよい
        spr->setOpacity(0) ;
        addChild(spr,99999);
        
        FadeIn *fade = FadeIn::create(2.0f) ;
        
        auto go_dungeon_func = CallFunc::create([this](){
            this->stopAllActions() ;
            
            SaveData::saveString("party_no", toString<int>(_now_deck_no));
            
            //ダンジョンシーンへの遷移
            auto scene = MapScene::createScene();
            auto transition = TransitionFade::create(2.0f, scene);
            Director::getInstance()->replaceScene(transition);
        });
        auto pause_music_func= CallFunc::create([this](){
            CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic() ;
        });
        
        //SE(シャキーンの音)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/shutujin.mp3",false);
        //音を1.5秒にかけて小さくし、その後BGMを停止し、最後にボリュームを通常時と同じ値に直す
        this->runAction(Sequence::create(FadeToVolumeOfBGM::create(1.5f, 0.0f), pause_music_func, FadeToVolumeOfBGM::create(0.0f, SaveData::loadFloat("bgm_volume")),nullptr));
        //画面を暗くしてから遷移する
        spr->runAction(Sequence::createWithTwoActions(fade, go_dungeon_func));
    }else{
        
        //未開催時
        auto normal_scene = (NormalScene *)this->getParent();
        
        auto func = [normal_scene](){
            normal_scene->changePage("QuestMenu");
        };
        
        auto popup_layer = PopupOkButtonLayer::create(func,"選択されたイベントは現在未開催です。") ;
        normal_scene->addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
    }
}

void BattleDeckChoiceLayer::changeDungeonScene()
{
    //タップ禁止レイヤー生成
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer();
    
    //黒いスプライトを画面いっぱいにかぶせる
    auto spr = Sprite::create();
    spr->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
    spr->setTextureRect(Rect(0, 0, _contentSize.width, _contentSize.height));
    spr->setColor(Color3B::BLACK);
    spr->setGlobalZOrder(1); //zOrderに関係無く強制的に前面に表示するメソッド よっぽどのことが無ければ基本的に使用しない方がよい
    spr->setOpacity(0) ;
    addChild(spr,99999);
    
    FadeIn *fade = FadeIn::create(2.0f) ;
    
    auto go_dungeon_func = CallFunc::create([this](){
        this->stopAllActions() ;
        
        SaveData::saveString("party_no", toString<int>(_now_deck_no));
        
        //ダンジョンシーンへの遷移
        auto scene = DungeonScene::createScene();
        auto transition = TransitionFade::create(2.0f, scene);
        Director::getInstance()->replaceScene(transition);
    });
    auto pause_music_func= CallFunc::create([this](){
        CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic() ;
    });
    
    //SE(シャキーンの音)
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/shutujin.mp3",false);
    //音を1.5秒にかけて小さくし、その後BGMを停止し、最後にボリュームを通常時と同じ値に直す
    this->runAction(Sequence::create(FadeToVolumeOfBGM::create(1.5f, 0.0f), pause_music_func, FadeToVolumeOfBGM::create(0.0f, SaveData::loadFloat("bgm_volume")),nullptr));
    //画面を暗くしてから遷移する
    spr->runAction(Sequence::createWithTwoActions(fade, go_dungeon_func));
}

bool BattleDeckChoiceLayer::isEventBattle(){
    bool isEvent = true ;
    string npc_id_str = SaveData::loadString("npc_battle_event_rank_list_id") ;
    if(npc_id_str=="0" || npc_id_str==""){
        isEvent = false ;
    }
    return isEvent;
}

bool BattleDeckChoiceLayer::isRankingEventBattle(){
    bool is_ranking_event = true ;
    string stage_id_str = SaveData::loadString("ranking_event_stage_id") ;
    if(stage_id_str=="0" || stage_id_str==""){
        is_ranking_event = false ;
    }
    return is_ranking_event;
}

bool BattleDeckChoiceLayer::isBossEventBattle(){
    bool is_boss_event = true ;
    string stage_id_str = SaveData::loadString("boss_event_stage_log_id") ;
    if(stage_id_str=="0" || stage_id_str==""){
        is_boss_event = false ;
    }
    return is_boss_event;
}

bool BattleDeckChoiceLayer::isChallengeEventBattle(){
    bool is_challenge_event = true ;
    string rank_list_id_str = SaveData::loadString("challenge_event_stage_id") ;
    if(rank_list_id_str=="0" || rank_list_id_str==""){
        is_challenge_event = false ;
    }
    return is_challenge_event;
}

void BattleDeckChoiceLayer::onEnter()
{
    Layer::onEnter();
    
    //初期データの読み込み
    setupData();
    
    //イベントリスナーの作成
    initEventListener();
    
    //タイトルバーの作成
    showTitleBar();
    
    //敵画像の作成
    createEnemySprite();
    
    //vs画像の表示
    showVsSprite();
    
    //デッキ画像の作成
    createDeckSprite();
    
    //出陣ボタンの作成
    createButton();
    
    //左右の矢印ボタンの作成
    createArrowSprite();
    
    //矢印ボタンアクション
    actionArrowSprite();
}
