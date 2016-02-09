#ifndef __war__WebsocketTestChatList__
#define __war__WebsocketTestChatList__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "hmsVector.h"

USING_NS_CC;
USING_NS_CC_EXT;

class WebsocketTestChatSprite;

class WebsocketTestChatList : public Layer, public ScrollViewDelegate
{
private:
    class ChatTextData : public Node{
    private:
        CC_SYNTHESIZE(string, _chatText, chatText);
        CC_SYNTHESIZE(bool, _chatType, chatType);
    public:
        CREATE_FUNC(ChatTextData);
        virtual bool init(){
            if ( !Node::init() )return false;
            return true;
        }
    };
    
    EventListenerTouchOneByOne* listener;
    HmsVector<WebsocketTestChatSprite *> _chatSpriteList;
    HmsVector<ChatTextData *> _chatTextList;
    int cellSize; //セルの大きさ
    float ratio;
    
    //タグ
    enum Tag{
        T_ScrollView = 1,
        T_ScrollLayer
    };
    
    //ZOrder
    enum ZOrder{
        Z_ScrollView = 1,
        Z_ScrollLayer
    };
    
    
    void initEventListener(); //イベントリスナーの設定
    Vec2 curriculateChatPosition(int i, bool isMine);
    void createScrollView();
    Layer* createList();
    void setScrollViewData();
    void createListLayer();
    //更新時の再生成
    void refreshList();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数

    
public:
    static WebsocketTestChatList* create();
    virtual bool init();
    
    void addChatText(string text, bool isMine);
};

#endif /* defined(__war__WebsocketTestChatList__) */
