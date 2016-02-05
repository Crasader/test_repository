#include "DeckPackSelectLayer.h"

bool DeckPackSelectLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("DeckPackSelectLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void DeckPackSelectLayer::onEnter()
{
    Layer::onEnter();
    
    //中心のデッキのインデックス
    _centerDeckIndex = 0 ;
    
    //選択するデッキの中心位置
    _deckSelectCenterPosition = Point(320, 500) ;
    
    //説明画像の中心位置のy座標
    _descriptionCenterPosition_y = 400 ; //説明画像中心点のy座標
    
    //説明画像の中心位置のx座標(デッキ毎で表示位置が異なるため全て配列として格納する）
    _descriptionCenterPosition_x_list[0] = 224 ;
    _descriptionCenterPosition_x_list[1] = 224 ;
    _descriptionCenterPosition_x_list[2] = 350;
    _descriptionCenterPosition_x_list[3] = 300 ;
    _descriptionCenterPosition_x_list[4] = 311 ;
    
    //デッキ名の画像の中心位置 (画面上端から120ピクセル)
    _decknameCenterPosition = Point(320, DISPLAY_HEIGHT-120) ;
    //デッキ名の画像が隠れる 中心位置
    _decknameHiddenCenterPosition = Point(320, 1400) ;
    
    //イベントリスナーの作成
    createEventLitener();
    
    //上部のヘッダー生成
    initHeaderSprite() ;
    
    //デッキ名の生成
    initDeckNameSprite() ;
    
    //スターターデッキ関係のスプライトの生成
    initDeckSelectSprite();
    
    //矢印のスプライトの生成
    initArrowSprite() ;
    
    //説明画像の生成
    initDescriptionSprite() ;
    
    //選択ボタン周りの生成
    initSelectButtonWindow() ;
    
    //矢印の繰り返しのアクション
    repeatArrowsAction() ;
    
    //中心のカードに応じて選択肢を所定の位置に変更する
    updatePositionForSelectDeck(0.0f) ;
}

void DeckPackSelectLayer::createEventLitener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(DeckPackSelectLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckPackSelectLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckPackSelectLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

ClippingNode *DeckPackSelectLayer::createMascByTargetSprite(Node *masc_target_node)
{
    //与えられたNodeのサイズの大きさのマスクを生成する
    Size deck_size = masc_target_node->getContentSize() ;
    
    //マスクの形状を取得する
    DrawNode *shape = this->getMascShapeByTargetSprite(masc_target_node);

    ClippingNode *clipper = ClippingNode::create();
    clipper->setAnchorPoint(Vec2(0.5, 0.5));
    clipper->setPosition(convertFromDisplaySize(_deckSelectCenterPosition, 0.5, 1));
    clipper->setStencil(shape);
    clipper->setScale(ratio);

    return clipper ;
}

DrawNode* DeckPackSelectLayer::getMascShapeByTargetSprite(Node *masc_target_node)
{
    float deck_width = masc_target_node->getContentSize().width ;
    float deck_height = masc_target_node->getContentSize().height;

    DrawNode *shape = DrawNode::create();
    static Vec2 square[4];
    square[0] = Vec2(-deck_width/2, -deck_height/2);
    square[1] = Vec2(deck_width/2, -deck_height/2);
    square[2] = Vec2(deck_width/2, deck_height/2);
    square[3] = Vec2(-deck_width/2, deck_height/2);
    
    static Color4F green(0, 1, 0, 1);
    shape->drawPolygon(square, 4, green, 0, green);
    return shape;
}

void DeckPackSelectLayer::initHeaderSprite(){
    //ヘッダー画像の生成
    Sprite *header = Sprite::create("init/Tutorial/deck_select_header.png");
    header->setScale(ratio) ;
    header->setAnchorPoint(Point(0.5, 1)) ;
    header->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT), 0.5, 1.0));
    this->addChild(header, 1) ;
    
    //ラベルの生成 (header画像の下端から73ピクセル)
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument("スターターデッキ選択", FONT_NAME_SYSTEM, 47);
    label->setPosition(Point(header->getContentSize().width/2, 73));
    
    //アウトラインと影は今回のみ普通より太めにする
