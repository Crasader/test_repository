#ifndef __war__MapStampScroll__
#define __war__MapStampScroll__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsVector.h"
#include "cocos-ext.h"
#include "MapStampMenu.h"

USING_NS_CC;
USING_NS_CC_EXT;
#define STAMP_ACTION_TAG 50052

class MapStampScroll : public ScrollView
{
private:
    float ratio;
    
    Point touch_in_point;
    
public:
    float swipe_distance = 0.0f;
    int now_page = 1;
    
    CREATE_FUNC(MapStampScroll);	//create関数
    virtual bool init();			//init関数
    
    EventListenerTouchOneByOne *listener;
    Menu *stamp_menu;
    
    void makeScroll();
    void onStampButton(Ref *aaa);
    Vector<MenuItem *> items;

    
    //タップ処理
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
};


#endif /* defined(__war__MapStampScroll__) */
