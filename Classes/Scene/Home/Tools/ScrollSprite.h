#ifndef __war__ScrollSprite__
#define __war__ScrollSprite__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "common.h"
#include "HmsVector.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ScrollSprite : public Sprite, public ScrollViewDelegate
{
private:
    enum Tag{
       T_SCROLL_VIEW = 0,
    };
    
    enum Order{
       O_SCROLL_VIEW = 0,
    };
    
    
    Size _scroll_view_size ;
    Size _inner_container_size;
    
    Node* _container ;
    
    void stretchContents(HmsVector<Sprite *> &item_list); //要素に追加するスプライトを全てスクロールの垂直方向のvnew_sizeの横幅にフィットさせる
    Size calcInnerContainerSize(HmsVector<Sprite *> &item_list); //内容物のサイズを計算
    void addContents(HmsVector<Sprite *> &item_list); //内容物の追加
    void createScrollView();
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view);
    
   
    ScrollView *_scroll_view;
    HmsVector<Sprite *> _item_list  ;
public:
    enum ScrollDirection{
        VERTICAL,
        HORIZONTAL
    };
    ScrollDirection _scroll_direction ;
    
    ScrollSprite();
    
    //stretch_mode: view_sizeに合わせて要素(item_list)のスプライトを伸縮させる
    static ScrollSprite* create(HmsVector<Sprite *> &item_list, const Size& view_size, ScrollDirection scroll_direction, bool stretch_mode=false);
    virtual bool init(HmsVector<Sprite *> &item_list, const Size& view_size, ScrollDirection scroll_direction, bool stretch_mode);
    
    void contentOffsetChangeFromOffsetDistance(float distance) ;
    
    void onEnter(); //初期実行関数
    
    
};


#endif /* defined(__war__ScrollSprite__) */
