#include "HelpLayer.h"

bool HelpLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("HelpLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void HelpLayer::onEnter()
{
    Layer::onEnter();
    
    setupImage();
}

void HelpLayer::setupImage(){
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("ヘルプ", "SubMenu", this);
    
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("ヘルプ", FONT_NAME, 40);
    label->setScale(ratio);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 1));
    this->addChild(label, 10);
    
}

void HelpLayer::changePage(Ref* sender){
    
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("SubMenu");
    HMSLOG("touch back!");
}