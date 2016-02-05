#ifndef __war__StampLayer__
#define __war__StampLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsVector.h"
#include "StampScroll.h"

USING_NS_CC;
#define STAMP_ACTION_TAG 50052

class StampLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    float ratio;
    StampScroll *stamp_scroll;
    
public:
    CREATE_FUNC(StampLayer);	//create関数
    virtual bool init();			//init関数
    
    Sprite *bg;
    Sprite *close_bt;
    Sprite *mute_bt;
    vector<Sprite *> stamp_spr;
    vector<bool> stamp_clicked;
    
    vector<Menu *> stamp_menu;
    
    void close();
    
    MenuItemSprite * right_item;
    MenuItemSprite * left_item;
    
    //タップ処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
};

#endif /* defined(__war__StampLayer__) */
