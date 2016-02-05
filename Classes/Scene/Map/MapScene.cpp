#include "MapScene.h"
#include "DownloadScene.h"
#include "cocos-ext.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsCrypto.h"
#include "SaveData.h"
#include "MapStampLayer.h"
#include "PlistAnimationSprite.h"
#include "MapSupportLayer.h"

Scene* MapScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MapScene::create();
    scene->addChild(layer);
    return scene;
}

bool MapScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    //メモリのキャッシュクリア
    Director::getInstance()->purgeCachedData();
    
    //スワイプ処理クラスの登録
    // Init gesture recognizer
    _gestureRecognizer = new TSimpleGestureRecognizer();
    _gestureRecognizer->init();
    _gestureRecognizer->setGestureHandler(this);
    
    // Enable all gesture kinds
    //_gestureRecognizer->setTapEnabled(true);
    //_gestureRecognizer->setDoubleTapEnabled(true);
    //_gestureRecognizer->setLongPressEnabled(true);
    //_gestureRecognizer->setPanEnabled(true);
    _gestureRecognizer->setPinchEnabled(true);
    //_gestureRecognizer->setRotationEnabled(true);
    //_gestureRecognizer->setSwipeEnabled(true);
    
    // Taps will be fired immediately without waiting for double tap
    _gestureRecognizer->setTapRequiresDoubleTapRecognitionToFail(false);
    
    //両端を黒くする
    cutLeftAndRightVisibleSpace();
    
    HMSLOG("MapScene::init()");
    
    return true;
}

//******************************初期化関連******************************

void MapScene::cutLeftAndRightVisibleSpace()
{
    int space_width = getVisibleSpaceWidth();
    
    //左右の余白がある場合、余白部分を黒くする
    if(space_width > 0){
        //スプライトの大きさを設定
        Rect rect = Rect(0, 0, space_width, _contentSize.height);
        
        //左の黒いスプライト
        Sprite *left_sprite = Sprite::create();
        left_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        left_sprite->setAnchorPoint(Point(0,0));
        left_sprite->setTextureRect(rect);
        left_sprite->setColor(Color3B::BLACK);
        left_sprite->setPosition(0,0);
        this->addChild(left_sprite);
        
        //右の黒いスプライト
        Sprite *right_sprite = Sprite::create();
        right_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        right_sprite->setAnchorPoint(Point(1,0));
        right_sprite->setTextureRect(rect);
        right_sprite->setColor(Color3B::BLACK);
        right_sprite->setPosition(_contentSize.width,0);
        this->addChild(right_sprite);
    }
}

void MapScene::onEnter()
{
    Layer::onEnter();
    
    getMapData();
}

