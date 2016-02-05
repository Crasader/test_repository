#include "BossEventRankMenuLayer.h"

#define SE_SORT_TAP "download/SE/se_sort_tap.mp3"

#define SWITCH_BUTTON_TAG 11

BossEventRankMenuLayer::~BossEventRankMenuLayer()
{
    //NG音出し中にページ遷移すると音が小さいままなので、デストラクタで戻す
    float bgm_volume    = SaveData::loadFloat("bgm_volume");
    float effect_volume = SaveData::loadFloat("effect_volume");
    
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
}

bool BossEventRankMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BossEventRankMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void BossEventRankMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントランクデータの設定
    setEventRankData();
    
    //最終確認画面の敵スプライトのデータ初期化
    initEventEnemyData();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("討伐対象選択", "BossEventTop", this, HeaderLayer::TitleBarType::WIDTH_390);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //イベント詳細ボタンの作成
//    createDetailButton();
    
    //イベント説明画像の作成
    createEventExplainSprite();
    
    //スクロールビューの作成
    createScrollView();
    
}

void BossEventRankMenuLayer::setEventRankData()
{
    string json_text = SaveData::loadString("boss_event_stage_data");
    string err2;
    _boss_event_stage_data = Json::parse(json_text, err2);
    
    //イベントのランクの数を取得
    _event_stage_count = countJsonData(_boss_event_stage_data);
    
}

void BossEventRankMenuLayer::initEventEnemyData()
{
    //最終確認画面でのデータを初期化
    SaveData::saveString("boss_event_stage_log_id", "");
    SaveData::saveString("floor_count", "");
    SaveData::saveString("event_gold", "0");
    SaveData::saveString("event_rank_point", "0");
    SaveData::saveString("event_rank_event_stage_level", "0");
    SaveData::saveString("event_reward_point", "0");
    SaveData::saveString("boss_event_enemy_sprite_data", "");
    
}

void BossEventRankMenuLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BossEventRankMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BossEventRankMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BossEventRankMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BossEventRankMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void BossEventRankMenuLayer::createDetailButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/bt_event_detail.png");
    auto detail_on  = Sprite::create("init/Home/Quest/bt_event_detail.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string boss_event_id = SaveData::loadString("boss_event_id");
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__event_detail_" + boss_event_id + "&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント詳細");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, ZORDER_BUTTON, T_Detail_Menu);
}

void BossEventRankMenuLayer::createEventExplainSprite()
{
    //イベント説明画像データの取得
    string json_text = SaveData::loadString("boss_event_explain_data");
    string err2;
    Json boss_event_explain_data = Json::parse(json_text, err2);
    string comment = boss_event_explain_data["comment"].string_value();
}

void BossEventRankMenuLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
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
    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * _event_rank_sp.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* BossEventRankMenuLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 150 * ratio;
    auto layer = Layer::create();

    int num = 0;
    for (int i = 0 ; i<_event_stage_count ; i++)
    {
        string numStr = toString<int>(num);
        
        _event_rank_sp.push_back(BossEventListSprite::create(_boss_event_stage_data[numStr], name_or_reward));
        _event_rank_sp[num]->setScale(ratio);
        _event_rank_sp[num]->setTag(fromString<int>(_boss_event_stage_data[numStr]["boss_event_stage_log_id"].string_value()));
        _event_rank_sp[num]->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2) - 10 * ratio));
        layer->addChild(_event_rank_sp[num], 1);
        
        //子ノードにも色合いを適用させる処理
        _setCascadeColorEnabled(_event_rank_sp[num], true);
        
        num++;
    }
    
    return layer;
}

bool BossEventRankMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_event_rank_sp.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void BossEventRankMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void BossEventRankMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        //表示切り替えボタン判定
//        Layer *layer = (Layer *)this->getChildByTag(SWITCH_BUTTON_TAG);
//        Sprite *sprite = (Sprite *)layer->getChildByTag(1);
//        Rect rect = sprite->getBoundingBox();
//        if(rect.containsPoint(touch->getLocation())) {
//            CCLOG("switch");
//            
//            //音を鳴らす
//            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect(SE_SORT_TAP,false);
//            
//            auto menuItem = (MenuItem *)this->getChildByTag(T_Detail_Menu)->getChildByTag(T_Detail_MenuItem);
//            auto sum_bg = (Sprite *)this->getChildByTag(T_Sum_Bg);
//            
//            if(name_or_reward=="name") {
//                name_or_reward = "reward";
//                menuItem->setVisible(false);
//                menuItem->setEnabled(false);
//                sum_bg->setVisible(true);
//            }
//            else {
//                name_or_reward = "name";
//                menuItem->setVisible(true);
//                menuItem->setEnabled(true);
//                sum_bg->setVisible(false);
//            }
//            removeChildByTag(SCROLLVIEW_TAG);
//            _event_rank_sp.clear();
//            createEventExplainSprite();
//            createScrollView();
//            
//            return;
//        }
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (BossEventListSprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                //ページ遷移
                changePage(sp->getTag());
            }
            tag++;
        }
    }
}
void BossEventRankMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void BossEventRankMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    touchButtonFlg = false;
}

void BossEventRankMenuLayer::changePage(int tag)
{
    auto event_sp = (BossEventListSprite *)this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(tag);
    
    //体力チェック
    auto normal_scene = (NormalScene *)this->getParent();

    //選択したイベントステージIDの保存
    saveUseDungeonData(event_sp->getBossEventStageLogId());

    //最終確認画面の敵スプライト画像の設定
    saveBattleUserSpriteData(event_sp);

    //デッキ選択画面へ遷移
    normal_scene->changePage("BattleDeckChoiceForBossEvent");
}

void BossEventRankMenuLayer::saveUseDungeonData(string boss_event_stage_id)
{
    //ダンジョンシーンで使用するデータを保存
    SaveData::saveString("boss_event_stage_log_id", boss_event_stage_id);
    HMSLOG("boss_event_stage_id = %s", boss_event_stage_id.c_str());
    
}

void BossEventRankMenuLayer::saveBattleUserSpriteData(BossEventListSprite* event_sp)
{
    //最終確認画面の対戦相手画像データの設定
    string user_master_id = "" ;
    string friend_id = "";
    string card_image_id = event_sp->getCardImageId();
    string user_name = event_sp->getUserName();
    string level = "0" ;
    string last_play_date = "";
    string comment = "";
    string rank_name = event_sp->getBossName();
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
    
    SaveData::saveString("boss_event_enemy_sprite_data", data);
}

void BossEventRankMenuLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void BossEventRankMenuLayer::_setCascadeColorEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeColorEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeColorEnabled(parent->getChildren().at(i), enabled);
    }
}

void BossEventRankMenuLayer::soundNG()
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
