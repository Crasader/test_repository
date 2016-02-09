#include "NormalScene.h"
#include "RankingEventResultLayer.h"
#include "TurnManage.h"

RankingEventResultLayer::RankingEventResultLayer()
:_check_count(0)
{
}

RankingEventResultLayer* RankingEventResultLayer::create(bool winner_flg, string from_page_name)
{
    RankingEventResultLayer *node = new RankingEventResultLayer();
    if (node && node->init(winner_flg, from_page_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool RankingEventResultLayer::init(bool winner_flg, string from_page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    //メンバ変数に登録
    _winner_flg = winner_flg;
    
    //シーンきりかえフラグ初期化
    _change_scened_flg = false;
    _from_page_name = from_page_name;
    
    //再開用データをクリア
    SaveData::saveString("battle_data_for_restart", "");
    
    return true;
}

void RankingEventResultLayer::getPointDataRequest()
{
    string user_master_id                = SaveData::loadString("user_master_id");
    string choice_friend_id              = SaveData::loadString("choice_friend_id");
    string ranking_event_stage_id        = SaveData::loadString("ranking_event_stage_id");
    string result_log_id                 = SaveData::loadString("result_log_id");
    
    string score_param_turn = "0";
    string score_param_damage = "0";
    string score_param_chara_count = "0";
    string score_param_skill_count = "0";
    string score_param_overkill = "0";
    auto dungeon_scene = (DungeonScene *)this->getParent();
    if(_from_page_name!="TitleScene") {
        score_param_turn = toString<int>(TurnManage::getTurnCount());
        score_param_damage = toString<int>(dungeon_scene->score_param_damage);
        score_param_chara_count = toString<int>(dungeon_scene->score_param_chara_count);
        score_param_skill_count = toString<int>(dungeon_scene->score_param_skill_count);
        score_param_overkill = toString<int>(dungeon_scene->score_param_overkill);
    }
    
    string achi_params = "";
    string check_sum = "";
 
    achi_params = "turn=" + score_param_turn;
    achi_params += "&damage=" + score_param_damage;
    achi_params += "&chara_count=" + score_param_chara_count;
    achi_params += "&skill_count=" + score_param_skill_count;
    achi_params += "&overkill=" + score_param_overkill;

    check_sum = HmsCrypto::md5encode(score_param_turn+"_"+score_param_damage+"_"+score_param_chara_count+"_"+score_param_skill_count+"_"+score_param_overkill) ; //[turn]_[nodamage_flg]_[noskill_flg]_[param_resthp]の順

    //リザルトデータリクエスト
    string url = HOST;
    url += "battle/ranking_event_battle_result.php";
    auto selector = httpresponse_selector(RankingEventResultLayer::getPointDataCallback);
    string body = "user_master_id="+user_master_id+"&result_log_id="+result_log_id+"&winner_flg="+toString<int>(_winner_flg)+"&ranking_event_stage_id="+ranking_event_stage_id+"&" + achi_params + "&chk_sum=" + check_sum ;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void RankingEventResultLayer::getPointDataCallback(HttpClient *sender, HttpResponse *response)
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
    scheduleOnce(schedule_selector(RankingEventResultLayer::showRankingEventResultLabel), 1.0f);
}

void RankingEventResultLayer::createPopup()
{
    //ポップアップ画像の作成
    _titleSprite = Sprite::create("init/Dungeon/result/title_result.png");
    _titleSprite->setScale(ratio);
    _titleSprite->setOpacity(0);
    _titleSprite->setPosition(convertFromDisplaySize(Point(320, 820), 0.5, 0.5));
    addChild(_titleSprite, 2);

    //スコアを表示する
    _scorePopup = ScorePopupSprite::create(json);
    _scorePopup->setScale(ratio);
    _scorePopup->setOpacity(0);
    //_scorePopup->setPosition(convertFromDisplaySize(Point(320, 350), 0.5, 0.5));
    addChild(_scorePopup, 2);
	
    //子ノードのにも適用させる処理
    _setCascadeOpacityEnabled(_scorePopup, true);
}

	
void RankingEventResultLayer::initPopupTypesOrder() {
	_now_popup_queue_index = 0 ;
    _popup_types_queue.insert(PopupType::POPUP_POINT) ;
}

void RankingEventResultLayer::createShadow()
{
    //影レイヤーの作成
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    addChild(shadow, 1);
}

void RankingEventResultLayer::showRankingEventResultLabel(float dt)
{
    //影の表示
    createShadow();

    if (_winner_flg) {
        //勝利時
        auto dungeon_scene = (DungeonScene *)this->getParent();
        float player_hp     = dungeon_scene->player[TurnManage::getPlayerIndex()]->getHp();
        float player_max_hp = dungeon_scene->player[TurnManage::getPlayerIndex()]->getMaxHp();
        HMSLOG("player_hp = %f, player_max_hp = %f", player_hp, player_max_hp);
        
        if (player_hp / player_max_hp < 0.2f) {
            HMSLOG("辛勝");
            showEffect(this, "init/Dungeon/result/plist/win3.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
        }
        else if (player_hp / player_max_hp > 0.8f) {
            HMSLOG("完全勝利");
            showEffect(this, "init/Dungeon/result/plist/win.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
        }
        else {
            HMSLOG("勝利");
            showEffect(this, "init/Dungeon/result/plist/win2.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
        }

        SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/08_win.mp3",true);
    }
    else
    {
        //敗北時
        showEffect(this, "init/Dungeon/result/plist/lose.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
        SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/09_lose.mp3",true);
    }
}

void RankingEventResultLayer::showBattleRankingEventResult()
{
    //獲得ポイントポップアップアクション
    showPointPopupAction();
}

void RankingEventResultLayer::showPointPopupAction()
{
    auto move = MoveTo::create(0.4f, convertFromDisplaySize(Point(320, 520), 0.5, 0.5));
    
    auto func = CallFunc::create([&](){
        auto fadeIn1 = FadeIn::create(0.5f);
        _titleSprite->runAction(fadeIn1);
        
        auto fadeIn2 = FadeIn::create(0.5f);
        _scorePopup->runAction(fadeIn2);
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

Node *RankingEventResultLayer::prevPopupNode(){
	
//	if(_now_popup_queue_index >= _popup_types_queue.size())return Node::create() ;
//	if(_now_popup_queue_index<=0) return Node::create() ;
	
	Node *prevNode ;
	
	//前回に表示されていたポップアップのタイプを取得する
	PopupType prevPopupType = _popup_types_queue[_now_popup_queue_index-1] ;
	
	if(prevPopupType == PopupType::POPUP_POINT){
		prevNode = _scorePopup ;
    }
	
	return prevNode ;
}

void RankingEventResultLayer::touchStartRemove()
{
    
    _touchScreenSprite->stopAllActions() ;
    _touchScreenSprite->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr )) ;
    
}

void RankingEventResultLayer::touchStartAction()
{
    //タッチ画像
    _touchScreenSprite = TouchScreenSprite::create();
    _touchScreenSprite->setScale(ratio);
    _touchScreenSprite->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0));
    this->addChild(_touchScreenSprite, ZORDER_TOUCH_SCREAN, TOUCH_SCREAN_TAG);
}

void RankingEventResultLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void RankingEventResultLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(RankingEventResultLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(RankingEventResultLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(RankingEventResultLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(RankingEventResultLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void RankingEventResultLayer::removeEventListener()
{
    //タップイベントの削除
    _eventDispatcher->removeEventListener(listener);
}

void RankingEventResultLayer::changeScene()
{
    if(!_change_scened_flg){
        _change_scened_flg = true;
        if(SaveData::loadInt("tutorialStatus") == 2){
            //シーンの切り替え
            auto scene = TutorialScene::createScene();
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        }
        else {
            //遷移先ページ名
            string to_page_name = "";
            
            if (_from_page_name == "DungeonScene") {
                to_page_name = "NormalQuest";
            }
            
            //ダンジョンデータの初期化
            Utility::initDungeonData();
            
            //ページ遷移の処理
            auto scene = NormalScene::createScene(to_page_name);
            auto transition = TransitionFade::create(3.0f, scene);
            Director::getInstance()->replaceScene(transition);
        }
    }
}

void RankingEventResultLayer::showEffect(Layer* layer, string filename, float scale, Point pos)
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
        showBattleRankingEventResult();
    });
    
    _resultLabelSprite->runAction(Sequence::create(animate, DelayTime::create(0.4f), func, nullptr));
}

void RankingEventResultLayer::onEnter()
{
    Layer::onEnter();
    
    //リザルトデータ通信処理
    getPointDataRequest();
}

bool RankingEventResultLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch RankingEventResultLayer!");
    return true;
}
void RankingEventResultLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void RankingEventResultLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
	if(_now_popup_queue_index >= _popup_types_queue.size()){
		//インデックスを超えていたらシーン遷移
		changeScene() ;
		return ;
	}
}

void RankingEventResultLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
