#include "FriendUserDetailSprite.h"
FriendUserDetailSprite::FriendUserDetailSprite()
:_friend_id()
,_friend_card_id()
,_user_name()
,_level()
,_user_comment()
,_rank_name()
{
}

FriendUserDetailSprite* FriendUserDetailSprite::create(string friend_id, string rank_name, string user_name, string level,string comment, string card_image_id){
    
    FriendUserDetailSprite *sprite = new FriendUserDetailSprite();
    
    if (sprite && sprite->init(friend_id, rank_name, user_name, level, comment, card_image_id))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool FriendUserDetailSprite::init(string friend_id, string rank_name, string user_name, string level,string comment, string card_image_id)
{
    if ( !Sprite::initWithFile("init/Home/Friend/user_info_base.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    setFriendId(friend_id);
    setFriendCardId(card_image_id);
    setUserName(user_name);
    setLevel(level);
    setRankName(rank_name);
    setUserComment(comment);
    
    return true;
}



void FriendUserDetailSprite::createFriendCard()
{
    //フレンドカードの作成
    string card_pass = "download/Chara/all400/" + _friend_card_id + ".png";
    auto friendCard = Sprite::create(card_pass.c_str());
    addChild(friendCard, 1);
    
    //画像のサイズを230×230に統一する処理
    Size imgSize = friendCard->getContentSize();
    friendCard->setScaleX(230 / imgSize.width);
    friendCard->setScaleY(230 / imgSize.height);
    
    friendCard->setPosition(Point(155, this->getContentSize().height / 2));
}

void FriendUserDetailSprite::createFriendId()
{
    //フレンドIDの作成
    string friend_id_text = "ID:" + _friend_id;
    auto friend_id = HMSFontUtil::createWithTTFBySystemFontArgument(friend_id_text, FONT_NAME_SYSTEM, 21);
    friend_id->setHorizontalAlignment(TextHAlignment::RIGHT);
    friend_id->setAnchorPoint(Point(0, 0));
    friend_id->setPosition(Point(432, 274));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(friend_id, 3);
}

void FriendUserDetailSprite::showUserData()
{
    //フレンドカードの作成
    createFriendCard();
    
    //フレンドIDの作成
//    createFriendId();
    
    //タイトル(「フレンド申請」という枠付きの画像)
    auto title_sp = Sprite::create("init/Dungeon/result/title_friend.png") ;
    title_sp->setScale(0.75f) ;
    title_sp->setPosition(Point(getContentSize().width/2, getContentSize().height-30)) ;
    title_sp->setAnchorPoint(Point(0.5, 0.5));
    this->addChild(title_sp) ;
    
    //階級名
    auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_name, FONT_NAME_SYSTEM, 30);
    rank_label->setAnchorPoint(Point(0.5, 0));
    rank_label->setPosition(Point(520, 264));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    rank_label->setColor(Color3B(255, 204, 0));
    this->addChild(rank_label, 1);

    //文字「Lv.」
    auto lavel_lv = HMSFontUtil::createWithTTFBySystemFontArgument("Lv.", FONT_NAME_SYSTEM, 20);
    lavel_lv->setAnchorPoint(Point(0, 0));
    lavel_lv->setColor(Color3B(255, 204, 0));
    lavel_lv->setPosition(Point(312, 264));
    this->addChild(lavel_lv) ;
    
    //レベル
    auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument(_level, FONT_NAME_SYSTEM, 30);
    level_label->setAnchorPoint(Point(0, 0));
    level_label->setPosition(Point(352, 264));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    level_label->setColor(Color3B(255, 204, 0));
    this->addChild(level_label, 1);
    
    //フレンドユーザー名
    auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_name.c_str(), FONT_NAME_SYSTEM, 30);
    name_label->setHorizontalAlignment(TextHAlignment::LEFT);
    name_label->setPosition(Point(290, 214));
    name_label->setAnchorPoint(Point(0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(name_label, 2);
    
    //ユーザコメント
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("コメント", FONT_NAME_SYSTEM, 23);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(446, 184));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(label, 2);
    
    if (_user_comment == "")
    {
        _user_comment = "よろしくお願いします。";
    }
    auto comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_comment, FONT_NAME_SYSTEM, 21);
    comment_label->setHorizontalAlignment(TextHAlignment::LEFT);
    comment_label->setDimensions(330, 90);
    comment_label->setAnchorPoint(Point(0, 1.0f));
    comment_label->setPosition(Point(286, 160));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(comment_label, 2);
}

void FriendUserDetailSprite::onEnter()
{
    Sprite::onEnter();
    
    //ユーザーデータの作成
    showUserData() ;
}