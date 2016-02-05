#include "CharaCollectivelySellConfirmPopupLayer.h"
#include "CharaSprite.h"
#include "CharaSellListLayer.h"

CharaCollectivelySellConfirmPopupLayer* CharaCollectivelySellConfirmPopupLayer::create(HmsVector<CharaSprite *> &sell_chara_list, int sell_price)
{
    CharaCollectivelySellConfirmPopupLayer *sprite = new CharaCollectivelySellConfirmPopupLayer();
    
    if (sprite && sprite->init(sell_chara_list, sell_price))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void CharaCollectivelySellConfirmPopupLayer::showSellChara(){
    createScrollView();
    setScrollViewData();
    AllSellComment();
}

void CharaCollectivelySellConfirmPopupLayer::setNoAction(){
    noAction = [this](Ref* sender){
        //閉じる処理
        CharaSellListLayer* charaSellListLayer = (CharaSellListLayer*)this->getParent();
        charaSellListLayer->clearSellList();
        Utility::closePopupAction(this);
    };
    
}

void CharaCollectivelySellConfirmPopupLayer::createScrollView()
{
    //スクロールビューの作成
    auto pScrollView = ScrollView::create();
    pScrollView->setViewSize(Size(516 + (getVisibleSpaceWidth() * 2), 380));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    _bg->addChild(pScrollView, Z_ScrollView, T_ScrollView);
    
    pScrollView->setAnchorPoint(Point(0.5, 0));
    pScrollView->setPosition(30, 220);
    //スクロールの中身を作成
    pScrollView->setContainer(createCharaList());
    
    //バーを表示
    bar = ScrollBarLayer::create(pScrollView);
}


void CharaCollectivelySellConfirmPopupLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    ScrollView* scrollView = (ScrollView *)_bg->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scrollView->setContentSize(Size(scrollView->getContentSize().width, scrollView->getContentSize().height));
    
    scrollView->setContentOffset(Vec2(0, scrollView->getViewSize().height - scrollView->getContentSize().height));
    
    //バーを表示
    bar->refresh();
}

Layer* CharaCollectivelySellConfirmPopupLayer::createCharaList(){
    //売却キャラの表示
    Layer* charaListLayer = Layer::create();
    charaListLayer->setTag(T_ScrollLayer);
    charaListLayer->setContentSize(Size(charaListLayer->getContentSize().height, (POS_Y_SELL_DISTANCE * ((_chara.size()-1)/5))+100));
    //キャラの表示
    int num = 0;
    for (int i = 0; i < _chara.size(); i++)
    {
        for (int j = 0; j < ROW_1_CHARA_COUNT; j++)
        {
            if (num == _chara.size()) break;
            _chara[num]->setScale(0.8f); //枠が小さい為、少し小さく表示する
            _chara[num]->setPosition(POS_X_SELL_FIRST + POS_X_SELL_DISTANCE * j, (POS_Y_SELL_DISTANCE * ((_chara.size()-1)/5))+50 - POS_Y_SELL_DISTANCE * i);
            charaListLayer->addChild(_chara[num]);
            
            num++;
        }
    }
    
    return charaListLayer;
}

void CharaCollectivelySellConfirmPopupLayer::AllSellComment()
{
    //一括売却の場合「NとRを一括売却します」って表示する
    auto warning_label = HMSFontUtil::createWithTTFBySystemFontArgument("NとRのキャラを一括売却します\n※デッキに編成されているキャラとお気に入り\n登録されているキャラは対象外です", FONT_NAME_SYSTEM, 20);
    warning_label->setPosition(Point(_bg->getContentSize().width / 2, 174));
    _bg->addChild(warning_label);
    
    auto tint1 = TintTo::create(0.5f, 255, 130, 130);
    auto tint2 = TintTo::create(0.5f, 255, 255, 255);
    warning_label->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.5f), tint1, tint2, nullptr)));
}

void CharaCollectivelySellConfirmPopupLayer::scrollViewDidScroll(ScrollView *view)
{
    //バーを表示
    bar->refresh();
}
