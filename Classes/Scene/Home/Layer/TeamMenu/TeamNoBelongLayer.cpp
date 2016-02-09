#include "TeamNoBelongLayer.h"
#include "HmsGeometricFunction.h"
#include "NormalScene.h"
#include "HmsTemplateFunction.h"
#include "Utility.h"
#include "SSPlayer.h"

USING_NS_CC;

bool TeamNoBelongLayer::init()
{
    if(!Layer::init()) {
        return false;
    }
    HMSLOG("TeamNoBelongLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamNoBelongLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チームトップ", "Mypage", this);
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamNoBelongLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamNoBelongLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamNoBelongLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamNoBelongLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    getTeamData();
}

void TeamNoBelongLayer::getTeamData()
{
    string url = HOST;
    url += "team/get_team_data.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    
    //HMSLOG("url=%s", url.c_str());
    auto selector = httpresponse_selector(TeamNoBelongLayer::getTeamDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamNoBelongLayer::getTeamDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    
    Json json = Json::parse(result, err);
    int team_id = json["team_id"].int_value();
    
    if(team_id!=SaveData::loadInt("team_id")) {
        SaveData::saveInt("team_id", team_id);
        
        if(team_id>0) {
            //メッセージ表示
            popupChangePageLayer *layer = popupChangePageLayer::create("チーム所属状態が変更されました。", "TeamTop");
            this->addChild(layer, 9999);
            
            return;
        }
    }
    
    if(team_id==0) {
        setup();
    } else {
        //メッセージ表示
        popupChangePageLayer *layer = popupChangePageLayer::create("チームに所属中です。", "TeamTop");
        this->addChild(layer, 9999);
    }
}

void TeamNoBelongLayer::setup()
{
    //説明画像1
    Sprite *detail = Sprite::create("init/Team/no_belong/top/popup_614_246.png");
    detail->setAnchorPoint(Point(0.5, 1)) ;
    detail->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT-HEADER_SIZE-TITLE_BAR_SIZE), 0.5, 1));
    detail->setScale(getVisibleRatio());
    this->addChild(detail);
    
    //文字列の追加
    createLabels(detail) ;
    
    //参加できるチームの数は1つです
    Label *team_detail_label_line4 = HMSFontUtil::createWithTTFBySystemFontArgument("※参加できるチームの数は1つです", FONT_NAME_SYSTEM, 23);
    team_detail_label_line4->setTextColor(Color4B::RED) ;
    team_detail_label_line4->setAnchorPoint(Point(0.5,0.5)) ;
    team_detail_label_line4->setHorizontalAlignment(TextHAlignment::CENTER) ;
    team_detail_label_line4->setPosition(Point(detail->getContentSize().width/2, 40)) ;
    detail->addChild(team_detail_label_line4) ;
    
    //新規ギルド作成
    Sprite *create = Sprite::create("init/Team/no_belong/top/bt_create.png");
    create->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2-40), 0.5, 0.5));
    create->setScale(getVisibleRatio());
    this->addChild(create, 1, 1);
    
    //ギルド検索
    Sprite *search = Sprite::create("init/Team/no_belong/top/bt_search.png");
    search->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2-40-90*1), 0.5, 0.5));
    search->setScale(getVisibleRatio());
    this->addChild(search, 1, 2);
    
    //申請中ギルド
    Sprite *app = Sprite::create("init/Team/no_belong/top/bt_app.png");
    app->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2-40-90*2), 0.5, 0.5));
    app->setScale(getVisibleRatio());
    this->addChild(app, 1, 3);
    
    //勧誘中ギルド
    Sprite *soli = Sprite::create("init/Team/no_belong/top/bt_soli.png");
    soli->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2-40-90*3), 0.5, 0.5));
    soli->setScale(getVisibleRatio());
    this->addChild(soli, 1, 4);
    
    //勧誘があれば表示
    HmsList invitation_list;
    invitation_list.import_tsv(SaveData::loadString("team_invitation_list"));
    if(invitation_list.size()>0) {
        unsigned char* filebuffer = 0;
        
        SSImageList* image_list1;
        SSPlayer* anime1;
        
        SSPlayerHelper::createFromFile( &filebuffer , &anime1 , &image_list1 , "request.ssba" ,"download/SpriteStudio/Team/");
        anime1->setPosition(Point(135,70));
        //anime1->setScale(2.0f);
        soli->addChild(anime1, 1);
    }
    
    //ヘルプボタン
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, CC_CALLBACK_1(TeamNoBelongLayer::onTouchButton, this));
    help_btn->setScale(ratio);
    help_btn->setAnchorPoint(Point(0.5, 1));
    help_btn->setTag(T_HELP_BUTTON) ;
    auto Menu3 = Menu::create(help_btn, nullptr);
    Menu3->setPosition(convertFromDisplaySize(Point(570, 838), 0.5, 1));
    this->addChild(Menu3, 5);
}

