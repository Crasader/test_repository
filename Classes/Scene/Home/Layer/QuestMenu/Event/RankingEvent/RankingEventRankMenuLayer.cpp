#include "RankingEventRankMenuLayer.h"

#define SE_SORT_TAP "download/SE/se_sort_tap.mp3"

#define SWITCH_BUTTON_TAG 11

RankingEventRankMenuLayer::~RankingEventRankMenuLayer()
{
    //NG音出し中にページ遷移すると音が小さいままなので、デストラクタで戻す
    float bgm_volume    = SaveData::loadFloat("bgm_volume");
    float effect_volume = SaveData::loadFloat("effect_volume");
    
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
}

bool RankingEventRankMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("RankingEventRankMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void RankingEventRankMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントランクデータの設定
    setEventRankData();
    
    //最終確認画面の敵スプライトのデータ初期化
    initEventEnemyData();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("難易度選択", "RankingEventTop", this, HeaderLayer::TitleBarType::WIDTH_390);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //イベント詳細ボタンの作成
//    createDetailButton();
    
    //イベント説明画像の作成
    createEventExplainSprite();
    
    //パワーの表示
    createEventPower();
    
//    //表示切り替えボタンの作成
//    createSwitchButton();
    
    //星の数の作成
    createSumRewardSprite();
    
    //スクロールビューの作成
    createScrollView();
    
    //セルの表示を替えるアップデート関数
    updateCellInfo(0) ; //1回目はすぐに使用
    schedule(schedule_selector(RankingEventRankMenuLayer::updateCellInfo), 1.0f);
}

void RankingEventRankMenuLayer::setEventRankData()
{
    string json_text = SaveData::loadString("ranking_event_stage_data");
    string err2;
    _ranking_event_stage_data = Json::parse(json_text, err2);
    
    //イベントのランクの数を取得
    _event_stage_count = countJsonData(_ranking_event_stage_data);
    
}

void RankingEventRankMenuLayer::initEventEnemyData()
{
    //最終確認画面でのデータを初期化
    SaveData::saveString("ranking_event_stage_id", "");
    SaveData::saveString("floor_count", "");
    SaveData::saveString("event_gold", "0");
    SaveData::saveString("event_rank_point", "0");
    SaveData::saveString("event_rank_event_stage_level", "0");
    SaveData::saveString("event_reward_point", "0");
    SaveData::saveString("ranking_event_enemy_sprite_data", "");
    
}

void RankingEventRankMenuLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(RankingEventRankMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(RankingEventRankMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(RankingEventRankMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(RankingEventRankMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void RankingEventRankMenuLayer::createDetailButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/bt_event_detail.png");
    auto detail_on  = Sprite::create("init/Home/Quest/bt_event_detail.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string ranking_event_id = SaveData::loadString("ranking_event_id");
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__event_detail_" + ranking_event_id + "&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント詳細");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, ZORDER_BUTTON, T_Detail_Menu);
}

void RankingEventRankMenuLayer::createEventPower()
{
    _eventPowerSprite = EventPowerSprite::create() ;
    _eventPowerSprite->setScale(ratio) ;
    _eventPowerSprite->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH, DISPLAY_HEIGHT-HEADER_SIZE+10), 0.5, 1)) ;
    _eventPowerSprite->setAnchorPoint(Point(1, 1)) ;
    this->addChild(_eventPowerSprite, O_eventPowerSprite_weak, T_eventPowerSprite) ;
}

void RankingEventRankMenuLayer::createEventExplainSprite()
{
    //イベント説明画像データの取得
    string json_text = SaveData::loadString("ranking_event_explain_data");
    string err2;
    Json ranking_event_explain_data = Json::parse(json_text, err2);
    string comment = ranking_event_explain_data["comment"].string_value();
    
    //イベント説明画像の作成
    string ranking_event_id = SaveData::loadString("ranking_event_id");
    string image_path = "download/EventMenu/RankingEvent/event" + ranking_event_id + "/event_explain_" + ranking_event_id + ".png";
    _explain_sp = RankingEventExplainSprite::create(comment, image_path);
    _explain_sp->setScale(ratio);
//    _explain_sp->setPosition(convertFromDisplaySize(Point(320, 638), 0.5, 0.5)); // 80 = ヘッダー下部の戻るボタンの高さ
    _explain_sp->setPosition(convertFromDisplaySize(Point(320, 640), 0.5, 1));
    
    addChild(_explain_sp, O_RankingEventExplainSprite);
}


void RankingEventRankMenuLayer::createSumRewardSprite()
{
    //合計報酬（星）の数の表示
    auto sum_bg = Sprite::create("init/Home/Quest/star_bk.png");
    sum_bg->setScale(ratio);
    sum_bg->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(sum_bg, 1, T_Sum_Bg);
    sum_bg->setVisible(false); //始め非表示
    
    //星画像
    auto star_sp = Sprite::create("init/Home/Quest/star_l.png");
    star_sp->setPosition(Point(40, sum_bg->getContentSize().height / 2));
    sum_bg->addChild(star_sp, 1);
    
    //合計数
    string sum_reward_flg = toString<int>(SaveData::loadInt("sum_reward_flg"));
    auto count_label = HMSFontUtil::createWithTTFBySystemFontArgument(sum_reward_flg, FONT_NAME_SYSTEM , 36);
    count_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    count_label->setAnchorPoint(Point(1.0, 0.5));
    count_label->setPosition(Point(142, sum_bg->getContentSize().height / 2));
    sum_bg->addChild(count_label, 2);
}

void RankingEventRankMenuLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio) - (_explain_sp->getContentSize().height * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, 1, SCROLLVIEW_TAG);
    
    //PresentMenuのレイヤーを戻り値に指定して使う
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, cell_size * _event_rank_sp.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * _event_rank_sp.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio) - (_explain_sp->getContentSize().height * ratio)))));
}

Layer* RankingEventRankMenuLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 150 * ratio;
    auto layer = Layer::create();

    int num = 0;
    for (int i = 0 ; i<_event_stage_count ; i++)
    {
        string numStr = toString<int>(num);
        
        _event_rank_sp.push_back(RankingEventListSprite::create(_ranking_event_stage_data[numStr], name_or_reward));
        _event_rank_sp[num]->setScale(ratio);
        _event_rank_sp[num]->setTag(fromString<int>(_ranking_event_stage_data[numStr]["ranking_event_stage_id"].string_value()));
        _event_rank_sp[num]->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2) - 10 * ratio));
        layer->addChild(_event_rank_sp[num], 1);
        
        //子ノードにも色合いを適用させる処理
        _setCascadeColorEnabled(_event_rank_sp[num], true);
        
        num++;
    }
    
    return layer;
}

bool RankingEventRankMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タップ位置がeventPowerSpriteの範囲内であればZOrderを手前に変更する
    auto eventPowerSprite = (EventPowerSprite *)this->getChildByTag(T_eventPowerSprite);
    if(eventPowerSprite && Utility::containsPoint(eventPowerSprite, touch->getLocation())){
        eventPowerSprite->setLocalZOrder(O_eventPowerSprite_strong) ;
    }
    
    
    if (_event_rank_sp.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void RankingEventRankMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void RankingEventRankMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タップが終了したときeventPowerSpriteのZOrderを奥に変更する
    auto eventPowerSprite = (EventPowerSprite *)this->getChildByTag(T_eventPowerSprite);
    if(eventPowerSprite){
        eventPowerSprite->setLocalZOrder(O_eventPowerSprite_weak) ;
    }
    
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (RankingEventListSprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                int max_power = fromString<int>(SaveData::loadString("max_event_power"));

                if (max_power < fromString<int>(sp->getNeedPower()))
                {
                    //NG音の再生
                    soundNG();
                }
                else {
                    //ページ遷移(現在の体力が足りていない場合、体力回復のポップアップ表示)
                    changePage(sp->getTag());
                }
            }
            tag++;
        }
    }
}
void RankingEventRankMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void RankingEventRankMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    touchButtonFlg = false;
}

