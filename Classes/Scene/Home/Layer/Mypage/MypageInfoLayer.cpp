#include "MypageInfoLayer.h"

MypageInfoLayer::MypageInfoLayer()
:_image_path()
,_touchable(false)
{
}

MypageInfoLayer* MypageInfoLayer::create(string image_path)
{
    MypageInfoLayer *node = new MypageInfoLayer();
    if (node && node->init(image_path))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool MypageInfoLayer::init(string image_path)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("MypageInfoLayer::init()");
    
    ratio = getVisibleRatio();
    
    //イベントリスナーの初期化
    initEventListener();
    
    //データの設定
    _image_path = image_path;
    
    return true;
}

void MypageInfoLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(MypageInfoLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MypageInfoLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MypageInfoLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(MypageInfoLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void MypageInfoLayer::onEnter()
{
    Layer::onEnter();
    
    //背景の作成
    createBg();
    
    //画像の表示
    showImage();
    
    //ポップアップアクション
    Utility::openPopupAction(this);
}

void MypageInfoLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
}

void MypageInfoLayer::showImage()
{
    //画像の表示
    auto sp = Sprite::create(_image_path);
    sp->setScale(0);
    sp->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(sp, Utility::Z_Popup_Action, Utility::T_Popup_Action);
}

//void MypageInfoLayer::popupAction()
//{
//    for (auto child : getChildren())
//    {
//        auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
//        child->runAction(scale);
//    }
//    
//    auto func = CallFunc::create([this](){
//        _touchable = true;
//    });
//    
//    runAction(Sequence::create(DelayTime::create(0.2), func, nullptr));
//}
//
//void MypageInfoLayer::deleteAction()
//{
//    auto mypage_layer = (MypageLayer *)this->getParent()->getChildByTag(MAIN_LAYER_TAG);
//    
//    for (auto child : getChildren())
//    {
//        if (child->getTag() == POPUP_ACTION_TAG)
//        {
//            auto scale = ScaleTo::create(0.1f, 0);
//            child->runAction(scale);
//        }
//    }
//    
//    auto func = CallFunc::create([mypage_layer](){
//
//        if (mypage_layer->_login_bonus_flg == false && SaveData::loadString("beginner_guide_complete_flg") == "N")
//            //ログインボーナスが無い、且つビギナーガイドをクリアしていない場合、openチェック
//            mypage_layer->checkOpenCheckBeginnerGuideLayer();
//            
//    });
//    
//    runAction(Sequence::create(DelayTime::create(0.2f), RemoveSelf::create(), func, nullptr));
//}

bool MypageInfoLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch MypageInfoLayer!");
    
    return true;
}
void MypageInfoLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void MypageInfoLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //画面をタップされたとき削除
    auto mypage_layer = (MypageLayer *)this->getParent()->getChildByTag(MAIN_LAYER_TAG);
    auto normal_scene = (NormalScene *)mypage_layer->getParent() ;
    if(mypage_layer && normal_scene->getMainName()=="")mypage_layer->showNextPopupStatus();
    
    Utility::closePopupAction(this);
    
//    if (_touchable)
//        deleteAction();
}
void MypageInfoLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
