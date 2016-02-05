
#ifndef __war__ContinueLayer__
#define __war__ContinueLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "HttpConnect.h"

USING_NS_CC;
using namespace std;

class ContinueLayer : public Layer
{
private:
    float ratio;
    Sprite *bg_img;
    MenuItemSprite *yes_btn;
    MenuItemSprite *no_btn;
    
public:
    virtual bool init();
    CREATE_FUNC(ContinueLayer);
    
    void showDetail();
    void close();
};

#endif /* defined(__war__ContinueLayer__) */
