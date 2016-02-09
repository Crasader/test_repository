#ifndef __War__Effect__
#define __War__Effect__

#include "cocos2d.h"
#include "HmsConsoleUtil.h"

USING_NS_CC;
using namespace std;

class Effect : public Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Effect);
    
    void setData(Layer *obj, string filename, Vec2 position, float scale);
    void setData(Layer *obj, string filename, int count, Vec2 position, float ratio_, int z);
    void setData(Layer *obj, string filename, int count, Vec2 position, float ratio_, int z, int roop_count);
};

#endif /* defined(__War__Effect__) */
