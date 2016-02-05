#include "HexLayer.h"
#include "HmsGeometricFunction.h"

bool HexLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    return true;
}
void HexLayer::initialize(int hex_no, bool camp_flg) {
    ratio = getVisibleRatio();
    
    _hex_no = hex_no;
    _camp_flg = camp_flg;
    
    //スクウェア表示
    Sprite *img = Sprite::create("init/Map/square.png");
    img->setScale(ratio);
    this->addChild(img);
    
    //フィールドNo表示
    Label *label = Label::createWithSystemFont(to_string(hex_no).c_str(), FONT_NAME_SYSTEM, 20);
    this->addChild(label);
}
