#include "NormalScene.h"

time_t NormalScene::_tournament_next_battle_time = 0;
time_t NormalScene::_team_battle_next_battle_time = 0;

NormalScene::NormalScene()
:_main_page_name("Mypage")
,_change_page_flg(false)
,_change_page_count(0)
,_change_page_name()
,_bg_status(BgStatus::Main)
{
}

Scene* NormalScene::createScene(string page_name)
{
    auto scene = Scene::create();
    auto layer = NormalScene::create(page_name);
    scene->addChild(layer);
    return scene;
}

NormalScene* NormalScene::create(string page_name)
{
    NormalScene *layer = new NormalScene();
    
    if (layer && layer->init(page_name))
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool NormalScene::init(string page_name)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("NormalScene::init()");
    
    ratio = getVisibleRatio();
    
    //背景の表示
    showBg();

    //キャッシュの解放
    Director::getInstance()->purgeCachedData();
    
    //最初のページ
    auto main_layer = MypageLayer::create();
    this->addChild(main_layer, ZORDER_MAIN_LAYER, MAIN_LAYER_TAG);
    
    //タイトルバーレイヤーの作成
    auto title_layer = TitleBarLayer::create();
    this->addChild(title_layer, ZORDER_TITLE_LAYER, TITLE_LAYER_TAG);
    
    //ヘッダーの設定
    auto header_layer = HeaderLayer::create();
    this->addChild(header_layer, ZORDER_HEADER_LAYER, HEADER_LAYER_TAG);
    
    //チャットレイヤーの設定
    auto chat_base_upper_layer = ChatBaseUpperLayer::create("NormalScene");
    header_layer->addChild(chat_base_upper_layer, 999999, HeaderLayer::T_ChatLayerTag);
    
    //フッターの設定
    auto footer_layer = FooterLayer::create();
    this->addChild(footer_layer, ZORDER_FOOTER_LAYER, FOOTER_LAYER_TAG);
    
    //タップエフェクトレイヤーの作成
    auto tap_effect_layer = TapEffectLayer::create();
    this->addChild(tap_effect_layer,9999999);
    
    if (page_name == "TitleScene")
    {
        //お知らせレイヤー（タイトルからマイページに入ったとき表示させたい場合に使用）
        //        string url = HOST;
        //        url += "user/index.php?p=page__info";
        //        auto webview_layer = WebViewLoadLayer::create(url, "お知らせ");
        //        this->addChild(webview_layer, ZORDER_INFO_LAYER, INFO_LAYER_TAG);
    }
    
    if(page_name != "" && SaveData::loadBool("beginner_layer_open_flg") != true) {
        this->_change_page_name = page_name;
        //スケジューラー登録
        this->schedule(schedule_selector(NormalScene::updateOne), 0.3f);//高速化
    }
    
    //両サイドの余白に画像を配置する
    cutLeftAndRightVisibleSpace() ;
    
    //キャラクター情報のセットアップ
    updateUserCharaDataByUserDefault();
    
    //並び替えのパラメータを設定
    sort_param = "power";
    
    //選択中デッキ情報読み込み
    deckNo = fromString<int>( SaveData::loadString("party_no"));
    
    if (deckNo < 1 || deckNo > deck_list.size()) {
        deckNo = 1;
    }
	
    //BGMの設定と再生
    this->runAction(Sequence::create(DelayTime::create(0.5f),
                                     PlayBGM::create("download/BGM/03_home.mp3"),
                                     FadeToVolumeOfBGM::create(0.f, SaveData::loadFloat("bgm_volume")),
                                     CallFunc::create([](){HMSLOG("volume:%f", SimpleAudioEngine::getInstance()->getBackgroundMusicVolume());}),nullptr)
                                     );
    //自動遷移の設定
    SaveData::saveBool("normal_scene_change_page_ok_flg", false);
    
    return true;
}

void NormalScene::updateOne(float dt)
{
    if(SaveData::loadBool("normal_scene_change_page_ok_flg")) {
        unschedule(schedule_selector(NormalScene::updateOne));
        
        this->changePage(_change_page_name);
    }
}

