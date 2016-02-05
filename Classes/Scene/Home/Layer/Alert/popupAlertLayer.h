#ifndef __War__popupAlertLayer__
#define __War__popupAlertLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class popupAlertLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    string _return_page_name;
    
public:
    float ratio;
    Size visibleSize;
    
    static popupAlertLayer* create(string return_page_name);
    virtual bool init(string return_page_name);
//    CREATE_FUNC(popupAlertLayer);
    
    void setupData(string text); //画像などの表示
    void popupAction(); //ポップアップアクション
    void deleteAction(float dt); //消すアクション
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif /* defined(__War__popupAlertLayer__) */
