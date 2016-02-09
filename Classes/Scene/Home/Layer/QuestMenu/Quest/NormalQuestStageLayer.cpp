/* クエスト(小ステージ)選択メニュー */
#include "NormalQuestStageLayer.h"

#define SE_SORT_TAP "download/SE/se_sort_tap.mp3"

#define SWITCH_BUTTON_TAG 11

NormalQuestStageLayer::NormalQuestStageLayer()
{
}

NormalQuestStageLayer::~NormalQuestStageLayer()
{
    //NG音出し中にページ遷移すると音が小さいままなので、デストラクタで戻す
    float bgm_volume    = SaveData::loadFloat("bgm_volume");
    float effect_volume = SaveData::loadFloat("effect_volume");
    
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgm_volume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effect_volume);
}

NormalQuestStageLayer* NormalQuestStageLayer::create()
{
    NormalQuestStageLayer *pRet = new NormalQuestStageLayer();
    
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool NormalQuestStageLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("NormalQuestStageLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void NormalQuestStageLayer::onEnter()
{
    Layer::onEnter();
    
    //クエスト説明データの設定
    setQuestExplainData();
    
    //ステージデータの設定
    setStageData();
    
    //イベントリスナーの登録
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("ステージ選択", "NormalQuest", this);
    
    //ヘルプボタン作成
    createHelpButton() ;
    
    //クエスト説明画像の作成
    createQuestExplainSprite();
    
    //ステージレベルデータの作成
    createStageLavelData();
    
    //ステージリストの作成
    createQuestStageSprite();
    
    //表示切り替えボタンの作成
    createSwitchButton();
    
    //報酬（星）ボタンの数の表示
    createRewardCountSprite();
    
    
    
    //星を五つ集めよう！という表示
    createStarHighlight() ;
    
    //スクロールビューの作成
    createScrollView();
    
    //スケジューラー
    scheduleUpdate();
}


void NormalQuestStageLayer::setQuestExplainData()
{
    string json_text = SaveData::loadString("quest_explain_data");
    string err2;
    _quest_explain_data = Json::parse(json_text, err2);
}

void NormalQuestStageLayer::setStageData()
{
    string json_text = SaveData::loadString("quest_stage_data");
    string err2;
    _quest_stage_data = Json::parse(json_text, err2);
}

void NormalQuestStageLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(NormalQuestStageLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(NormalQuestStageLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(NormalQuestStageLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(NormalQuestStageLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void NormalQuestStageLayer::createQuestExplainSprite()
{
    //クエスト詳細の作成
    _explain_sp = QuestExplainSprite::create(_quest_explain_data);
    _explain_sp->setScale(ratio);
    _explain_sp->setPosition(convertFromDisplaySize(Point(320, 640), 0.5, 1));
    addChild(_explain_sp, 1);
}

void NormalQuestStageLayer::createStageLavelData()
{
    //ステージリストの作成
    int json_count = countJsonData(_quest_stage_data) ;
    for (int i = 0; i < json_count; i++)
    {
        string num = toString<int>(i);
        _quest_stage_level_data.push_back(_quest_stage_data[num].object_items());
    }
    
    //ステージレベルを基準にソート
    sort(_quest_stage_level_data.begin(), _quest_stage_level_data.end(), [](const Json &json1, const Json &json2){ return fromString<int>(json1["quest_stage_level"].string_value()) > fromString<int>(json2["quest_stage_level"].string_value());});
}

void NormalQuestStageLayer::createQuestStageSprite()
{
    //クエストリストの作成
    for (int i = 0; i < (int)_quest_stage_level_data.size(); i++)
    {
        _stage_sp.push_back(QuestStageSprite::create(_quest_stage_level_data[i].object_items(), name_or_reward));
        _stage_sp[i]->setTag(fromString<int>(_quest_stage_level_data[i]["quest_stage_id"].string_value()));
    }
}

void NormalQuestStageLayer::createSwitchButton()
{
    auto title_bar_layer = this->getParent()->getChildByTag(TITLE_LAYER_TAG);
    
    Sprite *img = Sprite::create("init/Home/Quest/bt_switch.png");
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(390, 788), 0.5, 1));
    title_bar_layer->addChild(img, TitleBarLayer::T_Button, SWITCH_BUTTON_TAG);
}

void NormalQuestStageLayer::createRewardCountSprite()
{
    //合計報酬（星）の数の表示
    auto sum_bg = Sprite::create("init/Home/Quest/star_bk.png");
    sum_bg->setScale(ratio);
    sum_bg->setPosition(convertFromDisplaySize(Point(550, 789), 0.5, 1));
    this->addChild(sum_bg, 1, T_SUM_BG);
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

void NormalQuestStageLayer::createStarHighlight()
{
    //星を５個集めよう! ポップアップ
    auto star_highlight = Sprite::create("init/Home/Quest/collect_star.png");
    star_highlight->setPosition(convertFromDisplaySize(Point(390, 870), 0.5, 1));
    star_highlight->setScale(0);
    star_highlight->setGlobalZOrder(1);
    this->addChild(star_highlight, 100) ;

    //表示して削除するアクション
    ScaleTo *scaleTo_big = ScaleTo::create(2.0f, ratio) ;
    EaseElasticOut *scale_ease_out = EaseElasticOut::create(scaleTo_big);
    
    ScaleTo *scaleTo_small = ScaleTo::create(0.7f, 0) ;
    EaseBackIn *scale_back_in = EaseBackIn::create(scaleTo_small);
    
    Sequence *popup_seq = Sequence::create(DelayTime::create(1.0f), scale_ease_out, scale_back_in, RemoveSelf::create(), nullptr) ;
    star_highlight->runAction(popup_seq) ;
}

void NormalQuestStageLayer::createScrollView()
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
    pScrollView->setContentSize(Size(_contentSize.width, cell_size * _stage_sp.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((cell_size * _stage_sp.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio) - (_explain_sp->getContentSize().height * ratio)))));
}

Layer* NormalQuestStageLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    cell_size = 170 * ratio;
    auto layer = Layer::create();
    
    for (int i = (int)_stage_sp.size()-1; 0 <= i; i--)
    {
        string num = toString<int>((int)_stage_sp.size() - 1 -i);
        
        auto bg = _stage_sp[_stage_sp.size() - 1 -i];
        bg->setScale(ratio);
        bg->setPosition(Point(320 * ratio, (cell_size * i) + (cell_size / 2)));
        layer->addChild(bg, 1);
        
        //子ノードにも色合いを適用させる処理
        _setCascadeColorEnabled(_stage_sp[_stage_sp.size() - 1 -i], true);
    }
    
    return layer;
}

bool NormalQuestStageLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //ボタンタッチフラグtrue
    touchButtonFlg = true;
    
    return true;
}
void NormalQuestStageLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void NormalQuestStageLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
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

            auto sum_bg = (Sprite *)this->getChildByTag(T_SUM_BG);
            auto button_help = (Sprite *)this->getChildByTag(T_HELP_BUTTON);
            
            if(name_or_reward=="name") {
                name_or_reward = "reward";
                sum_bg->setVisible(true);
                button_help->setVisible(false) ;
                
            }
            else {
                name_or_reward = "name";
                sum_bg->setVisible(false);
                button_help->setVisible(true) ;
            }
            removeChildByTag(SCROLLVIEW_TAG);
            _stage_sp.clear();
            createQuestStageSprite();
            createScrollView();
            
            return;
        }
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (QuestStageSprite *)child;
            
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

void NormalQuestStageLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらボタンタッチをfalseにする
    touchButtonFlg = false;
}

NormalQuestStageLayer::Power NormalQuestStageLayer::checkPower(int need_power)
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

void NormalQuestStageLayer::changePage(int tag)
{
    //タップしたスプライトの取得
    auto stage_sp = (QuestStageSprite *)this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(tag);
  
    //体力チェック
    auto normal_scene = (NormalScene *)this->getParent();
    
    switch (checkPower(fromString<int>(stage_sp->getNeedPower()))) {
        case GO:
        {
            //選択したクエストIDの保存
            SaveData::saveString("quest_stage_id", stage_sp->getStageId());
            
            //クエストの対戦相手スプライト情報の保存
            saveQuestEnemySpriteData(stage_sp);
            
            //クエストデータの保存
            saveQuestPointData(stage_sp);
            
            //フレンド選択ページ遷移
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("BattleFriendChoiceForQuest");
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

void NormalQuestStageLayer::saveQuestEnemySpriteData(QuestStageSprite* stage_sp)
{
    //クエストの対戦相手スプライト情報の保存
    string chara_id   = stage_sp->getStageCharaId();
    string rank_name  = stage_sp->getStageRankName();
    string rare       = stage_sp->getStageCharaRare();
    string npc_name   = stage_sp->getStageNpcName();
    string offense    = "??" ;
    string defense    = "??" ;
    string strategist = "??" ;
    string general    = "??" ;
    string battle_point_rate = "0" ;
    
    string data = "";
    string cr = "\n";
    data += "{" + cr;
    data += "  \"chara_id\":\"" + chara_id+ "\"," + cr;
    data += "  \"rank_name\":\"" + rank_name + "\"," + cr;
    data += "  \"rare\":\"" + rare + "\"," + cr;
    data += "  \"npc_name\":\"" + npc_name + "\"," + cr;
    data += "  \"offense\":\"" + offense + "\"," + cr;
    data += "  \"defense\":\"" + defense + "\"," + cr;
    data += "  \"strategist\":\"" + strategist + "\"," + cr;
    data += "  \"general\":\"" + general + "\"," + cr;
    data += "  \"battle_point_rate\":\"" + battle_point_rate + "\"" + cr;
    data += "}";
    HMSLOG("quest_enemy_sprite_data = %s", data.c_str()) ;
    
    SaveData::saveString("quest_enemy_sprite_data", data);
}

void NormalQuestStageLayer::saveQuestPointData(QuestStageSprite* stage_sp)
{
    //選択したクエストのデータを設定
    SaveData::saveString("quest_gold", stage_sp->getGainGold());
    SaveData::saveString("quest_reward_point", stage_sp->getGainRewardPoint());
    SaveData::saveString("quest_rank_point", stage_sp->getGainRankPoint());
    
}

void NormalQuestStageLayer::update(float dt)
{
    int now_power = fromString<int>(SaveData::loadString("power"));
    
    for (int i = 0; i < _stage_sp.size(); i++) {
        if (fromString<int>(_stage_sp[i]->getNeedPower()) <= now_power)
        {
            //現在の体力が必要行動力以上になった場合、通常表示
            _stage_sp[i]->setColor(Color3B(255, 255, 255));
            _stage_sp[i]->changeVisibleSprite(true);
            
        }
        else
        {
            //必要行動力に足りていない場合、グレー表示
            _stage_sp[i]->setColor(Color3B(140, 140, 140));
            _stage_sp[i]->changeVisibleSprite(false);
        }
    }
}

void NormalQuestStageLayer::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void NormalQuestStageLayer::_setCascadeColorEnabled(Node* parent, bool enabled)
{
    //子ノード全てに適用させる処理
    parent->setCascadeColorEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeColorEnabled(parent->getChildren().at(i), enabled);
    }
}

void NormalQuestStageLayer::soundNG()
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

void NormalQuestStageLayer::createHelpButton()
{
    //ヘルプボタン
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [this](Ref* sender) {
                                                 //イベント詳細の表示
                                                 auto normal_scene = (NormalScene *)this->getParent();
                                                 string url = HOST;
                                                 url += "user/index.php?p=page__star_help&user_id=" + SaveData::loadString("user_master_id") ;
                                                 
                                                 auto WebViewLoadLayer = WebViewLoadLayer::create(url, "ヘルプ");
                                                 normal_scene->addChild(WebViewLoadLayer, 10);
                                         });
    
    help_btn->setScale(ratio);
    help_btn->setAnchorPoint(Point(0.5, 1));
    auto menu = Menu::create(help_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(570, 838), 0.5, 1));
    this->addChild(menu, 5, T_HELP_BUTTON);
}