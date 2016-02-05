
#ifndef __war__HMSLogUtil__
#define __war__HMSLogUtil__ 

#include "cocos2d.h"
#include "common.h"

USING_NS_CC;
using namespace std;

#define HMSLOG(format, ...)      HmsConsoleUtil::Log(format, ##__VA_ARGS__)

class HmsConsoleUtil{
    
public:
    //有効か無効か切り替え
    static void setVisibleLog(bool b) ;
    
    //CCLOGと同じ引数を渡してログ出力
    static void Log(const char * format, ...) ;
    
private:
    //ログの有効か無効か切り替えるフラグ
    static bool _visible_log ;
    
    //CCConsole.cppをそのまま流用
    static void _log(const char *format, va_list args);
};

#endif /* defined(__war__HMSLogUtil__) */
