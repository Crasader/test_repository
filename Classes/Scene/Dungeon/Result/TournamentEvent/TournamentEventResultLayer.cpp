
#include "NormalScene.h"
#include "TurnManage.h"
#include "PlistAnimationSprite.h"
#include "TournamentEventResultLayer.h"

TournamentEventResultLayer::TournamentEventResultLayer()
{
}

TournamentEventResultLayer* TournamentEventResultLayer::create(bool winner_flg, string from_page_name)
{
    TournamentEventResultLayer *node = new TournamentEventResultLayer();
    if (node && node->init(winner_flg, from_page_name))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool TournamentEventResultLayer::init(bool winner_flg, string from_page_name)
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

void TournamentEventResultLayer::onEnter()
{
    Layer::onEnter();
    
    //リザルトデータ通信処理
    getPointDataRequest();
}

void TournamentEventResultLayer::getPointDataRequest()
{
    string user_master_id                = SaveData::loadString("user_master_id");
    string enemy_user_master_id          = SaveData::loadString("enemy_user_master_id");
    int battle_list_id                   = SaveData::loadInt("battle_list_id");
    string tournament_event_list_id = SaveData::loadString("tournament_event_list_id");
    
    //リザルトデータリクエスト
    string url = HOST;
    url += "battle/tournament_event_battle_result.php";
    auto selector = httpresponse_selector(TournamentEventResultLayer::getPointDataCallback);
    string body = "user_master_id="+user_master_id;
    body += "&enemy_user_master_id="+enemy_user_master_id;
    body += "&winner_flg="+toString<int>(_winner_flg);
    body += "&battle_list_id="+toString<int>(battle_list_id);
    body += "&tournament_event_list_id="+tournament_event_list_id;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEventResultLayer::getPointDataCallback(HttpClient *sender, HttpResponse *response)
{
    //リザルトデータレスポンス
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result: %s",result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    //表示内容のタイプ
    _tournament_result_type = (TournamentResultType)json["tournament_event_data"]["tournament_result_type"].int_value() ;
    
    if(_from_page_name == "DungeonScene"){
        
        //ユーザーデータの更新
    	updateUserData();
        
        //ポップアップ画像の作成（透明）
        createSprites(json);
    	
        //結果文字画像の表示
        scheduleOnce(schedule_selector(TournamentEventResultLayer::showResultLabel), 1.0f);
        
    }else if(_from_page_name == "TitleScene"){
        
        //ユーザーデータの更新
    	updateUserData();
        
        //Normalシーンへ遷移
        changeScene() ;
    }
}

void TournamentEventResultLayer::updateUserData()
{
    //リザルト結果をユーザーデータに更新
}

void TournamentEventResultLayer::createSprites(Json json)
{
    string navi_comment = json["tournament_event_data"]["navi_comment"].string_value() ;
    string next_round_time_text = json["tournament_event_data"]["next_round_time_text"].string_value() ;
    string round_no_text = "第" + json["tournament_event_data"]["now_round_no"].string_value() + "回戦目" ;
    string tournament_name = json["tournament_event_data"]["tournament_name"].string_value() ;
    
    //「対戦結果」と書かれた円形の画像の表示
    _battle_result_circle = Sprite::create("init/Dungeon/result/title_result.png");
    _battle_result_circle->setScale(ratio);
    _battle_result_circle->setOpacity(0);
    _battle_result_circle->setPosition(convertFromDisplaySize(Point(320, 820), 0.5, 0.5));
    this->addChild(_battle_result_circle, O_battle_result_circle);
    
    //ナビキャラの表示 (右上をアンカーポイントに左下に表示)
    _navi_sprite = Sprite::create("init/Dungeon/result/navi.png");
    _navi_sprite->setScale(ratio);
    _navi_sprite->setAnchorPoint(Point(1,1));
    _navi_sprite->setPosition(Point(0,0));
    this->addChild(_navi_sprite, O_navi_chara);
    if(_tournament_result_type == TournamentResultType::VICTORY){
        //優勝の場合Jumpさせる
        _navi_sprite->runAction(RepeatForever::create(Sequence::create(DelayTime::create(1), JumpBy::create(0.5, Point::ZERO, ratio*10, 2), nullptr)));
    }
    
    //ナビキャラの吹き出し
    _navi_balloon = Sprite::create("init/Dungeon/result/navi_balloon.png");
    _navi_balloon->setAnchorPoint(Point(0,0.5)) ;
    _navi_balloon->setPosition(Point(166*ratio,182*ratio));
    _navi_balloon->setScale(0);
    this->addChild(_navi_balloon, O_navi_balloon);
    
    //ナビキャラのコメント
    auto balloon_label = HMSFontUtil::createWithTTFBySystemFontArgument(navi_comment , FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(navi_comment, 32, 370));
    balloon_label->enableOutline(Color4B(255, 255, 255, 0), 1) ;
    balloon_label->setPosition(Point(256,70)) ;
    balloon_label->setTextColor(Color4B::BLACK) ;
    balloon_label->setAlignment(TextHAlignment::CENTER);
    _navi_balloon->addChild(balloon_label) ;
    
    if(_tournament_result_type == TournamentResultType::EXIST_NEXT_BATTLE){
        //トーナメントの結果を表示するスプライト
        _result_sprite = Sprite::create("init/Dungeon/result/bk_match_result2.png") ;
        _result_sprite->setScale(ratio);
        _result_sprite->setOpacity(0);
        _result_sprite->setCascadeOpacityEnabled(true) ;
        _result_sprite->setPosition(convertFromDisplaySize(Point(320, 400), 0.5, 0.5));
        this->addChild(_result_sprite, O_result_sprite);
        
        //トーナメントの名前
        auto label_tournament_name = HMSFontUtil::createWithTTFBySystemFontArgument(tournament_name , FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(tournament_name, 32, 370));
        label_tournament_name->setPosition(Point(_result_sprite->getContentSize().width/2, 228)) ;
        label_tournament_name->setAlignment(TextHAlignment::CENTER);
        label_tournament_name->setTextColor(Color4B::YELLOW) ;
        _result_sprite->addChild(label_tournament_name) ;
        
        //何回戦目かの表示(e.g. 「第1回戦目」)
        auto round = HMSFontUtil::createWithTTFBySystemFontArgument(round_no_text , FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(round_no_text, 32, 370));
        round->setPosition(Point(_result_sprite->getContentSize().width/2, 170)) ;
        round->setTextColor(Color4B::YELLOW) ;
        round->setAlignment(TextHAlignment::CENTER);
        _result_sprite->addChild(round) ;
        
        //「次回対戦日時」という文字
        auto next_time_text = HMSFontUtil::createWithTTFBySystemFontArgument("次回対戦日時" , FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize("次回対戦日時", 28, 370));
        next_time_text->setPosition(Point(_result_sprite->getContentSize().width/2, 121)) ;
        next_time_text->setAlignment(TextHAlignment::CENTER);
        _result_sprite->addChild(next_time_text) ;
        
        //次回の日時の表示(e.g. 「2015年09月15日(火)11:11」)
        auto next_time = HMSFontUtil::createWithTTFBySystemFontArgument(next_round_time_text , FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(next_round_time_text, 32, 500));
        next_time->setPosition(Point(_result_sprite->getContentSize().width/2, 75)) ;
        next_time->setAlignment(TextHAlignment::CENTER);
        _result_sprite->addChild(next_time) ;
    }
}

void TournamentEventResultLayer::createShadow()
{
    //影レイヤーの作成
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, O_shadow);
}

void TournamentEventResultLayer::showResultLabel(float dt)
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
        } else if (player_hp / player_max_hp > 0.8f) {
            HMSLOG("完全勝利");
            showEffect(this, "init/Dungeon/result/plist/win.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
        } else {
            HMSLOG("勝利");
            showEffect(this, "init/Dungeon/result/plist/win2.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
        }
        SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/08_win.mp3",true);
        
    } else {
        
        //敗北時
        HMSLOG("敗北");
        showEffect(this, "init/Dungeon/result/plist/lose.plist", 1.0, convertFromDisplaySize(Point(320, 440), 0.5, 0.5));
        SimpleAudioEngine::getInstance()->playBackgroundMusic("download/BGM/09_lose.mp3",true);
    }
}

