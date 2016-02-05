#include "PopupOkButtonLayer.h"

PopupOkButtonLayer* PopupOkButtonLayer::create(function<void()> ok_func, string message, bool shadow_flg, PopupSize popup_size)
{
    PopupOkButtonLayer *node = new PopupOkButtonLayer();
    if (node && node->init(ok_func, message, shadow_flg, popup_size))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool PopupOkButtonLayer::init(function<void()> ok_func, string message, bool shadow_flg, PopupSize popup_size)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupOkButtonLayer::init()");
    
    ratio = getVisibleRatio();
    
    //表示テキストの取得
    _message = message;
    
    //サイズ指定
    _popup_size = popup_size;
    
    //実行関数の保存
    _ok_function = ok_func;
    
    //コールバック
    battle_list_id = 0;
    _shadow_flg = shadow_flg;
    
    //背景画像の作成
    createBg();
    
    return true;
}

void PopupOkButtonLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //ボタンの作成
    createButton();
    
    //テキストの表示
    showMessage();
    
    //ポップアップアクション
    Utility::openPopupAction(this);
}

void PopupOkButtonLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(PopupOkButtonLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PopupOkButtonLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PopupOkButtonLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(PopupOkButtonLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void PopupOkButtonLayer::createBg()
{
    //影
    if (_shadow_flg) {
        auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
        shadow->setScale(ratio);
        shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(shadow, 1);
    }
    
    //背景
    string path = "init/Home/Popup/popup_300.png";;
    switch (_popup_size)
    {
        case Popup_400: path = "init/Home/Popup/popup_400.png"; break;
        case Popup_500: path = "init/Home/Popup/popup_500.png"; break;
        case Popup_800: path = "init/Home/Popup/popup_800.png"; break;
        case Popup_900: path = "init/Home/Popup/popup_900.png"; break;
        default: break;
    }
    _bg = Sprite::create(path);
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
}

void PopupOkButtonLayer::createButton()
{
    //OKボタンの作成
    auto ok_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto ok_on  = Sprite::create("init/Home/Common/bt_ok.png");
    ok_on->setColor(Color3B(180, 180, 180));
    
    auto ok_btn = MenuItemSprite::create(ok_off, ok_on, [this](Ref* sender)
                                          {
                                              //SE
                                              CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
                                              _ok_function();
                                              
                                              //ポップアップ閉じる
                                              Utility::closePopupAction(this);
                                          });
    ok_btn->setAnchorPoint(Point(0.5, 0));
    
    auto menu = Menu::create(ok_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 20));
    _bg->addChild(menu, 1);
}

void PopupOkButtonLayer::showMessage()
{
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_message, 28, _bg->getContentSize().width - 20));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height * 2.0 / 3.0));
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _bg->addChild(label, 2);
}

Sprite *PopupOkButtonLayer::getBackground()
{
    return _bg;
}

void PopupOkButtonLayer::setIdForBattleRestart(int battle_list_id)
{
    this->battle_list_id = battle_list_id;
}

bool PopupOkButtonLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch PopupOkButtonLayer!");
    
    return true;
}
void PopupOkButtonLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupOkButtonLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void PopupOkButtonLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
