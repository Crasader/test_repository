#include "PopupNameConfirmLayer.h"

PopupNameConfirmLayer* PopupNameConfirmLayer::create(function<void()> yesFunc,function<void()> noFunc,string name)
{
    PopupNameConfirmLayer *node = new PopupNameConfirmLayer();
    if (node && node->init(yesFunc, noFunc, name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool PopupNameConfirmLayer::init(function<void()> &yesFunc,function<void()> &noFunc, string name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupNameConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    //入力した名前を格納
    _name = name;
    
    //各ボタンが押されたときの処理を格納
    _noFunc = noFunc ;
    _yesFunc = yesFunc ;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(PopupNameConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupNameConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupNameConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupNameConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void PopupNameConfirmLayer::createMessageBg()
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

void PopupNameConfirmLayer::showMessage()
{
    //名前の表示
    Label *name = HMSFontUtil::createWithTTFBySystemFontArgument(_name + " でよろしいですか？", FONT_NAME_SYSTEM, 25);
    name->setPosition(Point(message_bg->getContentSize().width / 2 + 60, message_bg->getContentSize().height / 2 - 16));
    message_bg->addChild(name, 1);
    
    //表示テキスト
//    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("でよろしいですか？", FONT_NAME_SYSTEM, 25);
//    label->setAnchorPoint(Point(0 , 0.5));
//    label->setPosition(Point(message_bg->getContentSize().width / 2 + 60, message_bg->getContentSize().height / 2 - 16));
//enableOutline(Color4B(0, 0, 0, 255), 1);
//    message_bg->addChild(label, 1);
}

void PopupNameConfirmLayer::createButtonBg()
{
    //背景
    bg = Sprite::create("init/Home/Popup/popup_150.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(320, 420), 0.5, 0.5));
    this->addChild(bg, 1, POPUP_ACTION_TAG);
    
}

void PopupNameConfirmLayer::createButton()
{
    //YESボタンの作成
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender)
    {
		//SE
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
        _yesFunc() ;
    });
    yes_btn->setAnchorPoint(Point(0.5, 0));
    
    //NOボタンの作成
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender)
    {
		//SE
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_no.mp3");
        _noFunc() ;
        this->deleteAction();
    });
    no_btn->setAnchorPoint(Point(0.5, 0));
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(30);
    menu->setPosition(Point(bg->getContentSize().width / 2, 20));
    bg->addChild(menu, 1);
}

void PopupNameConfirmLayer::popupAction()
{
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_on.mp3");

	auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void PopupNameConfirmLayer::deleteAction()
{
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_off.mp3");

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

void PopupNameConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //メッセージ背景の作成
    createMessageBg();
    
    //メッセージの表示
    showMessage();
    
    //ボタン背景の作成
    createButtonBg();
    
    //ボタンの作成
    createButton();
    
    //ポップアップアクション
    popupAction();
}

bool PopupNameConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch PopupNameConfirmLayer!");
    return true;
}
void PopupNameConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupNameConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupNameConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