//enableOutline(Color4B::BLACK,3); //注意 太めだよ
    label->enableShadow(Color4B(0, 0, 0, 255), Size(3, -3), 3); //注意 太めだよ
    header->addChild(label, 1);
}

void DeckPackSelectLayer::initDeckNameSprite(){
    //デッキの説明についてのスプライトを生成して追加
    for(int i=0; i<STARTER_DECK_ARRAY_SIZE; i++){
        Sprite *deck_name = Sprite::create("init/Tutorial/deck_select_title"+toString<int>(i+1)+".png");
        deck_name->setScale(ratio) ;
        deck_name_sprite_list[i] = deck_name;
        
        if(i==0){ //最初のデッキだけ初期位置
            deck_name_sprite_list[i]->setPosition(convertFromDisplaySize(Point(_decknameCenterPosition), 0.5, 1));
        }else{
            deck_name_sprite_list[i]->setPosition(convertFromDisplaySize(Point(_decknameHiddenCenterPosition), 0.5, 1));
        }
        
        this->addChild(deck_name_sprite_list[i], 2) ;
    }
    
}
void DeckPackSelectLayer::initDeckSelectSprite()
{
    //選択式のデッキについて 
    for(int i=0; i<STARTER_DECK_ARRAY_SIZE; i++){
        //デッキの背景
        Sprite *deck = Sprite::create("init/Tutorial/deck_select_bk"+toString<int>(i+1)+".png");
        deck_select_bk_list[i] = deck ;
        
        //デッキのキャラ画像
        Sprite *chara= Sprite::create("init/Tutorial/deck_select_chara"+toString<int>(i+1)+".png");
        chara->setPosition(deck_select_bk_list[i]->getContentSize()/2) ;
        deck_select_bk_list[i]->addChild(chara) ;
    }
    
    //マスクのClippingNodeを生成する
    ClippingNode *masc = createMascByTargetSprite(deck_select_bk_list[0]) ;
        
    for(int i=0; i<STARTER_DECK_ARRAY_SIZE; i++){
        //マスクにスプライトを追加
        masc->addChild(deck_select_bk_list[i]) ;
    }
    
    this->addChild(masc);
}

void DeckPackSelectLayer::initArrowSprite(){
    
    Size back_size = deck_select_bk_list[0]->getContentSize() ;
    float helf_back_size = back_size.width/2 ;
    float arrow_left_point = _deckSelectCenterPosition.x - helf_back_size ;
    float arrow_right_point = _deckSelectCenterPosition.x + helf_back_size ;
    float arrow_y = _deckSelectCenterPosition.y + 60 ;
    
    //左の矢印
    Sprite *left_arrow = Sprite::create("init/Tutorial/arrow_left_big.png");
    left_arrow->setScale(ratio);
    left_arrow->setAnchorPoint(Point(1/3, 0.5)) ;
    left_arrow->setPosition(convertFromDisplaySize(Point(arrow_left_point, arrow_y), 0.5, 1));
    this->addChild(left_arrow, 3, TAG_LEFT_ARROW) ;
    
    //右の矢印
    Sprite *right_arrow = Sprite::create("init/Tutorial/arrow_right_big.png");
    right_arrow->setScale(ratio);
    right_arrow->setAnchorPoint(Point(3/2, 0.5)) ;
    right_arrow->setPosition(convertFromDisplaySize(Point(arrow_right_point, arrow_y), 0.5, 1));
    this->addChild(right_arrow, 3, TAG_RIGHT_ARROW) ;
}

void DeckPackSelectLayer::initDescriptionSprite()
{
    //デッキの説明についてのスプライトを生成して追加
    for(int i=0; i<STARTER_DECK_ARRAY_SIZE; i++){
        Sprite *description = Sprite::create("init/Tutorial/deck_select_balloon"+toString<int>(i+1)+".png");
        description_sprite_list[i] = description;
        if(i==0){ //最初のデッキだけ表示しておく
            description_sprite_list[i]->setScale(ratio);
        }else{
            description_sprite_list[i]->setScale(0);
            description_sprite_list[i]->setOpacity(0);
        }
        description_sprite_list[i]->setPosition(convertFromDisplaySize(Point(_descriptionCenterPosition_x_list[i], _descriptionCenterPosition_y), 0.5, 0.5));
		
        this->addChild(description_sprite_list[i]) ;
    }
}

