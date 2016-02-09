//
//  ScrollBarLayer.cpp
//  war
//
//  Created by HMUser on 2015/11/02.
//
//

#include "ScrollBarLayer.h"

#define BAR_FIX_SIZE 10.0f
#define BAR_MIN_SIZE 20.0f

ScrollBarLayer::ScrollBarLayer(cocos2d::extension::ScrollView *view)
:view(view),
barSize(0.0f)
{
}

ScrollBarLayer* ScrollBarLayer::create(cocos2d::extension::ScrollView *view)
{
    ScrollBarLayer *sprite = new ScrollBarLayer(view);
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}


bool ScrollBarLayer::init(){
    if(!Layer::init()){
        return false;
    }
    
    this->setAnchorPoint(Vec2::ZERO);
    
    float offset;
    
    
    Point position = view->getPosition();
    Rect imageSize = Rect(0, 0, BAR_FIX_SIZE, BAR_MIN_SIZE);
    Rect insets    = Rect(1, 4, 8, 12);
    bar = ui::Scale9Sprite::create("init/CharaMenu/scroll_bar.png", imageSize, insets);
    Size bSize          = bar->getContentSize();
    bar->setContentSize(bSize * 2);
    
    bar->setAnchorPoint(Vec2(0.5f, 0));
    bar->setPosition(Vec2(bar->getContentSize().width/2 - getVisibleSpaceWidth(), 0));
    
    Size size          = view->cocos2d::Node::getContentSize();
    viewFixSize        = size.height;
    currentContentSize = beforeContentSize = view->getContentSize().height;
    
    this->setPosition(position.x + size.width - BAR_FIX_SIZE*2 - getVisibleSpaceWidth(), position.y);
    //    this->setPosition(0,0);
    
    bool isVisible = ( currentContentSize <= viewFixSize ) ? false: true;
    this->setVisible(isVisible);
    
    this->addChild(bar);
    
    initEventListener();
    
    setBarScale();
    setBarPosition(offset);
    
    Node *parent = view->getParent();
    parent->addChild(this, 1, -1);
    
    footer_height = getContentSize().height - view->getPositionY();
    header_height = getContentSize().height - view->getViewSize().height - view->getPositionY();

    return true;


}

void ScrollBarLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(ScrollBarLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ScrollBarLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ScrollBarLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ScrollBarLayer::onTouchCancelled, this);
    
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    _eventDispatcher->setPriority(listener, -10000);
}


void ScrollBarLayer::setBarPosition(float offset)
{
    offset *= -1;
    
    // Calculate bar position
    float pos = (((offset - viewFixSize) / currentContentSize) * viewFixSize) + barSize;
    if ( pos < 0 || pos > (viewFixSize - barSize + 1) )
    {
        return;
    }
    bar->setPosition(Vec2(bar->getPositionX(), pos));
}

void ScrollBarLayer::setBarScale()
{
    Size size          = view->cocos2d::Node::getContentSize();
    viewFixSize        = size.height;
    
    
    beforeContentSize = currentContentSize;
    
    // Calculate bar scale ( float unit )
    float scale = (viewFixSize / currentContentSize) * viewFixSize;
    barSize = scale;
    
//    if ( scale < BAR_MIN_SIZE || scale >= viewFixSize )
//    {
//        return;
//    }
    bar->setContentSize(Size(BAR_FIX_SIZE, scale));
}


void ScrollBarLayer::refresh()
{
//    if ( bar->getScaleX() == 0.0f || bar->getScaleY() == 0.0f )
//    {
//        fadeIn();
//    }
    
    this->unscheduleAllCallbacks();
    
    float offset;
    offset = view->getContentOffset().y;
    currentContentSize = view->getContentSize().height;
    
    bool isVisible = ( currentContentSize <= viewFixSize ) ? false : true;
    this->setVisible(isVisible);
    
//    if ( beforeContentSize != currentContentSize )
//    {
    setBarScale();
//    }
    
    
    // Set position
    setBarPosition(offset);
}

Point ScrollBarLayer::calcPos(cocos2d::extension::ScrollView *view, Point point){
    if(point.y < header_height){
        point.y = header_height;
    }
    if(point.y > footer_height + view->getViewSize().height){
        point.y = footer_height + view->getViewSize().height;
    }
    return Point(0, MIN(- view->getContentSize().height + view->getViewSize().height + ((point.y - header_height) * view->getContentSize().height / (footer_height - header_height)),0));
//    return Point(view->getPositionX(), - view->getContentSize().height + (point.y * view->getContentSize().height / view->getViewSize().height));
    
    
}

bool ScrollBarLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(touch->getLocation().x < view->getPosition().x + cocos2d::Node::getContentSize().width - BAR_FIX_SIZE*2 - getVisibleSpaceWidth() * 2)
    {
        return false;
    }
    listener->setSwallowTouches(true);
    Point point = touch->getLocationInView();
    point = calcPos(view, point);
    
    view->setContentOffset(point);
    refresh();

    return true;
}
void ScrollBarLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    Point point = touch->getLocationInView();
    point = calcPos(view, point);
    
    view->setContentOffset(point);
    refresh();

}
void ScrollBarLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    listener->setSwallowTouches(false);
    view->setBounceable(false);
    //0.1秒後スライド停止解除
    this->runAction(Sequence::create(DelayTime::create(0.1f),CallFunc::create([this](){view->setBounceable(true);}), NULL));
}
void ScrollBarLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

