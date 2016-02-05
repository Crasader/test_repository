#ifndef __War__StoneShopBuyConfirmLayer__
#define __War__StoneShopBuyConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"

#define CONFIRM_MENU_ITEM_TAG 6

USING_NS_CC;

class StoneShopBuyConfirmLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    void initEventListener();
    
    
    void onFirstFunc(Ref* obj); //はいボタンコールバック
    void onSecondFunc(Ref* obj); //いいえボタンコールバック
    
    function<void()> _firstFunc ;
    function<void()> _secondFunc ;
    string _message ;
    
public:
    enum class ButtonType{
        YES_AND_NO= 1,
        OK, //OKの場合firstFuncにのみラムダ式を入れればOK
    };
    //ボタンの種類
    ButtonType _type ;
    
    float ratio;
    Size visibleSize;
    
    static StoneShopBuyConfirmLayer* create(ButtonType type, string message, function<void()> firstFunc, function<void()> secondFunc) ;
    virtual bool init(ButtonType type, string message, function<void()> &firstFunc,function<void()> &secondFunc);
    void setupImage();
    
    void popupAction();
    void deleteAction();
    
    //初期実行関数
    void onEnter();
    
};

#endif /* defined(__War__StoneShopBuyConfirmLayer__) */