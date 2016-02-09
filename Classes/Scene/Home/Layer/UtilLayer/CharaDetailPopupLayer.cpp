#include "CharaDetailPopupLayer.h"

CharaDetailPopupLayer* CharaDetailPopupLayer::create(int chara_list_id)
{
    return create(chara_list_id, true) ;
}

CharaDetailPopupLayer* CharaDetailPopupLayer::create(int chara_list_id, bool auto_remove)
{
    return create(chara_list_id, auto_remove, false) ;
}

CharaDetailPopupLayer* CharaDetailPopupLayer::create(int chara_list_id, bool auto_remove, bool hide_lock_button)
{
    CharaDetailPopupLayer *pRet = new CharaDetailPopupLayer();
    
    if (pRet && pRet->init(chara_list_id, auto_remove, hide_lock_button))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool CharaDetailPopupLayer::init(int chara_list_id, bool auto_remove, bool hide_lock_button)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaDetailPopupLayer::init()");
    
    _chara_list_id = chara_list_id ;
    _auto_remove = auto_remove;
    _hide_lock_button = hide_lock_button ;
    
    ratio = getVisibleRatio() ;
    
    return true;
}

void CharaDetailPopupLayer::setCharaListIdList(HmsVector<int> chara_list_id_list)
{
    _chara_list_id_list = chara_list_id_list;
}

void CharaDetailPopupLayer::setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel)
{
    _parent = parent;
    _sel_for_close = sel;
}

void CharaDetailPopupLayer::setSchedulerForChangeBase(Layer *parent, SEL_SCHEDULE sel)
{
    _parent = parent;
    _sel_for_change_base = sel;
}

void CharaDetailPopupLayer::initEventListener(){
    //レイヤーの後ろにタップ処理が伝搬しないように設定する
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        return true ;
    };
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void CharaDetailPopupLayer::createDetailSprite(){
    
    _chara_detail_sprite = CharaDetailSprite::create(_chara_list_id, _hide_lock_button);
    _chara_detail_sprite->setScale(0);
    _chara_detail_sprite->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    
    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button8.mp3");
    
    auto scale = ScaleTo::create(0.2f, 1.0*ratio);
    _chara_detail_sprite->runAction(scale);
    this->addChild(_chara_detail_sprite,500);
    
    //ウィンドウが閉じられたときに呼び出されるコールバックメソッドを登録
    SEL_SCHEDULE s = schedule_selector(CharaDetailPopupLayer::notifyClose);
    _chara_detail_sprite->setSchedulerForClose(this, s);
    
}

void CharaDetailPopupLayer::createDetailSprites(){
    _chara_detail_sprite_list = CharaDetailSpriteList::create(_chara_list_id_list, _chara_list_id, false);
    this->addChild(_chara_detail_sprite_list, 500, TAG_SHADOW);
    
    //ウィンドウが閉じられたときに呼び出されるコールバックメソッドを登録
    SEL_SCHEDULE sel_close = schedule_selector(CharaDetailPopupLayer::notifyClose);
    _chara_detail_sprite_list->setSchedulerForClose(this, sel_close);
    
    //base_indexが変更されたとき呼び出されるコールバックメソッドを登録
    SEL_SCHEDULE sel_change_base = schedule_selector(CharaDetailPopupLayer::notifyChangeBaseIndex);
    _chara_detail_sprite_list->setSchedulerForChangeBaseIndex(this, sel_change_base);
}

void CharaDetailPopupLayer::createShadowSprite(){
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setTag(TAG_SHADOW) ;
    shadow->setOpacity(0) ;
    shadow->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2), 0.5f, 0.5f)) ;
    shadow->runAction(FadeIn::create(0.2f)) ;
    this->addChild(shadow, 499);
}
    
void CharaDetailPopupLayer::onEnter(){
    Layer::onEnter() ;
    
    initEventListener() ;
    
    if(_chara_list_id_list.size()==0) {
        //キャラ詳細スプライト生成
        createDetailSprite() ;
    
        //影をかける
        createShadowSprite() ;
    } else {
        createDetailSprites();
    }
}

void CharaDetailPopupLayer::notifyClose(float dt){
    
    if(_parent!=nullptr && _sel_for_close!=nullptr) {
        _parent->scheduleOnce(_sel_for_close, 0.0f);
        _sel_for_close = nullptr;
    }
    
    if(_auto_remove == true){
        removeSelf(0.0f) ;
    }
}

void CharaDetailPopupLayer::notifyChangeBaseIndex(float dt){
    if(_parent!=nullptr && _sel_for_change_base!=nullptr) {
        _parent->scheduleOnce(_sel_for_change_base, 0.0f);
    }
}

void CharaDetailPopupLayer::removeSelf(float dt){
    CallFunc *remove= CallFunc::create([this]()
    {
        removeFromParent() ;
    });
    
    auto node = getChildByTag(TAG_SHADOW) ;
    node->runAction(Sequence::createWithTwoActions(FadeOut::create(0.2f), remove)) ;
}

bool CharaDetailPopupLayer::isLockStateChanged(){
    return _chara_detail_sprite->isLockStateChanged() ;
}

bool CharaDetailPopupLayer::getLockFlg(){
    return _chara_detail_sprite->getLockFlg() ;
}

int CharaDetailPopupLayer::getBaseIndex(){
    return _chara_detail_sprite_list->getBaseIndex() ;
}
