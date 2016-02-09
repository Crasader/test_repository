#include "BannerContentsSlideSprite.h"

BannerContentsSlideSprite::BannerContentsSlideSprite()
:_banner_size(0)
{
    
}

BannerContentsSlideSprite* BannerContentsSlideSprite::create(HmsVector<Sprite*> contents)
{
    return create(contents, ScrollDirection::HORIZONTAL) ;
}

BannerContentsSlideSprite* BannerContentsSlideSprite::create(HmsVector<Sprite*> contents, ScrollDirection direction)
{
    BannerContentsSlideSprite *sprite = new BannerContentsSlideSprite();
    
    if (sprite && sprite->init(contents, direction))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool BannerContentsSlideSprite::init(HmsVector<Sprite*> contents, ScrollDirection direction)
{
    //最初の要素をスプライトのサイズとする
    if (!Sprite::initWithSpriteFrame(contents[0]->getSpriteFrame()))
    {
        return false;
    }
    HMSLOG("BannerContentsSlideSprite::init()");
    this->setOpacity(0);
    
    _scroll_direction = direction ;
    
    //バナーの種類を格納
    setBannerSize(contents.size()) ;
    
    
    //バナーが二つ以下のときは両サイドにスプライトが表示されるように二つスプライトを複製する
    if(1 <= contents.size() && contents.size() <= 2){
        contents.push_back(Sprite::createWithSpriteFrame(contents[0]->getSpriteFrame()));
        contents.push_back(Sprite::createWithSpriteFrame(contents[1]->getSpriteFrame()));
    }
    
    _contents = contents;
    
    _center_contents_index = 0 ;
    _now_auto_swipe = false ;
    
    return true;
}

int BannerContentsSlideSprite::getCenterIndex() {
    return _center_contents_index ;
}

void BannerContentsSlideSprite::updateContentsPosition(float dt){
    
    //すべてのデッキを見えないようにする
    for(int i=0; i<_contents.size(); i++){
        _contents[i]->setVisible(false);
    }
    
    int prev_contents_index = calcPrevContentsIndex(_center_contents_index) ;
    int next_contents_index = calcNextContentsIndex(_center_contents_index) ;
    
    //中心と左右に位置するカードを可視化する
    _contents[_center_contents_index]->setVisible(true);
    _contents[prev_contents_index]->setVisible(true);
    _contents[next_contents_index]->setVisible(true);
    
    _contents[_center_contents_index]->setPosition(_center_contents_point) ;
    _contents[prev_contents_index]->setPosition(_prev_contents_point) ;
    _contents[next_contents_index]->setPosition(_next_contents_point) ;
    
    _now_auto_swipe = false ;
}


void BannerContentsSlideSprite::autoSwipeCenter()
{
    //スワイプ中、又は、バナーの個数が１つだったら移動させない
    if(_now_auto_swipe || _banner_size == 1)return ;
    _now_auto_swipe = true;
    
    //移動時間
    float auto_swipe_time = 0.3f ;
    
    int prev_contents_index = calcPrevContentsIndex(_center_contents_index) ;
    int next_contents_index = calcNextContentsIndex(_center_contents_index) ;
    
    //中心のデッキを中心に戻す
    MoveTo *move_center = MoveTo::create(auto_swipe_time, _center_contents_point) ;
    _contents[_center_contents_index]->runAction(move_center) ;
    
    //右側のデッキを右側に戻す
    MoveTo *move_next = MoveTo::create(auto_swipe_time, _next_contents_point) ;
    _contents[next_contents_index]->runAction(move_next) ;
    
    //左側のデッキを左側に戻す
    MoveTo *move_prev = MoveTo::create(auto_swipe_time, _prev_contents_point) ;
    _contents[prev_contents_index]->runAction(move_prev) ;
    
    //スワイプ終了後 中心のカードに応じて選択肢を所定の位置に変更する
    this->scheduleOnce(schedule_selector(BannerContentsSlideSprite::updateContentsPosition), auto_swipe_time + 0.5f);
    
}

void BannerContentsSlideSprite::autoSwipePrev()
{
    //スワイプ中、又は、バナーの個数が１つだったら移動させない
    if(_now_auto_swipe || _banner_size == 1)return ;
    _now_auto_swipe = true;
    
    float auto_swipe_time = 0.3f ;
    
    int next_contents_index = calcNextContentsIndex(_center_contents_index) ;
    
    //右側のデッキを中心に移動
    MoveTo *move_center = MoveTo::create(auto_swipe_time, _center_contents_point) ;
    _contents[next_contents_index]->runAction(move_center) ;
    
    //中心のデッキを左側に移動
    MoveTo *move_prev = MoveTo::create(auto_swipe_time, _prev_contents_point) ;
    _contents[_center_contents_index]->runAction(move_prev) ;
    
    //中心のカードインデックスをスワイプ前の右側に位置するカードのインデックスに変更する
    _center_contents_index = next_contents_index ;
    
    //スワイプ終了後 中心のカードに応じて選択肢を所定の位置に変更する
    this->scheduleOnce(schedule_selector(BannerContentsSlideSprite::updateContentsPosition), auto_swipe_time + 0.5f);
    
}

void BannerContentsSlideSprite::autoSwipeNext()
{
    //スワイプ中、又は、バナーの個数が１つだったら移動させない
    if(_now_auto_swipe || _banner_size == 1)return ;
    _now_auto_swipe = true;
    
    float auto_swipe_time = 0.3f ;
    
    int prev_contents_index = calcPrevContentsIndex(_center_contents_index) ;
    
    //左側のデッキを中心に移動
    MoveTo *move_center = MoveTo::create(auto_swipe_time, _center_contents_point) ;
    _contents[prev_contents_index]->runAction(move_center) ;
    
    //中心のデッキを右側に移動
    MoveTo *move_next = MoveTo::create(auto_swipe_time, _next_contents_point) ;
    _contents[_center_contents_index]->runAction(move_next) ;
    
    //中心のカードインデックスをスワイプ前の右側に位置するカードのインデックスに変更する
    _center_contents_index = prev_contents_index ;
    
    //スワイプ終了後 中心のカードに応じて選択肢を所定の位置に変更する
    this->scheduleOnce(schedule_selector(BannerContentsSlideSprite::updateContentsPosition), auto_swipe_time + 0.5f);
    
}

void BannerContentsSlideSprite::swipeEndSound(){
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/gacha_banner_slide.mp3",false);
}

//与えたのVec2の上下か左右に移動する, HORIZONTALの場合x成分、VERTICALの場合y成分の分移動する
void BannerContentsSlideSprite::moveDelta(const Vec2& deltaVec2){
    //スワイプ中、又は、バナーの個数が１つだったら移動させない
    if(_now_auto_swipe || _banner_size == 1)return ;
    
    for(int i=0; i<_contents.size(); i++){
        if(_scroll_direction == ScrollDirection::HORIZONTAL){
            _contents[i]->setPositionX(_contents[i]->getPositionX() + deltaVec2.x) ;
        }else if(_scroll_direction == ScrollDirection::VERTICAL){
            _contents[i]->setPositionY(_contents[i]->getPositionY() + deltaVec2.y) ;
        }
    }
}


int BannerContentsSlideSprite::calcPrevContentsIndex(int center_index){
    int prev_contents_index = (center_index - 1) % _contents.size();
    if(prev_contents_index<0)prev_contents_index = prev_contents_index + _contents.size() ;
    return prev_contents_index ;
}

int BannerContentsSlideSprite::calcNextContentsIndex(int center_index){
    int next_contents_index = (center_index + 1) % _contents.size() ;
    return next_contents_index ;
}

void BannerContentsSlideSprite::onEnter(){
    Sprite::onEnter() ;
    
    //contentsの内容が停止する位置の初期化
    initContentsPoint() ;
    
    //マスク処理をするクリッピングノードの生成
    initClippingNode() ;
    
    //スプライトをクリッピングノードにaddChild
    initContents() ;
}

void BannerContentsSlideSprite::initContentsPoint(){
    Size contents_size = getContentSize() ;
    
    _center_contents_point = Point::ZERO ;
    
    if(_scroll_direction == ScrollDirection::HORIZONTAL){
         _prev_contents_point = -Vec2(contents_size.width, 0) ;
         _next_contents_point = Vec2(contents_size.width, 0) ;
    }else if(_scroll_direction == ScrollDirection::VERTICAL){
         _prev_contents_point = -Vec2(0,contents_size.height) ;
         _next_contents_point = Vec2(0,contents_size.height) ;
    }
}

void BannerContentsSlideSprite::initClippingNode() {
    Rect rect = Rect(0, 0, getContentSize().width, getContentSize().height);
    Sprite* mask = Sprite::create();
    mask->setTextureRect(rect);
    mask->setAnchorPoint(Point(0,0));
    
	_clipper = ClippingNode::create();
	_clipper->setStencil(mask);
	_clipper->setAlphaThreshold(0.01f);
	this->addChild(_clipper);
}

void BannerContentsSlideSprite::initContents() { 
    for(int i=0 ; i<_contents.size(); i++){
        _contents[i]->setScale(1);
        _contents[i]->setPosition(Point::ZERO);
        _contents[i]->setAnchorPoint(Point(0,0));
        _clipper->addChild(_contents[i]) ;
    }
    updateContentsPosition(0.0f) ;
}
