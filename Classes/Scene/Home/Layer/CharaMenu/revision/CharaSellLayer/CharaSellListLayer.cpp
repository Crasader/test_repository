#include "CharaSellListLayer.h"
#include "CharaCollectivelySellConfirmPopupLayer.h"

CharaSellListLayer::CharaSellListLayer()
:_chara_data_list(nullptr)
,_user_party_list(nullptr)
,_sell_price(0)
,_select_count(0)
,_chara_count(0)
,_exchange_ticket(0)
,_row_count(0)
,_sort_status(CharaListSortPopupLayer::SortStatus::Power)
,_touch_flg(false)
,_touch_chara(nullptr)
,_view_count_default(0)
{
}

CharaSellListLayer* CharaSellListLayer::create(bool *chara_type, bool *chara_rarity)
{
    CharaSellListLayer *sprite = new CharaSellListLayer();
    
    if (sprite && sprite->init(chara_type, chara_rarity))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaSellListLayer::init(bool *chara_type, bool *chara_rarity)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaSellListLayer::init()");
    
    ratio = getVisibleRatio();
    
    CharaSellListLayer::viewCount = _view_count_default;
    
    //データの初期化
    initData(chara_type, chara_rarity);
    
    return true;
}

void CharaSellListLayer::initData(bool *chara_type, bool *chara_rarity)
{
    //データの初期化
    _chara.clear();
    _refine_index_list.clear();
    _position_index_list.clear();
    _sell_chara_list.clear();
    
    _sell_price = 0;
    _select_count = 0;
    _exchange_ticket = 0;
    _chara_count = 0;
    _row_count   = 0;
    _sort_status = (CharaListSortPopupLayer::SortStatus)SaveData::loadInt("sort_status");
    
    //配列の代入（引数で与えられた配列は配列サイズをsizeofで求められないので注意！）
    for(int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        _chara_type[i] = chara_type[i];
    }
    //レア度絞り込み、キャラタイプの初期化
    for (int i = 0; i < sizeof(_chara_rarity) / sizeof(_chara_rarity[0]); i++)
    {
        _chara_rarity[i] = chara_rarity[i];
    }
}

void CharaSellListLayer::initSortData(bool* chara_type)
{
    //ソート時のデータの初期化
//    _chara.clear(); //キャラデータは初期化してはいけない！
    _refine_index_list.clear();
    _position_index_list.clear();
    _chara_count = 0;
    _row_count   = 0;
    _sort_status = (CharaListSortPopupLayer::SortStatus)SaveData::loadInt("sort_status");
    
    for(int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        _chara_type[i] = chara_type[i];
    }
}

void CharaSellListLayer::createTitleBar(){
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
        //ページ遷移
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("CharaMenu");
    });
    back_btn->setScale(ratio);
    Menu* menu = Menu::create(back_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(58, 788), 0.5, 1));
    this->addChild(menu, Z_Back_Btn, T_Back_Btn);
    
    //ページ名
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("キャラ売却", FONT_NAME_SYSTEM, 32);
    title_label->setScale(ratio);
    title_label->setAnchorPoint(Point(0, 0.5));
    title_label->setPosition(convertFromDisplaySize(Point(111, 788), 0.5, 1));
    title_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    this->addChild(title_label, Z_Title_Label, T_Title_Label);
    
    Sprite* sellOff = Sprite::create("init/CharaMenu/bt_bulk_sale.png");
    Sprite* sellOn  = Sprite::create("init/CharaMenu/bt_bulk_sale.png");
    sellOn->setCascadeColorEnabled(true);
    sellOn->setColor(Color3B(180,180,180));
    
    MenuItemSprite* sellButton = MenuItemSprite::create(sellOff, sellOn, CC_CALLBACK_1(CharaSellListLayer::collectivelySell, this));
    sellButton->setTag(T_SortBtn_MenuItem);
    Menu* sellButtonMenu = Menu::create(sellButton, nullptr);
    sellButtonMenu->setPosition(Point(title_bar->getContentSize().width - sellButton->getContentSize().width / 2, title_bar->getContentSize().height / 2));
    title_bar->addChild(sellButtonMenu, 99999, 99999);
    

}

void CharaSellListLayer::onEnter()
{
    Layer::onEnter();
    
    //タイトルバーの作成
    createTitleBar();
    
    //データの設定
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //並び替えボタンの作成
    createSortButton();
    
    //キャラ画像の生成
    createCharaSprite();
    
    //レア度絞り込みボタンの作成
    createRarityButton();
    
    //属性絞り込みボタンの作成
    createTypeButton();
    
    //絞り込み切り替えボタンの作成
    createSwitchButton();
    
    //キャラの並び替え
    sortCharaData();
    
    //表示キャラの絞り込み
    createRefineIndexList();
    
    //表示キャラのポジションインデックスを作成
    createPositionIndexList();
    
    //画面下、売却ポップアップの作成、表示
    showSellPopup();
    
    //スクロールの作成
    createScrollView();
    
    //スクロールデータの設定
    setScrollViewData();
    
    //デッキ使用中チェック
    checkDeckUsed();
    
    //お気に入り登録チェック
    checkFavoriteChara();
    
    //交換チケットの表示
    showExchageTicket();
    
    //キャラの表示
    showCharaSprite();
    
    //絞り込みボタンのデータの設定
    setRarityButtonAndTypeButtonData();
    
    //キャラ画像の生成
    this->schedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
    
}

void CharaSellListLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CharaSellListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaSellListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaSellListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaSellListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaSellListLayer::setData()
{
    //キャラデータの設定
    string json_text = SaveData::loadString("chara_data_list");
    string err;
    _chara_data_list = Json::parse(json_text, err);
    
    //デッキ使用データの設定
    string json_text2 = SaveData::loadString("user_party_list");
    string err2;
    _user_party_list = Json::parse(json_text2, err2);
}

void CharaSellListLayer::createSortButton()
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
    
    auto sort_btn = MenuItemSprite::create(sortOff, sortOn, CC_CALLBACK_1(CharaSellListLayer::showSortPopup, this));
    sort_btn->setScale(ratio);
    sort_btn->setTag(T_SortBtn_MenuItem);
    auto menu = Menu::create(sort_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(555, 788), 0.5, 1));
    this->addChild(menu, Z_SortBtn_Menu, T_SortBtn_Menu);
}

string CharaSellListLayer::createSortText()
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