void TeamNoBelongLayer::createLabels(Node *parent_sprite)
{
    //現在、チームに参加していません
    Label *team_detail_label_line1 = HMSFontUtil::createWithTTFBySystemFontArgument("現在、チームに参加していません", FONT_NAME_SYSTEM, 35);
    team_detail_label_line1->setAnchorPoint(Point(0.5,0.5)) ;
    team_detail_label_line1->setHorizontalAlignment(TextHAlignment::CENTER) ;
    team_detail_label_line1->setPosition(Point(parent_sprite->getContentSize().width/2, 180)) ;
    parent_sprite->addChild(team_detail_label_line1) ;
    
    //「新規チームを結成する」
    Label *team_detail_label_line2_left = HMSFontUtil::createWithTTFBySystemFontArgument("「新規チームを結成する」", FONT_NAME_SYSTEM, 25);
    team_detail_label_line2_left->setTextColor(Color4B::YELLOW) ;
    team_detail_label_line2_left->setAnchorPoint(Point(1,0.5)) ;
    team_detail_label_line2_left->setHorizontalAlignment(TextHAlignment::RIGHT) ;
    team_detail_label_line2_left->setPosition(Point(330, 120)) ;
    parent_sprite->addChild(team_detail_label_line2_left) ;
    
    //か
    Label *team_detail_label_line2_right = HMSFontUtil::createWithTTFBySystemFontArgument("か", FONT_NAME_SYSTEM, 25);
    team_detail_label_line2_right->setAnchorPoint(Point(0,0.5)) ;
    team_detail_label_line2_right->setHorizontalAlignment(TextHAlignment::LEFT) ;
    team_detail_label_line2_right->setPosition(Point(330, 120)) ;
    parent_sprite->addChild(team_detail_label_line2_right) ;
    
    //「既存チームに参加する」
    Label *team_detail_label_line3_left = HMSFontUtil::createWithTTFBySystemFontArgument("「既存チームに参加する」", FONT_NAME_SYSTEM, 25);
    team_detail_label_line3_left->setTextColor(Color4B::YELLOW) ;
    team_detail_label_line3_left->setAnchorPoint(Point(1,0.5)) ;
    team_detail_label_line3_left->setHorizontalAlignment(TextHAlignment::RIGHT) ;
    team_detail_label_line3_left->setPosition(Point(330, 90)) ;
    parent_sprite->addChild(team_detail_label_line3_left) ;
    
    //を選択してください
    Label *team_detail_label_line3_right = HMSFontUtil::createWithTTFBySystemFontArgument("を選択してください", FONT_NAME_SYSTEM, 25);
    team_detail_label_line3_right->setAnchorPoint(Point(0,0.5)) ;
    team_detail_label_line3_right->setHorizontalAlignment(TextHAlignment::LEFT) ;
    team_detail_label_line3_right->setPosition(Point(330, 90)) ;
    parent_sprite->addChild(team_detail_label_line3_right) ;
}


bool TeamNoBelongLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamNoBelongLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamNoBelongLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    NormalScene *normal_scene = (NormalScene*)this->getParent();
    
    Sprite *img;
    Rect rect;
    
    if(this->getChildByTag(1)==nullptr) return;

    img = (Sprite*)this->getChildByTag(1);
    rect = img->getBoundingBox();getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        normal_scene->changePage("TeamCreateTeam");
        return;
    }
    
    img = (Sprite*)this->getChildByTag(2);
    rect = img->getBoundingBox();getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        normal_scene->changePage("TeamSearchTeam");
        return;
    }
    
    img = (Sprite*)this->getChildByTag(3);
    rect = img->getBoundingBox();getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        normal_scene->changePage("TeamApplication");
//        string url = HOST;
//        url += "team/team_search.php";
//        url += "?user_master_id=" + SaveData::loadString("user_master_id");
//        url += "&search_type=petition";
//        
//        HMSLOG("url=%s", url.c_str());
//        auto selector = httpresponse_selector(TeamNoBelongLayer::petitionCallback);
//        HttpConnect::sendHttpRequest(this, url, selector);
//        return;
    }

    img = (Sprite*)this->getChildByTag(4);
    rect = img->getBoundingBox();getBoundingBox();
    if(rect.containsPoint(touch->getLocation())) {
        normal_scene->changePage("TeamInvited");
//        string url = HOST;
//        url += "team/team_search.php";
//        url += "?user_master_id=" + SaveData::loadString("user_master_id");
//        url += "&search_type=invitation";
//        
//        HMSLOG("url=%s", url.c_str());
//        auto selector = httpresponse_selector(TeamNoBelongLayer::invitationCallback);
//        HttpConnect::sendHttpRequest(this, url, selector);
//        return;
    }
}

void TeamNoBelongLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void TeamNoBelongLayer::petitionCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    //Json json = Json::parse(result, err);
    
    if(result=="") {
        string text = "申請中のチームはありません。";
        popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
        this->addChild(comment, 9999);
    } else {
        SaveData::saveString("team_app_result", result);
        SaveData::saveString("team_list_mode", "app");
        NormalScene *normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("TeamList");
    }
}

void TeamNoBelongLayer::invitationCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    //Json json = Json::parse(result, err);
    
    if(result=="") {
        string text = "勧誘中のチームはありません。";
        popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
        this->addChild(comment, 9999);
    } else {
        SaveData::saveString("team_soli_result", result);
        SaveData::saveString("team_list_mode", "soli");
        NormalScene *normal_scene = (NormalScene*)this->getParent();
        normal_scene->changePage("TeamList");
    }
}

void TeamNoBelongLayer::onTouchButton(Ref *sender)
{
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();
    
    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == T_HELP_BUTTON)
    {
        string url = HOST;
        url += "user/index.php?&p=page__team_help&user_id=" + SaveData::loadString("user_master_id");
        WebViewLoadLayer *web_view_load_layer = WebViewLoadLayer::create(url, "チームヘルプ");
        normal_scene->addChild(web_view_load_layer, 9);
    }
}