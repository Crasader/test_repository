#include "HeaderLayer.h"

HeaderLayer::HeaderLayer()
:_rank_name()
,_user_name()
,_gold()
,_jewel()
,_now_battle_point(0)
,_max_battle_point(0)
,_now_rank_point(0)
,_need_rank_point(0)
,_now_reward_point(0)
,_need_reward_point(0)
,_check_time(0)
,_power_use_time(0)
,_server_diff_time(0)
{
}

bool HeaderLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("HeaderLayer::init()");
    
    ratio = getVisibleRatio();
    
    //サーバーとの差分時間を取得
    _server_diff_time = SaveData::loadInt("server_diff_time");
    
    return true;
}

void HeaderLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(HeaderLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HeaderLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HeaderLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(HeaderLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void HeaderLayer::loadData()
{
    //データの読み込み
    setRankName(SaveData::loadString("rank_name"));
    setUserName(SaveData::loadString("user_name"));
    setGold(SaveData::loadString("gold"));
    setJewel(SaveData::loadString("jewel"));
    setLevel(fromString<int>(SaveData::loadString("level")));
    setNowBattlePoint(fromString<int>(SaveData::loadString("power")));
    setMaxBattlePoint(fromString<int>(SaveData::loadString("max_power")));
    setNowRankPoint(fromString<int>(SaveData::loadString("rank_point")));
    setNeedRankPoint(fromString<int>(SaveData::loadString("need_rank_point")));
    setNowRewardPoint(fromString<int>(SaveData::loadString("reward_point")));
    setNeedRewardPoint(fromString<int>(SaveData::loadString("need_reward_point")));    
}

void HeaderLayer::showBg()
{
    //pointバーの背景作成
    auto point_bg = Sprite::create("init/Header/header_g.png");
    point_bg->setScale(ratio);
    point_bg->setAnchorPoint(Point(0, 1));
    point_bg->setPosition(convertFromDisplaySize(Point(0, 960), 0.5, 1));
    this->addChild(point_bg, Z_Point_Bg, T_Point_Bg);
    
    //ヘッダー背景作成
    auto header_bg = Sprite::create("init/Header/header.png");
    header_bg->setScale(ratio);
    header_bg->setAnchorPoint(Point(0, 1));
    header_bg->setPosition(convertFromDisplaySize(Point(0, 960), 0.5, 1));
    this->addChild(header_bg, Z_Header_Bg, T_Header_Bg);

}

void HeaderLayer::setFrameData()
{
    //フレームデータの設定
    auto header_bg = (Sprite *)this->getChildByTag(T_Header_Bg);
    
    //現在のランキングなどを載せるフレーム
    auto frame = Sprite::create("init/Header/frame.png");
    frame->setPosition(Point(convertFromDisplaySize(Point(320, 960 - header_bg->getContentSize().height / 2 - 32), 0.5, 1)));
    frame->setScale(ratio);
    frame->setOpacity(0); //最初見えないように設定
    frame->setCascadeOpacityEnabled(true); //子にもフェードを適用
    this->addChild(frame, Z_Frame, T_Frame);
    
//    //今週の順位
//    string weekly_rank = SaveData::loadString("weekly_rank");
//    if (weekly_rank == "") {
//        weekly_rank = "--";
//    }
//    auto weekly_ranking_label = HMSFontUtil::createWithTTFBySystemFontArgument("今週の順位 : " + weekly_rank + "位", FONT_NAME_SYSTEM, 20);
//    weekly_ranking_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
//    weekly_ranking_label->setAnchorPoint(Point(0, 0.5));
//    weekly_ranking_label->setPosition(Point(16, frame->getContentSize().height * 4 / 5));
//    frame->addChild(weekly_ranking_label, 1);
//    
//    //今週の獲得Pt
//    string total_weekly_rank_point = SaveData::loadString("weekly_rank_point");
//    if (total_weekly_rank_point == "") {
//        total_weekly_rank_point = "--";
//    }
//    auto total_weekly_rank_point_label = HMSFontUtil::createWithTTFBySystemFontArgument("今週の獲得称号Pt  : " + total_weekly_rank_point, FONT_NAME_SYSTEM, 20);
//    total_weekly_rank_point_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
//    total_weekly_rank_point_label->setAnchorPoint(Point(0, 0.5));
//    total_weekly_rank_point_label->setPosition(Point(16, frame->getContentSize().height * 3 / 5));
//    frame->addChild(total_weekly_rank_point_label, 1);
    
    //トータル順位
    string total_ranking = SaveData::loadString("total_rank");
    if (total_ranking == "") {
        total_ranking = "--";
    }
    auto total_ranking_label = HMSFontUtil::createWithTTFBySystemFontArgument("総合順位    : " + total_ranking + "位", FONT_NAME_SYSTEM, 20);
    total_ranking_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    total_ranking_label->setAnchorPoint(Point(0, 0.5));
    total_ranking_label->setPosition(Point(16, frame->getContentSize().height * 2 / 3));
    frame->addChild(total_ranking_label, 1);
    
    //合計称号Pt
    string total_rank_point = SaveData::loadString("total_rank_point");
    auto total_rank_point_label = HMSFontUtil::createWithTTFBySystemFontArgument("合計獲得称号Pt : " + total_rank_point, FONT_NAME_SYSTEM, 20);
    total_rank_point_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    total_rank_point_label->setAnchorPoint(Point(0, 0.5));
    total_rank_point_label->setPosition(Point(16, frame->getContentSize().height * 1 / 3));
    frame->addChild(total_rank_point_label, 1);
    
//    //合計経験値
//    string total_reward_point = SaveData::loadString("total_reward_point");
//    auto total_reward_point_label = HMSFontUtil::createWithTTFBySystemFontArgument("合計獲得経験値  : " + total_reward_point, FONT_NAME_SYSTEM, 20);
//    total_reward_point_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
//    total_reward_point_label->setAnchorPoint(Point(0, 0.5));
//    total_reward_point_label->setPosition(Point(16, frame->getContentSize().height * 1 / 5));
//    frame->addChild(total_reward_point_label, 1);
    
    
    /*
    if(Utility::isTest()){
        //デバッグ用
        Rect rect = Rect(0, 0, _contentSize.width, _contentSize.height);
        
        auto debug_frame = Sprite::create();
        debug_frame->setCascadeOpacityEnabled(true) ;
        debug_frame->setColor(Color3B::BLACK) ;
        debug_frame->setTextureRect(rect) ;
        debug_frame->setAnchorPoint(Point(0, 1)) ;
        frame->addChild(debug_frame);
        
        HmsVector<string> debug_messages ;
        debug_messages.push_back("user_master_id: "+SaveData::loadString("user_master_id")) ;
        
        for(int i=0; i<debug_messages.size(); i++){
            string debug_message = debug_messages.at(i);
            auto total_rank_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(debug_message, FONT_NAME_SYSTEM, 20);
            total_rank_point_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
            total_rank_point_label->setAnchorPoint(Point(0, 1));
            total_rank_point_label->setPosition(Point(16, debug_frame->getContentSize().height - i*34));
            debug_frame->addChild(total_rank_point_label, 1);
        }
        
    }
     */
}

void HeaderLayer::showData(){
    
    auto level = createLevelNumber(_level) ;
    level->setScale(ratio) ;
    level->setPosition(convertFromDisplaySize(Point(27, 891), 0.5, 1));
    this->addChild(level, Z_Label) ;
    
    //称号名
    auto rank_name = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_name.c_str(), FONT_NAME_SYSTEM, 17);
    rank_name->setScale(ratio);
    rank_name->setAnchorPoint(Point(0, 0.5));
    rank_name->setPosition(convertFromDisplaySize(Point(444, 866), 0.5, 1));
    rank_name->setTextColor(Color4B(255, 204, 0, 255));
    this->addChild(rank_name, Z_Label);

    //ユーザー名（最大7文字）
    if (_user_name == "") {
        _user_name = "NO_NAME";
    }
    auto user_name = HMSFontUtil::createWithTTFBySystemFontArgument(_user_name.c_str(), FONT_NAME_SYSTEM, 24);
//    user_name->setHorizontalAlignment(TextHAlignment::LEFT);
    user_name->setScale(ratio);
    user_name->setAnchorPoint(Point(0, 0.5));
    user_name->setPosition(convertFromDisplaySize(Point(80, 895), 0.5, 1));
    this->addChild(user_name, Z_Label, NAME_LABEL_TAG);
    
    //資金
    if (fromString<int>(_gold) > MAX_GOLD_COUNT) {
        _gold = MAX_GOLD_COUNT;
    }
    
    int gold_font_size = 24;
    if (fromString<int>(_gold) > 100000){
        gold_font_size = 22 ; //6桁でフォントサイズを小さくする
    }
    
    auto gold = HMSFontUtil::createWithTTFBySystemFontArgument(_gold.c_str(), FONT_NAME_SYSTEM, gold_font_size);
    gold->setHorizontalAlignment(TextHAlignment::RIGHT);
    gold->setScale(ratio);
    gold->setAnchorPoint(Point(1, 0.5));
    gold->setPosition(convertFromDisplaySize(Point(436, 895), 0.5, 1));
    this->addChild(gold, Z_Label, GOLD_LABEL_TAG);
    
    //宝玉（オーブ）
    if (fromString<int>(_jewel) > MAX_JEWEL_COUNT) {
        _jewel = MAX_JEWEL_COUNT;
    }
    auto jewel = HMSFontUtil::createWithTTFBySystemFontArgument(_jewel.c_str(), FONT_NAME_SYSTEM, 22);
    jewel->setHorizontalAlignment(TextHAlignment::RIGHT);
    jewel->setScale(ratio);
    jewel->setAnchorPoint(Point(1.0, 0.5));
    jewel->setPosition(convertFromDisplaySize(Point(577, 895), 0.5, 1));
    this->addChild(jewel, Z_Label, JEWEL_LABEL_TAG);
    
    //称号ポイント（文字）
    //string rank_point_text = toString(_now_rank_point) + "/" + toString(_need_rank_point); //← 分数で表示する場合(今後変更の可能性あり)
    string rank_point_text = "あと" + toString(_need_rank_point - _now_rank_point); //← 必要な残りのポイントを表示する場合(今後変更の可能性あり)
    
    if (_need_rank_point == 0)
    {
        rank_point_text = "最大";
    }
    auto rank_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(rank_point_text, FONT_NAME_SYSTEM, 17);
//    rank_point_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    rank_point_label->setScale(ratio);
    rank_point_label->setAnchorPoint(Point(1, 0.5));
    rank_point_label->setPosition(convertFromDisplaySize(Point(614, 866), 0.5, 1));
    this->addChild(rank_point_label, Z_Label);
    
    //称号ポイントゲージ
    float rank_point_percent = 1.0f;
    if (_need_rank_point != 0)
    {
        rank_point_percent = ((float)_now_rank_point / _need_rank_point);
    }
    auto rank_point_bar = Sprite::create("init/Header/gauge_orange.png");
    rank_point_bar->setScale(ratio);
    rank_point_bar->setScaleX(rank_point_percent*ratio);
    rank_point_bar->setAnchorPoint(Point(0, 0.5));
    rank_point_bar->setPosition(convertFromDisplaySize(Point(507, 844), 0.5, 1));
    
    if(_now_rank_point<21)//降格危機の時バーを点滅させる
    {
        auto fadeIn  = FadeIn::create(1.2f);
        auto fadeOut = FadeOut::create(1.2f);
        auto seq     = Sequence::create(fadeIn, fadeOut, NULL);
        auto repeat  = RepeatForever::create(seq);
        rank_point_bar->runAction(repeat);
    }
    
    this->addChild(rank_point_bar, Z_Point_Bar);

    //報酬ポイント（文字）
    //string reward_point_text = toString(_now_reward_point) + "/" + toString(_need_reward_point); //← 分数で表示する場合(今後変更の可能性あり)
    string reward_point_text = "あと " + toString(_need_reward_point - _now_reward_point) ; //← 必要な残りのポイントを表示する場合(今後変更の可能性あり)
    if (_need_reward_point == 0)
    {
        reward_point_text = "最大";
    }
    auto reward_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(reward_point_text, FONT_NAME_SYSTEM, 17);
//    reward_point_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    reward_point_label->setScale(ratio);
    reward_point_label->setAnchorPoint(Point(1, 0.5));
    reward_point_label->setPosition(convertFromDisplaySize(Point(216, 866), 0.5, 1));
    this->addChild(reward_point_label, Z_Label);
    
    //報酬ポイントゲージ
    float reward_point_percent = 1.0f;
    if (_need_reward_point != 0)
    {
        reward_point_percent = ((float)_now_reward_point / _need_reward_point);
    }
    auto reward_point_bar = Sprite::create("init/Header/gauge_green.png");
    reward_point_bar->setScale(ratio);
    reward_point_bar->setScaleX(reward_point_percent*ratio);
    reward_point_bar->setAnchorPoint(Point(0, 0.5));
    reward_point_bar->setPosition(convertFromDisplaySize(Point(108, 844), 0.5, 1));
    this->addChild(reward_point_bar, Z_Point_Bar);
    
    //宝玉ボタン
    auto stone_shop_off = Sprite::create("init/Home/Mypage/header_plus.png");
    auto stone_shop_on  = Sprite::create("init/Home/Mypage/header_plus.png");
    stone_shop_on->setColor(Color3B(180, 180, 180));
    
    auto stone_shop_btn = MenuItemSprite::create(stone_shop_off, stone_shop_on, [this](Ref* sender)
    {
        //毎回年齢確認を表示させる
        auto normal_scene = (NormalScene *)this->getParent();
        auto popup_layer = StoneShopAgeConfirmLayer::create();
        normal_scene->addChild(popup_layer, NormalScene::Z_Popup_Layer, NormalScene::T_Popup_Layer);
        
//        auto normal_scene = (NormalScene *)this->getParent();
//        normal_scene->changePage("StoneShop");
    });
    stone_shop_btn->setScale(ratio*0.8);
    stone_shop_btn->setAnchorPoint(Point(0.5, 0.5));
    auto Menu4 = Menu::create(stone_shop_btn, nullptr);
    Menu4->setPosition(convertFromDisplaySize(Point(605, 895), 0.5, 1));
    this->addChild(Menu4, Z_Stone_Shop_Button);

}

Node *HeaderLayer::createLevelNumber(int level_num){
    Node *node = Node::create() ;
    int digit = calcDigit(level_num) ;
    
    //桁数によってスケールを変更する(4桁まで想定)
    float scale = 1.0 ;
    if(digit==3)scale = 0.8 ;
    if(digit>=4)scale = 0.6 ;
    
    int level_size = 23 * scale ;
    
    for(int i=0 ; i<digit ; i++){
        int num = level_num % 10;
        Sprite *s_num = Sprite::create("init/Header/lv_num"+ toString<int>(num)+ ".png");
        s_num->setScale(scale);
        node->addChild(s_num);
        
        Point point = Point((0.5*(digit-1) - i)*level_size,0) ;
        s_num->setPosition(point);
        
        level_num = level_num / 10 ;
    }
    
    return node ;
}

int HeaderLayer::calcDigit(int num){
    
    if(num < 0 ){
        HMSLOG("レベルが負の値なのでおかしい: %d", num) ;
        num = 0 ;
    }
    
    int count = 1;
    while(true){
        num = num / 10.0 ;
        if(num < 1.0){
            break ;
        }
        count++ ;
    }
    
    return count ;
}

void HeaderLayer::createBattlePoint()
{
    //バトルポイント（文字）
    _battle_point_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 17);
    _battle_point_label->setScale(ratio);
    _battle_point_label->setAnchorPoint(Point(1, 0.5));
    _battle_point_label->setPosition(convertFromDisplaySize(Point(413, 866), 0.5, 1));
    this->addChild(_battle_point_label, Z_Label) ;
    
    _battle_point_gauge = Sprite::create("init/Header/gauge_red.png");
    _battle_point_gauge->setScale(ratio);
    _battle_point_gauge->setScaleX(0);
    _battle_point_gauge->setAnchorPoint(Point(0, 0.5));
    _battle_point_gauge->setPosition(convertFromDisplaySize(Point(304, 844), 0.5, 1));
    this->addChild(_battle_point_gauge, Z_Point_Bar);
}

