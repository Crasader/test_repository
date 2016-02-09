/*いいえ・はいボタンのポップアップ*/

#ifndef __war__PopupNoYes__
#define __war__PopupNoYes__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"


USING_NS_CC;
using namespace std;

class PopupNoYes : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(PopupNoYes);
    void setPopup(string messaage, std::function<void()> no_func, std::function<void()> yes_func);
    
};

#endif /* defined(__war__PopupNoYes__) */
