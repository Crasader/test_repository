#include "OptionsLayer.h"

bool OptionsLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("OptionsLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void OptionsLayer::onEnter()
{
    Layer::onEnter();
    
    setupImage();
}

void OptionsLayer::setupImage(){
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("オプション", "SubMenu", this);

}

void OptionsLayer::changePage(Ref* sender){
    MenuItem* menuItem = (MenuItem *)sender;
    int tag_no = menuItem->getTag();
    
    if(tag_no == SOUND_TAG){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("Sound");
        HMSLOG("touch Sound!");
    }else
    if(tag_no == NICKNAME_TAG){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("Nickname");
        HMSLOG("touch Nickname!");
    }else
    if(tag_no == BACK_BUTTON_TAG){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("SubMenu");
        HMSLOG("touch back!");
    }
}