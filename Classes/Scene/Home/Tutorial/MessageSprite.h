#ifndef __War__MessageSprite__
#define __War__MessageSprite__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "common.h"

USING_NS_CC;
using namespace std;

class MessageSprite : public cocos2d::Sprite
{
private:
    string _message;
    Label* _message_label;
    
public:
    static MessageSprite* create(string message, TextHAlignment alignment = TextHAlignment::CENTER);
    virtual bool init(string message, TextHAlignment alignment);
//    CREATE_FUNC(MessageSprite);
    
    void showMessage(string message, TextHAlignment alignment); //メッセージの表示
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__MessageSprite__) */
