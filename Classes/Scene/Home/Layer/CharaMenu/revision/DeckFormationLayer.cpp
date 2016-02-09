#include "DeckFormationLayer.h"

DeckFormationLayer::DeckFormationLayer()
:_row_count(0)
,_deck_no(1)
,_total_number(0)
,_offense(0)
,_defense(0)
,_stratege(0)
,_chara_data_list(nullptr)
,_user_party_list(nullptr)
,_sort_status(CharaListSortPopupLayer::SortStatus::Power)
,_touch_chara_index(-1)
,_deck_touch_flg(false)
,_deck_status(DeckSetStatus::Initialize)
,_view_count_default(0)
{
}

DeckFormationLayer* DeckFormationLayer::create(bool *chara_rarity, bool *chara_type)
{
    DeckFormationLayer *sprite = new DeckFormationLayer();
    
    if (sprite && sprite->init(chara_rarity, chara_type))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckFormationLayer::init(bool *chara_rarity, bool *chara_type)
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("DeckFormationLayer::init()");
    
    ratio = getVisibleRatio();
    
    DeckFormationLayer::viewCount = _view_count_default;
    //データの初期化
    initData(chara_rarity, chara_type);
    
    return true;
}

void DeckFormationLayer::initData(bool *chara_rarity, bool *chara_type)
{
    //データの初期化
    _deck_chara.clear();
    _deck_pos_index_list.clear();
    _chara.clear();
    _chara_pos_index_list.clear();
    _refine_index_list.clear();
    _row_count = 0;
    _sort_status = (CharaListSortPopupLayer::SortStatus)SaveData::loadInt("sort_status");
    
    //デッキリストの初期化
    _deck_chara_list.reserve(MAX_DECK_CHARA_COUNT); //ベクターの要素確保
    for (int i = 0 ; i < MAX_DECK_CHARA_COUNT; i++)
    {
        _deck_chara_list.push_back(nullptr);
    }
    
    //レア度絞り込み、キャラタイプの初期化
    for (int i = 0; i < sizeof(_chara_rarity) / sizeof(_chara_rarity[0]); i++)
    {
        _chara_rarity[i] = chara_rarity[i];
    }
    
    for (int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++)
    {
        _chara_type[i] = chara_type[i];
    }
}

void DeckFormationLayer::initSortData(bool *chara_type)
{
    //データの初期化
    _chara_pos_index_list.clear();
    _refine_index_list.clear();
    _row_count = 0;
    _sort_status = (CharaListSortPopupLayer::SortStatus)SaveData::loadInt("sort_status");
    
    //キャラタイプの初期化
    for (int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++)
    {
        _chara_type[i] = chara_type[i];
    }
}

void DeckFormationLayer::onEnter()
{
    Layer::onEnter();
    
    //データの設定
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //ページタイトルの作成
//    HeaderLayer::setupTitleBar("デッキ編成", "DeckFormationTop", this);
    
    //ページタイトルの作成（戻るボタンに判定を持たせたいのでここで作成）
    createTitleBar();
    
    //並び替えボタンの作成
    createSortButton();
    
    //選択デッキ背景の作成
    createDeckBg();
    
    //デッキ背景のアイコン作成
    createDeckBgIcon();
    
    //デッキのパラメータ作成
    createDeckParamLabel();
    
    //デッキポジションインデックスの作成
    createDeckPosIndex();
    
    //デッキキャラ枠の作成
    createDeckCharaBg();
    
    //レア度絞り込みボタンの作成
    createRarityButton();
    
    //属性絞り込みボタンの作成
    createTypeButton();
    
    //絞り込み切り替えボタンの作成
    createSwitchButton();
    
    //キャラ一覧の画像の作成
    createCharaSprite();
    
    //キャラのソート
    sortCharaData();
    
    //絞り込みインデックスの作成
    createRefineIndexList();
    
    //キャラ一覧のポジションインデックスの作成
    createCharaPosIndex();
    
    //スクロールの作成
    createScrollView();
    
    //スクロールデータの設定
    setScrollViewData();
    
    //キャラ一覧の初期表示
    showCharaSprite();
    
    //キャラ画像の生成
    this->schedule(schedule_selector(DeckFormationLayer::showCharaSpriteInterval));
    
    //ページバーの作成
    createPageBar();
    
    //絞り込みボタンのデータの設定
    setRarityButtonAndTypeButtonData();
    
    //選択デッキ状態の設定
    setDeckData();
}

void DeckFormationLayer::setData()
{
    //キャラデータの設定
    string json_text = SaveData::loadString("chara_data_list");
    string err;
    _chara_data_list = Json::parse(json_text, err);
    
    //デッキ使用データの設定
    string json_text2 = SaveData::loadString("user_party_list");
    string err2;
    _user_party_list = Json::parse(json_text2, err2);
    
    //デッキ番号の設定
    _deck_no = fromString<int>(SaveData::loadString("party_no"));
}

void DeckFormationLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(DeckFormationLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckFormationLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckFormationLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckFormationLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckFormationLayer::createTitleBar()
{
    //タイトルバーの作成
    auto title_bar = Sprite::create("init/Header/title_base.png");
    title_bar->setScale(ratio);
    title_bar->setPosition(convertFromDisplaySize(Point(236, 789), 0.5, 1));
    this->addChild(title_bar, Z_Title_Bar, T_Title_Bar);
    
    //戻るボタン
    auto back_off = Sprite::create("init/Header/bt_return.png");
    auto back_on = Sprite::create("init/Header/bt_return.png");
    back_on->setColor(Color3B(180, 180, 180));
    
    auto back_btn = MenuItemSprite::create(back_off, back_on, [this](Ref* sender){
        
        if (_deck_status == DeckSetStatus::Initialize)
        {
            //ページ遷移
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("DeckFormationTop");
        }
        else {
            //警告ポップアップの表示
            auto ok_func = [this](){
                //ページ遷移
                auto normal_scene = (NormalScene *)this->getParent();
                normal_scene->changePage("DeckFormationTop");
            };
            
            auto ng_func = [](){}; //いいえを押したとき何もしない
            popupConfirmLayer *layer = popupConfirmLayer::create("デッキ変更が完了されていません。\n変更内容を保存せず戻ってもよろしいですか？");
            layer->setFunction(ok_func, ng_func);
            this->addChild(layer, Z_Confirm_Layer, T_Confirm_Layer);
        }
        
    });
    back_btn->setScale(ratio);
    auto Menu = Menu::create(back_btn, nullptr);
    Menu->setPosition(convertFromDisplaySize(Point(58, 788), 0.5, 1));
    this->addChild(Menu, Z_Back_Btn, T_Back_Btn);
    
    //ページ名
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("デッキ編成", FONT_NAME_SYSTEM, 32);
    title_label->setScale(ratio);
    title_label->setAnchorPoint(Point(0, 0.5));
    title_label->setPosition(convertFromDisplaySize(Point(111, 788), 0.5, 1));
    title_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(title_label, Z_Title_Label, T_Title_Label);
    
    //おすすめボタン
    auto recommend_off = Sprite::create("init/CharaMenu/bt_recommended.png");
    auto recommend_on  = Sprite::create("init/CharaMenu/bt_recommended.png");
    recommend_on->setColor(Color3B(180, 180, 180));
    
    auto menuItem = MenuItemSprite::create(recommend_off, recommend_on, [this](Ref* sender){
        
        //おすすめポップアップの表示
        auto layer = DeckRecommendLayer::create();
        this->addChild(layer, Z_Recommend_Layer, T_Recommend_Layer);
        
    });
    menuItem->setTag(T_Recommend_MenuItem);
    
    auto menu2 = Menu::create(menuItem, nullptr);
    menu2->setPosition(Point(360, title_bar->getContentSize().height / 2));
    title_bar->addChild(menu2, Z_Recommend_Menu, T_Recommend_Menu);
}

void DeckFormationLayer::createSortButton()
{
    //並び替えボタンの生成
    auto sortOff = Sprite::create("init/Home/Common/bt_display_change.png");
    auto sortOn  = Sprite::create("init/Home/Common/bt_display_change.png");
    sortOn->setCascadeColorEnabled(true);
    sortOn->setColor(Color3B(180,180,180));
    
    //表示テキスト
    auto sort_label_off = HMSFontUtil::createWithTTFBySystemFontArgument(createSortText(), FONT_NAME_SYSTEM, 20);
    sort_label_off->enableOutline(Color4B::BLACK, 1);
    sort_label_off->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    sort_label_off->setPosition(Point(sortOff->getContentSize().width / 2, 22));
    sortOff->addChild(sort_label_off, T_SortOff_Text, Z_SortOff_Text);
    
    auto sort_label_on = HMSFontUtil::createWithTTFBySystemFontArgument(createSortText(), FONT_NAME_SYSTEM, 20);
    sort_label_on->enableOutline(Color4B::BLACK, 1);
    sort_label_on->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    sort_label_on->setPosition(Point(sortOff->getContentSize().width / 2, 22));
    sortOn->addChild(sort_label_on, T_SortOn_Text, Z_SortOn_Text);
    
    auto sort_btn = MenuItemSprite::create(sortOff, sortOn, CC_CALLBACK_1(DeckFormationLayer::showSortPopup, this));
    sort_btn->setScale(ratio);
    sort_btn->setTag(T_SortBtn_MenuItem);
    auto menu = Menu::create(sort_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(555, 788), 0.5, 1));
    this->addChild(menu, Z_SortBtn_Menu, T_SortBtn_Menu);
}

