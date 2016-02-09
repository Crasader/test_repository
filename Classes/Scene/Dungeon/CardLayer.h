
#ifndef __war__CardLayer__
#define __war__CardLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "Chara.h"

USING_NS_CC;
using namespace std;

class CardLayer : public Layer
{
public:
    virtual bool init();
    CREATE_FUNC(CardLayer);
    
    void initialize();
    void iconChangeOrder(float dt);
    SpriteBatchNode *batchNode;
    
    RenderTexture *white_filter = nullptr;
    void whiteFilterCreate(vector<Point> field_place_vec);
    void whiteFilterCreatePlace(vector<pair<int,int>> field_place_vec);
};

#endif /* defined(__war__CardLayer__) */
