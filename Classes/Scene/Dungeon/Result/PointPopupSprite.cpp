#include "PointPopupSprite.h"
#include "ResultLayer.h"

PointPopupSprite::PointPopupSprite()
:_get_rank_point(0)
,_get_reward_point(0)
,_now_rank_point(0)
,_need_rank_point(0)
,_now_reward_point(0)
,_need_reward_point(0)
,_next_rank_name()
,_change_rank_count(0)
,_change_reward_count(0)
,_rank_effect_flg(false)
,_reward_effect_flg(false)
,_gauge_speed_up_flg(false)
{
}

PointPopupSprite* PointPopupSprite::create(PopupType popup_type, Json json)
{
    PointPopupSprite *pRet = new PointPopupSprite();
    
    if (pRet && pRet->init(popup_type, json)) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool PointPopupSprite::init(PopupType popup_type, Json json)
{
    //PopupTypeによって画像を切り替える
    string filename ;
    if(popup_type==PopupType::TYPE_ALL_POINT){
        filename = "init/Dungeon/result/popup_reward1.png" ;
    }else if(popup_type==PopupType::TYPE_NO_RANK_POINT){
        filename = "init/Dungeon/result/popup_reward2.png" ;
    }
    
    if(!Sprite::initWithFile(filename))
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //ポップアップのタイプを保持
    _popup_type = popup_type ;
    
    //初期データの読み込み
    _before_user_data_list  = json["before_user_data_list"].object_items();
    _after_user_data_list   = json["after_user_data_list"].object_items();
    _add_point_list         = json["add_point_list"].object_items();
    _rank_data_list         = json["rank_data_list"].object_items();
    _reward_data_list       = json["reward_data_list"].object_items();
    _max_reward_point_list_id = fromString<int>(json["max_reward_point_list_id"].string_value());
    _max_rank_point_list_id = fromString<int>(json["max_rank_point_list_id"].string_value());
    
    _rank_data_count      = countJsonData(_rank_data_list);
    _reward_data_count    = countJsonData(_reward_data_list);
    _before_rank_point_id = fromString<int>(_before_user_data_list["rank_point_list_id"].string_value());
    _after_rank_point_id  = fromString<int>(_after_user_data_list["rank_point_list_id"].string_value());
    _before_reward_point_id = fromString<int>(_before_user_data_list["reward_point_list_id"].string_value());
    _after_reward_point_id  = fromString<int>(_after_user_data_list["reward_point_list_id"].string_value());
    _before_level = fromString<int>(_before_user_data_list["level"].string_value());
    _after_level = fromString<int>(_after_user_data_list["level"].string_value());
    //    HMSLOG("_rank_data_count = %d, _reward_data_count = %d", _rank_data_count, _reward_data_count);

    
    return true;
}

void PointPopupSprite::onEnter()
{
    Sprite::onEnter();
    
    //初期データの設定
    setData();
    
    //獲得資金文字の作成
    createRewardMoneyLabel();
    
    //次のランクの文字作成
    createNextRankLabel();
    
    //次のレベルの文字作成
    createNextLevelLabel();
    
    //ゲージの背景画像の作成
    createGaugeBg();
    
    //現在のポイント表示
    showCurrentPointGauge();
    
    //獲得したポイントの表示
    showGetPoint();
    
    //レベルアップに必要なポイントの表示
    showNeedPoint();
}


void PointPopupSprite::setData()
{
    //初期表示に必要なデータの設定
    setRankPoint(_add_point_list["rank_point"].int_value());
    setRewardPoint(_add_point_list["reward_point"].int_value());
    setNowRankPoint(fromString<int>(_before_user_data_list["rank_point"].string_value()));
    setNowRewardPoint(fromString<int>(_before_user_data_list["reward_point"].string_value()));
    
    //ランクポイント
    if (_after_rank_point_id >= _before_rank_point_id)
    {
        if (_rank_data_count >= 2)
        {
            //ランクアップ、又は変動なし
            setNeedRankPoint(fromString<float>(_rank_data_list["1"]["need_rank_point"].string_value()) - fromString<float>(_rank_data_list["0"]["need_rank_point"].string_value()));
            setNextRankName(_rank_data_list["1"]["rank_name"].string_value());
        }
        else
        {
            //レベル最大時ランクデータを一つしか取れないので場合分け
            setNeedRankPoint(0);
            setNextRankName("最大");
        }
        
    }
    else
    {
        if (_rank_data_count >= 2 && _before_rank_point_id < _max_rank_point_list_id)
        {
            //ランクダウン
            setNeedRankPoint(fromString<float>(_rank_data_list[toString<int>(_rank_data_count - 1)]["need_rank_point"].string_value()) - fromString<float>(_rank_data_list[toString<int>(_rank_data_count - 2)]["need_rank_point"].string_value()));
            setNextRankName(_rank_data_list[toString<int>(_rank_data_count - 1)]["rank_name"].string_value());
        }
        else
        {
            //ランク最大から下がるとき次の必要ポイントが取れないので場合わけ
            setNeedRankPoint(0);
            setNextRankName("最大");
        }
    }
    
    //報酬ポイント
    if (_after_reward_point_id >= _before_reward_point_id)
    {
        if (_reward_data_count >= 2)
        {
            //ランクアップ、又は変動なし
            setNeedRewardPoint(fromString<float>(_reward_data_list["1"]["need_reward_point"].string_value()) - fromString<float>(_reward_data_list["0"]["need_reward_point"].string_value()));
            setNextLevel(_before_level+1) ;
        }
        else
        {
            //レベル最大時ランクデータを一つしか取れないので場合分け
            setNeedRewardPoint(0);
            setNextLevel(-1) ; //最大値の時-1
        }
        
    }
    else
    {
        if (_reward_data_count >= 2 && _before_reward_point_id < _max_rank_point_list_id)
        {
            //ランクダウン
            setNeedRewardPoint(fromString<float>(_reward_data_list[toString<int>(_reward_data_count - 1)]["need_reward_point"].string_value()) - fromString<float>(_reward_data_list[toString<int>(_reward_data_count - 2)]["need_reward_point"].string_value()));
            setNextLevel(_before_level) ;
        }
        else
        {
            //ランク最大から下がるとき次の必要ポイントが取れないので場合わけ
            setNeedRewardPoint(0);
            setNextLevel(-1) ; //最大値の時-1
        }
    }
    
    HMSLOG("get_rank_point = %d, get_reward_point = %d, now_rank_point = %d, need_rank_point = %d, now_reward_point = %d, need_reward_point = %d, rank_name = %s", _get_rank_point, _get_reward_point, _now_rank_point, _need_rank_point, _now_reward_point, _need_reward_point, _next_rank_name.c_str());
    
}

void PointPopupSprite::createRewardMoneyLabel()
{
    //獲得金
    int money = _add_point_list["gold"].int_value();
    auto money_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(money), FONT_NAME_SYSTEM, 33);
    money_label->setAnchorPoint(Point(0, 0));
    money_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        money_label->setPosition(Point(240, 241));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        money_label->setPosition(Point(240, 157));
    }
    this->addChild(money_label, 1);
}

