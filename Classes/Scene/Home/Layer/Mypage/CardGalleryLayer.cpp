#include "CardGalleryLayer.h"


CardGalleryLayer::CardGalleryLayer()
:_float_time(0.0)
,_show_chara_detail_flg (false)
{
}

bool CardGalleryLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CardGalleryLayer::init()");
    
    ratio = getVisibleRatio();
    show_detail_detect_distance = 50;
    
    _card_center_position = Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2) ;
    _scroll_detect_speed = 0.1f ;
    
    return true;
}

void CardGalleryLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CardGalleryLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CardGalleryLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardGalleryLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardGalleryLayer::setupUserDeckList()
{
    //現在のパーティナンバーの取得
    _party_no = SaveData::loadString("party_no");
    
    //ユーザデッキリストの読み込み
    string result = SaveData::loadString("user_party_list");
    string err;
    Json json = Json::parse(result, err);
    
    int list_no = fromString<int>(_party_no) - 1;
    _user_deck_list = json[toString(list_no)].object_items();
}


void CardGalleryLayer::onEnter()
{
    Layer::onEnter();
    
    _card_state_decide_number = 9.0f ;
    _pre_sound_index = (int)_card_state_decide_number ;
    
    //イベントリスナーの作成
    createEventListener();
    
    //デッキリストの読み込み
    setupUserDeckList();
    
    //カード画像の初期化
    initCards();
    
    //カード位置ののアップデート
    updateCardPositionAndRotate() ;
    
    //カード浮遊アニメーションのスケジュール
    this->schedule(schedule_selector(CardGalleryLayer::floatAnimationUpdate), 0.01f);
    
}

void CardGalleryLayer::floatAnimationUpdate(float dt){
    _float_time += dt ;
    for(int i=0; i<CARD_ARRAY_SIZE; i++) {
        float floatEffectRate = _floatEffectRate[i] ;
        if(floatEffectRate>0){//計算量を節約するため、浮遊の度合いが0より大きいもの以外は計算しない
            float float_distance = calcFloatDistanceByTime(_float_time) ;
            _cards[i]->setPositionY(float_distance*floatEffectRate*ratio) ;
            _reflection_cards[i]->setPositionY(-2*float_distance*floatEffectRate*ratio) ;
        }
    }
    
}


float CardGalleryLayer::calcFloatDistanceByTime(float time) {
    float x = time ;
    float answer = f7(x) ;
    return answer ;
}

float CardGalleryLayer::f7(float x){
    float y = (sin(x) * 1 + 1)*2.5;
    return y;
}