void NormalScene::showBg()
{
    //背景
    auto bg = Sprite::create("init/Home/Mypage/back.png");
    bg->setScale(ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    bg->setCascadeOpacityEnabled(true);
    this->addChild(bg, 1, T_Bg);
}

void NormalScene::cutLeftAndRightVisibleSpace()
{
    int space_width = getVisibleSpaceWidth();
    
    //左右の余白がある場合、余白部分を黒くする
    if(space_width > 0){
        //左の柄
        Sprite *left_sprite = Sprite::create("init/Home/Common/side.png");
        left_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        left_sprite->setAnchorPoint(Point(1,0));
        left_sprite->setScale(ratio);
        left_sprite->setPosition(convertFromDisplaySize(Point(0, 0), 0.5, 0));
        this->addChild(left_sprite);
        
        //右の柄
        Sprite *right_sprite = Sprite::create("init/Home/Common/side.png");
        right_sprite->setGlobalZOrder(1); //←強制的に描画を優先させる(なるべく使用は控える)
        right_sprite->setAnchorPoint(Point(0,0));
        right_sprite->setScale(ratio);
        right_sprite->setPosition(convertFromDisplaySize(Point(640, 0), 0.5, 0));
        this->addChild(right_sprite);
        
    }
}

void NormalScene::changePage(string next_page_name)
{
    CCLOG("main_page_name = %s, next_page_name = %s", _main_page_name.c_str(), next_page_name.c_str());
    
    //同じページの場合又は、通信中の場合遷移させない
    if (_main_page_name != next_page_name && !(HttpConnect::_connect_flg) && !(_change_page_flg)) {
        
        _change_page_flg = true;
        
        //ページ切り替えを行う
        changePageActionRemoveAndSet(next_page_name);
        
        return;
    }
}

void NormalScene::changePageActionRemoveAndSet(string next_page_name)
{
    //アクション中はタップ禁止にする
    addTapDisableLayer();
    
    //ページを画面外へ消すアクション
    auto main_layer = this->getChildByTag(MAIN_LAYER_TAG);
    auto move = MoveTo::create(MOVING_TIME, Point(_contentSize.width, 0));
    auto ease = EaseIn::create(move, 1);
    
    auto title_bar_layer = this->getChildByTag(TITLE_LAYER_TAG);
    auto move2 = MoveTo::create(MOVING_TIME, Point(-(_contentSize.width), 0));
    auto ease2 = EaseIn::create(move2, 1);
    
    auto remove_func = CallFunc::create([this, title_bar_layer](){
        
        //タイトルバーレイヤーのクリア
        title_bar_layer->removeAllChildren();
    });
    
    auto change_func = CallFunc::create([this, title_bar_layer, next_page_name](){

        //ページ名によって作成するページを切り替え
        Layer* next_layer = createPageLayer(next_page_name);

        if (next_layer) {
            
            //作成したページを画面右に配置
            next_layer->setPosition(Point(_contentSize.width, 0));
            this->addChild(next_layer, ZORDER_MAIN_LAYER, MAIN_LAYER_TAG);
            
            //背景の切り替え
            changeBgAction(next_page_name);
            
            //フッターの表示切り替え
            changeFotterAction(next_page_name);
            
            //挑戦者ボタンのチェック
            removeChallengerSprite();
            
            //トーナメントイベントの終了時間チェック
            checkTournamentStartDateSprite(next_page_name);
            
            //フッターの画像を再構築※対象のレイヤーに移動時のみ
            FooterLayer* footer_layer = (FooterLayer*)this->getChildByTag(FOOTER_LAYER_TAG);
            if (next_page_name == "Mypage") footer_layer->recreateButtonSprite(HOME_BUTTON_TAG);
            else if (next_page_name == "QuestMenu" || next_page_name == "NormalQuest" || next_page_name == "EventMenu") footer_layer->recreateButtonSprite(QUEST_BUTTON_TAG);
            else if (next_page_name == "CharaMenu") footer_layer->recreateButtonSprite(CHARA_BUTTON_TAG);
            else if (next_page_name == "GachaMenu") footer_layer->recreateButtonSprite(GACHA_BUTTON_TAG);
            else if (next_page_name == "RevisionGachaMenu") footer_layer->recreateButtonSprite(GACHA_BUTTON_TAG);
            else if (next_page_name == "ShopMenu") footer_layer->recreateButtonSprite(SHOP_BUTTON_TAG);
            else if (next_page_name == "FriendMenu") footer_layer->recreateButtonSprite(FRIEND_BUTTON_TAG);
            
            //表示アクション
            setActionMainPage(next_page_name);
        }
    });
    
    TargetedAction *target_action_main_layer = TargetedAction::create(main_layer, Sequence::create(ease, RemoveSelf::create(true), nullptr)) ;
    TargetedAction *target_action_title_bar = TargetedAction::create(title_bar_layer, Sequence::create(ease2, remove_func, nullptr)) ;
    this->runAction(Sequence::create(Spawn::create(target_action_main_layer, target_action_title_bar, nullptr), change_func, nullptr));

    
    //効果音
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/normal_tap.mp3",false);
}

void NormalScene::setActionMainPage(string next_page_name)
{
    //ページの表示アクション
    auto next_layer = (Layer *)this->getChildByTag(MAIN_LAYER_TAG);
    auto title_bar_layer = this->getChildByTag(TITLE_LAYER_TAG);
    
    if (next_layer && title_bar_layer) {
        
        //タグとページ名の更新
        setMainName(next_page_name);
        
        auto move = MoveTo::create(MOVING_TIME, Point(0, 0));
        auto ease = EaseOut::create(move, 4);
        
        auto move2 = MoveTo::create(MOVING_TIME, Point(0, 0));
        auto ease2 = EaseOut::create(move2, 4);
        
        auto func = CallFunc::create([this](){
            
            //ページ切り替え数の更新
            _change_page_count++;

            //タップ可能にする
            removeTapDisableLayer();
            
            _change_page_flg = false;
        });
        
        this->runAction(Sequence::create(Spawn::create(TargetedAction::create(next_layer, ease),
                                                       TargetedAction::create(title_bar_layer, ease2),nullptr)
                                         ,func ,nullptr));
    }
}

void NormalScene::changePageFadeAction(string next_page_name)
{
    //アクション中はタップ禁止にする
    addTapDisableLayer();

    //黒いスプライトを透明度0で追加
    auto color_sprite = Sprite::create();
    color_sprite->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
    color_sprite->setTextureRect(Rect(0, 0, _contentSize.width, _contentSize.height));
    color_sprite->setColor(Color3B::BLACK);
    color_sprite->setGlobalZOrder(1); //zOrderに関係無く強制的に前面に表示するメソッド よっぽどのことが無ければ基本的に使用しない方がよい
    color_sprite->setOpacity(0) ;
    addChild(color_sprite,99999);
    
    auto layer_change_func = CallFunc::create([this, next_page_name]() {
        //タップ可能にする
        removeTapDisableLayer();
        changePageActionRemoveAndSet(next_page_name);
    });
    
    //徐々に色をつける処理
    FadeIn *fade_in = FadeIn::create(0.5f) ;
    FadeOut *fade_out = FadeOut::create(0.5f) ;
    color_sprite->runAction(Sequence::create(fade_in, layer_change_func, DelayTime::create(1.4f), fade_out, nullptr));
}

void NormalScene::getBgImagePathAndMusicPath(string next_page_name, string *path)
{
    //クエストページへの遷移
    if ((_main_page_name == "QuestMenu" || _main_page_name == "Mypage" || _main_page_name == "ChallengeEventMenu") && next_page_name == "NormalQuest") {
        _bg_status = BgStatus::Quest;
        path[0] = "init/Home/Quest/back_map.png";
        path[1] = "download/BGM/04_quest.mp3";
    }
    
    //通常イベントページへの遷移
    else if ((_main_page_name == "QuestMenu" || _main_page_name == "Mypage") && next_page_name == "EventMenu") {
        _bg_status = BgStatus::Event;
        path[0] = "init/Home/Event/NpcBattleEvent/back_event.png";
        path[1] = "download/BGM/11_event.mp3";
    }
    
    //ランキングイベント（国士無双）ページへの遷移
    else if ((_main_page_name == "QuestMenu" || _main_page_name == "Mypage") && next_page_name == "RankingEventTop") {
        _bg_status = BgStatus::Ranking;
        path[0] = "init/Home/Event/RankingEvent/back.png";
        path[1] = "download/BGM/01_opening.mp3";
    }
    
    //チャレンジイベント（難関ステージ）ページへの遷移
    else if ((_main_page_name == "NormalQuest" || _main_page_name == "Mypage") && next_page_name == "ChallengeEventMenu") {
        _bg_status = BgStatus::Challenge;
        path[0] = "init/Home/Event/ChallengeEvent/back.png";
        path[1] = "download/BGM/01_opening.mp3";
    }
    
    //トーナメントページへの遷移
    else if ((_main_page_name == "QuestMenu" || _main_page_name == "Mypage") && (next_page_name == "TournamentEventTop" || next_page_name == "TournamentEventBattleWait")) {
        _bg_status = BgStatus::Tournament;
        path[0] = "init/Home/Event/TournamentEvent/back.png";
        path[1] = "download/BGM/11_event.mp3";
    }
    //ボスイベントへの遷移
    else if ((_main_page_name == "QuestMenu" || _main_page_name == "Mypage" || _main_page_name == "TeamTop" ) && next_page_name == "BossEventTop") {
        _bg_status = BgStatus::Boss;
        path[0] = "init/Home/Event/BossEvent/back.png";
        path[1] = "download/BGM/11_event.mp3";
    }
    //チームへの遷移 (マイページまたはボスイベントTOPからチーム関係のページに遷移した時のBGMと背景の変更)
    else if ((_main_page_name == "Mypage" || _main_page_name == "BossEventTop") && (next_page_name == "TeamTop" || next_page_name == "TeamNoBelong")) {
        _bg_status = BgStatus::Team;
        path[0] = "init/Team/back.png";
        path[1] = "download/BGM/11_event.mp3";
    }
    
    //メインページへ戻す
    else if (_bg_status != Main && (next_page_name == "Mypage" || next_page_name == "QuestMenu"
                                    || next_page_name == "CharaMenu" || next_page_name == "GachaMenu" || next_page_name == "RevisionGachaMenu"
                                    || next_page_name == "ShopMenu"  || next_page_name == "FriendMenu")) {
        _bg_status = BgStatus::Main;
        path[0] = "init/Home/Mypage/back.png";
        path[1] = "download/BGM/03_home.mp3";
    }
}

void NormalScene::changeBgAction(string next_page_name)
{
    //背景の切り替え処理を行う
    auto bg = (Sprite *)this->getChildByTag(T_Bg);
    
    auto fadeOut = FadeOut::create(0.5f);
    auto fadeIn  = FadeIn::create(0.5f);
    
    string path[2];
    getBgImagePathAndMusicPath(next_page_name, path);
    
    auto change_func = CallFunc::create([this, bg, path](){
        
        if (_bg_status == Quest) createCloud();
        else if (_bg_status == Event) createSparks();
        else if (_bg_status == Tournament) {
            this->showEffect("init/Home/Event/TournamentEvent/fire.plist", Point(43, 352), 1.8);
            this->showEffect("init/Home/Event/TournamentEvent/fire.plist", Point(592, 352), 1.8);
        }
        else if (_bg_status == Main) bg->removeAllChildren();
        
        bg->setTexture(path[0]);
        
        //BGMの設定と再生
        this->runAction(PlayBGM::create(path[1].c_str()));
    });
    //    HMSLOG("path[0] = %s, path[1] = %s path.size = %ld", path[0].c_str(), path[1].c_str(), (sizeof(path) / sizeof(path[0])));
    
    if (path[0] != "" && path[1] != "") {
        bg->runAction(Sequence::create(fadeOut, change_func, fadeIn, nullptr));
    }
}

void NormalScene::createCloud()
{
    //クエストメニューの雲を生成し、動かす処理
    auto bg = (Sprite *)this->getChildByTag(T_Bg);
    auto cloud1 = (Sprite *)bg->getChildByTag(T_Cloud1);
    
    if(cloud1 == nullptr) {
        auto cloud1 = Sprite::create("init/Home/Quest/cloud1.png");
        cloud1->setAnchorPoint(Point(0.5, 0.5));
        cloud1->setPosition(Point(bg->getContentSize() / 2));
        bg->addChild(cloud1, Z_Cloud1, T_Cloud1);
        
        auto cloud2 = Sprite::create("init/Home/Quest/cloud2.png");
        cloud2->setAnchorPoint(Point(0.5, 0.5));
        cloud2->setPosition(Point(bg->getContentSize().width / 2 + cloud1->getContentSize().width, bg->getContentSize().height / 2));
        bg->addChild(cloud2, T_Cloud2, T_Cloud2);
        
        auto move1 = MoveBy::create(17.0f, Point(- (cloud1->getContentSize().width), 0));
        auto move2 = MoveBy::create(17.0f, Point(- (cloud2->getContentSize().width), 0));
        
        auto func1 = CallFunc::create([this, cloud1, cloud2, bg](){
            if (cloud1->getPosition().x == -320) {
                cloud1->setPosition(Point(bg->getContentSize().width / 2 + cloud2->getContentSize().width, bg->getContentSize().height / 2));
            }
        });
        
        auto func2 = CallFunc::create([this, cloud1, cloud2, bg](){
            if (cloud2->getPosition().x == -320) {
                cloud2->setPosition(Point(bg->getContentSize().width / 2 + cloud1->getContentSize().width, bg->getContentSize().height / 2));
            }
        });
        
        bg->runAction(Spawn::create(TargetedAction::create(cloud1, Repeat::create(Sequence::create(move1, func1, nullptr), -1)),
                                    TargetedAction::create(cloud2, Repeat::create(Sequence::create(move2, func2, nullptr), -1)),
                                    nullptr));
    }
}

void NormalScene::createSparks()
{
    //イベントの火の粉を作成、動かす
    auto bg = (Sprite *)this->getChildByTag(T_Bg);
    
    auto sparks1 = Sprite::create("init/Home/Event/NpcBattleEvent/sparks2.png");
    sparks1->setAnchorPoint(Point(0.5, 0.5));
    sparks1->setPosition(Point(bg->getContentSize() / 2));
    bg->addChild(sparks1, 2, 12);
    
    auto sparks2 = Sprite::create("init/Home/Event/NpcBattleEvent/sparks2.png");
    sparks2->setAnchorPoint(Point(0.5, 0.5));
    sparks2->setPosition(Point(bg->getContentSize().width / 2, bg->getContentSize().height / 2 - (sparks1->getContentSize().height)));
    bg->addChild(sparks2, 2, 13);
    
    //ブレンド　（加算合成）
    BlendFunc blend;
    blend.src = GL_SRC_ALPHA;
    blend.dst = GL_ONE;
    sparks1->setBlendFunc(blend);
    sparks2->setBlendFunc(blend);

    float up_position = bg->getContentSize().height / 2 + sparks1->getContentSize().height;
    float bottom_position = bg->getContentSize().height / 2 - sparks1->getContentSize().height;
    
    auto move1 = MoveBy::create(23.0f, Point(0, sparks1->getContentSize().height));
    auto move2 = MoveBy::create(23.0f, Point(0, sparks2->getContentSize().height));
    
    auto func1 = CallFunc::create([this, bg, sparks1, up_position, bottom_position](){
        if (sparks1->getPosition().y == up_position) {
            sparks1->setPosition(Point(bg->getContentSize().width / 2 , bottom_position));
        }
    });
    
    auto func2 = CallFunc::create([this, bg, sparks2, up_position, bottom_position](){
        if (sparks2->getPosition().y == up_position) {
            sparks2->setPosition(Point(bg->getContentSize().width / 2 , bottom_position));
        }
    });
    
    bg->runAction(Spawn::create(TargetedAction::create(sparks1, Repeat::create(Sequence::create(move1, func1, nullptr), -1)),
                                TargetedAction::create(sparks2, Repeat::create(Sequence::create(move2, func2, nullptr), -1)),
                                nullptr));
}

void NormalScene::removeChallengerSprite()
{
    //マイページからの遷移時、挑戦者ボタンがあったら削除する
    if (_main_page_name == "Mypage")
    {
        auto challenger_sp = this->getChildByTag(FOOTER_LAYER_TAG)->getChildByTag(CHALLENGER_BUTTON_TAG);
        if (challenger_sp)
        {
            //アクションを止める
            challenger_sp->stopAllActions();
            
            //小さくして消す
            auto scale = ScaleTo::create(0.2f, 0);
            challenger_sp->runAction(Sequence::create(scale, RemoveSelf::create(), nullptr));
        }
    }
}

void NormalScene::changeFotterAction(string next_page_name)
{
    //フッターレイヤーの表示切り替え
    
    //_main_page_name(遷移元)が以下のどれかのとき、フッターを表示
    if (_main_page_name == "DeckFormation" || _main_page_name == "TournamentEventBracket" || _main_page_name == "CreateRoom" || _main_page_name == "BattleFree") {
        showFotterLayer();
    }
    
    //_main_page_name(遷移先)が以下のどれかのとき、フッターを隠す
    if (next_page_name == "DeckFormation" || next_page_name == "TournamentEventBracket" || next_page_name == "CreateRoom" || next_page_name == "BattleFree") {
        hideFotterLayer();
    }
    
    //_main_page_name(遷移先)が以下のどれかのとき、ヘッダを表示
    if (next_page_name == "TournamentEventTop" || next_page_name == "TournamentList" || next_page_name == "TournamentEntryList") {
        showHeaderLayer();
    }
    
    //_main_page_name(遷移先)が以下のどれかのとき、ヘッダを隠す
    if (next_page_name == "TournamentEventBracket") {
        hideHeaderLayer();
    }
    
}

void NormalScene::hideHeaderLayer()
{
    auto header_layer = (HeaderLayer*)this->getChildByTag(HEADER_LAYER_TAG);
    auto move = MoveTo::create(0.5f, Point(0, _contentSize.height));
    auto ease = EaseIn::create(move, 2);
    
    auto chat_layer = (ChatBaseUpperLayer*)header_layer->getChildByTag(HeaderLayer::T_ChatLayerTag);
    if(chat_layer){
        chat_layer->hideChatLayer() ;
    }
    
    header_layer->runAction(ease);
}

void NormalScene::showHeaderLayer()
{
    auto header_layer = (HeaderLayer*)this->getChildByTag(HEADER_LAYER_TAG);
    auto move = MoveTo::create(0.5f, Point(0, 0));
    auto ease = EaseOut::create(move, 2);
    header_layer->runAction(ease);
}

void NormalScene::hideFotterLayer()
{
    auto footer_layer = (FooterLayer*)this->getChildByTag(FOOTER_LAYER_TAG);
    auto move = MoveTo::create(0.5f, Point(0, - _contentSize.height));
    auto ease = EaseIn::create(move, 2);
    footer_layer->runAction(ease);
}

void NormalScene::showFotterLayer()
{
    auto footer_layer = (FooterLayer*)this->getChildByTag(FOOTER_LAYER_TAG);
    auto move = MoveTo::create(0.5f, Point(0, 0));
    auto ease = EaseOut::create(move, 2);
    footer_layer->runAction(ease);
}

void NormalScene::showEffect(string filename, Vec2 position, float scale)
{
    auto bg = (Sprite *)this->getChildByTag(T_Bg);
    
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
    auto animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.06f);
    auto animate   = cocos2d::Animate::create(animation);
    
    // スプライトを生成してアクションを実行（空のスプライトでも大丈夫）
    auto sprite = Sprite::createWithSpriteFrame(animFrames.front());
    sprite->setScale(scale);
    sprite->setPosition(position);
    bg->addChild(sprite, 10000);
    
    //ブレンド　（加算合成）
    BlendFunc blend;
    blend.src = GL_SRC_ALPHA;
    blend.dst = GL_ONE;
    sprite->setBlendFunc(blend);
    
    //永続的に発生させる
    auto repeat = RepeatForever::create(animate);
    
    sprite->runAction(repeat);
}