void PointPopupSprite::createNextRankLabel()
{
    //次のランクの文字作成
    string rank_message = "次の称号:";
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(rank_message, FONT_NAME_SYSTEM, 18);
    label->setAnchorPoint(Point(0, 1));
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        label->setPosition(Point(320, 145));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        label->setVisible(false);
    }
    this->addChild(label, 1);
    
    
    string rank_name = _next_rank_name;
    _rank_name_label = HMSFontUtil::createWithTTFBySystemFontArgument(rank_name, FONT_NAME_SYSTEM, 20);
    _rank_name_label->setAnchorPoint(Point(0, 1));
    _rank_name_label->setColor(Color3B(255, 204, 0));
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _rank_name_label->setPosition(Point(400, 145));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _rank_name_label->setVisible(false);
    }
    this->addChild(_rank_name_label, 1);
    
}

void PointPopupSprite::createNextLevelLabel()
{
    //次のランクの文字作成
    string level_message  = "次のレベル:";
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(level_message , FONT_NAME_SYSTEM, 18);
    label->setAnchorPoint(Point(0, 1));
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        label->setPosition(Point(320, 58));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        label->setPosition(Point(320, 63));
    }
    this->addChild(label, 1);
    
    
    //次のレベルの表示を更新
    string level_string ;
    if(_next_level==-1){
        level_string = "最大";
    }else{
        level_string = toString<int>(_next_level);
    }
    _level_label = HMSFontUtil::createWithTTFBySystemFontArgument(level_string, FONT_NAME_SYSTEM, 20);
    _level_label->setAnchorPoint(Point(0, 1));
    _level_label->setColor(Color3B(255, 204, 0));
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _level_label->setPosition(Point(420, 58));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _level_label->setPosition(Point(420, 63));
    }
    this->addChild(_level_label, 1);
}


void PointPopupSprite::createGaugeBg()
{
    //ゲージの背景画像の作成
    auto gauge_bg = Sprite::create("init/Dungeon/result/popup_pt_g.png");
    
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        gauge_bg->setPosition(Point(this->getContentSize().width/2, 160));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        gauge_bg->setPosition(Point(this->getContentSize().width/2, 165));
    }
    this->addChild(gauge_bg, -2);
}

