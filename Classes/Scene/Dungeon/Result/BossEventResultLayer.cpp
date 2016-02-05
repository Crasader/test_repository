#include "NormalScene.h"
#include "BossEventResultLayer.h"
#include "TurnManage.h"
#include "MapScene.h"

BossEventResultLayer::BossEventResultLayer()
:_check_count(0)
{
}

BossEventResultLayer* BossEventResultLayer::create(bool winner_flg, string winner_string ,string from_page_name)
{
    BossEventResultLayer *node = new BossEventResultLayer();
    if (node && node->init(winner_flg, winner_string, from_page_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool BossEventResultLayer::init(bool winner_flg, string winner_string, string from_page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //メンバ変数に登録
    _winner_flg = winner_flg;
    _winner_string = winner_string;
    
    //シーンきりかえフラグ初期化
    _change_scened_flg = false;
    _from_page_name = from_page_name;
    
    //再開用データをクリア
    SaveData::saveString("battle_data_for_restart", "");
    
    return true;
}

void BossEventResultLayer::getPointDataRequest()
{
    string user_master_id                = SaveData::loadString("user_master_id");
    string choice_friend_id              = SaveData::loadString("choice_friend_id");
    string boss_event_battle_log_id        = SaveData::loadString("boss_event_battle_log_id");
    string result_log_id                 = SaveData::loadString("result_log_id");
    
    string score_param_hp = "";
    auto dungeon_scene = (DungeonScene *)this->getParent();
    if(_from_page_name!="TitleScene") {
        if(dungeon_scene->player[0]!=nullptr) {
            score_param_hp = toString<int>(dungeon_scene->player[0]->getHp());
        } else {
            score_param_hp = "0";
        }
    }
    
    string achi_params = "";
    string check_sum = "";
    
    achi_params += "hp=" + score_param_hp;
    
    check_sum = HmsCrypto::md5encode(toString(user_master_id) + "_" + toString(result_log_id) + "_" + score_param_hp);
    
     //リザルトデータリクエスト
    string url = HOST;
    url += "battle/boss_event_battle_result.php";
    auto selector = httpresponse_selector(BossEventResultLayer::getPointDataCallback);
    string body = "user_master_id="+user_master_id+"&result_log_id="+result_log_id+"&winner_flg="+toString<int>(_winner_flg)+"&winner_string="+_winner_string+"&boss_event_battle_log_id="+boss_event_battle_log_id + "&" + achi_params + "&chk_sum=" + check_sum ;;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void BossEventResultLayer::getPointDataCallback(HttpClient *sender, HttpResponse *response)
{
    //リザルトデータレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("%s",result.c_str());
    
    string err;
    json = Json::parse(result, err);
    
    
    //ポップアップ画像の作成（透明）
    createPopup();
    
    //ポップアップ表示順序(表示されるポップアップを順番に入れてる配列)を作成
    initPopupTypesOrder() ;
    
    //結果文字画像の表示
    scheduleOnce(schedule_selector(BossEventResultLayer::showBossEventResultLabel), 1.0f);
    
    //体力更新
    SaveData::saveString("power", json["after_user_data_list"]["power"].string_value());
    SaveData::saveString("max_power", json["after_user_data_list"]["max_power"].string_value());
    SaveData::saveString("power_use_time", json["after_user_data_list"]["power_use_time"].string_value());
}

void BossEventResultLayer::createPopup()
{
    //ポップアップ画像の作成
    _titleSprite = Sprite::create("init/Dungeon/result/title_result.png");
    _titleSprite->setScale(ratio);
    _titleSprite->setOpacity(0);
    _titleSprite->setPosition(convertFromDisplaySize(Point(320, 820), 0.5, 0.5));
    addChild(_titleSprite, 2);

    //結果画面を表示する
    _popup = BossEventPopupSprite::create(json);
    _popup->setScale(ratio);
    _popup->setOpacity(0);
    //_scorePopup->setPosition(convertFromDisplaySize(Point(320, 350), 0.5, 0.5));
    addChild(_popup, 2);
	
    //子ノードのにも適用させる処理
    _setCascadeOpacityEnabled(_popup, true);
}

	
void BossEventResultLayer::initPopupTypesOrder() {
	_now_popup_queue_index = 0 ;
    _popup_types_queue.insert(PopupType::POPUP_POINT) ;
}

void BossEventResultLayer::createShadow()
{
    //影レイヤーの作成
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(shadow, 1);
}

void BossEventResultLayer::showBossEventResultLabel(float dt)
{
    //影の表示
    createShadow();

    
    if (_winner_flg) {
        //勝利時
        HMSLOG("勝利");
        showEffect(this, "init/Dungeon/result/plist/win2.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));

        SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/08_win.mp3",true);
    }
    else
    {
        if(_winner_string=="escape") {
            //逃走時
            showEffect(this, "init/Dungeon/result/plist/escape.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
            SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/09_lose.mp3",true);
        } else {
            //敗北時
            showEffect(this, "init/Dungeon/result/plist/lose.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
            SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/09_lose.mp3",true);
        }
    }
}

void BossEventResultLayer::showBattleBossEventResult()
{
    //獲得ポイントポップアップアクション
    showPointPopupAction();
}

void BossEventResultLayer::showPointPopupAction()
{
    auto move = MoveTo::create(0.4f, convertFromDisplaySize(Point(320, 520), 0.5, 0.5));
    
    auto func = CallFunc::create([&](){
        auto fadeIn1 = FadeIn::create(0.5f);
        _titleSprite->runAction(fadeIn1);
        
        auto fadeIn2 = FadeIn::create(0.5f);
        _popup->runAction(fadeIn2);
    });
    
    auto func2 = CallFunc::create([&]()
    {
        //ポップアップ表示後にタッチ可能にする
        createEventListener();
        
        //ポイントポップアップアクション開始
        //_scorePopup->scheduleOnce(schedule_selector(PPopupSprite::gaugeAction), 1.0f);
    });
    
    _resultLabelSprite->runAction(Sequence::create(move, func, func2, nullptr));
}

Node *BossEventResultLayer::prevPopupNode(){
	
//	if(_now_popup_queue_index >= _popup_types_queue.size())return Node::create() ;
//	if(_now_popup_queue_index<=0) return Node::create() ;
	
	Node *prevNode ;
	
	//前回に表示されていたポップアップのタイプを取得する
	PopupType prevPopupType = _popup_types_queue[_now_popup_queue_index-1] ;
	
	if(prevPopupType == PopupType::POPUP_POINT){
		prevNode = _popup ;
    }
	
	return prevNode ;
}

void BossEventResultLayer::touchStartRemove()
{
    
    _touchScreenSprite->stopAllActions() ;
    _touchScreenSprite->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr )) ;
    
}

void BossEventResultLayer::touchStartAction()
{
    //タッチ画像
    _touchScreenSprite = TouchScreenSprite::create();
    _touchScreenSprite->setScale(ratio);
    _touchScreenSprite->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0));
    this->addChild(_touchScreenSprite, ZORDER_TOUCH_SCREAN, TOUCH_SCREAN_TAG);
}

void BossEventResultLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void BossEventResultLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BossEventResultLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BossEventResultLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BossEventResultLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BossEventResultLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BossEventResultLayer::removeEventListener()
{
    //タップイベントの削除
    _eventDispatcher->removeEventListener(listener);
}

void BossEventResultLayer::changeScene()
{
    if(!_change_scened_flg){
        _change_scened_flg = true;
       
        //Mapシーンへの遷移
        auto scene = MapScene::createScene();
        auto transition = TransitionFade::create(3.0f, scene);
        Director::getInstance()->replaceScene(transition);
    }
}

void BossEventResultLayer::showEffect(Layer* layer, string filename, float scale, Point pos)
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
    _resultLabelSprite = Sprite::createWithSpriteFrame(animFrames.front());
    _resultLabelSprite->setScale(scale * ratio);
    _resultLabelSprite->setPosition(pos);
    layer->addChild(_resultLabelSprite, 1000);
    
    auto func = CallFunc::create([this]()
    {
        showBattleBossEventResult();
    });
    
    _resultLabelSprite->runAction(Sequence::create(animate, DelayTime::create(0.4f), func, nullptr));
}

void BossEventResultLayer::onEnter()
{
    Layer::onEnter();
    
    //リザルトデータ通信処理
    getPointDataRequest();
}

bool BossEventResultLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch BossEventResultLayer!");
    return true;
}
void BossEventResultLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BossEventResultLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
	if(_now_popup_queue_index >= _popup_types_queue.size()){
		//インデックスを超えていたらシーン遷移
		changeScene() ;
		return ;
	}
}

void BossEventResultLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
