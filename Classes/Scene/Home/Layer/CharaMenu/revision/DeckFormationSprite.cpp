#include "DeckFormationSprite.h"

DeckFormationSprite::DeckFormationSprite()
:_general_card_id()
,_deck_name()
,_offense()
,_defense()
,_strategist()
,_general()
,_deck_no(0)
{
}

DeckFormationSprite* DeckFormationSprite::create(HmsVector<Json> &vec_chara_data, int deck_no)
{
    DeckFormationSprite *sprite = new DeckFormationSprite();
    
    if (sprite && sprite->init(vec_chara_data, deck_no))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckFormationSprite::init(HmsVector<Json> &vec_chara_data, int deck_no)
{
    if ( !Sprite::initWithFile("init/CharaMenu/popup_deck1.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    _deck_no = deck_no;
    _vec_chara_list = vec_chara_data;
    
    //初期情報の設定
    setupData();
    
    return true;
}

void DeckFormationSprite::setupData()
{
    string user_party_list = SaveData::loadString("user_party_list");
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
}

void DeckFormationSprite::onEnter()
{
    Sprite::onEnter();
    
    //デッキ名の表示
    showDeckName();
    
    //総大将の表示
    showGeneralChara();
    
    //能力パラメータの表示
    showDeckParam();
    
    //キャラ画像の生成
    generateCharaCard();
}

void DeckFormationSprite::showDeckName()
{
    //デッキ番号画像の表示
    auto deck_num = Sprite::create("init/CharaMenu/revision/deck_num" + toString<int>(_deck_no + 1) + ".png");
    deck_num->setPosition(Point(202, 554));
    this->addChild(deck_num, Z_Deck_Number, T_Deck_Number);
    
    //デッキ名の表示
    auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_deck_name, FONT_NAME_SYSTEM, 30);
    name_label->setPosition(Point(354, 554));
    name_label->enableOutline(Color4B::BLACK, 2);
    this->addChild(name_label, Z_Deck_Name, T_Deck_Name);
}

void DeckFormationSprite::showGeneralChara()
{
    //総大将の表示
    auto general_card = CharaSprite::create(_vec_chara_list[20], false); //20番目が総大将データ
    general_card->setScale(0.9f);
    general_card->setPosition(Point(78, 488));
    general_card->setTag(T_Card + 20);
    addChild(general_card, Z_Card + 20);
    
    //総大将の名前表示
    string name_text = _vec_chara_list[20]["name"].string_value();
    auto general_label = HMSFontUtil::createWithTTFBySystemFontArgument(name_text, FONT_NAME_SYSTEM, 31);
    general_label->setHorizontalAlignment(TextHAlignment::LEFT);
    general_label->setAnchorPoint(Point(0, 0.5));
    general_label->setPosition(Point(144, 502));
    this->addChild(general_label);
}

void DeckFormationSprite::showDeckParam()
{
    //攻撃力
    auto offense_label = HMSFontUtil::createWithTTFBySystemFontArgument(getOffense(), FONT_NAME_SYSTEM, 32);
    offense_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    offense_label->setAnchorPoint(Point(1.0, 0.5));
    offense_label->setPosition(Point(255, 458));
    this->addChild(offense_label);
    
    //防御力
    auto defense_label = HMSFontUtil::createWithTTFBySystemFontArgument(getDefense(), FONT_NAME_SYSTEM, 32);
    defense_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    defense_label->setAnchorPoint(Point(1.0, 0.5));
    defense_label->setPosition(Point(405, 458));
    this->addChild(defense_label);
    
    //軍師力
    auto strategist_label = HMSFontUtil::createWithTTFBySystemFontArgument(getStrategist(), FONT_NAME_SYSTEM, 32);
    strategist_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    strategist_label->setAnchorPoint(Point(1.0, 0.5));
    strategist_label->setPosition(Point(555, 458));
    this->addChild(strategist_label);
}

void DeckFormationSprite::generateCharaCard()
{
    //キャラ画像の作成
    int num = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            auto chara = CharaSprite::create(_vec_chara_list[num], false);
            chara->setPosition(calcCardPosition(i, j));
            chara->setTag(T_Card + num);
            chara->setScale(0.9f);
            this->addChild(chara, Z_Card + num);
            
            num++;
        }
    }
}

Point DeckFormationSprite::calcCardPosition(int i, int j)
{
    //カードを置く位置を計算
    float x, y;
    
    x = DECK_1_X_POS + (DECK_1_X_DISTANCE * j);
    y = DECK_1_Y_POS - (i * DECK_1_Y_DISTANCE);
    
    return Point(x, y);
}

void DeckFormationSprite::refreshDeckSprite(HmsVector<Json > &vec_chara_data)
{
    //デッキ表示内容を更新する
    
    //デッキ内のオブジェクトを全て取り除く
    this->removeAllChildren();
    
    //データの初期化
    _vec_chara_list = vec_chara_data;
    
    //データの更新
    setupData();
    
    //デッキ名の表示
    showDeckName();
    
    //総大将の表示
    showGeneralChara();
    
    //能力パラメータの表示
    showDeckParam();
    
    //キャラ画像の生成
    generateCharaCard();
}


