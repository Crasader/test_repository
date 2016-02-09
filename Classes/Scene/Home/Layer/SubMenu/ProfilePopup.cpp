#include "ProfilePopup.h"

ProfilePopup* ProfilePopup::create(string return_page_name,string page)
{
    ProfilePopup *node = new ProfilePopup();
    if (node && node->init(return_page_name,page))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool ProfilePopup::init(string return_page_name,string page)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ProfilePopup::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //メンバ変数に保存
    _return_page_name = return_page_name;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ProfilePopup::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ProfilePopup::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ProfilePopup::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ProfilePopup::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    
    if(_return_page_name==profilehikaku){
        //3秒たったら消す
        this->scheduleOnce(schedule_selector(ProfilePopup::deleteAction), 3.0f);
        HMSLOG("profilePopup1!");
    }else if(_return_page_name==submenuhikaku){
        this->scheduleOnce(schedule_selector(ProfilePopup::deleteAction2), 3.0f);
        HMSLOG("profilePopup2!");
    }else if(_return_page_name==otherhikaku){
        this->scheduleOnce(schedule_selector(ProfilePopup::deleteAction3), 3.0f);
        HMSLOG("profilePopup3!");
    }else if(_return_page_name==shophikaku){
        this->scheduleOnce(schedule_selector(ProfilePopup::deleteAction4), 3.0f);
        HMSLOG("profilePopup4!");
    }
    return true;
}

void ProfilePopup::setupImage(string text,string page)
{
    change_page = page;
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_200_s.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 2 ,ACTION_SPRITE_TAG);
    
    //テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24);
    label->setScale(0);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(label, 3, ACTION_SPRITE_TAG);
    
    //ポップアップアクション
    popupAction();
}

void ProfilePopup::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}
//プロフィール遷移用デリートアクション
void ProfilePopup::deleteAction(float dt)
{
    auto normal_scene = (NormalScene *)this->getParent()->getParent();
    
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == ACTION_SPRITE_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    auto func2 = CallFunc::create([normal_scene](){
        normal_scene->changePage("Profile");
    });
    runAction(Sequence::create(DelayTime::create(0.2f), func, func2, NULL));
}

void ProfilePopup::deleteAction2(float dt)
{
    auto normal_scene = (NormalScene *)this->getParent()->getParent();
    
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == ACTION_SPRITE_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    auto func2 = CallFunc::create([normal_scene](){
        normal_scene->changePage("SubMenu");
    });
    runAction(Sequence::create(DelayTime::create(0.2f), func, func2, NULL));
}

void ProfilePopup::deleteAction3(float dt)
{
    auto normal_scene = (NormalScene *)this->getParent()->getParent();
    
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == ACTION_SPRITE_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    auto func2 = CallFunc::create([normal_scene](){
        normal_scene->changePage("Other");
    });
    runAction(Sequence::create(DelayTime::create(0.2f), func, func2, NULL));
}

void ProfilePopup::deleteAction4(float dt)
{
    auto normal_scene = (NormalScene *)this->getParent()->getParent();
    
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == ACTION_SPRITE_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    auto func2 = CallFunc::create([normal_scene](){
        normal_scene->changePage("ShopMenu");
    });
    runAction(Sequence::create(DelayTime::create(0.2f), func, func2, NULL));
}

bool ProfilePopup::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void ProfilePopup::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ProfilePopup::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_return_page_name==profilehikaku){
        this->unschedule(schedule_selector(ProfilePopup::deleteAction));
        
        this->scheduleOnce(schedule_selector(ProfilePopup::deleteAction), 0);
    }else if(_return_page_name==submenuhikaku){
        this->unschedule(schedule_selector(ProfilePopup::deleteAction2));
        
        this->scheduleOnce(schedule_selector(ProfilePopup::deleteAction2), 0);
    }
}
void ProfilePopup::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
