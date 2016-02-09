#include "HelperSprite.h"

HelperSprite::HelperSprite()
:_user_master_id ()
,_card_image_id ()
,_rank_name ()
,_user_name ()
,_last_play_date()
,_follow_flg(false)
,_json()
{
}

HelperSprite* HelperSprite::create(json11::Json json){
    
    HelperSprite *sprite = new HelperSprite();
    
    if (sprite && sprite->init(json))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool HelperSprite::init(json11::Json json)
{
    if ( !Sprite::initWithFile("init/Home/Quest/popup_friend_green.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //初期情報の設定
    setupData(json);
    
    return true;
}

void HelperSprite::setupData(json11::Json json)
{
    //ユーザー情報の設定
    setUserMasterId(json["user_master_id"].string_value());
    setCardImageId(json["card_image_id"].string_value());
    setRankName(json["rank_name"].string_value());
    setUserName(json["user_name"].string_value());
	setUserLevel(json["level"].string_value());
    setFriendId(json["friend_id"].string_value());
    setLastPlayDate(json["last_play_date"].string_value());
    setComment(json["comment"].string_value());
    setFollowFlg(json["follow_flg"].bool_value());
	setCharaType(json["chara_type"].string_value());
	setCharaParam1(json["chara_param1"].string_value());
	setCharaParam2(json["chara_param2"].string_value());
    
    setJson(json.dump());
}

void HelperSprite::showCardImage()
{
    string card_pass = "download/Chara/parts100/card_bk1_reverse.png";
    if(_card_image_id != "") {
        card_pass = "download/Chara/thumb100/"+_card_image_id+".png";
    }
    
    _card_image = Sprite::create(card_pass);
    _card_image->setPosition(Point(70, this->getContentSize().height /2));
    addChild(_card_image, 1);
}

void HelperSprite::showData()
{
    //カードの表示
    showCardImage();
	
	if (_follow_flg) {
		//フォロー中画像
		auto follow_sp = Sprite::create("init/Home/Quest/follow.png");
		follow_sp->setPosition(Point(_card_image->getContentSize().width / 2 - 5, 0));
		_card_image->addChild(follow_sp, 1);
	}
	
    if(_rank_name != ""){
        //レベル(文字)
		auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument("Lv.", FONT_NAME_SYSTEM, 16);
		level_label->setTextColor(Color4B(255, 204, 0, 255));
		level_label->setAnchorPoint(Point(0, 0.5));
		level_label->setPosition(Point(125, 100));
        level_label->enableOutline(Color4B(0, 0, 0, 255), 2) ;
		this->addChild(level_label, 1);
		
		//ユーザーレベル
		auto user_level_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_level, FONT_NAME_SYSTEM, 24);
		user_level_label->setTextColor(Color4B(255, 204, 0, 255));
		user_level_label->setAnchorPoint(Point(0, 0.5));
		user_level_label->setPosition(Point(155, 100));
        user_level_label->enableOutline(Color4B(0, 0, 0, 255), 2) ;
		this->addChild(user_level_label, 1);
		
        //階級名
        auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_name, FONT_NAME_SYSTEM, 24);
		rank_label->setTextColor(Color4B(255, 204, 0, 255));
        rank_label->setAnchorPoint(Point(0, 0.5));
        rank_label->setPosition(Point(125, 75));
        rank_label->enableOutline(Color4B(0, 0, 0, 255), 2) ;
        this->addChild(rank_label, 1);
        
        //5文字以上の場合はスケールを小さくする
        if (calcStrLength(_rank_name) == 5) {
            rank_label->setScale(0.8);
        }
        else if (calcStrLength(_rank_name) >= 6) {
            rank_label->setScale(0.7);
        }
    }
    
    if(_user_name != ""){
        //ユーザー名
        auto name_label = HMSFontUtil::createWithTTFBySystemFontArgument(_user_name.c_str(), FONT_NAME_SYSTEM, 33);
        name_label->setHorizontalAlignment(TextHAlignment::LEFT);
        name_label->setPosition(Point(230, 65));
        name_label->setAnchorPoint(Point(0, 0));
        name_label->enableOutline(Color4B(0, 0, 0, 255), 2);
        this->addChild(name_label, 2);
    }
	
    //最終ログイン日時
    auto login_label = HMSFontUtil::createWithTTFBySystemFontArgument("最終ログイン", FONT_NAME_SYSTEM , 18);
    login_label->setAnchorPoint(Point(1.0f, 1.0f));
    login_label->setPosition(Point(this->getContentSize().width - 30, this->getContentSize().height - 13));
    login_label->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(login_label, 2);
    
    string login_text = parseLastPlayDate();
    auto login_label2 = HMSFontUtil::createWithTTFBySystemFontArgument(login_text.c_str(), FONT_NAME_SYSTEM , 23);
    login_label2->setAnchorPoint(Point(0.5f, 0.5f));
    login_label2->setPosition(Point(550, 78));
	login_label2->enableOutline(Color4B(0, 0, 0, 255), 2);
    this->addChild(login_label2, 2);
	
	//フレンドカードのパラメータ
	if(_chara_type != ""){
		//キャラタイプによる表示画像
		auto type_sp = Sprite::create("init/Home/Quest/popup_friend_green" + _chara_type + ".png");
		type_sp->setAnchorPoint(Point::ZERO);
		type_sp->setPosition(Point::ZERO);
		this->addChild(type_sp, 2);
		
		//パラメータ1
		string chara_param1 = "";
		if(_chara_param1 != "") chara_param1 = _chara_param1;
		else chara_param1 = "0";//保険
		auto param_label1 = HMSFontUtil::createWithTTFBySystemFontArgument(chara_param1, FONT_NAME_SYSTEM, 33);
		param_label1->setAnchorPoint(Point::ZERO);
		if(_chara_type=="3")param_label1->setPosition(Point(300, 15)); //軍師の場合継続ターンの能力値を20ピクセルずらす
        else param_label1->setPosition(Point(280, 15));
        param_label1->enableOutline(Color4B(0, 0, 0, 255), 2);
		type_sp->addChild(param_label1, 1);
		
		//パラメータ2
		string chara_param2 = "";
		if(_chara_param2 != "") chara_param2 = _chara_param2;
		else chara_param2 = "0";//保険
		auto param_label2 = HMSFontUtil::createWithTTFBySystemFontArgument(chara_param2, FONT_NAME_SYSTEM, 33);
		param_label2->setAnchorPoint(Point::ZERO);
		param_label2->setPosition(Point(530, 15));
        param_label2->enableOutline(Color4B(0, 0, 0, 255), 2);
		type_sp->addChild(param_label2, 1);
	}
}

string HelperSprite::parseLastPlayDate()
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

void HelperSprite::onEnter()
{
    Sprite::onEnter();
    
    //データの表示
    showData();
}
