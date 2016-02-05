#ifndef __war__HexLayer__
#define __war__HexLayer__

#include <stdio.h>
#include "cocos2d.h"

#define HEX_GREEN_FRAME_Z 1
#define HEX_GREEN_FRAME_TAG 1

USING_NS_CC;
using namespace std;

class HexLayer : public Layer
{
private:
    float ratio;
    
    CC_SYNTHESIZE(int, _hex_no, HexNo);//hexマスNo
    CC_SYNTHESIZE(int, _camp_flg, CampFlg);//キャンプフラグ
public:
    virtual bool init();
    CREATE_FUNC(HexLayer);
    
    void initialize(int hex_no, bool camp_flg);
};

#endif /* defined(__war__HexLayer__) */