void TournamentEventResultLayer::showResult()
{
    auto move = MoveTo::create(0.4f, convertFromDisplaySize(Point(320, 520), 0.5, 0.5));
    
    auto func = CallFunc::create([&](){
        if(_battle_result_circle != nullptr){
            auto fadeIn1 = FadeIn::create(0.5f);
            _battle_result_circle->runAction(fadeIn1);
        }
        
        //対戦結果をフェードインで表示
        if(_tournament_result_type == TournamentResultType::EXIST_NEXT_BATTLE){
            auto fadeIn2 = FadeIn::create(0.5f);
            _result_sprite->runAction(fadeIn2);
        }
        
        if(_navi_sprite != nullptr){
            //ナビキャラの表示
            auto move_chara = MoveTo::create(0.2f, Point(278*ratio, 316*ratio));
            auto ease_chara = EaseOut::create(move_chara, 2);
            _navi_sprite->runAction(Sequence::create(DelayTime::create(0.5),ease_chara, nullptr)) ;
            
            //SE
            auto sound = CallFunc::create([this](){
                if(_tournament_result_type == TournamentResultType::VICTORY) {
                    CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/beginner_guide/get_item.mp3");
                    createConfetti() ;
                }
            });
            
            //吹き出しの表示
            auto scale_balloon = ScaleTo::create(0.5f, 1.0f*ratio);
            auto ease_balloon = EaseSineOut::create(scale_balloon);
            _navi_balloon->runAction(Sequence::create(DelayTime::create(0.7), sound, ease_balloon, nullptr)) ;
        }
    });
    
    auto func2 = CallFunc::create([&]() {
        //ポップアップ表示後にタッチ可能にする
        touchStartAction() ;
        createEventListener();
    });
    
    _resultLabelSprite->runAction(Sequence::create(move, func, DelayTime::create(1.2f), func2, nullptr));
}

