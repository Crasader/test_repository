#include "GeneralFormationLayer.h"

GeneralFormationLayer::GeneralFormationLayer()
:_chara_data_list(nullptr)
,_user_party_list(nullptr)
,_chara_count(0)
,_row_count(0)
,_sort_status(CharaListSortPopupLayer::SortStatus::Power)
,_touch_chara(nullptr)
,_view_count_default(0)
{
}

GeneralFormationLayer* GeneralFormationLayer::create(bool *chara_type)
{
    GeneralFormationLayer *sprite = new GeneralFormationLayer();
    
    if (sprite && sprite->init(chara_type))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool GeneralFormationLayer::init(bool *chara_type)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("GeneralFormationLayer::init()");
    
    GeneralFormationLayer::viewCount = _view_count_default;
    
    ratio = getVisibleRatio();
    
    //データの初期化
    initData(chara_type);
    
    return true;
}

void GeneralFormationLayer::initData(bool *chara_type)
{
    //データの初期化
    _chara.clear();
    _refine_index_list.clear();
    _position_index_list.clear();

    _chara_count = 0;
    _row_count   = 0;
    _deck_no     = SaveData::loadString("party_no");
    _sort_status = (CharaListSortPopupLayer::SortStatus)SaveData::loadInt("sort_status");
    
    //配列の代入（引数で与えられた配列は配列サイズをsizeofで求められないので注意！）
    for(int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        _chara_type[i] = chara_type[i];
    }
}

void GeneralFormationLayer::initSortData(bool* chara_type)
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

void GeneralFormationLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("総大将編成", "DeckFormationTop", this);
    
    //データの設定
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //並び替えボタンの作成
    createSortButton();
    
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
    
    //キャラの表示
    showCharaSprite();
    
    //キャラ画像の生成
    this->schedule(schedule_selector(GeneralFormationLayer::showCharaSpriteInterval));
    
}

void GeneralFormationLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GeneralFormationLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GeneralFormationLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GeneralFormationLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(GeneralFormationLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GeneralFormationLayer::setData()
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

void GeneralFormationLayer::createSortButton()
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
    
    auto sort_btn = MenuItemSprite::create(sortOff, sortOn, CC_CALLBACK_1(GeneralFormationLayer::showSortPopup, this));
    sort_btn->setScale(ratio);
    sort_btn->setTag(T_SortBtn_MenuItem);
    auto menu = Menu::create(sort_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(555, 788), 0.5, 1));
    this->addChild(menu, Z_SortBtn_Menu, T_SortBtn_Menu);
}

string GeneralFormationLayer::createSortText()
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

void GeneralFormationLayer::createCharaSprite()
{
    //キャラ画像の作成
    int count = countJsonData(_chara_data_list);
    for (int i = 0; i < count; i++)
    {
        string iStr = toString(i);
        _chara.push_back(CharaSprite::create(_chara_data_list[iStr].object_items()));
    }
}

void GeneralFormationLayer::sortCharaData()
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

void GeneralFormationLayer::showCharaSpriteInterval(float f)
{
    if(_refine_index_list.size()<=GeneralFormationLayer::viewCount){
        HMSLOG("UNSCHEDULE:Sprite2End");
        unschedule(schedule_selector(GeneralFormationLayer::showCharaSpriteInterval));
        return;
    }
    int i = 0;
    while(true){
        if(!_chara[_refine_index_list[GeneralFormationLayer::viewCount]]->isVisible()){
            _chara[_refine_index_list[GeneralFormationLayer::viewCount]]->setVisible(true);
            //            break;
            i++;
            if(i>=2)
                break;
        }
        GeneralFormationLayer::viewCount++;
        if(_refine_index_list.size()<=GeneralFormationLayer::viewCount){
            HMSLOG("UNSCHEDULE:Sprite2End2");
            unschedule(schedule_selector(GeneralFormationLayer::showCharaSpriteInterval));
            return;
        }
    }
}

//HmsVector<CharaSprite *> GeneralFormationLayer::sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param)
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

long GeneralFormationLayer::getSortVal(CharaSprite *chara, string sort_param)
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

void GeneralFormationLayer::createRefineIndexList()
{
    //絞り込みインデックスの作成（表示するキャラの選抜）
    for (int i = 0; i < _chara.size(); i++) {
        string iStr = toString<int>(i);
        if (_chara_type[_chara[i]->getCharaType() - 1])
        {
            //絞り込み
            _refine_index_list.push_back(i);
        }
    }
    
    //表示するキャラ数と表示行数の取得
    _chara_count = _refine_index_list.size();
    _row_count = (_chara_count / ROW_1_CHARA_COUNT) + 1;
}

void GeneralFormationLayer::createPositionIndexList()
{
    //ポジションインデックスの作成
    for (int i = 0; i < _refine_index_list.size(); i++)
    {
        for (int j = 0; j < ROW_1_CHARA_COUNT; j++)
        {
            _position_index_list.push_back(calcPositionChara(i, j));
        }
    }
}

