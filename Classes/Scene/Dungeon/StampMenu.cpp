
#include "StampMenu.h"
#include "DungeonScene.h"
#include "TurnManage.h"
#include "StampLayer.h"

bool StampMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    ratio = getVisibleRatio();
    

    return true;
}

bool StampMenu::initWithArray(const vector<MenuItem *> arrayOfItems)
{
    if (Layer::init())
    {
        _enabled = true;
        // menu in the center of the screen
        Size s = Director::getInstance()->getWinSize();
        
        this->ignoreAnchorPointForPosition(true);
        setAnchorPoint(Vec2(0.5f, 0.5f));
        this->setContentSize(s);
        
        setPosition(s.width/2, s.height/2);
        
        int z=0;
        
        for (auto& item : arrayOfItems)
        {
            this->addChild(item, z);
            z++;
        }
        
        _selectedItem = nullptr;
        _state = Menu::State::WAITING;
        
        // enable cascade color and opacity on menus
        setCascadeColorEnabled(true);
        setCascadeOpacityEnabled(true);
        
        
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->setSwallowTouches(false);//ここを変えた
        
        touchListener->onTouchBegan = CC_CALLBACK_2(Menu::onTouchBegan, this);
        touchListener->onTouchMoved = CC_CALLBACK_2(Menu::onTouchMoved, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(Menu::onTouchEnded, this);
        touchListener->onTouchCancelled = CC_CALLBACK_2(Menu::onTouchCancelled, this);
        
        //_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
        //_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 2);
        
        return true;
    }
    return false;
}

/*bool StampMenu::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    CCLOG("タッチー");
    if(touch->getLocation().x < 360) return false;
    return Menu::onTouchBegan(touch, event);
}
*/

/*void StampMenu::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(static_cast<StampScroll *>(getParent())->swipe_distance > 20.0f){
        CCLOG("スタンプ停止");
        Menu::onTouchCancelled(touch, event);
    }else{
        CCLOG("スタンプ停止ではない");
        Menu::onTouchEnded(touch, event);
    }
}
*/
