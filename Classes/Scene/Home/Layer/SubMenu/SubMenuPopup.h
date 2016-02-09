#ifndef __war__SubMenuPopup__
#define __war__SubMenuPopup__

#include "cocos2d.h"
#include "NormalScene.h"

#define ACTION_SPRITE_TAG 1

USING_NS_CC;

class SubMenuPopup : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _return_page_name;
    
public:
    float ratio;
    Size visibleSize;
    
    static SubMenuPopup* create(string return_page_name);
    virtual bool init(string return_page_name);
    //    CREATE_FUNC(SubMenuPopup);
    
    void setupImage(string text);
    
    void popupAction();
    void deleteAction(float dt);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};
#endif /* defined(__war__SubMenuPopup__) */