void CharaSellListLayer::createCharaSprite()
{
    //キャラ画像の作成
    int count = countJsonData(_chara_data_list);
    for (int i = 0; i < count; i++)
    {
        string iStr = toString(i);
        _chara.push_back(CharaSprite::create(_chara_data_list[iStr].object_items()));
    }
}

void CharaSellListLayer::showCharaSpriteInterval(float f)
{
    HMSLOG("Schedule::");
    if(_refine_index_list.size()<=CharaSellListLayer::viewCount){
        HMSLOG("UNSCHEDULE:Sprite2End");
        unschedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
        return;
    }
    int i = 0;
    while(true){
        if(!_chara[_refine_index_list[CharaSellListLayer::viewCount]]->isVisible()){
            _chara[_refine_index_list[CharaSellListLayer::viewCount]]->setVisible(true);
            //            break;
            i++;
            if(i>=2)
                break;
        }
        CharaSellListLayer::viewCount++;
        if(_refine_index_list.size()<=CharaSellListLayer::viewCount){
            HMSLOG("UNSCHEDULE:Sprite2End2");
            unschedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
            return;
        }
    }
}

void CharaSellListLayer::sortCharaData()
{
    //キャラクターのソート
    switch (_sort_status)
    {
        case CharaListSortPopupLayer::SortStatus::Rare:
        {
            _chara = CharaSprite::sortCharacter(_chara, "rare");
        } break;
        case CharaListSortPopupLayer::SortStatus::NeedCost:
        {
            _chara = CharaSprite::sortCharacter(_chara, "cost");
        } break;
        case CharaListSortPopupLayer::SortStatus::InsertDate:
        {
            _chara = CharaSprite::sortCharacter(_chara, "insert_date");
        } break;
        default:
        {
            _chara = CharaSprite::sortCharacter(_chara, "power");
        }break;
    }
}

//HmsVector<CharaSprite *> CharaSellListLayer::sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param)
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

long CharaSellListLayer::getSortVal(CharaSprite *chara, string sort_param)
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
void CharaSellListLayer::createRefineIndexList()
{
    //絞り込みインデックスの作成（表示するキャラの選抜）
    for (int i = 0; i < _chara.size(); i++)
    {
        string iStr = toString<int>(i);
        if (_chara_type[_chara[i]->getCharaType() - 1] && _chara_rarity[fromString<int>(_chara[i]->getRare()) - 1])
        {
            //絞り込み
            _refine_index_list.push_back(i);
        }
    }
    
    //表示するキャラ数と表示行数の取得
    _chara_count = _refine_index_list.size();
    _row_count = (_refine_index_list.size() / ROW_1_CHARA_COUNT) + 1;
}

void CharaSellListLayer::createPositionIndexList()
{
    //ポジションインデックスの作成
    for (int i = 0; i < _row_count; i++)
    {
        for (int j = 0; j < ROW_1_CHARA_COUNT; j++)
        {
            _position_index_list.push_back(calcPositionChara(i, j));
        }
    }
}

void CharaSellListLayer::createScrollView()
{
    //スクロールビューの作成
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE + SELL_BG_HEIGHT), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio) - (SELL_BG_HEIGHT * ratio) - (SORT_BTN_SIZE * ratio)));
    
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


void CharaSellListLayer::createRarityButton()
{
    //レア度絞り込みボタン
    
    //N
    auto normal_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray4.png", "init/CharaMenu/revision/bt_gray4.png");
    auto normal_on  = MenuItemImage::create("init/CharaMenu/revision/bt4.png", "init/CharaMenu/revision/bt4.png");
    auto normal_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectRarity, this), normal_off, normal_on, nullptr);
    normal_btn->setTag(T_Normal_Btn);
    normal_btn->setScale(ratio);
    
    //R
    auto rare_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray5.png", "init/CharaMenu/revision/bt_gray5.png");
    auto rare_on  = MenuItemImage::create("init/CharaMenu/revision/bt5.png", "init/CharaMenu/revision/bt5.png");
    auto rare_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectRarity, this), rare_off, rare_on, nullptr);
    rare_btn->setTag(T_Rare_Btn);
    rare_btn->setScale(ratio);
    
    //SR
    auto super_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray6.png", "init/CharaMenu/revision/bt_gray6.png");
    auto super_on  = MenuItemImage::create("init/CharaMenu/revision/bt6.png", "init/CharaMenu/revision/bt6.png");
    auto super_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectRarity, this), super_off, super_on, nullptr);
    super_btn->setTag(T_Super_Btn);
    super_btn->setScale(ratio);
    
    //UR
    auto ultra_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray7.png", "init/CharaMenu/revision/bt_gray7.png");
    auto ultra_on  = MenuItemImage::create("init/CharaMenu/revision/bt7.png", "init/CharaMenu/revision/bt7.png");
    auto ultra_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectRarity, this), ultra_off, ultra_on, nullptr);
    ultra_btn->setTag(T_Ultra_Btn);
    ultra_btn->setScale(ratio);
    
    //PT
    auto platinum_off = MenuItemImage::create("init/CharaMenu/revision/bt_gray8.png", "init/CharaMenu/revision/bt_gray8.png");
    auto platinum_on  = MenuItemImage::create("init/CharaMenu/revision/bt8.png", "init/CharaMenu/revision/bt8.png");
    auto platinum_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectRarity, this), platinum_off, platinum_on, nullptr);
    platinum_btn->setTag(T_Platinum_Btn);
    platinum_btn->setScale(ratio);
    
    auto menu = Menu::create(normal_btn, rare_btn, super_btn, ultra_btn, platinum_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(4);
    menu->setPosition(convertFromDisplaySize(Point(250, 715), 0.5, 1.0));
    this->addChild(menu, Z_Rarity_Menu, T_Rarity_Menu);
}

