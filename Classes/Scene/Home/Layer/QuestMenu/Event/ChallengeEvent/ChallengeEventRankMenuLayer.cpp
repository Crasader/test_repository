#include "ChallengeEventRankMenuLayer.h"

#define SE_SORT_TAP "download/SE/se_sort_tap.mp3"

#define SWITCH_BUTTON_TAG 11

ChallengeEventRankMenuLayer::~ChallengeEventRankMenuLayer()
{
    //NG音出し中にページ遷移すると音が小さいままなので、デストラクタで戻す
    float bgm_volume    = SaveData::loadFloat("bgm_volume");
    float effect_volume = SaveData::loadFloat("effect_volume");
    
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
}

bool ChallengeEventRankMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ChallengeEventRankMenuLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void ChallengeEventRankMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントランクデータの設定
    setChallengeEventRankData();
    
    //最終確認画面の敵スプライトのデータ初期化
    initChallengeEventEnemyData();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("試練に挑戦", "ChallengeEventMenu", this);
    
    //イベントリスナーの登録
    initEventLitsener();
    
    //イベント詳細ボタンの作成
    createDetailButton();
    
    //イベント説明画像の作成
    createChallengeEventExplainSprite();
    
    //スクロールビューの作成
    createScrollView();
    
    //体力チェックアップデート関数
    scheduleUpdate();
}

void ChallengeEventRankMenuLayer::setChallengeEventRankData()
{
    string json_text = SaveData::loadString("challenge_event_rank_data");
    string err2;
    _event_rank_data = Json::parse(json_text, err2);
    
    //イベントのランクの数を取得
    _event_rank_count = countJsonData(_event_rank_data);
    
}

void ChallengeEventRankMenuLayer::initChallengeEventEnemyData()
{
    //最終確認画面でのデータを初期化
    SaveData::saveString("npc_battle_event_rank_list_id", "");
    SaveData::saveString("event_gold", "0");
    SaveData::saveString("event_rank_point", "0");
    SaveData::saveString("event_rank_level", "0");
    SaveData::saveString("event_reward_point", "0");
    SaveData::saveString("event_enemy_sprite_data", "");
}

void ChallengeEventRankMenuLayer::initEventLitsener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(ChallengeEventRankMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ChallengeEventRankMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ChallengeEventRankMenuLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ChallengeEventRankMenuLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void ChallengeEventRankMenuLayer::createDetailButton()
{
    //イベント詳細ボタンの作成
    auto detail_off = Sprite::create("init/Home/Quest/bt_event_detail.png");
    auto detail_on  = Sprite::create("init/Home/Quest/bt_event_detail.png");
    detail_on->setColor(Color3B(180, 180, 180));
    
    auto detail_btn = MenuItemSprite::create(detail_off, detail_on, [this](Ref* sender)
                                             {
                                                 string challenge_event_id = SaveData::loadString("challenge_event_id");
                                                 
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__challenge_event_detail_" + challenge_event_id + "&challenge_event_id=" + challenge_event_id + "&user_id=" + SaveData::loadString("user_master_id");
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "イベント詳細");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                             });
    detail_btn->setScale(ratio);
    detail_btn->setTag(T_Detail_MenuItem);
    
    auto menu = Menu::create(detail_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(menu, ZORDER_BUTTON, T_Detail_Menu);
}

void ChallengeEventRankMenuLayer::createChallengeEventExplainSprite()
{
    //イベント説明画像データの取得
    string json_text = SaveData::loadString("challenge_event_explain_data");
    string err2;
    Json event_explain_data = Json::parse(json_text, err2);
    
    //イベント説明画像の作成
    HMSLOG("json_text=%s", json_text.c_str());
    string challenge_event_id = SaveData::loadString("challenge_event_id");
    string image_path = "download/EventMenu/ChallengeEvent/event" + challenge_event_id + "/event_explain_" + challenge_event_id + ".png";
    _explain_sp = ChallengeEventExplainSprite::create(event_explain_data, image_path);
    _explain_sp->setScale(ratio);
    _explain_sp->setPosition(convertFromDisplaySize(Point(320, 640), 0.5, 1));
    addChild(_explain_sp, 1);
}

void ChallengeEventRankMenuLayer::createScrollView()
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

Layer* ChallengeEventRankMenuLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 220 * ratio;
    auto layer = Layer::create();

    int num = 0;
    for (int i = (_event_rank_count - 1); 0 <= i; i--)
    {
        string numStr = toString<int>(num);
        
        _event_rank_sp.push_back(ChallengeEventListSprite::create(_event_rank_data[numStr]));
        _event_rank_sp[num]->setScale(ratio);
        //_event_rank_sp[num]->setTag(1);
        _event_rank_sp[num]->setTag(fromString<int>(_event_rank_data[numStr]["ranking_event_stage_id"].string_value()));
        _event_rank_sp[num]->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2) - 10 * ratio));
        layer->addChild(_event_rank_sp[num], 1);
        
        //子ノードにも色合いを適用させる処理
        _setCascadeColorEnabled(_event_rank_sp[num], true);
        
        num++;
    }
    
    return layer;
}