void PointPopupSprite::showCurrentPointGauge()
{
    //ランクポイントゲージ画像
    float rank_point_percent = 1.0f;
    if (_need_rank_point != 0)
    {
        rank_point_percent = (float)_now_rank_point / _need_rank_point;
    }
    _rank_point_bar = Sprite::create("init/Dungeon/result/gauge_orange2.png");
    _rank_point_bar->setScaleX(rank_point_percent);
    _rank_point_bar->setAnchorPoint(Point(0, 0.5));
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _rank_point_bar->setPosition(Point(70, 159));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _rank_point_bar->setVisible(false) ;
    }
    this->addChild(_rank_point_bar, -1);
    
    
    //報酬ポイントゲージ画像
    float reward_point_percent = 1.0f;
    if (_need_reward_point != 0)
    {
        reward_point_percent = (float)_now_reward_point / _need_reward_point;
    }
    _reward_point_bar = Sprite::create("init/Dungeon/result/gauge_green2.png");
    _reward_point_bar->setScaleX(reward_point_percent);
    _reward_point_bar->setAnchorPoint(Point(0, 0.5));
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _reward_point_bar->setPosition(Point(70, 71));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _reward_point_bar->setPosition(Point(70, 76));
    }
    this->addChild(_reward_point_bar, -1);
    
    //    HMSLOG("rank_point_percent = %f, reward_point_percent = %f", rank_point_percent, reward_point_percent);/
    
}

void PointPopupSprite::showGetPoint()
{
    //称号獲得ポイントの表示
    _get_rank_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(_get_rank_point), FONT_NAME_SYSTEM, 34);
    _get_rank_point_label->setAnchorPoint(Point(0, 0));
    _get_rank_point_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _get_rank_point_label->setPosition(Point(240, 169));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _get_rank_point_label->setVisible(false);
    }
    this->addChild(_get_rank_point_label, 1);
    
    //報酬獲得ポイントの表示
    _get_reward_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(_get_reward_point), FONT_NAME_SYSTEM, 34);
    _get_reward_point_label->setAnchorPoint(Point(0, 0));
    _get_reward_point_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _get_reward_point_label->setPosition(Point(240, 81));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _get_reward_point_label->setPosition(Point(240, 86));
    }
    this->addChild(_get_reward_point_label, 1);
}

void PointPopupSprite::showNeedPoint()
{
    //ランクレベルアップ必要ポイントの表示
    int need_rank_point = _need_rank_point - _now_rank_point;
    string need_rank_text = "あと"+toString<int>(need_rank_point);
    if (_need_rank_point == 0) {
        need_rank_text = "最大";
    }
    _need_rank_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(need_rank_text, FONT_NAME_SYSTEM, 24);
    _need_rank_point_label->setAnchorPoint(Point(1.0, 0));
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _need_rank_point_label->setPosition(Point(546, 173));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _need_rank_point_label->setVisible(false) ;
    }
    this->addChild(_need_rank_point_label, 1);
    
    //報酬レベルアップ必要ポイントの表示
    int need_reward_point = _need_reward_point - _now_reward_point;
    string need_reward_text = "あと"+toString<int>(need_reward_point);
    if (_need_reward_point == 0) {
        need_reward_text = "最大";
    }
    _need_reward_point_label = HMSFontUtil::createWithTTFBySystemFontArgument(need_reward_text, FONT_NAME_SYSTEM, 24);
    _need_reward_point_label->setAnchorPoint(Point(1.0, 0));
    //enableOutline(Color4B(0, 0, 0, 255), 1);
    if(_popup_type==PopupType::TYPE_ALL_POINT){
        _need_reward_point_label->setPosition(Point(546, 83));
    }else if(_popup_type==PopupType::TYPE_NO_RANK_POINT){
        _need_reward_point_label->setPosition(Point(546, 88));
    }
    this->addChild(_need_reward_point_label, 1);
}

void PointPopupSprite::gaugeAction(float dt)
{
    //ランクポイント増減アクション
    if (_get_rank_point > 0) {
        //ランクポイント増加アクション
        schedule(schedule_selector(PointPopupSprite::rankPointUpAction), 0.01f);
    }
    else if (_get_rank_point < 0) {
        //ランクポイント減少アクション
        schedule(schedule_selector(PointPopupSprite::rankPointDownAction), 0.01f);
    }
    else {
        //報酬ポイントアクション
        scheduleOnce(schedule_selector(PointPopupSprite::rewardPointAction), 0.01f);
    }
}