void HeaderLayer::createBattlePointRecoveryTime()
{
    //バトルポイント回復時間の作成
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 15);
    label->setTextColor(Color4B::RED);
    label->setScale(ratio);
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(convertFromDisplaySize(Point(242, 866), 0.5, 1));
    label->enableOutline(Color4B::BLACK, 1) ;
    this->addChild(label, Z_Label, T_Recovery_Label);
}

void HeaderLayer::createTimeStamp()
{
    //初期時間の作成
    time_t now_time;
    time(&now_time);
    
    //現在時刻の_check_timeを端末時間からサーバーとの差の時間を引いたものに設定
    _check_time = now_time - _server_diff_time;
    
//    //最後にバトルポイントを使った時間のタイムスタンプを取得
//    struct tm tm;
//    strptime(SaveData::loadString("power_use_time").c_str(), "%Y-%m-%d %H:%M:%S", &tm);
//    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
//    _power_use_time = mktime(&tm);
    
    _power_use_time = fromString<time_t>(SaveData::loadString("timestamp_power_use_time"));
}

void HeaderLayer::updateNowBattlePoint()
{
    //バトルポイントの更新処理
    if ( _now_battle_point < _max_battle_point)
    {
        int add_point = difftime(_check_time, _power_use_time) / POWER_HEAL_TIME;
        HMSLOG("add_point = %d", add_point);
        
        //バトルポイントの更新
        _now_battle_point = _now_battle_point + add_point;
        
        //メンバ変数の更新
        _power_use_time = _power_use_time + (add_point * POWER_HEAL_TIME);
        
        if (_now_battle_point > _max_battle_point)
        {
            _now_battle_point = _max_battle_point;
        }
    }
    else
    {
        _now_battle_point = _max_battle_point;
    }
    
    //現在の体力更新
    SaveData::saveString("power", toString<int>(_now_battle_point));
    SaveData::saveString("timestamp_power_use_time", toString<time_t>(_check_time));
}