void CardGalleryLayer::initCards()
{
    HMSLOG("json_dump=%s", _user_deck_list.dump().c_str());
    
    _nodes.clear();
	_cards.clear();
	_reflection_cards.clear();
    _card_names.clear();
	_card_frame.clear();
	_card_rare.clear();
	
    
    //初期カードの作成
    for(int num=1; num<=CARD_ARRAY_SIZE; num++) {
        string cardId = _user_deck_list["chara_id" + toString(num)].string_value();
        
        string card_pass = "download/Chara/parts400/card_bk1_reverse.png";
        if (cardId!="0") {
            card_pass = "download/Chara/all400/" + cardId + ".png";
        }
        
        //レア度を取得
        string chara_rare = _user_deck_list["chara_rare" + toString(num)].string_value();
		HMSLOG("chara_rare [%d] : %s", num, chara_rare.c_str());
        
        Node *node = Node::create() ;
        node->setCascadeColorEnabled(true) ;
        node->setCascadeOpacityEnabled(true) ;
        _nodes.push_back(node);
        this->addChild(node, 9);
        
        //カードの画像を追加 //nodeに依存
        Sprite *card = Sprite::create(card_pass) ;
        card->setScale(ratio*0.6f) ;
        _cards.push_back(card);
        node->addChild(card) ;
        
        //カードが床に反射している画像を作成
        Sprite* reflection_card = Sprite::create("download/Chara/parts400/reflection" + chara_rare + ".png") ;
        reflection_card->setPosition(card->getContentSize().width/2, 0) ;
        reflection_card->setAnchorPoint(Point(0.5, 1)) ;
        _reflection_cards.push_back(reflection_card);
        card->addChild(reflection_card) ;
		
        //カードの名前画像を作成
        Sprite *name_back= Sprite::create("init/Home/Mypage/name_bk.png");
        name_back->setScale(ratio*0.6f) ;
        name_back->setAnchorPoint(Point(0.5, 0)) ;
        name_back->setPosition(Point(0, -card->getContentSize().height/2*ratio*0.6f)) ;
        name_back->setCascadeOpacityEnabled(true) ;
        name_back->setOpacity(0) ;
        
        string name_image_path = StringUtils::format("download/Chara/name/%s.png", cardId.c_str());
        Sprite *sp_name = Sprite::create(name_image_path.c_str());
        sp_name->setPosition(name_back->getContentSize()/2) ;
        name_back->addChild(sp_name) ;
        
        _card_names.push_back(name_back);
        node->addChild(name_back, 10) ;
		
        
        //以下はcardスプライトに依存
        
		//カード枠をクリッピングノードで光らせる
		Sprite* frame_kira_img = Sprite::create("download/Chara/parts400/frame_light_bar.png");
        
        float frame_kira_start_posx = - 100 ;
        float frame_kira_start_posy = card->getContentSize().height  + 100 ;
        float frame_kira_end_posx = card->getContentSize().width  + 100 ;
		float frame_kira_end_posy =  - 100;
		frame_kira_img->setAnchorPoint(Vec2(0.5f, 0.5f));
		frame_kira_img->setPosition(Point(frame_kira_start_posx, frame_kira_start_posy));
		frame_kira_img->setRotation(45.0f);
		frame_kira_img->setOpacity(0.0f);
		frame_kira_img->setScaleY(2.0f);
        
        Sprite* chara_frame_img = Sprite::create("download/Chara/parts400/frame" + chara_rare + ".png");
        chara_frame_img->setAnchorPoint(Vec2(0, 0));

        auto clipper = ClippingNode::create();
        clipper->setStencil(chara_frame_img);
        clipper->setAlphaThreshold(0.0f);
        clipper->addChild(frame_kira_img);
        clipper->setPosition(Point(0, 0));
        card->addChild(clipper);

        //アクション
        Sequence* seq = Sequence::create(DelayTime::create(1.0f), MoveTo::create(0.8f, Point(frame_kira_end_posx,frame_kira_end_posy)), Place::create(Point(frame_kira_start_posx, frame_kira_start_posy)),  nullptr);
        RepeatForever* repeat = RepeatForever::create(seq);
        frame_kira_img->runAction(repeat);
        _card_frame.push_back(frame_kira_img);
        
        
		//SR以上の場合、クリッピングによる光を演出
		Sprite* rare_kira_img = Sprite::create("download/Chara/parts400/light_bar.png");
		float rare_kira_start_posx = -rare_kira_img->getContentSize().width / 2;
		float rare_kira_start_posy = card->getContentSize().height  + 20;
		float rare_kira_end_posx = card->getContentSize().width / 2;
		float rare_kira_end_posy = card->getContentSize().height/2 ;
		rare_kira_img->setAnchorPoint(Vec2(0.5f, 0.5f));
		rare_kira_img->setPosition(Point(rare_kira_start_posx, rare_kira_start_posy));
		rare_kira_img->setRotation(45.0f);
		rare_kira_img->setOpacity(0.0f);
		
		if(stoi(chara_rare) > 2) {
			Sprite* chara_rare_img = Sprite::create("download/Chara/parts400/card_rare" + chara_rare + "_mask.png");
			chara_rare_img->setAnchorPoint(Vec2(0, 0));
			
			auto clipper = ClippingNode::create();
			clipper->setStencil(chara_rare_img);
			clipper->setAlphaThreshold(0.0f);
			clipper->addChild(rare_kira_img);
			clipper->setPosition(Point(0, 0));
			card->addChild(clipper);
			
			//アクション
			Sequence* seq = Sequence::create(DelayTime::create(0.5f), MoveTo::create(0.8f, Point(rare_kira_end_posx,rare_kira_end_posy)), Place::create(Point(rare_kira_start_posx, rare_kira_start_posy)),  nullptr);
			RepeatForever* repeat = RepeatForever::create(seq);
			rare_kira_img->runAction(repeat);
			
			_card_rare.push_back(rare_kira_img);
		} else {
			rare_kira_img->setVisible(false);
			card->addChild(rare_kira_img);
			_card_rare.push_back(rare_kira_img);
		}
    }
}