void CharaSellListLayer::createTypeButton()
{
    //属性絞り込みボタン
    
    float scale = 0.9;
    float scaleX = 0.6;
    
    //攻撃選択
    auto bt_offense_off = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    auto bt_offense_on = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    bt_offense_off->setScale(ratio * scale);
    bt_offense_off->setScaleX(ratio * scaleX);
    bt_offense_off->setContentSize(Size(bt_offense_off->getContentSize().width * scaleX * ratio, bt_offense_off->getContentSize().height));
    bt_offense_off->setColor(Color3B(180, 180, 180));
    
    bt_offense_on->setScale(ratio * scale);
    bt_offense_on->setScaleX(ratio * scaleX);
    bt_offense_on->setContentSize(Size(bt_offense_on->getContentSize().width * scaleX * ratio, bt_offense_on->getContentSize().height));
    
    
    auto offense_off = MenuItemImage::create("init/CharaMenu/icon_offense.png", "init/CharaMenu/icon_offense.png");
    auto offense_on  = MenuItemImage::create("init/CharaMenu/icon_offense.png", "init/CharaMenu/icon_offense.png");
    
//    offense_off->setScale(ratio / scale);
    offense_off->setScaleX(1 / scaleX);
    offense_off->setScaleY(1 / scale);
    offense_off->setColor(Color3B(180, 180, 180));
    offense_off->setAnchorPoint(Point(0.5, 0.5));
    offense_off->setPosition(Point((bt_offense_off->getContentSize().width/2) / bt_offense_off->getScaleX(), bt_offense_off->getContentSize().height/2));

//    offense_on->setScale(ratio / scale);
    offense_on->setScaleX(1 / scaleX);
    offense_on->setScaleY(1 / scale);
    offense_on->setAnchorPoint(Point(0.5, 0.5));
    offense_on->setPosition(Point((bt_offense_on->getContentSize().width/2) / bt_offense_off->getScaleX(), bt_offense_on->getContentSize().height/2));
    
    bt_offense_off->addChild(offense_off);
    bt_offense_on->addChild(offense_on);
    
    auto offense_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectType, this), bt_offense_off, bt_offense_on, nullptr);
    offense_btn->setTag(T_Offense_Btn);
    
    
    //防御選択
    auto bt_defense_off = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    auto bt_defense_on = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    bt_defense_off->setScale(ratio * scale);
    bt_defense_off->setScaleX(ratio * scaleX);
    bt_defense_off->setContentSize(Size(bt_defense_off->getContentSize().width * scaleX * ratio, bt_defense_off->getContentSize().height));
    bt_defense_off->setColor(Color3B(180, 180, 180));
    
    bt_defense_on->setScale(ratio * scale);
    bt_defense_on->setScaleX(ratio * scaleX);
    bt_defense_on->setContentSize(Size(bt_defense_on->getContentSize().width * scaleX * ratio, bt_defense_on->getContentSize().height));
    
    
    auto defense_off = MenuItemImage::create("init/CharaMenu/icon_defense.png", "init/CharaMenu/icon_defense.png");
    auto defense_on  = MenuItemImage::create("init/CharaMenu/icon_defense.png", "init/CharaMenu/icon_defense.png");
    
//    defense_off->setScale(ratio / scale);
    defense_off->setScaleX(1 / scaleX);
    defense_off->setScaleY(1 / scale);
    defense_off->setColor(Color3B(180, 180, 180));
    defense_off->setAnchorPoint(Point(0.5, 0.5));
    defense_off->setPosition(Point((bt_defense_off->getContentSize().width/2) / bt_defense_off->getScaleX(), bt_defense_off->getContentSize().height/2));
    
//    defense_on->setScale(ratio / scale);
    defense_on->setScaleX(1 / scaleX);
    defense_on->setScaleY(1 / scale);
    defense_on->setAnchorPoint(Point(0.5, 0.5));
    defense_on->setPosition(Point((bt_defense_on->getContentSize().width/2) / bt_defense_on->getScaleX(), bt_defense_on->getContentSize().height/2));
    
    bt_defense_off->addChild(defense_off);
    bt_defense_on->addChild(defense_on);
    
    auto defense_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectType, this), bt_defense_off, bt_defense_on, nullptr);
    defense_btn->setTag(T_Defense_Btn);
    
    //軍師選択
    auto bt_strategist_off = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    auto bt_strategist_on = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    bt_strategist_off->setScale(ratio * scale);
    bt_strategist_off->setScaleX(ratio * scaleX);
    bt_strategist_off->setColor(Color3B(180, 180, 180));
    bt_strategist_off->setContentSize(Size(bt_strategist_off->getContentSize().width * scaleX * ratio, bt_strategist_off->getContentSize().height));
    
    bt_strategist_on->setScale(ratio * scale);
    bt_strategist_on->setScaleX(ratio * scaleX);
    bt_strategist_on->setContentSize(Size(bt_strategist_on->getContentSize().width * scaleX * ratio, bt_strategist_on->getContentSize().height));
    
    
    auto strategist_off = MenuItemImage::create("init/CharaMenu/icon_strategist.png", "init/CharaMenu/bt_order_strategist.png");
    auto strategist_on  = MenuItemImage::create("init/CharaMenu/icon_strategist.png", "init/CharaMenu/bt_order_strategist.png");
    
//    strategist_off->setScale(ratio / scale);
    strategist_off->setScaleX(1 / scaleX);
    strategist_off->setScaleY(1 / scale);
    strategist_off->setColor(Color3B(180, 180, 180));
    strategist_off->setAnchorPoint(Point(0.5, 0.5));
    strategist_off->setPosition(Point((bt_strategist_off->getContentSize().width/2) / bt_strategist_off->getScaleX(), bt_strategist_off->getContentSize().height/2));
    
//    strategist_on->setScale(ratio / scale);
    strategist_on->setScaleX(1 / scaleX);
    strategist_on->setScaleY(1 / scale);
    strategist_on->setAnchorPoint(Point(0.5, 0.5));
    strategist_on->setPosition(Point((bt_strategist_on->getContentSize().width/2) / bt_strategist_on->getScaleX(), bt_strategist_on->getContentSize().height/2));
    
    bt_strategist_off->addChild(strategist_off);
    bt_strategist_on->addChild(strategist_on);
    
    auto strategist_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectType, this), bt_strategist_off, bt_strategist_on, nullptr);
    strategist_btn->setTag(T_Stratege_Btn);
    
    //総大将選択
    auto bt_general_off = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    auto bt_general_on = MenuItemImage::create("init/Home/Common/bt1.png", "init/Home/Common/bt1.png");
    bt_general_off->setScale(ratio * scale);
    bt_general_off->setScaleX(ratio * scaleX);
    bt_general_off->setColor(Color3B(180, 180, 180));
    bt_general_off->setContentSize(Size(bt_general_off->getContentSize().width * scaleX * ratio, bt_general_off->getContentSize().height));
    
    bt_general_on->setScale(ratio * scale);
    bt_general_on->setScaleX(ratio * scaleX);
    bt_general_on->setContentSize(Size(bt_general_on->getContentSize().width * scaleX * ratio, bt_general_on->getContentSize().height));
    
    
    auto general_off = MenuItemImage::create("init/CharaMenu/icon_general.png", "init/CharaMenu/bt_order_general.png");
    auto general_on  = MenuItemImage::create("init/CharaMenu/icon_general.png", "init/CharaMenu/bt_order_general.png");
    
