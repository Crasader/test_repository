#include "DeckFormationTopLayer.h"

DeckFormationTopLayer::DeckFormationTopLayer()
:_deck_no(0)
{
}

DeckFormationTopLayer* DeckFormationTopLayer::create()
{
    DeckFormationTopLayer *sprite = new DeckFormationTopLayer();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckFormationTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("DeckFormationTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの設定
    _deck_no = fromString<int>(SaveData::loadString("party_no"));
    
    return true;
}

void DeckFormationTopLayer::onEnter()
{
    Layer::onEnter();
    
    //データの設定
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("デッキ選択", "CharaMenu", this);
    
    //デッキコピーボタンの作成
    createDeckCopyButton();
    
    //キャラマップデータの作成
    createMapData();
    
    //デッキ画像の作成
    createDeckSprite();
    
    //デッキの表示
    showDeckSprite();
    
    //矢印の作成
    createArrowSprite() ;
    
    actionArrowSprite() ;
    
    //ボタンの作成
    createButton();
    
    //トーナメントエントリー状態作成
    createTournamentButton();
}

void DeckFormationTopLayer::setData()
{
    //デッキ情報の取得と設定
    string json_text = SaveData::loadString("chara_data_list");
    string err;
    _chara_data_list = Json::parse(json_text, err);
    
    string json_text2 = SaveData::loadString("user_party_list");
    string err2;
    _user_party_list = Json::parse(json_text2, err2);
    
}

void DeckFormationTopLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(DeckFormationTopLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckFormationTopLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckFormationTopLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckFormationTopLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckFormationTopLayer::createDeckCopyButton()
{
    //デッキコピーボタンの作成
    auto copy_off = Sprite::create("init/CharaMenu/revision/deck_copy/bt_deck_copy.png");
    auto copy_on  = Sprite::create("init/CharaMenu/revision/deck_copy/bt_deck_copy.png");
    copy_on->setCascadeColorEnabled(true);
    copy_on->setColor(Color3B(180, 180, 180));
    
    auto copy_btn = MenuItemSprite::create(copy_off, copy_on, [this](Ref* sender){
        
        //矢印を隠す
        _left_arrow->setVisible(false);
        _right_arrow->setVisible(false);
        
        //コピーポップアップの表示
        auto popup_layer = DeckCopyLayer::create();
        this->addChild(popup_layer, Z_Copy_Layer, T_Copy_Layer);
    });
    copy_btn->setScale(ratio);
    
    auto menu = Menu::create(copy_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(555, 788), 0.5, 1));
    this->addChild(menu);
}

void DeckFormationTopLayer::createMapData()
{
    //キャラデータのMapリストの作成
    int count = countJsonData(_chara_data_list);
    for (int i = 0; i < count; i++)
    {
        string iStr = toString<int>(i);
        _chara_list[_chara_data_list[iStr]["chara_list_id"].string_value()] = _chara_data_list[iStr].object_items();
    }
}

void DeckFormationTopLayer::createDeckSprite()
{
    //デッキ画像の作成
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        //キャラJsonDataの作成
        createCharaJsonData(i);
        
        _deck_bg.push_back(DeckFormationSprite::create(_vec_chara_data, i));
        _deck_bg[i]->setPosition(convertFromDisplaySize(Point(320, 460), 0.5, 0.5));
        _deck_bg[i]->setScale(ratio);
        _deck_bg[i]->setVisible(false); //見えない状態で設定
        this->addChild(_deck_bg[i], Z_Deck_Bg, T_Deck_Bg);
    }
}

void DeckFormationTopLayer::createCharaJsonData(int i)
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

void DeckFormationTopLayer::showDeckSprite()
{
    //現在設定中のデッキの表示
    _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->setPosition(convertFromDisplaySize(Point(320, 460), 0.5, 0.5));
    _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->setVisible(true);
}

void DeckFormationTopLayer::createArrowSprite()
{
    //左矢印
    _left_arrow = Sprite::create("init/CharaMenu/arrow_left.png");
    _left_arrow->setScale(ratio);
    _left_arrow->setAnchorPoint(Point(0.37, 0.5)); //画像の中心位置を起点とする
    _left_arrow->setPosition(convertFromDisplaySize(Point(30, 460), 0.5, 0.5));
    this->addChild(_left_arrow, Z_Left_Arrow, T_Left_Arrow);
    
    //右矢印
    _right_arrow = Sprite::create("init/CharaMenu/arrow_right.png");
    _right_arrow->setScale(ratio);
    _right_arrow->setPosition(convertFromDisplaySize(Point(610, 460), 0.5, 0.5));
    _right_arrow->setAnchorPoint(Point(0.37, 0.5)); //画像の中心位置を起点とする
    this->addChild(_right_arrow, Z_Right_Arrow, T_Right_Arrow);
}

void DeckFormationTopLayer::actionArrowSprite()
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

