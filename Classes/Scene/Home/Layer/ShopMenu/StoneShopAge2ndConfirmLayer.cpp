#include "StoneShopAge2ndConfirmLayer.h"

bool StoneShopAge2ndConfirmLayer::init()
{
    if ( !Layer::init() ) {
        return false;
    }
    HMSLOG("StoneShopAge2ndConfirmLayer::init()");
    
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    initEventListener();
	used_age_type = 0;
    
    return true;
}

void StoneShopAge2ndConfirmLayer::onEnter(){
    Layer::onEnter();
    
    //0.1数秒後ポップアップアクション
    auto func = CallFunc::create([this](){
        popupAction();
    });
    runAction(Sequence::create(DelayTime::create(0.1f), func, nullptr));
}

void StoneShopAge2ndConfirmLayer::setupImage(string confirmMessage)
{
	//影
	auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
	shadow->setScale(ratio);
	shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
	this->addChild(shadow, 1);
	
	//背景
	auto bg = Sprite::create("init/Home/Popup/popup_200.png");
	bg->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
	bg->setScale(0);
	this->addChild(bg, 2, POPUP_ACTION_TAG);
	
	//モーダルレイヤーを消すボタン
	auto close = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close_L.png", CC_CALLBACK_0(StoneShopAge2ndConfirmLayer::deleteAction, this));
	close->setPosition(Point(bg->getContentSize().width, bg->getContentSize().height)) ;
	auto menu1 = Menu::create(close, nullptr);
	menu1->setPosition(Point::ZERO);
	bg->addChild(menu1, 25, BACK_TAG);
	
	auto label = HMSFontUtil::createWithTTFBySystemFontArgument(confirmMessage, FONT_NAME_SYSTEM, 30);
	label->setHorizontalAlignment(TextHAlignment::CENTER);
	label->setPosition(Point(bg->getContentSize()/2));
	//enableOutline(Color4B(0, 0, 0, 255), 3);
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
	
	bg->addChild(label);
}

void StoneShopAge2ndConfirmLayer::setupImage(string confirmMessage, int age_type)
{
	if(age_type > 0) used_age_type = age_type;
	
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    auto bg = Sprite::create("init/Home/Popup/popup_500.png");
    bg->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    bg->setScale(0);
    this->addChild(bg, 2, POPUP_ACTION_TAG);
    
    //モーダルレイヤーを消すボタン
    auto close = MenuItemImage::create("init/Home/Popup/bt_close.png", "init/Home/Popup/bt_close_L.png", CC_CALLBACK_0(StoneShopAge2ndConfirmLayer::deleteAction, this));
    close->setPosition(Point(bg->getContentSize().width, bg->getContentSize().height)) ;
    auto menu1 = Menu::create(close, nullptr);
    menu1->setPosition(Point::ZERO);
    bg->addChild(menu1, 25, BACK_TAG);
    
    //いいえボタン
    MenuItemImage* pCanselBtn = MenuItemImage::create("init/Home/Common/bt_no.png", "init/Home/Common/bt_no.png", CC_CALLBACK_1(StoneShopAge2ndConfirmLayer::onNoButton, this)) ;

    pCanselBtn->setPosition(Point(400, 100));
    
    //はいボタン
    MenuItemImage* pOkBtn = MenuItemImage::create("init/Home/Common/bt_yes.png", "init/Home/Common/bt_yes.png", CC_CALLBACK_1(StoneShopAge2ndConfirmLayer::onYesButton, this));
    pOkBtn->setPosition(Point(170, 100));

    Menu* pMenu2 = Menu::create(pCanselBtn,pOkBtn, NULL);
    pMenu2->setPosition(Point::ZERO);
    bg->addChild(pMenu2);

    //ボタン画像の余白部分をのぞいたときのサイズ
    int bt_visual_size_y = 70 ;
    //テキストの中心位置(フレームの下端を原点とするときの) = ボタンの上端とフレームの上端の中心 = 二点の座標の合計/2
    float text_position_y = (bg->getContentSize().height + (pOkBtn->getPosition().y + bt_visual_size_y/2))/ 2 ;

    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(confirmMessage, FONT_NAME_SYSTEM, 30);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(bg->getContentSize().width/2, text_position_y));
//enableOutline(Color4B(0, 0, 0, 255), 3);
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);

    bg->addChild(label);
}

//いいえボタンコールバック
void StoneShopAge2ndConfirmLayer::onNoButton(Ref *obj)
{
    deleteAction() ;
}

//はいボタンコールバック
void StoneShopAge2ndConfirmLayer::onYesButton(Ref *obj)
{
	if(used_age_type > 0) SaveData::saveInt("ageBuyStatus", used_age_type);
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->changePage("StoneShopMenu");
    this->removeFromParent();
}

void StoneShopAge2ndConfirmLayer::initEventListener()
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
void StoneShopAge2ndConfirmLayer::popupAction()
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
void StoneShopAge2ndConfirmLayer::deleteAction()
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
