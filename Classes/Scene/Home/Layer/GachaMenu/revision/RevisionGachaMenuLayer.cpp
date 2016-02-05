#include "RevisionGachaMenuLayer.h"
#include "CharaCollectionGetRewardPopup.h"

bool RevisionGachaMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("RevisionGachaMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void RevisionGachaMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //ガチャメニューの表示情報をしゅとく
    gachaMenuInfoRequest();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("ガチャメニュー", "Mypage", this);
    
}

void RevisionGachaMenuLayer::gachaMenuInfoRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
    
    string url = HOST;
    url += "user/gacha_menu_info.php";
    auto selector = httpresponse_selector(RevisionGachaMenuLayer::gachaMenuInfoCallback);
    string body = "user_master_id=" + user_master_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void RevisionGachaMenuLayer::gachaMenuInfoCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    //リストの生成
    createList(json["gacha_menu_data_list"].object_items());
}

void RevisionGachaMenuLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(RevisionGachaMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(RevisionGachaMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(RevisionGachaMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(RevisionGachaMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void RevisionGachaMenuLayer::createList(Json json_gacha_menu_data_list)
{
    //string path_array[] = {"bt_formation.png", "bt_sell.png", "bt_charalist.png", "bt_up.png", "bt_up.png"};
    HmsVector<Sprite *> button_list;
    
    int json_count = countJsonData(json_gacha_menu_data_list) ;
    for(int i=0; i<json_count; i++){
        string iStr = toString<int>(i) ;
        button_list.insert(createButton(json_gacha_menu_data_list[iStr].object_items())) ;
    }
    
    auto func = [this](int tag){
        changePage(tag);
    };
    
    //スクロールレイヤーの作成
    int height = 180; //<-バナーのPositionのy方向の距離
    auto scroll_layer = HmsScrollLayer::create(height * ratio, button_list, func);
    this->addChild(scroll_layer, 1, SCROLLLAYER_TAG);
}

Sprite *RevisionGachaMenuLayer::createButton(Json json_banner_data){
    Sprite *button = Sprite::create(json_banner_data["image_path"].string_value());
    string type = json_banner_data["banner_type"].string_value();
    string id = json_banner_data["id"].string_value();
    
    if(type == "free") button->setTag(T_GACHA_FREE) ;
    if(type == "premium") button->setTag(T_GACHA_PREMIUM) ;
    if(type == "special") button->setTag(T_GACHA_PREMIUM_PLUS) ;
    if(type == "box") button->setTag(T_BOX_GACHA + fromString<int>(id)) ;
    
    //時間表示スプライトの生成
    Sprite *end_date_back = createEndDateBack(button) ;
    button->addChild(end_date_back, 1, T_END_DATE_BACK) ;
    
    //時間管理クラスの生成
    TimeCounter *timeCounter = TimeCounter::createWithDateTimeFormat(json_banner_data["end_date"].string_value(), 1,
         [this, end_date_back](TimeCounter::CountState count_state, time_t now_time, time_t end_time){
            CCLOG("now_time: %d", (int)now_time) ;
            CCLOG("end_time: %d", (int)end_time) ;
             
         if(count_state == TimeCounter::CountState::NORMAL_COUNT || count_state == TimeCounter::CountState::COUNT_END){
            //時間表示スプライトの更新
             updateEndDateBack(end_date_back, end_time - now_time) ;
         }
    });
    
    if(timeCounter){
        button->addChild(timeCounter) ;
    }
    
    return button ;
}

void RevisionGachaMenuLayer::changePage(int tag)
{
    auto normal_scene = (NormalScene *)this->getParent();
    
    if (tag == T_GACHA_FREE) {
        //無料ガチャのタブを開く
        SaveData::saveInt("gacha_tab", 1);
        normal_scene->changePage("GachaMenu");
    }
    else if (tag == T_GACHA_PREMIUM) {
        //最初にプレミアムガチャを表示させるタグの設定
        SaveData::saveInt("gacha_tab", 2);
        normal_scene->changePage("GachaMenu");
    }
    else if (tag == T_GACHA_PREMIUM_PLUS) {
        //最初にスペシャルガチャを表示させるタグの設定
        SaveData::saveInt("gacha_tab", 3);
        normal_scene->changePage("GachaMenu");
    }
    else if (tag > T_BOX_GACHA) {
        int box_gacha_id = tag - T_BOX_GACHA;
        SaveData::saveString("box_gacha_id", toString(box_gacha_id));
        normal_scene->changePage("BoxGacha");
    }
    else if (tag == T_GACHA_UNKNOWN1) {
        //TODO 遷移コード
    }
}

bool RevisionGachaMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void RevisionGachaMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void RevisionGachaMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void RevisionGachaMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

Sprite* RevisionGachaMenuLayer::createEndDateBack(Sprite *button)
{
    //時間表示の背景
    Sprite *end_date_back = Sprite::create("init/Home/Common/bk_time.png") ;
    end_date_back->setVisible(false) ;
    end_date_back->setAnchorPoint(Point(0, 1)) ;
    end_date_back->setPosition(Point(0, button->getContentSize().height));
    end_date_back->setCascadeOpacityEnabled(true) ;
    
    //あと
    Label *prefix_time_text = HMSFontUtil::createWithTTFBySystemFontArgument("あと", FONT_NAME_SYSTEM, 24);
    prefix_time_text->setHorizontalAlignment(TextHAlignment::RIGHT);
    prefix_time_text->setAnchorPoint(Point(1, 0)) ;
    prefix_time_text->setPosition(Point(end_date_back->getContentSize().width/2-26 ,10));
    end_date_back->addChild(prefix_time_text, 1, T_PREFIX_TIME_TEXT) ;
    
    //時間で更新するラベル
    Label *end_date_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 35);
    end_date_label->setHorizontalAlignment(TextHAlignment::CENTER);
    end_date_label->setAnchorPoint(Point(0.5, 0)) ;
    end_date_label->setPosition(Point(end_date_back->getContentSize().width/2, 10));
    end_date_label->setTextColor(Color4B::RED) ;
    end_date_back->addChild(end_date_label, 1, T_END_DATE_LABEL) ;
    
    //"時間"や"分"を入れる
    Label *suffix_time_text = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 24);
    suffix_time_text->setHorizontalAlignment(TextHAlignment::LEFT);
    suffix_time_text->setAnchorPoint(Point(0, 0)) ;
    suffix_time_text->setPosition(Point(end_date_back->getContentSize().width/2+30 ,10));
    suffix_time_text->setTextColor(Color4B::RED) ;
    end_date_back->addChild(suffix_time_text, 1, T_SUFFIX_TIME_TEXT) ;
    
    return end_date_back ;
}

void RevisionGachaMenuLayer::updateEndDateBack(Sprite *end_date_back, time_t remain_time_second){
    
    Label *suffix_time_text = (Label*)end_date_back->getChildByTag(T_SUFFIX_TIME_TEXT);
    Label *end_date_label = (Label*)end_date_back->getChildByTag(T_END_DATE_LABEL);
    Label *prefix_time_text = (Label*)end_date_back->getChildByTag(T_PREFIX_TIME_TEXT);
    
    if (0 < remain_time_second)
    {
        end_date_back->setVisible(true) ;
        int minute = (int)(remain_time_second / 60.0f); //分
        int hour = (int)(minute / 60.0f); //時間
        int day = (int)(hour / 24.0f); //日
        
        if(day >= 1){
            end_date_label->setString(toString<int>(day)) ;
            suffix_time_text->setString("日") ;
        } else if(hour >= 1){
            end_date_label->setString(toString<int>(hour)) ;
            suffix_time_text->setString("時間") ;
        }else{
            end_date_label->setString(toString<int>(ceil(remain_time_second / 60.0f))) ;
            suffix_time_text->setString("分") ;
        }
    } else{
        suffix_time_text->setString("") ;
        
        end_date_label->setSystemFontSize(25) ;
        end_date_label->setString("終了しました") ;
        
        prefix_time_text->setString("") ;
        
        this->setColor(Color3B::GRAY) ;
    }
}
