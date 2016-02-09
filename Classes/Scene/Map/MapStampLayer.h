#ifndef __war__MapStampLayer__
#define __war__MapStampLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsVector.h"
#include "MapStampScroll.h"

USING_NS_CC;
#define STAMP_ACTION_TAG 50052

class MapStampLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    float ratio;
    MapStampScroll *stamp_scroll;
    
public:
    CREATE_FUNC(MapStampLayer);	//create関数
    virtual bool init();			//init関数
    
    Sprite *bg;
    Sprite *close_bt;
    Sprite *mute_bt;
    HmsVector<Sprite *> stamp_spr;
    vector<bool> stamp_clicked;
    
    HmsVector<Menu *> stamp_menu;
    
    void close();
    
    MenuItemSprite * right_item;
    MenuItemSprite * left_item;
    
    //タップ処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
};

#endif /* defined(__war__MapStampLayer__) */