void HeaderLayer::showBattlePoint()
{
    
    //バトルポイントのラベル更新
    string battle_point_text = toString(_now_battle_point) + "/" + toString(_max_battle_point);
    _battle_point_label->setString(battle_point_text) ;
    
    //バトルポイントのゲージ更新
    float battle_point_percent = 1.0f;
    if (_max_battle_point != 0) {
        battle_point_percent = ((float)_now_battle_point/ _max_battle_point);
    }
    
    _battle_point_gauge->setScaleX(battle_point_percent*ratio);
    
}

void HeaderLayer::update(float dt)
{
    //現在時刻の取得
    time_t now_time;
    time(&now_time);
//    HMSLOG("now_time = %ld", now_time);
    
    //端末時刻とサーバー時間の差を加える
    now_time = now_time - _server_diff_time;
    
    //１秒計測の処理
    if (0 < difftime(now_time, _check_time))
    {
        //体力が満タンになったら止める
        if (_now_battle_point == _max_battle_point)
        {
            this->unscheduleUpdate();
            
            //パワーの更新
            SaveData::saveString("power", toString<int>(_max_battle_point));
            SaveData::saveString("timestamp_power_use_time", toString<time_t>(now_time));
            
            //回復時間の削除
            auto label = (Label *)this->getChildByTag(T_Recovery_Label);
            if (label)
            {
                label->removeFromParent();
            }
        }
        else
        {
            _check_time = now_time;
            this->scheduleOnce(schedule_selector(HeaderLayer::setRecoveryTime), 0);
        }
    }
}