string DeckFormationLayer::createSortText()
{
    //ソートボタンのパラメータ作成
    string sort_text = "";
    switch (_sort_status)
    {
        case CharaListSortPopupLayer::SortStatus::Rare:       sort_text = "レア度"; break;
        case CharaListSortPopupLayer::SortStatus::NeedCost:   sort_text = "配置士気"; break;
        case CharaListSortPopupLayer::SortStatus::InsertDate: sort_text = "入手順"; break;
        default: sort_text = "戦闘力"; break;
    }
    
    return sort_text;
}

void DeckFormationLayer::createDeckBg()
{
    //選択デッキの背景
    _deck_bg = Sprite::create("init/CharaMenu/revision/deck.png");
    _deck_bg->setScale(ratio);
    _deck_bg->setPosition(convertFromDisplaySize(Point(320, 659), 0.5, 1.0));
    addChild(_deck_bg, Z_Deck_Bg, T_Deck_Bg);
    
//    //全体の背景
//    auto bg = Sprite::create("init/CharaMenu/revision/deck_bk.png");
//    bg->setScale(ratio);
//    bg->setAnchorPoint(Point(0.5, 0));
//    bg->setPosition(convertFromDisplaySize(Point(320, 0), 0.5, 0));
//    addChild(bg, Z_Bg, T_Bg);
}

void DeckFormationLayer::createDeckBgIcon()
{
    //攻撃
    auto offense_icon = Sprite::create("init/CharaMenu/icon_offense3.png");
    offense_icon->setPosition(Point(262, 166));
    _deck_bg->addChild(offense_icon);
    
    //防御
    auto defense_icon = Sprite::create("init/CharaMenu/icon_defense3.png");
    defense_icon->setPosition(Point(387, 166));
    _deck_bg->addChild(defense_icon);
    
    //軍師
    auto strategist_icon = Sprite::create("init/CharaMenu/icon_strategist3.png");
    strategist_icon->setPosition(Point(512, 166));
    _deck_bg->addChild(strategist_icon);
}

void DeckFormationLayer::createDeckParamLabel()
{
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("カード枚数", FONT_NAME_SYSTEM, 17);
    label->setPosition(Point(80, 166));
    _deck_bg->addChild(label);
    
    //合計カード枚数
    auto total_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(getTotalNumber()) + "/" + toString<int>(MAX_DECK_CHARA_COUNT), FONT_NAME_SYSTEM, 22);
    total_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    total_label->setAnchorPoint(Point(1.0, 0.5));
    total_label->setPosition(Point(225, 166));
    _deck_bg->addChild(total_label, Z_Total_Label, T_Total_Label);
    
    //攻撃カード枚数
    auto offense_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(getOffense()) + "枚", FONT_NAME_SYSTEM, 22);
    offense_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    offense_label->setAnchorPoint(Point(1.0, 0.5));
    offense_label->setPosition(Point(355, 166));
    _deck_bg->addChild(offense_label, Z_Offense_Label, T_Offense_Label);
    
    //防御カード枚数
    auto defense_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(getDefense()) + "枚", FONT_NAME_SYSTEM, 22);
    defense_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    defense_label->setAnchorPoint(Point(1.0, 0.5));
    defense_label->setPosition(Point(480, 166));
    _deck_bg->addChild(defense_label, Z_Defense_Label, T_Defense_Label);
    
    //軍師カード枚数
    auto strategist_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(getStratege()) + "枚", FONT_NAME_SYSTEM, 22);
    strategist_label->setHorizontalAlignment(TextHAlignment::RIGHT);
    strategist_label->setAnchorPoint(Point(1.0, 0.5));
    strategist_label->setPosition(Point(605, 166));
    _deck_bg->addChild(strategist_label, Z_Strategist_Label, T_Strategist_Label);
}

void DeckFormationLayer::createDeckPosIndex()
{
    //デッキのポジションインデックスの作成
    Utility::createPosisionIndex(_deck_pos_index_list, 2, 10, 58, 110, 58, 58);
    
}

void DeckFormationLayer::createDeckCharaBg()
{
    //デッキキャラ枠の背景
    _deck_empty = SpriteBatchNode::create("init/CharaMenu/revision/deck_empty.png");
    this->addChild(_deck_empty);
    
    for (int i = 0; i < MAX_DECK_CHARA_COUNT; i++)
    {
        _deck_chara.push_back(Sprite::createWithTexture(_deck_empty->getTexture()));
        _deck_chara[i]->setPosition(_deck_pos_index_list[i]);
        _deck_bg->addChild(_deck_chara[i], Z_Deck_Empty_Chara, T_Deck_Empty_Chara);
    }
}

void DeckFormationLayer::createRarityButton()
{
    //レア度絞り込みボタン
    
    //N
    auto normal_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray4.png", "init/CharaMenu/revision/bt_gray4.png");
    auto normal_on  = MenuItemImage::create("init/CharaMenu/revision/bt4.png", "init/CharaMenu/revision/bt4.png");
    auto normal_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectRarity, this), normal_off, normal_on, nullptr);
    normal_btn->setTag(T_Normal_Btn);
    normal_btn->setScale(ratio);
    
    //R
    auto rare_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray5.png", "init/CharaMenu/revision/bt_gray5.png");
    auto rare_on  = MenuItemImage::create("init/CharaMenu/revision/bt5.png", "init/CharaMenu/revision/bt5.png");
    auto rare_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectRarity, this), rare_off, rare_on, nullptr);
    rare_btn->setTag(T_Rare_Btn);
    rare_btn->setScale(ratio);
    
    //SR
    auto super_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray6.png", "init/CharaMenu/revision/bt_gray6.png");
    auto super_on  = MenuItemImage::create("init/CharaMenu/revision/bt6.png", "init/CharaMenu/revision/bt6.png");
    auto super_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectRarity, this), super_off, super_on, nullptr);
    super_btn->setTag(T_Super_Btn);
    super_btn->setScale(ratio);
    
    //UR
    auto ultra_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray7.png", "init/CharaMenu/revision/bt_gray7.png");
    auto ultra_on  = MenuItemImage::create("init/CharaMenu/revision/bt7.png", "init/CharaMenu/revision/bt7.png");
    auto ultra_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectRarity, this), ultra_off, ultra_on, nullptr);
    ultra_btn->setTag(T_Ultra_Btn);
    ultra_btn->setScale(ratio);
    
    //PT
    auto platinum_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray8.png", "init/CharaMenu/revision/bt_gray8.png");
    auto platinum_on  = MenuItemImage::create("init/CharaMenu/revision/bt8.png", "init/CharaMenu/revision/bt8.png");
    auto platinum_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectRarity, this), platinum_off, platinum_on, nullptr);
    platinum_btn->setTag(T_Platinum_Btn);
    platinum_btn->setScale(ratio);
    
    auto menu = Menu::create(normal_btn, rare_btn, super_btn, ultra_btn, platinum_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(4);
    menu->setPosition(convertFromDisplaySize(Point(250, 535), 0.5, 1.0));
    this->addChild(menu, Z_Rarity_Menu, T_Rarity_Menu);
}

void DeckFormationLayer::createTypeButton()
{
    //属性絞り込みボタン
    
    //攻撃選択
    auto offense_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray1.png", "init/CharaMenu/revision/bt_gray1.png");
    auto offense_on  = MenuItemImage::create("init/CharaMenu/revision/bt1.png", "init/CharaMenu/revision/bt1.png");
    auto offense_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectType, this), offense_off, offense_on, nullptr);
    offense_btn->setTag(T_Offense_Btn);
    offense_btn->setScale(ratio);
    
    //防御選択
    auto defense_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray2.png", "init/CharaMenu/revision/bt_gray2.png");
    auto defense_on  = MenuItemImage::create("init/CharaMenu/revision/bt2.png", "init/CharaMenu/revision/bt2.png");
    auto defense_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectType, this), defense_off, defense_on, nullptr);
    defense_btn->setTag(T_Defense_Btn);
    defense_btn->setScale(ratio);
    
    //軍師選択
    auto strategist_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray3.png", "init/CharaMenu/revision/bt_gray3.png");
    auto strategist_on  = MenuItemImage::create("init/CharaMenu/revision/bt3.png", "init/CharaMenu/revision/bt3.png");
    auto strategist_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(DeckFormationLayer::selectType, this), strategist_off, strategist_on, nullptr);
    strategist_btn->setTag(T_Stratege_Btn);
    strategist_btn->setScale(ratio);
    
    auto menu = Menu::create(offense_btn, defense_btn, strategist_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(convertFromDisplaySize(Point(250 - _contentSize.width, 535), 0.5, 1.0));
    this->addChild(menu, Z_Type_Menu, T_Type_Menu);
    
    menu->setVisible(false);
}

