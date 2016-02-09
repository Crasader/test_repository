#include "PopupGeneralConfirmLayer.h"

PopupGeneralConfirmLayer* PopupGeneralConfirmLayer::create(string type)
{
    PopupGeneralConfirmLayer *node = new PopupGeneralConfirmLayer();
    if (node && node->init(type))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool PopupGeneralConfirmLayer::init(string type)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupGeneralConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    //選択した属性の取得
    _type = type;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(PopupGeneralConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupGeneralConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupGeneralConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupGeneralConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void PopupGeneralConfirmLayer::createMessageBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //文字の背景画像
    message_bg = Sprite::create("init/Tutorial/navi1.png");
    message_bg->setScale(0);
    message_bg->setPosition(convertFromDisplaySize(Point(320, 600), 0.5, 0.5));
    addChild(message_bg, 1, POPUP_ACTION_TAG);
}

void PopupGeneralConfirmLayer::createTypeImage()
{
    //属性画像の表示
    if (_type == "1")
    {
        type_image = Sprite::create("init/Tutorial/country1.png");
    }
    else if (_type == "2")
    {
        type_image = Sprite::create("init/Tutorial/country2.png");
    }
    else if (_type == "3")
    {
        type_image = Sprite::create("init/Tutorial/country3.png");
    }
    else if (_type == "4")
    {
        type_image = Sprite::create("init/Tutorial/country3.png");
    }
    else {
        type_image = Sprite::create("init/Tutorial/country3.png");
    }
    
    type_image->setPosition(Point(message_bg->getContentSize().width / 2 - 60, message_bg->getContentSize().height / 2 - 16));
    message_bg->addChild(type_image, 1);
}

void PopupGeneralConfirmLayer::showDeckName()
{
    //選んだデッキ名の表示
    if (_type == "1")
    {
        _deck_label = HMSFontUtil::createWithTTFBySystemFontArgument("豪傑", FONT_NAME_SYSTEM, 30);
    }
    else if (_type == "2")
    {
        _deck_label = HMSFontUtil::createWithTTFBySystemFontArgument("知略", FONT_NAME_SYSTEM, 30);
    }
    else if (_type == "3")
    {
        _deck_label = HMSFontUtil::createWithTTFBySystemFontArgument("非道", FONT_NAME_SYSTEM, 30);
    }
    else if (_type == "4")
    {
        _deck_label = HMSFontUtil::createWithTTFBySystemFontArgument("美女と猛将", FONT_NAME_SYSTEM, 30);
    }
    else {
        _deck_label = HMSFontUtil::createWithTTFBySystemFontArgument("南蛮", FONT_NAME_SYSTEM, 30);
    }
    
    _deck_label->setPosition(Point(message_bg->getContentSize().width / 2 - 60, message_bg->getContentSize().height / 2 - 16));
    message_bg->addChild(_deck_label, 1);

}

void PopupGeneralConfirmLayer::showMessage()
{
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("でよろしいですか？", FONT_NAME_SYSTEM, 30);
    label->setAnchorPoint(Point(0 , 0.5));
    label->setPosition(Point(message_bg->getContentSize().width / 2, message_bg->getContentSize().height / 2 - 16));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    message_bg->addChild(label, 1);
}

void PopupGeneralConfirmLayer::createButtonBg()
{
    //背景
    bg = Sprite::create("init/Home/Popup/popup_150.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(320, 420), 0.5, 0.5));
    this->addChild(bg, 1, POPUP_ACTION_TAG);
    
}

void PopupGeneralConfirmLayer::createButton()
{
    //YESボタンの作成
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender)
    {
        //選択した属性の保存
        SaveData::saveString("generalType", _type);

        //ページの切り替え
        auto tutorial_scene = (TutorialScene *)this->getParent()->getParent();
        auto next_layer = LastConfirmLayer::create();
        tutorial_scene->changePage(next_layer);
    });
    yes_btn->setAnchorPoint(Point(0.5, 0));
    
    //NOボタンの作成
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender)
    {
        this->deleteAction();

        //消したメッセージ画像の表示
        auto general_select_layer = (GeneralSelectLayer *)this->getParent();
        general_select_layer->message_bg->setVisible(true);
    });
    no_btn->setAnchorPoint(Point(0.5, 0));
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(30);
    menu->setPosition(Point(bg->getContentSize().width / 2, 20));
    bg->addChild(menu, 1);
}

void PopupGeneralConfirmLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void PopupGeneralConfirmLayer::deleteAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == POPUP_ACTION_TAG)
        {
            auto scale    = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func = CallFunc::create([this](){
        this->removeFromParent();
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, NULL));
}

void PopupGeneralConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //メッセージ背景の作成
    createMessageBg();
    
    //属性画像の表示
//    createTypeImage();
    
    //選んだデッキの名前
    showDeckName();
    
    //メッセージの表示
    showMessage();
    
    //ボタン背景の作成
    createButtonBg();
    
    //ボタンの作成
    createButton();
    
    //ポップアップアクション
    popupAction();
}

bool PopupGeneralConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch PopupGeneralConfirmLayer!");
    return true;
}
void PopupGeneralConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupGeneralConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupGeneralConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
