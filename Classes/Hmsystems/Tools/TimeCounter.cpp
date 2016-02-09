#include "TimeCounter.h"
#include "HmsConsoleUtil.h"

TimeCounter* TimeCounter::createWithDateTimeFormat(std::string date_time_format, float interval, const std::function<void(CountState count_state, time_t now_time, time_t end_time)> countActionFunc){
    
    //以上終了の通知
    if(date_time_format == "0000-00-00 00:00:00" || date_time_format == ""){
        time_t now_time;
        time(&now_time);
        CCLOG("CountState: %s", "COUNT_FAILED_END") ;
        countActionFunc(COUNT_FAILED_END, now_time, 0) ;
        return nullptr ;
    }
    
    struct tm tm;
    strptime(date_time_format.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
    
    return createWithTimeStampFormat(mktime(&tm), interval, countActionFunc) ;
}

TimeCounter* TimeCounter::createWithTimeStampFormat(time_t end_time, float interval, const std::function<void(CountState count_state, time_t now_time, time_t end_time)> countActionFunc){
    TimeCounter *node = new TimeCounter();
    
    if (node && node->init(end_time, interval, countActionFunc))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool TimeCounter::init(time_t end_time, float interval, const std::function<void(CountState count_state, time_t now_time, time_t end_time)> countActionFunc){
    if(!Node::init()) return false;
    HMSLOG("TimeCounter::init()");
    _end_time = end_time ;
    _countActionFunc = countActionFunc ;
    _interval = interval ;
    
    return true;
}

void TimeCounter::onEnter(){
    Node::onEnter() ;
    this->schedule(schedule_selector(TimeCounter::updateCount), _interval);
}

void TimeCounter::updateCount(float delta){
    //現在時刻の取得
    time_t now_time;
    time(&now_time);
    
    if(now_time>_end_time){
        //カウント終了通知
        CCLOG("CountState: %s", "COUNT_END") ;
        _countActionFunc(COUNT_END, now_time, _end_time) ;
        this->unschedule(schedule_selector(TimeCounter::updateCount));
    }else{
        //カウント通知
        CCLOG("CountState: %s", "NORMAL_COUNT") ;
        _countActionFunc(NORMAL_COUNT, now_time, _end_time) ;
    }
    
}
