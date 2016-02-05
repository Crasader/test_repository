#ifndef __war__MapTreasureLayer__
#define __war__MapTreasureLayer__

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class MapTreasureLayer : public Layer
{
private:
    float ratio;

    CC_SYNTHESIZE(int, _position, Point);//xy座標
    CC_SYNTHESIZE(string, _treasure_id, TreasureId);//お宝ID
    CC_SYNTHESIZE(string, _type, Type);//お宝タイプ
    CC_SYNTHESIZE(bool, _open_flg, OpenFlg);//開封済かどうか
public:
    virtual bool init();
    CREATE_FUNC(MapTreasureLayer);
    
    void initialize(Point position, string treasure_id, string type, bool open_flg);
    void open();
};

#endif /* defined(__war__MapTreasureLayer__) */