//    general_off->setScale(ratio / scale);
    general_off->setScaleX(1 / scaleX);
    general_off->setScaleY(1 / scale);
    general_off->setColor(Color3B(180, 180, 180));
    general_off->setAnchorPoint(Point(0.5, 0.5));
    general_off->setPosition(Point((bt_general_off->getContentSize().width/2) / bt_general_off->getScaleX(), bt_general_off->getContentSize().height/2));
    
//    general_on->setScale(ratio / scale);
    general_on->setScaleX(1 / scaleX);
    general_on->setScaleY(1 / scale);
    general_on->setAnchorPoint(Point(0.5, 0.5));
    general_on->setPosition(Point((bt_general_on->getContentSize().width/2) / bt_general_on->getScaleX(), bt_general_on->getContentSize().height/2));
    
    bt_general_off->addChild(general_off);
    bt_general_on->addChild(general_on);
    
    auto general_btn = MenuItemToggle::createWithCallback(CC_CALLBACK_1(CharaSellListLayer::selectType, this), bt_general_off, bt_general_on, nullptr);
    general_btn->setTag(T_General_Btn);
    
    
    auto menu = Menu::create(offense_btn, defense_btn, strategist_btn, general_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(0);
    menu->setPosition(convertFromDisplaySize(Point(230 - _contentSize.width, 710), 0.5, 1.0));
    this->addChild(menu, Z_Type_Menu, T_Type_Menu);
    
    menu->setVisible(false);
}

void CharaSellListLayer::createSwitchButton()
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
    menu->setPosition(convertFromDisplaySize(Point(564, 715), 0.5, 1.0));
    this->addChild(menu, Z_Switch_Menu, T_Switch_Menu);
}


void CharaSellListLayer::selectRarity(cocos2d::Ref *sender)
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
//    actionSort();
//    initData(_chara_type);
}

void CharaSellListLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, (POS_Y_DISTANCE * ratio) * _row_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - (((POS_Y_DISTANCE * ratio) * _row_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio + SELL_BG_HEIGHT * ratio + SORT_BTN_SIZE * ratio)))));
    
    //バーを表示
    bar->refresh();
}

Layer* CharaSellListLayer::createScrollList()
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

void CharaSellListLayer::checkDeckUsed()
{
    //デッキ使用中チェック
    
    //デッキ使用idのリスト作成
    HmsVector<string > _deck_used_chara_list;
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        string iStr = toString<int>(i);
        _deck_used_chara_list.push_back(_user_party_list[iStr]["leader_chara_list_id"].string_value());
        
        for (int j = 0; j < MAX_DECK_CHARA_COUNT; j++)
        {
            _deck_used_chara_list.push_back(_user_party_list[iStr]["chara_list_id" + toString<int>(j + 1)].string_value());
        }
    }
    
    //検索
    for (int i = 0; i < _chara.size(); i++)
    {
        auto found = find(_deck_used_chara_list.begin(), _deck_used_chara_list.end(), _chara[i]->getCharaListId());
        
        if (found != _deck_used_chara_list.end())
        {
            //あった場合
            _chara[i]->setDeckUsedFlg(true);
            _chara[i]->setColor(Color3B(150, 150, 150));
            
            //使用中文字とアクション
            auto chara_node = _chara[i]->getChildByTag(CharaSprite::T_Node);
            
            auto used_label = HMSFontUtil::createWithTTFBySystemFontArgument("使用中", FONT_NAME_SYSTEM, 24);
            used_label->setTextColor(Color4B(255, 255, 140, 255));
            used_label->setPosition(Point(chara_node->getContentSize() / 2));
            chara_node->addChild(used_label);
            
            auto fadeOut = FadeOut::create(0.8f);
            auto fadeIn  = FadeIn::create(0.4f);
            auto repeat  = RepeatForever::create(Sequence::create(DelayTime::create(1.0f), fadeOut, DelayTime::create(2.5f), fadeIn, nullptr));
            used_label->runAction(repeat);
        }
    }
}

void CharaSellListLayer::showCharaSprite()
{
    //キャラの初期表示
    for (int i = 0; i < _refine_index_list.size(); i++)
    {
        bool visible_flg = false;
        if(_refine_index_list[i]<_view_count_default) visible_flg = true;
        _chara[_refine_index_list[i]]->setVisible(visible_flg);
        _chara[_refine_index_list[i]]->setPosition(_position_index_list[i]);
        _chara[_refine_index_list[i]]->setZOrder(Z_FadeIn_Chara);
    }
}

void CharaSellListLayer::refreshPage(bool* chara_type)
{
    //キャラの並び順を変更する
    
    //データの初期化
    initSortData(chara_type);
    
    //キャラの並び替え
    sortCharaData();
    
    //キャラの絞り込み
    createRefineIndexList();
    
    //表示キャラのポジションインデックスを作成
    createPositionIndexList();
    
    //スクロールデータの更新
    setScrollViewData();
    
    //並び替えアクション
    actionSort();
    
    //ソートボタン文字列の変更
    auto sort_menu = (MenuItemSprite *)this->getChildByTag(T_SortBtn_Menu)->getChildByTag(T_SortBtn_MenuItem);
    
    auto label_off = (Label *)sort_menu->getNormalImage()->getChildByTag(T_SortOff_Text);
    label_off->setString(createSortText());
    
    auto label_on = (Label *)sort_menu->getSelectedImage()->getChildByTag(T_SortOn_Text);
    label_on->setString(createSortText());
    
    
    //所持キャラ数の変更
    string text = "所持数 " + toString<int>(_chara_count) + " / " + SaveData::loadString("max_chara_count");
    
    for (int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++){
        if (_chara_type[i] == false)
        {
            text = "絞り込み該当数 " + toString<int>(_chara_count);
            break;
        }
    }

    auto num_label = (Label *)this->getChildByTag(T_Num_Base)->getChildByTag(T_Num_Label);
    num_label->setString(text);
    
    
    this->unschedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
    CharaSellListLayer::viewCount = 0;
    this->schedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));

}

