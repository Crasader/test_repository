#include "ScrollSprite.h"



ScrollSprite::ScrollSprite()
{
}

ScrollSprite* ScrollSprite::create(HmsVector<Sprite *> &item_list, const Size& view_size, ScrollDirection scroll_direction, bool stretch_mode)
{
    ScrollSprite *sprite = new ScrollSprite();
    if (sprite && sprite->init(item_list, view_size, scroll_direction, stretch_mode))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool ScrollSprite::init(HmsVector<Sprite *> &item_list, const Size& view_size, ScrollDirection scroll_direction, bool stretch_mode)
{
    if (!Sprite::init())
    {
        return false;
    }
    
    _scroll_direction = scroll_direction ;
    _inner_container_size = calcInnerContainerSize(item_list) ; //内容物のサイズを計算
    _scroll_view_size = view_size ;
    
    _container = Node::create() ;
    _container->setContentSize(_inner_container_size) ;
    _item_list = item_list ;
    
    //要素のスプライトをview_sizeに合わせて伸縮させる
    if(stretch_mode)stretchContents(item_list) ;
    
    //内容の追加
    addContents(item_list);
    
    this->setContentSize(_scroll_view_size) ;
    
    createScrollView() ;
    
    return true;
}

void ScrollSprite::stretchContents(HmsVector<Sprite *> &item_list)
{
    for(vector<Sprite *>::iterator iterator = item_list.begin(); iterator != item_list.end(); iterator++){
        Sprite *content = *iterator ;
        if(_scroll_direction==HORIZONTAL){
            content->setScale(_scroll_view_size.height / content->getContentSize().height) ;
        }else if(_scroll_direction == ScrollDirection::VERTICAL){
            content->setScale(_scroll_view_size.width / content->getContentSize().width) ;
        }
    }
}

void ScrollSprite::addContents(HmsVector<Sprite *> &item_list)
{
    Point sprite_position_offset = Point::ZERO;
    
    if(_scroll_direction==HORIZONTAL){
        for(vector<Sprite *>::iterator iterator = item_list.begin(); iterator != item_list.end(); iterator++){
            Sprite *content = *iterator ;
            content->setPosition(sprite_position_offset + Point(content->getContentSize().width/2 * content->getScaleX(), _scroll_view_size.height/2));
            _container->addChild(content) ;
            sprite_position_offset += Point(content->getContentSize().width * content->getScaleX(), 0) ;
        }
    }else if(_scroll_direction == ScrollDirection::VERTICAL){
        for(vector<Sprite *>::reverse_iterator iterator = item_list.rbegin(); iterator != item_list.rend(); iterator++){
            Sprite *content = *iterator ;
            content->setPosition(sprite_position_offset + Point(_scroll_view_size.width/2, content->getContentSize().height/2 * content->getScaleY()));
            _container->addChild(content) ;
            sprite_position_offset += Point(0, content->getContentSize().height * content->getScaleY()) ;
        }
    }
    
}

Size ScrollSprite::calcInnerContainerSize(HmsVector<Sprite *> &item_list)
{
    Size inner_container_size = Size::ZERO ;
    for(vector<Sprite *>::iterator iterator = item_list.begin(); iterator != item_list.end(); iterator++){
        Sprite *content = *iterator ;
        if(_scroll_direction == ScrollDirection::VERTICAL){
            inner_container_size.height += content->getContentSize().height * content->getScaleY() ;
            if(inner_container_size.width < content->getContentSize().width * content->getScaleX()){
                inner_container_size.width = content->getContentSize().width * content->getScaleX() ;
            }
        }else if(_scroll_direction == ScrollDirection::HORIZONTAL){
            inner_container_size.width += content->getContentSize().width * content->getScaleX() ;
            if(inner_container_size.height < content->getContentSize().height * content->getScaleY()){
                inner_container_size.height = content->getContentSize().height * content->getScaleY() ;
            }
        }
    }
    
    //containerの横幅がview_sizeの横幅を下回るとScrollViewの挙動がおかしくなるのでここで調整
    if(inner_container_size.width < _scroll_view_size.width){
        inner_container_size.width = _scroll_view_size.width ;
    }
    
    return inner_container_size ;
}

void ScrollSprite::createScrollView()
{
    _scroll_view = ScrollView::create(_scroll_view_size, _container);
    
    if(_scroll_direction == ScrollDirection::VERTICAL){
        _scroll_view->setDirection(ScrollView::Direction::VERTICAL);
    }else if(_scroll_direction == ScrollDirection::HORIZONTAL){
        _scroll_view->setDirection(ScrollView::Direction::HORIZONTAL);
    }
    
    _scroll_view->setBounceable(true);
    _scroll_view->setDelegate(this);
    if(_scroll_direction == ScrollSprite::VERTICAL){
        _scroll_view->setContentOffset(Point(0, _scroll_view_size.height -_container->getContentSize().height)) ; //表示された時点で左上が見えるようにしておく(デフォルトの_content_offsetはPoint(0,0)で左下が見える状態となっている)
    }
    this->addChild(_scroll_view, O_SCROLL_VIEW, T_SCROLL_VIEW);
}

void ScrollSprite::scrollViewDidScroll(cocos2d::extension::ScrollView *view)
{
}

void ScrollSprite::scrollViewDidZoom(cocos2d::extension::ScrollView *view)
{
}

void ScrollSprite::onEnter()
{
    Sprite::onEnter();
}

void ScrollSprite::contentOffsetChangeFromOffsetDistance(float distance)
{
    if(_scroll_direction == ScrollSprite::VERTICAL){
        _scroll_view->setContentOffset(Point(0, _scroll_view_size.height -_container->getContentSize().height + distance),true) ; //表示された時点で左上が見えるようにしておく(デフォルトの_content_offsetはPoint(0,0)で左下が見える状態となっている)
    }
}