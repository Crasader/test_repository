#ifndef __War__Numeric__
#define __War__Numeric__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class Numeric : public Layer
{
private:
    int now_value;
    int counter;
    
    vector<Sprite *> sprite_vec;
public:
    int last_value;
    int target_value=0;
    
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Numeric);
    
    void setValue(int value, bool counting_flg = true);
    //void setValue(int value);
    void setData(int value);
    void setFont(bool high_flg = false);
    void update(float dt);
    
    //void setValueEffect(int old_value, int new_value);
};

#endif /* defined(__War__Numeric__) */