void DeckPackSelectLayer::initSelectButtonWindow(){
    
    //ベースとなるウィンドウを生成する(画面下端から40ピクセル)
    Sprite *window = Sprite::create("init/Tutorial/popup_select.png");
    window->setScale(ratio);
    window->setAnchorPoint(Point(0.5, 0)) ;
    window->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0.0));
    this->addChild(window, 3) ;
    
    //ラベルの生成 (下から3/4の位置が中心位置)
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument("デッキを選択してください", FONT_NAME_SYSTEM, 38);
    label->setPosition(Point(window->getContentSize().width/2, window->getContentSize().height*3/4)) ;
//enableOutline(Color4B::BLACK,1);
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    window->addChild(label, 1);
    
    //ボタン関係を生成する(下から60ピクセルが中心位置)
    Sprite *btn_off = Sprite::create("init/Tutorial/bt_deck_select.png");
    Sprite *btn_on = Sprite::create("init/Tutorial/bt_deck_select.png");
    btn_on->setColor(Color3B(180, 180, 180));
    
    MenuItemSprite *battle_btn = MenuItemSprite::create(btn_off, btn_on, CC_CALLBACK_1(DeckPackSelectLayer::changePage, this));
    battle_btn->setPosition(Point(window->getContentSize().width/2, 60)) ;
    Menu *menu = Menu::create(battle_btn, nullptr);
    menu->setPosition(Point::ZERO) ;
    window->addChild(menu, 1);
    
}
void DeckPackSelectLayer::updatePositionForSelectDeck(float dt) {
    
    //すべてのデッキを見えないようにする
    for(int i=0; i<STARTER_DECK_ARRAY_SIZE; i++){
        deck_select_bk_list[i]->setVisible(false);
    }
    
    int leftDeckIndex = calcLeftDeckIndex(_centerDeckIndex) ;
    int rightDeckIndex = calcRightDeckIndex(_centerDeckIndex) ;
    
    //中心と左右に位置するカードを可視化する
    deck_select_bk_list[_centerDeckIndex]->setVisible(true);
    deck_select_bk_list[leftDeckIndex]->setVisible(true);
    deck_select_bk_list[rightDeckIndex]->setVisible(true);
    
    Size deck_size = deck_select_bk_list[0]->getContentSize() ;
    Point center_deck_point = Point::ZERO ;
    Point left_deck_point = -Vec2(deck_size.width, 0) ;
    Point right_deck_point = Vec2(deck_size.width, 0) ;
    
    deck_select_bk_list[_centerDeckIndex]->setPosition(center_deck_point) ;
    deck_select_bk_list[leftDeckIndex]->setPosition(left_deck_point) ;
    deck_select_bk_list[rightDeckIndex]->setPosition(right_deck_point) ;
    
    //タップ可能にする
    setEnableTap(true) ;
}

void DeckPackSelectLayer::finalizationSprite(){
    removeAllChildren() ;
}

void DeckPackSelectLayer::autoSwipeLeft()
{
    float auto_swipe_time = 0.3f ;
    
    int rightDeckIndex = calcRightDeckIndex(_centerDeckIndex) ;
    
    Size deck_size = deck_select_bk_list[0]->getContentSize() ;
    Point center_deck_point = Point::ZERO ;
    Point left_deck_point = -Vec2(deck_size.width, 0) ;
    
    //右側のデッキを中心に移動
    MoveTo *move_center = MoveTo::create(auto_swipe_time, center_deck_point) ;
    deck_select_bk_list[rightDeckIndex]->runAction(move_center) ;
    
    //中心のデッキを左側に移動
    MoveTo *move_left = MoveTo::create(auto_swipe_time, left_deck_point) ;
    deck_select_bk_list[_centerDeckIndex]->runAction(move_left) ;
    
    //中心のカードインデックスをスワイプ前の右側に位置するカードのインデックスに変更する
    _centerDeckIndex = rightDeckIndex ;
    
    //スワイプ終了後 中心のカードに応じて選択肢を所定の位置に変更する
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::updatePositionForSelectDeck), auto_swipe_time);
    
    //スワイプ終了後 説明画像のポップアップ
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::popupDescriptionAction), auto_swipe_time);
    
    //スワイプ終了後 タイトル画像をおろす
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::downDeckNameAction), auto_swipe_time);
    
    //スワイプ終了後 矢印を消す
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::showArrows), auto_swipe_time);
}