RankingEventRankMenuLayer::UserCondition RankingEventRankMenuLayer::checkUserCondition(int need_power, bool open_flg)
{
    int now_power = fromString<int>(SaveData::loadString("event_power"));
    int max_power = fromString<int>(SaveData::loadString("max_event_power"));
    
    if (!open_flg) {
        return UserCondition::STAGE_CLOSE;
    }
    else if (max_power < need_power) {
        return UserCondition::LESS_MAX_POWER;
    }
    else
    {
        if (need_power <= now_power) {
            return UserCondition::GO;
        }
        else {
            return UserCondition::LESS_POWER;
        }
    }
}

void RankingEventRankMenuLayer::changePage(int tag)
{
    auto event_sp = (RankingEventListSprite *)this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(tag);
    
    //体力チェック
    auto normal_scene = (NormalScene *)this->getParent();
    
    switch (checkUserCondition(fromString<int>(event_sp->getNeedPower()), event_sp->getOpenFlg())) {
        case GO:
        {
            //選択したイベントランクリストIDの保存
            saveUseDungeonData(event_sp->getRankingEventStageId(), event_sp->getFloorCount());
            
            //最終確認画面の敵スプライト画像の設定
            saveBattleUserSpriteData(event_sp);
            
            //選択したイベントデータの設定
            saveEventPointData(event_sp);
            
            //フレンド選択画面へ遷移
            normal_scene->changePage("BattleFriendChoiceForRankingEvent");
            break;
        }
        case STAGE_CLOSE:
        {
            string comment = "一つ下の階級をクリアして開放しよう!";
            auto popup_layer = popupNoChangePageLayer::create(comment);
            addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
            break;
        }
        case LESS_MAX_POWER:
        {
            string comment = "出陣に必要な行動力が\n足りません。";
            auto popup_layer = popupNoChangePageLayer::create(comment);
            addChild(popup_layer, ZORDER_POPUP_LAYER, POPUP_LAYER_TAG);
            break;
        }
        default:
        {
            
            HMSLOG("※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※");
            HMSLOG("回復機能は1.6以上でテストしてね");
            HMSLOG("※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※");
            //体力足りないポップアップ
            SEL_SCHEDULE s = schedule_selector(RankingEventRankMenuLayer::recoveryPower);
            normal_scene->modalLayerCreate(MODAL_CONTENTS_EVENT_POWER,this,s,1);
            break;
        }
    }
}

void RankingEventRankMenuLayer::recoveryPower(float dt)
{
    //行動力を全回復しました」という表示の後にコールバックが呼ばれる
    _eventPowerSprite->loadEventPower();
    _eventPowerSprite->recoveryAction();
}

void RankingEventRankMenuLayer::saveUseDungeonData(string ranking_event_stage_id, string floor_count)
{
    //ダンジョンシーンで使用するデータを保存
    SaveData::saveString("ranking_event_stage_id", ranking_event_stage_id);
    SaveData::saveString("floor_count", floor_count);
    HMSLOG("ranking_event_stage_id = %s", ranking_event_stage_id.c_str());
    HMSLOG("floor_count = %s", floor_count.c_str());
    
}