void HeaderLayer::setRecoveryTime(float dt)
{
    auto label = (Label *)this->getChildByTag(T_Recovery_Label);
    
    //体力残り回復時間の取得
    time_t remain_time = POWER_HEAL_TIME - (_check_time - _power_use_time);
//    HMSLOG("remain_time = %ld", remain_time);
    
    int minute = (int)remain_time / 60; //分
    int second = (int)remain_time % 60; //秒
//    HMSLOG("minute = %d, second = %d", minute ,second);
    
    //2桁で表示させる処理
    if (minute >= 0 && second >= 0)
    {
        string second_text = toString<int>(second);
        if (second < 10)
        {
            second_text = "0" + toString<int>(second);
        }
        
        string remain_text = "あと" + toString<int>(minute) + ":" + second_text;
        label->setString(remain_text);
    }
    
    if (remain_time <= 0)
    {
        //体力回復
        int add_point = 1;
        if (0 < abs(remain_time / POWER_HEAL_TIME))
        {
            //スリープ時の体力が１つ以上増えた場合の対策処理
            add_point += (int)abs(remain_time / POWER_HEAL_TIME);
        }
        
        _now_battle_point += add_point;
        
        if (_now_battle_point > _max_battle_point)
        {
            _now_battle_point = _max_battle_point;

        }
        SaveData::saveString("power", toString<int>(_now_battle_point));
        
        //最後に体力を使った時間の更新
        _power_use_time += (POWER_HEAL_TIME * add_point);
        if (_power_use_time > _check_time)
        {
            _power_use_time = _check_time;
        }
        SaveData::saveString("timestamp_power_use_time", toString<time_t>(_power_use_time));
        
        //回復アクション
        recoveryAction();
    }
}