void NormalScene::checkTournamentStartDateSprite(string next_page_name)
{
    if (next_page_name == "TournamentEventDeckChoice" ||
        next_page_name == "TournamentEventBattleWait") {
        
        //トーナメント終了時刻の表示
        showUntilStartDate();
    }
    else if (next_page_name != "TournamentEventConfirmEntry" &&
             next_page_name != "TournamentEventConfirmCancel") {
        
        //トーナメント終了時刻を閉じる
        removeUtilStartDate();
    }
}

void NormalScene::showUntilStartDate()
{
    //受付終了画像の表示
    auto node = this->getChildByTag(NormalScene::T_Tournament_Time_Sprite);
    
    //現在時刻の取得
    time_t now_timestamp = Utility::getNowTimeStampCheckServerTime();
    
    if (!node && now_timestamp < _tournament_next_battle_time) {
//    if (!node && 0 < _tournament_next_battle_time) {
        auto sp = TournamentTermSprite::create();
        sp->setScale(ratio);
        sp->setAnchorPoint(Point(0.5, 0.5));
        sp->setPosition(convertFromDisplaySize(Point(552 + _contentSize.width , 787), 0.5, 1));
        this->addChild(sp, NormalScene::Z_Tournament_Time_Sprite, NormalScene::T_Tournament_Time_Sprite);
        
        auto move = MoveTo::create(MOVING_TIME, convertFromDisplaySize(Point(552, 789), 0.5, 1));
        sp->runAction(Sequence::create(DelayTime::create(MOVING_TIME), move, nullptr));
    }
}