void DeckPackSelectLayer::autoSwipeRight()
{
    float auto_swipe_time = 0.3f ;
    
    int leftDeckIndex = calcLeftDeckIndex(_centerDeckIndex) ;
    
    Size deck_size = deck_select_bk_list[0]->getContentSize() ;
    Point center_deck_point = Point::ZERO ;
    Point right_deck_point = Vec2(deck_size.width, 0) ;
    
    //左側のデッキを中心に移動
    MoveTo *move_center = MoveTo::create(auto_swipe_time, center_deck_point) ;
    deck_select_bk_list[leftDeckIndex]->runAction(move_center) ;
    
    //中心のデッキを右側に移動
    MoveTo *move_right = MoveTo::create(auto_swipe_time, right_deck_point) ;
    deck_select_bk_list[_centerDeckIndex]->runAction(move_right) ;
    
    //中心のカードインデックスをスワイプ前の右側に位置するカードのインデックスに変更する
    _centerDeckIndex = leftDeckIndex ;
    
    //スワイプ終了後 中心のカードに応じて選択肢を所定の位置に変更する
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::updatePositionForSelectDeck), auto_swipe_time);
    
    //スワイプ終了後 説明画像のポップアップ
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::popupDescriptionAction), auto_swipe_time);
    
    //スワイプ終了後 タイトル画像をおろす
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::downDeckNameAction), auto_swipe_time);
    
    //スワイプ終了後 矢印を消す
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::showArrows), auto_swipe_time);
}

void DeckPackSelectLayer::autoSwipeCenter()
{
    //移動時間
    float auto_swipe_time = 0.3f ;
    
    int leftDeckIndex = calcLeftDeckIndex(_centerDeckIndex) ;
    int rightDeckIndex = calcRightDeckIndex(_centerDeckIndex) ;
    
    Size deck_size = deck_select_bk_list[0]->getContentSize() ;
    Point center_deck_point = Point::ZERO ;
    Point left_deck_point = -Vec2(deck_size.width, 0) ;
    Point right_deck_point = Vec2(deck_size.width, 0) ;
    
    //中心のデッキを中心に戻す
    MoveTo *move_center = MoveTo::create(auto_swipe_time, center_deck_point) ;
    deck_select_bk_list[_centerDeckIndex]->runAction(move_center) ;
    
    //右側のデッキを右側に戻す
    MoveTo *move_right = MoveTo::create(auto_swipe_time, right_deck_point) ;
    deck_select_bk_list[rightDeckIndex]->runAction(move_right) ;
    
    //左側のデッキを左側に戻す
    MoveTo *move_left = MoveTo::create(auto_swipe_time, left_deck_point) ;
    deck_select_bk_list[leftDeckIndex]->runAction(move_left) ;
    
    //スワイプ終了後 中心のカードに応じて選択肢を所定の位置に変更する
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::updatePositionForSelectDeck), auto_swipe_time);
    
    //スワイプ終了後 説明画像のポップアップ
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::popupDescriptionAction), auto_swipe_time);
    
    //スワイプ終了後 タイトル画像をおろす
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::downDeckNameAction), auto_swipe_time);
    
    //スワイプ終了後 矢印を消す
    this->scheduleOnce(schedule_selector(DeckPackSelectLayer::showArrows), auto_swipe_time);
}