void MapScene::getMapData()
{
    _boss_event_stage_log_id = SaveData::loadString("boss_event_stage_log_id");
    HMSLOG("boss_event_stage_id=%s", _boss_event_stage_log_id.c_str());
    
    string url = HOST;
    url += "map/get_map_data.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + _boss_event_stage_log_id;
    url += "&deck_no=" + SaveData::loadString("party_no");//デッキナンバー追加
    
    HMSLOG("url=%s", url.c_str());
    
    auto selector = httpresponse_selector(MapScene::getMapDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void MapScene::getMapDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("result=%s", result.c_str());
    
    string err;
    _json_map = Json::parse(result, err);
    
    initialize();
}

void MapScene::initialize()
{
    ratio = getVisibleRatio();
    
    _user_master_id = fromString<int>(SaveData::loadString("user_master_id"));
    
    //マップデータ
    Json json = _json_map["map_base_data"].object_items();
    _map_width = json["map_width"].int_value();
    _map_height = json["map_height"].int_value();
    _map_filename = json["map_filename"].string_value();
    
    _map_layer = MapLayer::create();
    this->addChild(_map_layer, MAP_LAYER_Z);
    _map_layer->setScale(0.6f);
    _map_layer->setAnchorPoint(Point(0, 0));
    
    //システム関連データ
    _delay_time = _json_map["system_data"]["delay_time"].int_value();
    _movable_distance = _json_map["system_data"]["movable_distance"].int_value();
    _move_speed = _json_map["system_data"]["move_speed"].int_value();
    _contact_distance = _json_map["system_data"]["contact_distance"].int_value();
    _camp_distance = _json_map["system_data"]["camp_distance"].int_value();

    _map_layer->initialize(_contact_distance, _camp_distance);
    
    //軍営地データ
    int camp_count = _json_map["camp_count"].int_value();
    for(int i=0; i<camp_count; i++) {
        int x = _json_map["camp_data"][i]["x"].int_value();
        int y = _json_map["camp_data"][i]["y"].int_value();
        _map_layer->addCamp(Point(x, y), false);
    }
    
    //お宝データ
    int treasure_count = _json_map["treasure_count"].int_value();
    for(int i=0; i<treasure_count; i++) {
        string id = _json_map["treasure_data"][i]["id"].string_value();
        int x = _json_map["treasure_data"][i]["x"].int_value();
        int y = _json_map["treasure_data"][i]["y"].int_value();
        string type = _json_map["treasure_data"][i]["type"].string_value();
        bool open_flg = _json_map["treasure_data"][i]["open_flg"].bool_value();
        _map_layer->addTreasure(Point(x, y), id, type, open_flg);
    }
    
    //各種レイヤーの作成
    _map_navi_layer = MapNaviLayer::create();
    this->addChild(_map_navi_layer, MAP_NAVI_LAYER_Z);
    _map_navi_layer->initialize();
    
    //各種ボタンの作成
    _stamp_img = Sprite::create("download/Dungeon/bt_emotion_on.png");
    _stamp_img->setScale(ratio);
    _stamp_img->setAnchorPoint(Point(0.5, 0.5));
    _stamp_img->setPosition(convertFromDisplaySize(Point(50, 675-32), 0.5, 1));
    this->addChild(_stamp_img, MAP_STAMP_Z);
    
    _escape_img = Sprite::create("init/Map/battle_bk_menu.png");
    _escape_img->setScale(ratio*0.8);
    _escape_img->setAnchorPoint(Point(0.5, 0.5));
    _escape_img->setPosition(convertFromDisplaySize(Point(594, 817-32), 0.5, 1));
    this->addChild(_escape_img, MAP_ESCAPE_Z);
    
    _support_img = Sprite::create("init/Map/bt_support.png");
    _support_img->setScale(ratio);
    _support_img->setAnchorPoint(Point(0.5, 0.5));
    _support_img->setPosition(convertFromDisplaySize(Point(275, 817-32), 0.5, 1));
    this->addChild(_support_img, MAP_SUPPORT_Z);

    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(MapScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MapScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MapScene::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //各種レイヤーの作成
    _chat_base_upper_layer = ChatBaseUpperLayer::create("MapScene");
    this->addChild(_chat_base_upper_layer, MAP_CHAT_LAYER_Z);
    
    //BGM再生
    
    this->runAction(Sequence::create(DelayTime::create(0.5f),
                                     PlayBGM::create("download/BGM/10_battle2.mp3"),
                                     FadeToVolumeOfBGM::create(0.f, SaveData::loadFloat("bgm_volume")),
                                     CallFunc::create([](){HMSLOG("volume:%f", SimpleAudioEngine::getInstance()->getBackgroundMusicVolume());}),nullptr)
                    );    
    
    /*
    //BGMと効果音(エフェクト)の音量設定
    float bgm_volume = 0.5f;
    float effect_volume = 0.5f;
    
    bgm_volume    = SaveData::loadFloat("bgm_volume");
    effect_volume = SaveData::loadFloat("effect_volume");
    bgm_volume = 0.10f;
    HMSLOG("bgm_volume=%f, effect_volume=%f", bgm_volume, effect_volume);
    
    //BGMと効果音の音量設定
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
    
    //BGM再生
    SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/10_battle2.mp3",true);
    */
    getCharaData();
}

void MapScene::getCharaData()
{
    if(_connection_flg==true) return;
    _connection_flg = true;
    
    string url = HOST;
    url += "map/get_chara_data.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + _boss_event_stage_log_id;
    
    HMSLOG("url=%s", url.c_str());
    
    auto selector = httpresponse_selector(MapScene::getCharaDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void MapScene::getCharaDataCallback(HttpClient *sender, HttpResponse *response)
{
    _connection_flg = false;
    
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    //HMSLOG("result=%s", result.c_str());
    
    string err;
    _json_chara = Json::parse(result, err);
    
    //チームデータ
    _enemy_team_name = _json_chara["enemy_data"]["team_name"].string_value();
    _player_team_name = _json_chara["player_data"]["team_name"].string_value();
    
    //敵キャラデータ
    int enemy_count = _json_chara["enemy_data"]["enemy_count"].int_value();
    for(int i=1; i<=enemy_count; i++) {
        int x = _json_chara["enemy_data"]["enemy" + toString(i)]["x"].int_value();
        int y = _json_chara["enemy_data"]["enemy" + toString(i)]["y"].int_value();
        int user_master_id = _json_chara["enemy_data"]["enemy" + toString(i)]["user_master_id"].int_value();
        string user_name = _json_chara["enemy_data"]["enemy" + toString(i)]["user_name"].string_value();
        int level = _json_chara["enemy_data"]["enemy" + toString(i)]["level"].int_value();
        int chara_id = _json_chara["enemy_data"]["enemy" + toString(i)]["chara_id"].int_value();
        string deck_id_list = _json_chara["enemy_data"]["enemy" + toString(i)]["deck_chara_id_list"].string_value();
        string comment = _json_chara["enemy_data"]["enemy" + toString(i)]["comment"].string_value();
        int hp = _json_chara["enemy_data"]["enemy" + toString(i)]["hp"].int_value();
        int max_hp = _json_chara["enemy_data"]["enemy" + toString(i)]["max_hp"].int_value();
        bool camp_flg = _json_chara["enemy_data"]["enemy" + toString(i)]["camp_flg"].int_value();
        
        HMSLOG("add chara (enemy)");
        _map_layer->addChara(Point(x, y), MAP_CHARA_TYPE_ENEMY, user_master_id, user_name, level, chara_id, deck_id_list, comment, hp, max_hp, camp_flg, false);
    }
    
    //味方キャラデータ
    int player_count = _json_chara["player_data"]["player_count"].int_value();
    for(int i=1; i<=player_count; i++) {
        int x = _json_chara["player_data"]["player" + toString(i)]["x"].int_value();
        int y = _json_chara["player_data"]["player" + toString(i)]["y"].int_value();
        int user_master_id = _json_chara["player_data"]["player" + toString(i)]["user_master_id"].int_value();
        string user_name = _json_chara["player_data"]["player" + toString(i)]["user_name"].string_value();
        int level = _json_chara["player_data"]["player" + toString(i)]["level"].int_value();
        int chara_id = _json_chara["player_data"]["player" + toString(i)]["chara_id"].int_value();
        string deck_id_list = _json_chara["player_data"]["player" + toString(i)]["deck_chara_id_list"].string_value();
        string comment = _json_chara["player_data"]["player" + toString(i)]["comment"].string_value();
        int hp = _json_chara["player_data"]["player" + toString(i)]["hp"].int_value();
        int max_hp = _json_chara["player_data"]["player" + toString(i)]["max_hp"].int_value();
        bool camp_in_flg = _json_chara["player_data"]["player" + toString(i)]["camp_in_flg"].bool_value();
        
        HMSLOG("add chara (player)");
        _map_layer->addChara(Point(x, y), MAP_CHARA_TYPE_PLAYER, user_master_id, user_name, level, chara_id, deck_id_list, comment, hp, max_hp, false, false);
        
        //戦闘中の場合はアニメーションを表示する
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        int enemy_user_master_id = _json_chara["player_data"]["player" + toString(i)]["enemy_user_master_id"].int_value();
        if(enemy_user_master_id>0) {
            MapCharaLayer *chara2 = _map_layer->getChara(MAP_CHARA_TYPE_ENEMY, enemy_user_master_id);
            if(chara2!=nullptr) {
                _map_layer->addBattleAnimation(chara, chara2);
            }
        }
        
        //キャンプイン判定
        if(camp_in_flg==true) {
            chara->campIn();
        }
    }
    
    //コマンドデータ
    int id = _json_chara["command_data"]["command_history_id"].int_value();
    if(id>0) {
        _command_history_id = id;
    }
    
    int turn_no = _json_chara["turn_no"].int_value();
    if(turn_no>=0) {
        string command = "{\"command\": \"changeturn_" + toString(turn_no) + "_-_-_\"}";
        //{"command": "changeturn_1"}
        _receive_command_list.insert(command);
    }
    
    //自分のキャラデータがない場合は、追加リクエストを送る
    bool found_flg = false;
    HmsVector<MapCharaLayer *> chara_list = _map_layer->getCharaList(MAP_CHARA_TYPE_PLAYER);
    for(int i=0; i<chara_list.size(); i++) {
        MapCharaLayer *chara = chara_list.at(i);
        if(chara->getUserMasterId()==_user_master_id) found_flg = true;
    }
    if(found_flg==false) {
        _send_command_list.insert("insertuser_" + toString(_user_master_id));
        _initialize_flg = true;
    } else {
        //checkBattle(0.0f);
        this->scheduleOnce(schedule_selector(MapScene::checkBattle), 3.0f);
    }
    
    //スケジュール登録
    _rest_time_update_time = getNowTime();
    this->schedule(schedule_selector(MapScene::updateData), 1.0f);
    this->schedule(schedule_selector(MapScene::executeCommand), 0.3f);
    
    //_initialize_flg = true;
}

//******************************その他******************************
bool MapScene::isPlayerTurn()
{
    if(_turn_no==MAP_CHARA_TYPE_PLAYER) return true;
    else return false;
}

void MapScene::setSupportUserMasterId(int support_user_master_id, string support_user_name)
{
    _support_user_master_id = support_user_master_id;
    _support_user_name = support_user_name;
    
    setSupportAnimation();
}

void MapScene::resetSupportUserMasterId()
{
    _support_user_master_id = 0;
    _support_user_name = "";
    
    setSupportAnimation();
}

void MapScene::setSupportAnimation()
{
    if(_support_user_master_id==0) {
        _support_img->stopAllActions();
        _support_img->setRotation(0.0f);
        this->removeChild(_support_label);
        this->removeChild(_support_bk_img);
    } else {
        RotateTo *rotate1 = RotateTo::create(0.25f, -15.0f);
        RotateTo *rotate2 = RotateTo::create(0.25f, 15.0f);
        RepeatForever *action = RepeatForever::create(Sequence::create(rotate1, rotate2, nullptr));
        _support_img->runAction(action);
        
        _support_bk_img = Sprite::create("init/Map/balloon_support.png");
        _support_bk_img->setScale(ratio);
        _support_bk_img->setAnchorPoint(Point(0.5, 0.5));
        _support_bk_img->setPosition(convertFromDisplaySize(Point(416, 817-32), 0.5, 1));
        this->addChild(_support_bk_img, MAP_SUPPORT_Z);
        
        //_support_label = HMSFontUtil::createWithTTFBySystemFontArgument(_support_user_name + "を応援中", FONT_NAME_SYSTEM, 20*ratio);
        _support_label = Label::createWithSystemFont(_support_user_name + "を応援中", FONT_NAME_SYSTEM, 20*ratio);
        _support_label->setColor(Color3B(0, 0, 0));
        _support_label->setPosition(convertFromDisplaySize(Point(422, 817-32), 0.5, 1));
        this->addChild(_support_label, MAP_SUPPORT_Z);
    }
}

//******************************タップ処理******************************
bool MapScene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //HMSLOG("onTouchBegan");

    _gestureRecognizer->onTouchBegan(touch, event);
    
    return true;
}
void MapScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
    _gestureRecognizer->onTouchMoved(touch, event);
    
    //前回からの距離が著しく大きい場合は、何もしない
    Point tap_point = touch->getLocation();
    Point previous_tap_point = touch->getPreviousLocation();
    float distance = calculateDistance(tap_point, previous_tap_point);
    //HMSLOG("last=%f,%f, touch=%f,%f, distance=%f", previous_tap_point.x, previous_tap_point.y, tap_point.x, tap_point.y, distance);
    if(distance<_limit_distance) {
        //前回からの差分計算
        int diff_x = tap_point.x - previous_tap_point.x;
        int diff_y = tap_point.y - previous_tap_point.y;
        
        //マップレイヤーを動かす
        Point now_map_point = _map_layer->getPosition();
        Point new_map_point = Point(now_map_point.x + diff_x, now_map_point.y + diff_y);
        _map_layer->setPosition(new_map_point);
        checkBoundary();
    }
}
void MapScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    //HMSLOG("onTouchEnded");
    
    _gestureRecognizer->onTouchEnded(touch, event);
    
    if(_stamp_img->boundingBox().containsPoint(touch->getLocation())) {
        HMSLOG("map stamp layer create");
        
        auto map_stamp_layer = MapStampLayer::create();
        this->addChild(map_stamp_layer, 9999);
        
        return;
    }
    
    if(_support_img->boundingBox().containsPoint(touch->getLocation())) {
        HMSLOG("support confirm");
        
        HmsVector<MapCharaLayer *> chara_list;
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, _user_master_id);
        chara_list = _map_layer->getCharaListNearBy(chara, _movable_distance);

        if(chara_list.size()==0) {
            auto func = [](){
            };
            
            auto popup_layer = PopupOkButtonLayer::create(func, "応援できる仲間がいません。") ;
            this->addChild(popup_layer, MAP_POPUP_Z);
        } else {
            MapSupportLayer *support_layer = MapSupportLayer::create("応援する仲間を選択してください", true, chara_list, _boss_event_stage_log_id, _support_user_master_id);
            this->addChild(support_layer, MAP_POPUP_Z);
        }
        
        return;
    }
    
    if(_escape_img!=nullptr && _escape_img->boundingBox().containsPoint(touch->getLocation())) {
        HMSLOG("escape confirm");
        
        //ポップアップ表示
        auto ok_func = [this](){
            HMSLOG("escape");
            insertSendCommandList("deleteuser_" + SaveData::loadString("user_master_id"));
            sendCommandData();
            
            _battle_scene_flg = true; 
            this->stopAllActions() ;
            Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
            
            //ページ遷移の処理
            auto scene = NormalScene::createScene("TeamTop");
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        };
        
        auto ng_func = [](){}; //いいえを押したとき何もしない
        popupConfirmLayer *layer = popupConfirmLayer::create("戦場から退却します。\nよろしいですか？");
        layer->setFunction(ok_func, ng_func);
        this->addChild(layer, 100);
        
        return;
    }
    
    //自分のキャラの移動を行う
    if(isPlayerTurn()) {
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, _user_master_id);
        if(chara!=nullptr) {
            Point target_position = _map_layer->getPoint(touch->getLocation());
            float distance = calculateDistanceAtHexMap(_move_start_point, target_position);

            HMSLOG("distance=%f", distance);
            if(distance<_movable_distance*ratio) {
                //HMSLOG("chara move %f,%f -> %f,%f", chara_position.x, chara_position.y, target_position.x, target_position.y);
                chara->moveStart(target_position, _move_speed, false);
                return;
            }
        }
    }
        
    //前回からの距離が著しく大きい場合は、何もしない
    Point tap_point = touch->getLocation();
    Point previous_tap_point = touch->getPreviousLocation();
    float distance = calculateDistance(tap_point, previous_tap_point);
    //HMSLOG("last=%f,%f, touch=%f,%f, distance=%f", previous_tap_point.x, previous_tap_point.y, tap_point.x, tap_point.y, distance);
    if(distance<_limit_distance) {
        //前回からの差分計算
        int diff_x = tap_point.x - previous_tap_point.x;
        int diff_y = tap_point.y - previous_tap_point.y;
        
        //マップレイヤーを動かす
        Point now_map_point = _map_layer->getPosition();
        Point new_map_point = Point(now_map_point.x + diff_x, now_map_point.y + diff_y);
        _map_layer->setPosition(new_map_point);
        checkBoundary();
        
        return;
    }
}

