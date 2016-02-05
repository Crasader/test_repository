
#ifndef __war__StampMenu__
#define __war__StampMenu__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsVector.h"

class StampMenu : public Menu
{
private:
    float ratio;
    
public:
    CREATE_FUNC(StampMenu);	//create関数
    virtual bool init();			//init関数
    
    virtual bool initWithArray(const vector<MenuItem *> arrayOfItems);
//    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
//    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
};
#endif /* defined(__war__StampMenu__) */
