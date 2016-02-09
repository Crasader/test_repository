#include "UserDetailContainFriendIdSprite.h"

UserDetailContainFriendIdSprite::UserDetailContainFriendIdSprite()
:_user_master_id()
,_friend_id()
,_user_name()
,_user_comment()
,_rank_name()
{
}

UserDetailContainFriendIdSprite* UserDetailContainFriendIdSprite::create(){
    
    UserDetailContainFriendIdSprite *sprite = new UserDetailContainFriendIdSprite();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool UserDetailContainFriendIdSprite::init()
{
    if ( !Sprite::initWithFile("init/Home/Friend/user_info_base2.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
 
    return true;
}

void UserDetailContainFriendIdSprite::setupUserData(Json json)
{
    //ユーザデータの保存
    setUserMasterId(json["user_master_id"].string_value());
    setLevel(json["level"].string_value());
    setFriendCardId(json["friend_card_id"].string_value());
    setFriendId(json["friend_id"].string_value());
    setUserName(json["user_name"].string_value());
    setUserComment(json["comment"].string_value());
    setRankName(json["rank_name"].string_value());
}

void UserDetailContainFriendIdSprite::createFriendCard()
{
    //フレンドカードの作成
    string card_pass = "download/Chara/all400/" + _friend_card_id + ".png";
    auto friendCard = Sprite::create(card_pass.c_str());
    addChild(friendCard, 1);
    
    //画像のサイズを150×150に統一する処理
    Size imgSize = friendCard->getContentSize();
    friendCard->setScaleX(150 / imgSize.width);
    friendCard->setScaleY(150 / imgSize.height);
    friendCard->setPosition(Point(123, 240));
}

void UserDetailContainFriendIdSprite::createFriendId()
{
    //フレンドIDの作成
    string friend_id_text = "ID " + _friend_id;
    auto friend_id = HMSFontUtil::createWithTTFBySystemFontArgument(friend_id_text, FONT_NAME_SYSTEM, 30);
    friend_id->setHorizontalAlignment(TextHAlignment::CENTER);
    friend_id->setAnchorPoint(Point(0.5, 0.5));
    friend_id->setPosition(Point(393, 180));
    this->addChild(friend_id, 3);
}

void UserDetailContainFriendIdSprite::showUserData()
{
    //フレンドカードの作成
    createFriendCard();
    
    //フレンドIDの作成
    createFriendId();
    
    //階級名
    auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_name, FONT_NAME_SYSTEM, 30);
    rank_label->setAnchorPoint(Point(0.5, 0));
    rank_label->setHorizontalAlignment(TextHAlignment::CENTER);
    rank_label->setPosition(Point(480, 269));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    rank_label->setColor(Color3B(255, 204, 0));
    this->addChild(rank_label, 1);

    //文字「Lv.」
    auto lavel_lv = HMSFontUtil::createWithTTFBySystemFontArgument("Lv.", FONT_NAME_SYSTEM, 20);
    lavel_lv->setHorizontalAlignment(TextHAlignment::RIGHT);
    lavel_lv->setAnchorPoint(Point(1, 0));
    lavel_lv->setColor(Color3B(255, 204, 0));
    lavel_lv->setPosition(Point(293, 269));
    this->addChild(lavel_lv) ;
    
    //レベル
    auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument(_level, FONT_NAME_SYSTEM, 30);
    level_label->setHorizontalAlignment(TextHAlignment::LEFT);
    level_label->setAnchorPoint(Point(0, 0));
    level_label->setPosition(Point(293, 269));
    level_label->setColor(Color3B(255, 204, 0));
    this->addChild(level_label, 1);
    
    //フレンドユーザー名
    auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_name.c_str(), FONT_NAME_SYSTEM, 30);
    name_label->setHorizontalAlignment(TextHAlignment::CENTER);
    name_label->setPosition(Point(393, 214));
    name_label->setAnchorPoint(Point(0.5, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(name_label, 2);
    
//    //ユーザコメント
//    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("コメント", FONT_NAME_SYSTEM, 23);
//    label->setHorizontalAlignment(TextHAlignment::CENTER);
//    label->setPosition(Point(446, 184));
////enableOutline(Color4B(0, 0, 0, 255), 1);
//    this->addChild(label, 2);
    
    if (_user_comment == "")
    {
        _user_comment = "よろしくお願いします。";
    }
    auto comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_comment, FONT_NAME_SYSTEM, 21);
    comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
    comment_label->setDimensions(512, 100);
    comment_label->setAnchorPoint(Point(0, 1.0f));
    comment_label->setPosition(Point(47, 146));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(comment_label, 2);
}

void UserDetailContainFriendIdSprite::onEnter()
{
    Sprite::onEnter();
    
    //ユーザ情報取得通信
//    getUserDataRequest();
}