bool MapScene::onGesturePinch(TGesturePinch* gesture)
{
    float pinch_map_scale = gesture->getScale();
    pinch_map_scale = 1.0f + (pinch_map_scale - 1.0f)/30.0f;
    
    //拡大縮小を行う
    float map_scale = _map_layer->getScale();
    float map_scale_old = map_scale;
    map_scale = pinch_map_scale * map_scale;
    HMSLOG("pinch_scale=%f, map_scale=%f", pinch_map_scale, map_scale);
    //if(map_scale<min_map_scale) map_scale = min_map_scale;
    //if(map_scale>max_map_scale) map_scale = max_map_scale;
    _map_layer->setScale(map_scale);
    
    //拡大縮小に伴い、位置も修正する
    //拡大時はマイナス、縮小時はプラス
    int diff_x = ((map_scale_old-map_scale)*_map_width)/2;
    int diff_y = ((map_scale_old-map_scale)*_map_height)/2;
    Point now_map_point = _map_layer->getPosition();
    Point new_map_point = Point(now_map_point.x + diff_x, now_map_point.y + diff_y);
    _map_layer->setPosition(new_map_point);
    //HMSLOG("diff=%d,%d", diff_x, diff_y);
    
    checkBoundary();
    
    return false;
}

bool MapScene::onGestureSwipe(TGestureSwipe* gesture)
{
    SwipeDirection dir = gesture->getDirection();
    float swipe_length = gesture->getLength();
    Point p1 = gesture->getLocation();
    Point p2 = gesture->getEndLocation();
    HMSLOG("dir=%d, swipe_length=%f", dir, swipe_length);
    HMSLOG("point %f,%f -> %f,%f", p1.x, p1.y, p2.x, p2.y);
    
    if(_layer_animation_flg==true) return false;
    _layer_animation_flg = true;
    
    float map_scale = _map_layer->getScale();
    int diff_x = (p2.x - p1.x) * ratio * map_scale / 2.0f;
    int diff_y = (p2.y - p1.y) * ratio * map_scale / 2.0f;

    Point now_map_point = _map_layer->getPosition();
    Point new_map_point = Point(now_map_point.x + diff_x, now_map_point.y + diff_y);
    MoveTo *move = MoveTo::create(0.15f, new_map_point);
    CallFunc *expire = CallFunc::create([this]()
                                        {
                                            _layer_animation_flg = false;
                                            this->checkBoundary();
                                        });//ラムダ式
    EaseOut *ease_action = EaseOut::create(move, 3);
    Action *action = Sequence::create(ease_action, expire, nullptr);
    _map_layer->runAction(action);
    
    HMSLOG("diff_x=%d,diff_y=%d", diff_x, diff_y);
    
    return false;
}

