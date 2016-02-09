#include "DeckChoiceBaseLayer.h"
#include "CardDetailLayer.h"

DeckChoiceBaseLayer* DeckChoiceBaseLayer::create(float _deck_position_y, float _deck_scale, bool _is_half)
{
    DeckChoiceBaseLayer *layer = new DeckChoiceBaseLayer();
    
    if (layer && layer->init(_deck_position_y,  _deck_scale, _is_half))
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool DeckChoiceBaseLayer::init(float _deck_position_y, float _deck_scale, bool _is_half)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("DeckChoiceBaseLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの設定（通常のデッキ番号を初期番号として設定しておく）
    _deck_no = fromString<int>(SaveData::loadString("party_no"));
    deck_position_y = _deck_position_y;
    deck_scale = _deck_scale;
    is_half = _is_half;
    
    return true;
}

void DeckChoiceBaseLayer::onEnter()
{
    Layer::onEnter();
    
    //データの設定
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //キャラマップデータの作成
    createMapData();
    
    //デッキ画像の作成
    createDeckSprite();
    
    //デッキの表示
    showDeckSprite();
    
    //矢印の作成
    createArrowSprite() ;
    
    actionArrowSprite() ;
}

void DeckChoiceBaseLayer::setData()
{
    //デッキ情報の取得と設定
    string json_text = SaveData::loadString("chara_data_list");
    string err;
    _chara_data_list = Json::parse(json_text, err);
    
    string json_text2 = SaveData::loadString("user_party_list");
    string err2;
    _user_party_list = Json::parse(json_text2, err2);
}

void DeckChoiceBaseLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(DeckChoiceBaseLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckChoiceBaseLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckChoiceBaseLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckChoiceBaseLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckChoiceBaseLayer::createMapData()
{
    //キャラデータのMapリストの作成
    for (int i = 0; i < countJsonData(_chara_data_list); i++)
    {
        string iStr = toString<int>(i);
        _chara_list[_chara_data_list[iStr]["chara_list_id"].string_value()] = _chara_data_list[iStr].object_items();
    }
}

void DeckChoiceBaseLayer::createDeckSprite()
{
    //デッキ画像の作成
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        //キャラJsonDataの作成
        createCharaJsonData(i);
        
        if(is_half){
            _deck_bg.push_back(DeckSprite::create(i));
        }else{
            _deck_bg.push_back(DeckFormationSprite::create(_vec_chara_data, i));
        }
        _deck_bg[i]->setPosition(convertFromDisplaySize(Point(320, deck_position_y), 0.5, 0.5));
        _deck_bg[i]->setScale(deck_scale * ratio);
        _deck_bg[i]->setVisible(false); //見えない状態で設定
        this->addChild(_deck_bg[i], Z_Deck_Bg, T_Deck_Bg);
    }
}

void DeckChoiceBaseLayer::createCharaJsonData(int i)
{
    //キャラJsonDataの作成
    
    //初期化
    _vec_chara_data.clear();
    
    string iStr = toString<int>(i);
    for (int j = 0; j < MAX_DECK_CHARA_COUNT; j++)
    {
        _vec_chara_data.push_back(_chara_list[_user_party_list[iStr]["chara_list_id" + toString<int>(j + 1)].string_value()]);
    }
    
    //vecの21番目(配列だと20番目)に総大将データを入れる
    _vec_chara_data.push_back(_chara_list[_user_party_list[iStr]["leader_chara_list_id"].string_value()]);
}

void DeckChoiceBaseLayer::showDeckSprite()
{
    //現在設定中のデッキの表示
    _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->setPosition(convertFromDisplaySize(Point(320, deck_position_y), 0.5, 0.5));
    _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->setVisible(true);
}

void DeckChoiceBaseLayer::createArrowSprite()
{
    //左矢印
    _left_arrow = Sprite::create("init/CharaMenu/arrow_left.png");
    _left_arrow->setScale(ratio);
    _left_arrow->setAnchorPoint(Point(0.37, 0.5)); //画像の中心位置を起点とする
    _left_arrow->setPosition(convertFromDisplaySize(Point(30, deck_position_y), 0.5, 0.5));
    this->addChild(_left_arrow, Z_Left_Arrow, T_Left_Arrow);
    
    //右矢印
    _right_arrow = Sprite::create("init/CharaMenu/arrow_right.png");
    _right_arrow->setScale(ratio);
    _right_arrow->setPosition(convertFromDisplaySize(Point(610, deck_position_y), 0.5, 0.5));
    _right_arrow->setAnchorPoint(Point(0.37, 0.5)); //画像の中心位置を起点とする
    this->addChild(_right_arrow, Z_Right_Arrow, T_Right_Arrow);
}

void DeckChoiceBaseLayer::actionArrowSprite()
{
    MoveBy *left_arrow_move = MoveBy::create(0.6f, Point(-20,0)) ;
    ActionEase *left_arrow_move_ease_in = EaseExponentialIn::create(left_arrow_move);
    ActionEase *left_arrow_move_ease_in_back = left_arrow_move_ease_in->reverse();
    MoveTo *left_arrow_move_default = MoveTo::create(0.01, _left_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *left_arrow_forever_action = RepeatForever::create(Sequence::create(left_arrow_move_ease_in, left_arrow_move_ease_in_back, left_arrow_move_default, nullptr)) ;
    
    MoveBy *right_arrow_move = MoveBy::create(0.6f, Point(+20,0)) ;
    ActionEase *right_arrow_move_ease_in = EaseExponentialIn::create(right_arrow_move);
    ActionEase *right_arrow_move_ease_in_back = right_arrow_move_ease_in->reverse();
    MoveTo *right_arrow_move_default = MoveTo::create(0.01, _right_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *right_arrow_forever_action = RepeatForever::create(Sequence::create(right_arrow_move_ease_in, right_arrow_move_ease_in_back, right_arrow_move_default, nullptr)) ;
    
    _left_arrow->runAction(left_arrow_forever_action) ;
    _right_arrow->runAction(right_arrow_forever_action) ;
}

int DeckChoiceBaseLayer::calcArrayNo(Pos position, int deck_no)
{
    //デッキ番号から配列上での番号を取得する（配列番号とデッキ番号は1ずれているので注意）
    int array_no = deck_no - 1;
    switch (position)
    {
        case Pos::Left:
        {
            //左側のデッキ番号の取得
            array_no  = deck_no - 2;
            if (array_no < 0) array_no = MAX_DECK_COUNT - 1 ; //左が0より下の場合、デッキNo最大を左に表示
        }break;
        case Pos::Right:
        {
            //右側のデッキ番号の取得
            array_no = deck_no;
            if (array_no >= MAX_DECK_COUNT) array_no = 0; //右が最大より上の場合、デッキNo最小を右に表示
        }break;
        default: break;
            
    }
    return array_no;
}

void DeckChoiceBaseLayer::showTouchDeckSprite()
{
    //左画像
    _deck_bg[calcArrayNo(Pos::Left, _deck_no)]->setVisible(true);
    _deck_bg[calcArrayNo(Pos::Left, _deck_no)]->setPosition(convertFromDisplaySize(Point(-320, deck_position_y), 0.5, 0.5));
    
    //右画像
    _deck_bg[calcArrayNo(Pos::Right, _deck_no)]->setVisible(true);
    _deck_bg[calcArrayNo(Pos::Right, _deck_no)]->setPosition(convertFromDisplaySize(Point(960, deck_position_y), 0.5, 0.5));
}

void DeckChoiceBaseLayer::moveTouchDeckSprite(float delta)
{
    //デッキ画像を移動させる
    for (int i = 0; i < _deck_bg.size(); i++)
    {
        _deck_bg[i]->setPosition(Point(_deck_bg[i]->getPosition().x + delta, _deck_bg[i]->getPosition().y));
    }
}

void DeckChoiceBaseLayer::showCharaDetail(float dt)
{
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(_touch_chara->getCharaId());
    this->addChild(detail_layer, Z_Detail_Layer, T_Detail_Layer);
}

bool DeckChoiceBaseLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckChoiceBaseLayer!");
    
    //タッチデータの初期化
    _touch_chara = nullptr;
    
    for (int i = 0; i <= MAX_DECK_CHARA_COUNT; i++)
    {
        auto chara = (CharaSprite *)_deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->getChildByTag(DeckFormationSprite::T_Card + i);
        if (chara)
        {
            float scale = chara->getScale() * ratio;
            Point convertedPos = chara->convertToWorldSpace(Point::ZERO);
            Rect rect = Rect(convertedPos.x, convertedPos.y, chara->getContentSize().width * scale, chara->getContentSize().height * scale);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                _touch_chara = chara;
                
                break;
            }
        }
    }
    
    //左右に画像を表示させる
    showTouchDeckSprite();
    
    //矢印の非表示
    _left_arrow->setVisible(false) ;
    _right_arrow->setVisible(false) ;
    
    return true;
}
void DeckChoiceBaseLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //デッキ画像の移動
    moveTouchDeckSprite(touch->getDelta().x);
}
void DeckChoiceBaseLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //動きが20px以下チェック
    if(abs(touch->getStartLocation().x - touch->getPreviousLocation().x) < 20 * ratio) {
        
        //キャラをタッチしていた場合キャラ詳細の表示
        if (_touch_chara != nullptr) showCharaDetail(0);
    }
    
    listener->setEnabled(false); //アクション中はタッチ不可にする
    
    if(touch->getStartLocation().x - touch->getLocation().x < - _contentSize.width / 3 || Utility::containsPoint(getChildByTag(T_Left_Arrow), touch->getLocation()))
    {
        //右にスワイプ（左が出てくる）
        _deck_no--;
        if (_deck_no < 1) _deck_no = MAX_DECK_COUNT;
        
        //音の設定
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_deck_move.mp3");
    }
    else if (touch->getStartLocation().x - touch->getLocation().x > _contentSize.width / 3 || Utility::containsPoint(getChildByTag(T_Right_Arrow), touch->getLocation()))
    {
        //左にスワイプ（右が出てくる）
        _deck_no++;
        if (_deck_no > MAX_DECK_COUNT) _deck_no = 1;
        
        //音の設定
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_deck_move.mp3");
    }
    
    //中心点からずれていれば中心に戻す
    float move_x_distance = _contentSize.width / 2 - _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->getPosition().x;
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        auto move = MoveBy::create(0.3f, Point(move_x_distance, 0));
        auto ease = EaseOut::create(move, 2);
        _deck_bg[i]->runAction(ease);
    }
    
    //onTouchBeginからの移動距離が少ない場合、中心のデッキをスクロールする。（下半分のときだけスクロールする）
    if(is_half){
        halfDeckScroll(touch);
    }
    
    auto hide_func = CallFunc::create([this]()
    {
        //選択中以外のデッキを見えなくする
        for (int i = 0; i < MAX_DECK_COUNT; i++)
        {
            if (i == calcArrayNo(Pos::Middle, _deck_no)) continue;
            _deck_bg[i]->setVisible(false);
        }
        
        listener->setEnabled(true); //タッチ可能にする
        
        _left_arrow->setVisible(true) ;
        _right_arrow->setVisible(true) ;
    });
    
    this->runAction(Sequence::create(DelayTime::create(0.3f), hide_func, nullptr));
    
    //選択中デッキNoの更新
    SaveData::saveString("party_no", toString<int>(_deck_no));
    CCLOG("deck_no = %s", SaveData::loadString("party_no").c_str());
}

void DeckChoiceBaseLayer::halfDeckScroll(Touch *touch)
{
    
    if (abs(touch->getLocation().x - touch->getStartLocation().x) < _contentSize.width/10){
        
        auto spWorldPoint = _deck_bg[_deck_no - 1]->convertToWorldSpace(Point::ZERO);
        Rect rect_under = Rect(spWorldPoint.x, spWorldPoint.y, _deck_bg[_deck_no - 1]->getContentSize().width * ratio, _deck_bg[_deck_no - 1]->getContentSize().height * ratio/2);
        
        if (rect_under.containsPoint(touch->getLocation())){
            DeckSprite *deck_sp = (DeckSprite*)_deck_bg[_deck_no - 1];
            deck_sp->scrollAction(0.0f);
        }
    }
}

void DeckChoiceBaseLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
