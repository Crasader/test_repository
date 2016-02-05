
#ifndef __war__StampScroll__
#define __war__StampScroll__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsVector.h"
#include "cocos-ext.h"
#include "StampMenu.h"

USING_NS_CC;
USING_NS_CC_EXT;
#define STAMP_ACTION_TAG 50052

class StampScroll : public ScrollView
{
private:
    float ratio;
    
    //Point touch_in_point;
    
public:
    //float swipe_distance = 0.0f;
    int now_page = 1;
    
    CREATE_FUNC(StampScroll);	//create関数
    virtual bool init();			//init関数
    
    EventListenerTouchOneByOne *listener;
    vector<Label *> stamp_label;
    Menu *stamp_menu;
    
    void makeScroll();
    void onStampButton(Ref *aaa);
    Vector<MenuItem *> items;

    
    //タップ処理
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
};


#endif /* defined(__war__StampScroll__) */