void HeaderLayer::recoveryAction()
{
    //バトルポイント回復アクション
    int battle_point = fromString<int>(SaveData::loadString("power"));
    
    //バトルポイントのラベル更新
    string battle_point_text = toString(battle_point) + "/" + toString(_max_battle_point);
    _battle_point_label->setString(battle_point_text) ;
    
    //バトルポイントのゲージ更新
    float battle_point_percent = 1.0f;
    if (_max_battle_point != 0) {
        battle_point_percent = ((float)battle_point/ _max_battle_point);
    }
    _battle_point_gauge->runAction(ScaleTo::create(1.0f, battle_point_percent*ratio, ratio) ) ;
    
}

void HeaderLayer::setMaxBattlePointAction()
{
    //スケジューラのストップ
    this->unscheduleAllCallbacks();
    
    //バトルポイントのラベル更新(最大値/最大値という数値にする)
    string battle_point_text = toString(_max_battle_point) + "/" + toString(_max_battle_point);
    _battle_point_label->setString(battle_point_text) ;
    
    //バトルポイントのゲージ更新(ゲージを満タンにする)
    float battle_point_percent = 1.0f;
    _battle_point_gauge->runAction(ScaleTo::create(1.0f, battle_point_percent*ratio, ratio) ) ;
    
    //回復時間の削除
    auto label = (Label *)this->getChildByTag(T_Recovery_Label);
    if (label)
    {
        label->removeFromParent();
    }
}

