#ifndef __war__MapStampMenu__
#define __war__MapStampMenu__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsVector.h"

class MapStampMenu : public Menu
{
private:
    float ratio;
    
public:
    CREATE_FUNC(MapStampMenu);	//create関数
    virtual bool init();			//init関数
    
    virtual bool initWithArray(const Vector<MenuItem*>& arrayOfItems);
//    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
//    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
};
#endif /* defined(__war__MapStampMenu__) */