void DeckFormationLayer::createSwitchButton()
{
    //絞り込み切り替えボタン
    auto switch_off = MenuItemImage::create("init/CharaMenu/revision/switch_rarity.png", "init/CharaMenu/revision/switch_rarity.png");
    auto switch_on  = MenuItemImage::create("init/CharaMenu/revision/switch_type.png", "init/CharaMenu/revision/switch_type.png");
    auto switch_btn = MenuItemToggle::createWithCallback([this](Ref* sender){
        
        auto menuItem = dynamic_cast<MenuItemToggle*>(sender);
        menuItem->setEnabled(false);
        
        auto rarity_menu = (Menu *)this->getChildByTag(T_Rarity_Menu);
        auto type_menu   = (Menu *)this->getChildByTag(T_Type_Menu);
        
        auto move = MoveBy::create(0.2f, Point(_contentSize.width * ratio + getVisibleSpaceWidth() * 2 * 0.5, 0));
        
        if(menuItem->getSelectedIndex() == 1){
            this->runAction(Sequence::create(TargetedAction::create(rarity_menu, Sequence::create(move->reverse(), Hide::create(), nullptr)),
                                             TargetedAction::create(type_menu,   Sequence::create(Show::create(),   move->clone(), nullptr)),
                                             CallFunc::create([menuItem](){menuItem->setEnabled(true);}),
                                             NULL));
        }else{
            this->runAction(Sequence::create(TargetedAction::create(type_menu, Sequence::create(move->reverse(), Hide::create(), nullptr)),
                                             TargetedAction::create(rarity_menu,   Sequence::create(Show::create(),   move->clone(), nullptr)),
                                             CallFunc::create([menuItem](){menuItem->setEnabled(true);}),
                                             NULL));
            
        }
    }, switch_off, switch_on, nullptr);
    
    switch_btn->setTag(T_Switch_MenuItem);
    switch_btn->setScale(ratio);
    
    auto menu = Menu::create(switch_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(564, 535), 0.5, 1.0));
    this->addChild(menu, Z_Switch_Menu, T_Switch_Menu);
}

void DeckFormationLayer::createCharaSprite()
{
//    auto batch = SpriteBatchNode::create("init/CharaMenu/card_empty.png");
    
    //キャラ画像の作成
    int count = countJsonData(_chara_data_list);
    for (int i = 0; i < count; i++)
    {
//        _chara.push_back(Sprite::createWithTexture(batch->getTexture()));
        
        string iStr = toString(i);
        _chara.push_back(CharaSprite::create(_chara_data_list[iStr].object_items()));
    }
}

void DeckFormationLayer::showCharaSpriteInterval(float f)
{
    if(_refine_index_list.size()<=DeckFormationLayer::viewCount){
        HMSLOG("UNSCHEDULE:Sprite2End");
        unschedule(schedule_selector(DeckFormationLayer::showCharaSpriteInterval));
        return;
    }
    int i = 0;
    while(true){
        if(!_chara[_refine_index_list[DeckFormationLayer::viewCount]]->isVisible()){
            _chara[_refine_index_list[DeckFormationLayer::viewCount]]->setVisible(true);
//            break;
            i++;
            if(i>=2)
                break;
        }
        DeckFormationLayer::viewCount++;
        if(_refine_index_list.size()<=DeckFormationLayer::viewCount){
            HMSLOG("UNSCHEDULE:Sprite2End2");
            unschedule(schedule_selector(DeckFormationLayer::showCharaSpriteInterval));
            return;
        }
    }
}
float DeckFormationLayer::calcMaxCharaIdLength()
{
    float max_length = 0;
    for (int i = 0; i < _chara.size(); i++) {
        if (max_length < (float)_chara[i]->getCharaId().size())
        {
            max_length = (float)_chara[i]->getCharaId().size();
        }
    }
    
    return max_length;
}

void DeckFormationLayer::sortCharaData()
{
    int id = TimeMeasure::start() ;
    
    //キャラIDの最大文字列の長さを取得
    float max_length = calcMaxCharaIdLength();
    
    //キャラクターのソート
    switch (_sort_status)
    {
        case CharaListSortPopupLayer::SortStatus::Rare:
        {
            //レア度でソート
            sort(_chara.begin(), _chara.end(), [max_length](const CharaSprite* chara1, const CharaSprite* chara2){
                return fromString<int>(chara1->getRare()) * pow(10.0, max_length) + fromString<int>(chara1->getCharaId()) > fromString<int>(chara2->getRare()) * pow(10.0, max_length) + fromString<int>(chara2->getCharaId());
            });
        } break;
        case CharaListSortPopupLayer::SortStatus::NeedCost:
        {
            //配置士気の高い順にソート
            sort(_chara.begin(), _chara.end(), [max_length](const CharaSprite* chara1, const CharaSprite* chara2){
                return fromString<int>(chara1->getNeedCost()) * pow(10.0, max_length) + fromString<int>(chara1->getCharaId()) > fromString<int>(chara2->getNeedCost()) * pow(10.0, max_length) + fromString<int>(chara2->getCharaId());
            });
        } break;
        case CharaListSortPopupLayer::SortStatus::InsertDate:
        {
            //入手順でソート（最新順）
            sort(_chara.begin(), _chara.end(), [](const CharaSprite* chara1, const CharaSprite* chara2){
                return chara1->getInsertDateTimestamp() > chara2->getInsertDateTimestamp();
            });
        } break;
        default:
        {
            //戦闘力順にソート
            sort(_chara.begin(), _chara.end(), [max_length](const CharaSprite* chara1, const CharaSprite* chara2){
                return fromString<int>(chara1->getPower()) * pow(10.0, max_length) + fromString<int>(chara1->getCharaId()) > fromString<int>(chara2->getPower()) * pow(10.0, max_length) + fromString<int>(chara2->getCharaId());
            });
        }break;
    }
    
//    switch (_sort_status)
//    {
//        case CharaListSortPopupLayer::SortStatus::Rare:
//        {
//            _chara = sortCharacter(_chara, "rare");
//        } break;
//        case CharaListSortPopupLayer::SortStatus::NeedCost:
//        {
//            _chara = sortCharacter(_chara, "cost");
//        } break;
//        case CharaListSortPopupLayer::SortStatus::InsertDate:
//        {
//            _chara = sortCharacter(_chara, "insert_date");
//        } break;
//        default:
//        {
//            _chara = sortCharacter(_chara, "power");
//        }break;
//    }
    
    TimeMeasure::endAndPrint(id) ;

}

//HmsVector<CharaSprite *> DeckFormationLayer::sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param)
//{
//    HmsVector<CharaSprite *> base_list, left_list, right_list;
//    if(chara_list.size()==0) return base_list;
//    
//    CharaSprite *chara0 = chara_list.at(0);
//    time_t sort_val0 = getSortVal(chara0, sort_param);
//    int chara_id0 = fromString<int>(chara0->getCharaId());
//    for(int i=1; i<chara_list.size(); i++) {
//        CharaSprite *chara = chara_list.at(i);
//        time_t sort_val = getSortVal(chara_list.at(i), sort_param);
//        int chara_id = fromString<int>(chara_list.at(i)->getCharaId());
//        
//        if(sort_val>sort_val0) {
//            left_list.push_back(chara);
//        } else if(sort_val<sort_val0) {
//            right_list.push_back(chara);
//        } else {
//            if(chara_id>chara_id0) left_list.push_back(chara);
//            else right_list.push_back(chara);
//        }
//    }
//    
//    left_list = sortCharacter(left_list, sort_param);
//    right_list = sortCharacter(right_list, sort_param);
//    
//    for(int i=0; i<left_list.size(); i++) {
//        base_list.push_back(left_list.at(i));
//    }
//    base_list.push_back(chara0);
//    for(int i=0; i<right_list.size(); i++) {
//        base_list.push_back(right_list.at(i));
//    }
//    
//    return base_list;
//}

