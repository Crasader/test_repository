#include "SoundLayer.h"

bool SoundLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("SoundLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void SoundLayer::onEnter()
{
    Layer::onEnter();
    
    setupImage();
}

void SoundLayer::setupImage(){
    
    FileUtils::getInstance()->addSearchPath("extensions");
    
    //エフェクト情報取得
    bool effect_bool = SaveData::loadBool("effect");
    auto chageEffectLayer = LayerColor::create(Color4B(0, 0, 0, 0));
    if(effect_bool){
        chageEffectLayer->setPosition(convertFromDisplaySize(Point(0,0), 0, 0));
    }else{
        chageEffectLayer->setPosition(convertFromDisplaySize(Point(-1000,0), 0, 0));
    }
    this->addChild(chageEffectLayer,25,CHANGE_EFFECT_TAG);
    
    //音量取得
    float bgm_volume = SaveData::loadFloat("bgm_volume");
    bgm_volume = bgm_volume;
    float effect_volume = SaveData::loadFloat("effect_volume");
    effect_volume = effect_volume;

    auto setsp = Sprite::create("init/Home/SubMenu/popup_setting.png");
    setsp->setTag(1);
    setsp->setScale(ratio);
    setsp->setAnchorPoint(Point(0.5,0.5));
    setsp->setPosition(convertFromDisplaySize(Point(320, 427), 0.5, 1));
    this->addChild(setsp,10,1);
    
    auto setsp2 = Sprite::create("init/Home/SubMenu/popup_setting_g.png");
    setsp2->setTag(1);
    setsp2->setScale(ratio);
    setsp2->setAnchorPoint(Point(0.5,0.5));
    setsp2->setPosition(convertFromDisplaySize(Point(320, 427), 0.5, 1));
    this->addChild(setsp2,8,1);
    
    ControlSlider* pSlider = ControlSlider::create("init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_volume_circle.png");
    pSlider->setMinimumValue(0.f);          // スライダー最小値
    pSlider->setMaximumValue(1.f);          // スライダー最大値
    pSlider->setMinimumAllowedValue(0.0f);  // スライダー最小移動値
    pSlider->setMaximumAllowedValue(1.0f);  // スライダー最大移動値
    pSlider->setValue(bgm_volume);                // スライダーの初期値
    pSlider->setTag(SOUND_SLIDER);
    pSlider->setScale(ratio);
    pSlider->setPosition(convertFromDisplaySize(Point(370, 530), 0.5, 1));
    pSlider->addTargetWithActionForControlEvents(this, cccontrol_selector(SoundLayer::changeBgmVolume), Control::EventType::VALUE_CHANGED);
    this->addChild(pSlider,SOUND_SLIDER);
    
    ControlSlider* effectSlider = ControlSlider::create( "init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_volume_circle.png");
    effectSlider->setMinimumValue(0.f);          // スライダー最小値
    effectSlider->setMaximumValue(1.f);          // スライダー最大値
    effectSlider->setMinimumAllowedValue(0.0f);  // スライダー最小移動値
    effectSlider->setMaximumAllowedValue(1.0f);  // スライダー最大移動値
    effectSlider->setValue(effect_volume);                // スライダーの初期値
    effectSlider->setTag(EFFECT_SLIDER);
    effectSlider->setScale(ratio);
    effectSlider->setPosition(convertFromDisplaySize(Point(370, 443), 0.5, 1));
    effectSlider->addTargetWithActionForControlEvents(this, cccontrol_selector(SoundLayer::changeSeVolume), Control::EventType::VALUE_CHANGED);
    this->addChild(effectSlider,EFFECT_SLIDER);
    
    auto setOn = MenuItemImage::create("init/Home/SubMenu/bt_on.png", "init/Home/SubMenu/bt_on.png", CC_CALLBACK_1(SoundLayer::changeEffect, this));
    setOn->setScale(ratio);
    setOn->setTag(EFFECT_ON);
    auto setOn1 = Menu::create(setOn, nullptr);
    setOn1->setPosition(convertFromDisplaySize(Point(320, 250), 0.5, 1));
    chageEffectLayer->addChild(setOn1, 14, EFFECT_ON);
    
    auto setoff_g = MenuItemImage::create("init/Home/SubMenu/bt_off_gray.png", "init/Home/SubMenu/bt_off_gray.png", CC_CALLBACK_1(SoundLayer::changeEffect, this));
    setoff_g->setScale(ratio);
    setoff_g->setTag(EFFECT_OFF_GRAY);
    auto setoff_g1 = Menu::create(setoff_g, nullptr);
    setoff_g1->setPosition(convertFromDisplaySize(Point(1320, 250), 0.5, 1));
    chageEffectLayer->addChild(setoff_g1, 14, EFFECT_OFF_GRAY);
    
    auto setdef = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png", CC_CALLBACK_1(SoundLayer::changeVolume, this));
    setdef->setScale(ratio);
    setdef->setTag(SOUND_DEFAULT);
    auto setdef1 = Menu::create(setdef, nullptr);
    setdef1->setPosition(convertFromDisplaySize(Point(220, 130), 0.5, 1));
    this->addChild(setdef1, 15, SOUND_DEFAULT);
    
    auto def_lb = HMSFontUtil::createWithTTFBySystemFontArgument("デフォルト" , FONT_NAME_SYSTEM, 28);
    def_lb->setScale(ratio);
    def_lb->setPosition(convertFromDisplaySize(Point(220, 130),0.5,1));
    this->addChild(def_lb, 17);
    
    auto setgo = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png", CC_CALLBACK_1(SoundLayer::changeVolume, this));
    setgo->setScale(ratio);
    setgo->setTag(SOUND_SET);
    auto setgo1 = Menu::create(setgo, nullptr);
    setgo1->setPosition(convertFromDisplaySize(Point(420, 130), 0.5, 1));
    this->addChild(setgo1, 16, SOUND_SET);
    
    auto set_lb = HMSFontUtil::createWithTTFBySystemFontArgument("保存" , FONT_NAME_SYSTEM, 28);
    set_lb->setScale(ratio);
    set_lb->setPosition(convertFromDisplaySize(Point(420, 130),0.5,1));
    this->addChild(set_lb, 17);
    
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("オプション", "Other", this);

    //BGM
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    //エフェクト音
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(SoundLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(SoundLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(SoundLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void SoundLayer::changeVolume(Ref* sender){
    MenuItem* menuItem = (MenuItem *)sender;
    int tag_no = menuItem->getTag();
    
    if(tag_no == SOUND_DEFAULT){//初期化
        //スライダーの初期化
        ControlSlider* bgm = (ControlSlider*)this->getChildByTag(SOUND_SLIDER);
        bgm->setValue(0.5f);
        ControlSlider* effect = (ControlSlider*)this->getChildByTag(EFFECT_SLIDER);
        effect->setValue(0.5f);
        //音量の初期化
        CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5f);
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0.5f);
        //音量の保存(float保存)
        SaveData::saveFloat("bgm_volume", 0.5f);
        SaveData::saveFloat("effect_volume", 0.5f);
    }else if(tag_no == SOUND_SET){//調整
        //スライダーから音量の取得
        ControlSlider* bgm = (ControlSlider*)this->getChildByTag(SOUND_SLIDER);
        float bgm_volume = bgm->getValue();
        ControlSlider* effect = (ControlSlider*)this->getChildByTag(EFFECT_SLIDER);
        float effect_volume = effect->getValue();
        
        //取得した音量の適用
        CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
        //エフェクト音(１回)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/push.mp3",false);
        //音量の保存(float保存)
        SaveData::saveFloat("bgm_volume", bgm_volume);
        SaveData::saveFloat("effect_volume", effect_volume);
        
        auto popup_layer = ProfilePopup::create("Other","Other");
        popup_layer->setupImage("音量設定を保存しました","Other");
        this->addChild(popup_layer, 1000);
    }
}
void SoundLayer::changeBgmVolume(Ref* sender, Control::EventType controlEvent)
{
    //取得した音量（BGM）の適用
    ControlSlider* bgm = (ControlSlider*)sender;
    float bgm_volume = bgm->getValue();
    
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SaveData::saveFloat("bgm_volume", bgm_volume);
    HMSLOG("bgm_volume = %f", bgm_volume);
}
void SoundLayer::changeSeVolume(Ref* sender, Control::EventType controlEvent)
{
    //取得した音量（effect）の適用
    ControlSlider* effect = (ControlSlider*)sender;
    float effect_volume = effect->getValue();

    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
    SaveData::saveFloat("effect_volume", effect_volume);
    HMSLOG("effect_volume = %f", effect_volume);
}

void SoundLayer::changeEffect(Ref* sender){
    MenuItem* menuItem = (MenuItem *)sender;
    int tag_no = menuItem->getTag();

    Sprite* chageEffectLayer = (Sprite*)this->getChildByTag(CHANGE_EFFECT_TAG);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/switch.mp3",false);
    if(tag_no == EFFECT_ON){//エフェクトOFF
        auto move = MoveBy::create(0, convertFromDisplaySize(Point(-1000, 0), 0, 0));
        chageEffectLayer->runAction(move);
        SaveData::saveBool("effect", false);
    }else if (tag_no == EFFECT_OFF_GRAY){//エフェクトON
        auto move = MoveBy::create(0, convertFromDisplaySize(Point(1000, 0), 0, 0));
        chageEffectLayer->runAction(move);
        SaveData::saveBool("effect", true);
    }
}

void SoundLayer::changePage(Ref* sender)
{
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("Other");
}

bool SoundLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){
    
    CCLOG("tourch");
    return true;
}

void SoundLayer::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
    
}

void SoundLayer::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void valueChanged(Node *pSender,Control::EventType pControlEvent)
{
    
}