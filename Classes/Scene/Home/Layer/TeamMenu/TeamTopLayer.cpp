#include "TeamTopLayer.h"

USING_NS_CC;

#define TAG_MEMBER 11
//#define TAG_CHAT 12
#define TAG_MANAGEMENT 13
#define TAG_BANNER 21

bool TeamTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamTopLayer::onEnter()
{
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チームトップ", "Mypage", this);
    
    Layer::onEnter();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamTopLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamTopLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamTopLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamTopLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    //ダンジョンデータの初期化
    Utility::initDungeonData();
    
    getTeamData();
}

void TeamTopLayer::getTeamData()
{
    string url = HOST;
    url += "team/get_team_data.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    
    //HMSLOG("url=%s", url.c_str());
    auto selector = httpresponse_selector(TeamTopLayer::getTeamDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamTopLayer::getTeamDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;

    Json json = Json::parse(result, err);
    int team_id = json["team_id"].int_value();
    int team_admin_user_master_id = json["team_admin_user_master_id"].int_value();
    _is_boss_event_open = json["is_boss_event_open"].bool_value();
    _boss_event_data = json["boss_event_data"][0].object_items() ;
    _boss_count = json["boss_count"].int_value();
  
    if(team_id!=SaveData::loadInt("team_id")) {
        SaveData::saveInt("team_id", team_id);
        
        if(team_id==0) {
            //メッセージ表示
            popupChangePageLayer *layer = popupChangePageLayer::create("チーム所属状態が変更されました。", "TeamNoBelong");
            this->addChild(layer, 9999);
            
            return;
        }
    }
    
    if(team_id>0) {
        SaveData::saveInt("team_admin_user_master_id", team_admin_user_master_id);
        if(SaveData::loadString("user_master_id")==toString(team_admin_user_master_id)) {
            SaveData::saveBool("team_leader_flg", true);
        } else {
            SaveData::saveBool("team_leader_flg", false);
        }
        setup();
    } else {
        //メッセージ表示
        popupChangePageLayer *layer = popupChangePageLayer::create("チームに所属していません。", "TeamNoBelong");
        this->addChild(layer, 9999);
    }
}

void TeamTopLayer::setup()
{
    //キャラ画像
    Sprite *team_chara = Sprite::create("init/Team/top/team_chara.png");
    team_chara->setAnchorPoint(Point(0.5, 0)) ;
    team_chara->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, FOOTER_SIZE + 304), 0.5, 0.0));
    team_chara->setScale(ratio);
    this->addChild(team_chara, 1);
    
    //管理
    Sprite *managenent = Sprite::create("init/Team/top/bt_management.png");
    managenent->setAnchorPoint(Point(0.5, 0)) ;
    managenent->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/6 * 2, FOOTER_SIZE + 200), 0.5, 0.0));
    managenent->setScale(ratio);
    this->addChild(managenent, 1, TAG_MANAGEMENT);
    
    //メンバー
    Sprite *member = Sprite::create("init/Team/top/bt_menber.png");
    member->setAnchorPoint(Point(0.5, 0)) ;
    member->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/6 * 4, FOOTER_SIZE + 200), 0.5, 0.0));
    member->setScale(ratio);
    this->addChild(member, 1, TAG_MEMBER);
    
