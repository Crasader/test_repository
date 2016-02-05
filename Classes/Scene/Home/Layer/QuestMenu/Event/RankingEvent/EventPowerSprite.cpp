#include "EventPowerSprite.h"

EventPowerSprite* EventPowerSprite::create()
{
    EventPowerSprite *sprite = new EventPowerSprite();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool EventPowerSprite::init()
{
    if ( !Sprite::initWithFile("init/Home/Event/RankingEvent/action_force.png"))
    {
        return false;
    }
    HMSLOG("EventPowerSprite::init()");
    
    //サーバーとの時間差を取得
    _server_diff_time = SaveData::loadInt("server_diff_time");
    
    return true;
}


void EventPowerSprite::onEnter()
{
    Sprite::onEnter();
    
    loadEventPower() ;
    
    //「パワーの最終更新時間」と「最終更新時間時点におけるパワー」に基づき、現在のイベント専用パワーの現在の回復量を求める
    adjustTime() ;
    
    createBattlePointRecoveryTime();
    
    createEventPowerMark() ;
    updateEventPowerMark(false) ;
    
    //バトルポイント更新処理
    schedule(schedule_selector(EventPowerSprite::updateTime), 0.1f);
}

void EventPowerSprite::loadEventPower()
{
    
    _now_event_power = fromString<int>(SaveData::loadString("event_power")); //最終使用時間におけるパワー
    _max_event_power = fromString<int>(SaveData::loadString("max_event_power"));//パワー最大値
    
    //現在時刻の作成
    time_t now_time;
    time(&now_time);
    
    _check_time = now_time - _server_diff_time;
    
//    //最後にバトルポイントを使った時間のタイムスタンプを取得
//    struct tm tm;
//    strptime(SaveData::loadString("event_power_use_time").c_str(), "%Y-%m-%d %H:%M:%S", &tm);
//    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
//    _event_power_use_time = mktime(&tm) ;
    
    _event_power_use_time = fromString<time_t>(SaveData::loadString("timestamp_event_power_use_time"));
}

//unixタイムスタンプを日時と時間の形に整形して保存
void EventPowerSprite::savePowerUseTime(time_t save_time){
    struct tm* target_time_tm;
    char* time_string;
    int string_length=20;
    
    time_string=(char *)malloc(string_length*sizeof(char));
    
    // time_tをとってそれからローカルなtm構造体を取得
    target_time_tm=localtime(&save_time);
    
    // tm構造体からフォーマットで指定した文字列を生成
    (void)strftime(time_string,string_length,"%Y-%m-%d %H:%M:%S",target_time_tm);
    
    string time_string_2 = time_string ;
    
    HMSLOG("save before event_power_use_time:%s, timestamp_event_power_use_time:%s", SaveData::loadString("event_power_use_time").c_str(), SaveData::loadString("timestamp_event_power_use_time").c_str()) ;
    
    SaveData::saveString("event_power_use_time", time_string_2) ;
    SaveData::saveString("timestamp_event_power_use_time", toString<time_t>(save_time)) ;
    
    HMSLOG("save after event_power_use_time:%s, timestamp_event_power_use_time:%s", SaveData::loadString("event_power_use_time").c_str(), SaveData::loadString("timestamp_event_power_use_time").c_str()) ;
}


void EventPowerSprite::createBattlePointRecoveryTime()
{
    //バトルポイント回復時間背景の作成
    Sprite *time_back = Sprite::create("init/Home/Event/RankingEvent/bk_time.png") ;
    time_back->setAnchorPoint(Point(1, 1));
    time_back->setPosition(Point(this->getContentSize().width, 0));
    time_back->setVisible(false);
    time_back->setCascadeOpacityEnabled(true);
    this->addChild(time_back,1,T_RECOVERY_BACK) ;
    
    //時間の表示
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 30);
    label->setHorizontalAlignment(TextHAlignment::CENTER) ;
    label->setPosition(Point(125,42)) ;
    label->setTextColor(Color4B::RED);
    label->enableOutline(Color4B::BLACK, 1) ;
    time_back->addChild(label, 2, T_RECOVERY_LABEL);
}

void EventPowerSprite::adjustTime()
{
    //バトルポイントの更新処理
    if ( _now_event_power < _max_event_power)
    {
        //現在時刻と最終使用時刻の差異から回復量を求める
        int add_point = difftime(_check_time, _event_power_use_time) / EVENT_POWER_HEAL_TIME;
        HMSLOG("add_point = %d", add_point);
        
        //回復量を最終使用時刻に加算してバトルポイントの更新
        _now_event_power = _now_event_power + add_point;
        
        //
        _event_power_use_time = _event_power_use_time + (add_point * EVENT_POWER_HEAL_TIME);
        
        if (_now_event_power > _max_event_power)
        {
            _now_event_power = _max_event_power;
        }
    }
    else
    {
        _now_event_power = _max_event_power;
    }
    
    savePowerUseTime(_event_power_use_time) ;
    
    SaveData::saveString("event_power", toString<int>(_now_event_power));
    HMSLOG("event_power:%s", SaveData::loadString("event_power").c_str()) ;
}

