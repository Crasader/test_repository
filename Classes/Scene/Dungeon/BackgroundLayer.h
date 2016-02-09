#ifndef __war__BackgroundLayer__
#define __war__BackgroundLayer__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class BackgroundLayer : public Layer
{
public:
    static BackgroundLayer* create(string background_path, int area_battle_flg);
    virtual bool init(string background_path, int area_battle_flg);
//    CREATE_FUNC(BackgroundLayer);

    string _background_path;
    int _area_battle_flg;

};
#endif /* defined(__war__BackgroundLayer__) */
