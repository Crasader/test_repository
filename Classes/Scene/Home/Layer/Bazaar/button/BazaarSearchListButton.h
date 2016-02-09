#ifndef __war__BazaarSearchListButton__
#define __war__BazaarSearchListButton__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class BazaarSearchListButton : public MenuItemSprite
{
private:
    EventListenerTouchOneByOne* listener;
    Sprite* buttonSprite;
    
public:
    float ratio;
    static BazaarSearchListButton* Create(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer);
    virtual bool init(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer);
    
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
