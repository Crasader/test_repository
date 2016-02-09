#ifndef __war__TitleBarLayer__
#define __war__TitleBarLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "HmsVector.h"

USING_NS_CC;

class TitleBarLayer : public cocos2d::Layer
{
public:
    
    enum Tag{
        T_Title_Bar = 1,
        T_Back_Button,
        T_Title_Label,
        T_Button,
    };
    
    enum Zorder{
        Z_Title_Bar = 1,
        Z_Back_Button,
        Z_Title_Label,
        Z_Button,
    };
    
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TitleBarLayer);
};

#endif /* defined(__war__TitleBarLayer__) */