//    //チャット
//    Sprite *chat = Sprite::create("init/Team/top/bt_chat.png");
//    chat->setAnchorPoint(Point(0.5, 0)) ;
//    chat->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/6 * 5, FOOTER_SIZE + 200), 0.5, 0.0));
//    chat->setScale(ratio);
//    this->addChild(chat, 1, TAG_CHAT);
    
    //ヘルプボタン
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [this](Ref* sender) {
                                                    auto normal_scene = (NormalScene *)this->getParent();

                                                    string url = HOST;
                                                    url += "user/index.php?&p=page__team_help&user_id=" + SaveData::loadString("user_master_id");
                                                    WebViewLoadLayer *web_view_load_layer = WebViewLoadLayer::create(url, "チームヘルプ");
                                                    normal_scene->addChild(web_view_load_layer, 9);
                                         });
    
    help_btn->setScale(ratio);
    help_btn->setAnchorPoint(Point(0.5, 1));
    help_btn->setTag(T_HELP_BUTTON) ;
    auto menu = Menu::create(help_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(570, 838), 0.5, 1));
    this->addChild(menu, 5);
    
    /*
    //加入申請があれば表示
    HmsList demand_user_list;
    demand_user_list.import_tsv(UserDefaultUtil::getTeamDemandUserList());
    if(demand_user_list.size()>0) {
        unsigned char* filebuffer = 0;
        
        SSImageList* image_list1;
        SSPlayer* anime1;
        
        SSPlayerHelper::createFromFile( &filebuffer , &anime1 , &image_list1 , "request.ssba" ,"download/SpriteStudio/Team/");
        anime1->setPosition(Point(100,100));
        //anime1->setScale(2.0f);
        managenent->addChild(anime1, 1);
    }
     */
    
    if(_is_boss_event_open && _boss_event_data != nullptr) {
        //イベントバナー
        BannerSprite *banner = BannerSprite::create(_boss_event_data["image_path"].string_value());
        banner->setPosition(convertFromDisplaySize(Point(320, FOOTER_SIZE + 15), 0.5, 0));
        banner->setAnchorPoint(Point(0.5, 0)) ;
        banner->setScale(ratio);
        this->addChild(banner, 4, TAG_BANNER);
        
        //ボスの数を表示
        if(_boss_count>0) {
            Layer *layer = Layer::create();
            //画像
            Sprite *img = Sprite::create("init/Home/Event/BossEvent/boss_appearance2.png");
            layer->addChild(img);
            CallFunc *func1 = CallFunc::create([img]()
                                               {
                                                   img->setTexture("init/Home/Event/BossEvent/boss_appearance1.png");
                                               });//ラムダ式
            DelayTime *delay = DelayTime::create(0.5f);
            CallFunc *func2 = CallFunc::create([img]()
                                               {
                                                   img->setTexture("init/Home/Event/BossEvent/boss_appearance2.png");
                                               });//ラムダ式

            RepeatForever *action_img =  RepeatForever::create(Sequence::create(func1, delay, func2, nullptr));
            img->runAction(action_img);
            
            /*
             Layer *layer = Layer::create();
             //画像
             auto img = Sprite::create("init/Home/Event/BossEvent/boss_appearance1.png");
             
             //textureを0.5秒おきに変更する
             RepeatForever* repeat_texture_change = RepeatForever::create(Sequence::create(
             ChangeTexture::create("init/Home/Event/BossEvent/boss_appearance1.png"),
             DelayTime::create(0.5),
             ChangeTexture::create("init/Home/Event/BossEvent/boss_appearance1.png"),
             DelayTime::create(0.5),
             nullptr));
             
             img->runAction(repeat_texture_change);
             layer->addChild(img);
             */
            
            //テキスト
            if(_boss_count>=100) _boss_count = 99;
            auto label = HMSFontUtil::createWithTTFBySystemFontArgument("ボス" + to_string(_boss_count) + "体出現中", FONT_NAME_SYSTEM, 26);
            label->setHorizontalAlignment(TextHAlignment::CENTER);
            label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
            label->setPosition(Point(0, -50));
            layer->addChild(label);
        
            //レイヤー
            layer->setAnchorPoint(Point(0, 0));
            layer->setPosition(Point(525, 120));
            banner->addChild(layer);
        
            ScaleTo *scale1 = ScaleTo::create(0.5f, 1.1);
            ScaleTo *scale2 = ScaleTo::create(0.5f, 1.0);
            RepeatForever *action =  RepeatForever::create(Sequence::create(scale1, scale2, nullptr));
            layer->runAction(action);
        }
    }
}

bool TeamTopLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamTopLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamTopLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    NormalScene *normal_scene = (NormalScene*)this->getParent();
    
    if(this->getChildByTag(TAG_MEMBER)==nullptr) return;
    
    //メンバー
    Sprite *member = (Sprite*)this->getChildByTag(TAG_MEMBER);
    Rect member_rect = member->boundingBox();
    if(member_rect.containsPoint(touch->getLocation())) {
        SaveData::saveString("team_member_list_mode", "member");
        normal_scene->changePage("TeamMember");
        return;
    }
//    //チャット
//    Sprite *chat = (Sprite*)this->getChildByTag(TAG_CHAT);
//    Rect chat_rect = chat->boundingBox();
//    if(chat_rect.containsPoint(touch->getLocation())) {
//        normal_scene->changePage("OldTeamChatBase");
//        return;
//    }
    //管理
    Sprite *managenent = (Sprite*)this->getChildByTag(TAG_MANAGEMENT);
    Rect managenent_rect = managenent->boundingBox();
    if(managenent_rect.containsPoint(touch->getLocation())) {
        normal_scene->changePage("TeamManagement");
        return;
    }
    //イベントバナー
    Sprite *event_banner = (Sprite*)this->getChildByTag(TAG_BANNER);
    if(event_banner!=NULL){
        Rect banner_rect = event_banner->boundingBox();
        if(banner_rect.containsPoint(touch->getLocation())) {
            HMSLOG("banner");
            normal_scene->changePage("BossEventTop");
        }
    }

}

void TeamTopLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

