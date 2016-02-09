#ifndef __war__DungeonSubMenu__
#define __war__DungeonSubMenu__

#include "cocos2d.h"
#include "DungeonScene.h"
#include "DungeonSubMenuSurrender.h"
#include "DungeonSubMenuOption.h"

USING_NS_CC;

class DungeonSubMenuLayer : public cocos2d::Layer
{
private:
    enum Order{
        ORDER_RECEIVE_LAYER=20,
    };
    
    EventListenerTouchOneByOne *listener;
    
    void onSurrenderButton(Ref* obj);   //降伏ボタンの実行関数
    void onOptionButton(Ref* obj);  //オプションボタン
    
    MenuItemSprite* _option_btn; //オプションボタン
    MenuItemSprite* _surrender_btn; //降伏ボタン
    MenuItemImage* _tutorial_finish_btn; //チュートリアル終了ボタン
    
    Label* _alert_surrender_label; //降伏できない警告ラベル
    Label* turn_label;//ターン数表示のラベル
    
public:
    float ratio;
    
    int halfWidth;
    int halfHeight;
    
    Sprite* bg;
    Sprite* shadow;
    
    Size visibleSize;
    
    virtual bool init();
    CREATE_FUNC(DungeonSubMenuLayer);
    
    void createEventListener(); //イベントリスナーの作成
    void createBg(); //背景画像の作成
    
    void deleteAction();
    //void changeSurrenderButton(); //降伏ボタンの切り替え
    
    //初期実行関数
    void onEnter();
};

#endif /* defined(__war__DungeonSubMenu__) */
