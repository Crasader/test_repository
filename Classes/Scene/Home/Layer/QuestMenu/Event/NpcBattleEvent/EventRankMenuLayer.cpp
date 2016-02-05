#include "EventRankMenuLayer.h"

#define SE_SORT_TAP "download/SE/se_sort_tap.mp3"

#define SWITCH_BUTTON_TAG 11

EventRankMenuLayer::~EventRankMenuLayer()
{
    //NG音出し中にページ遷移すると音が小さいままなので、デストラクタで戻す
    float bgm_volume    = SaveData::loadFloat("bgm_volume");
    float effect_volume = SaveData::loadFloat("effect_volume");
    
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
}

bool EventRankMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("EventRankMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void EventRankMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントランクデータの設定
    setEventRankData();
    
    //最終確認画面の敵スプライトのデータ初期化
    initEventEnemyData();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("難易度選択", "EventMenu", this);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //イベント詳細ボタンの作成
    createDetailButton();
    
    //イベント説明画像の作成
    createEventExplainSprite();
    
    //表示切り替えボタンの作成
    createSwitchButton();
    
    //星の数の作成
    createSumRewardSprite();
    
    //星を五つ集めよう！という表示
    createStarHighlight() ;
    
    //スクロールビューの作成
    createScrollView();
    
    //体力チェックアップデート関数
    scheduleUpdate();
}

void EventRankMenuLayer::setEventRankData()
{
    string json_text = SaveData::loadString("event_rank_data");
    string err2;
    _event_rank_data = Json::parse(json_text, err2);
    
    //イベントのランクの数を取得
    _event_rank_count = countJsonData(_event_rank_data);
    
}

void EventRankMenuLayer::initEventEnemyData()
{
    //最終確認画面でのデータを初期化
    SaveData::saveString("npc_battle_event_rank_list_id", "");
    SaveData::saveString("event_gold", "0");
    SaveData::saveString("event_rank_point", "0");
    SaveData::saveString("event_rank_level", "0");
    SaveData::saveString("event_reward_point", "0");
    SaveData::saveString("event_enemy_sprite_data", "");
}

void EventRankMenuLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(EventRankMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(EventRankMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(EventRankMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(EventRankMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void EventRankMenuLayer::createDetailButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/bt_event_detail.png");
    auto detail_on  = Sprite::create("init/Home/Quest/bt_event_detail.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string event_id = SaveData::loadString("event_id");
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__event_detail_" + event_id + "&event_id=" + event_id + "&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント詳細");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    detail_btn->setTag(T_Detail_MenuItem);
    
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, ZORDER_BUTTON, T_Detail_Menu);
}

void EventRankMenuLayer::createEventExplainSprite()
{
    //イベント説明画像データの取得
    string json_text = SaveData::loadString("event_explain_data");
    string err2;
    Json event_explain_data = Json::parse(json_text, err2);
    
    //イベント説明画像の作成
    string event_id = SaveData::loadString("event_id");
    string image_path = "download/EventMenu/NpcBattleEvent/event" + event_id + "/event_explain_" + event_id + ".png";
    _explain_sp = EventExplainSprite::create(event_explain_data, image_path);
    _explain_sp->setScale(ratio);
    _explain_sp->setPosition(convertFromDisplaySize(Point(320, 640), 0.5, 1));
    addChild(_explain_sp, 1);
}

void EventRankMenuLayer::createSwitchButton()
{
    auto title_bar_layer = this->getParent()->getChildByTag(TITLE_LAYER_TAG);
    
    Sprite *img = Sprite::create("init/Home/Quest/bt_switch.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(390, 788), 0.5, 1));
    title_bar_layer->addChild(img, TitleBarLayer::T_Button, SWITCH_BUTTON_TAG);
}

void EventRankMenuLayer::createSumRewardSprite()
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
void EventRankMenuLayer::createStarHighlight()
{
    HeaderLayer *header = (HeaderLayer *)((NormalScene *)this->getParent())->getChildByTag(HEADER_LAYER_TAG);
    
    if(header){
        //星を５個集めよう! ポップアップ
        auto star_highlight = Sprite::create("init/Home/Quest/event_term.png");
        star_highlight->setPosition(convertFromDisplaySize(Point(390, 870), 0.5, 1));
        star_highlight->setScale(0);
        header->addChild(star_highlight, 100) ;

        //ラベル1
        auto label_free1 = HMSFontUtil::createWithTTFBySystemFontArgument("星を5個", FONT_NAME_SYSTEM, 24);
        label_free1->setHorizontalAlignment(TextHAlignment::CENTER);
        label_free1->setPosition(Point(star_highlight->getContentSize().width / 2 - 5, star_highlight->getContentSize().height / 2 + 20));
        label_free1->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        star_highlight->addChild(label_free1, 3, ACTION_SPRITE_TAG);

        //ラベル2
        auto label_free2 = HMSFontUtil::createWithTTFBySystemFontArgument("集めよう!", FONT_NAME_SYSTEM, 24);
        label_free2->setHorizontalAlignment(TextHAlignment::CENTER);
        label_free2->setPosition(Point(star_highlight->getContentSize().width / 2 , star_highlight->getContentSize().height / 2 - 20));
        label_free2->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        star_highlight->addChild(label_free2, 3, ACTION_SPRITE_TAG);

        //表示して削除するアクション
        ScaleTo *scaleTo_big = ScaleTo::create(2.0f, ratio) ;
        EaseElasticOut *scale_ease_out = EaseElasticOut::create(scaleTo_big);
        
        ScaleTo *scaleTo_small = ScaleTo::create(0.7f, 0) ;
        EaseBackIn *scale_back_in = EaseBackIn::create(scaleTo_small);
        
        Sequence *popup_seq = Sequence::create(DelayTime::create(1.0f), scale_ease_out, scale_back_in, RemoveSelf::create(), nullptr) ;
        star_highlight->runAction(popup_seq) ;
    }
}


void EventRankMenuLayer::createScrollView()
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

Layer* EventRankMenuLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 150 * ratio;
    auto layer = Layer::create();

    int num = 0;
    for (int i = (_event_rank_count - 1); 0 <= i; i--)
    {
        string numStr = toString<int>(num);
        
        _event_rank_sp.push_back(EventListSprite::create(_event_rank_data[numStr], name_or_reward));
        _event_rank_sp[num]->setScale(ratio);
        _event_rank_sp[num]->setTag(fromString<int>(_event_rank_data[numStr]["npc_battle_event_rank_list_id"].string_value()));
        _event_rank_sp[num]->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2) - 10 * ratio));
        layer->addChild(_event_rank_sp[num], 1);
        
        //子ノードにも色合いを適用させる処理
        _setCascadeColorEnabled(_event_rank_sp[num], true);
        
        num++;
    }
    
    return layer;
}

