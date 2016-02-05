#include "DungeonSubMenuOption.h"

bool DungeonSubMenuOptionLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("DungeonSubMenuOptionLayer::()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    //画面の半分のサイズの取得
    halfWidth = DISPLAY_WIDTH / 2 ;
    halfHeight = DISPLAY_HEIGHT / 2 ;
    
    //音量取得
    _bgm_volume = SaveData::loadFloat("bgm_volume");
    _effect_volume = SaveData::loadFloat("effect_volume");
    _get_bgm_volume = _bgm_volume;
    _get_effect_volume = _effect_volume;
    
    //エフェクト情報の取得
    _effectFlg = SaveData::loadBool("effect");
    
    HMSLOG("BGM:%f", _bgm_volume);
    HMSLOG("BGM:%f", _effect_volume);
    
    return true;
}

void DungeonSubMenuOptionLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    createEventListener();
    
    //背景画像の作成
    createBg();
    
    //ラベルの作成
    createLabel();
    
    //ボタンの作成
    createButton();
    
    //スライダーの作成
    createSlider();
    
    //ポップアップアクション
    popupAction();
}

void DungeonSubMenuOptionLayer::createEventListener()
{
    //イベントリスナーの作成
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(DungeonSubMenuOptionLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DungeonSubMenuOptionLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DungeonSubMenuOptionLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DungeonSubMenuOptionLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(halfWidth, halfHeight), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
//    bg = Sprite::create("init/Home/Popup/popup_600.png");
    bg = Sprite::create();
    bg->setContentSize(Size(570, 600));
    bg->setPosition(convertFromDisplaySize(Point(halfWidth, halfHeight), 0.5, 0.5));
    bg->setScale(0);
    this->addChild(bg, 2, POPUP_ACTION_TAG);
    
    auto setsp = Sprite::create("init/Home/SubMenu/popup_setting.png");
    setsp->setTag(1);
    setsp->setAnchorPoint(Point(0.5,0.5));
    setsp->setPosition(Point(285, 350));
    bg->addChild(setsp, 2);
    
    //ゲージ画像
    auto setsp2 = Sprite::create("init/Home/SubMenu/popup_setting_g.png");
    setsp2->setTag(1);
    setsp2->setAnchorPoint(Point(0.5,0.5));
    setsp2->setPosition(Point(285, 350));
    bg->addChild(setsp2, 1);
}

void DungeonSubMenuOptionLayer::createLabel()
{
    //サウンド設定メッセージ
    string soundMessage = "BGM・SEなどのサウンドの音量を調整することができます";
    auto soundTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument(soundMessage, FONT_NAME_SYSTEM, 19);
    soundTextLabel->setAnchorPoint(Point(0, 0));
    soundTextLabel->setPosition(Point(20, bg->getContentSize().height - 90));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    bg->addChild(soundTextLabel, 3);
    
    string effectMessage = "エフェクトの表示の有無を選択することができます";
    auto effectTextLabel = HMSFontUtil::createWithTTFBySystemFontArgument(effectMessage, FONT_NAME_SYSTEM, 19);
    effectTextLabel->setAnchorPoint(Point(0, 0));
    effectTextLabel->setPosition(Point(20, bg->getContentSize().height/2 - 80));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    bg->addChild(effectTextLabel, 3);

}

//    confirmTextLabel->setHorizontalAlignment(TextHAlignment::CENTER);
//    confirmTextLabel->setPosition(Point(bg->getContentSize().width/2, bg->getContentSize().height/2 + 50));
//enableOutline(Color4B(0, 0, 0, 255), 1);


void DungeonSubMenuOptionLayer::createSlider()
{
    //スライダーの作成
    ControlSlider* bgmSlider = ControlSlider::create( "init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_volume_circle.png");
    bgmSlider->setMinimumValue(0.f); // スライダー最小値
    bgmSlider->setMaximumValue(1.f); // スライダー最大値
    bgmSlider->setMinimumAllowedValue(0.0f); // スライダー最小移動値
    bgmSlider->setMaximumAllowedValue(1.0f); // スライダー最大移動値
    bgmSlider->setValue(_bgm_volume); // スライダーの初期値
    bgmSlider->setTag(SOUND_SLIDER);
    bgmSlider->setPosition(Point(335, bg->getContentSize().height * 0.76));
    bgmSlider->addTargetWithActionForControlEvents(this, cccontrol_selector(DungeonSubMenuOptionLayer::changeBgmVolume), Control::EventType::VALUE_CHANGED);
    bg->addChild(bgmSlider,SOUND_SLIDER);
    
    ControlSlider* seSlider = ControlSlider::create( "init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_gauge.png","init/Home/SubMenu/setup_volume_circle.png");;
    seSlider->setMinimumValue(0.f);          // スライダー最小値
    seSlider->setMaximumValue(1.f);          // スライダー最大値
    seSlider->setMinimumAllowedValue(0.0f);  // スライダー最小移動値
    seSlider->setMaximumAllowedValue(1.0f);  // スライダー最大移動値
    seSlider->setValue(_effect_volume);                // スライダーの初期値
    seSlider->setTag(EFFECT_SLIDER);
    seSlider->setPosition(Point(335, bg->getContentSize().height * 0.61));
    seSlider->addTargetWithActionForControlEvents(this, cccontrol_selector(DungeonSubMenuOptionLayer::changeSeVolume), Control::EventType::VALUE_CHANGED);
    bg->addChild(seSlider,EFFECT_SLIDER);
}

void DungeonSubMenuOptionLayer::createButton()
{
    //エフェクト機能のボタン
    auto btn_on_select = Sprite::create("init/Home/SubMenu/bt_on.png");
    auto btn_on_unselect  = Sprite::create("init/Home/SubMenu/bt_off_gray.png");
    
    _effect_btn_on = MenuItemSprite::create(btn_on_select, btn_on_unselect, CC_CALLBACK_1(DungeonSubMenuOptionLayer::changeEffect, this));
    _effect_btn_on->setTag(EFFECT_ON);
    _effect_btn_on->setScale(1.2);
    
//    auto btn_off_select = Sprite::create("init/Home/SubMenu/bt_off.png");
//    auto btn_off_unselect  = Sprite::create("init/Home/SubMenu/bt_off_gray.png");
//    
//    _effect_btn_off = MenuItemSprite::create(btn_off_select, btn_off_unselect, CC_CALLBACK_1(DungeonSubMenuOptionLayer::changeEffect, this));
//    _effect_btn_off->setTag(EFFECT_OFF);
//    _effect_btn_off->setScale(1.2);
    
    Menu* menu = Menu::create(_effect_btn_on, nullptr);
    menu->alignItemsHorizontallyWithPadding(30);
    menu->setPosition(Point(bg->getContentSize().width / 2, 180));
    bg->addChild(menu, 3, EFFECT_BUTTON_TAG);
    
    
    //保存ボタン
    auto save_btn_off = Sprite::create("init/Home/Common/bt1.png");
    auto save_btn_on  = Sprite::create("init/Home/Common/bt1.png");
    save_btn_on->setColor(Color3B(200, 200, 200));
    
    auto save_label1 = HMSFontUtil::createWithTTFBySystemFontArgument("保存", FONT_NAME_SYSTEM, 40);
    save_label1->setPosition(save_btn_off->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    save_btn_off->addChild(save_label1);
    
    auto save_label2 = HMSFontUtil::createWithTTFBySystemFontArgument("保存", FONT_NAME_SYSTEM, 40);
    save_label2->setPosition(save_btn_on->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    save_btn_on->addChild(save_label2);
    
    auto save_btn = MenuItemSprite::create(save_btn_off, save_btn_on, CC_CALLBACK_1(DungeonSubMenuOptionLayer::saveSetting, this));
    save_btn->setAnchorPoint(Point(0.5, 0));
    
    
    //デフォルトボタン
    auto defalut_btn_off = Sprite::create("init/Home/Common/bt1.png");
    auto defalut_btn_on  = Sprite::create("init/Home/Common/bt1.png");
    defalut_btn_on->setColor(Color3B(180, 180, 180));
    
    auto defalut_label1 = HMSFontUtil::createWithTTFBySystemFontArgument("デフォルト", FONT_NAME_SYSTEM, 30);
    defalut_label1->setPosition(save_btn_off->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    defalut_btn_off->addChild(defalut_label1);
    
    auto defalut_label2 = HMSFontUtil::createWithTTFBySystemFontArgument("デフォルト", FONT_NAME_SYSTEM, 30);
    defalut_label2->setPosition(save_btn_on->getContentSize() / 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    defalut_btn_on->addChild(defalut_label2);
    
    auto default_btn = MenuItemSprite::create(defalut_btn_off, defalut_btn_on, CC_CALLBACK_1(DungeonSubMenuOptionLayer::loadSetting, this));
    default_btn->setAnchorPoint(Point(0.5, 0));
    
    Menu* menu2 = Menu::create(default_btn, save_btn, nullptr);
    menu2->alignItemsHorizontallyWithPadding(30);
    menu2->setPosition(Point(bg->getContentSize().width / 2, 8));
    bg->addChild(menu2, 3);
    

    //モーダルレイヤーを消すボタン
    auto back = MenuItemImage::create("init/Home/Popup/bt_close_L.png","init/Home/Popup/bt_close_L.png", CC_CALLBACK_0(DungeonSubMenuOptionLayer::deleteAction, this));
    auto Menu1 = Menu::create(back, nullptr);
    Menu1->setPosition(Point(bg->getContentSize().width - 10, bg->getContentSize().height - 10));
    bg->addChild(Menu1, 3);
    
    //ON、OFFボタンの切り替え
    changeButton();
    
}

void DungeonSubMenuOptionLayer::changeButton()
{

    if (_effectFlg)
    {
//        //ボタンonを押した状態にする
//        _effect_btn_off->setEnabled(true);
//        _effect_btn_off->unselected();
        
//        _effect_btn_on->setEnabled(false);
        _effect_btn_on->unselected();

        _effectFlg = false;
    }
    else
    {
//        //ボタンoffを押した状態にする
//        _effect_btn_off->setEnabled(false);
//        _effect_btn_off->selected();
        
//        _effect_btn_on->setEnabled(true);
        _effect_btn_on->selected();
        
        _effectFlg = true;
    }
    if(first_flg){
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3",false);
        first_flg=false;
    }else{
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/switch.mp3",false);
    }
}

void DungeonSubMenuOptionLayer::popupAction()
{
    //窓を出現させる
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->addTapDisableLayer();
    
    //自身の子からPOPUP_ELEMENTの要素をすべて拡大する
    auto children = this->getChildren();
    
    for (auto child : children)
    {
//        HMSLOG("children:%d", child->getTag());
        if (child->getTag() == POPUP_ACTION_TAG) {
            auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
            child->runAction(scale);
        }
    }
    
    //指定秒数待ってからタップを可能にする。
    auto func = CallFunc::create([normal_scene](){
        normal_scene->removeTapDisableLayer() ;
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

void DungeonSubMenuOptionLayer::deleteAction()
{
    //音量を適用
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(_bgm_volume);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(_effect_volume);
    
    //窓をひっこめ削除する
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer() ;
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3",false);
    
    //自身の子からPOPUP_ELEMENTの要素をすべて縮小する
    auto children = this->getChildren();
    for (auto child : children) {
        HMSLOG("children:%d", child->getTag());
        if (child->getTag() == POPUP_ACTION_TAG) {
            auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this,normal_scene](){
        this->removeFromParent();
        //タップ禁止レイヤー削除
        normal_scene->removeTapDisableLayer() ;
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

void DungeonSubMenuOptionLayer::saveSetting(Ref* sender)
{
    //音量の保存
    SaveData::saveFloat("bgm_volume", _get_bgm_volume);
    SaveData::saveFloat("effect_volume", _get_effect_volume);
    
    //保存前の音量を変更
    _bgm_volume = _get_bgm_volume;
    _effect_volume = _get_effect_volume;
    
    //エフェクトの保存
    //ボタン押した際にflgをボタンチェンジ用に変更しているので、逆の真偽を保存する
    if (_effectFlg)
        SaveData::saveBool("effect", false);
    else
        SaveData::saveBool("effect", true);
    
    deleteAction();
    
//    //保存した音量の確認
//    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/attack2.mp3",false);
}

void DungeonSubMenuOptionLayer::loadSetting(Ref* sender)
{
    //音量を初期化
    _get_bgm_volume = 0.5f;
    _get_effect_volume = 0.5f;
    
    ControlSlider* bgm = (ControlSlider*)this->getChildByTag(POPUP_ACTION_TAG)->getChildByTag(SOUND_SLIDER);
    bgm->setValue(_get_bgm_volume);
    
    ControlSlider* effect = (ControlSlider*)this->getChildByTag(POPUP_ACTION_TAG)->getChildByTag(EFFECT_SLIDER);
    effect->setValue(_get_effect_volume);
    
    //初期音量を適用
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(_get_bgm_volume);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(_get_effect_volume);
    
    //エフェクトを初期化
    _effectFlg = true;
    changeButton();
}

void DungeonSubMenuOptionLayer::changeBgmVolume(Ref* sender, Control::EventType controlEvent)
{
    //スライダーからBGMの値を取得
    ControlSlider* bgm = (ControlSlider*)this->getChildByTag(POPUP_ACTION_TAG)->getChildByTag(SOUND_SLIDER);
    _get_bgm_volume = bgm->getValue();
    _bgm_volume = _get_bgm_volume;
    
    //変更した音量を適用
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(_get_bgm_volume);
    SaveData::saveFloat("bgm_volume", _get_bgm_volume);
    HMSLOG("BGM:%f", _get_bgm_volume);
}

void DungeonSubMenuOptionLayer::changeSeVolume(Ref* sender, Control::EventType controlEvent)
{
    //スライダーから効果音の値を取得
    ControlSlider* effect = (ControlSlider*)this->getChildByTag(POPUP_ACTION_TAG)->getChildByTag(EFFECT_SLIDER);
    _get_effect_volume = effect->getValue();
    _effect_volume = _get_effect_volume;
    
    //変更した音量を適用
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(_get_effect_volume);
    SaveData::saveFloat("effect_volume", _get_effect_volume);
    HMSLOG("EFFECT:%f", _get_effect_volume);
}

void DungeonSubMenuOptionLayer::changeEffect(Ref* sender)
{
    changeButton();
}

//タップ処理
bool DungeonSubMenuOptionLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}

void DungeonSubMenuOptionLayer::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
}
//指を離した時にスライダーを動かしたか判定する
void DungeonSubMenuOptionLayer::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
//    //エフェクト音(１回)
//    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/attack2.mp3",false);
//    
//    HMSLOG("onTouchEnded!!!");
}
