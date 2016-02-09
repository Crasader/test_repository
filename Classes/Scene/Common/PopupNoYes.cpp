
#include "PopupNoYes.h"

bool PopupNoYes::init()
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

void PopupNoYes::setPopup(string message , std::function<void()> no_func, std::function<void()> yes_func){
    
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_300.png");
    bg->setScale(0);
    bg->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5, 0.5) - bg->convertToWorldSpaceAR(Size::ZERO));
    this->addChild(bg, 2 );
    
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(message, FONT_NAME_SYSTEM, 26);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(bg->getContentSize().width / 2, bg->getContentSize().height / 2 + 40));
    //enableOutline(Color4B(0, 0, 0, 255), 3);
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(label, 1);
    
    //ボタンの作成
    Vector<MenuItem*> buttons;
    
    //いいえボタン
    MenuItemImage* pCanselBtn = MenuItemImage::create("init/Home/Common/bt_no.png", "init/Home/Common/bt_no.png", [this,no_func](Ref *){
        no_func() ;
        //deleteAction() ;
        this->removeFromParent();
    });
    buttons.pushBack(pCanselBtn) ;
    
    //はいボタン
    MenuItemImage* pOkBtn = MenuItemImage::create("init/Home/Common/bt_yes.png", "init/Home/Common/bt_yes.png", [this,yes_func](Ref *){
        yes_func();
        this->removeFromParent();
    });
    
    buttons.pushBack(pOkBtn) ;
    
    
    Menu* pMenu = Menu::createWithArray(buttons);
    pMenu->alignItemsHorizontallyWithPadding(30) ;
    pMenu->setPosition(Point(285, 65));
    //pMenu->setAnchorPoint(Point(0.5, 0));
    bg->addChild(pMenu);
    
    //閉じるボタン
    auto back = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", [this,no_func](Ref *){
        no_func() ;
        //deleteAction();
        this->removeFromParent();
    });
    
    auto Menu1 = Menu::create(back, nullptr);
    Menu1->setPosition(Point(bg->getContentSize().width, bg->getContentSize().height));
    bg->addChild(Menu1, 25);

    
    //ポップアップアクション
    auto children = this->getChildren();
    
    for (auto child : children)
    {
        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
        child->runAction(scale);
    }
}