void PointPopupSprite::rankPointUpAction(float dt)
{
    if (_get_rank_point <= 0)
    {
        if (_now_rank_point == _need_rank_point && _need_rank_point != 0)
        {
            //現在のポイントが0で必要も0になったとき一度だけランクアップアクションをさせる
            changeRankData();
        }
        
        //ポイント振り分け終了処理
        unschedule(schedule_selector(PointPopupSprite::rankPointUpAction));
        
        //報酬ポイントアクション
        scheduleOnce(schedule_selector(PointPopupSprite::rewardPointAction), 0.5f);
    }
    else
    {
        if (_now_rank_point >= _need_rank_point)
        {
            if (_need_rank_point > 0)
            {
                //ランクアップ、ランクデータの更新
                changeRankData();
            }
            else
            {
                //レベル最大時の処理
                
                //現在の残り_get_rank_pointをspeedが上回ってしまっている場合、_get_rank_pointをspeedにする
                int speed = gaugeSpeedByNeedPoint(_need_rank_point);
                if(_get_rank_point <= speed){
                    speed = _get_rank_point ;
                }
                
                //ランクポイント増加アクション
                _get_rank_point -= speed;
                _get_rank_point_label->setString(toString<int>(_get_rank_point));
            }
        }
        else
        {
            int speed = gaugeSpeedByNeedPoint(_need_rank_point);
            
            //現在の残り_get_rank_pointをspeedが上回ってしまっている場合、_get_rank_pointをspeedにする
            if(_get_rank_point <= speed){
                speed = _get_rank_point ;
            }
            
            //_need_rank_point-_now_rank_point(即ち称号が上昇するまでに必要な値)がspeedを上回っている場合スピードを_need_rank_point-_now_rank_pointにする
            if((_need_rank_point - _now_rank_point <= speed)){
                speed = _need_rank_point - _now_rank_point ;
            }
            
            //ランクポイント増加アクション
            _get_rank_point -= speed;
            _get_rank_point_label->setString(toString<int>(_get_rank_point));
            
            _now_rank_point += speed;
            int need_rank_point = _need_rank_point - _now_rank_point;
            _need_rank_point_label->setString("あと"+toString<int>(need_rank_point));
            
            float rank_point_percent = (float)_now_rank_point / _need_rank_point;
            _rank_point_bar->setScaleX(rank_point_percent);
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
            
        }
    }
}

void PointPopupSprite::rankPointDownAction(float dt)
{
    if (_get_rank_point >= 0)
    {
        //ポイント振り分け終了処理
        unschedule(schedule_selector(PointPopupSprite::rankPointDownAction));
        
        //報酬ポイントアクション
        scheduleOnce(schedule_selector(PointPopupSprite::rewardPointAction), 0.5f);
    }
    else
    {
        if (_now_rank_point <= 0)
        {
//            if (fromString<int>(_after_user_data_list["total_rank_point"].string_value()) <= 0)
            if (_before_rank_point_id - _after_rank_point_id <= _change_rank_count)
            {
                //これ以上下がらせない処理
                int speed = gaugeSpeedByNeedPoint(_need_rank_point);
                
                //現在の残り-_get_rank_pointをspeedが上回ってしまっている場合、_get_rank_pointをspeedにする
                if(-_get_rank_point <= speed){
                    speed = -_get_rank_point ;
                }
                
                _get_rank_point += speed;
                _get_rank_point_label->setString(toString<int>(_get_rank_point));
            }
            else
            {
                //ランクダウン、ランクデータの更新
                changeRankData();
            }
        }
        else
        {
            int speed = gaugeSpeedByNeedPoint(_need_rank_point);
            
            // 現在の残り_get_rank_pointをspeedが上回ってしまっている場合、_get_rank_pointをspeedにする
            
            if(-_get_rank_point <= speed){
                speed = -_get_rank_point ;
            }
            
            //_need_rank_point-_now_rank_point(即ち称号が上昇するまでに必要な値)がspeedを上回っている場合スピードを_need_rank_point-_now_rank_pointにする
            if((_now_rank_point <= speed)){
                speed = _now_rank_point ;
            }
            
            //ランクポイント減少アクション
            _get_rank_point += speed;
            _get_rank_point_label->setString(toString<int>(_get_rank_point));
            
            _now_rank_point -= speed;
            int need_rank_point = _need_rank_point - _now_rank_point;
            string need_rank_text = "あと"+toString<int>(need_rank_point);
            if (_need_rank_point == 0) {
                need_rank_text = "最大";
            }
            _need_rank_point_label->setString(need_rank_text);
            
            float rank_point_percent = 1.0f;
            if (_need_rank_point != 0)
            {
                rank_point_percent = (float)_now_rank_point / _need_rank_point;
            }
            _rank_point_bar->setScaleX(rank_point_percent);
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
            
        }
    }
}