void CharaSellListLayer::actionSort()
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
            
            auto move = MoveTo::create(0.3f, _position_index_list[i]);
            _chara[_refine_index_list[i]]->runAction(move);
        }
        this->unschedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
        CharaSellListLayer::viewCount = 0;
        this->schedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
    });
    
    auto touch_enable = CallFunc::create([this](){
        
        //ボタンのタッチを有効にする
        setEnableMenuButton(true);
    });
    
    this->runAction(Sequence::create(fade_func, DelayTime::create(0.2f), move_func, DelayTime::create(0.3f), touch_enable, nullptr));
}

void CharaSellListLayer::setEnableMenuButton(bool boolean)
{
    //並び替えボタンのタッチ切り替え
    auto rarity_menu = (Menu *)this->getChildByTag(T_Rarity_Menu);
    auto type_menu   = (Menu *)this->getChildByTag(T_Type_Menu);
    
    rarity_menu->setEnabled(boolean);
    type_menu->setEnabled(boolean);
}

Point CharaSellListLayer::calcPositionChara(int i, int j)
{
    float x, y;
    
    x = (POS_X_FIRST * ratio) + ((POS_X_DISTANCE * ratio) * j);
    y = (POS_Y_DISTANCE * ratio) * (_row_count - i) - (POS_Y_DISTANCE * ratio / 2);
    
    return Point(x, y);
}

void CharaSellListLayer::showSortPopup(Ref* sender)
{
    //並び替えポップアップの表示
    auto popup_layer = CharaListSortPopupLayer::create(_chara_type, "CharaSellList");
    addChild(popup_layer, 999999);
}

void CharaSellListLayer::showCharaCount()
{
    //所持キャラ数の背景
    auto num_base = Sprite::create("init/CharaMenu/num_base.png");
    num_base->setScale(ratio);
    num_base->setAnchorPoint(Point(1.0, 0.5));
    num_base->setPosition(convertFromDisplaySize(Point(620, 122), 0.5, 0));
    addChild(num_base, Z_Num_Base, T_Num_Base);
    
    //キャラ数
    string text = "所持数 " + toString<int>(_chara_count) + " / " + SaveData::loadString("max_chara_count");
    auto num_label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24);
    num_label->setPosition(Point(num_base->getContentSize() / 2));
    num_base->addChild(num_label, Z_Num_Label, T_Num_Label);
}

void CharaSellListLayer::showSellPopup()
{
    //売却ポップアップの作成
    createSellPopupBgAndButton();
    
    //売却テキストの作成
    createSellText();
}

void CharaSellListLayer::createSellPopupBgAndButton()
{
    //売却ポップアップの作成
    
    //背景
    _sell_bg = Sprite::create("init/CharaMenu/popup_sell.png");
    _sell_bg->setScale(ratio);
    _sell_bg->setAnchorPoint(Point(0.5, 0));
    _sell_bg->setPosition(convertFromDisplaySize(Point(320, FOOTER_SIZE ), 0.5, 0));
    this->addChild(_sell_bg, Z_Sell_Bg, T_Sell_Bg);
    
    //選択解除ボタン
    auto cancel_off = Sprite::create("init/CharaMenu/bt_cancel.png");
    auto cancel_on  = Sprite::create("init/CharaMenu/bt_cancel.png");
    cancel_on->setColor(Color3B(180, 180, 180));
    
    auto cancel_btn = MenuItemSprite::create(cancel_off, cancel_on, [this](Ref* sender){
        //キャラ選択解除
        initCharaSelect();
        
        //売却ラベル初期化
        updateSellBgLabel(CharaSprite::Rarity::NoneRarity, SellStatus::Initialize);
        
        //解除音
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_reset.mp3");
        
    });
    auto menu = Menu::create(cancel_btn, nullptr);
    menu->setPosition(Point(80, _sell_bg->getContentSize().height / 2));
    _sell_bg->addChild(menu);
    
    //決定ボタン
    auto decision_off = Sprite::create("init/CharaMenu/bt_decision.png");
    auto decision_on  = Sprite::create("init/CharaMenu/bt_decision.png");
    decision_on->setColor(Color3B(180, 180, 180));
    
    auto decision_btn = MenuItemSprite::create(decision_off, decision_on, [this](Ref* sender){
        //決定ポップアップ表示処理
        if (_sell_chara_list.size() > 0) {
            string rarity = "";
            for(int i=0;i<sizeof(_chara_rarity);i++){
                if(_chara_rarity[i]) rarity += "1";
                else rarity += "0";
            }
            HMSLOG("sortRare:%s", rarity.c_str());
            SaveData::saveString("sortRarity", rarity);
            string type = "";
            for(int i=0;i<sizeof(_chara_type);i++){
                if(_chara_type[i]) type += "1";
                else type += "0";
            }
            HMSLOG("sortType:%s", type.c_str());
            SaveData::saveString("sortType", type);
            
            auto popup_layer = CharaSellConfirmLayer::create(_sell_chara_list, _sell_price);
            this->addChild(popup_layer, 999999);
        }
        else {
            
            //NG音
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/ng.mp3");
        }
    });
    auto menu2 = Menu::create(decision_btn, nullptr);
    menu2->setPosition(Point(560, _sell_bg->getContentSize().height / 2));
    _sell_bg->addChild(menu2);
}