void HeaderLayer::setupTitleBar(string titlename, string return_page_name, Layer* page)
{
    HeaderLayer::setupTitleBar(titlename, return_page_name, page, TitleBarType::WIDTH_466) ;
}

void HeaderLayer::setupTitleBar(string titlename, string return_page_name, Layer* page, TitleBarType titleBarType)
{
    HeaderLayer::setupTitleBar(titlename, return_page_name, page, titleBarType, TitleBarPosition::BELOW_HEADER) ;
}

void HeaderLayer::setupTitleBar(string titlename, string return_page_name, Layer* page, TitleBarType titleBarType, TitleBarPosition titleBarPosition)
{
    float ratio = getVisibleRatio();
    
    auto title_bar_layer = page->getParent()->getChildByTag(TITLE_LAYER_TAG);
    
    //タイトルバーの長さをtitleBarTypeによって条件分岐
    string title_bar_path = "" ;
    if(titleBarType == TitleBarType::WIDTH_466) title_bar_path = "init/Header/title_base.png" ;
    if(titleBarType == TitleBarType::WIDTH_390) title_bar_path = "init/Header/title_base3.png" ;
    if(titleBarType == TitleBarType::WIDTH_324) title_bar_path = "init/Header/title_base4.png" ;
    if(titleBarType == TitleBarType::WIDTH_215) title_bar_path = "init/Header/title_base2.png" ;
    if(titleBarType == TitleBarType::WIDTH_348) title_bar_path = "init/Header/title_base5.png" ;
    
    //タイトルバーの場所を
    float header_pos_y = 0 ;
    if(titleBarPosition==TitleBarPosition::BELOW_HEADER) header_pos_y = 789 ;
    if(titleBarPosition==TitleBarPosition::ALIGN_LAYER_TOP) header_pos_y = 920 ;
    
    //ページタイトルバーの作成
    auto title_bar = Sprite::create(title_bar_path);
    title_bar->setScale(ratio);
    title_bar->setPosition(convertFromDisplaySize(Point(236, header_pos_y), 0.5, 1));
    title_bar_layer->addChild(title_bar, TitleBarLayer::Z_Title_Bar, TitleBarLayer::T_Title_Bar);
    
    //戻るボタン
    auto back_off = Sprite::create("init/Header/bt_return.png");
    auto back_on = Sprite::create("init/Header/bt_return.png");
    back_on->setColor(Color3B(180, 180, 180));
    
    auto back_btn = MenuItemSprite::create(back_off, back_on, [page, return_page_name](Ref* sender){
        //ページ遷移
        auto normal_scene = (NormalScene *)page->getParent();
        normal_scene->changePage(return_page_name);
    });
    back_btn->setScale(ratio);
    auto Menu = Menu::create(back_btn, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(58, header_pos_y), 0.5, 1));
    title_bar_layer->addChild(Menu, ZORDER_BACK_BUTTON, BACK_BUTTON_TAG);
    
    //ページ名
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument(titlename, FONT_NAME_SYSTEM, 32);
    title_label->setScale(ratio);
    title_label->setAnchorPoint(Point(0, 0.5));
    title_label->setPosition(convertFromDisplaySize(Point(111, header_pos_y), 0.5, 1));
    title_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    title_bar_layer->addChild(title_label, TitleBarLayer::Z_Title_Label, TitleBarLayer::T_Title_Label);
}