void PointPopupSprite::changeRankData()
{
    //ランクデータ更新処理
    _change_rank_count++;
    
    if (_after_rank_point_id >= _before_rank_point_id)
    {
        //ランクアップ時
        if (_rank_data_list[toString<int>(_change_rank_count + 1)].is_object())
        {
            setNeedRankPoint(fromString<float>(_rank_data_list[toString<int>(_change_rank_count + 1)]["need_rank_point"].string_value()) - fromString<float>(_rank_data_list[toString<int>(_change_rank_count)]["need_rank_point"].string_value()));
            setNextRankName(_rank_data_list[toString<int>(_change_rank_count + 1)]["rank_name"].string_value());
        }
        else
        {
            //ランクが最大になったとき
            setNeedRankPoint(0);
            setNextRankName("最大");
        }
        setNowRankPoint(0);
        
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/lvup.mp3");
        
        showEffect("init/Dungeon/result/plist/up.plist", 1.0, Point(this->getContentSize().width / 2 - 15, 270), removeEffectFunc());
        _rank_effect_flg = true;
    }
    else
    {
        if (_before_rank_point_id >= _max_rank_point_list_id)
        {
            //ランク最大からランクダウンするとき
            setNeedRankPoint(fromString<float>(_rank_data_list[toString<int>(_rank_data_count - _change_rank_count)]["need_rank_point"].string_value()) - fromString<float>(_rank_data_list[toString<int>(_rank_data_count - (_change_rank_count + 1))]["need_rank_point"].string_value()));
            setNextRankName(_rank_data_list[toString<int>(_rank_data_count - _change_rank_count)]["rank_name"].string_value());
            setNowRankPoint(fromString<float>(_rank_data_list[toString<int>(_rank_data_count - _change_rank_count)]["need_rank_point"].string_value()) - fromString<float>(_rank_data_list[toString<int>(_rank_data_count - (_change_rank_count + 1))]["need_rank_point"].string_value()));
        }
        else
        {
            //ランクダウン時
            setNeedRankPoint(fromString<float>(_rank_data_list[toString<int>(_rank_data_count - (_change_rank_count + 1))]["need_rank_point"].string_value()) - fromString<float>(_rank_data_list[toString<int>(_rank_data_count - (_change_rank_count + 2))]["need_rank_point"].string_value()));
            setNextRankName(_rank_data_list[toString<int>(_rank_data_count - (_change_rank_count + 1))]["rank_name"].string_value());
            setNowRankPoint(fromString<float>(_rank_data_list[toString<int>(_rank_data_count - (_change_rank_count + 1))]["need_rank_point"].string_value()) - fromString<float>(_rank_data_list[toString<int>(_rank_data_count - (_change_rank_count + 2))]["need_rank_point"].string_value()));
        }
        
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/rank_down.mp3");
        
        showEffect("init/Dungeon/result/plist/down.plist", 1.0, Point(this->getContentSize().width / 2 - 15, 270), removeEffectFunc());
        _rank_effect_flg = true;
    }
    
    //必要ポイントとゲージの更新
    
    int need_rank_point = _need_rank_point - _now_rank_point;
    _need_rank_point_label->setString("あと"+toString<int>(need_rank_point));
    if (_need_rank_point == 0) {
        _need_rank_point_label->setString("最大");
    }
    
    float rank_point_percent = 1.0f;
    if (_need_rank_point != 0)
    {
        rank_point_percent = (float)_now_rank_point / _need_rank_point;
    }
    _rank_point_bar->setScaleX(rank_point_percent);
    
    //次のランク名の表示を更新
    _rank_name_label->setString(_next_rank_name);
    
}

void PointPopupSprite::rewardPointAction(float dt)
{
    //報酬ポイント増減アクション
    if (_get_reward_point > 0)
    {
        //報酬ポイント増加アクション
        schedule(schedule_selector(PointPopupSprite::rewardPointUpAction), 0.01f);
    }
    else if (_get_reward_point < 0)
    {
        //報酬ポイント減少アクション
        schedule(schedule_selector(PointPopupSprite::rewardPointDownAction), 0.01f);
    }
    else
    {
        //報酬ポイントが無い場合タッチアクション
        auto result_layer = (ResultLayer *)this->getParent();
        
        //チェックカウントの変更
        result_layer->_now_popup_queue_index++;
        
        result_layer->touchStartAction();
    }
}

void PointPopupSprite::rewardPointUpAction(float dt)
{
    if (_get_reward_point <= 0)
    {
        if (_now_reward_point == _need_reward_point && _need_reward_point != 0)
        {
            //現在のポイントが0で必要も0になったとき一度だけ報酬ポイントアップアクションをさせる
            changeRewardData();
        }
        
        //ポイント振り分け終了処理
        unschedule(schedule_selector(PointPopupSprite::rewardPointUpAction));
        
        //タッチアクション開始
        auto result_layer = (ResultLayer *)this->getParent();
        
        //チェックカウントの変更
        result_layer->_now_popup_queue_index++;
        
        result_layer->touchStartAction();
    }
    else
    {
        if (_now_reward_point >= _need_reward_point)
        {
            if (_need_reward_point > 0)
            {
                //報酬ポイントアップ、報酬ポイントデータの更新
                changeRewardData();
            }
            else
            {
                int speed = gaugeSpeedByNeedPoint(_need_reward_point);
                if(_get_reward_point <= speed ){
                    speed = _get_reward_point ;
                }
                
                //レベル最大時の処理
                _get_reward_point -= speed;
                _get_reward_point_label->setString(toString<int>(_get_reward_point));
            }
        }
        else
        {
            //現在の残り_get_reward_pointをspeedが上回ってしまっている場合、_get_reward_pointをspeedにする
            int speed = gaugeSpeedByNeedPoint(_need_reward_point);
            if(_get_reward_point <= speed){
                speed = _get_reward_point ;
            }
            
            //_need_reward_point-_now_reward_point(即ちレベルが上昇するまでに必要な値)がspeedを上回っている場合スピードを_need_reward_point-_now_reward_pointにする
            if((_need_reward_point - _now_reward_point <= speed)){
                speed = _need_reward_point - _now_reward_point ;
            }
            
            //ランクポイント増加アクション
            _get_reward_point -= speed;
            _get_reward_point_label->setString(toString<int>(_get_reward_point));
            
            _now_reward_point += speed;
            int need_reward_point = _need_reward_point - _now_reward_point;
            _need_reward_point_label->setString("あと"+toString<int>(need_reward_point));
            
            float reward_point_percent = (float)_now_reward_point / _need_reward_point;
            _reward_point_bar->setScaleX(reward_point_percent);
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
            
        }
    }
}

