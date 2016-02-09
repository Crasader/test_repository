#ifndef __war__BazaarSelectCostButton__
#define __war__BazaarSelectCostButton__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class PopupSelectBoxLayer;
class BazaarSearchListLayer;

class BazaarSelectCostButton : public MenuItemSprite
{
private:
    EventListenerTouchOneByOne* listener;
    Sprite* buttonSprite;
    Sprite* textSprite;
    PopupSelectBoxLayer* selectBox;
    BazaarSearchListLayer* baseLayer;
    HmsVector<string> pathList;
public:
    float ratio;
    static BazaarSelectCostButton* Create(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer);
    virtual bool init(string imagePath1, string imagePath2 , BazaarSearchListLayer* layer);
    void OnTapAction(Ref* sender);
    void createTextSprite();
    void initEventListener(); //イベントリスナーの設定
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
};

#endif