bool DeckPackSelectLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    //タップ不可のとき他のonTouch~に処理を伝搬させずに終了
    if(!getEnableTap())return false ;
    
    
    //説明画像を消すActionの再生
    deletePopupDescriptionAction() ;
    
    //タイトル画像を消すActionの再生
    upDeckNameAction() ;
    
    //矢印を見えなくする
    hideArrows() ;
    
    if(checkContainTouchPointByTag(touch->getLocation(), TAG_RIGHT_ARROW)){
        //タップした位置が 右の矢印だった場合
        setEnableTap(false) ; //タップ不可能にする
        autoSwipeLeft();
        return false ;
    }else if(checkContainTouchPointByTag(touch->getLocation(), TAG_LEFT_ARROW)){
        //タップした位置が 左の矢印だった場合
        setEnableTap(false) ; //タップ不可能にする
        autoSwipeRight();
        return false ;
    }
    
    //ここで選択肢を触っているか,タップ可能か判定
    if(checkContainDeckPosition(touch->getLocation())){
        return true ;
    }else{
        return false ; //ここでfalseを返した時点で onTouchMoved, onTouchEndedに伝搬することはない
    }
    
}

void DeckPackSelectLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    //前回のonTouchMovedからの変化量を取得する 選択肢はマスクに依存していて、マスクがratio倍されているので、ratioで割る
    float add_delta_x =  touch->getDelta().x / ratio;
    
    //前回からの変化量分 移動する
    for(int i=0; i<STARTER_DECK_ARRAY_SIZE; i++){
        Point before_point = deck_select_bk_list[i]->getPosition();
        deck_select_bk_list[i]->setPosition(before_point + Point(add_delta_x, 0)) ;
    }
}

void DeckPackSelectLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    //最初のタップからの移動距離を求める
    float diff_x = touch->getLocation().x - touch->getStartLocation().x ;
    
    //画面サイズの横幅
    float visibleWidth = Director::getInstance()->getVisibleSize().width;
    
    if(diff_x >= visibleWidth/3){
        // 画面の 1/3 以上移動していたら、右にスワイプされたものと見なす
        setEnableTap(false) ; //タップ不可能にする
        autoSwipeRight() ;
    }else if(diff_x <= -visibleWidth/3){
        // 画面の 1/3 以上移動していたら、左にスワイプされたものと見なす
        setEnableTap(false) ; //タップ不可能にする
        autoSwipeLeft() ;
    }else{
        //他の条件に当てはまらない場合、画像を中心位置に戻す
        setEnableTap(false) ; //タップ不可能にする
        autoSwipeCenter() ;
    }
	//SE
	CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/deck_select.mp3");
}

void DeckPackSelectLayer::changePage(Ref *ref){
	if(getEnableTap()){
        //選択した属性の保存
        SaveData::saveString("generalType", toString<int>(_centerDeckIndex+1));
        
        //ページの切り替え
        TutorialScene *tutorial_scene = (TutorialScene *)this->getParent();
        LastConfirmLayer *next_layer = LastConfirmLayer::create();
        tutorial_scene->changePage(next_layer);
		//SE
		CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/deck_select.mp3");
    }
}

//選択肢を触っているか判定
bool DeckPackSelectLayer::checkContainDeckPosition(Point touch_point){
    return true ;
}

//タップ可能か   タップしてから離してリストの位置が定位置にいくまでの間false
bool DeckPackSelectLayer::getEnableTap(){
    return _enable_tap;
}

void DeckPackSelectLayer::setEnableTap(bool b){
    _enable_tap = b;
}

int DeckPackSelectLayer::calcLeftDeckIndex(int center_index){
    int leftDeckIndex = (center_index - 1) % STARTER_DECK_ARRAY_SIZE ;
    if(leftDeckIndex<0)leftDeckIndex = leftDeckIndex + STARTER_DECK_ARRAY_SIZE ;
    return leftDeckIndex ;
}

int DeckPackSelectLayer::calcRightDeckIndex(int center_index){
    int rightDeckIndex = (center_index + 1) % STARTER_DECK_ARRAY_SIZE ;
    return rightDeckIndex ;
}


