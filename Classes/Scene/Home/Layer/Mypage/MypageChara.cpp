#include "MypageChara.h"

MypageChara::MypageChara()
:_primaryId(0)
{
}

MypageChara* MypageChara::create(const string filename)
{
    MypageChara *pRet = new MypageChara();
    
    if (pRet && pRet->init(filename)) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool MypageChara::init(string filename)
{
    if(!Sprite::initWithFile(filename))
    {
        return false;
    }
    
    return true;
}

void MypageChara::onEnter()
{
    Sprite::onEnter();
}

void MypageChara::setParentLayer()
{
    this->parent_layer = (Layer *)this->getParent();
}
