#ifndef __war__CharaCollectivelySellConfirmPopupLayer__
#define __war__CharaCollectivelySellConfirmPopupLayer__

#include "CharaSellConfirmPopupBase.h"
#include "ScrollBarLayer.h"
#include "extensions/cocos-ext.h"

USING_NS_CC_EXT;

class CharaCollectivelySellConfirmPopupLayer : public CharaSellConfirmPopupBase, ScrollViewDelegate
{
public:
    static CharaCollectivelySellConfirmPopupLayer* create(HmsVector<CharaSprite *> &sell_chara_list, int sell_price);
    
private:
    ScrollBarLayer *bar;
    
    Layer* createCharaList();
    void createScrollView(); //スクロールビューの作成
    void setScrollViewData();
    
    //override
    void showSellChara();
    void setNoAction();
    void AllSellComment();
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
};

#endif