void PointPopupSprite::rewardPointDownAction(float dt)
{
    if (_get_reward_point >= 0)
    {
        //ポイント振り分け終了処理
        unschedule(schedule_selector(PointPopupSprite::rewardPointDownAction));
        
        //タッチアクション開始
        auto result_layer = (ResultLayer *)this->getParent();
        
        //チェックカウントの変更
        result_layer->_now_popup_queue_index++;
        
        result_layer->touchStartAction();
    }
    else
    {
        if (_now_reward_point <= 0)
        {
            if (fromString<int>(_after_user_data_list["total_reward_point"].string_value()) <= 0)
            {
                int speed = gaugeSpeedByNeedPoint(_need_reward_point);
                
                //現在の残り-_get_reward_pointをspeedが上回ってしまっている場合、_get_reward_pointをspeedにする
                if(-_get_reward_point <= speed){
                    speed = -_get_reward_point ;
                }
                
                //これ以上下がらせない処理
                _get_reward_point += speed;
                _get_reward_point_label->setString(toString<int>(_get_reward_point));
            }
            else
            {
                //報酬ポイントダウン、報酬ポイントデータの更新
                changeRewardData();
            }
        }
        else
        {
            int speed = gaugeSpeedByNeedPoint(_need_reward_point);
            
            //            現在の残り-_get_reward_pointをspeedが上回ってしまっている場合、_get_reward_pointをspeedにする
            if(-_get_reward_point <= speed){
                speed = -_get_reward_point ;
            }
            
            //_need_reward_point-_now_reward_point(即ち称号が上昇するまでに必要な値)がspeedを上回っている場合スピードを_need_reward_point-_now_reward_pointにする
            if((_now_reward_point <= speed)){
                speed = _now_reward_point ;
            }
            
            //報酬ポイント減少アクション
            _get_reward_point += speed;
            _get_reward_point_label->setString(toString<int>(_get_reward_point));
            
            _now_reward_point -= speed;
            int need_reward_point = _need_reward_point - _now_reward_point;
            string need_reward_text = "あと"+toString<int>(need_reward_point);
            if (_need_reward_point == 0) {
                need_reward_text = "最大";
            }
            _need_reward_point_label->setString(need_reward_text);
            
            float reward_point_percent = 1.0f;
            if (_need_reward_point != 0)
            {
                reward_point_percent = (float)_now_reward_point / _need_reward_point;
            }
            _reward_point_bar->setScaleX(reward_point_percent);
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
            
        }
    }
}