void DeckPackSelectLayer::popupDescriptionAction(float dt)
{
    //アニメーション生成
    FadeIn *fadeIn = FadeIn::create(0.1f) ;
    ScaleTo *scaleTo = ScaleTo::create(1.0f, ratio) ;
    EaseElasticOut *move_ease_out = EaseElasticOut::create(scaleTo);
    
    Spawn *popup = Spawn::createWithTwoActions(fadeIn, move_ease_out) ;
    description_sprite_list[_centerDeckIndex]->runAction(popup) ;
}

void DeckPackSelectLayer::deletePopupDescriptionAction()
{
    //前回のポップアップのアクションの途中だった場合, そのアクションを停止する
    description_sprite_list[_centerDeckIndex]->stopAllActions() ;
    
    FadeOut *fadeOut = FadeOut::create(0.1f) ;
    ScaleTo *scaleTo = ScaleTo::create(0.1f, 0) ;
    
    Spawn *popup_delete = Spawn::createWithTwoActions(fadeOut, scaleTo) ;
    description_sprite_list[_centerDeckIndex]->runAction(popup_delete) ;
}

void DeckPackSelectLayer::downDeckNameAction(float dt)
{
    //アニメーション生成
    MoveTo *move = MoveTo::create(0.2f, convertFromDisplaySize(_decknameCenterPosition, 0.5f, 1)) ;
    
    EaseSineOut *move_ease_out = EaseSineOut::create(move);
    deck_name_sprite_list[_centerDeckIndex]->runAction(move_ease_out) ;
}

void DeckPackSelectLayer::upDeckNameAction()
{
    //前回の降りて来るアクションの途中だった場合, そのアクションを停止する
    deck_name_sprite_list[_centerDeckIndex]->stopAllActions() ;
    
    MoveTo *move = MoveTo::create(0.1f, convertFromDisplaySize(_decknameHiddenCenterPosition, 0.5f, 1)) ;
    deck_name_sprite_list[_centerDeckIndex]->runAction(move) ;
}

void DeckPackSelectLayer::repeatArrowsAction(){
    Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
    Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
    
    MoveBy *left_arrow_move = MoveBy::create(0.5f, Point(-20,0)) ;
    ActionEase *left_arrow_move_ease_in = EaseExponentialIn::create(left_arrow_move);
    ActionEase *left_arrow_move_ease_in_back = left_arrow_move_ease_in->reverse();
    MoveTo *left_arrow_move_default = MoveTo::create(0.01, left_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *left_arrow_forever_action = RepeatForever::create(Sequence::create(left_arrow_move_ease_in, left_arrow_move_ease_in_back,left_arrow_move_default, nullptr)) ;
 
    MoveBy *right_arrow_move = MoveBy::create(0.5f, Point(+20,0)) ;
    ActionEase *right_arrow_move_ease_in = EaseExponentialIn::create(right_arrow_move);
    ActionEase *right_arrow_move_ease_in_back = right_arrow_move_ease_in->reverse();
    MoveTo *right_arrow_move_default = MoveTo::create(0.01, right_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *right_arrow_forever_action = RepeatForever::create(Sequence::create(right_arrow_move_ease_in, right_arrow_move_ease_in_back, right_arrow_move_default, nullptr)) ;
    
    left_arrow->runAction(left_arrow_forever_action) ;
    right_arrow->runAction(right_arrow_forever_action) ;
}

void DeckPackSelectLayer::showArrows(float dt){
    Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
    Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
    
    left_arrow->setVisible(true) ;
    right_arrow->setVisible(true) ;
}

void DeckPackSelectLayer::hideArrows(){
    Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
    Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
    
    left_arrow->setVisible(false) ;
    right_arrow->setVisible(false) ;
}

bool DeckPackSelectLayer::checkContainTouchPointByTag(Point touch_point, int tag){
    Node *target_sprite = getChildByTag(tag) ;
    
    Rect targetBox = target_sprite->getBoundingBox();
    
    //touchPointがtargetBoxの中に含まれているか判定
    if (targetBox.containsPoint(touch_point)) return true;
    else return false ;
}