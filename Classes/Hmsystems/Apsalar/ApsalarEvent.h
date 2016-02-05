#ifndef __APSALAR_EVENT_H__
#define __APSALAR_EVENT_H__

#include "cocos2d.h"

class ApsalarEvent
{
public:
    static void sendEvent(std::string event_name);
};

#endif // __APSALAR_EVENT_H__
