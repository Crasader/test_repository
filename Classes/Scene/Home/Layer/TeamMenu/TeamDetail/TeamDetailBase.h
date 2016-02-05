#ifndef __War__TeamDetailBase__
#define __War__TeamDetailBase__

#include "cocos2d.h"
#include "NormalScene.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;
using namespace json11;

class TeamDetailBase : public Layer
{
protected:
    EventListenerTouchOneByOne *listener;
    Json teamData; //チームデータ
    float ratio;
    Sprite* frame;
    
public:
    static TeamDetailBase* create(json11::Json json);
    bool init(json11::Json json);
    void onEnter(); //初期実行関数
    
    void setupData();
    void createCloseButton();
    void refreshList();
    
    virtual void createLayout();
    virtual void createButton();
    virtual void buttonTapAction();
    virtual void buttonTapCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    //イベントリスナー処理
    void createEventListener(); //イベントリスナーの作成
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif /* defined(__War__TeamDetailBase__) */
