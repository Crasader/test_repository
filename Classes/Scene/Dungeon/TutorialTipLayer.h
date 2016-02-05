#ifndef __War__TutorialTipLayer__
#define __War__TutorialTipLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "UserInfoCellSprite.h"
#include "HmsList.h"
#include "HomeCommon.h"
#include "SimpleAudioEngine.h"

#define MESSEGE_BAR_TAG 1

USING_NS_CC;

#define IMG_OK_BT "init/Home/Common/bt_ok.png"                  //OKボタン画像

class TutorialTipLayer : public Layer
{
private:
    float ratio;
    
    EventListenerTouchOneByOne *listener;
    void initListener();
    bool onTouchBegan(Touch *touch, Event *event) ;
    void onEnter(); //初期実行関数
public:
    virtual bool init();
    CREATE_FUNC(TutorialTipLayer);
    
    void setup(int index);

    //基本
    void popupAction();
    void deleteAction();
    void close();
};

#endif /* defined(__War__TutorialTipLayer__) */
