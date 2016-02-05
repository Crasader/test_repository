#include "BattleTopLayer.h"
#include "PlistAnimationSprite.h"

bool BattleTopLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BattleTopLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void BattleTopLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("オンライン対戦", "QuestMenu", this);
    
    //Dungeonシーンで使用するデータを初期化
    Utility::initDungeonData();
    
    //クイック対戦の対戦相手存在チェック
    ArenaChallengerCheckRequest();
    
    //ボタンの作成
    createButton();
    
    //デッキ選択画面で使用するデータの初期化
    initBattleUserSpriteData();
}

void BattleTopLayer::ArenaChallengerCheckRequest()
{
	//クイック対戦対戦相手存在チェックのリクエスト
	string url = HOST;
	url += "battle/arena_state_check.php";
	string body = "user_master_id="+ SaveData::loadString("user_master_id");
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        ArenaChallengerCheckCallback(sender, response);
    }, body);
}

void BattleTopLayer::ArenaChallengerCheckCallback(HttpClient *sender, HttpResponse *response)
{
	//データ取得コールバック
	string result = HttpConnectHms::getHttpResponse(this, sender, response);
	
	//通信エラー以外
	if (result != "not connect" && result != "response NG"){
		
		string err;
		Json json = Json::parse(result, err);

		HMSLOG("result : %s",json["challenger_exists"].string_value().c_str());
		_challenger_exists_result = json["challenger_exists"].string_value();
		HMSLOG("_challenger_exists_result : %s",_challenger_exists_result.c_str());
		
		HMSLOG("rank_point_up_rate : %s",json["rank_point_up_rate"].string_value().c_str());
		_rank_point_up_rate = json["rank_point_up_rate"].string_value();
		HMSLOG("_rank_point_up_rate : %s",_rank_point_up_rate.c_str());

        //オンライン対戦に対戦相手が存在する場合、画像を設定する。
        if(_challenger_exists_result=="challenger_coming"){
            auto challenger_coming = Sprite::create("init/Home/Quest/challenger.png");
            challenger_coming->setScale(0);
            challenger_coming->setPosition(convertFromDisplaySize(Point(100, 685), 0.5, 0.5));
            
            auto *challenger_coming_scale_up1  = ScaleTo::create(0.2f, 1.0f*ratio);
            challenger_coming->runAction(
                                         Sequence::create(
                                                          EaseBounceOut::create(challenger_coming_scale_up1),
                                                          nullptr)
                                         );
            
            auto *challenger_coming_scale_up   = ScaleTo::create(0.8f, 1.0f*ratio);
            auto *challenger_coming_scale_down = ScaleTo::create(0.8f, 0.8f*ratio);
            RepeatForever *challenger_coming_forever_action = RepeatForever::create(
                                                                                    Sequence::create(
                                                                                                     challenger_coming_scale_up,
                                                                                                     challenger_coming_scale_down,
                                                                                                     nullptr)
                                                                                    ) ;
            challenger_coming->runAction(challenger_coming_forever_action);
            addChild(challenger_coming, 5);
        }
        _rank_point_up_rate="2";
        //オンライン対戦のポイント倍率があがっている場合のみバナーに文言を表示。
        if(_rank_point_up_rate != "0"){
            auto up_rate_free = Sprite::create("init/Home/Quest/event_term.png");
            up_rate_free->setPosition(convertFromDisplaySize(Point(580, 540), 0.5, 0.5));
            up_rate_free->setScale(0);
            addChild(up_rate_free, 3, ACTION_SPRITE_TAG);
            
            //出現時のアクション
            auto *up_rate_scale_up1  = ScaleTo::create(0.05f, 1.0f);
            up_rate_free->runAction(Sequence::create(EaseIn::create(up_rate_scale_up1,100),nullptr));
            
            //ラベル1
            auto label_free1 = HMSFontUtil::createWithTTFBySystemFontArgument("称号Pt", FONT_NAME_SYSTEM, 26);
            label_free1->setHorizontalAlignment(TextHAlignment::CENTER);
            label_free1->setPosition(Point(up_rate_free->getContentSize().width / 2 - 5, up_rate_free->getContentSize().height / 2 + 20));
            label_free1->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
            
            //ラベル2
            auto label_free2 = HMSFontUtil::createWithTTFBySystemFontArgument(_rank_point_up_rate + "倍！", FONT_NAME_SYSTEM, 26);
            label_free2->setHorizontalAlignment(TextHAlignment::CENTER);
            label_free2->setPosition(Point(up_rate_free->getContentSize().width / 2 , up_rate_free->getContentSize().height / 2 - 20));
            label_free2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
            
            up_rate_free->addChild(label_free1, 3, ACTION_SPRITE_TAG);
            up_rate_free->addChild(label_free2, 3, ACTION_SPRITE_TAG);
            
            //表示されている間のアクション
            auto *up_rate_free_scale_up  = ScaleTo::create(0.8f, 1.0f*ratio);
            auto *up_rate_free_scale_down = ScaleTo::create(0.8f, 0.8f*ratio);
            RepeatForever *up_rate_free_forever_action = RepeatForever::create(
                                                                               Sequence::create(up_rate_free_scale_up,
                                                                                                up_rate_free_scale_down,
                                                                                                nullptr)
                                                                               ) ;
            up_rate_free->runAction(up_rate_free_forever_action);
        }
        
	}
}

