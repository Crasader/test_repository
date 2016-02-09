#ifndef __war__BazaarListRefreshButton__
#define __war__BazaarListRefreshButton__

#include "cocos2d.h"
#include "NormalScene.h"
#include "../BazaarListLayer.h"



USING_NS_CC;
using namespace std;
using namespace json11;

class BazaarListRefreshButton : public MenuItemSprite
{
private:
    EventListenerTouchOneByOne* listener;
    Sprite* buttonSprite;
    BazaarListLayer* bazaarListLayer;
    
public:
    float ratio;
    static BazaarListRefreshButton* Create(string imagePath1, string imagePath2, BazaarListLayer* bazaarListLayer);
    virtual bool init(string imagePath1, string imagePath2, BazaarListLayer* bazaarListLayer);
    
    void OnTapAction(Ref* sender);
    
    void initEventListener(); //イベントリスナーの設定
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
};

#endif /* defined(__war__BazaarListRefreshButton__) */
