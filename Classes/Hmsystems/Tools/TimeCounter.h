#ifndef __war__TimeCounter__
#define __war__TimeCounter__

#include "cocos2d.h"

USING_NS_CC;

/*

 与えられたend_dateまでカウントを続ける
 初期化に失敗した場合nullptrを返す
 
 生成例 :
    TimeCounter *timeCounter = TimeCounter::createWithDateTimeFormat(json_banner_data["end_date"].string_value(), 1,
         [this](TimeCounter::CountState count_state, time_t now_time, time_t end_time){
 
         if(count_state == TimeCounter::CountState::COUNT_END){
         }else if(count_state == TimeCounter::CountState::NORMAL_COUNT){
         }else if(count_state == TimeCounter::CountState::COUNT_FAILED_END){
         }
 
        CCLOG("now_time: %d", (int)now_time) ;
        CCLOG("end_time: %d", (int)end_time) ;
    });
 
    addChild(timeCounter) ;
 
 
 見えないのでどのNodeに追加しても良いですが、
 時間管理の期間と一致するNodeに追加してください
 例えば、複数あるバナーの残り時間を表示する際はバナーにaddChildしてください
 */

class TimeCounter : public Node{
 public:
    
    enum CountState{
        COUNT_END, //カウント終了時 end
        NORMAL_COUNT, //普通にカウントしてる時
        COUNT_FAILED_END, //生成に失敗した場合
    };
    
    static TimeCounter* createWithDateTimeFormat(std::string end_time_date_time_format, float interval, const std::function<void(CountState count_state, time_t now_time, time_t end_time)> countActionFunc); // 例:2014-01-02 12:12:21 mysql> select now();
    static TimeCounter* createWithTimeStampFormat(time_t end_time, float interval, const std::function<void(CountState count_state, time_t now_time, time_t end_time)> countActionFunc); // 例:1454399708  mysql> select unix_timestamp(now()); で得られる値で初期化
    
    void onEnter(); //初期実行関数
    
    
 private:
    virtual bool init(time_t end_time, float interval, const std::function<void(CountState count_state, time_t now_time, time_t end_time)> countActionFunc);
    time_t _end_time ;
    float _interval ;
    
    std::function<void(CountState count_state, time_t now_time, time_t end_time)> _countActionFunc; //intevalの間隔で呼ばれる、引数のCountStateで状況を取得できる。
    
    void updateCount(float delta);
};

#endif /* defined(__war__TimeCounter__) */
