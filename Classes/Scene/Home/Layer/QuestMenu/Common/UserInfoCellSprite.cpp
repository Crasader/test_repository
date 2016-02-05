#include "UserInfoCellSprite.h"

UserInfoCellSprite::UserInfoCellSprite()
:_user_master_id ()
,_friend_id ()
,_card_image_id ()
,_user_name ()
,_last_play_date ()
,_comment ()
,_rank_name ()
,_battle_point_rate()
,_json()
{
}

UserInfoCellSprite* UserInfoCellSprite::create(json11::Json json){
    return create(json, -1) ;
}

UserInfoCellSprite* UserInfoCellSprite::create(json11::Json json, int count){
    
    UserInfoCellSprite *sprite = new UserInfoCellSprite();
    
    if (sprite && sprite->init(json, count))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool UserInfoCellSprite::init(json11::Json json, int count)
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

void UserInfoCellSprite::setupData(json11::Json json, int count)
{
    if(count==-1){
        //フレンド情報の設定
        setUserMasterId(json["user_master_id"].string_value());
        setFriendId(json["friend_id"].string_value());
        setCardImageId(json["card_image_id"].string_value());
        setUserName(json["user_name"].string_value());
        setLastPlayDate(json["last_play_date"].string_value());
        setComment(json["comment"].string_value());
        setRankName(json["rank_name"].string_value());
        setBattlePointRate(json["battle_point_rate"].string_value());
        setJson(json.dump());
    }else{
        string num = toString<int>(count);
        
        //jsonデータをメンバ変数に設定
        //フレンド情報の設定
        setUserMasterId(json["user_master_id"].string_value());
        setFriendId(json[num]["friend_id"].string_value());
        setCardImageId(json[num]["card_image_id"].string_value());
        setUserName(json[num]["user_name"].string_value());
        setLastPlayDate(json[num]["last_play_date"].string_value());
        setComment(json[num]["comment"].string_value());
        setRankName(json[num]["rank_name"].string_value());
        setBattlePointRate(json[num]["battle_point_rate"].string_value());
        setJson(json[num].dump());
    }
}

void UserInfoCellSprite::showCardImage()
{
    if(_card_image_id != "" && fromString<int>(_card_image_id)>1){
        string card_pass = "download/Chara/all100/"+_card_image_id+".png";
        _card_image = Sprite::create(card_pass);
        //画像のサイズを86×86に統一する処理
        _card_image->setScaleX((86 / _card_image->getContentSize().width));
        _card_image->setScaleY((86 / _card_image->getContentSize().height));
        
        _card_image->setPosition(Point(70, this->getContentSize().height /2));
        addChild(_card_image, 1);
    }else{
        //カードが存在しない場合カードの裏面を見せる
        string card_pass = "download/Chara/parts100/card_bk1_reverse.png";
        _card_image = Sprite::create(card_pass);
        //画像のサイズを86×86に統一する処理
        _card_image->setScaleX((86 / _card_image->getContentSize().width));
        _card_image->setScaleY((86 / _card_image->getContentSize().height));
        
        _card_image->setPosition(Point(70, this->getContentSize().height /2));
        addChild(_card_image, 1);
    }
}

void UserInfoCellSprite::showData()
{
    
    //カードの表示
    showCardImage();
    
    if(_friend_id != "" && fromString<int>(_friend_id)>1){
        //フレンドID
        string friend_id_text = "ID:" + _friend_id;
        auto friend_id = HMSFontUtil::createWithTTFBySystemFontArgument(friend_id_text, FONT_NAME_SYSTEM, 25);
        friend_id->setHorizontalAlignment(TextHAlignment::RIGHT);
        friend_id->setAnchorPoint(Point(1.0f, 0.5f));
        friend_id->setPosition(Point(610, 94));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        this->addChild(friend_id, 2);
    }
    
    if(_rank_name != ""){
        //階級背景
        auto rank_sp = Sprite::create("init/Home/Friend/rank_base.png");
        rank_sp->setAnchorPoint(Point(0, 0));
        rank_sp->setPosition(Point(126, 54));
        this->addChild(rank_sp, 2);
        
        //階級名
        auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_name, FONT_NAME_SYSTEM, 25);
    //    rank_label->setColor(Color3B(255, 204, 0));
        rank_label->setAnchorPoint(Point(0.5, 0.5));
        rank_label->setPosition(Point(rank_sp->getContentSize() / 2));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        rank_sp->addChild(rank_label, 1);
    }
    
    /*
    if(&_battle_point_rate != nullptr && _battle_point_rate != "" && fromString<float>(_battle_point_rate)>1){
        //ポイント倍率背景
        auto point_rate_sp = Sprite::create("init/Home/Quest/battle_pt_base.png");
        point_rate_sp->setAnchorPoint(Point(0, 0));
        point_rate_sp->setPosition(Point(286, 54));
        this->addChild(point_rate_sp, 2);
        Size pointRateSpSize = point_rate_sp->getContentSize() ;
        
        //「ポイント」文字
        auto point_rate_label_1 = HMSFontUtil::createWithTTFBySystemFontArgument("ポイント", FONT_NAME_SYSTEM, 18);
        point_rate_label_1->setAnchorPoint(Point(0, 0));
        point_rate_label_1->setPosition(Point(10, 8));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        point_rate_sp->addChild(point_rate_label_1, 1);
        
        //倍率数値文字
        auto point_rate_label_2 = HMSFontUtil::createWithTTFBySystemFontArgument(_battle_point_rate, FONT_NAME_SYSTEM, 30);
        point_rate_label_2->setAnchorPoint(Point(0.5, 0));
        point_rate_label_2->setPosition(Point(pointRateSpSize.width-50, 3));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        point_rate_sp->addChild(point_rate_label_2, 2);
        
        //「倍」文字
        auto point_rate_label_3 = HMSFontUtil::createWithTTFBySystemFontArgument("倍", FONT_NAME_SYSTEM, 20);
        point_rate_label_3->setAnchorPoint(Point(1, 0));
        point_rate_label_3->setPosition(Point(pointRateSpSize.width-10, 8));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        point_rate_sp->addChild(point_rate_label_3, 1);
        
    }
     */
    
    if(_user_name != ""){
        //フレンドユーザー名
        auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_name.c_str(), FONT_NAME_SYSTEM, 30);
        name_label->setHorizontalAlignment(TextHAlignment::LEFT);
        name_label->setPosition(Point(126, 8));
        name_label->setAnchorPoint(Point(0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        this->addChild(name_label, 2);
    }
    
    if(_last_play_date != ""){
    
        //最終ログイン日時
        auto login_label = HMSFontUtil::createWithTTFBySystemFontArgument("最終ログイン:", FONT_NAME_SYSTEM , 25);
        login_label->setAnchorPoint(Point(1.0f, 0));
        login_label->setPosition(Point(508, 10));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        this->addChild(login_label, 2);
        
        string login_text = parseLastPlayDate(_last_play_date);
        auto login_label2 = HMSFontUtil::createWithTTFBySystemFontArgument(login_text.c_str(), FONT_NAME_SYSTEM , 28);
        login_label2->setAnchorPoint(Point(1.0f, 0));
        login_label2->setPosition(Point(618, 10));
//enableOutline(Color4B(0, 0, 0, 255), 1);
        this->addChild(login_label2, 2);
    }
}

string UserInfoCellSprite::parseLastPlayDate(string lastLogin)
{
    //last_play_dateの分解処理
    string last_play_date = lastLogin ;
    
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
//    HMSLOG("diff_time = %ld", diff_time);
    
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

void UserInfoCellSprite::onEnter()
{
    Sprite::onEnter();
    
    //データの表示
    showData();
}