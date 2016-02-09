#ifndef __war__PopupSelectBoxLayer__
#define __war__PopupSelectBoxLayer__

#include "cocos2d.h"
#include "HmsVector.h"

USING_NS_CC;
using namespace std;

class PopupSelectBoxLayer: public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    Sprite* background; //背景
    HmsVector<string> imagePaths;
    HmsVector<int> numbers;
    HmsVector<Sprite*> contentsSprites;
    bool isShadow;
    int nowIndex;
    float imageHeight;
    const float MARGIN = 20.0f;
    Vec2 center;
    Vec2 prevTouchPosition;
    Vec2 velocity;
    bool isMoving;
    ClippingNode* clipper ;
    
    CC_SYNTHESIZE(function<void()>, _close_func, CloseFunc); //閉じるボタンを押したときに呼ばれるfunction
    CC_SYNTHESIZE(function<void()>, selectFunc, SelectFunc); //決定ボタンを押した時に呼ばれるfunction
    
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Center_Bar,
        T_Contents,
        T_Close_Button,
        T_Ok_Button,
        T_Popup_Layer
    };
    
    //ZOrder
    enum ZOrder{
        Z_Shadow = 1,
        Z_Center_Bar,
        Z_Contents,
        Z_Close_Button,
        Z_Ok_Button,
        Z_Popup_Layer
    };
    
    float ratio;
    
    PopupSelectBoxLayer(); //コンストラクタ
    
    static PopupSelectBoxLayer* create(HmsVector<string> imagePaths,HmsVector<int> number, int defaultNumber, bool isShadow = true);
    virtual bool init(HmsVector<string> imagePaths, HmsVector<int> number, int defaultNumber, bool isShadowshadow_flg = true);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createCenterBar();
    void createContentsImage();
    void createOkButton();
    int getSelectNumber();
    bool isContainPopupPosition(Vec2 position);
    
    void updateContentsPosition(float t);
    int culcClosestCenterIndex();
    void initClippingNode();
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    void onEnter() ; //初期実行関数
    
};


#endif /* defined(__war__PopupSelectBoxLayer__) */