bool MapScene::checkBoundary()
{
    bool animation_flg = false;
    
    //マップレイヤーの移動限界をチェック
    float map_scale = _map_layer->getScale();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    int max_x = 0;
    int min_x = - (_map_width * ratio * map_scale - visibleSize.width);
    int max_y = 0;
    int min_y = - (_map_height * ratio * map_scale - visibleSize.height);
    int buffer_length = 3 * ratio;
    
    //HMSLOG("min_x=%d, min_y=%d, map_scale=%f", min_x, min_y, map_scale);
    Point map_point = _map_layer->getPosition();
    Point map_point_old = map_point;
    if(map_point.x > max_x + buffer_length) {
        //HMSLOG("max_x");
        map_point = Point(max_x, map_point.y);
    } else if(map_point.x < min_x - buffer_length) {
        //HMSLOG("min_x");
        map_point = Point(min_x, map_point.y);
    }
    if(map_point.y > max_y + buffer_length) {
        //HMSLOG("max_y");
        map_point = Point(map_point.x, max_y);
    } else if(map_point.y < min_y - buffer_length) {
        //HMSLOG("min_y");
        map_point = Point(map_point.x, min_y);
    }
    //HMSLOG("map_point=%f,%f->%f,%f", map_point_old.x, map_point_old.y, map_point.x, map_point.y);
    
    //マップレイヤーの拡大縮小比率限界をチェック
    float buffer_ratio = 1.2f;
    if(map_scale<_min_map_scale/buffer_ratio) map_scale = _min_map_scale;
    if(map_scale>_max_map_scale*buffer_ratio) map_scale = _max_map_scale;
    
    //いずれかの限界を超えている場合は、戻すように補正
    bool position_change_flg = false;
    bool scale_change_flg = false;
    if(map_point!=_map_layer->getPosition()) position_change_flg = true;
    if(map_scale!=_map_layer->getScale()) scale_change_flg = true;
    if((position_change_flg==true||scale_change_flg==true) && _layer_animation_flg==false) {
        if(position_change_flg) {
            HMSLOG("###map_point=%f,%f->%f,%f", _map_layer->getPosition().x, _map_layer->getPosition().y, map_point.x, map_point.y);
        }
        if(scale_change_flg==true) {
            HMSLOG("###map_scale=%f->%f", _map_layer->getScale(), map_scale);
            int diff_x = ((_map_layer->getScale()-map_scale)*_map_width)/2;
            int diff_y = ((_map_layer->getScale()-map_scale)*_map_height)/2;
            map_point = Point(map_point.x + diff_x, map_point.y + diff_y);
        }
        
        _layer_animation_flg = true;
        MoveTo *move = MoveTo::create(0.3f, map_point);
        ScaleTo *scale = ScaleTo::create(0.3f, map_scale);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _layer_animation_flg = false;
                                            });//ラムダ式
        Action *action;
        if(scale_change_flg==false) {
            action = Sequence::create(move, expire, nullptr);
        } else {
            action = Sequence::create(Spawn::createWithTwoActions(move, scale), expire, nullptr);
        }
        _map_layer->runAction(action);
        animation_flg = true;
    }

    
    return animation_flg;
}

