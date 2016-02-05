#ifndef __TimeMeasure__
#define __TimeMeasure__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"
#include <chrono>

using namespace std;
USING_NS_CC;

class TimeMeasure {
private:
    static std::map<int, clock_t> _start_times ;
    static int _unique_id ;
    
public:
    //startが呼ばれてからendAndPrintが呼ばれるまでの時間を計測
    //startメソッドを呼んだときに発行されるidをendAndPrintメソッドに渡すと経過時間を計測出来る。
    static int start();
    static void endAndPrint(int unique_id);
};

#endif // __TimeMeasure__ 
