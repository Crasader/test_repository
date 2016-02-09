#ifndef __war__CharaSellConfirmLayer__
#define __war__CharaSellConfirmLayer__

#include "CharaSellConfirmPopupBase.h"

class CharaSellConfirmLayer : public CharaSellConfirmPopupBase
{
public:
    static CharaSellConfirmLayer* create(HmsVector<CharaSprite *> &sell_chara_list, int sell_price);
    
private:
    //override
    void showSellChara();
    void setNoAction();
};

#endif /* defined(__war__CharaSellConfirmLayer__) */
