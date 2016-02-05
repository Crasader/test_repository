#include "TimeMeasure.h"

int TimeMeasure::_unique_id ;
map<int, clock_t> TimeMeasure::_start_times;

int TimeMeasure::start(){
    _unique_id++ ;
    _start_times[_unique_id] = clock();
    HMSLOG("========= TimeMeasure start || id:%d ========= ", _unique_id);
    return _unique_id ;
}

void TimeMeasure::endAndPrint(int unique_id){
    if(unique_id > _unique_id)HMSLOG("this unique_id(%d) is not found", unique_id) ;
    else HMSLOG("========= TimeMeasure end || id:%d 秒:%f ========= ", unique_id ,(double)(clock() - _start_times[unique_id]) / CLOCKS_PER_SEC);
}