//******************************スケジューラー******************************
void MapScene::timeup()
{
    if(_battle_scene_flg==true) return;
    
    //タイプアップの判定
    if(_time_up==1 && _time_up_flg==false) {
        HMSLOG("timeup");
        _time_up_flg = true;
        
        auto func = [this](){
            _battle_scene_flg = true;
            
            this->stopAllActions() ;
            Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
            
            //ページ遷移の処理
            auto scene = NormalScene::createScene("TeamTop");
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        };
        
        auto popup_layer = PopupOkButtonLayer::create(func, "敵に逃走されました") ;
        this->addChild(popup_layer, MAP_POPUP_Z);
    }
}

void MapScene::finish()
{
    if(_connection_flg==true) return;
    _connection_flg = true;
        
    string url = HOST;
    url += "map/finish.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + _boss_event_stage_log_id;
    
    HMSLOG("url=%s", url.c_str());
    
    auto selector = httpresponse_selector(MapScene::finishCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void MapScene::finishCallback(HttpClient *sender, HttpResponse *response)
{
    _finish_effect_flg = true;
    _connection_flg = false;
    
    string result = HttpConnect::getHttpResponse(sender, response);

    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    //討伐の演出
    showEffect(this, "download/Map/defeat.plist", 2.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
    
    //画面表示（討伐成功のアニメーション表示＋退却ボタン消す＋ホームに戻るボタン設置）
    DelayTime *delay = DelayTime::create(3.0f);
    CallFunc *expire1 = CallFunc::create([this]()
                                         {
                                             //退却ボタン消す
                                             this->removeChild(_escape_img);
                                             _escape_img = nullptr;
                                             
                                             //ホームに戻るボタン
                                             MenuItemImage* home_btn = MenuItemImage::create("init/Map/bt_home.png",
                                                                                             "init/Map/bt_home.png",
                                                                                             [&, this](Ref* pSender){
                                                                                                 _battle_scene_flg = true;
                                                                                                 
                                                                                                 this->stopAllActions() ;
                                                                                                 Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
                                                                                                 
                                                                                                 //ページ遷移の処理
                                                                                                 auto scene = NormalScene::createScene("TeamTop");
                                                                                                 auto transition = TransitionFade::create(3.0f, scene);
                                                                                                 Director::getInstance()->replaceScene(transition);
                                                                                                 
                                                                                             });
                                             home_btn->setPosition(convertFromDisplaySize(Point(320, 55), 0.5, 0));
                                             Menu* home_menu = Menu::create(home_btn, nullptr);
                                             home_menu->setPosition(Point::ZERO);
                                             this->addChild(home_menu, MAP_ESCAPE_Z);
                                             
                                             //討伐成功のアニメーション
                                             auto success_sprite = PlistAnimationSprite::setData(this, "download/Effect/Sprite/success", 0, 0.05f, MAP_POPUP_Z);
                                             success_sprite->setPosition(convertFromDisplaySize(Point(320, 480-32), 0.5, 1));
                                             success_sprite->setScale(ratio);
                                         });//ラムダ式
    this->runAction(Sequence::create(delay, expire1, NULL));
}

void MapScene::showEffect(Layer* layer, string filename, float scale, Point pos)
{
    //エフェクトの表示
    auto cache = SpriteFrameCache::getInstance();
    cache->removeSpriteFrames();
    cache->addSpriteFramesWithFile(filename);
    
    // 配列に各フレームを格納
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    char str[100] = {0};
    
    // 各フレームを、plistに登録されているファイル名を元に生成
    for (int i = 1; i <= 99; i++) {
        sprintf(str, "%02d.png", i);
        auto frame = cache->getSpriteFrameByName(str);
        if (!frame)
        {
            break;
        }
        animFrames.pushBack(frame);
    }
    
    // 上記フレーム配列からアニメーションを生成
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.08f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    Sprite *_resultLabelSprite = Sprite::createWithSpriteFrame(animFrames.front());
    _resultLabelSprite->setScale(scale * ratio);
    _resultLabelSprite->setPosition(pos);
    layer->addChild(_resultLabelSprite, 1000);
    
    auto func = CallFunc::create([this, _resultLabelSprite]()
                                 {
                                     //this->removeChild(_resultLabelSprite);
                                     //showBattleBossEventResult();
                                 });
    
    _resultLabelSprite->runAction(Sequence::create(animate, DelayTime::create(1.2f), func, nullptr));
}

void MapScene::insertSendCommandList(string command)
{
    _send_command_list.insert(command);
}

void MapScene::sendCommandData()
{
    if(_battle_scene_flg==true) return;
    
    if(_send_command_list.size()==0) {
        getCommandData();
        return;
    }
    if(_connection_flg==true) return;
    _connection_flg = true;
    
    string command = _send_command_list.at(0);
    
    string url = HOST;
    url += "map/send_command_data.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + _boss_event_stage_log_id;
    url += "&command=" + command;
    
    HMSLOG("url=%s", url.c_str());
    
    auto selector = httpresponse_selector(MapScene::sendCommandDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
    
    _send_command_list.remove(0);
}

void MapScene::sendCommandDataCallback(HttpClient *sender, HttpResponse *response)
{
    _connection_flg = false;
    
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;

    getCommandData();
}

void MapScene::getCommandData()
{
    if(_battle_scene_flg==true) return;
    
    if(_connection_flg==true) return;
    _connection_flg = true;

    string url = HOST;
    url += "map/get_command_data.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + _boss_event_stage_log_id;
    url += "&command_history_id=" + toString(_command_history_id);
    
    HMSLOG("url=%s", url.c_str());
    
    auto selector = httpresponse_selector(MapScene::getCommandDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void MapScene::getCommandDataCallback(HttpClient *sender, HttpResponse *response)
{
    _connection_flg = false;
    
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("result=%s", result.c_str());
    
    string err;
    _json_command = Json::parse(result, err);

    //コマンドデータ
    int command_count = _json_command["command_data"]["command_count"].int_value();
    int id = _json_command["command_data"]["command_history_id"].int_value();
    if(id>0) {
        _command_history_id = id;
    }
    for(int i=0; i<command_count; i++) {
        //string command = _json_command["command_data_list"][toString(i)]["command"].string_value();
        string command = _json_command["command_data_list"][toString(i)].dump();
        HMSLOG("command=%s", command.c_str());
        if(command!="") {
            _receive_command_list.insert(command);
        }
    }
    
    //戦況データ
    HmsVector<string> data_list = split(_json_command["situation_data"].string_value(), "_");
    string player_count = data_list.at(0);
    string enemy_count = data_list.at(1);
    _rest_time = fromString<int>(data_list.at(2));
    _rest_time_update_time = getNowTime();
    _time_up = fromString<int>(data_list.at(3));
    int turn_time = fromString<int>(data_list.at(4));
    int turn_time_max = fromString<int>(data_list.at(5));
    
    if(_time_up==1) {
        //checkTimeup();
        timeup();
        return;
    }
    
    _map_navi_layer->displaySituationInfo(_player_team_name, player_count, _enemy_team_name, enemy_count, _turn_no, turn_time, turn_time_max);
    
    //スコアデータ
    _personal_point = _json_command["point_data"]["personal_point"].int_value();
    _team_point = _json_command["point_data"]["team_point"].int_value();
    _map_navi_layer->setEventPoint(_personal_point, _team_point);
}

void MapScene::executeCommand(float dt)
{
    if(_receive_command_list.size()==0) return;
    if(_receive_command_execute_flg==true) return;
    if(_battle_scene_flg==true) return;
    
    string command_text = _receive_command_list.at(0);
    string err;
    Json json = Json::parse(command_text, err);
    string command = json["command"].string_value();
    HMSLOG("execute_command(). command_text=%s", command_text.c_str());
    
    if(command.find("changeturn", 0)!=string::npos) {
        //*********************
        //ターン切り替え
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        _turn_no = fromString<int>(data_list.at(1));
        
        //データ更新
        //turn_noの更新と移動可能範囲の表示
        string message;
        if(isPlayerTurn()) {
            message = _player_team_name + "のターンです";
            //自分のキャラの移動可能範囲を表示する
            MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, _user_master_id);
            if(chara!=nullptr) {
                _move_start_point = chara->getPosition();
                _map_layer->addMovableArea(chara->getPosition(), _user_master_id, _movable_distance);
            }
        } else {
            message = _enemy_team_name + "のターンです";
            _map_layer->removeMovableArea(_user_master_id);
        }
        
        //演出
        _map_navi_layer->displayTurnChangeMessage(message, isPlayerTurn());

        //フラグ管理
        DelayTime *delay = DelayTime::create(3.0f);
        CallFunc *expire = CallFunc::create([this]()
                                             {
                                                 _receive_command_execute_flg = false;
                                             });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("insertuser_", 0)!=string::npos) {
        //*********************
        //ユーザ追加
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int chara_type = fromString<int>(data_list.at(2));
        HmsVector<string> position_str_list = split(data_list.at(3), ",");
        int x = fromString<int>(position_str_list.at(0));
        int y = fromString<int>(position_str_list.at(1));
        string user_name = data_list.at(4);
        int chara_id = fromString<int>(data_list.at(5));
        int level = fromString<int>(data_list.at(6));
        int hp = fromString<int>(data_list.at(7));
        int max_hp = fromString<int>(data_list.at(8));
        string deck_id_list = "";
        string comment = "";
        
        //データ更新
        //演出
        _map_navi_layer->addBattleMessage(user_name + "が参戦しました。");
        _map_layer->addChara(Point(x, y), chara_type, user_master_id, user_name, level, chara_id, deck_id_list, comment, hp, max_hp, false, true);
        
        //フラグ管理
        DelayTime *delay = DelayTime::create(1.0f);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _receive_command_execute_flg = false;
                                            });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("attacked_", 0)!=string::npos) {
        //*********************
        //敵追加
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int enemy_user_master_id = fromString<int>(data_list.at(2));
        int chara_type = fromString<int>(data_list.at(3));
        HmsVector<string> position_str_list = split(data_list.at(4), ",");
        int x = fromString<int>(position_str_list.at(0));
        int y = fromString<int>(position_str_list.at(1));
        string enemy_name = data_list.at(5);
        int chara_id = fromString<int>(data_list.at(6));
        int level = fromString<int>(data_list.at(7));
        int hp = fromString<int>(data_list.at(8));
        int max_hp = fromString<int>(data_list.at(9));
        string deck_id_list = "";
        string comment = "";
        
        //データ更新
        //演出
        string user_name = "";
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            user_name = chara->getUserName();
        }
        _map_navi_layer->addBattleMessage(user_name + "が" + enemy_name + "に遭遇しました。");
        _map_layer->addChara(Point(x, y), chara_type, enemy_user_master_id, enemy_name, level, chara_id, deck_id_list, comment, hp, max_hp, false, true);
        
        //フラグ管理
        DelayTime *delay = DelayTime::create(1.0f);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _receive_command_execute_flg = false;
                                            });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("moveuser_", 0)!=string::npos) {
        //*********************
        //ユーザ移動
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int chara_type = fromString<int>(data_list.at(2));
        HmsVector<string> position_str_list = split(data_list.at(3), ",");
        int x = fromString<float>(position_str_list.at(0));
        int y = fromString<float>(position_str_list.at(1));
        Point target_position = Point(x*ratio, y*ratio);
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(chara_type, user_master_id);
        if(user_master_id!=_user_master_id && chara!=nullptr) {
            chara->moveStart(target_position, _move_speed, true);
        }
        
        //フラグ管理
        DelayTime *delay = DelayTime::create(0.5f);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _receive_command_execute_flg = false;
                                            });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));

        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("deleteuser_", 0)!=string::npos) {
        //*********************
        //ユーザ削除
        //*********************
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        string user_name = data_list.at(2);
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            _map_layer->removeChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
            _map_navi_layer->addBattleMessage(user_name + "が退却しました。");
        }
        if(user_master_id==_support_user_master_id) resetSupportUserMasterId();
        
        //コマンドリストから削除
        
        _receive_command_list.remove(0);
    } else if(command.find("campin_", 0)!=string::npos) {
        //*********************
        //ユーザキャンプイン
        //*********************
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            chara->campIn();
        }
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("battle_", 0)!=string::npos) {
        //*********************
        //バトル突入
        //*********************
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int enemy_user_master_id = fromString<int>(data_list.at(2));
        int boss_event_battle_log_id = fromString<int>(data_list.at(3));
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        string user_name = "";
        if(chara!=nullptr) user_name = chara->getUserName();
        MapCharaLayer *chara2 = _map_layer->getChara(MAP_CHARA_TYPE_ENEMY, enemy_user_master_id);
        string enemy_name = "";
        if(chara2!=nullptr) enemy_name = chara2->getUserName();
        if(user_name!="" && enemy_name!="") {
            _map_navi_layer->addBattleMessage(user_name + "が" + enemy_name + "との戦闘を開始しました。");
            _map_layer->addBattleAnimation(chara, chara2);
        }
        if(user_master_id==_user_master_id && boss_event_battle_log_id>0) {
            goToBattle(boss_event_battle_log_id);
        }
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("escape_", 0)!=string::npos||command.find("lose_", 0)!=string::npos) {
        //*********************
        //敵に負けるor逃走される
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int enemy_user_master_id = fromString<int>(data_list.at(2));
        HmsVector<string> position_str_list = split(data_list.at(3), ",");
        int x = fromString<float>(position_str_list.at(0));
        int y = fromString<float>(position_str_list.at(1));
        Point target_position = Point(x*ratio, y*ratio);

        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        string user_name = "";
        if(chara!=nullptr) {
            user_name = chara->getUserName();
            chara->displayCharaMessage("");
        }
        MapCharaLayer *chara2 = _map_layer->getChara(MAP_CHARA_TYPE_ENEMY, enemy_user_master_id);
        string enemy_name = "";
        if(chara2!=nullptr) {
            enemy_name = chara2->getUserName();
            _map_layer->removeBattleAnimation(chara, enemy_user_master_id);
        }
        if(user_name!="" && enemy_name!="") {
            if(command.find("escape_", 0)!=string::npos) {
                _map_navi_layer->addBattleMessage(user_name + "は" + enemy_name + "を倒しきれませんでした");
            } else {
                _map_navi_layer->addBattleMessage(user_name + "が" + enemy_name + "に敗れました");
            }
        }
        
        chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            chara->moveStart(target_position, _move_speed, true);
        }
        if(user_master_id==_support_user_master_id) resetSupportUserMasterId();
        
        //フラグ管理
        DelayTime *delay = DelayTime::create(0.5f);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _receive_command_execute_flg = false;
                                            });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("win_", 0)!=string::npos) {
        //*********************
        //敵に勝利
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int enemy_user_master_id = fromString<int>(data_list.at(2));
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        string user_name = "";
        if(chara!=nullptr) {
            user_name = chara->getUserName();
            chara->displayCharaMessage("");
        }
        MapCharaLayer *chara2 = _map_layer->getChara(MAP_CHARA_TYPE_ENEMY, enemy_user_master_id);
        string enemy_name = "";
        if(chara2!=nullptr) {
            enemy_name = chara2->getUserName();
            _map_layer->removeBattleAnimation(chara, enemy_user_master_id);
            _map_layer->stopAllActions();
            _map_layer->removeChara(MAP_CHARA_TYPE_ENEMY, enemy_user_master_id);
        }
        if(user_name!="" && enemy_name!="") {
            _map_navi_layer->addBattleMessage(user_name + "が" + enemy_name + "を撃破しました！");
        }
        if(user_master_id==_support_user_master_id) resetSupportUserMasterId();
        
        //フラグ管理
        DelayTime *delay = DelayTime::create(0.5f);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _receive_command_execute_flg = false;
                                            });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("camp_", 0)!=string::npos) {
        //*********************
        //敵に勝利
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        HmsVector<string> position_str_list = split(data_list.at(2), ",");
        int x = fromString<float>(position_str_list.at(0));
        int y = fromString<float>(position_str_list.at(1));
        Point target_position = Point(x, y);
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            string user_name = chara->getUserName();
            _map_navi_layer->addBattleMessage(user_name + "が軍営地を獲得しました！");
            _map_layer->addCamp(target_position, true);
        }
        
        //フラグ管理
        DelayTime *delay = DelayTime::create(0.5f);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _receive_command_execute_flg = false;
                                            });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
        
    } else if(command.find("hp_", 0)!=string::npos) {
        //*********************
        //HP変更
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int hp = fromString<int>(data_list.at(2));
        int heal_flg = fromString<int>(data_list.at(3));
        int heal_value = fromString<int>(data_list.at(4));
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            chara->setHp(hp);
            Point chara_position = chara->getPosition();
            float dt = 0.0f;
            if(heal_flg==1) {
                //_map_layer->showEffect(Point(chara_position.x*ratio, chara_position.y*ratio), "download/Effect/Particle/map/heal.plist", true);
                chara->heal();
                dt = 0.5f;
            }
            DelayTime *delay = DelayTime::create(dt);
            CallFunc *func = CallFunc::create([chara, heal_value]()
                                              {
                                                  if(chara!=nullptr) {
                                                      chara->displayHpGauge(heal_value);
                                                  }
                                              });//ラムダ式
            this->runAction(Sequence::create(delay, func, NULL));
        }
        
        //フラグ管理
        _receive_command_execute_flg = false;
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("damage_", 0)!=string::npos) {
        //*********************
        //ダメージ表示
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int enemy_user_master_id = fromString<int>(data_list.at(2));
        string damage_str = data_list.at(3);
        
        //データ更新
        //演出
        float dt = 0;
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_ENEMY, enemy_user_master_id);
        if(chara!=nullptr) {
            HmsVector<string> damage_list = split(damage_str, ":");
            for(int i=0; i<damage_list.size(); i++) {
                int damage = fromString<int>(damage_list.at(i));
                HMSLOG("damage[%d] at user[%d]", damage, enemy_user_master_id);
                dt = 0.3f*i;
                DelayTime *delay = DelayTime::create(dt);
                CallFunc *func = CallFunc::create([chara, damage]()
                                                     {
                                                         if(chara!=nullptr) {
                                                             chara->displayDamage(damage);
                                                         }
                                                     });//ラムダ式
                this->runAction(Sequence::create(delay, func, NULL));
                //chara->displayDamage(damage);
            }
        }

        //フラグ管理
        DelayTime *delay = DelayTime::create(dt+0.5f);
        CallFunc *expire = CallFunc::create([this]()
                                            {
                                                _receive_command_execute_flg = false;
                                            });//ラムダ式
        this->runAction(Sequence::create(delay, expire, NULL));
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("treasure_", 0)!=string::npos) {
        //*********************
        //スタンプ表示
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        string treasure_id = data_list.at(2);
        string item_name = data_list.at(3);
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            string user_name = chara->getUserName();
            _map_navi_layer->addBattleMessage(user_name + "が宝箱を開けました。");
            _map_navi_layer->addBattleMessage(item_name + "を獲得！");
        }
        _map_layer->openTreasure(treasure_id);
        
        //フラグ管理
        _receive_command_execute_flg = false;
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("stamp_", 0)!=string::npos) {
        //*********************
        //スタンプ表示
        //*********************
        _receive_command_execute_flg = true;
        
        HmsVector<string> data_list = split(command, "_");
        int user_master_id = fromString<int>(data_list.at(1));
        int stamp_id = fromString<int>(data_list.at(2));
        
        //データ更新
        //演出
        MapCharaLayer *chara = _map_layer->getChara(MAP_CHARA_TYPE_PLAYER, user_master_id);
        if(chara!=nullptr) {
            HMSLOG("stamp[%d] from user[%d]", stamp_id, user_master_id);
            chara->displayCharaStamp(stamp_id);
        }
        
        //フラグ管理
        _receive_command_execute_flg = false;
        
        //コマンドリストから削除
        _receive_command_list.remove(0);
    } else if(command.find("finish_", 0)!=string::npos) {
        if(command=="finish_" + to_string(_user_master_id)) {
            _finish_flg = true;
        }
        //コマンドリストから削除
        _receive_command_list.remove(0);
    }
}

