#ifndef __war__HelpLayer__
#define __war__HelpLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "SubMenuLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"

#define MESSEGE_BAR_TAG 1

#define ZORDER_BAR 1

USING_NS_CC;

class HelpLayer : public Layer
{
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(HelpLayer);
    
    //画像の設定
    void setupImage();
    
    void changePage(Ref* sender);
    
    void onEnter();
};
#endif /* defined(__war__HelpLayer__) */
