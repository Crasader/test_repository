#include "MapSupportLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "Utility.h"
#include "PopupOkButtonLayer.h"
#include "MapScene.h"

#define MAP_SUPPORT_TAG_ID 100

MapSupportLayer* MapSupportLayer::create(string message, bool shadow_flg, HmsVector<MapCharaLayer *> chara_list, string boss_event_stage_log_id, int now_support_user_master_id)
{
    MapSupportLayer *node = new MapSupportLayer();
    if (node && node->init(message, shadow_flg, chara_list, boss_event_stage_log_id, now_support_user_master_id))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool MapSupportLayer::init(string message, bool shadow_flg, HmsVector<MapCharaLayer *> chara_list, string boss_event_stage_log_id, int now_support_user_master_id)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("MapSupportLayer::init()");
    
    ratio = getVisibleRatio();
    
    //表示テキストの取得
    _message = message;
    
    //コールバック
    battle_list_id = 0;
    _shadow_flg = shadow_flg;
    
    //リストの保存
    _chara_list = chara_list;
    
    //ログIDの保存
    _boss_event_stage_log_id = boss_event_stage_log_id;
    
    //現在応援しているuidの保存
    _now_support_user_master_id = now_support_user_master_id;
    
    //背景画像の作成
    createBg();
    
    return true;
}

void MapSupportLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //ボタンの作成
    createButton();
    
    //テキストの表示
    showMessage();
    
    //応援リストの表示
    createList();
    
    //ポップアップアクション
    Utility::openPopupAction(this);
}

void MapSupportLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(MapSupportLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MapSupportLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MapSupportLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(MapSupportLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void MapSupportLayer::createBg()
{
    //影
    if (_shadow_flg) {
        auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
        shadow->setScale(ratio);
        shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
        this->addChild(shadow, 1);
    }
    
    //背景
    string path = "init/Home/Popup/popup_800.png";;
    _bg = Sprite::create(path);
    _bg->setScale(0);
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
}

void MapSupportLayer::createButton()
{
    //OKボタンの作成
    auto ok_off = Sprite::create("init/Map/bt_close.png");
    auto ok_on  = Sprite::create("init/Map/bt_close.png");
    ok_on->setColor(Color3B(180, 180, 180));
    
    auto ok_btn = MenuItemSprite::create(ok_off, ok_on, [this](Ref* sender)
                                          {
                                              //SE
                                              CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/button_yes.mp3");
                                              
                                              //ポップアップ閉じる
                                              Utility::closePopupAction(this);
                                          });
    ok_btn->setAnchorPoint(Point(0.5, 0.5));
    
    auto menu = Menu::create(ok_btn, nullptr);
    menu->setPosition(Point(_bg->getContentSize().width / 2, -38));
    _bg->addChild(menu, 1);
    
    /*
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("閉じる", FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_message, 28, _bg->getContentSize().width - 20));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, 20));
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _bg->addChild(label, 2);
     */
}

void MapSupportLayer::showMessage()
{
    //テキスト背景
    Sprite *img = Sprite::create("init/Map/bk_title.png");
    img->setPosition(Point(_bg->getContentSize().width / 2, 747));
    _bg->addChild(img, 2);
    
    //表示テキスト
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(_message, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_message, 28, _bg->getContentSize().width - 20));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_bg->getContentSize().width / 2, 747));
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    _bg->addChild(label, 2);
}

void MapSupportLayer::createList()
{
    for(int i=0; i<min(5, _chara_list.size()); i++) {
        MapCharaLayer *chara = _chara_list.at(i);
        string user_name = chara->getUserName();
        int chara_id = chara->getCharaId();
        int user_master_id = chara->getUserMasterId();
        HMSLOG("user_name=%s, chara_id=%d, user_master_id=%d", user_name.c_str(), chara_id, user_master_id);
        
        int y = 644- 135*i;
        
        //背景
        Sprite *img;
        img = Sprite::create("init/Map/list_cheer.png");
        img->setPosition(Point(_bg->getContentSize().width / 2, y));
        _bg->addChild(img, 2);
        
        string path = "download/Chara/thumb100/" + toString(chara_id) + ".png";
        auto card = Sprite::create(path);
        card->setPosition(Point(84, y));
        card->setAnchorPoint(Point(0.5, 0.5));
        _bg->addChild(card, 2);
        
        //テキスト
        auto label = HMSFontUtil::createWithTTFBySystemFontArgument(user_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(user_name, 28, _bg->getContentSize().width - 20));
        label->setHorizontalAlignment(TextHAlignment::CENTER);
        label->setPosition(Point(144, y));
        label->setAnchorPoint(Point(0, 0.5));
        label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        _bg->addChild(label, 2);

        /*
        label = HMSFontUtil::createWithTTFBySystemFontArgument(user_name, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_message, 28, _bg->getContentSize().width - 20));
        label->setHorizontalAlignment(TextHAlignment::CENTER);
        label->setPosition(Point(184, y-15));
        label->enableShadow(Color4B(255, 221, 0, 255), Size(2, -2), 2);
        _bg->addChild(label, 2);
         */
        
        //OKボタンの作成
        auto ok_off = Sprite::create("init/Map/bt_cheer.png");
        auto ok_on  = Sprite::create("init/Map/bt_cheer.png");
        ok_on->setColor(Color3B(180, 180, 180));
        
        auto ok_btn = MenuItemSprite::create(ok_off, ok_on, [this, user_master_id](Ref* sender)
                                             {
                                                 _target_user_master_id = user_master_id;
                                                 sendCheerRequest();
                                             });
        ok_btn->setAnchorPoint(Point(0.5, 0.5));
        
        auto menu = Menu::create(ok_btn, nullptr);
        menu->setPosition(Point(440, y));
        _bg->addChild(menu, 2);
    }
}

Sprite *MapSupportLayer::getBackground()
{
    return _bg;
}

void MapSupportLayer::sendCheerRequest()
{
    string url = HOST;
    url += "map/cheer.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + _boss_event_stage_log_id;
    url += "&target_user_master_id=" + to_string(_target_user_master_id);
    
    HMSLOG("url=%s", url.c_str());
    
    //auto selector = httpresponse_selector(MapSupportLayer::sendCheerRequestCallback);
    HttpConnectHms::sendHttpRequest(url, [this](HttpClient *sender, HttpResponse *response){
        if(response->isSucceed()){
            sendCheerRequestCallback(sender, response);
        }else{//送信失敗したら時間おいて再送信
            this->runAction(Sequence::create(DelayTime::create(1.0f),CallFunc::create([this](){
                sendCheerRequest();
            }), NULL));
        }
    });
}

void MapSupportLayer::sendCheerRequestCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnectHms::getHttpResponse(this, sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;

    HMSLOG("result=%s", result.c_str());
    
    MapScene *map_scene = (MapScene *)this->getParent();
    if(result!="") {
        map_scene->setSupportUserMasterId(_target_user_master_id, result);
    }
    
    //ポップアップ閉じる
    Utility::closePopupAction(this);
}

bool MapSupportLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch MapSupportLayer!");
    
    return true;
}
void MapSupportLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void MapSupportLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void MapSupportLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
