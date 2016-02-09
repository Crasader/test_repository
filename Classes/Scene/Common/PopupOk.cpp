
#include "PopupOk.h"

bool PopupOk::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("PopupOk::init()");

    ratio = getVisibleRatio();
    
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    
    return true;
}

void PopupOk::setPopup(string message , std::function<void()> func){
    
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_300.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5, 0.5) - bg->convertToWorldSpaceAR(Size::ZERO));
    //bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 2 );
    
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(message, FONT_NAME_SYSTEM, 26);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(bg->getContentSize().width / 2, bg->getContentSize().height / 2 + 40));
    //enableOutline(Color4B(0, 0, 0, 255), 3);
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(label, 1);
    
    //ボタンの作成
    auto btn_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto btn_on  = Sprite::create("init/Home/Common/bt_ok.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    auto ok_btn = MenuItemSprite::create(btn_off, btn_on, [this, func](Ref* sender)
                                         {
                                             func();
                                             
                                             auto fade = FadeOut::create(0.15f);
                                             this->runAction(Sequence::create(fade, RemoveSelf::create(), NULL));
                                         });
    ok_btn->setAnchorPoint(Point(0.5, 0));
    auto Menu = Menu::create(ok_btn, nullptr);
    Menu->setPosition(Point(bg->getContentSize().width / 2, 15*ratio));
    bg->addChild(Menu, 1);
    
    
    //ポップアップアクション
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}