//void HeaderLayer::setTitleBarAddSprite(Layer* layer, cocos2d::Node *node, bool menu_flg)
//{
//    auto title_bar_layer = layer->getParent()->getChildByTag(TITLE_LAYER_TAG);
//    auto title_bar = title_bar_layer->getChildByTag(TitleBarLayer::T_Title_Bar);
//    if (title_bar) {
//        if (menu_flg == true) {
//            node->setPosition(Point(title_bar->getContentSize().width / 2 + 80, title_bar->getContentSize().height / 2));
//        }
//        else {
//            node->setAnchorPoint(Point(1.0, 0.5));
//            node->setPosition(Point(title_bar->getContentSize().width - 20, title_bar->getContentSize().height / 2));
//        }
//        title_bar->addChild(node);
//    }
//}

void HeaderLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //データの読み込み
    loadData();
    
    //ヘッダー背景の表示
    showBg();
    
    //データの表示
    showData();

    //バトルポイントの作成
    createBattlePoint();
    
    //バトルポイント回復時間の作成
    createBattlePointRecoveryTime();
    
    //タイムスタンプの作成
    createTimeStamp();
    
    //現在のバトルポイントの更新
    updateNowBattlePoint();
    
    //バトルptの表示
    showBattlePoint();
    
    //フレームデータの設定
    setFrameData();
    
    //バトルポイント更新処理
    this->scheduleUpdate();
}

bool HeaderLayer::onTouchBegan(Touch *touch, Event *event)
{
    auto bg = (Sprite *)this->getChildByTag(T_Header_Bg);
    auto rect = bg->getBoundingBox();
    if (rect.containsPoint(touch->getLocation()))
    {
        //タッチがヘッダーを触っていたとき表示
        auto frame = (Sprite *)this->getChildByTag(T_Frame);
        auto fade = FadeIn::create(0.05f);
        frame->runAction(fade);
        
        frame->setPosition(Point(calcFrameX(touch), frame->getPosition().y));
        
        
        return true;
    }
    
    return false;
}

void HeaderLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto frame = (Sprite *)this->getChildByTag(T_Frame);
    frame->setPosition(Point(calcFrameX(touch), frame->getPosition().y));
}

void HeaderLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //小ポップアップを見えなくする
    auto frame = (Sprite *)this->getChildByTag(T_Frame);
    auto fade = FadeOut::create(0.05f);
    frame->runAction(fade);
}

void HeaderLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タップエンドと同じ処理をする
    onTouchEnded(touch, event);
}


float HeaderLayer::calcFrameX(cocos2d::Touch *touch)
{
    //フレームのポジションを計算する
    float target_x;
    if (touch->getLocation().x < _contentSize.width / 2)
    {
        //画面より左側をタッチした場合
        target_x = touch->getLocation().x + 300 * ratio;
        if ((target_x + FRAME_SIZE / 2 * ratio) > _contentSize.width - getVisibleSpaceWidth())
            //右側にはみ出そうなとき止める
            target_x = (_contentSize.width - getVisibleSpaceWidth()) - FRAME_SIZE / 2 * ratio;
    }
    else
    {
        //画面より右側をタッチした場合
        target_x = touch->getLocation().x - 300 * ratio;
        if (target_x - FRAME_SIZE / 2 * ratio < 0 + getVisibleSpaceWidth())
            //左側にはみ出そうなときとめる
            target_x = FRAME_SIZE / 2 * ratio + getVisibleSpaceWidth();
    }

    return target_x;
}
