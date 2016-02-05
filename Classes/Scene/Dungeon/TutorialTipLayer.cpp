#include "TutorialTipLayer.h"
#include "TutorialNaviLayer.h"

#define TUTORIAL_TIP_KEY "tutorial_tip_index"
#define TUTORIAL_TIP_MAX 30

#define TAG_FRAME 1

bool TutorialTipLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TutorialTipLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TutorialTipLayer::onEnter(){
    Layer::onEnter() ;
    
    //リスナー初期化
    initListener() ;
}

void TutorialTipLayer::initListener(){
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch *touch, Event *event)->bool{ return true; };
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool TutorialTipLayer::onTouchBegan(Touch *touch, Event *event)
{
    HMSLOG("tapped");
    
    return true;
}

void TutorialTipLayer::setup(int index)
{
    //枠 (枠も含めてチュートリアル画像になります。)
    string str = to_string(index);
    if(str.length()==1) str = "0" + str;
    Sprite *tutorial_img = Sprite::create("download/Tutorial/" + str + ".png");
    tutorial_img->setScale(0) ;
    tutorial_img->setTag(TAG_FRAME);
    tutorial_img->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(tutorial_img) ;
    
    //ボタン
    auto ok_btn_off = Sprite::create(IMG_OK_BT);
    auto ok_btn_on  = Sprite::create(IMG_OK_BT);
    ok_btn_on->setColor(Color3B(180, 180, 180));
    auto ok_btn = MenuItemSprite::create(ok_btn_off, ok_btn_on, CC_CALLBACK_0(TutorialTipLayer::deleteAction, this));
    ok_btn->setPosition(Point(tutorial_img->getContentSize().width/2, 53)) ;
    auto menu = Menu::create(ok_btn, nullptr);
    menu->setPosition(Point::ZERO) ;
    tutorial_img->addChild(menu, 2);

    popupAction();
}

void TutorialTipLayer::popupAction()
{
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_on.mp3");

    auto frame = this->getChildByTag(TAG_FRAME) ;
    auto scale = ScaleTo::create(0.2f, 1.0*ratio);
    frame->runAction(scale);
}

void TutorialTipLayer::deleteAction()
{
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_off.mp3");
	
	auto frame = this->getChildByTag(TAG_FRAME) ;
    auto scale = ScaleTo::create(0.2f, 0);
    frame->runAction(scale);
    
    auto func = CallFunc::create([this](){
        close();
    });
    
    runAction(Sequence::create(DelayTime::create(0.1f), func, NULL));
}

void TutorialTipLayer::close()
{
    TutorialNaviLayer *parent_layer = (TutorialNaviLayer *)this->getParent();
    parent_layer->callback();
    this->removeFromParentAndCleanup(true);
}
