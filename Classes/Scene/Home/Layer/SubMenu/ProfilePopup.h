#ifndef __war__ProfilePopup__
#define __war__ProfilePopup__

#include "cocos2d.h"
#include "NormalScene.h"

#define ACTION_SPRITE_TAG 1

USING_NS_CC;

class ProfilePopup : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _return_page_name;
    string chage_page;
    string profilehikaku="ProfilePopup";
    string submenuhikaku="SubMenu";
    string otherhikaku="Other";
    string shophikaku="Shop";
    
public:
    float ratio;
    Size visibleSize;
    string change_page;
    
    static ProfilePopup* create(string return_page_name,string page);
    virtual bool init(string return_page_name,string page);
    //    CREATE_FUNC(ProfilePopup);
    
    void setupImage(string text,string page);
    
    void popupAction();
    void deleteAction(float dt);
    void deleteAction2(float dt);
    void deleteAction3(float dt);
    void deleteAction4(float dt);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};


#endif /* defined(__war__ProfilePopup__) */
