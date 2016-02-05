#ifndef __war__DungeonSubMenuSurrender__
#define __war__DungeonSubMenuSurrender__

#include "cocos2d.h"
#include "DungeonScene.h"
#include "NormalScene.h"
#include "DungeonSubMenu.h"

USING_NS_CC;

class DungeonSubMenuSurrenderLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;    
    
    void onCancelButton(Ref* obj);
    void onOkButton(Ref* obj);
public:
    float ratio;
    
    int halfWidth;
    int halfHeight;
    
    Sprite* bg;
    
    Size visibleSize;
    
    virtual bool init();
    CREATE_FUNC(DungeonSubMenuSurrenderLayer);
    
    void createEventListener(); //イベントリスナーの作成
    void createBg(); //背景画像の作成
    void createButton(); //ボタンの作成
    void createLabel(); //確認用文字の作成
    
    void popupAction();
    void deleteAction();
    
    //初期実行関数
    void onEnter();
};

#endif /* defined(__war__DungeonSubMenuSurrender__) */
