#ifndef __war__AllTapDisableLayer__
#define __war__AllTapDisableLayer__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"

USING_NS_CC;

class AllTapDisableLayer: public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(AllTapDisableLayer);
    void initEventListener();
};


#endif /* defined(__war__AllTapDisableLayer__) */


