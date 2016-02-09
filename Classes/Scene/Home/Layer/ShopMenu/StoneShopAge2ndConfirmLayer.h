#ifndef __War__StoneShopAge2ndConfirmLayer__
#define __War__StoneShopAge2ndConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"

#define CONFIRM_MENU_TAG_1 3
#define CONFIRM_MENU_TAG_2 4
#define CONFIRM_MENU_ITEM_TAG 6

USING_NS_CC;

class StoneShopAge2ndConfirmLayer : public cocos2d::Layer
{
    
private:
    EventListenerTouchOneByOne *listener;
	int used_age_type;
	
    void initEventListener();
    void onNoButton(Ref* obj); //いいえボタンコールバック
    void onYesButton(Ref* obj); //はいボタンコールバック 
public:
    float ratio;
    Size visibleSize;
    
    virtual bool init();
    CREATE_FUNC(StoneShopAge2ndConfirmLayer);
    void setupImage(string confirmMessage);
	void setupImage(string confirmMessage, int age_type);
    
    void popupAction();
    void deleteAction();
    
    //初期実行関数
    void onEnter();
    
};

#endif /* defined(__War__StoneShopAge2ndConfirmLayer__) */