void TournamentEventResultLayer::createConfetti()
{
    //紙吹雪の生成
    _confetti_1 = Sprite::create("init/Home/Beginner/Clear/confetti.png") ;
    _confetti_1->setAnchorPoint(Point(0.5, 0));
    _confetti_1->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0), 0.5, 0.0)) ;
    _confetti_1->setScale(ratio) ;
    this->addChild(_confetti_1, O_confetti) ;


    //紙吹雪の生成
    _confetti_2 = Sprite::create("init/Home/Beginner/Clear/confetti.png") ;
    _confetti_2->setAnchorPoint(Point(0.5, 0));
    _confetti_2->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, _confetti_2->getContentSize().height), 0.5, 0.0)) ;
    _confetti_2->setScale(ratio) ;
    this->addChild(_confetti_2, O_confetti) ;

    //ブレンド　（加算合成）
    BlendFunc blend;
    blend.src = GL_SRC_ALPHA;
    blend.dst = GL_ONE;
    _confetti_1->setBlendFunc(blend);
    _confetti_2->setBlendFunc(blend);

    schedule(schedule_selector(TournamentEventResultLayer::moveConfetti), 0.01f);
}

void TournamentEventResultLayer::moveConfetti(float dt)
{
    _confetti_1->setPosition(_confetti_1->getPosition().x, _confetti_1->getPosition().y - 1.5f*ratio);
    _confetti_2->setPosition(_confetti_2->getPosition().x, _confetti_2->getPosition().y - 1.5f*ratio);
    
    if (_confetti_1->getPosition().y <= -_confetti_1->getContentSize().height*ratio)
    {
        _confetti_1->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT), 0.5, 1.0)) ;
    }
    
    if (_confetti_2->getPosition().y <= -_confetti_2->getContentSize().height*ratio)
    {
        _confetti_2->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT), 0.5, 1.0)) ;
    }
}

void TournamentEventResultLayer::touchStartRemove()
{
    TouchScreenSprite* touchScreenSprite = (TouchScreenSprite*)getChildByTag(TOUCH_SCREAN_TAG)  ;
    if(touchScreenSprite!=nullptr){
        touchScreenSprite->stopAllActions() ;
        touchScreenSprite->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr )) ;
    }
}

void TournamentEventResultLayer::touchStartAction()
{
    //タッチ画像
    TouchScreenSprite *touchScreenSprite = TouchScreenSprite::create();
    touchScreenSprite->setScale(ratio);
    touchScreenSprite->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0));
    this->addChild(touchScreenSprite, O_touch_screen);
}


void TournamentEventResultLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TournamentEventResultLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentEventResultLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentEventResultLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TournamentEventResultLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentEventResultLayer::removeEventListener()
{
    //タップイベントの削除
    _eventDispatcher->removeEventListener(listener);
}

void TournamentEventResultLayer::changeScene()
{
    if(!_change_scened_flg){
        _change_scened_flg = true;
        
        //ページ遷移の処理
        auto scene = NormalScene::createScene("");
        auto transition = TransitionFade::create(3.0f, scene);
        Director::getInstance()->replaceScene(transition);
    }
}

void TournamentEventResultLayer::showEffect(Layer* layer, string filename, float scale, Point pos)
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
        showBattleResult();
    });
    
    _resultLabelSprite->runAction(Sequence::create(animate, DelayTime::create(0.4f), func, nullptr));
}

void TournamentEventResultLayer::showBattleResult()
{
    //獲得ポイントポップアップアクション
    showResult();
}


bool TournamentEventResultLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("touch TournamentEventResultLayer!");
    return true;
}
void TournamentEventResultLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TournamentEventResultLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タッチスクリーンのスプライトを削除
    touchStartRemove() ;

    //インデックスを超えていたらシーン遷移
    changeScene() ;
}

void TournamentEventResultLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
