#ifndef _TEAM_INFO_LAYER_H_
#define _TEAM_INFO_LAYER_H_

#include "cocos2d.h"
#include "common.h"
#include "XTLayer.h"

USING_NS_CC;
using namespace std;

//インフォ
class TeamInfoLayer : public XTLayer
{
protected:
    
public:
    virtual bool init();
    CREATE_FUNC(TeamInfoLayer);
    
    //セット
    void setData();
    
    //タップイベント
    virtual void xtTouchesBegan(cocos2d::CCSet* _touches, cocos2d::CCEvent* event);
    virtual void xtTouchesMoved(cocos2d::CCSet* _touches, cocos2d::CCEvent* event);
    virtual void xtTouchesEnded(cocos2d::CCSet* _touches, cocos2d::CCEvent* event);
    
};

#endif