long DeckFormationLayer::getSortVal(CharaSprite *chara, string sort_param)
{
    time_t sort_val;
    if(sort_param=="rare") sort_val = chara->_rare_int;
    if(sort_param=="cost") sort_val = chara->_need_cost_int;
    if(sort_param=="power") sort_val = chara->_power_int;
    string insert_date = chara->getInsertDate();
    
    if(sort_param == "insert_date") {
        //入手した時間のタイムスタンプを取得
        struct tm tm;
        strptime(insert_date.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
        tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
        sort_val = mktime(&tm);
    }
    return sort_val;
}
void DeckFormationLayer::createRefineIndexList()
{
    //絞り込みインデックスの作成
    for (int i = 0; i < _chara.size(); i++)
    {
        if (_chara_type[(int)_chara[i]->getCharaType() - 1] && _chara_rarity[fromString<int>(_chara[i]->getRare()) - 1])
        {
            _refine_index_list.push_back(i);
        }
    }
    
    //表示する行数の設定
    _row_count = (_refine_index_list.size() / ROW_1_CHARA_COUNT) + 1;
}

void DeckFormationLayer::createCharaPosIndex()
{
    //キャラのポジションインデックスの作成
    for (int i = 0; i < _row_count; i++)
    {
        for (int j = 0; j < ROW_1_CHARA_COUNT; j++)
        {
            _chara_pos_index_list.push_back(calcPositionChara(i, j));
        }
    }
    
    //    Utility::createPosisionIndex(_chara_pos_index_list, countJsonData(_chara_data_list) / 5 + 1, _column_count, 89, 450, 115, 116);
}

Point DeckFormationLayer::calcPositionChara(int i, int j)
{
    float x, y;
    
    x = (POS_X_FIRST * ratio) + ((POS_X_DISTANCE * ratio) * j);
    y = (POS_Y_DISTANCE * ratio) * (_row_count - i) - (POS_Y_DISTANCE * ratio / 2);
    
    return Point(x, y);
}

void DeckFormationLayer::createScrollView()
{
    //スクロールビューの作成
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, PAGE_BAR_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (PAGE_BAR_SIZE * ratio) - (DECK_BG_SIZE * ratio) - (SORT_BTN_SIZE * ratio)));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, Z_ScrollView, T_ScrollView);
    
    //スクロールの中身を作成
    pScrollView->setContainer(createScrollList());
    
    //バーを表示
    bar = ScrollBarLayer::create(pScrollView);
}


void DeckFormationLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, (POS_Y_DISTANCE * ratio) * _row_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - (((POS_Y_DISTANCE * ratio) * _row_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + PAGE_BAR_SIZE * ratio + DECK_BG_SIZE * ratio + SORT_BTN_SIZE * ratio)))));
    
    //バーを表示
    bar->refresh();
}

Layer* DeckFormationLayer::createScrollList()
{
    //スクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    layer->setTag(T_ScrollLayer);
    
    for (int i = 0; i < _chara.size(); i++)
    {
        //見えない状態でレイヤーにセット
        _chara[i]->setVisible(false);
        _chara[i]->setScale(ratio);
        layer->addChild(_chara[i], Z_FadeOut_Chara);
    }
    
    return layer;
}

void DeckFormationLayer::showCharaSprite()
{
    //キャラの初期表示
    for (int i = 0; i < _refine_index_list.size(); i++)
    {
        bool visible_flg = false;
        if(_refine_index_list[i]<_view_count_default) visible_flg = true;
        _chara[_refine_index_list[i]]->setVisible(visible_flg);
        _chara[_refine_index_list[i]]->setPosition(_chara_pos_index_list[i]);
        _chara[_refine_index_list[i]]->setZOrder(Z_FadeIn_Chara);
    }
}