void CharaSellListLayer::createSellText()
{
    //売却合計
    string text1 = "　　　　売却合計　" + toString<int>(_sell_price);
    auto sell_price_label = HMSFontUtil::createWithTTFBySystemFontArgument(text1, FONT_NAME_SYSTEM, 20);
    sell_price_label->setHorizontalAlignment(TextHAlignment::LEFT);
    sell_price_label->setAnchorPoint(Point(0, 0.5));
    sell_price_label->setPosition(Point(160, _sell_bg->getContentSize().height * 4 / 5));
    _sell_bg->addChild(sell_price_label, Z_Sell_Price_Label, T_Sell_Price_Label);
    
    //選択数
    string text2 = "　　　　　選択数　" + toString<int>(_select_count) + " / " + toString<int>(MAX_SELL_COUNT);
    auto select_label = HMSFontUtil::createWithTTFBySystemFontArgument(text2, FONT_NAME_SYSTEM, 20);
    select_label->setHorizontalAlignment(TextHAlignment::LEFT);
    select_label->setAnchorPoint(Point(0, 0.5));
    select_label->setPosition(Point(160, _sell_bg->getContentSize().height * 3 / 5));
    _sell_bg->addChild(select_label, Z_Select_Label, T_Select_Label);
    
    //所持数
    string text3 = "　　　　　所持数　" + toString<int>(_chara_count) + " / " + SaveData::loadString("max_chara_count");
    auto chara_count_label = HMSFontUtil::createWithTTFBySystemFontArgument(text3, FONT_NAME_SYSTEM, 20);
    chara_count_label->setHorizontalAlignment(TextHAlignment::LEFT);
    chara_count_label->setAnchorPoint(Point(0, 0.5));
    chara_count_label->setPosition(Point(160, _sell_bg->getContentSize().height * 2 / 5));
    _sell_bg->addChild(chara_count_label, Z_Chara_Count_Label, T_Chara_Count_Label);
    
    //交換チケット
    string text4 = "獲得交換チケット　" + toString<int>(_exchange_ticket) + "枚";
    auto exchange_ticket_label = HMSFontUtil::createWithTTFBySystemFontArgument(text4, FONT_NAME_SYSTEM, 20);
    exchange_ticket_label->setHorizontalAlignment(TextHAlignment::LEFT);
    exchange_ticket_label->setAnchorPoint(Point(0, 0.5));
    exchange_ticket_label->setPosition(Point(160, _sell_bg->getContentSize().height * 1 / 5));
    _sell_bg->addChild(exchange_ticket_label, Z_Exchange_Ticket_Label, T_Exchange_Ticket_Label);
}

void CharaSellListLayer::checkFavoriteChara()
{
    //お気に入りチェック
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    HmsVector<string> favorite_list = split(favorite_character_list, "/");
    
    for (int i = 0; i < _chara.size(); i++)
    {
        auto found = find(favorite_list.begin(), favorite_list.end(), _chara[i]->getCharaListId());
        
        if (found != favorite_list.end())
        {
            //見つかった場合
            
            //お気に入りスプライトの表示
            _chara[i]->setFavoriteSprite();
            
            
            _chara[i]->setDeckUsedFlg(true);
            _chara[i]->setColor(Color3B(150, 150, 150));
            
            //使用中文字とアクション
            auto chara_node = _chara[i]->getChildByTag(CharaSprite::T_Node);
            
            auto favorite_label = HMSFontUtil::createWithTTFBySystemFontArgument("保護中", FONT_NAME_SYSTEM, 24);
            favorite_label->setTextColor(Color4B(154, 160, 240, 255));
            favorite_label->setPosition(Point(chara_node->getContentSize() / 2));
            chara_node->addChild(favorite_label);
            
            auto fadeOut = FadeOut::create(0.8f);
            auto fadeIn  = FadeIn::create(0.4f);
            auto repeat  = RepeatForever::create(Sequence::create(DelayTime::create(1.0f), fadeOut, DelayTime::create(2.5f), fadeIn, nullptr));
            favorite_label->runAction(repeat);
        }
    }
}

void CharaSellListLayer::showExchageTicket()
{
    //交換チケットの表示
    for (int i = 0; i < _chara.size(); i++) {
        if (fromString<int>(_chara[i]->getExchangeTicket()) > 0) _chara[i]->setExchangeTicketSprite();
    }
}

void CharaSellListLayer::refreshSortPage(bool* chara_type)
{
    //キャラの並び順を変更する
    
    //データの初期化
    initSortData(chara_type);
    
    //キャラの並び替え
    sortCharaData();
    
    //キャラの絞り込み
    createRefineIndexList();
    
    //表示キャラのポジションインデックスを作成
    createPositionIndexList();
    
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
    
    this->unschedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
    CharaSellListLayer::viewCount = 0;
    this->schedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
}


void CharaSellListLayer::selectType(cocos2d::Ref *sender)
{
    auto menuItem = (MenuItemToggle *)sender;
    
    //選択状態が全て同じ状態又は１つだけの状態の同じボタンを押されたとき、そのボタンだけを選択状態にする
    int selected_count = countCharaTypeSelected();
    if (selected_count == (sizeof(_chara_type) / sizeof(_chara_type[0])))
    {
        //全ての選択状態を初期化（後につづく処理で選択状態になる）
        initCharaType(ButtonStatus::All_Unselected);
    }
    
    switch (menuItem->getTag())
    {
        case T_Offense_Btn:   _chara_type[0] = changeBoolean(_chara_type[0]); break;
        case T_Defense_Btn:   _chara_type[1] = changeBoolean(_chara_type[1]); break;
        case T_Stratege_Btn:  _chara_type[2] = changeBoolean(_chara_type[2]); break;
        case T_General_Btn:   _chara_type[3] = changeBoolean(_chara_type[3]); break;
        default: break;
    }
    
    //全部解除状態であった場合、全選択にする
    if (countCharaTypeSelected() == 0) initCharaType(ButtonStatus::All_Selected);
    
    //音の設定
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
    
    //並び替え
    refreshSortPage(_chara_type);
//    actionSort();
//    initData(_chara_type);

}

bool CharaSellListLayer::changeBoolean(bool status)
{
    //引数のboolと反対のboolを返す
    if (status) return false;
    else return true;
}

int CharaSellListLayer::countCharaRaritySelected()
{
    //キャラレアリティの選択状態の数（trueの数）を返す
    int selected_count  = 0;
    
    for (int i = 0; i < sizeof(_chara_rarity) / sizeof(_chara_rarity[0]); i++)
    {
        if (_chara_rarity[i]) selected_count++;
    }
    
    return selected_count;
}

void CharaSellListLayer::initCharaRarity(ButtonStatus status)
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

int CharaSellListLayer::countCharaTypeSelected()
{
    //キャラタイプの選択状態の数（trueの数）を返す
    int selected_count  = 0;
    
    for (int i = 0; i < (sizeof(_chara_type) / sizeof(_chara_type[0]));     i++)
    {
        if (_chara_type[i]) selected_count++;
    }
    
    return selected_count;
}

void CharaSellListLayer::initCharaType(ButtonStatus status)
{
    //キャラタイプ状態を初期化
    for (int i = 0; i < (sizeof(_chara_type) / sizeof(_chara_type[0])) ; i++)
    {
        switch (status)
        {
            case ButtonStatus::All_Selected:   _chara_type[i] = true;  break;
            case ButtonStatus::All_Unselected: _chara_type[i] = false; break;
            default: break;
        }
    }
}


