#ifndef TournamentStartAnimationLayer_hpp
#define TournamentStartAnimationLayer_hpp

#include "cocos2d.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "HomeCommon.h"
#include "HMSFontUtil.h"
#include "NormalScene.h"

USING_NS_CC;

class TournamentStartAnimationLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
public:
    
    enum Tag{
        T_Shadow = 1,
        T_Obi,
        T_Message,
        T_Navi,
    };
    
    enum ZOrder{
        Z_Shadow = 1,
        Z_Obi,
        Z_Message,
        Z_Navi,
    };
    
    Node* _node; //ノーマルシーンのアドレスを保持するノード
    
    float ratio;
    
    static TournamentStartAnimationLayer* create(Node* node);
    virtual bool init(Node* node);
//    CREATE_FUNC(TournamentStartAnimationLayer);
    
    void initEventLitsener();
    void createImage();
    void animationStart(float dt);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* TournamentStartAnimationLayer_hpp */
