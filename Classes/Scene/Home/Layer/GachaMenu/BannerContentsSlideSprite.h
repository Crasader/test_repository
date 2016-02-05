#ifndef __War__BannerContentsSlideSprite__
#define __War__BannerContentsSlideSprite__

#include "cocos2d.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "HmsVector.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;
using namespace CocosDenshion;

class BannerContentsSlideSprite : public cocos2d::Sprite
{
private:
    int _center_contents_index;
    
    void swipeEndSound() ;
    
public:
    
    BannerContentsSlideSprite();
    
    //バナーの個数
    CC_SYNTHESIZE(int, _banner_size, BannerSize);
    
    //現在の中心に位置するcontentsのindexを取得する
    int getCenterIndex() ;
    
    enum class ScrollDirection
    {
        HORIZONTAL = 0,
        VERTICAL,
    };
    
    //HmsVectorにSpriteを複数渡すと、複数がスクロールするスプライトを生成する,
    //第二引数でHORIZONTALを指定すると水平方向にスワイプ VERTICALの場合垂直方向に移動する
    static BannerContentsSlideSprite* create(HmsVector<Sprite*> contents);
    static BannerContentsSlideSprite* create(HmsVector<Sprite*> contents, ScrollDirection direction);
    void onEnter();//初期実行関数
    
    //与えたのVec2の上下か左右に移動する, HORIZONTALの場合x成分、VERTICALの場合y成分の分移動する
    void moveDelta(const Vec2& deltaVec2) ;
    
    //中心に移動する
    void autoSwipeCenter() ;
    
    //HORIZONTALの場合: 左にスワイプ VERTICALの場合: 上にスワイプ
    void autoSwipePrev() ;
    
    //HORIZONTALの場合: 右にスワイプ VERTICALの場合: 下にスワイプ
    void autoSwipeNext() ;
    
private:
    
    float _now_auto_swipe ;
    
    Point _center_contents_point ;
    Point _prev_contents_point ;
    Point _next_contents_point ;
    
    ScrollDirection _scroll_direction ;
    HmsVector<Sprite *> _contents;
    virtual bool init(HmsVector<Sprite*> contents, ScrollDirection direction);
    
    
    void initContentsPoint() ;
    void initClippingNode() ;
    void initContents() ;
    void updateContentsPosition(float dt) ;
    ClippingNode *_clipper ;
    
    int calcPrevContentsIndex(int center_index) ; //現在の中心のカードから見て左側のコンテンツのインデックスを取得する
    int calcNextContentsIndex(int center_index) ; //現在の中心のカードから見て右側のコンテンツのインデックスを取得する
    
};

#endif /* defined(__War__BannerContentsSlideSprite__) */
