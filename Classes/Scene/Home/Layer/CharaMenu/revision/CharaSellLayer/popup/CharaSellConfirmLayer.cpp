#include "CharaSellConfirmLayer.h"
#include "CharaSprite.h"

CharaSellConfirmLayer* CharaSellConfirmLayer::create(HmsVector<CharaSprite *> &sell_chara_list, int sell_price)
{
    CharaSellConfirmLayer *sprite = new CharaSellConfirmLayer();
    
    if (sprite && sprite->init(sell_chara_list, sell_price))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void CharaSellConfirmLayer::showSellChara()
{
    //売却キャラの表示
    
    //背景スクエア
    auto chara_bg = Sprite::create("init/Home/Quest/popup_battle_deck_square2.png");
    chara_bg->setAnchorPoint(Point(0.5, 1.0));
    chara_bg->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height - 75));
    _bg->addChild(chara_bg, Z_Chara_Bg, T_Chara_Bg);
    
    
    //キャラの表示
    int num = 0;
    for (int i = 0; i < _chara.size(); i++)
    {
        for (int j = 0; j < ROW_1_CHARA_COUNT; j++)
        {
            if (num == _chara.size()) break;
            _chara[num]->setScale(0.8f); //枠が小さい為、少し小さく表示する
            _chara[num]->setPosition(calcPositionChara(i, j));
            chara_bg->addChild(_chara[num]);
            
            num++;
        }
    }
}

void CharaSellConfirmLayer::setNoAction(){
    noAction = [this](Ref* sender){
        //閉じる処理
        Utility::closePopupAction(this);
    };
    
}