void DeckFormationTopLayer::createButton()
{
    //総大将編成、デッキ編成ボタンの作成
    auto general_off = Sprite::create("init/Home/Common/bt1.png");
    auto general_on  = Sprite::create("init/Home/Common/bt1.png");
    general_on->setColor(Color3B(180, 180, 180));
    
    auto general_text_off = HMSFontUtil::createWithTTFBySystemFontArgument("総大将編成", FONT_NAME_SYSTEM, 28);
    general_text_off->setPosition(general_off->getContentSize() / 2) ;
    general_text_off->enableOutline(Color4B(0, 0, 0, 255), 2);
    general_off->addChild(general_text_off);
    
    auto general_text_on = HMSFontUtil::createWithTTFBySystemFontArgument("総大将編成", FONT_NAME_SYSTEM, 28);
    general_text_on->setPosition(general_on->getContentSize() / 2) ;
    general_text_on->enableOutline(Color4B(0, 0, 0, 255), 2);
    general_text_on->setColor(Color3B(180, 180, 180));
    general_on->addChild(general_text_on);
    
    auto general_btn = MenuItemSprite::create(general_off, general_on, [this](Ref* sender)
    {
        
        //総大将編成ページへ
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("GeneralFormation");
    });
    general_btn->setScale(ratio);
    
    auto formation_off = Sprite::create("init/Home/Common/bt1_gold.png");
    auto formation_on  = Sprite::create("init/Home/Common/bt1_gold.png");
    formation_on->setColor(Color3B(180, 180, 180));
    
    auto formation_text_off = HMSFontUtil::createWithTTFBySystemFontArgument("デッキ編成", FONT_NAME_SYSTEM, 28);
    formation_text_off->setPosition(formation_off->getContentSize() / 2) ;
    formation_text_off->enableOutline(Color4B(0, 0, 0, 255), 2);
    formation_off->addChild(formation_text_off);
    
    auto formation_text_on = HMSFontUtil::createWithTTFBySystemFontArgument("デッキ編成", FONT_NAME_SYSTEM, 28);
    formation_text_on->setPosition(formation_on->getContentSize() / 2) ;
    formation_text_on->enableOutline(Color4B(0, 0, 0, 255), 2);
    formation_text_on->setColor(Color3B(180, 180, 180));
    formation_on->addChild(formation_text_on);
    
    auto formation_btn = MenuItemSprite::create(formation_off, formation_on, [this](Ref* sender)
    {
        auto normal_scene = (NormalScene *)this->getParent();
        
        //デッキ編成ページへ
        normal_scene->changePage("DeckFormation");
    });
    formation_btn->setScale(ratio);
    
    auto menu = Menu::create(general_btn, formation_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20 * ratio);
    menu->setPosition(convertFromDisplaySize(Point(320, 138), 0.5, 0.5));
    this->addChild(menu);
}

void DeckFormationTopLayer::createTournamentButton()
{
    //トーナメント参加中ボタンの作成
    auto general_off = Sprite::create("init/Home/Common/bt3.png");
    auto general_on  = Sprite::create("init/Home/Common/bt3.png");
    
    auto general_text_off = HMSFontUtil::createWithTTFBySystemFontArgument("トーナメント参加中", FONT_NAME_SYSTEM, 28);
    general_text_off->setPosition(general_off->getContentSize() / 2) ;
    general_text_off->enableOutline(Color4B(0, 0, 0, 255), 2);
    general_off->addChild(general_text_off);
    
    auto general_text_on = HMSFontUtil::createWithTTFBySystemFontArgument("トーナメント参加中", FONT_NAME_SYSTEM, 28);
    general_text_on->setPosition(general_on->getContentSize() / 2) ;
    general_text_on->enableOutline(Color4B(0, 0, 0, 255), 2);
    general_on->addChild(general_text_on);
    
    auto general_btn = MenuItemSprite::create(general_off, general_on, [this](Ref* sender)
                                              {});
    general_btn->setScale(ratio);
    
    
    auto menu = Menu::create(general_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20 * ratio);
    menu->setPosition(convertFromDisplaySize(Point(320, 138), 0.5, 0.5));
    this->addChild(menu, Z_Tournament_Entry_Layer, T_Tournament_Entry_Layer);
    
    if(_deck_no==fromString<int>(SaveData::loadString("tournament_event_deck_no"))){
        menu->setVisible(true);
    }else{
        menu->setVisible(false);
    }

}

int DeckFormationTopLayer::calcArrayNo(Pos position, int deck_no)
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

void DeckFormationTopLayer::showTouchDeckSprite()
{
    //左画像
    _deck_bg[calcArrayNo(Pos::Left, _deck_no)]->setVisible(true);
    _deck_bg[calcArrayNo(Pos::Left, _deck_no)]->setPosition(convertFromDisplaySize(Point(-320, 460), 0.5, 0.5));
    
    //右画像
    _deck_bg[calcArrayNo(Pos::Right, _deck_no)]->setVisible(true);
    _deck_bg[calcArrayNo(Pos::Right, _deck_no)]->setPosition(convertFromDisplaySize(Point(960, 460), 0.5, 0.5));
}

