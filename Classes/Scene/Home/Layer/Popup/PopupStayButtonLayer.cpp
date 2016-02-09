#include "PopupStayButtonLayer.h"

PopupStayButtonLayer* PopupStayButtonLayer::create(function<void()> yes_func, function<void()> no_func,string message)
{
    PopupStayButtonLayer *node = new PopupStayButtonLayer();
    if (node && node->init(yes_func, no_func, message))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool PopupStayButtonLayer::init(function<void()> yes_func, function<void()> no_func, string message)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupStayButtonLayer::init()");
    
    ratio = getVisibleRatio();
    
    //表示テキストの取得
    _message = message;
    
    //実行関数の保存
    _yes_function = yes_func;
    _no_function  = no_func;
    
    //コールバック
    battle_list_id = 0;
//    _shadow_visible_flg = shadow_visible_flg;
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(PopupStayButtonLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupStayButtonLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupStayButtonLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupStayButtonLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void PopupStayButtonLayer::setIdForBattleRestart(int battle_list_id)
{
    this->battle_list_id = battle_list_id;
}

void PopupStayButtonLayer::createBg()
{
    //影
    if (_shadow_visible_flg) {
        auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
        shadow->setScale(ratio);
        shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(shadow, 1);
    }
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_400.png");
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, 2 ,POPUP_ACTION_TAG);
}

void PopupStayButtonLayer::createButton()
{
    //はいボタンの作成
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender)
    {
        _yes_function();
    });
    yes_btn->setAnchorPoint(Point(0.5, 0));
    
    
    //いいえボタンの作成
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender)
    {
        //ポップアップ閉じるアクション
        deleteAction();
        
        _no_function();
    });
    no_btn->setAnchorPoint(Point(0.5, 0));
    
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(30 * ratio);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 20));
    _bg->addChild(menu, 1);
}

void PopupStayButtonLayer::showMessage()
{
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, 250));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    _bg->addChild(label, 2);
}

void PopupStayButtonLayer::popupAction()
{
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale    = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}

void PopupStayButtonLayer::deleteAction()
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

void PopupStayButtonLayer::onEnter()
{
    Layer::onEnter();
    
    //背景画像の作成
    createBg();
    
    //ボタンの作成
    createButton();
    
    //テキストの表示
    showMessage();
    
    //ポップアップアクション
    popupAction();
}

bool PopupStayButtonLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch PopupStayButtonLayer!");
    
    return true;
}
void PopupStayButtonLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupStayButtonLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupStayButtonLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}