bool ChallengeEventRankMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_event_rank_sp.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void ChallengeEventRankMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void ChallengeEventRankMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (ChallengeEventListSprite *)child;
            
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
void ChallengeEventRankMenuLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void ChallengeEventRankMenuLayer::scrollViewDidScroll(ScrollView *view)
{
    touchButtonFlg = false;
}

ChallengeEventRankMenuLayer::Power ChallengeEventRankMenuLayer::checkPower(int need_power)
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

void ChallengeEventRankMenuLayer::changePage(int tag)
{
    auto event_sp = (ChallengeEventListSprite *)this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(tag);
    
    //体力チェック
    auto normal_scene = (NormalScene *)this->getParent();
    
    switch (checkPower(fromString<int>(event_sp->getNeedPower()))) {
        case GO:
        {
            //選択したイベントランクリストIDの保存
            SaveData::saveString("challenge_event_stage_id", event_sp->getChallengeEventStageId());
            HMSLOG("challenge_event_stage_id = %s", event_sp->getChallengeEventStageId().c_str());
            
            //最終確認画面の敵スプライト画像の設定
            saveBattleUserSpriteData(event_sp);
            
            //選択したイベントデータの設定
            saveChallengeEventPointData(event_sp);
            
            //フレンド選択画面へ遷移
            //normal_scene->changePage("BattleFriendChoiceForChallengeEvent");
            //デッキ選択画面へ遷移
            normal_scene->changePage("BattleDeckChoiceForChallengeEvent");
            //報酬確認画面へ遷移
            //normal_scene->changePage("ChallengeEventRewardListLayer");
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

void ChallengeEventRankMenuLayer::saveBattleUserSpriteData(ChallengeEventListSprite* event_sp)
{
    //最終確認画面の対戦相手画像データの設定
    string user_master_id = "" ;
    string friend_id = "";
    string card_image_id = event_sp->getCardImageId();
    string user_name = event_sp->getUserName();
    string level = "0" ;
    string last_play_date = "";
    string comment = "";
    string rank_name = event_sp->getRankName();
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
    
    SaveData::saveString("challenge_event_enemy_sprite_data", data);
}

void ChallengeEventRankMenuLayer::saveChallengeEventPointData(ChallengeEventListSprite *event_sp)
{
    //選択したイベントのデータを設定
    SaveData::saveString("event_gold", event_sp->getGold());
    SaveData::saveString("event_reward_point", event_sp->getRewardPoint());
    SaveData::saveString("event_rank_point", event_sp->getRankPoint());
    SaveData::saveString("event_rank_level", event_sp->getRankLevel());
    
    HMSLOG("event_gold = %s, event_reward_point = %s, event_rank_point = %s", event_sp->getGold().c_str(), event_sp->getRewardPoint().c_str(), event_sp->getRankPoint().c_str());
    
}

void ChallengeEventRankMenuLayer::update(float dt)
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

void ChallengeEventRankMenuLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void ChallengeEventRankMenuLayer::_setCascadeColorEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeColorEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeColorEnabled(parent->getChildren().at(i), enabled);
    }
}

void ChallengeEventRankMenuLayer::soundNG()
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
