#ifndef __war__ScrollBarLayer__
#define __war__ScrollBarLayer__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "HmsGeometricFunction.h"

USING_NS_CC;

class ScrollBarLayer : public Layer
{
private:
    float barSize;
    float viewFixSize;
    float currentContentSize;
    float beforeContentSize;
    void initVertical();
    void setBarPosition(float offset);
    void setBarScale();
    cocos2d::extension::ScrollView *view;
    ui::Scale9Sprite *bar;
    EventListenerTouchOneByOne *listener;
    
    float header_height;
    float footer_height;
    
public:
    
    enum class BarType {
        VERTICAL_IN = 1,
        VERTICAL_OUT,
        HORIZONTAL_IN,
        HORIZONTAL_OUT,
    };

    
    void initEventListener(); //イベントリスナーの設定
    ScrollBarLayer(cocos2d::extension::ScrollView *view); //コンストラクタ
    
    float ratio;
    
    static ScrollBarLayer* create(cocos2d::extension::ScrollView *view);
    virtual bool init();
//    CREATE_FUNC(ScrollBarLayer);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    Point calcPos(cocos2d::extension::ScrollView *view, Point point);
                          
    void refresh();
//    void setBarSize(float barSize);
};

#endif /* defined(__war__ScrollBarLayer__) */
