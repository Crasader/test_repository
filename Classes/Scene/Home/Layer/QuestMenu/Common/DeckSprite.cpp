#include "DeckSprite.h"

DeckSprite::DeckSprite()
:_general_card_id()
,_friend_card_id()
,_deck_name()
,_offense()
,_defense()
,_strategist()
,_general()
,_json()
{
}

DeckSprite* DeckSprite::create(int deck_no){
    
    DeckSprite *sprite = new DeckSprite();
    
    if (sprite && sprite->init(deck_no))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckSprite::init(int deck_no)
{
    if ( !Sprite::initWithFile("init/Home/Quest/popup_battle_deck.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //デッキ番号の取得
    _deck_no = deck_no;
    
    //スクロールの状態を格納  0:上部分表示 1:下部分表示
    deck_position_state = 0 ;
    
    //初期情報の設定
    setupData();
    
    return true;
}

void DeckSprite::setupData()
{
    string user_party_list = SaveData::loadString("user_party_list");
//    HMSLOG("user_party_list = %s", user_party_list.c_str());
    
    string err;
    Json json = Json::parse(user_party_list, err);
    _user_party_list = json[toString<int>(_deck_no)].object_items();
    
    //デッキ情報の設定
    setGeneralCardId(_user_party_list["leader_chara_id"].string_value());
    setDeckName(_user_party_list["deck_name"].string_value());
    setOffense(_user_party_list["offense"].string_value());
    setDefense(_user_party_list["defense"].string_value());
    setStrategist(_user_party_list["strategist"].string_value());
    setGeneral(_user_party_list["general"].string_value());
    
    //選択したフレンドカードIDの設定
    string jsonStr = SaveData::loadString("helper_user_sprite_data");
    Json helper_user_data = Json::parse(jsonStr, err); //助っ人ユーザーデータ
    setFriendCardId(helper_user_data["card_image_id"].string_value());
    
//    HMSLOG("deck offense = %s, defense = %s, strategist = %s, general = %s, ", _offense.c_str(), _defense.c_str(), _strategist.c_str(), _general.c_str());
//    HMSLOG("friend_card_id = %s", _friend_card_id.c_str());
    
    setJson(json.dump());
}

void DeckSprite::showDeckName()
{
    //デッキ名の表示
    auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_deck_name, FONT_NAME_SYSTEM, 34);
    name_label->setPosition(Point(this->getContentSize().width / 2, 340));
    this->addChild(name_label, 10);
}

void DeckSprite::showGeneralCard()
{
    //総大将カードの表示
    string card_pass = "download/Chara/parts100/card_bk1_reverse.png";
    if(_general_card_id != "0") {
        card_pass = "download/Chara/thumb100/"+_general_card_id+".png";
    }
    
    _general_card = Sprite::create(card_pass);
    _general_card->setScale(0.84);
    _general_card->setPosition(Point(94, 270));
    addChild(_general_card, 1);
}

void DeckSprite::showFriendCard()
{
    string challenge_event_stage_id = SaveData::loadString("challenge_event_stage_id");
    if(challenge_event_stage_id!="0" && challenge_event_stage_id!="") return;
    
    //フレンドカードの表示
    if(_friend_card_id != "0" && _friend_card_id != "") {
        string card_pass = "download/Chara/thumb100/"+_friend_card_id+".png";
        Sprite* friend_card = Sprite::create(card_pass);//助っ人カードの画像
        friend_card->setScale(0.84);
        friend_card->setPosition(Point(540, 270));
        addChild(friend_card, 1);
    }
    
}

void DeckSprite::createDeckCard()
{
    //デッキカードの作成
    for (int i = 1; i <= DECK_CARD_NUM; i++) {
        string card_pass = "download/Chara/parts100/card_bk1_reverse.png";
        string card_id = _user_party_list["chara_id"+toString<int>(i)].string_value();
        if (card_id != "0") {
            card_pass = "download/Chara/thumb100/"+card_id+".png";
        }
        _deck_card.push_back(Sprite::create(card_pass));
    }
}

void DeckSprite::showDeckCard()
{
    _deck_base_square = Sprite::create("init/Home/Quest/popup_battle_deck_square2.png")  ;
    _deck_base_square->setAnchorPoint(Point(0,0));
    _deck_base_square->setPosition(Point(0, -HALF_SQUARE_HEIGHT));
    
    ClippingNode* stencil = createClippingNode() ;
    stencil->setAnchorPoint(Point(0,0));
    stencil->setPosition(Point(SQUARE_LEFT_X, SQUARE_BOTTOM_Y));
    stencil->addChild(_deck_base_square) ;
    
    //デッキカードの表示
    int num = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            _deck_card[num]->setScale(0.85);
            _deck_card[num]->setPosition(Point(INNER_SQUARE_CARD_POS_X_1 + (j * DISTANCE_X), INNER_SQUARE_CARD_POS_Y_1 - (i * DISTANCE_Y)));
            _deck_base_square->addChild(_deck_card[num]);
            num++;
        }
    }
    
    this->addChild(stencil, 2) ;
    
    /*
    //まだ未完成、10より先はけす
    for (int i = 10; i < 20; i++) {
        _deck_card[i]->setvisible(false);
    }
     */
}

