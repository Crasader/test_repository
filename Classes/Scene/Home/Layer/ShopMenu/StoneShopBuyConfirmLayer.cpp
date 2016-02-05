#include "StoneShopBuyConfirmLayer.h"

StoneShopBuyConfirmLayer*StoneShopBuyConfirmLayer::create(ButtonType type, string message, function<void()> firstFunc, function<void()> secondFunc)
{
    StoneShopBuyConfirmLayer *pRet = new StoneShopBuyConfirmLayer();
    
    if(pRet && pRet->init(type, message, firstFunc, secondFunc)) {
        pRet->autorelease() ;
        return pRet ;
    }else{
        delete pRet ;
        pRet = NULL;
        return NULL ;
    }
}

bool StoneShopBuyConfirmLayer::init(ButtonType type, string message, function<void()> &firstFunc,function<void()> &secondFunc)
{
    if ( !Layer::init() ) {
        return false;
    }
    HMSLOG("StoneShopBuyConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    
    //メンバ変数に保存
    _firstFunc = firstFunc;
    _secondFunc = secondFunc;
    _message = message;
    _type = type ;

    return true;
}

void StoneShopBuyConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    initEventListener() ;
    
    //画像の作成
    setupImage();
    
    //ポップアップアクション
    popupAction();
}

void StoneShopBuyConfirmLayer::setupImage()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_300.png");
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    bg->setScale(0);
    this->addChild(bg, 2, POPUP_ACTION_TAG);
    
    Point button_center = Point(285, 100) ;
    Vector<MenuItem*> buttons;
    if(_type==ButtonType::YES_AND_NO){
        //いいえボタン
        MenuItemImage* pCanselBtn = MenuItemImage::create("init/Home/Common/bt_no.png", "init/Home/Common/bt_no.png", CC_CALLBACK_1(StoneShopBuyConfirmLayer::onSecondFunc, this));
        buttons.pushBack(pCanselBtn) ;
        
        //はいボタン
        MenuItemImage* pOkBtn = MenuItemImage::create("init/Home/Common/bt_yes.png", "init/Home/Common/bt_yes.png",  CC_CALLBACK_1(StoneShopBuyConfirmLayer::onFirstFunc, this));
        buttons.pushBack(pOkBtn) ;
    }else if(_type==ButtonType::OK){
        //OKボタン
        MenuItemImage* okBtn = MenuItemImage::create("init/Home/Common/bt_ok.png", "init/Home/Common/bt_ok.png",  CC_CALLBACK_1(StoneShopBuyConfirmLayer::onFirstFunc, this));
        buttons.pushBack(okBtn) ;
    }

    Menu* pMenu = Menu::createWithArray(buttons);
    pMenu->alignItemsHorizontallyWithPadding(30) ;
    pMenu->setPosition(button_center);
    bg->addChild(pMenu);
    
    //閉じるボタン
    auto back = MenuItemImage::create("init/Home/Popup/bt_close_L.png", "init/Home/Popup/bt_close_L.png", CC_CALLBACK_0(StoneShopBuyConfirmLayer::deleteAction, this));
    auto Menu1 = Menu::create(back, nullptr);
    Menu1->setPosition(Point(bg->getContentSize().width, bg->getContentSize().height));
    bg->addChild(Menu1, 25, BACK_TAG);
    
    //ボタン画像の余白部分をのぞいたときのサイズ
    int bt_visual_size_y = 70 ;
    //テキストの中心位置(フレームの下端を原点とするときの) = ボタンの上端とフレームの上端の中心 = 二点の座標の合計/2
    float text_position_y = (bg->getContentSize().height + (button_center.y + bt_visual_size_y/2))/ 2 ;
    
    //テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, 27);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(bg->getContentSize().width/2, text_position_y));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    bg->addChild(label);
}

//いいえボタンコールバック
void StoneShopBuyConfirmLayer::onSecondFunc(Ref *obj)
{
    _secondFunc() ;
    deleteAction() ;
}

//はいボタンコールバック
void StoneShopBuyConfirmLayer::onFirstFunc(Ref *obj)
{
    _firstFunc();

    this->removeFromParent();
}

void StoneShopBuyConfirmLayer::initEventListener()
{
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
}

//窓を出現させる
void StoneShopBuyConfirmLayer::popupAction()
{
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer() ;
    
    //自身の子からPOPUP_ELEMENTの要素をすべて拡大する
    auto children = this->getChildren();
    for (auto child : children) {
        HMSLOG("children:%d", child->getTag()) ;
        if (child->getTag() == POPUP_ACTION_TAG) {
            auto scale    = ScaleTo::create(0.1f, ratio);
            child->runAction(scale);
        }
    }
    
    //指定秒数待ってからタップを可能にする。
    auto func = CallFunc::create([normal_scene](){
        normal_scene->removeTapDisableLayer() ;
    });
    
    runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
}

//窓をひっこめる
void StoneShopBuyConfirmLayer::deleteAction()
{
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer() ;
    
    //自身の子からPOPUP_ELEMENTの要素をすべて縮小する
    auto children = this->getChildren();
    for (auto child : children) {
        HMSLOG("children:%d", child->getTag()) ;
        if (child->getTag() == POPUP_ACTION_TAG) {
            auto scale = ScaleTo::create(0.1f, 0);
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