void NormalScene::removeUtilStartDate()
{
    //トーナメント終了時刻画像を閉じる
    auto node = this->getChildByTag(T_Tournament_Time_Sprite);
    if (node){
        auto move = MoveBy::create(MOVING_TIME, Point(_contentSize.width, 0));
        node->runAction(Sequence::create(move, RemoveSelf::create(true), nullptr));
    }
}

void NormalScene::modalLayerCreate(int contents)
{
    //モーダルレイヤー作成
    modalLayerCreate(contents, nullptr, nullptr, 0) ;
}

void NormalScene::modalLayerCreate(int contents, Layer *parent, SEL_SCHEDULE sel, int schedulerTime)
{
    //モーダルレイヤー削除
	ModalLayer* layer = ModalLayer::create(contents, parent, sel, schedulerTime);
	this->addChild(layer, 3000, 100);
}

void NormalScene::addTapDisableLayer()
{
    //タップ禁止レイヤー生成
    AllTapDisableLayer *allTapDisaleLayer = AllTapDisableLayer::create();
    this->addChild(allTapDisaleLayer,10000,ALL_TAP_DISABLE_LAYER_TAG);
}

void NormalScene::removeTapDisableLayer()
{
    //タップ無効化レイヤーを削除す
    this->removeChildByTag(ALL_TAP_DISABLE_LAYER_TAG) ;
}

