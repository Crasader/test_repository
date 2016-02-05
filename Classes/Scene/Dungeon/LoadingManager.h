
#ifndef __war__LoadingManager__
#define __war__LoadingManager__

#include <stdio.h>
#include "cocos2d.h"
#include "HmsConsoleUtil.h"
#include "HmsGeometricFunction.h"
#include "SuperAnimNodeV2.h"
#include "HomeCommon.h"

USING_NS_CC;

class LoadingManager : public cocos2d::Layer, public SuperAnim::SuperAnimNodeListener
{
private:
    static LoadingManager *loading_manager;
    EventListenerTouchOneByOne *listener;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(LoadingManager);
    static LoadingManager* setLoading(Layer *layer, string option = "");
    
    //void showEffect(string filename, Vec2 position, float scale);
    
    void onExit();
    static void removeLoading();
};

#endif /* defined(__war__LoadingManager__) */
