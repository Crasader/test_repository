#include "ProfileLayer.h"

bool ProfileLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ProfileLayer::init()");

    ratio = getVisibleRatio();

    return true;
}
void ProfileLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("プロフィール", "SubMenu", this);
    
    //プロフィールの情報を取得
    profileRequest();
}

void ProfileLayer::setupImage()
{
    //背景
    auto bg = Sprite::create("init/Home/Friend/user_info_base.png");
    bg->setScale(ratio);
    bg->setTag(P_BG_TAG);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(bg, 1);

    bgsize = bg->getContentSize();

    //コメント編集画面遷移ボタン
    auto comment_off = Sprite::create("init/Home/Common/bt2.png");
    auto comment_on  = Sprite::create("init/Home/Common/bt2.png");
    comment_on->setColor(Color3B(180, 180, 180));
    
    auto text_off = HMSFontUtil::createWithTTFBySystemFontArgument("コメント編集", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    text_off->setPosition(Point(comment_off->getContentSize() / 2));
//enableOutline(Color4B(0, 0, 0, 255), 2);
    comment_off->addChild(text_off, 1);
    
    auto text_on = HMSFontUtil::createWithTTFBySystemFontArgument("コメント編集", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    text_on->setPosition(Point(comment_on->getContentSize() / 2));
//enableOutline(Color4B(0, 0, 0, 255), 2);
    comment_on->addChild(text_on, 1);
    
    auto comment_btn = MenuItemSprite::create(comment_off, comment_on, CC_CALLBACK_1(ProfileLayer::changePage, this));
    comment_btn->setScale(ratio);
    comment_btn->setTag(COMMENT_TAG);


    //ニックネーム変更遷移ボタン
    auto name_off = Sprite::create("init/Home/Common/bt2.png");
    auto name_on  = Sprite::create("init/Home/Common/bt2.png");
    name_on->setColor(Color3B(180, 180, 180));
    
    auto text_name_off = HMSFontUtil::createWithTTFBySystemFontArgument("名前変更", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    text_name_off->setPosition(Point(name_off->getContentSize() / 2));
//enableOutline(Color4B(0, 0, 0, 255), 2);
    name_off->addChild(text_name_off, 1);
    
    auto text_name_on = HMSFontUtil::createWithTTFBySystemFontArgument("名前変更", FONT_NAME_SYSTEM, DEFAULT_FONT_SIZE);
    text_name_on->setPosition(Point(name_on->getContentSize() / 2));
//enableOutline(Color4B(0, 0, 0, 255), 2);
    name_on->addChild(text_name_on, 1);
    
    auto name_btn = MenuItemSprite::create(name_off, name_on, CC_CALLBACK_1(ProfileLayer::changePage, this));
    name_btn->setScale(ratio);
    name_btn->setTag(NICKNAME_TAG);
    
    auto menu = Menu::create(name_btn, comment_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(30 * ratio);
    menu->setPosition(convertFromDisplaySize(Point(320, 180), 0.5, 0.5));
    this->addChild(menu, 1);
}

//プロフィール
void ProfileLayer::profileRequest(){
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += PROFILE_NAME_URL;
    auto selector = httpresponse_selector(ProfileLayer::nameCallback);

    string body = "user_master_id=";
    body += user_master_id;//ユーザーマスターID(string型)
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

//プロフィールのレスポンスを表示する
void ProfileLayer::nameCallback(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    SaveData::saveString("user_name_change_span", json["span"].string_value());

    //画像の設定
    setupImage();
    
    //addChildする対象を取得
    Sprite* popup = (Sprite*)this->getChildByTag(P_BG_TAG);//中心 285,214
    HMSLOG("%f", ratio);

    //階級名
    auto rank_label = HMSFontUtil::createWithTTFBySystemFontArgument(json["rank_name"].string_value(), FONT_NAME_SYSTEM, 30);
    rank_label->setAnchorPoint(Point(0.5, 0));
    rank_label->setPosition(Point(520, 264));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    rank_label->setColor(Color3B(255, 204, 0));
    popup->addChild(rank_label, 1);

    //文字「Lv.」
    auto lavel_lv = HMSFontUtil::createWithTTFBySystemFontArgument("Lv.", FONT_NAME_SYSTEM, 20);
    lavel_lv->setAnchorPoint(Point(0, 0));
    lavel_lv->setColor(Color3B(255, 204, 0));
    lavel_lv->setPosition(Point(312, 264));
    popup->addChild(lavel_lv) ;
    
    //レベル
    auto level_label = HMSFontUtil::createWithTTFBySystemFontArgument(json["level"].string_value(), FONT_NAME_SYSTEM, 30);
    level_label->setAnchorPoint(Point(0, 0));
    level_label->setPosition(Point(352, 264));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    level_label->setColor(Color3B(255, 204, 0));
    popup->addChild(level_label, 1);

    //ニックネーム
    auto lb_user_name = HMSFontUtil::createWithTTFBySystemFontArgument(json["user_name"].string_value() , FONT_NAME_SYSTEM, 30);
    lb_user_name->setHorizontalAlignment(TextHAlignment::LEFT);
    lb_user_name->setPosition(Point(290, 210));
    lb_user_name->setAnchorPoint(Point(0, 0));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    popup->addChild(lb_user_name, 10);

    //ユーザコメントタイトル
    auto comment_title = HMSFontUtil::createWithTTFBySystemFontArgument("コメント", FONT_NAME_SYSTEM, 25);
    comment_title->setHorizontalAlignment(TextHAlignment::CENTER);
    comment_title->setPosition(Point(446, 184));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    popup->addChild(comment_title, 10);

    //コメント
    auto lb_comment = HMSFontUtil::createWithTTFBySystemFontArgument(json["comment"].string_value() , FONT_NAME_SYSTEM, 21);
//    lb_comment->setHorizontalAlignment(TextHAlignment::LEFT);
    lb_comment->setDimensions(320, 100);
    lb_comment->setAnchorPoint(Point(0, 1.0f));
    lb_comment->setPosition(Point(286, 164));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    popup->addChild(lb_comment, 10);

    /*
    //フレンドID
    auto lb_friend = HMSFontUtil::createWithTTFBySystemFontArgument(json["friend_id"].string_value() , FONT_NAME_SYSTEM, 22);
    lb_friend->setHorizontalAlignment(TextHAlignment::RIGHT);
    lb_friend->setAnchorPoint(Point(0, 0));
    lb_friend->setPosition(Point(432, 274));
    lb_friend->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    popup->addChild(lb_friend, 10);
    */
     
    //フレンドカードの作成
    string card_pass = "download/Chara/all400/" + json["friend_card_id"].string_value() + ".png";
    auto friendCard = Sprite::create(card_pass.c_str());
    popup->addChild(friendCard, 3);

    //画像のサイズを220×220に統一する処理
    Size imgSize = friendCard->getContentSize();
    friendCard->setScaleX(220 / imgSize.width);
    friendCard->setScaleY(220 / imgSize.height);
    friendCard->setPosition(Point(154, popup->getContentSize().height / 2));
}

void ProfileLayer::changePage(Ref* sender){
    MenuItem* menuItem = (MenuItem *)sender;
    int tag_no = menuItem->getTag();
    if(tag_no == NICKNAME_TAG){
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("Nickname");
    }else
    if(tag_no == COMMENT_TAG){
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("Comment");
    }else{
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("SubMenu");
    }
}