void CharaSellListLayer::setRarityButtonAndTypeButtonData()
{
    //絞り込みボタンの選択状態のデータ設定
    
    //レア度ボタンの設定
    setRarityButtonStatus();
    
    //キャラタイプボタンの設定
    setTypeButtonStatus();
}

void CharaSellListLayer::setRarityButtonStatus()
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

void CharaSellListLayer::setTypeButtonStatus()
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

void CharaSellListLayer::refreshNewPage(bool *chara_type)
{
    bool chara_rarity[6] = {true, true, true, true, true, true}; //レア度絞り込みのパラメータ※N、R、SR、UR、UR+、PT

    string rarity = SaveData::loadString("sortRarity");
    if(rarity.length() > 0){
        for(int i=0;i<sizeof(_chara_rarity);i++){
            if(rarity.substr(i,1)=="0") chara_rarity[i] = false;
            else chara_rarity[i] = true;
        }
    }

    string type = SaveData::loadString("sortType");
    if(type.length() > 0){
        for(int i=0;i<sizeof(_chara_type);i++){
            if(type.substr(i,1)=="0") chara_type[i] = false;
            else chara_type[i] = true;
        }
    }
    
    //既存のスクロールレイヤーの削除
    removeChildByTag(T_ScrollView);
    removeChild(bar);
    
    //キャラとデータの初期化
    initData(chara_type, chara_rarity);
    
    //データの設定
    setData();
    
    //キャラ画像の生成
    createCharaSprite();
    
    //キャラの並び替え
    sortCharaData();
    
    //表示キャラの絞り込み
    createRefineIndexList();
    
    //表示キャラのポジションインデックスを作成
    createPositionIndexList();
    
    //スクロールの作成
    createScrollView();
    
    //スクロールデータの設定
    setScrollViewData();
    
    //デッキ使用中チェック
    checkDeckUsed();
    
    //お気に入り登録チェック
    checkFavoriteChara();
    
    //交換チケットの表示
    showExchageTicket();
    
    //キャラの表示
    showCharaSprite();

    //所持数表示のリセット
    _chara_count = countJsonData(_chara_data_list);

    //売却ラベルの更新
    auto price_label  = (Label *)_sell_bg->getChildByTag(T_Sell_Price_Label);
    auto select_label = (Label *)_sell_bg->getChildByTag(T_Select_Label);
    auto chara_count_label = (Label *)_sell_bg->getChildByTag(T_Chara_Count_Label);
    auto exchange_ticket_label = (Label *)_sell_bg->getChildByTag(T_Exchange_Ticket_Label);
    
    price_label->setString("　　　　売却合計　" + toString<int>(_sell_price));
    select_label->setString("　　　　　選択数　" + toString<int>(_select_count) + " / " + toString<int>(MAX_SELL_COUNT));
    chara_count_label->setString("　　　　　所持数　" + toString<int>(_chara_count) + " / " + SaveData::loadString("max_chara_count"));
    exchange_ticket_label->setString("獲得交換チケット　" + toString<int>(_exchange_ticket) + "枚");
    
    
    this->unschedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
    CharaSellListLayer::viewCount = 0;
    this->schedule(schedule_selector(CharaSellListLayer::showCharaSpriteInterval));
}

void CharaSellListLayer::createSellCharaList(CharaSprite* chara)
{
    //売却リストの作成
    
    //売却リストに選択したcharaがいるかどうかチェックする（見つから無い場合はendのイテレータを返す）
//    auto found = find(std::begin(_sell_chara_list), std::end(_sell_chara_list), chara);
    auto found = find(_sell_chara_list.begin(), _sell_chara_list.end(), chara);
    
    if (found != _sell_chara_list.end())
    {
        //売却リストにあるので削除、詰める
        _sell_chara_list.erase(found);
        
        //売却価格と選択数の更新
        updateSellBgLabel((CharaSprite::Rarity)fromString<int>(chara->getRare()), SellStatus::Less, fromString<int>(chara->getExchangeTicket()));
        
        //選択状態を解除
        removeNumberCircle(chara);
        
        //減って9個になった場合反転してもどす
        if (_sell_chara_list.size() == MAX_SELL_COUNT - 1)
        {
            turnColorSelect();
            
            //解除して明るくなったものを又反転して暗くしてしまうので対策としてここで戻す
            chara->setColor(Color3B(255, 255, 255));
        }
    }
    else {
        //売却リストに無いので挿入（10個まで！)
        if (_sell_chara_list.size() < MAX_SELL_COUNT)
        {
            _sell_chara_list.push_back(chara);
            
            //売却価格と選択数の更新
            updateSellBgLabel((CharaSprite::Rarity)fromString<int>(chara->getRare()), SellStatus::Add, fromString<int>(chara->getExchangeTicket()));
            
            //キャラを選択状態にする
            createNumberCircle(_sell_chara_list.size(), chara);
            
            //増えて10個になった場合反転
            if(_sell_chara_list.size() == MAX_SELL_COUNT) turnColorSelect();
        }
    }
    
    //ログ出力
    for (int i = 0; i < _sell_chara_list.size(); i++)
    {
        CCLOG("chara_list_id[%d] = %s", i, _sell_chara_list[i]->getCharaListId().c_str());
    }
}

void CharaSellListLayer::createNumberCircle(int value, CharaSprite* chara)
{
    //選択キャラを暗くする
    chara->setColor(Color3B(150, 150, 150));
    
    //選択ナンバーをノードの上につける(暗くする処理を適用させないためノードの上に乗せる)
    auto chara_node = chara->getChildByTag(CharaSprite::T_Node);
    
    auto circle = Sprite::create("init/CharaMenu/num/card_num" + toString<int>(value) + ".png");
    circle->setPosition(Point(chara_node->getContentSize() / 2));
    chara_node->addChild(circle, Z_Circle, T_Circle);
}

void CharaSellListLayer::removeNumberCircle(CharaSprite* chara)
{
    //キャラを通常の色に戻す
    chara->setColor(Color3B(255, 255, 255));
    
    //キャラの番号を取り除く
    chara->getChildByTag(CharaSprite::T_Node)->removeChildByTag(T_Circle);
    
    //全てのキャラ番号を取り除いて再番しなおす
    for (int i = 0; i < _sell_chara_list.size(); i++)
    {
        auto chara_node = _sell_chara_list[i]->getChildByTag(CharaSprite::T_Node);
        
        chara_node->removeChildByTag(T_Circle);
        
        auto circle = Sprite::create("init/CharaMenu/num/card_num" + toString<int>(i + 1) + ".png");
        circle->setPosition(Point(chara_node->getContentSize() / 2));
        chara_node->addChild(circle, Z_Circle, T_Circle);
    }
}

