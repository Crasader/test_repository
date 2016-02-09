
#ifndef __war__PopupOk__
#define __war__PopupOk__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"


USING_NS_CC;
using namespace std;

class PopupOk : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(PopupOk);
    void setPopup(string messaage, std::function<void()> func);
    
};


#endif /* defined(__war__PopupOk__) */