void GeneralFormationLayer::createScrollView()
{
    //スクロールビューの作成
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
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


void GeneralFormationLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, (POS_Y_DISTANCE * ratio) * _row_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - (((POS_Y_DISTANCE * ratio) * _row_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
    
    //バーを表示
    bar->refresh();
}

Layer* GeneralFormationLayer::createScrollList()
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

void GeneralFormationLayer::checkDeckUsed()
{
    //選択中の総大将を取得
    string selected_general_id = _user_party_list[toString<int>(fromString<int>(_deck_no) - 1)]["leader_chara_list_id"].string_value();
    
    //検索
    for (int i = 0; i < _chara.size(); i++)
    {
        if (_chara[i]->getCharaListId() == selected_general_id)
        {
            //使用中の総大将がある場合
            _chara[i]->setDeckUsedFlg(true);
            _chara[i]->setColor(Color3B(150, 150, 150));
            
            auto chara_node = _chara[i]->getChildByTag(CharaSprite::T_Node);
            
            auto used_label = HMSFontUtil::createWithTTFBySystemFontArgument("選択中", FONT_NAME_SYSTEM, 24);
            used_label->setTextColor(Color4B(255, 255, 140, 255));
            used_label->setPosition(Point(chara_node->getContentSize() / 2));
            chara_node->addChild(used_label);
            
            auto fadeOut = FadeOut::create(0.8f);
            auto fadeIn  = FadeIn::create(0.4f);
            auto repeat  = RepeatForever::create(Sequence::create(DelayTime::create(1.0f), fadeOut, DelayTime::create(2.5f), fadeIn, nullptr));
            used_label->runAction(repeat);
            
            break;
        }
    }
}

void GeneralFormationLayer::showCharaSprite()
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

void GeneralFormationLayer::actionSort()
{
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
        this->unschedule(schedule_selector(GeneralFormationLayer::showCharaSpriteInterval));
        GeneralFormationLayer::viewCount = 0;
        this->schedule(schedule_selector(GeneralFormationLayer::showCharaSpriteInterval));
    });
    
    this->runAction(Sequence::create(fade_func, DelayTime::create(0.2f), move_func, nullptr));
}

Point GeneralFormationLayer::calcPositionChara(int i, int j)
{
    float x, y;
    
    x = (POS_X_FIRST * ratio) + ((POS_X_DISTANCE * ratio) * j);
    y = (POS_Y_DISTANCE * ratio) * (_row_count - i) - (POS_Y_DISTANCE * ratio / 2);
    
    return Point(x, y);
}

void GeneralFormationLayer::showSortPopup(Ref* sender)
{
    //並び替えポップアップの表示
    auto popup_layer = CharaListSortPopupLayer::create(_chara_type, "GeneralFormation");
    addChild(popup_layer, 999999);
}

void GeneralFormationLayer::showCharaCount()
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

void GeneralFormationLayer::refreshSortPage(bool* chara_type)
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
    
    this->unschedule(schedule_selector(GeneralFormationLayer::showCharaSpriteInterval));
    GeneralFormationLayer::viewCount = 0;
    this->schedule(schedule_selector(GeneralFormationLayer::showCharaSpriteInterval));
}

void GeneralFormationLayer::showCharaDetail(float dt)
{
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(_touch_chara->getCharaId());
    this->addChild(detail_layer, Z_Detail_Layer, T_Detail_Layer);
    
    //タッチしたキャラデータの初期化
    _touch_chara = nullptr;
}

bool GeneralFormationLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan GeneralFormationLayer!");
    
    //タッチデータの初期化
    _touch_chara = nullptr;
    
    //タッチしたキャラデータの取得
    for (int i = 0; i < _refine_index_list.size(); i++)
    {
        if (Utility::containsPoint(_chara[_refine_index_list[i]], touch->getLocation()))
        {
            _touch_chara = _chara[_refine_index_list[i]];
            
            break;
        }
    }
    
    //キャラ詳細の表示（長押し0.3秒後）
    if (_touch_chara != nullptr) this->scheduleOnce(schedule_selector(GeneralFormationLayer::showCharaDetail), 0.3f);
    
    return true;
}
void GeneralFormationLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void GeneralFormationLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //スケジューラの停止
    this->unscheduleAllCallbacks();
    
    //キャラタッチ判定
    if (_touch_chara != nullptr)
    {
        if (!_touch_chara->getDeckUsedFlg() && this->getChildByTag(T_Confirm_layer) == nullptr)
        {
            //使用中のもの以外を選択した場合、総大将変更確認画面の表示
            auto confirm_layer = GeneralFormationConfirmLayer::create(_touch_chara);
            this->addChild(confirm_layer, Z_Confirm_layer, T_Confirm_layer);
        }
    }
}

void GeneralFormationLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void GeneralFormationLayer::scrollViewDidScroll(ScrollView *view)
{
    //スケジューラの停止
//    this->unscheduleAllCallbacks();
    this->unschedule(schedule_selector(GeneralFormationLayer::showCharaDetail));
    
    bar->refresh();
    
    //タッチキャラの初期化
    _touch_chara = nullptr;
}