void PointPopupSprite::changeRewardData()
{
    //報酬ポイントデータ更新処理
    _change_reward_count++;
    
    if (_after_reward_point_id >= _before_reward_point_id)
    {
        //報酬ポイントランクアップ時
        if (_reward_data_list[toString<int>(_change_reward_count + 1)].is_object())
        {
            setNeedRewardPoint(fromString<float>(_reward_data_list[toString<int>(_change_reward_count + 1)]["need_reward_point"].string_value()) - fromString<float>(_reward_data_list[toString<int>(_change_reward_count)]["need_reward_point"].string_value()));
            setNextLevel(getNextLevel()+1) ; //最大値の時-1
        }
        else
        {
            //ランクが最大になったとき
            setNeedRewardPoint(0);
            setNextLevel(-1) ; //最大値の時-1
        }
        setNowRewardPoint(0);
        
        //SE
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/bonus_get.mp3");
        
        showEffect("init/Dungeon/result/plist/heal2.plist", 1.0, Point(this->getContentSize().width / 2, 75), removeEffectFunc());
        showEffect("init/Dungeon/result/plist/bonus.plist", 1.0, Point(this->getContentSize().width / 2, 125), removeEffectFunc());
        
        _reward_effect_flg = true;
    }
    else
    {
        if (_before_reward_point_id >= _max_reward_point_list_id)
        {
            //ランク最大からランクダウンするとき
            setNeedRewardPoint(fromString<float>(_reward_data_list[toString<int>(_reward_data_count - _change_reward_count)]["need_reward_point"].string_value()) - fromString<float>(_reward_data_list[toString<int>(_reward_data_count - (_change_reward_count + 1))]["need_reward_point"].string_value()));
            setNowRewardPoint(fromString<float>(_reward_data_list[toString<int>(_reward_data_count - _change_reward_count)]["need_reward_point"].string_value()) - fromString<float>(_reward_data_list[toString<int>(_reward_data_count - (_change_reward_count + 1))]["need_reward_point"].string_value()));
        }
        else
        {
            //ランクダウン時
            setNeedRewardPoint(fromString<float>(_reward_data_list[toString<int>(_reward_data_count - (_change_reward_count + 1))]["need_reward_point"].string_value()) - fromString<float>(_reward_data_list[toString<int>(_reward_data_count - (_change_reward_count + 2))]["need_reward_point"].string_value()));
            setNowRewardPoint(fromString<float>(_reward_data_list[toString<int>(_reward_data_count - (_change_reward_count + 1))]["need_reward_point"].string_value()) - fromString<float>(_reward_data_list[toString<int>(_reward_data_count - (_change_reward_count + 2))]["need_reward_point"].string_value()));
        }
    }
    
    //必要ポイントとゲージの更新
    int need_reward_point = _need_reward_point - _now_reward_point;
    _need_reward_point_label->setString("あと"+toString<int>(need_reward_point));
    if (_need_reward_point == 0) {
        _need_reward_point_label->setString("最大");
    }
    
    float reward_point_percent = 1.0f;
    if (_need_reward_point != 0)
    {
        reward_point_percent = (float)_now_reward_point / _need_reward_point;
    }
    _reward_point_bar->setScaleX(reward_point_percent);
    
    //次のレベルの表示を更新
    if(_next_level==-1){
        _level_label->setString("最大");
    }else{
        _level_label->setString(toString<int>(_next_level));
    }
}

void PointPopupSprite::gaugeSpeedUp(){
    _gauge_speed_up_flg = true;
    
}

void PointPopupSprite::showfinishData()
{
    //アクション終了後の結果表示
    _get_rank_point   = 0;
    _get_reward_point = 0;
    _get_rank_point_label->setString(toString<int>(_get_rank_point));
    _get_reward_point_label->setString(toString<int>(_get_reward_point));
    
    //振り分け後のランクポイント表示
    showfinishRankData();
    
    //振り分け後の報酬ポイント表示
    showfinishRewardData();
}

void PointPopupSprite::showfinishRankData()
{
    //振り分け後のランクデータ
    if (_after_rank_point_id >= _before_rank_point_id)
    {
        //勝利後の結果表示
        if (_rank_data_count >= 2 && _after_rank_point_id < _max_rank_point_list_id)
        {
            _now_rank_point = fromString<int>(_after_user_data_list["rank_point"].string_value());
            _need_rank_point = fromString<int>(_rank_data_list[toString<int>(_rank_data_count - 1)]["need_rank_point"].string_value()) - fromString<int>(_rank_data_list[toString<int>(_rank_data_count - 2)]["need_rank_point"].string_value());
            int need_rank_point = _need_rank_point - _now_rank_point;
            _need_rank_point_label->setString("あと"+toString<int>(need_rank_point));
            
            float rank_point_percent = (float)_now_rank_point / _need_rank_point;
            _rank_point_bar->setScaleX(rank_point_percent);
            
            string rank_name = _rank_data_list[toString<int>(_rank_data_count - 1)]["rank_name"].string_value();
            _rank_name_label->setString(rank_name);
        }
        else
        {
            //ランク最大時
            _need_rank_point_label->setString("最大");
            _rank_point_bar->setScaleX(1.0f);
            _rank_name_label->setString("最大");
        }
        
        if (_after_rank_point_id > _before_rank_point_id && !_rank_effect_flg)
        {
            showEffect("init/Dungeon/result/plist/up.plist", 1.0, Point(this->getContentSize().width / 2 - 15, 270), removeEffectFunc());
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/lvup.mp3");
        }
    }
    else
    {
        //敗北後の結果表示
        _now_rank_point = fromString<int>(_after_user_data_list["rank_point"].string_value());
        _need_rank_point = fromString<int>(_rank_data_list["1"]["need_rank_point"].string_value()) - fromString<int>(_rank_data_list["0"]["need_rank_point"].string_value());
        int need_rank_point = _need_rank_point - _now_rank_point;
        _need_rank_point_label->setString("あと"+toString<int>(need_rank_point));
        
        float rank_point_percent = (float)_now_rank_point / _need_rank_point;
        _rank_point_bar->setScaleX(rank_point_percent);
        
        string rank_name = _rank_data_list["1"]["rank_name"].string_value();
        _rank_name_label->setString(rank_name);
        
        if (!_rank_effect_flg)
        {
            showEffect("init/Dungeon/result/plist/down.plist", 1.0, Point(this->getContentSize().width / 2 - 15, 270), removeEffectFunc());
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/rank_down.mp3");
        }
    }
}

