#include "HmsActionInstant.h"


NS_CC_BEGIN

//
// ChangeTexture
//

ChangeTexture *ChangeTexture::create(const std::string& filename)
{
    ChangeTexture *ret = new (std::nothrow) ChangeTexture();

    if (ret && ret->initWithChangeTexture(filename)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ChangeTexture::initWithChangeTexture(const std::string& filename) {
    _filename = filename;
    return true;
}

void ChangeTexture::update(float time) {
    CC_UNUSED_PARAM(time);
    static_cast<Sprite*>(_target)->setTexture(_filename) ;
}


ChangeTexture * ChangeTexture::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) ChangeTexture();
    a->initWithChangeTexture(_filename);
    a->autorelease();
    return a;
}

NS_CC_END