void DeckFormationTopLayer::moveTouchDeckSprite(float delta)
{
    //デッキ画像を移動させる
    for (int i = 0; i < _deck_bg.size(); i++)
    {
        _deck_bg[i]->setPosition(Point(_deck_bg[i]->getPosition().x + delta, _deck_bg[i]->getPosition().y));
    }
}

void DeckFormationTopLayer::showCharaDetail(float dt)
{
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(_touch_chara->getCharaId());
    this->addChild(detail_layer, Z_Detail_Layer, T_Detail_Layer);
}

void DeckFormationTopLayer::setNowDeckSprite(int origin_deck_no, int previous_deck_no)
{
    //引数で与えられたデッキ番号に内容と位置を更新する
    
    //データの更新
    setData();
    
    //コピー先のデッキスプライトの内容を更新
    createCharaJsonData(previous_deck_no - 1);
    _deck_bg[previous_deck_no - 1]->refreshDeckSprite(_vec_chara_data);
    
    //引数以外のデッキスプライトを非表示にする
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        if (i != previous_deck_no - 1) _deck_bg[i]->setVisible(false);
        else _deck_bg[i]->setVisible(true);
    }
    
    //位置の設定
    auto move = MoveTo::create(0.1f, convertFromDisplaySize(Point(320, 460), 0.5, 0.5));
    auto ease = EaseOut::create(move, 2);
    _deck_bg[previous_deck_no - 1]->runAction(ease);
    
    //矢印の表示
    _left_arrow->setVisible(true);
    _right_arrow->setVisible(true);
    
    //選択中デッキNoの更新
    _deck_no = previous_deck_no;
    SaveData::saveString("party_no", toString<int>(_deck_no));
    CCLOG("previous_deck_no = %s", SaveData::loadString("party_no").c_str());
}

void DeckFormationTopLayer::setNewDeckName(int deck_no)
{
    //デッキ名を新しいものへ変更する
    
    //データの更新
    setData();
    
    //デッキ名の更新
    auto name_label = (Label *)_deck_bg[deck_no - 1]->getChildByTag(DeckFormationSprite::T_Deck_Name);
    if (name_label) name_label->setString(_user_party_list[toString<int>(deck_no - 1)]["deck_name"].string_value());
    
}

bool DeckFormationTopLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckFormationTopLayer!");
    
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
void DeckFormationTopLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //デッキ画像の移動
    moveTouchDeckSprite(touch->getDelta().x);
}
void DeckFormationTopLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //動きが20px以下チェック
    if(abs(touch->getStartLocation().x - touch->getPreviousLocation().x) < 20 * ratio) {
        
        //キャラをタッチしていた場合キャラ詳細の表示
        if (_touch_chara != nullptr) showCharaDetail(0);
        
        //デッキ名をタッチしていた場合、デッキ名変更ポップアップの表示
        auto deck_name_label = (Label *)_deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->getChildByTag(DeckFormationSprite::T_Deck_Name);
        if (deck_name_label)
        {
            float scale = deck_name_label->getScale() * ratio;
            Point convertedPos = deck_name_label->convertToWorldSpace(Point::ZERO);
//            Rect rect = Rect(convertedPos.x, convertedPos.y, deck_name_label->getContentSize().width * scale, deck_name_label->getContentSize().height * scale);
            Rect rect = Rect(convertedPos.x, convertedPos.y, 280 * scale, 45 * scale);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                //デッキ名変更ポップアップの表示
                auto layer = DeckNamePopupLayer::create(_deck_no, deck_name_label->getString());
                addChild(layer, Z_Deck_Name_Layer, T_Deck_Name_Layer);
            }
        }
    }
    
    
//    //動きが20px以下かつ、キャラをタッチしていた場合、キャラ詳細表示
//    if(abs(touch->getStartLocation().x - touch->getPreviousLocation().x) < 20 * ratio && _touch_chara != nullptr) showCharaDetail(0);
    
    
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
    
    auto hide_func = CallFunc::create([this]()
    {
        //選択中以外のデッキを見えなくする
        for (int i = 0; i < MAX_DECK_COUNT; i++)
        {
            if (i == calcArrayNo(Pos::Middle, _deck_no)) continue;
            _deck_bg[i]->setVisible(false);
        }
        
        listener->setEnabled(true); //タッチ可能にする
        
        //名前変更レイヤーが無かった場合表示する
        if (!this->getChildByTag(T_Deck_Name_Layer))
        {
            _left_arrow->setVisible(true) ;
            _right_arrow->setVisible(true) ;
        }
    });
    
    this->runAction(Sequence::create(DelayTime::create(0.3f), hide_func, nullptr));
    
    //選択中デッキNoの更新
    SaveData::saveString("party_no", toString<int>(_deck_no));
    CCLOG("deck_no = %s", SaveData::loadString("party_no").c_str());
    
    auto tournament_btn = this->getChildByTag(T_Tournament_Entry_Layer);
    if(_deck_no==fromString<int>(SaveData::loadString("tournament_event_deck_no"))){
        tournament_btn->setVisible(true);
    }else{
        tournament_btn->setVisible(false);
    }

}

void DeckFormationTopLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