void RankingEventRankMenuLayer::saveBattleUserSpriteData(RankingEventListSprite* event_sp)
{
    //最終確認画面の対戦相手画像データの設定
    string user_master_id = "" ;
    string friend_id = "";
    string card_image_id = event_sp->getCardImageId();
    string user_name = event_sp->getUserName();
    string level = "0" ;
    string last_play_date = "";
    string comment = "";
    string rank_name = event_sp->getEventRankName();
    string offense = "??" ;
    string defense = "??" ;
    string strategist = "??" ;
    string general = "100" ;
    string battle_point_rate = "0" ;
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"user_master_id\":\"" + user_master_id + "\"," + cr;
    data += "  \"friend_id\":\"" + friend_id + "\"," + cr;
    data += "  \"card_image_id\":\"" + card_image_id+ "\"," + cr;
    data += "  \"user_name\":\"" + user_name + "\"," + cr;
    data += "  \"level\":\"" + level + "\"," + cr;
    data += "  \"last_play_date\":\"" + last_play_date + "\"," + cr;
    data += "  \"comment\":\"" + comment + "\"," + cr;
    data += "  \"rank_name\":\"" + rank_name + "\"," + cr;
    data += "  \"offense\":\"" + offense + "\"," + cr;
    data += "  \"defense\":\"" + defense + "\"," + cr;
    data += "  \"strategist\":\"" + strategist + "\"," + cr;
    data += "  \"general\":\"" + general + "\"," + cr;
    data += "  \"battle_point_rate\":\"" + battle_point_rate + "\"" + cr;
    data += "}";
    HMSLOG("%s", data.c_str()) ;
    
    SaveData::saveString("ranking_event_enemy_sprite_data", data);
}

void RankingEventRankMenuLayer::saveEventPointData(RankingEventListSprite *event_sp)
{
    //選択したイベントのデータを設定
    SaveData::saveString("event_gold", event_sp->getGold());
    SaveData::saveString("event_reward_point", event_sp->getRewardPoint());
    SaveData::saveString("event_rank_point", event_sp->getRankPoint());
    SaveData::saveString("event_rank_event_stage_level", event_sp->getEventRankLevel());
    
    HMSLOG("event_gold = %s, event_reward_point = %s, event_rank_point = %s", event_sp->getGold().c_str(), event_sp->getRewardPoint().c_str(), event_sp->getRankPoint().c_str());
    
}

void RankingEventRankMenuLayer::updateCellInfo(float dt)
{
    int now_power = fromString<int>(SaveData::loadString("event_power"));
    
    for (int i = 0; i < _event_rank_sp.size(); i++) {
        
        if(_event_rank_sp[i]->getOpenFlg() == false){
            //開放していない場合、鍵の表示と共にグレーアウト
            _event_rank_sp[i]->setColor(Color3B(140, 140, 140));
            _event_rank_sp[i]->changeVisibleSprite(false);
            _event_rank_sp[i]->showKey() ;
        }
        else if (fromString<int>(_event_rank_sp[i]->getNeedPower()) <= now_power)
        {
            //現在の体力が必要行動力以上になった場合、通常表示
            _event_rank_sp[i]->setColor(Color3B(255, 255, 255));
            _event_rank_sp[i]->changeVisibleSprite(true);
            
        }
        else
        {
            //必要行動力に足りていない場合、グレー表示
            _event_rank_sp[i]->setColor(Color3B(140, 140, 140));
            _event_rank_sp[i]->changeVisibleSprite(false);
        }
    }
}

void RankingEventRankMenuLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void RankingEventRankMenuLayer::_setCascadeColorEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeColorEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeColorEnabled(parent->getChildren().at(i), enabled);
    }
}

void RankingEventRankMenuLayer::soundNG()
{
    //ボタンをタッチさせなくする
    listener->setEnabled(false);
    
    //最大行動力が足りていない場合ボタン押せない
    float back_volumne = SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
    float se_volumne = SimpleAudioEngine::getInstance()->getEffectsVolume();
    
    //音量を変える
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.2f);
    SimpleAudioEngine::getInstance()->setEffectsVolume(1.0f);
    
    //鳴らす
    SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3");
    
    auto func = CallFunc::create([this, back_volumne, se_volumne](){
        //戻す
        SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(back_volumne);
        SimpleAudioEngine::getInstance()->setEffectsVolume(se_volumne);
        
        //解放
        SimpleAudioEngine::getInstance()->unloadEffect("download/SE/ng.mp3");
        
        listener->setEnabled(true);
    });
    
    runAction(Sequence::create(DelayTime::create(0.8f), func, nullptr));
}