void NormalScene::updateUserCharaDataByUserDefault()
{
    /*
     キャラ情報、デッキ情報、パッシブスキル情報、アクティブスキル情報を
     UserDefaultに保存されたjsonに基づき更新する
     
     更新に使用するUserDefaultのキーは
     user_chara_list, user_party_list, user_active_skill_list, user_passive_skill_list
     */
    
    ////////キャラクター一覧情報の読み込み////////
    string json_text = SaveData::loadString("user_chara_list");
    string err2;
    Json json2 = Json::parse(json_text, err2);
    
    int max_chara_count = fromString<int>(SaveData::loadString("max_chara_count"));
    map<int, map<string, string> > list_;
    
    for (int i=0; i< max_chara_count; i++) {
        string ind = toString(i);
        if (!json2[ind]["chara_list_id"].is_string()) {
            break;
        }
        list_[i]["chara_list_id"]    = json2[ind]["chara_list_id"].string_value();
        list_[i]["chara_id"]         = json2[ind]["chara_id"].string_value();
        list_[i]["chara_no"]         = json2[ind]["chara_no"].string_value();
        list_[i]["second_name"]      = json2[ind]["second_name"].string_value();
        list_[i]["name"]             = json2[ind]["name"].string_value();
        list_[i]["name_kana"]        = json2[ind]["name_kana"].string_value();
        list_[i]["rare"]             = json2[ind]["rare"].string_value();
        list_[i]["chara_type"]       = json2[ind]["chara_type"].string_value();
        list_[i]["left_joint"]       = json2[ind]["left_joint"].string_value();
        list_[i]["right_joint"]      = json2[ind]["right_joint"].string_value();
        list_[i]["hp"]               = json2[ind]["hp"].string_value();
        list_[i]["power"]            = json2[ind]["power"].string_value();
        list_[i]["need_cost"]        = json2[ind]["need_cost"].string_value();
        list_[i]["add_cost"]         = json2[ind]["add_cost"].string_value();
        list_[i]["turn_count"]       = json2[ind]["turn_count"].string_value();
        list_[i]["active_skill_id"]  = json2[ind]["active_skill_id"].string_value();
        list_[i]["passive_skill_id"] = json2[ind]["passive_skill_id"].string_value();
        list_[i]["chara_comment"]    = json2[ind]["chara_comment"].string_value();
        list_[i]["insert_date"]      = json2[ind]["insert_date"].string_value();
    }
    
    //キャラクター一覧を更新
    character_list = list_;
    
    ////////アクティブスキル情報の読み込み////////
    string json_text_as = SaveData::loadString("user_active_skill_list");
    string err_as;
    Json json_as = Json::parse(json_text_as, err_as);
    
    map<int, map<string, string> > list_as;
    
    for (int i=1; i<= max_chara_count; i++) {
        string index = toString(i - 1);
        if (!json_as[index]["active_skill_id"].is_string()) {
            break;
        }
        list_as[i]["active_skill_id"]  = json_as[index]["active_skill_id"].string_value();
        list_as[i]["skill_pattern_id"] = json_as[index]["skill_pattern_id"].string_value();
        list_as[i]["name"]             = json_as[index]["name"].string_value();
        list_as[i]["detail"]           = json_as[index]["detail"].string_value();
        list_as[i]["icon_detail"]      = json_as[index]["icon_detail"].string_value();
        list_as[i]["target_type"]      = json_as[index]["target_type"].string_value();
        list_as[i]["value"]            = json_as[index]["value"].string_value();
        list_as[i]["turn_count"]       = json_as[index]["turn_count"].string_value();
        list_as[i]["need_cost"]        = json_as[index]["need_cost"].string_value();
    }
    
    //アクティブスキルリストを更新
    active_skill_list = list_as;
    
    ////////////////////////////////////////
    
    ////////パッシブスキル情報の読み込み////////
    string json_text_ps = SaveData::loadString("user_passive_skill_list");
    string err_ps;
    Json json_ps = Json::parse(json_text_ps, json_text_ps);
    
    map<int, map<string, string> > list_ps;
    
    for (int i=1; i<= max_chara_count; i++) {
        string index = toString(i - 1);
        if (!json_ps[index]["passive_skill_id"].is_string()) {
            break;
        }
        list_ps[i]["passive_skill_id"]  = json_ps[index]["passive_skill_id"].string_value();
        list_ps[i]["skill_pattern_id"] = json_ps[index]["skill_pattern_id"].string_value();
        list_ps[i]["name"]             = json_ps[index]["name"].string_value();
        list_ps[i]["detail"]           = json_ps[index]["detail"].string_value();
        list_ps[i]["target_type"]      = json_ps[index]["target_type"].string_value();
        list_ps[i]["value"]            = json_ps[index]["value"].string_value();
    }
    
    //パッシブスキルリストを更新
    passive_skill_list = list_ps;
}