//カードの回転、移動は全てここで行う
void CardGalleryLayer::updateCardPositionAndRotate(){
    
    //カードを配置
    for(int i=0; i<CARD_ARRAY_SIZE; i++) {
        float card_center_diff_x = calcCardCenterDifferXByIndex(i) ;
        float card_center_diff_z = calcCardCenterDifferZByIndex(i) ;
        float card_rotate = calcCardRotationAxisYByIndex(i) ;
        int card_color_byte = calcCardColorByteByIndex(i) ;
        
        Point p = convertFromDisplaySize(Point(card_center_diff_x + _card_center_position.x, _card_center_position.y), 0.5, 0.5) ;
        
        _nodes[i]->setRotation3D(Vec3(0, -card_rotate, 0));
        _nodes[i]->setPosition3D(Vec3(p.x, p.y, card_center_diff_z*ratio));
        _nodes[i]->setGlobalZOrder(0);
        _nodes[i]->setZOrder(card_center_diff_z);
        _nodes[i]->setColor(Color3B(card_color_byte,card_color_byte,card_color_byte)) ;
        
        //浮遊の影響を受ける倍率を求め配列に代入、(中心ほど影響を受け、中心を離れると影響は0になる)
        float floatEffectRate = calcFloatEffectRateByIndex(i) ;
        _floatEffectRate[i] = floatEffectRate ;
        
        //カード画像の名前を角度によって表示する
        float card_alfa = 255.0 * (1.0 - abs(card_rotate) / 80.0);
        if(card_alfa>255)card_alfa=255 ;
        _card_names[i]->setOpacity(card_alfa);
		_card_rare[i]->setOpacity(card_alfa);
        _card_frame[i]->setOpacity(card_alfa) ;
        
    }
    
    swipeSound() ;
}

void CardGalleryLayer::swipeSound(){
    
    int now_index = calcNearestCenterCard(_card_state_decide_number) ;
    
    //最も近いindexがdelta_xの移動後に異なる場合、効果音をならす
    if(now_index != _pre_sound_index){
        _pre_sound_index = now_index ;
        
        //詳細を開いていない場合のみカードのめくれる音を鳴らす
        if(!_show_chara_detail_flg){
            //SE
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/card_slash.mp3",false);
        }
    }
}

void CardGalleryLayer::showCharaDetail(float dt){
    _show_chara_detail_flg = true ;
    
    int target_index = calcNearestCenterCard(_card_state_decide_number) ;
    
    MypageLayer *mypageLayer = (MypageLayer *)getParent() ;
    detail = CharaDetailPopupLayer::create(fromString<int>(_user_deck_list["chara_list_id" + toString(target_index+1)].string_value()), true) ;
    
    //詳細が閉じられたときに呼ばれる関数を登録
    SEL_SCHEDULE sel_close = schedule_selector(CardGalleryLayer::closeCharaDetailCallback);
    detail->setSchedulerForClose(this, sel_close);
    
    //base_indexが変更されたときに呼び出される関数を登録
    SEL_SCHEDULE sel_change_base= schedule_selector(CardGalleryLayer::changeBaseCallback);
    detail->setSchedulerForChangeBase(this, sel_change_base);
    
    //リスト登録ここから
    HmsVector<int> chara_list_id_list;
    for(int i=0; i<DECK_CARD_NUM; i++) {
        int id = fromString<int>(_user_deck_list["chara_list_id" + toString(i+1)].string_value());
        chara_list_id_list.insert(id);
    }
    
    detail->setCharaListIdList(chara_list_id_list);
    //ここまで
    mypageLayer->addChild(detail, MypageLayer::Z_Popup_Layer) ;
}

void CardGalleryLayer::closeCharaDetailCallback(float dt){
    _show_chara_detail_flg = false ;
}

void CardGalleryLayer::changeBaseCallback(float dt){
    int base_index = detail->getBaseIndex() ;
    autoScrollByTargetIndex(base_index);
    
//    detail->removeSelf(0.0); 
}

bool CardGalleryLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    if(!checkContainCardPosition(touch->getLocation())){
        //もし範囲外だったらここでonTouch~系に伝搬せず終了
        return false ;
    }
    
    //スクロールしていたら停止
    unschedule(schedule_selector(CardGalleryLayer::updateForScrollByVelocity));
    //自動で中心位置に移動しようとしていたら停止
    unschedule(schedule_selector(CardGalleryLayer::updateForAutoScrollToCenter));
    
    on_touch_move_call_count=0 ;
    for(int i=0 ; i<10; i++){
        delta_xs[i] = 0 ;
    }
    
    return true ;
}

void CardGalleryLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    on_touch_move_call_count++ ;
    delta_xs[on_touch_move_call_count%10] = touch->getDelta().x ;
    
    moveCardsByDeltaX(touch->getDelta().x) ;
    
    
}

void CardGalleryLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    //最初のタップ位置から、しきいちの距離を超えていない　かつ　スクロール中でなければ詳細を表示する
    float distance = touch->getStartLocation().getDistance(touch->getLocation()) ;
    if(distance <= show_detail_detect_distance && now_scroll == false){
        CardGalleryLayer::showCharaDetail(0.0f);
    }
    
    Point currentPos = touch->getLocation() ;
    Point prevPos = touch->getPreviousLocation();
    
    //float vec_x = currentPos.x - prevPos.x ;
    
    float vec_x_average = 0 ;
    for(int i=0 ; i<10; i++){
        vec_x_average += delta_xs[i]/10;
    }
    
    //速度を与えてスクロールを行う
    now_scroll = scrollByVelocity(vec_x_average) ;
    if(!now_scroll) {
        autoScrollToCenter();
    }
    
    Point position = touch->getLocation();
}

void CardGalleryLayer::moveCardsByDeltaX(float delta_x){
    //微小距離delta_xによって_card_state_decide_numberを変更しupdateCaupdateCardPosition3DAndRotate関数によってカードを移動する
    //_card_state_decide_number += -delta_x/_contentSize.width*CARD_ARRAY_SIZE*ratio/3.0f;
    _card_state_decide_number += -delta_x/_contentSize.width*CARD_ARRAY_SIZE/3.0f;
    
    if(_card_state_decide_number<0){
        _card_state_decide_number = 0 ;
    }else if(_card_state_decide_number>CARD_ARRAY_SIZE-1){
        _card_state_decide_number = CARD_ARRAY_SIZE-1 ;
    }
    updateCardPositionAndRotate() ;
}


void CardGalleryLayer::autoScrollToCenter(){
    int target_index = calcNearestCenterCard(_card_state_decide_number) ;
    autoScrollByTargetIndex(target_index) ;
}


void CardGalleryLayer::autoScrollByTargetIndex(int target_index){
    //自動スクロールの経過時間を0に
    _autoScrollProgressTime=0 ;
    
    //自動スクロールの目的のindex
    _autoScrollDirectionIndex = target_index ;
    
    //自動スクロールの目的のスピード(現在の位置から離れていれば早くする)
    _initial_velocity = abs(target_index - _card_state_decide_number);
    
    this->schedule(schedule_selector(CardGalleryLayer::updateForAutoScrollToCenter));
}

void CardGalleryLayer::updateForAutoScrollToCenter(float dt){
    _autoScrollProgressTime+=dt ;
    
    float direction = _autoScrollDirectionIndex - _card_state_decide_number;
//    float speed = -2 * _initial_velocity * (_autoScrollProgressTime - 1) ;
    float x= _initial_velocity * dt ;
   
    if(direction>0){
        _card_state_decide_number+=x;
        float nowIndexDiff = _autoScrollDirectionIndex - _card_state_decide_number;
        if(nowIndexDiff<0){
            _card_state_decide_number = _autoScrollDirectionIndex ;
            unschedule(schedule_selector(CardGalleryLayer::updateForAutoScrollToCenter));
        }
    }else if(direction<0){
        _card_state_decide_number-=x;
        float nowIndexDiff = _autoScrollDirectionIndex - _card_state_decide_number;
        if(nowIndexDiff>0){
            _card_state_decide_number = _autoScrollDirectionIndex ;
            unschedule(schedule_selector(CardGalleryLayer::updateForAutoScrollToCenter));
        }
    }
    updateCardPositionAndRotate() ;
}

bool CardGalleryLayer::scrollByVelocity(float velocity){
    _init_scroll_velocity = -velocity/3.0f ;
    _now_time = 0 ;
    if(velocity<-_scroll_detect_speed){
        _acceleration = -10 ;
        this->schedule(schedule_selector(CardGalleryLayer::updateForScrollByVelocity));
        return true;
    }else if(velocity>_scroll_detect_speed){
        _acceleration = 10 ;
        this->schedule(schedule_selector(CardGalleryLayer::updateForScrollByVelocity));
        return true;
    }else{
        return false ;
    }
}

void CardGalleryLayer::updateForScrollByVelocity(float dt){
    _now_time += dt ;
    float now_v = _init_scroll_velocity + _acceleration * _now_time ;
    
    if((now_v>=0 && _acceleration>=0) || (now_v<=0 && _acceleration<=0)){ //加速度と速度の向きがそろったら終了
        now_scroll = false ;
        autoScrollToCenter(); //スクロールが終了したので最も近い距離の中心に戻す
        unschedule(schedule_selector(CardGalleryLayer::updateForScrollByVelocity));
    }
    
    _card_state_decide_number += now_v/_contentSize.width*CARD_ARRAY_SIZE/3.0f;
    
    if(_card_state_decide_number<0){
        now_scroll = false ;
        _card_state_decide_number = 0 ;
        unschedule(schedule_selector(CardGalleryLayer::updateForScrollByVelocity));
    }else if(_card_state_decide_number>CARD_ARRAY_SIZE-1){
        now_scroll = false ;
        _card_state_decide_number = CARD_ARRAY_SIZE-1 ;
        unschedule(schedule_selector(CardGalleryLayer::updateForScrollByVelocity));
    }
    
    updateCardPositionAndRotate() ;
}

