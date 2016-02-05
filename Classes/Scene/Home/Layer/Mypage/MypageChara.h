#ifndef __War__MypageChara__
#define __War__MypageChara__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class MypageChara : public Sprite
{
private:
    CC_SYNTHESIZE(int, _primaryId, primaryId);
    Layer *parent_layer;
public:
    MypageChara(); //コンストラクタ
    
    static MypageChara* create(const string filename);
    virtual bool init(string filename);
    
    void onEnter(); //初期実行関数
    
    void setParentLayer();
};

#endif /* defined(__War__MypageChara__) */
