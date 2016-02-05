#include "FollowSprite.h"

FollowSprite::FollowSprite()
:_user_master_id ()
,_friend_id ()
,_friend_card_id ()
,_friend_name ()
,_level()
,_last_play_date ()
,_user_comment ()
,_rank_name ()
{
}

FollowSprite* FollowSprite::create(json11::Json json, int count){
    
    FollowSprite *sprite = new FollowSprite();
    
    
    if (sprite && sprite->init(json, count))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool FollowSprite::init(json11::Json json, int count)
{
    if ( !Sprite::initWithFile("init/Home/Friend/popup_friend.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json, count);
    
    return true;
}

void FollowSprite::setupData(json11::Json json, int count)
{
    //jsonデータをメンバ変数に設定
    string num = toString<int>(count);
    
    //フレンド情報の設定
    setUserMasterId(json[num]["user_master_id"].string_value());
    setFriendId(json[num]["friend_id"].string_value());
    setFriendCardId(json[num]["friend_card_id"].string_value());
    setLevel(json[num]["level"].string_value());
    setFriendName(json[num]["user_name"].string_value());
    setLastPlayDate(json[num]["last_play_date"].string_value());
    setUserComment(json[num]["comment"].string_value());
    setRankName(json[num]["rank_name"].string_value());
}

void FollowSprite::createFriendCard()
{
    //フレンドカードの作成
    string card_pass = "download/Chara/thumb100/" + _friend_card_id + ".png";
    if (_friend_card_id == "")
    {
        card_pass = "download/Chara/parts100/card_bk1_reverse.png";
    }
    friendCard = Sprite::create(card_pass);
    addChild(friendCard, 1);
}

void FollowSprite::showfriendCard()
{
//    //画像のサイズを86×86に統一する処理
//    Size imgSize = friendCard->getContentSize();
//    friendCard->setScaleX((86 / imgSize.width) );
//    friendCard->setScaleY((86 / imgSize.height) );
    
    friendCard->setPosition(Point(70, this->getContentSize().height /2));
}

void FollowSprite::showData()
{
//    //フレンドID
//    string friend_id_text = "ID:" + _friend_id;
//    auto friend_id = HMSFontUtil::createWithTTFBySystemFontArgument(friend_id_text, FONT_NAME_SYSTEM, 22);
//    friend_id->setHorizontalAlignment(TextHAlignment::RIGHT);
//    friend_id->setAnchorPoint(Point(1.0, 1.0));
//    friend_id->setPosition(Point(616, 116));
////enableOutline(Color4B(0, 0, 0, 255), 1);
//    this->addChild(friend_id, 2);
    
    //レベル
    auto level_label1 = HMSFontUtil::createWithTTFBySystemFontArgument("Lv.", FONT_NAME_SYSTEM, 16);
    level_label1->setTextColor(Color4B(255, 204, 0, 255));
    level_label1->setAnchorPoint(Point(0, 0.5));
    level_label1->setPosition(Point(130, 100));
    level_label1->enableOutline(Color4B(0, 0, 0, 255), 2) ;
    this->addChild(level_label1, 1);
    
    
    auto level = HMSFontUtil::createWithTTFBySystemFontArgument(_level, FONT_NAME_SYSTEM, 24);
    level->setTextColor(Color4B(255, 204, 0, 255));
    level->setAnchorPoint(Point(0, 0.5));
    level->setPosition(Point(166, 100));
    level->enableOutline(Color4B(0, 0, 0, 255), 2) ;
    this->addChild(level, 1);
    
    //階級名
    auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_name, FONT_NAME_SYSTEM, 25);
    rank_label->setTextColor(Color4B(255, 204, 0, 255));
    rank_label->setAnchorPoint(Point(0, 0.5));
    rank_label->setPosition(Point(130, 74));
    rank_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(rank_label, 1);
    
    //5文字以上の場合はスケールを小さくする
    if (calcStrLength(_rank_name) == 5) {
        rank_label->setScale(0.8);
    }
    else if (calcStrLength(_rank_name) >= 6) {
        rank_label->setScale(0.7);
    }
    
    //ユーザー名
    auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_friend_name.c_str(), FONT_NAME_SYSTEM, 33);
    name_label->setHorizontalAlignment(TextHAlignment::LEFT);
    name_label->setPosition(Point(230, 86));
    name_label->setAnchorPoint(Point(0, 0.5));
    name_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(name_label, 2);
    
    //最終ログイン日時
    auto login_label = HMSFontUtil::createWithTTFBySystemFontArgument("最終ログイン", FONT_NAME_SYSTEM , 18);
    login_label->setAnchorPoint(Point(0.5, 0.5));
    login_label->setPosition(Point(560, 100));
    login_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(login_label, 2);
    
    string login_text = parseLastPlayDate();
    auto login_label2 = HMSFontUtil::createWithTTFBySystemFontArgument(login_text.c_str(), FONT_NAME_SYSTEM , 20);
    login_label2->setAnchorPoint(Point(0.5, 0.5));
    login_label2->setPosition(Point(560, 72));
    login_label2->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(login_label2, 2);
    
    //フレンドユーザーコメント
    string comment = _user_comment;
    if (calcStrLength(comment) >= 18 ) {
        comment = calcStrSub(_user_comment, 18);
        comment += "...";
    }
    
    auto comment_label = HMSFontUtil::createWithTTFBySystemFontArgument(comment, FONT_NAME_SYSTEM , 24);
    comment_label->setDimensions(470, 38);
    comment_label->setAnchorPoint(Point(0, 0.5f));
    comment_label->setPosition(Point(130, 28));
    comment_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(comment_label, 2);
}

string FollowSprite::parseLastPlayDate()
{
    //last_play_dateの分解処理
    string last_play_date = getLastPlayDate();
    
    //現在時刻の取得
    time_t now_time;
    time(&now_time);
    
    //最終ログイン日時のタイムスタンプ取得
    struct tm tm;
    strptime(last_play_date.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
    time_t last_time = mktime(&tm);
    
    //現在時刻から最終ログイン日時の差分を取得
    time_t diff_time = difftime(now_time, last_time);
    HMSLOG("diff_time = %ld", diff_time);
    
    string text = "";
    if (0 < diff_time && diff_time < DAY_1_SECOND)
    {
        //一日経過前の処理
        int hour = (int)diff_time / (60 * 60); //時
        if (hour < 1)
        {
            //最終ログインが１時間以内だったら１時間に設定する
            hour = 1;
        }
        text = toString<int>(hour) + "時間前";
    }
    else
    {
        //一日経過後の処理
        int day = (int)diff_time / ((60 * 60) * 24);
        
        if (day > 999 || day < 0)
        {
            //999日以上の場合999日にする
            day = 999;
        }
        
        text = toString<int>(day) + "日前";
    }
    
    return text;
}

void FollowSprite::onEnter()
{
    Sprite::onEnter();
    
    //フレンドカードの作成
    createFriendCard();
    
    //フレンドカードの表示
    showfriendCard();
    
    //フレンドデータの表示
    showData();
}