bool CardGalleryLayer::checkContainCardPosition(Point touch_point){
    //中心座標から、上下カードサイズ/2以内であれば有効判定
    Point center = convertFromDisplaySize(_card_center_position , 0.5, 0.5);
    float card_top_y = center.y + CARD_IMAGE_SIZE/2 * ratio; //カードの上端位置
    float card_bottom_y = center.y - CARD_IMAGE_SIZE/2 * ratio; //カードの下端位置
    
    //カードの下端がタップしたポイントより上 かつ カードの上端がタップしたポイントより下 ならばタップ位置はカードの位置に含まれる
    if(card_bottom_y <= touch_point.y && card_top_y >= touch_point.y ) {
        return true ;
    }else{
        return false ;
    }
}

float CardGalleryLayer::calcCardCenterDifferZByIndex(int target_index){
    
    float x = target_index - _card_state_decide_number ;
    
    float answer ;
    
    float abs_x = abs(x) ;
    
    answer = (f1(abs_x) + f5(abs_x)) * (float)CARD_IMAGE_SIZE ;
    answer += 100;
    
    return answer ;
}

float CardGalleryLayer::f1(float x){
    float y = exp(-x*x*10) * 0.6;
    return y;
}

float CardGalleryLayer::f5(float x){
    float y = - x * 0.01f ;
    return y ;
}

float CardGalleryLayer::calcCardCenterDifferXByIndex(int target_index){
    float x = target_index - _card_state_decide_number ;
    float answer ;
    
    float abs_x = abs(x) ;
    
    answer = (f2(abs_x)+f3(abs_x)) * (float)CARD_IMAGE_SIZE ;
    
    if(x<0){
        answer = -answer ;
    }
    
    return answer ;

}

float CardGalleryLayer::f2(float x){
    float y=0.3*(1.0-exp(-x*x*10.0)) ;
    return y;
}

float CardGalleryLayer::f3(float x){
    float slope = 0.1f ; //傾き
    float y = x*slope ;
    return y;
}

float CardGalleryLayer::calcCardRotationAxisYByIndex(int target_index){
    float x = target_index - _card_state_decide_number ;
    float abs_x = abs(x) ;
    
    float answer ;
    
    answer = f4(abs_x) ;

    if(x<0){
        answer = -answer ;
    }

    return answer ;
}

float CardGalleryLayer::f4(float x){
    float y=80.0*(1-exp(-x*x*4)) ;
    return y;
}

int CardGalleryLayer::calcCardColorByteByIndex(float target_index) {
    float x = target_index - _card_state_decide_number ;
    
    int answer = (int)f6(x) ;
    
    if(answer>=255)answer=255 ;
    
    return answer ;
}

float CardGalleryLayer::f6(float x){
    //float y = 1.25*x ;
    float y=exp(-x*x/30)*255 ;
    return y;
}


float CardGalleryLayer::calcFloatEffectRateByIndex(int target_index) {
    float x = target_index - _card_state_decide_number ;
    
    float answer ;
    if(-1<=x && x <= 1){
        answer = f8(x) ;
    }else{
        answer = 0 ;
    }
    
    return answer ;
}

float CardGalleryLayer::f8(float x){
    float y= exp(-x*x*10) ;
    return y;
}

int CardGalleryLayer::calcNearestCenterCard(float target_index){
    int near_center_index ;
    
    //_card_state_decide_numberを整数部と小数部に分解する
    int integer_place = (int)target_index;
    float decimal_place = target_index - integer_place ;
    
    //小数部が0.5以上であれば中心に近いindexは整数部+1 0.5fより小さければ中心に近いindexは整数部となる
    if(decimal_place >= 0.5f){
        near_center_index = integer_place + 1;
    }else if(decimal_place < 0.5f){
        near_center_index = integer_place ;
    }
    
    //indexが範囲外だったら修正する
    if(near_center_index < 0){
        near_center_index = 0 ;
    }else if(CARD_ARRAY_SIZE - 1 < near_center_index){
        near_center_index = CARD_ARRAY_SIZE - 1;
    }
    
    return near_center_index ;
}

