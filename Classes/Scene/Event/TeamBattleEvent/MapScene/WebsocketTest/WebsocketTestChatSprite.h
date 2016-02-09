#ifndef __war__WebsocketTestChatSprite__
#define __war__WebsocketTestChatSprite__

#include "cocos2d.h"
#include "json11.hpp"


USING_NS_CC;
using namespace std;
using namespace json11;

class WebsocketTestChatSprite : public Sprite{
private:
    CC_SYNTHESIZE(bool, _isMine, IsMine);
    EventListenerTouchOneByOne *listener;
    
    Sprite* backgorund;
    Label* messageLabel;
    
    string messageStr;

    //タグ
    enum Tag{
        T_MESSAGE_LABEL = 1,
    };
    
    //ZOrder
    enum ZOrder{
        Z_MESSAGE_LABEL = 1,
    };
    
    float ratio;
    
    void createMessageLabel();
    
    void initEventListener(); //イベントリスナーの設定
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
    
public:
    static WebsocketTestChatSprite* create(string text, bool isMine);
    virtual bool init(string text, bool isMine);
    
};


#endif /* defined(__war__WebsocketTestChatSprite__) */