void EventPowerSprite::createEventPowerMark() {
    float x_diff = 65 ; //体力の玉(?)の中心間の距離
    
    for(int i=0 ; i < _max_event_power ; i++){
        /*
        Sprite *empty_power_mark = Sprite::create("init/Home/Quest/star_l_gray.png");
        empty_power_mark->setPosition(Point(this->getContentSize().width/2 + x_diff * (i - _max_event_power/2), this->getContentSize().height/2));
        empty_power_marks.push_back(empty_power_mark);
        this->addChild(empty_power_mark, 1) ;
         */
        
        Sprite *power_mark = Sprite::create("init/Home/Event/RankingEvent/action_force_ball.png");
        power_mark->setPosition(Point(this->getContentSize().width/2 - 1 + x_diff * (i - _max_event_power/2), 33)) ;
        power_mark->setVisible(false) ;
        power_marks.push_back(power_mark);
        this->addChild(power_mark, 2) ;
    }
    
}


/*
 @param recovery_action
false:単に現在の体力を表示するだけ
true:体力が元々なかった場合、アクションを入れながら体力を表示する
*/
void EventPowerSprite::updateEventPowerMark(bool recovery_action)
{
    for(int i=0 ; i < _max_event_power ; i++){
        if(_now_event_power > i) {
            if( !power_marks[i]->isVisible()){
                power_marks[i]->setVisible(true) ;
                if(recovery_action){
                    power_marks[i]->setScale(0) ;
                    power_marks[i]->runAction((EaseBackOut::create(ScaleTo::create(1.1, 1.0)))) ;
                }
            }
        }
    }
}

void EventPowerSprite::updateTime(float dt)
{
    //現在時刻の取得
    time_t now_time;
    time(&now_time);
//    HMSLOG("now_time = %ld", now_time);
    
    now_time = now_time - _server_diff_time;
    
    //１秒計測の処理
    if (0 < difftime(now_time, _check_time))
    {
        //体力が満タンになったら止める
        if (_now_event_power == _max_event_power)
        {
            this->unscheduleUpdate();
            
            //パワーの更新
            SaveData::saveString("event_power", toString<int>(_max_event_power));
            
            //回復時間の削除
            auto recovery_back = (Label *)this->getChildByTag(T_RECOVERY_BACK) ;
            if (recovery_back)
            {
                recovery_back->setVisible(false) ;
            }
            
        }
        else
        {
            _check_time = now_time;
            this->scheduleOnce(schedule_selector(EventPowerSprite::setRecoveryTime), 0);
        }
    }
}

void EventPowerSprite::setRecoveryTime(float dt)
{
    auto label = (Label *)this->getChildByTag(T_RECOVERY_BACK)->getChildByTag(T_RECOVERY_LABEL);
    auto back = (Label *)this->getChildByTag(T_RECOVERY_BACK);
    
    //体力残り回復時間の取得
    time_t remain_time = EVENT_POWER_HEAL_TIME - (_check_time - _event_power_use_time);
//    HMSLOG("remain_time = %ld", remain_time);
    
    int minute = (int)remain_time / 60; //分
    int second = (int)remain_time % 60; //秒
    int hour = (int)minute / 60 ; //時間
    minute = (int)minute % 60 ;
    
//    HMSLOG("minute = %d, second = %d", minute ,second);
    
    //2桁で表示させる処理
    if (minute >= 0 && second >= 0)
    {
        
        string second_text = toString<int>(second);
        if (second < 10)
        {
            second_text = "0" + toString<int>(second);
        }
        
        string remain_text = "あと" ;
        if(hour > 0){
            remain_text += toString<int>(hour) + ":"  ;
        }
        remain_text += toString<int>(minute) + ":" + second_text;
        
        back->setVisible(true) ;
        label->setString(remain_text);
    }
    
    if (remain_time <= 0)
    {
        //体力回復
        int add_point = 1;
        if (0 < abs(remain_time / EVENT_POWER_HEAL_TIME))
        {
            //スリープ時の体力が１つ以上増えた場合の対策処理
            add_point += (int)abs(remain_time / EVENT_POWER_HEAL_TIME);
        }
        
        _now_event_power += add_point;
        
        if (_now_event_power > _max_event_power) {
            _now_event_power = _max_event_power;

        }
        SaveData::saveString("event_power", toString<int>(_now_event_power));
        
        //最後に体力を使った時間の更新
        _event_power_use_time += (EVENT_POWER_HEAL_TIME * add_point);
        if (_event_power_use_time > _check_time)
        {
            _event_power_use_time = _check_time;
        }
        
        savePowerUseTime(_event_power_use_time) ;
        
        //回復アクション
        recoveryAction();
    }
}

void EventPowerSprite::recoveryAction()
{
    updateEventPowerMark(true) ;
}