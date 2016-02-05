#include "SupportInfoLayer.h"
#include "HmsGeometricFunction.h"
#include "DungeonScene.h"


bool SupportInfoLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    HMSLOG("SupportInfoLayer::init()");
	return true;
}

void SupportInfoLayer::setBossEventSupportInfo(HmsVector<string> support_user_name_list)
{
    _support_user_name_list = support_user_name_list;
    
    Sprite *img = (Sprite *)this->getChildByTag(SUPPORT_IMG_TAG);
    if(_support_user_name_list.size()==0) {
        if(img!=nullptr) {
            this->removeChildByTag(SUPPORT_IMG_TAG);
            this->removeChildByTag(SUPPORT_BK_IMG_TAG);
            this->removeChildByTag(SUPPORT_LABEL1_TAG);
            this->removeChildByTag(SUPPORT_LABEL2_TAG);
            
            this->unschedule(schedule_selector(SupportInfoLayer::updateBossEventSupportInfo));
        }
    } else {
        if(img==nullptr) {
            img = Sprite::create("init/Map/bt_support.png");
            img->setScale(ratio);
            img->setAnchorPoint(Point(0.5, 0.5));
            img->setPosition(convertFromDisplaySize(Point(55, 902), 0.5, 0.5));
            this->addChild(img, SUPPORT_Z, SUPPORT_IMG_TAG);
            
            updateBossEventSupportInfo(0.0f);
            this->schedule(schedule_selector(SupportInfoLayer::updateBossEventSupportInfo), 3.0f);
        
            RotateTo *rotate1 = RotateTo::create(0.25f, -15.0f);
            RotateTo *rotate2 = RotateTo::create(0.25f, 15.0f);
            RepeatForever *action = RepeatForever::create(Sequence::create(rotate1, rotate2, nullptr));
            img->runAction(action);
        }
        
            /*
             Sprite *bk_img = Sprite::create("init/Map/balloon_support.png");
             bk_img->setScale(ratio);
             bk_img->setAnchorPoint(Point(0, 0.5));
             bk_img->setPosition(convertFromDisplaySize(Point(83+320, 484-7), 0.5, 0.5));
             this->addChild(bk_img, z, SUPPORT_BK_IMG_TAG);
             */
        
        this->removeChildByTag(SUPPORT_LABEL1_TAG);
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(toString(_support_user_name_list.size()) + "人の仲間が応援中♪ 仲間の力でダメージアップ!!", FONT_NAME_SYSTEM, 20*ratio);
        label->enableOutline(Color4B(0, 0, 0, 255), 3);
        label->setColor(Color3B(255, 255, 0));
        label->setAnchorPoint(Point(0, 0.5));
        label->setPosition(convertFromDisplaySize(Point(104, 926), 0.5, 0.5));
        this->addChild(label, SUPPORT_Z, SUPPORT_LABEL1_TAG);
    }
}

void SupportInfoLayer::updateBossEventSupportInfo(float dt)
{
    if(_support_user_name_list.size()==0) return;
    if(_support_user_name_list_index>_support_user_name_list.size()) _support_user_name_list_index = 0;
    
    Sprite *bk_img = Sprite::create("init/Map/balloon_support.png");
    bk_img->setScale(ratio*0.15);
    bk_img->setAnchorPoint(Point(0, 0.5));
    bk_img->setPosition(convertFromDisplaySize(Point(83, 895), 0.5, 0.5));
    this->addChild(bk_img, SUPPORT_Z, SUPPORT_BK_IMG_TAG);
    
    //auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_support_user_name_list.at(_support_user_name_list_index) + "が応援中", FONT_NAME_SYSTEM, 20*ratio);
    auto label = Label::createWithSystemFont(_support_user_name_list.at(_support_user_name_list_index) + "が応援中", FONT_NAME_SYSTEM, 20);
    label->setColor(Color3B(0, 0, 0));
    label->setAnchorPoint(Point(0, 0.5));
    label->setPosition(Point(21, 21));
    bk_img->addChild(label);
    
    ScaleTo *scale1 = ScaleTo::create(0.4f, ratio*1.0f);
    ScaleTo *scale2 = ScaleTo::create(0.05f, ratio*0.97f);
    ScaleTo *scale3 = ScaleTo::create(0.05f, ratio*1.0f);
    DelayTime *delay = DelayTime::create(2.5);
    RemoveSelf *remove = RemoveSelf::create();
    Action *action = Sequence::create(scale1, scale2, scale3, delay, remove, nullptr);
    bk_img->runAction(action);
    
    _support_user_name_list_index++;
}