void MapScene::checkBattle(float dt)
{
    //DungeonSceneへの遷移を開始していたらチェックを止める
    if(_battle_scene_flg==true) return;
    
    HMSLOG("checkBattle");
    
    string url = HOST;
    url += "map/check_battle.php?";
    url += "user_master_id=" + SaveData::loadString("user_master_id");
    url += "&boss_event_stage_log_id=" + _boss_event_stage_log_id;
    
    HMSLOG("url=%s", url.c_str());
    
    auto selector = httpresponse_selector(MapScene::checkBattleCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void MapScene::checkBattleCallback(HttpClient *sender, HttpResponse *response)
{
    _initialize_flg = true;
    
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    HMSLOG("result=%s", result.c_str());

    if(result!="") {
        int boss_event_battle_log_id = fromString<int>(result);
        
        if(boss_event_battle_log_id>0) {
            goToBattle(boss_event_battle_log_id);
        }
    }
}

void MapScene::goToBattle(int boss_event_battle_log_id)
{
    _battle_scene_flg = true;
    
    //タップ禁止レイヤー生成
    auto normal_scene = (NormalScene *)getParent();
    normal_scene->addTapDisableLayer();
    
    //黒いスプライトを画面いっぱいにかぶせる
    auto spr = Sprite::create();
    spr->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
    spr->setTextureRect(Rect(0, 0, _contentSize.width, _contentSize.height));
    spr->setColor(Color3B::BLACK);
    spr->setGlobalZOrder(1); //zOrderに関係無く強制的に前面に表示するメソッド よっぽどのことが無ければ基本的に使用しない方がよい
    spr->setOpacity(0) ;
    addChild(spr,99999);
    
    FadeIn *fade = FadeIn::create(2.0f) ;
    
    auto go_dungeon_func = CallFunc::create([this, boss_event_battle_log_id](){
        this->stopAllActions() ;
        
        //SaveData::saveString("party_no", toString<int>(_now_deck_no));
        SaveData::saveString("boss_event_battle_log_id", toString(boss_event_battle_log_id));
        
        Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
        
        //ダンジョンシーンへの遷移
        auto scene = DungeonScene::createScene();
        auto transition = TransitionFade::create(2.0f, scene);
        Director::getInstance()->replaceScene(transition);
    });
    auto pause_music_func= CallFunc::create([this](){
        CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic() ;
    });
    
    //SE(シャキーンの音)
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/shutujin.mp3",false);
    //音を1.5秒にかけて小さくし、その後BGMを停止し、最後にボリュームを通常時と同じ値に直す
    this->runAction(Sequence::create(FadeToVolumeOfBGM::create(1.5f, 0.0f), pause_music_func, FadeToVolumeOfBGM::create(0.0f, SaveData::loadFloat("bgm_volume")),nullptr));
    //画面を暗くしてから遷移する
    spr->runAction(Sequence::createWithTwoActions(fade, go_dungeon_func));
}

void MapScene::updateData(float dt)
{
    if(_initialize_flg==false) return;
    if(_battle_scene_flg==true) return;
    
    if(_finish_flg==true && _finish_effect_flg==false) {
        HMSLOG("finish");
        finish();
        return;
    }
    
    _map_navi_layer->displayRestTime(_rest_time);
    time_t now_time = getNowTime();
    int diff = (int)(now_time - _rest_time_update_time);
    HMSLOG("diff=%d, _rest_time_update_time=%ld", diff, _rest_time_update_time);
    if(diff>_delay_time && _delay_flg==false) {
        HMSLOG("reset");
        _delay_flg = true;
        
        auto func = [this](){
            //入り直し
            this->stopAllActions() ;
            Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
            
            auto scene = MapScene::createScene();
            auto transition = TransitionFade::create(2.0f, scene);
            Director::getInstance()->replaceScene(transition);
        };

        auto popup_layer = PopupOkButtonLayer::create(func, "通信に遅延が生じているため、\n戦場に入り直します。") ;
        this->addChild(popup_layer, MAP_POPUP_Z);
    }
    
    //checkTimeup();
    sendCommandData();
    //executeCommand();
}