bool EventRankMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_event_rank_sp.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void EventRankMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void EventRankMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        //表示切り替えボタン判定
        auto sprite = this->getParent()->getChildByTag(TITLE_LAYER_TAG)->getChildByTag(SWITCH_BUTTON_TAG);
        Rect rect = sprite->getBoundingBox();
        if(rect.containsPoint(touch->getLocation())) {
            CCLOG("switch");
            
            //音を鳴らす
            CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect(SE_SORT_TAP,false);
            
            auto menuItem = (MenuItem *)this->getChildByTag(T_Detail_Menu)->getChildByTag(T_Detail_MenuItem);
            auto sum_bg = (Sprite *)this->getChildByTag(T_Sum_Bg);
            
            if(name_or_reward=="name") {
                name_or_reward = "reward";
                menuItem->setVisible(false);
                menuItem->setEnabled(false);
                sum_bg->setVisible(true);
            }
            else {
                name_or_reward = "name";
                menuItem->setVisible(true);
                menuItem->setEnabled(true);
                sum_bg->setVisible(false);
            }
            removeChildByTag(SCROLLVIEW_TAG);
            _event_rank_sp.clear();
            createEventExplainSprite();
            createScrollView();
            
            return;
        }
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (EventListSprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                int max_power = fromString<int>(SaveData::loadString("max_power"));

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
void EventRankMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void EventRankMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    touchButtonFlg = false;
}

EventRankMenuLayer::Power EventRankMenuLayer::checkPower(int need_power)
{
    int now_power = fromString<int>(SaveData::loadString("power"));
    int max_power = fromString<int>(SaveData::loadString("max_power"));
    
    if (max_power < need_power) {
        return Power::LESS_MAX_POWER;
    }
    else {
        if (need_power <= now_power) {
            return Power::GO;
        }
        else {
            return Power::LESS_POWER;
        }
    }
}

void EventRankMenuLayer::changePage(int tag)
{
    auto event_sp = (EventListSprite *)this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(tag);
    
    //体力チェック
    auto normal_scene = (NormalScene *)this->getParent();
    
    switch (checkPower(fromString<int>(event_sp->getNeedPower()))) {
        case GO:
        {
            //選択したイベントランクリストIDの保存
            saveUseDungeonData(event_sp->getNpcBattleEventRankListId());
            
            //最終確認画面の敵スプライト画像の設定
            saveBattleUserSpriteData(event_sp);
            
            //選択したイベントデータの設定
            saveEventPointData(event_sp);
            
            //フレンド選択画面へ遷移
            normal_scene->changePage("BattleFriendChoiceForEvent");
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
            //体力足りないポップアップ
            normal_scene->modalLayerCreate(MODAL_CONTENTS_POWER);
            break;
        }
    }
}

void EventRankMenuLayer::saveUseDungeonData(string npc_battle_event_rank_list_id)
{
    //ダンジョンシーンで使用するデータを保存
    SaveData::saveString("npc_battle_event_rank_list_id", npc_battle_event_rank_list_id);
    HMSLOG("npc_battle_event_rank_list_id = %s", npc_battle_event_rank_list_id.c_str());
    
}

void EventRankMenuLayer::saveBattleUserSpriteData(EventListSprite* event_sp)
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
    
    SaveData::saveString("event_enemy_sprite_data", data);
}

void EventRankMenuLayer::saveEventPointData(EventListSprite *event_sp)
{
    //選択したイベントのデータを設定
    SaveData::saveString("event_gold", event_sp->getGold());
    SaveData::saveString("event_reward_point", event_sp->getRewardPoint());
    SaveData::saveString("event_rank_point", event_sp->getRankPoint());
    SaveData::saveString("event_rank_level", event_sp->getEventRankLevel());
    
    HMSLOG("event_gold = %s, event_reward_point = %s, event_rank_point = %s", event_sp->getGold().c_str(), event_sp->getRewardPoint().c_str(), event_sp->getRankPoint().c_str());
    
}

void EventRankMenuLayer::update(float dt)
{
    int now_power = fromString<int>(SaveData::loadString("power"));
    
    for (int i = 0; i < _event_rank_sp.size(); i++) {
        if (fromString<int>(_event_rank_sp[i]->getNeedPower()) <= now_power)
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

void EventRankMenuLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void EventRankMenuLayer::_setCascadeColorEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeColorEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeColorEnabled(parent->getChildren().at(i), enabled);
    }
}

void EventRankMenuLayer::soundNG()
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