void DeckSprite::showData()
{
    //デッキ名の表示
    showDeckName();
    
    //大将カードの表示
    showGeneralCard();
    
    //フレンドカードの表示
    showFriendCard();
    
    //デッキカードの作成
    createDeckCard();
    
    //デッキカードの表示
    showDeckCard();
}

void DeckSprite::showDeckParam()
{
    //攻撃
    auto offense_label = HMSFontUtil::createWithTTFBySystemFontArgument(_offense, FONT_NAME_SYSTEM, 30);
    offense_label->setPosition(Point(246, 220));
    offense_label->setAnchorPoint(Point(1.0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(offense_label, 2);
    
    //防御
    auto defense_label = HMSFontUtil::createWithTTFBySystemFontArgument(_defense, FONT_NAME_SYSTEM, 30);
    defense_label->setPosition(Point(364, 220));
    defense_label->setAnchorPoint(Point(1.0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(defense_label, 2);
    
    //軍師力
    auto strategist_label = HMSFontUtil::createWithTTFBySystemFontArgument(_strategist, FONT_NAME_SYSTEM, 30);
    strategist_label->setPosition(Point(478, 220));
    strategist_label->setAnchorPoint(Point(1.0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(strategist_label, 2);
    
//    //総大将
//    auto general_label = HMSFontUtil::createWithTTFBySystemFontArgument(_general, FONT_NAME_SYSTEM, 33);
//    general_label->setPosition(Point(596, 10));
//    general_label->setAnchorPoint(Point(1.0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
//    this->addChild(general_label, 2);
}

void DeckSprite::onEnter()
{
    
    
    Sprite::onEnter();
    
    //データの表示
    showData();
    
    //スクロールバーの作成
    createScrollBar();
    
    //デッキパラメータの表示
    showDeckParam();

}

void DeckSprite::createScrollBar(){
    _scroll_bar = Sprite::create("init/Home/Quest/scroll_bar2.png") ;
    _scroll_bar->setScaleY(0.5f) ;
    _scroll_bar->setAnchorPoint(Point(0,0)) ;
    _scroll_bar->setPosition(Point(SQUARE_LEFT_X + SQUARE_WIDTH, SQUARE_BOTTOM_Y+HALF_SQUARE_HEIGHT/2)) ;
    this->addChild(_scroll_bar,3) ;
}

ClippingNode *DeckSprite::createClippingNode()
{
    Sprite *deck_square_half_stencil = Sprite::create("init/Home/Quest/popup_battle_deck_square.png")  ;
    deck_square_half_stencil->setAnchorPoint(Point(0,0));
    ClippingNode *clipper = ClippingNode::create();
    clipper->setStencil(deck_square_half_stencil);
    return clipper ;
}

void DeckSprite::scrollAction(float dt)
{
    //デッキ枠についてのアクション
    _deck_base_square->stopAllActions() ;
    if(deck_position_state == 0){
        MoveTo *move = MoveTo::create(0.5, Point(0, 0) ) ;
        EaseExponentialOut *easeOut = EaseExponentialOut::create(move);
        _deck_base_square->runAction(easeOut) ;
    }else if(deck_position_state == 1){
        MoveTo *move = MoveTo::create(0.5, Point(0, -HALF_SQUARE_HEIGHT) ) ;
        EaseExponentialOut *easeOut = EaseExponentialOut::create(move);
        _deck_base_square->runAction(easeOut) ;
    }
    
    //スクロールバーについてのアクション
    _scroll_bar->stopAllActions();
    if(deck_position_state == 0){
        MoveTo *move = MoveTo::create(0.5, Point(SQUARE_LEFT_X + SQUARE_WIDTH, SQUARE_BOTTOM_Y) ) ;
        EaseExponentialOut *easeOut = EaseExponentialOut::create(move);
        _scroll_bar->runAction(easeOut) ;
    }else if(deck_position_state == 1){
        MoveTo *move = MoveTo::create(0.5, Point(SQUARE_LEFT_X + SQUARE_WIDTH, SQUARE_BOTTOM_Y+HALF_SQUARE_HEIGHT/2) ) ;
        EaseExponentialOut *easeOut = EaseExponentialOut::create(move);
        _scroll_bar->runAction(easeOut) ;
    }
    
    //ステータスを変更
    if(deck_position_state == 0){
        deck_position_state = 1 ;
    }else if(deck_position_state == 1){
        deck_position_state = 0 ;
    }
}