Layer* NormalScene::createPageLayer(string next_page_name)
{
    Layer* next_layer = nullptr;
    
    bool chara_type[4] = {true, true, true, true};
    
    //---------------------- フッター関連 -----------------
    if (next_page_name == "Mypage") next_layer = MypageLayer::create();
    else if (next_page_name == "QuestMenu") next_layer = QuestMenuLayer::create();
    else if (next_page_name == "CharaMenu") next_layer = CharaMenuLayer::create();
    else if (next_page_name == "GachaMenu") next_layer = GachaMenuLayer::create();
    else if (next_page_name == "RevisionGachaMenu") next_layer = RevisionGachaMenuLayer::create();
    else if (next_page_name == "BoxGacha") next_layer = BoxGachaLayer::create();
    else if (next_page_name == "ShopMenu")  next_layer = ShopMenuLayer::create();
    else if (next_page_name == "FriendMenu") next_layer = FriendMenuLayer::create();
    
    
    //-------------------------- 出陣 -----------------------------
    else if(next_page_name == "QuestMenu") next_layer = QuestMenuLayer::create(); //出陣メニュー
    else if(next_page_name == "NormalQuest") next_layer = NormalQuestLayer::create(); //ストーリーメニュー(大ステージ)
    else if(next_page_name == "NormalQuestStage") next_layer = NormalQuestStageLayer::create(); //ストーリー(小ステージ)
    else if(next_page_name == "EventMenu") next_layer = EventMenuLayer::create(); //イベント選択メニュー
    else if(next_page_name == "EventRankMenu") next_layer = EventRankMenuLayer::create(); //イベントの難易度選択メニュー
    else if(next_page_name == "RankingEventRankMenu") next_layer = RankingEventRankMenuLayer::create(); //イベントの難易度選択メニュー
    else if(next_page_name == "RankingEventTop") next_layer = RankingEventTopLayer::create(); //イベントトップ
    else if(next_page_name == "BossEventRankMenu") next_layer = BossEventRankMenuLayer::create(); //ボスイベントの難易度選択メニュー
    else if(next_page_name == "BossEventTop") next_layer = BossEventTopLayer::create(); //ボスイベントトップ
    else if(next_page_name == "ChallengeEventMenu") next_layer = ChallengeEventMenuLayer::create(); //試練イベント選択メニュー
    else if(next_page_name == "ChallengeEventRankMenu") next_layer = ChallengeEventRankMenuLayer::create(); //試練イベントの難易度選択メニュー
    
    
    //---------------------- キャラ --------------------
    else if(next_page_name == "CharaList"){
        bool chara_rarity[6] = {true, true, true, true, true, true}; //レア度絞り込みのパラメータ※N、R、SR、UR、UR+、PT
        bool chara_type[4] = {true, true, true, true};
        next_layer = CharaListLayer::create(chara_type, chara_rarity); //キャラ一覧メニュー
    }
    else if(next_page_name == "CharaSellList"){
        bool chara_rarity[6] = {true, true, true, true, true, true}; //レア度絞り込みのパラメータ※N、R、SR、UR、UR+、PT
        bool chara_type[4] = {true, true, true, true};
         next_layer = CharaSellListLayer::create(chara_type, chara_rarity); //キャラ売却
    }
    else if(next_page_name == "CharaEvolution") next_layer = CharaEvolutionListLayer::create(chara_type); //キャラ進化画面
    else if(next_page_name == "CharaExchange") next_layer = CharaExchangeLayer::create(); //キャラ交換画面
    else if(next_page_name == "CharaEvolutionConfirm") next_layer = CharaEvolutionConfirmLayer::create(); //キャラ進化確認画面
    else if(next_page_name == "DeckFormationTop") next_layer = DeckFormationTopLayer::create(); //デッキ編成（トップ画面）
    
    //総大将編成
    else if(next_page_name == "GeneralFormation") {
        bool chara_type[4] = {false, false, false, true}; //総大将のみ表示する
        next_layer = GeneralFormationLayer::create(chara_type);
    }
    
    //デッキ編成
    else if(next_page_name == "DeckFormation")
    {
        bool chara_rarity[6] = {true, true, true, true, true, true}; //レア度絞り込みのパラメータ※N、R、SR、UR、UR+、PT
        bool chara_type[4] = {true, true, true, false}; //総大将は表示しない
        next_layer = DeckFormationLayer::create(chara_rarity, chara_type);
    }
    
    
    //------------------- トーナメントイベント ------------------------
    else if(next_page_name == "TournamentEventTop") next_layer = TournamentEventTopLayer::create(); //トーナメントイベントトップ
    else if(next_page_name == "TournamentList") next_layer = TournamentListLayer::create(); //トーナメント一覧
    else if(next_page_name == "TournamentEntryList") next_layer = TournamentEntryListLayer::create(); //トーナメント一覧
    else if(next_page_name == "TournamentEventDeckChoice") next_layer = TournamentEventDeckChoiceLayer::create(); //トーナメントデッキ選択画面
    else if(next_page_name == "TournamentEventConfirmEntry"
             || next_page_name == "TournamentEventConfirmCancel") next_layer = TournamentEventConfirmLayer::create(next_page_name); //トーナメント参加最終確認
    else if(next_page_name == "TournamentEventBattleWait") next_layer = TournamentEventBattleWaitLayer::create(); //トーナメント対戦待機
    else if(next_page_name == "TournamentRecord") next_layer = TournamentRecordLayer::create();
    else if(next_page_name == "TournamentEventBracket") next_layer = TournamentEventBracketLayer::create();
    
    //------------------- チーム対抗戦イベント ------------------------
    else if(next_page_name == "TeamBattleEventTop") next_layer = TeamBattleEventLayer::create(); //チーム対抗戦イベントトップ
    else if(next_page_name == "TeamBattleEventTopAgain") next_layer = TeamBattleEventLayer::create(); //チーム対抗戦イベントトップ
    else if(next_page_name == "TeamBattleEventEntryList") next_layer = TeamBattleEventEntryListLayer::create(); //チーム対抗戦一覧
    else if(next_page_name == "TeamBattleEventBattleWait") next_layer = TeamBattleEventBattleWaitLayer::create(); //バトル待機画面
    else if(next_page_name == "TeamBattleEventDeckChoice") next_layer = TeamBattleEventDeckSelectLayer::create(); //デッキ選択画面
    else if(next_page_name == "TeamBattleEventItemExchange") next_layer = TeamBattleEventItemExchangeLayer::create(); //チーム対抗戦イベントアイテム交換所
    
    //----------------- オンライン対戦 ------------------------------
    else if(next_page_name == "BattleTop") next_layer = BattleTopLayer::create(); //オンラインのトップページ（クイック対戦、ルーム対戦）
    else if(next_page_name == "OfflinePage") next_layer = OfflineEnemyListLayer::create(); //オフラインのトップ（対戦相手選択ページ）
    else if(next_page_name == "BattleRoomTop") next_layer = BattleRoomTopLayer::create(); //バトルルームのトップ画面（ルーム作成、ルーム参加）
    else if(next_page_name == "CreateRoom") next_layer = CreateRoomLayer::create(); //ルーム作成画面
    else if(next_page_name == "SearchRoom") next_layer = SearchRoomLayer::create(); //ルーム参加画面
    else if(next_page_name == "BattleFree") next_layer = BattleFreeLayer::create(); //クイック対戦の待機ページ
    else if(next_page_name == "ReplayList") next_layer = ReplayListLayer::create(); //リプレイTOPページ
    
    else if(next_page_name == "BattleFriendChoiceForQuest"
            || next_page_name == "BattleFriendChoiceForOffline"
            || next_page_name == "BattleFriendChoiceForEvent"
            || next_page_name == "BattleFriendChoiceForOnline"
            || next_page_name == "BattleFriendChoiceForRankingEvent"
            || next_page_name == "BattleFriendChoiceForBossEvent"
            || next_page_name == "BattleFriendChoiceForChallengeEvent"
            ) next_layer = BattleFriendChoiceLayer::create(next_page_name); //助っ人選択ページ
    
    else if(next_page_name == "BattleDeckChoiceForQuest"
            || next_page_name == "BattleDeckChoiceForOffline"
            || next_page_name == "BattleDeckChoiceForEvent"
            || next_page_name == "BattleDeckChoiceForOnline"
            || next_page_name == "BattleDeckChoiceForRankingEvent"
            || next_page_name == "BattleDeckChoiceForBossEvent"
            || next_page_name == "BattleDeckChoiceForChallengeEvent"
            ) next_layer = BattleDeckChoiceLayer::create(next_page_name); //最終確認ページ
    
    
    //------------------------- ガチャ ----------------------------
    else if(next_page_name == "GachaMenu") next_layer = GachaMenuLayer::create(); //ガチャメニュー
    else if (next_page_name == "RevisionGachaMenu") next_layer = RevisionGachaMenuLayer::create();
    
    
    //----------------------- ショップ ----------------------------
    else if(next_page_name == "ShopMenu") next_layer = ShopMenuLayer::create(); //ショップメイン画面
    else if(next_page_name == "StoneShopMenu") next_layer = StoneShopMenuLayer::create(); //石(課金)購入画面
    
    
    //----------------------- フレンド ------------------------------
    else if(next_page_name == "FriendMenu") next_layer = FriendMenuLayer::create(); //フレンドメニュー
    else if(next_page_name == "FollowList"
            || next_page_name == "FollowerList"
            ) next_layer = FollowListLayer::create(next_page_name); //フォローリスト、フォロワーリスト
    else if(next_page_name == "FriendSearch") next_layer = FriendSearchLayer::create(); //フレンド検索
    else if(next_page_name == "UserDetailFromFollowList" ||
            next_page_name == "UserDetailFromFollowerList" ||
            next_page_name == "UserDetailFromFriendSearch") next_layer = UserDetailLayer::create(next_page_name); //ユーザ情報

    //チーム
    else if(next_page_name=="TeamTop") next_layer = TeamTopLayer::create();
    else if(next_page_name == "TeamCreateTeam") next_layer = TeamCreateTeamLayer::create();
    else if(next_page_name == "TeamNoBelong") next_layer = TeamNoBelongLayer::create();
    else if(next_page_name == "TeamSearchTeam") next_layer = TeamSearchListLayer::create();
    else if(next_page_name == "TeamApplication") next_layer = TeamApplicationListLayer::create();
    else if(next_page_name == "TeamInvited") next_layer = TeamInvitedListLayer::create();
    else if(next_page_name == "TeamList") next_layer = TeamListLayer::create();
    else if(next_page_name == "TeamManagement") next_layer = TeamManagementBaseLayer::create();
    else if(next_page_name == "TeamMember") next_layer = TeamMemberListLayer::create();
    else if(next_page_name == "TeamMemberDetail") next_layer = TeamMemberDetailLayer::create("");
    else if(next_page_name == "TeamInformationChange") next_layer = TeamInformationChangeLayer::create();
    else if(next_page_name == "TeamInvitation") next_layer = TeamInvitationLayer::create();
    
    //------------------------- ショップ --------------------------
    else if(next_page_name == "StoneShop") StoneShopAgeConfirmLayer::create(); //宝玉レイヤー
    
    //------------------------- バザー --------------------------
    else if(next_page_name == "BazaarTop") next_layer = BazaarTopLayer::create(); //バザーレイヤー
    else if(next_page_name == "BazaarSearchLayer") next_layer = BazaarSearchListLayer::create(); //バザーレイヤー
    else if(next_page_name == "BazaarExhibitTop") next_layer = BazaarExhibitTopLayer::create(); //出品レイヤー
    else if(next_page_name == "BazaarExhibitCharaList"){
        bool chara_rarity[6] = {true, true, true, true, true, true}; //レア度絞り込みのパラメータ※N、R、SR、UR、UR+、PT
        bool chara_type[4] = {true, true, true, true};
        next_layer = BazaarExhibitCharaListLayer::create(chara_type, chara_rarity); //出品キャラ選択レイヤー
    }
    else if(next_page_name == "BazaarExhibitConfirm") next_layer = BazaarExhibitConfirmLayer::create(); //出品確認レイヤー
    
    //--------------------------- その他 ---------------------------
    else if(next_page_name == "SubMenu") next_layer = SubMenuLayer::create(); //サブメニュー
    else if(next_page_name == "Other") next_layer = OtherLayer::create(); //その他
    else if(next_page_name == "PresentMenu") next_layer = PresentMenuLayer::create(); //プレゼントメニュー
    else if(next_page_name == "Options") next_layer = OptionsLayer::create(); //オプション
    else if(next_page_name == "Modelchangecode") next_layer = ModelchangecodeLayer::create(); //機種変更コード発行
    else if(next_page_name == "Modelchange") next_layer = ModelchangeLayer::create(); //機種変更コード入力
    else if(next_page_name == "Question")next_layer = QuestionLayer::create(); //お問い合わせ
    else if(next_page_name == "Help") next_layer = HelpLayer::create(); //ヘルプ
    else if(next_page_name == "Sound") next_layer = SoundLayer::create(); //音量調整
    else if(next_page_name == "Nickname") next_layer = NicknameLayer::create(); //ニックネーム変更
    else if(next_page_name == "Profile") next_layer = ProfileLayer::create(); //プロフィール
    else if(next_page_name == "Comment") next_layer = CommentLayer::create(); //コメント編集
    
    return next_layer;
}