void DeckFormationLayer::createPageBar()
{
    //画面下部のページバーの背景
    _page_bar = Sprite::create("init/CharaMenu/revision/page2.png");
    _page_bar->setScale(ratio);
    _page_bar->setAnchorPoint(Point(0.5, 0));
//    _page_bar->setPosition(convertFromDisplaySize(Point(320, 130), 0.5, 0));
    _page_bar->setPosition(convertFromDisplaySize(Point(320, -10), 0.5, 0));
    addChild(_page_bar, Z_Page_Bar, T_Page_Bar);
    
    //リセットボタン
    auto reset_off = Sprite::create("init/CharaMenu/revision/bt_reset.png");
    auto reset_on  = Sprite::create("init/CharaMenu/revision/bt_reset.png");
    reset_on->setColor(Color3B(150, 150, 150));
    
    auto reset_btn = MenuItemSprite::create(reset_off, reset_on, [this](Ref* sender){
        
        //デッキステータスの変更
        _deck_status = DeckSetStatus::Remove;
        
        //キャラ選択解除
        initCharaSelect();
        
        //カード枚数のカウントを初期化
        updateSelectedNumber(CharaSprite::CharaType::ALL, DeckSetStatus::Initialize);
        
        //決定ボタンの色を暗くする
        auto ok_btn = (MenuItemSprite *)this->getChildByTag(T_Page_Bar)->getChildByTag(T_Menu)->getChildByTag(T_Ok_MenuItem);
        ok_btn->selected();
        
        //解除音
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_reset.mp3");
    });
    reset_btn->setTag(T_Reset_MenuItem);
    
    //決定ボタン
    auto ok_off = Sprite::create("init/CharaMenu/revision/bt_decision2.png");
    auto ok_on  = Sprite::create("init/CharaMenu/revision/bt_decision2.png");
    ok_on->setColor(Color3B(150, 150, 150));
    
    auto ok_btn = MenuItemSprite::create(ok_off, ok_on, [this](Ref* sender){
        
        if (checkDeckListNullCount() > 0)
        {
            //20枚選択されていない（nullptrが見つかった）とき、選択を促すポップアップ表示
            auto ok_func = [](){}; //何もしない
            auto popup_layer = PopupOkButtonLayer::create(ok_func, "デッキは20枚で編成を行って下さい。");
            this->addChild(popup_layer, 999999);
            
            //押されたボタンを暗いままにする
            auto ok_btn = (MenuItemSprite *)sender;
            ok_btn->selected();
        }
        else {
            
            //最終確認画面の表示
            auto popup_layer = DeckFormationConfirmLayer::create(_deck_chara_list);
            this->addChild(popup_layer, 9999999);
        }
    });
    ok_btn->setTag(T_Ok_MenuItem);
    ok_btn->selected(); //押せるけど警告を出すようにする
    
    auto menu = Menu::create(reset_btn, ok_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(80);
//    menu->setPosition(Point(_page_bar->getContentSize().width / 2, 58));
     menu->setPosition(Point(_page_bar->getContentSize() / 2));
    _page_bar->addChild(menu, Z_Menu, T_Menu);
}

void DeckFormationLayer::setRarityButtonAndTypeButtonData()
{
    //絞り込みボタンの選択状態のデータ設定
    
    //レア度ボタンの設定
    setRarityButtonStatus();
    
    //キャラタイプボタンの設定
    setTypeButtonStatus();
}

void DeckFormationLayer::setRarityButtonStatus()
{
    //レア度ボタンの設定(UR+が_chara_rarityにいる為rarity_menu->getChildrenCount()に+1している！！)
    auto rarity_menu = (Menu *)this->getChildByTag(T_Rarity_Menu);
    for (int i = 0; i < rarity_menu->getChildrenCount() + 1; i++)
    {
        //絞り込み条件に当てはまるものだけ選択状態にする（UR+のi=4だけ入らないので注意!!）
        auto toggle = (MenuItemToggle *)rarity_menu->getChildByTag(T_Normal_Btn + i);
        if (toggle) {
            
            if (_chara_rarity[i]) {
                toggle->setSelectedIndex(1);
            }
            else {
                toggle->setSelectedIndex(0);
            }
        }
    }
}

void DeckFormationLayer::setTypeButtonStatus()
{
    //キャラタイプボタンの設定
    auto type_menu = (Menu *)this->getChildByTag(T_Type_Menu);
    for (int i = 0; i < type_menu->getChildrenCount(); i++)
    {
        auto toggle = (MenuItemToggle *)type_menu->getChildByTag(T_Offense_Btn + i);
        if (_chara_type[i])
        {
            toggle->setSelectedIndex(1);
        }
        else {
            toggle->setSelectedIndex(0);
        }
    }
}

void DeckFormationLayer::setDeckData()
{
    //現在選択中のデッキデータを設定する
    
    //デッキ使用IDリストの作成
    HmsVector<string > deck_used_chara_list;
    for (int i = 0; i < MAX_DECK_CHARA_COUNT; i++)
    {
        //キャラリストIDを取得
        deck_used_chara_list.push_back(_user_party_list[toString<int>(_deck_no - 1)]["chara_list_id" + toString<int>(i + 1)].string_value());
    }
    
    //検索、選択状態に設定
    int num = 0;
    for (int i = 0; i < _chara.size(); i++)
    {
        auto found = find(deck_used_chara_list.begin(), deck_used_chara_list.end(), _chara[i]->getCharaListId());
        
        if (found != deck_used_chara_list.end())
        {
            //あった場合
            
            //要素番号の取得
            int element_number = (int)(found - deck_used_chara_list.begin());
            
            //要素を代入
            _deck_chara_list.set(_chara[i], element_number);
            
            //キャラを選択状態にする
            createNumberCircle(element_number + 1, _chara[i]);

            //デッキスプライトに設定
            setSelectedCharaToDeckSprite(element_number, _chara[i]->getCharaId());
            
            //カード枚数の更新
            updateSelectedNumber(_chara[i]->getCharaType(), DeckSetStatus::Add);
            
            //カードが選択されて同一カードが制限枚数になった時、同じカードを選択できないようにする
            if (checkCharaHaveNumber(_chara[i]) == fromString<int>(_chara[i]->getLimitCount())) setUsedCharaSprite(_chara[i]);
            
            //デッキリストが全て埋まった場合
            if(checkDeckListNullCount() == 0)
            {
                //色を反転
                turnColorSelect();
                
                //決定ボタンの色を明るくする
                auto ok_btn = (MenuItemSprite *)this->getChildByTag(T_Page_Bar)->getChildByTag(T_Menu)->getChildByTag(T_Ok_MenuItem);
                ok_btn->unselected();
            }

            
            num++;
        }
    }
}

void DeckFormationLayer::selectRarity(cocos2d::Ref *sender)
{
    auto menuItem = (MenuItemToggle *)sender;
    
    //選択状態が全て同じ状態又からボタンを押されたとき、そのボタンだけを選択状態にする
    int selected_count = countCharaRaritySelected();
    if (selected_count == sizeof(_chara_rarity) / sizeof(_chara_rarity[0]))
    {
        //全ての選択状態を初期化（後につづく処理で選択状態になる）
        initCharaRarity(ButtonStatus::All_Unselected);
    }
    
    switch (menuItem->getTag())
    {
        case T_Normal_Btn: _chara_rarity[0] = changeBoolean(_chara_rarity[0]); break;
        case T_Rare_Btn:   _chara_rarity[1] = changeBoolean(_chara_rarity[1]); break;
        case T_Super_Btn:  _chara_rarity[2] = changeBoolean(_chara_rarity[2]); break;
        case T_Ultra_Btn:
        {
            _chara_rarity[3] = changeBoolean(_chara_rarity[3]);
            _chara_rarity[4] = changeBoolean(_chara_rarity[4]);
        } break;
        case T_Platinum_Btn: _chara_rarity[5] = changeBoolean(_chara_rarity[5]); break;
        default: break;
    }
    
    //全部解除状態であった場合、全選択にする
    if (countCharaRaritySelected() == 0) initCharaRarity(ButtonStatus::All_Selected);
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    //並び替え
    refreshSortPage(_chara_type);
}

void DeckFormationLayer::selectType(cocos2d::Ref *sender)
{
    auto menuItem = (MenuItemToggle *)sender;
    
    //選択状態が全て同じ状態又は１つだけの状態の同じボタンを押されたとき、そのボタンだけを選択状態にする
    int selected_count = countCharaTypeSelected();
    if (selected_count == (sizeof(_chara_type) / sizeof(_chara_type[0]) - 1))
    {
        //全ての選択状態を初期化（後につづく処理で選択状態になる）
        initCharaType(ButtonStatus::All_Unselected);
    }
    
    switch (menuItem->getTag())
    {
        case T_Offense_Btn:   _chara_type[0] = changeBoolean(_chara_type[0]); break;
        case T_Defense_Btn:   _chara_type[1] = changeBoolean(_chara_type[1]); break;
        case T_Stratege_Btn:  _chara_type[2] = changeBoolean(_chara_type[2]); break;
        default: break;
    }
    
    //全部解除状態であった場合、全選択にする
    if (countCharaTypeSelected() == 0) initCharaType(ButtonStatus::All_Selected);
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    //並び替え
    refreshSortPage(_chara_type);
}

bool DeckFormationLayer::changeBoolean(bool status)
{
    //引数のboolと反対のboolを返す
    if (status) return false;
    else return true;
}

int DeckFormationLayer::countCharaRaritySelected()
{
    //キャラレアリティの選択状態の数（trueの数）を返す
    int selected_count  = 0;
    
    for (int i = 0; i < sizeof(_chara_rarity) / sizeof(_chara_rarity[0]); i++)
    {
        if (_chara_rarity[i]) selected_count++;
    }
    
    return selected_count;
}

void DeckFormationLayer::initCharaRarity(ButtonStatus status)
{
    //キャラレアリティ状態を初期化
    for (int i = 0; i < sizeof(_chara_rarity) / sizeof(_chara_rarity[0]); i++)
    {
        switch (status)
        {
            case ButtonStatus::All_Selected:   _chara_rarity[i] = true;  break;
            case ButtonStatus::All_Unselected: _chara_rarity[i] = false; break;
            default: break;
        }
    }
}

int DeckFormationLayer::countCharaTypeSelected()
{
    //キャラタイプの選択状態の数（trueの数）を返す
    int selected_count  = 0;
    
    for (int i = 0; i < (sizeof(_chara_type) / sizeof(_chara_type[0]) - 1);     i++)
    {
        if (_chara_type[i]) selected_count++;
    }
    
    return selected_count;
}

void DeckFormationLayer::initCharaType(ButtonStatus status)
{
    //キャラタイプ状態を初期化
    for (int i = 0; i < (sizeof(_chara_type) / sizeof(_chara_type[0]) - 1) ; i++)
    {
        switch (status)
        {
            case ButtonStatus::All_Selected:   _chara_type[i] = true;  break;
            case ButtonStatus::All_Unselected: _chara_type[i] = false; break;
            default: break;
        }
    }
}

void DeckFormationLayer::refreshSortPage(bool* chara_type)
{
    //キャラの並び順を変更する
    
    //データの初期化
    initSortData(chara_type);
    
    //キャラの並び替え
    sortCharaData();
    
    //キャラの絞り込み
    createRefineIndexList();
    
    //表示キャラのポジションインデックスを作成
    createCharaPosIndex();
    
    //スクロールデータの更新
    setScrollViewData();
    
    //絞り込みボタンのデータの設定
    setRarityButtonAndTypeButtonData();
    
    //並び替えアクション
    actionSort();
    
    //ソートボタン文字列の変更
    auto sort_menu = (MenuItemSprite *)this->getChildByTag(T_SortBtn_Menu)->getChildByTag(T_SortBtn_MenuItem);
    
    auto label_off = (Label *)sort_menu->getNormalImage()->getChildByTag(T_SortOff_Text);
    label_off->setString(createSortText());
    
    auto label_on = (Label *)sort_menu->getSelectedImage()->getChildByTag(T_SortOn_Text);
    label_on->setString(createSortText());
    
    
    this->unschedule(schedule_selector(DeckFormationLayer::showCharaSpriteInterval));
    DeckFormationLayer::viewCount = 0;
    this->schedule(schedule_selector(DeckFormationLayer::showCharaSpriteInterval));
    
}

void DeckFormationLayer::actionSort()
{
    //ボタンのタッチを無効にする
    setEnableMenuButton(false);
    
    //表示、非表示の切り替え
    auto fade_func = CallFunc::create([this](){
        for (int i = 0; i < _chara.size(); i++)
        {
            auto found = find(_refine_index_list.begin(), _refine_index_list.end(), i);
            
            if (found != _refine_index_list.end())
            {
                //含まれている
                auto fadeIn = FadeIn::create(0.2f);
                _chara[i]->runAction(fadeIn);
                
                //ZOrderの切り替え（手前に）
                _chara[i]->setZOrder(Z_FadeIn_Chara);
                
            }
            else {
                //含まれていない
                auto fadeOut = FadeOut::create(0.2f);
                _chara[i]->runAction(fadeOut);
                
                //ZOrderの切り替え（後ろに）
                _chara[i]->setZOrder(Z_FadeOut_Chara);
            }
        }
    });
    
    //並び替えアクション
    auto move_func = CallFunc::create([this](){
        for (int i = 0; i < _refine_index_list.size(); i++)
        {
            if(!_chara[_refine_index_list[i]]->isVisible())
            {
                bool visible_flg = false;
                if(_refine_index_list[i]<_view_count_default) visible_flg = true;
                _chara[_refine_index_list[i]]->setVisible(visible_flg);
            }
            
            auto move = MoveTo::create(0.3f, _chara_pos_index_list[i]);
            _chara[_refine_index_list[i]]->runAction(move);
        }
        this->unschedule(schedule_selector(DeckFormationLayer::showCharaSpriteInterval));
        DeckFormationLayer::viewCount = 0;
        this->schedule(schedule_selector(DeckFormationLayer::showCharaSpriteInterval));
    });
    
    auto touch_enable = CallFunc::create([this](){
        
        //ボタンのタッチを有効にする
        setEnableMenuButton(true);
    });
    
    this->runAction(Sequence::create(fade_func, DelayTime::create(0.2f), move_func, DelayTime::create(0.3f), touch_enable, nullptr));
}

void DeckFormationLayer::setDataReturnConfirmLayer(HmsVector<CharaSprite *> &deck_chara_list)
{
    //データのリセット
    initCharaSelect();
    
    //カード枚数のカウントを初期化
    updateSelectedNumber(CharaSprite::CharaType::ALL, DeckSetStatus::Initialize);

    //データの設定
    for (int i = 0; i < deck_chara_list.size(); i++)
    {
        createDeckCharaList(deck_chara_list[i]);
    }
}

void DeckFormationLayer::setRecommendData(int offense_number, int defense_number, int strategist_number)
{
    //データのリセット
    initCharaSelect();
    
    //カード枚数のカウントを初期化
    updateSelectedNumber(CharaSprite::CharaType::ALL, DeckSetStatus::Initialize);
    
    //レア度の高い戦闘力順にデータをソートする
    HmsVector<CharaSprite *> recommend_chara = _chara; //おすすめ用に_charaデータをコピーしてソートする
    sort(recommend_chara.begin(), recommend_chara.end(), [](const CharaSprite* chara1, const CharaSprite* chara2){
        return fromString<int>(chara1->getRare()) * pow(10.0, 3.0) + fromString<int>(chara1->getPower()) > fromString<int>(chara2->getRare()) * pow(10.0, 3.0) + fromString<int>(chara2->getPower());
    });
    
    //データの作成、設定
    int offense = 0, defense = 0, strategist = 0;
    for (int i = 0; i < recommend_chara.size(); i++)
    {
        switch (recommend_chara[i]->getCharaType())
        {
            case CharaSprite::OFFENSE:
            {
                if (offense < offense_number && !recommend_chara[i]->getDeckUsedFlg())
                {
                    createDeckCharaList(recommend_chara[i]);
                    offense++;
                }
            }break;
            case CharaSprite::DEFENSE:
            {
                if (defense < defense_number && !recommend_chara[i]->getDeckUsedFlg())
                {
                    createDeckCharaList(recommend_chara[i]);
                    defense++;
                }
            }break;
            case CharaSprite::STRATEGIST:
            {
                if (strategist < strategist_number && !recommend_chara[i]->getDeckUsedFlg())
                {
                    createDeckCharaList(recommend_chara[i]);
                    strategist++;
                }
            }break;
            default: break;
        }
        
        if ((offense + defense + strategist) == 20) break;
    }
    
    //もし合計が20枚に達していなかった場合、ソートした順の含まれていないものを20枚になるように追加していく
    int null_count = checkDeckListNullCount();
    if (null_count > 0)
    {
        for (int i = 0; i < recommend_chara.size(); i++)
        {
            auto found = find(_deck_chara_list.begin(), _deck_chara_list.end(), recommend_chara[i]);
            
            if (found == _deck_chara_list.end() && !recommend_chara[i]->getDeckUsedFlg() && recommend_chara[i]->getCharaType() != CharaSprite::GENERAL)
            {
                //含まれていない場合、追加
                createDeckCharaList(recommend_chara[i]);
                null_count--;
            }
            
            //20枚選択状態になったとき抜ける
            if (null_count <= 0) break;
        }
    }
    
}

void DeckFormationLayer::createDeckCharaList(CharaSprite* chara)
{
    //デッキキャラリストの作成
    
    //同一のキャラチェック
    auto chara_it = find(_deck_chara_list.begin(), _deck_chara_list.end(), chara);
    
    if (chara_it != _deck_chara_list.end())
    {
        //デッキリストにあるので削除（削除の代わりにnullptrを入れる）
        *chara_it = nullptr;
        
        //デッキステータスの変更
        _deck_status = DeckSetStatus::Remove;
        
        //要素番号の取得
        int element_number = (int)(chara_it - _deck_chara_list.begin());
        
        //選択状態を解除
        removeNumberCircle(element_number + 1, chara);
        
        //解除キャラをデッキスプライトからも解除する
        removeSelectedCharaToDeckSprite(element_number);
        
        //カード枚数の更新
        updateSelectedNumber(chara->getCharaType(), DeckSetStatus::Remove);
        
        //カードが削除されて制限枚数より１枚少ない状態になった時、同一で選択できなかった状態を解除する
        if (checkCharaHaveNumber(chara) == fromString<int>(chara->getLimitCount()) - 1) removeUsedCharaSprite(chara);

        //削除してデッキリスト内のnullptrが1つになったとき
        if (checkDeckListNullCount() == 1)
        {
            turnColorSelect();
            
            //解除して明るくなったものを又反転して暗くしてしまうので対策としてここで戻す
            chara->setColor(Color3B(255, 255, 255));
            
            //決定ボタンの色を暗くする
            auto ok_btn = (MenuItemSprite *)this->getChildByTag(T_Page_Bar)->getChildByTag(T_Menu)->getChildByTag(T_Ok_MenuItem);
            ok_btn->selected();
        }
        
    }
    else {
        
        //空(nullptr)チェック
        auto null_it  = find(_deck_chara_list.begin(), _deck_chara_list.end(), nullptr);
        
        if (null_it != _deck_chara_list.end())
        {
            //デッキリストに代入
            *null_it = chara;
            
            //デッキステータスの変更
            _deck_status = DeckSetStatus::Add;
            
            //要素番号の取得
            int element_number = (int)(null_it - _deck_chara_list.begin());
            
            //キャラを選択状態にする
            createNumberCircle(element_number + 1, chara);
            
            //選択したキャラを画面上部のデッキスプライトに設定する
            setSelectedCharaToDeckSprite(element_number, chara->getCharaId());
            
            //カード枚数の更新
            updateSelectedNumber(chara->getCharaType(), DeckSetStatus::Add);
            
            //カードが選択されて同一カードが制限枚数になった時、同じカードを選択できないようにする
            if (checkCharaHaveNumber(chara) == fromString<int>(chara->getLimitCount())) setUsedCharaSprite(chara);

            //デッキリストが全て埋まった場合
            if(checkDeckListNullCount() == 0)
            {
                //色を反転
                turnColorSelect();
                
                //決定ボタンの色を明るくする
                auto ok_btn = (MenuItemSprite *)this->getChildByTag(T_Page_Bar)->getChildByTag(T_Menu)->getChildByTag(T_Ok_MenuItem);
                ok_btn->unselected();
            }
        }
        else
        {
            //空が無い場合はdeck_chara_listがいっぱいなので何もしない
        }
    }
    
    //ログ出力
    for (int i = 0; i < 20; i++)
    {
        if (_deck_chara_list[i] != nullptr) {
            CCLOG("deck_chara_list_id[%d] = %s", i, _deck_chara_list[i]->getCharaListId().c_str());
        }
    }
    
    CCLOG("vec size = %d", _deck_chara_list.size());
}

void DeckFormationLayer::createNumberCircle(int value, CharaSprite* chara)
{
    //選択キャラを暗くする
    chara->setColor(Color3B(150, 150, 150));
    
    //選択ナンバーをノードの上につける(暗くする処理を適用させないためノードの上に乗せる)
    auto chara_node = chara->getChildByTag(CharaSprite::T_Node);
    
    auto circle = Sprite::create("init/CharaMenu/num/card_num" + toString<int>(value) + ".png");
    circle->setPosition(Point(chara_node->getContentSize() / 2));
    chara_node->addChild(circle, Z_Circle, T_Circle);
    
    if (value == 1) {
        //選択番号1番のみフレンドマークを表示
        auto friend_sp = Sprite::create("init/CharaMenu/revision/friend.png");
        friend_sp->setPosition(Point(chara_node->getContentSize().width / 2, chara_node->getContentSize().height / 2 + 30));
        chara_node->addChild(friend_sp, Z_Friend, T_Friend);
    }
}

void DeckFormationLayer::removeNumberCircle(int value, CharaSprite* chara)
{
    //キャラを通常の色に戻す
    chara->setColor(Color3B(255, 255, 255));
    
    //キャラの番号を取り除く
    chara->getChildByTag(CharaSprite::T_Node)->removeChildByTag(T_Circle);
    
    //１番だった場合、フレンドマークも取り除く
    if (value == 1) chara->getChildByTag(CharaSprite::T_Node)->removeChildByTag(T_Friend);
}

void DeckFormationLayer::turnColorSelect()
{
    //選択色を反転させる
    for (int i = 0; i < _chara.size(); i++) {
        if (_chara[i]->getColor() == Color3B(150, 150, 150) && !_chara[i]->getDeckUsedFlg())
        {
            //暗かったら明るくする
            _chara[i]->setColor(Color3B(255, 255, 255));
        }
        else {
            //明るかったら暗くする
            _chara[i]->setColor(Color3B(150, 150, 150));
        }
        
    }
}

int DeckFormationLayer::checkDeckListNullCount()
{
    //デッキリストのnullptr数を調べる
    int null_count = 0;
    for (int i = 0; i < _deck_chara_list.size(); i++) {
        if (_deck_chara_list[i] == nullptr) null_count++;
    }
    
    return null_count;
}

void DeckFormationLayer::setSelectedCharaToDeckSprite(int element_number, string chara_id)
{
    //選択したキャラをデッキスプライトに設定する
    _deck_chara[element_number]->setTexture("download/Chara/thumb100/" + chara_id + ".png");
    
    //セットする画像が大きいのでdeck_emptyの54×54のサイズに統一する処理
    Size imgSize = _deck_chara[element_number]->getContentSize();
    _deck_chara[element_number]->setScale(54 / imgSize.width);
    
    //タグの設定
    _deck_chara[element_number]->setTag(T_Deck_Chara);
    
    //1番の場合(要素番号だと0)フレンドマークをつける
    if (element_number == 0) {
        auto friend_sp = Sprite::create("init/CharaMenu/revision/friend.png");
        friend_sp->setPosition(Point(58, 137));
        _deck_bg->addChild(friend_sp, Z_Deck_Friend_Sprite, T_Deck_Friend_Sprite);
    }
}

void DeckFormationLayer::removeSelectedCharaToDeckSprite(int element_number)
{
    //デッキ選択画面から画像を取り除く（選択していない状態に戻す）
    _deck_chara[element_number]->setTexture("init/CharaMenu/revision/deck_empty.png");
    
    //セットしたときにスプライトを小さくしたので戻す
    _deck_chara[element_number]->setScale(1.0f);
    
    //タグの設定
    _deck_chara[element_number]->setTag(T_Deck_Empty_Chara);
    
    //1番の場合(要素番号だと0)フレンドマークを取り除く
    if (element_number == 0) _deck_bg->removeChildByTag(T_Deck_Friend_Sprite);
}

void DeckFormationLayer::updateSelectedNumber(CharaSprite::CharaType type, DeckSetStatus status)
{
    //増減ポイント
    int point = 1;
    if (status == DeckSetStatus::Remove) point = -1;
    
    //タイプ数の更新
    switch (type)
    {
        case CharaSprite::CharaType::OFFENSE:    _offense += point;  break;
        case CharaSprite::CharaType::DEFENSE:    _defense += point;  break;
        case CharaSprite::CharaType::STRATEGIST: _stratege += point; break;
        default: break;
    }
    
    //合計数の更新
    _total_number += point;
    
    //初期化する場合DeckSetStatusをInitializeで送る
    if (status == DeckSetStatus::Initialize) {
        _total_number = 0;
        _offense = 0;
        _defense = 0;
        _stratege = 0;
    }
    
    //ラベルの更新
    auto total_label = (Label * )_deck_bg->getChildByTag(T_Total_Label);
    auto offense_label = (Label *)_deck_bg->getChildByTag(T_Offense_Label);
    auto defense_label = (Label *)_deck_bg->getChildByTag(T_Defense_Label);
    auto strategist_label = (Label *)_deck_bg->getChildByTag(T_Strategist_Label);
    
    total_label->setString(toString<int>(getTotalNumber()) + "/" + toString<int>(MAX_DECK_CHARA_COUNT));
    offense_label->setString(toString<int>(getOffense()) + "枚");
    defense_label->setString(toString<int>(getDefense()) + "枚");
    strategist_label->setString(toString<int>(getStratege()) + "枚");
}

void DeckFormationLayer::initCharaSelect()
{
    //全ての選択状態を解除する
    for (int i = 0; i < _deck_chara_list.size(); i++)
    {
        //デッキリストを初期化する
        _deck_chara_list.set(nullptr, i);

    }
    
    //色を戻す、ノード上にのっているマークを全て取り除く
    for (int i = 0; i < _chara.size(); i++)
    {
        _chara[i]->setColor(Color3B(255, 255, 255));
        _chara[i]->setDeckUsedFlg(false);
        
        auto node = _chara[i]->getChildByTag(CharaSprite::T_Node);
        node->removeAllChildren();
    
    }
    
    //デッキ選択画像を初期化する
    for (int i = 0; i < _deck_chara.size(); i++)
    {
        removeSelectedCharaToDeckSprite(i);
    }
}

int DeckFormationLayer::checkCharaHaveNumber(CharaSprite* chara)
{
    //同一カードが3枚以上選択されているかチェック
    HmsVector<string> check_list;
    for (int i = 0; i < _deck_chara_list.size(); i++)
    {
        if (_deck_chara_list[i] != nullptr) check_list.push_back(_deck_chara_list[i]->getCharaId());
    }
    
    int count = (int)std::count(check_list.begin(), check_list.end(), chara->getCharaId().c_str());
    count += (int)std::count(check_list.begin(), check_list.end(), chara->getPrevCharaId().c_str());
    count += (int)std::count(check_list.begin(), check_list.end(), chara->getNextCharaId().c_str());
    
    return count;
}

void DeckFormationLayer::setUsedCharaSprite(CharaSprite *chara)
{
    //同一カードが3枚選択された場合、選択できないようにする
    for (int i = 0; i < _chara.size(); i++)
    {
        auto circle = _chara[i]->getChildByTag(CharaSprite::T_Node)->getChildByTag(T_Circle);
        
        if ((_chara[i]->getCharaId() == chara->getCharaId() || _chara[i]->getPrevCharaId() == chara->getCharaId() || _chara[i]->getNextCharaId() == chara->getCharaId()) && circle == nullptr)
        {
            _chara[i]->setDeckUsedFlg(true);
            _chara[i]->setColor(Color3B(150, 150, 150));
            
            //使用中文字とアクション
            auto chara_node = _chara[i]->getChildByTag(CharaSprite::T_Node);
            
            auto used_label = HMSFontUtil::createWithTTFBySystemFontArgument("上限枚数", FONT_NAME_SYSTEM, 24);
            used_label->setTextColor(Color4B(255, 100, 100, 255));
            used_label->setPosition(Point(chara_node->getContentSize() / 2));
            chara_node->addChild(used_label, Z_Used_Label, T_Used_Label);
            
            auto fadeOut = FadeOut::create(0.8f);
            auto fadeIn  = FadeIn::create(0.4f);
            auto repeat  = RepeatForever::create(Sequence::create(DelayTime::create(1.0f), fadeOut, DelayTime::create(2.5f), fadeIn, nullptr));
            used_label->runAction(repeat);
        }
    }
}

void DeckFormationLayer::removeUsedCharaSprite(CharaSprite *chara)
{
    //3枚以上で選択できなかったものを解除する
    int null_count = checkDeckListNullCount();
    for (int i = 0; i < _chara.size(); i++)
    {
        auto used_label = _chara[i]->getChildByTag(CharaSprite::T_Node)->getChildByTag(T_Used_Label);
        
        if ((_chara[i]->getCharaId() == chara->getCharaId() || _chara[i]->getPrevCharaId() == chara->getCharaId() || _chara[i]->getNextCharaId() == chara->getCharaId()) && used_label != nullptr)
        {
            _chara[i]->setDeckUsedFlg(false);
            used_label->removeFromParent();
            if (null_count != 1) _chara[i]->setColor(Color3B(255, 255, 255)); //削除してnullptrが一つになった時、あとに続くturnColorで又暗くなってしまう為、対策
        }
    }
    
}

void DeckFormationLayer::showSortPopup(Ref* sender)
{
    //並び替えポップアップの表示
    auto popup_layer = CharaListSortPopupLayer::create(_chara_type, "DeckFormation");
    addChild(popup_layer, 999999);
}

void DeckFormationLayer::showCharaDetail(float dt)
{
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(_chara[_touch_chara_index]->getCharaId());
    this->addChild(detail_layer, Z_Confirm_Layer, T_Confirm_Layer);
    
    //タッチデータの初期化
    _deck_touch_flg = false;
    _touch_chara_index = -1;
}

void DeckFormationLayer::setTouchMode(int touch_index)
{
    //キャラをタッチ中の状態にする
    _deck_chara[touch_index]->setScale(1.2f);
    _deck_chara[touch_index]->setOpacity(200);
    _deck_chara[touch_index]->setGlobalZOrder(1);
    
    //元あった場所にからの画像を設定
    auto empty_sp = Sprite::createWithTexture(_deck_empty->getTexture());
    empty_sp->setPosition(_deck_pos_index_list[touch_index]);
    _deck_bg->addChild(empty_sp, Z_Empty, T_Empty);
}

void DeckFormationLayer::setUnTouchMode(int touch_index)
{
    //タッチ中の状態を解除する
    Size imgSize = _deck_chara[touch_index]->getContentSize();
    _deck_chara[touch_index]->setScale(54 / imgSize.width);
    _deck_chara[touch_index]->setGlobalZOrder(0);
    _deck_chara[touch_index]->setOpacity(255);
}

void DeckFormationLayer::changeCharaPosition(int touch_index)
{
    //タッチ先のキャラと場所を変更する
    _deck_chara[_touch_chara_index]->setPosition(_deck_pos_index_list[touch_index]);
    
    //タッチ先のキャラをタッチキャラの場所に移動する
    auto move = MoveTo::create(0.2f, Point(_deck_pos_index_list[_touch_chara_index]));
    auto func = CallFunc::create([this]()
    {
        //元あった場所の空画像を削除
        auto empty_sp = _deck_bg->getChildByTag(T_Empty);
        if (empty_sp != nullptr) empty_sp->removeFromParent();
        
    });
    _deck_chara[touch_index]->runAction(Sequence::create(move, func, nullptr));
    
    //インデックスの入れ替え
    Sprite* tmp = _deck_chara[_touch_chara_index];
    _deck_chara.set(_deck_chara[touch_index], _touch_chara_index);
    _deck_chara.set(tmp, touch_index);
}

void DeckFormationLayer::changeDeckCharaList(int touch_index)
{
    //デッキキャラリストの内容を入れ替える
    CharaSprite* tmp = _deck_chara_list[_touch_chara_index];
    _deck_chara_list.set(_deck_chara_list[touch_index], _touch_chara_index);
    _deck_chara_list.set(tmp, touch_index);
    
    for (int i = 0; i < _deck_chara_list.size(); i++)
    {
        if (_deck_chara_list[i] != nullptr) CCLOG("deck_chara[%d] = %s", i,_deck_chara_list[i]->getCharaListId().c_str());
    }
}

void DeckFormationLayer::refreshNumbering()
{
    //番号を採番しなおす
    for (int i = 0; i < _deck_chara_list.size(); i++)
    {
        if (_deck_chara_list[i] != nullptr)
        {
            auto chara_node = _deck_chara_list[i]->getChildByTag(CharaSprite::T_Node);
            
            chara_node->removeAllChildren();
            
            auto circle = Sprite::create("init/CharaMenu/num/card_num" + toString<int>(i + 1) + ".png");
            circle->setPosition(Point(chara_node->getContentSize() / 2));
            chara_node->addChild(circle, Z_Circle, T_Circle);
            
            if (i == 0) {
                //選択番号1番（配列番号）のみフレンドマークを表示
                auto friend_sp = Sprite::create("init/CharaMenu/revision/friend.png");
                friend_sp->setPosition(Point(chara_node->getContentSize().width / 2, chara_node->getContentSize().height / 2 + 30));
                chara_node->addChild(friend_sp, Z_Friend, T_Friend);
            }
        }
    }
}

void DeckFormationLayer::setEnableMenuButton(bool boolean)
{
    //並び替えボタンのタッチ切り替え
    auto rarity_menu = (Menu *)this->getChildByTag(T_Rarity_Menu);
    auto type_menu   = (Menu *)this->getChildByTag(T_Type_Menu);
    
    rarity_menu->setEnabled(boolean);
    type_menu->setEnabled(boolean);
}

bool DeckFormationLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckFormationLayer!");
    
    if(scroll_flg==true) return true;
    
    //タッチデータの初期化
    _deck_touch_flg = false;
    _touch_chara_index = -1; //-1で初期化！（0はある為やむえず...）
    
    //デッキスプライトのどのキャラをタッチしたかチェック
    for (int i = 0; i < _deck_chara.size(); i++)
    {
        float scale = _deck_chara[i]->getScale() * ratio;
        Point convertedPos = _deck_chara[i]->convertToWorldSpace(Point::ZERO);
        Rect rect_deck_chara = Rect(convertedPos.x, convertedPos.y, _deck_chara[i]->getContentSize().width * scale, _deck_chara[i]->getContentSize().height * scale);
        
        if (rect_deck_chara.containsPoint(touch->getLocation()) && _deck_chara[i]->getTag() != DeckFormationLayer::T_Deck_Empty_Chara)
        {
            //タッチ中に設定
            setTouchMode(i);

            //画像をタッチ位置より少し上に表示
            _deck_chara[i]->setPosition(Point(_deck_chara[i]->getPosition().x, _deck_chara[i]->getPosition().y + 70));
            
            //タッチしたキャラインデックスを保持
            _touch_chara_index = i;
            
            //デッキタッチフラグの変更
            _deck_touch_flg = true;
            
            //音の設定
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
            
            return true;
        }
    }
    
    //タッチしたキャラデータの取得
    auto scroll_rect = Rect(0, PAGE_BAR_SIZE * ratio, _contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (PAGE_BAR_SIZE * ratio) - (DECK_BG_SIZE * ratio) - (SORT_BTN_SIZE * ratio));
    if (scroll_rect.containsPoint(touch->getLocation()))
    {
        for (int i = 0; i < _refine_index_list.size(); i++)
        {
            if (Utility::containsPoint(_chara[_refine_index_list[i]], touch->getLocation()))
            {
                //選択中のものでもキャラ情報を観れるようにする
                _touch_chara_index = _refine_index_list[i];
                
                break;
            }
        }
    }
    
    //キャラ詳細の表示（長押し0.3秒後）
    if (_touch_chara_index != -1) this->scheduleOnce(schedule_selector(DeckFormationLayer::showCharaDetail), 0.3f);
    
    return true;
}

void DeckFormationLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(scroll_flg==true) return;

    if (_deck_touch_flg == true && _touch_chara_index != -1)
    {
        _deck_chara[_touch_chara_index]->setPosition(Point(_deck_chara[_touch_chara_index]->getPosition() + touch->getDelta() / ratio)); //ratioがかかったものの上に乗っているのでratioで割らなければいけない、注意！
    }
}

void DeckFormationLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(scroll_flg==true) return;
    
    //スケジューラの停止
//    this->unscheduleAllCallbacks();
    this->unschedule(schedule_selector(DeckFormationLayer::showCharaDetail));
    
    //デッキキャラの離した場所判定
    if (_deck_touch_flg == true && _touch_chara_index != -1)
    {
        //タッチ中の状態を解除
        setUnTouchMode(_touch_chara_index);
        
        if (_deck_bg->getBoundingBox().containsPoint(touch->getLocation()))
        {
            //デッキ範囲内であった場合タッチエンドのキャラを取得
            for (int i = 0; i < _deck_chara.size(); i++)
            {
                float scale = _deck_chara[i]->getScale() * ratio;
                Point convertedPos = _deck_chara[i]->convertToWorldSpace(Point::ZERO);
                Rect rect_deck_chara = Rect(convertedPos.x, convertedPos.y, _deck_chara[i]->getContentSize().width * scale, _deck_chara[i]->getContentSize().height * scale);
                
                if (rect_deck_chara.containsPoint(touch->getLocation()) && _deck_chara[i]->getTag() != DeckFormationLayer::T_Deck_Empty_Chara && _touch_chara_index != i)
                {
                    //あった場合
                    
                    //デッキスプライト内の画像の入れ替え
                    changeCharaPosition(i);
                    
                    //データの入れ替え
                    changeDeckCharaList(i);
                    
                    //スクロール上の番号を採番しなおす
                    refreshNumbering();
                    
                    //交換音
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/page.mp3");
                    
                    return;
                }
            }
        }
        else
        {
            //デッキスプライト範囲外であった場合その画像を消す
            createDeckCharaList(_deck_chara_list[_touch_chara_index]);
            
            //音の設定
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_reset.mp3");
        }
        
        //キャラ画像がない、又はEmpty状態だった場合キャラを元の位置に戻す
        _deck_chara[_touch_chara_index]->setPosition(Point(_deck_pos_index_list[_touch_chara_index]));

        //元あった場所の空画像を削除
        auto empty_sp = _deck_bg->getChildByTag(T_Empty);
        if (empty_sp != nullptr) empty_sp->removeFromParent();
        
        return;
    }
    
    //キャラタッチ判定
    if (_touch_chara_index != -1)
    {
        if (!_chara[_touch_chara_index]->getDeckUsedFlg())
        {
            //3枚以上選択中のものは入れない、DeckFormationConfirmLayer
            createDeckCharaList(_chara[_touch_chara_index]);
            
            //音の設定
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        }
    }
}

void DeckFormationLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void DeckFormationLayer::scrollViewDidScroll(ScrollView *view)
{
    //タッチデータの初期化
    _deck_touch_flg = false;
    _touch_chara_index = -1;
    
    //スケジューラの停止
//    this->unscheduleAllCallbacks();
    this->unschedule(schedule_selector(DeckFormationLayer::showCharaDetail));
    
    //バーを表示
    bar->refresh();
    
    scroll_flg = true;
    this->scheduleOnce(schedule_selector(DeckFormationLayer::scrollEnd), 0.1f);
}

void DeckFormationLayer::scrollEnd(float dt)
{
    scroll_flg = false;
}