void BattleTopLayer::createButton()
{
    //クイック対戦
    auto free_btn_off      = Sprite::create("init/Home/Quest/bt_battle_quick.png");
    auto free_btn_on       = Sprite::create("init/Home/Quest/bt_battle_quick.png");
    free_btn_on->setColor(Color3B(180, 180, 180));
    
    auto free_btn = MenuItemSprite::create(free_btn_off, free_btn_on , CC_CALLBACK_1(BattleTopLayer::changePage, this));
    free_btn->setScale(ratio);
    free_btn->setTag(QUICK_BUTTON_TAG);

	
    
    //リベンジモードの場合
    if(SaveData::loadBool("revenge_mode")==true) {
        auto revenge_sprite = PlistAnimationSprite::setData((Layer *)free_btn, "download/Effect/Sprite/ef_revenge2", 0, 0.04f, 10000);
        revenge_sprite->setPosition(Point(free_btn->getContentSize().width/2, free_btn->getContentSize().height/2 + 75));
        //revenge_sprite->setScale(ratio);
        BlendFunc blend;//加算合成
        blend.src = GL_SRC_ALPHA;
        blend.dst = GL_ONE;
        revenge_sprite->setBlendFunc(blend);
        
        auto revenge_sprite2 = Sprite::create("download/Effect/Sprite/revenge.png");
        //revenge_sprite2->setScale(ratio);
        revenge_sprite2->setPosition(Point(free_btn->getContentSize().width/2, free_btn->getContentSize().height/2 + 75));
        free_btn->addChild(revenge_sprite2, 10000-1);
    }

	

    //ルーム対戦
    auto room_btn_off = Sprite::create("init/Home/Quest/bt_battle_room.png");
    auto room_btn_on = Sprite::create("init/Home/Quest/bt_battle_room.png");
    room_btn_on->setColor(Color3B(180, 180, 180));
    
    auto room_btn = MenuItemSprite::create(room_btn_off, room_btn_on, CC_CALLBACK_1(BattleTopLayer::changePage, this));
    room_btn->setScale(ratio);
    room_btn->setTag(ROOM_BUTTON_TAG);
	
    auto menu = Menu::create(free_btn, room_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    menu->alignItemsVerticallyWithPadding(30 * ratio);
    this->addChild(menu, 2);
    
    //リプレイボタン
    auto replay_btn_off = Sprite::create("init/Home/Quest/bt_replay.png");
    auto replay_btn_on  = Sprite::create("init/Home/Quest/bt_replay.png");
    replay_btn_on->setColor(Color3B(180, 180, 180));
    
    auto replay_btn = MenuItemSprite::create(replay_btn_off,replay_btn_on, CC_CALLBACK_1(BattleTopLayer::changePage, this));
    replay_btn->setScale(ratio);
    replay_btn->setTag(REPLAY_BUTTON_TAG);
    replay_btn->setAnchorPoint(Point(0.5, 1));
    auto Menu3 = Menu::create(replay_btn, nullptr);
    Menu3->setPosition(convertFromDisplaySize(Point(552, 835), 0.5, 1));
    this->addChild(Menu3, 5);
}

void BattleTopLayer::changePage(Ref* sender)
{
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();

    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == QUICK_BUTTON_TAG)
    {
        normal_scene->changePage("BattleFree");
    }
    else if (tag == ROOM_BUTTON_TAG)
    {
        normal_scene->changePage("BattleRoomTop");
    }
    else if(tag == REPLAY_BUTTON_TAG)
    {
        normal_scene->changePage("ReplayList");
    }
}

void BattleTopLayer::initBattleUserSpriteData()
{
    string user_master_id = "" ;
    string level = "0" ;
    string friend_id = "";
    string card_image_id = "";
    string user_name = "???" ;
    string last_play_date = "";
    string comment = "";
    string rank_name =  "???";
    string battle_point_rate = "1" ;
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"user_master_id\":\"" + user_master_id + "\"," + cr;
    data += "  \"friend_id\":\"" + friend_id + "\"," + cr;
    data += "  \"card_image_id\":\"" + card_image_id+ "\"," + cr;
    data += "  \"user_name\":\"" + user_name + "\"," + cr;
    data += "  \"last_play_date\":\"" + last_play_date + "\"," + cr;
    data += "  \"comment\":\"" + comment + "\"," + cr;
    data += "  \"rank_name\":\"" + rank_name + "\"," + cr;
    data += "  \"level\":\"" + level + "\"," + cr;
    data += "  \"battle_point_rate\":\"" + battle_point_rate + "\"" + cr;
    data += "}";
    HMSLOG("%s", data.c_str()) ;
    
    SaveData::saveString("battle_user_sprite_data", data);
}