#ifndef __War__GeneralSelectLayer__
#define __War__GeneralSelectLayer__

#include "cocos2d.h"
#include "TutorialScene.h"

USING_NS_CC;
using namespace std;

class GeneralSelectLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    float _touchFlg;

public:
    float ratio;
    Sprite* _touchSprite;
    MessageSprite* message_bg;
    
    virtual bool init();
    CREATE_FUNC(GeneralSelectLayer);
    
    void createEventLitener(); //イベントリスナーの作成
    void createMessageBg(); //メッセージ背景の作成
    void setupImage(); //画像の作成
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__GeneralSelectLayer__) */