void CharaSellListLayer::initCharaSelect()
{
    //全ての選択状態を解除する
    for (int i = 0; i < _chara.size(); i++)
    {
        //色を戻す
        if (!_chara[i]->getDeckUsedFlg()) _chara[i]->setColor(Color3B(255, 255, 255));
        
        //番号を取り除く
        _chara[i]->getChildByTag(CharaSprite::T_Node)->removeChildByTag(T_Circle);
    }
    
    //売却リストの初期化
    _sell_chara_list.clear();
}

void CharaSellListLayer::turnColorSelect()
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

int CharaSellListLayer::calcSellPrice(CharaSprite::Rarity rarity)
{
    //売却価格の計算
    int price = 0;
    switch (rarity) {
        case CharaSprite::Rarity::Normal:     price = SELL_PRICE_NORMAL;      break;
        case CharaSprite::Rarity::Rare:       price = SELL_PRICE_RARE;        break;
        case CharaSprite::Rarity::SuperRare:  price = SELL_PRICE_SUPER_RARE;  break;
        case CharaSprite::Rarity::UltraRare:  price = SELL_PRICE_ULTRA_RARE;  break;
        case CharaSprite::Rarity::LegendRare: price = SELL_PRICE_LEGEND_RARE; break;
        default: price = SELL_PRICE_NONE; break;
    }
    
    return price;
}

void CharaSellListLayer::updateSellBgLabel(CharaSprite::Rarity rarity, SellStatus sell_status, int exchange_ticket)
{
    //画面下、売却ラベルの更新
    switch (sell_status) {
        case Add:
            _sell_price += calcSellPrice(rarity);
            _select_count++;
            _exchange_ticket += exchange_ticket;
            break;
        case Less:
            _sell_price -= calcSellPrice(rarity);
            _select_count--;
            _exchange_ticket -= exchange_ticket;
            break;
        default:
            _sell_price = 0;
            _select_count = 0;
            _exchange_ticket = 0;
            break;
    }
    
    auto price_label  = (Label *)_sell_bg->getChildByTag(T_Sell_Price_Label);
    auto select_label = (Label *)_sell_bg->getChildByTag(T_Select_Label);
    auto exchange_label = (Label *)_sell_bg->getChildByTag(T_Exchange_Ticket_Label);
    
    price_label->setString("　　　　売却合計　" + toString<int>(_sell_price));
    select_label->setString("　　　　　選択数　" + toString<int>(_select_count) + " / " + toString<int>(MAX_SELL_COUNT));
    exchange_label->setString("獲得交換チケット　" + toString<int>(_exchange_ticket) + "枚");
}

void CharaSellListLayer::showCharaDetail(float dt)
{
    _touch_flg = false;
    
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(_touch_chara->getCharaId());
    this->addChild(detail_layer, Z_Detail_Layer, T_Detail_Layer);
}

void CharaSellListLayer::collectivelySell(Ref* sender){
    
    clearSellList();
    for_each(_chara.begin(), _chara.end(), [this](CharaSprite* charaSprite){
        if((CharaSprite::Rarity)fromString<int>(charaSprite->getRare()) <= CharaSprite::Rarity::Rare && !charaSprite->getDeckUsedFlg()){
            _sell_chara_list.push_back(charaSprite);
            updateSellBgLabel((CharaSprite::Rarity)fromString<int>(charaSprite->getRare()), SellStatus::Add, fromString<int>(charaSprite->getExchangeTicket()));
        }
    });
    
    //ログ出力
    for (int i = 0; i < _sell_chara_list.size(); i++)
    {
        CCLOG("chara_list_id[%d] = %s", i, _sell_chara_list[i]->getCharaListId().c_str());
    }
    
    if(_sell_chara_list.size() > 0){
        CharaCollectivelySellConfirmPopupLayer* popup_layer = CharaCollectivelySellConfirmPopupLayer::create(_sell_chara_list, _sell_price);
        this->addChild(popup_layer, 999999);
    }else{
        auto func = [](){};
        PopupOkButtonLayer* popup = PopupOkButtonLayer::create(func, "売却できるキャラがいません。");
        this->addChild(popup, 999999);
    }
    
}

void CharaSellListLayer::clearSellList(){
    
    _sell_chara_list.clear();
    _sell_price = 0;
    _select_count = 0;
    _exchange_ticket = 0;
    initCharaSelect();
    CharaCollectivelySellConfirmPopupLayer::create(_sell_chara_list, _sell_price);
    
    updateSellBgLabel(CharaSprite::Rarity::NoneRarity, SellStatus::Initialize);
}


bool CharaSellListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan CharaSellListLayer!");
    
    //タッチデータの初期化
    _touch_chara = nullptr;
    _touch_flg = true;
    
    //タッチしたキャラデータの取得
    for (int i = 0; i < _refine_index_list.size(); i++)
    {
        if (Utility::containsPoint(_chara[_refine_index_list[i]], touch->getLocation()))
        {
            //選択中のものでもキャラ情報を観れるようにする
            _touch_chara = _chara[_refine_index_list[i]];
            
            break;
        }
    }
    
    //キャラ詳細の表示（長押し0.3秒後）
    if (_touch_chara != nullptr) this->scheduleOnce(schedule_selector(CharaSellListLayer::showCharaDetail), 0.3f);
    
    return true;
}
void CharaSellListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void CharaSellListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //スケジューラの停止
//    this->unscheduleAllCallbacks();
    this->unschedule(schedule_selector(CharaSellListLayer::showCharaDetail));

    //キャラタッチ判定
    if (_touch_flg && _touch_chara != nullptr)
    {
        if (!_touch_chara->getDeckUsedFlg())
        {
            //デッキ使用中のものは入れず、売却リストの作成
            createSellCharaList(_touch_chara);
            
            //音の設定
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
        }
    }
}

void CharaSellListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void CharaSellListLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらタッチフラグをfalseに
    _touch_flg = false;
    
    bar->refresh();
    //スケジューラの停止
//    this->unscheduleAllCallbacks();
    this->unschedule(schedule_selector(CharaSellListLayer::showCharaDetail));
}