void PointPopupSprite::showfinishRewardData()
{
    //振り分け後の報酬ポイントデータ
    if (_after_reward_point_id >= _before_reward_point_id)
    {
        //勝利後の結果表示
        if (_reward_data_count >= 2 && _after_reward_point_id < _max_reward_point_list_id)
        {
            _now_reward_point = fromString<int>(_after_user_data_list["reward_point"].string_value());
            _need_reward_point = fromString<int>(_reward_data_list[toString<int>(_reward_data_count - 1)]["need_reward_point"].string_value()) - fromString<int>(_reward_data_list[toString<int>(_reward_data_count - 2)]["need_reward_point"].string_value());
            int need_reward_point = _need_reward_point - _now_reward_point;
            _need_reward_point_label->setString("あと"+toString<int>(need_reward_point));
            
            float reward_point_percent = (float)_now_reward_point / _need_reward_point;
            _reward_point_bar->setScaleX(reward_point_percent);
        }
        else
        {
            //報酬ポイントランク最大時
            _need_reward_point_label->setString("最大");
            _reward_point_bar->setScaleX(1.0f);
        }
        
        if (_after_reward_point_id > _before_reward_point_id && !_reward_effect_flg)
        {
            
            //SE
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/bonus_get.mp3");
            
            //次のエフェクトを表示
            showEffect("init/Dungeon/result/plist/heal2.plist", 1.0, Point(this->getContentSize().width / 2, 70), removeEffectFunc());
            showEffect("init/Dungeon/result/plist/bonus.plist", 1.0, Point(this->getContentSize().width / 2, 125), removeEffectFunc());
        }
    }
    else
    {
        //敗北後の結果表示
        _now_reward_point = fromString<int>(_after_user_data_list["reward_point"].string_value());
        _need_reward_point = fromString<int>(_reward_data_list["1"]["need_reward_point"].string_value()) - fromString<int>(_reward_data_list["0"]["need_reward_point"].string_value());
        int need_reward_point = _need_reward_point - _now_reward_point;
        _need_reward_point_label->setString("あと"+toString<int>(need_reward_point));
        
        float reward_point_percent = (float)_now_reward_point / _need_reward_point;
        _reward_point_bar->setScaleX(reward_point_percent);
    }
    
    //次のレベルの表示を更新
    if(_next_level==-1){
        _level_label->setString("最大");
    }else{
        _level_label->setString(toString<int>(_next_level));
    }
}

void PointPopupSprite::checkRankPointMax()
{
    //ランクポイントレベルが最大かチェック
    if (_before_rank_point_id >= _max_rank_point_list_id)
    {
        _rank_point_bar->setScaleX(1.0);
        _need_rank_point_label->setString("最大");
    }
}

void PointPopupSprite::checkRewardPointMax()
{
    //報酬ポイントレベルが最大かチェック
    if (fromString<int>(_before_user_data_list["reward_point_list_id"].string_value()) >= 51)
    {
        _reward_point_bar->setScaleX(1.0);
        _need_reward_point_label->setString("最大");
    }
}

void PointPopupSprite::showEffect(string filename, float scale, Point pos, function<void()> _func)
{
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename);
    
    // 配列に各フレームを格納
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        animFrames.pushBack(frame);
    }
    
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.05f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    auto sprite = Sprite::createWithSpriteFrame(animFrames.front());
    sprite->setScale(scale);
    sprite->setPosition(pos);
    this->addChild(sprite, 1000, EFFECT_TAG);
    
    // 無限ループのアクションを生成
    sprite->runAction(Sequence::create(animate, CallFunc::create([_func](){
        _func();
    }), nullptr));
    
}

function<void()> PointPopupSprite::removeEffectFunc()
{
    auto func = [this](){
        auto effect_sp = (Sprite *)this->getChildByTag(EFFECT_TAG);
        if (effect_sp) {
            effect_sp->removeFromParentAndCleanup(true);
        }
    };
    
    return func;
}

int PointPopupSprite::gaugeSpeedByNeedPoint(int need_point)
{
    //ゲージの増減スピードを必要ポイントによって求める
    if (need_point == 0) {
        need_point = 10000; //必要ポイントが0であった場合デフォルト値として10000設定する
    }
    
    float speed = (float)need_point / 1000 ;

    if(_gauge_speed_up_flg){
        speed *= 20 ;
    }
    
    if(speed < 1){
        speed = 1 ;
    }
    
    return (int)speed;
}

