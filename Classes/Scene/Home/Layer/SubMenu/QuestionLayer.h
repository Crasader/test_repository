#ifndef __war__QuestionLayer__
#define __war__QuestionLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "SubMenuLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "ModalLayer.h"
#include "NativeBridge.h"

#define MESSEGE_BAR_TAG 1

#define ZORDER_BAR 1

#define QUESTION_B_URL "http://www.yahoo.co.jp/"  //ブラウザで開くURL

USING_NS_CC;

class QuestionLayer : public Layer
{
protected:
    enum btnMenuZOrder {
        Z_button = 1,
        Z_label,
    };
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(QuestionLayer);
    
    //画像の設定
    void setupImage();
    
    void questionPage(Ref* sender);
    
    void changePage(Ref* sender);
    
    void onEnter();
};
#endif /* defined(__war__QuestionLayer__) */
