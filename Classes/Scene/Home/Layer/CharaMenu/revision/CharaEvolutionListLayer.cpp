#include "CharaEvolutionListLayer.h"

CharaEvolutionListLayer::CharaEvolutionListLayer()
:_chara_data_list(nullptr)
,_chara_count(0)
,_row_count(0)
,_touch_chara(nullptr)
{
}

CharaEvolutionListLayer* CharaEvolutionListLayer::create(bool *chara_type)
{
    CharaEvolutionListLayer *sprite = new CharaEvolutionListLayer();
    
    if (sprite && sprite->init(chara_type))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool CharaEvolutionListLayer::init(bool *chara_type)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharaEvolutionListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの初期化
    initData(chara_type);
    
    return true;
}

void CharaEvolutionListLayer::initData(bool* chara_type)
{
    //データの初期化
    _chara.clear();
    _refine_index_list.clear();
    _position_index_list.clear();
    
    _chara_count = 0;
    _row_count   = 0;
    _sort_status = (CharaListSortPopupLayer::SortStatus)SaveData::loadInt("sort_status");
    _sort_status = CharaListSortPopupLayer::SortStatus::Rare;
    
    //配列の代入（引数で与えられた配列は配列サイズをsizeofで求められないので注意！）
    for(int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        _chara_type[i] = chara_type[i];
    }
}

void CharaEvolutionListLayer::initSortData(bool* chara_type)
{
    //ソート時のデータの初期化
    //    _chara.clear(); //キャラデータは初期化してはいけない！
    _refine_index_list.clear();
    _position_index_list.clear();
    _chara_count = 0;
    _row_count   = 0;
    _sort_status = (CharaListSortPopupLayer::SortStatus)SaveData::loadInt("sort_status");
    _sort_status = CharaListSortPopupLayer::SortStatus::Rare;
    
    for(int i = 0; i < sizeof(_chara_type) / sizeof(_chara_type[0]); i++) {
        _chara_type[i] = chara_type[i];
    }
}

void CharaEvolutionListLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("キャラ進化", "CharaMenu", this);
    
    //データの設定
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //並び替えボタンの作成
    //createSortButton();
    
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
    
    //キャラの表示
    showCharaSprite();
    
    //所持キャラ数の表示
    //showCharaCount();
    
    //デッキ使用中チェック
    checkDeckUsed();
    
    //お気に入り画像の表示
    checkFavoriteChara();
    
    //進化可能かどうかをチェック
    checkEnableEvolution();
    
    //ヘルプボタン作成
    createHelpButton();
}

void CharaEvolutionListLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CharaEvolutionListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CharaEvolutionListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CharaEvolutionListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CharaEvolutionListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CharaEvolutionListLayer::setData()
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

void CharaEvolutionListLayer::createSortButton()
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
    
    auto sort_btn = MenuItemSprite::create(sortOff, sortOn, CC_CALLBACK_1(CharaEvolutionListLayer::showSortPopup, this));
    sort_btn->setScale(ratio);
    sort_btn->setTag(T_SortBtn_MenuItem);
    auto menu = Menu::create(sort_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(555, 788), 0.5, 1));
    this->addChild(menu, Z_SortBtn_Menu, T_SortBtn_Menu);
}

string CharaEvolutionListLayer::createSortText()
{
    //ソートボタンのパラメータ作成
    string sort_text = "";
    switch (_sort_status)
    {
//        case CharaListSortPopupLayer::SortStatus::Power:      sort_text = "戦闘力"; break;
        case CharaListSortPopupLayer::SortStatus::Rare:       sort_text = "レア度"; break;
        case CharaListSortPopupLayer::SortStatus::NeedCost:   sort_text = "配置士気"; break;
        case CharaListSortPopupLayer::SortStatus::InsertDate: sort_text = "入手順"; break;
        default: sort_text = "戦闘力"; break;
    }
    
    return sort_text;
}

void CharaEvolutionListLayer::createCharaSprite()
{
    //キャラ画像の作成
    int count = countJsonData(_chara_data_list);
    for (int i = 0; i < count; i++)
    {
        string iStr = toString(i);
        Json chara_data = _chara_data_list[iStr].object_items();
        string rare = chara_data["rare"].string_value();
        string next_chara_id = chara_data["next_chara_id"].string_value();
        //HMSLOG("rare=%s", rare.c_str());
        //進化対象はURのみとするため、一覧も絞り込む
        if(rare=="4" && next_chara_id!="0") {
            _chara.push_back(CharaSprite::create(_chara_data_list[iStr].object_items()));
        }
    }
    
}

void CharaEvolutionListLayer::sortCharaData()
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

//HmsVector<CharaSprite *> CharaEvolutionListLayer::sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param)
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

long CharaEvolutionListLayer::getSortVal(CharaSprite *chara, string sort_param)
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

void CharaEvolutionListLayer::createRefineIndexList()
{
    //絞り込みインデックスの作成（表示するキャラの選抜）
    for (int i = 0; i < _chara.size(); i++)
    {
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

void CharaEvolutionListLayer::createPositionIndexList()
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

void CharaEvolutionListLayer::createScrollView()
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
}


void CharaEvolutionListLayer::setScrollViewData()
{
    //スクロールビューのデータの設定
    auto scroll_view = (ScrollView *)this->getChildByTag(T_ScrollView);
    
    //スクロールさせる範囲の設定
    scroll_view->setContentSize(Size(_contentSize.width, (POS_Y_DISTANCE * ratio) * _row_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    scroll_view->setContentOffset(Point(0, 0 - (((POS_Y_DISTANCE * ratio) * _row_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
}

Layer* CharaEvolutionListLayer::createScrollList()
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

Point CharaEvolutionListLayer::calcPositionChara(int i, int j)
{
    float x, y;
    
    x = (POS_X_FIRST * ratio) + ((POS_X_DISTANCE * ratio) * j);
    y = (POS_Y_DISTANCE * ratio) * (_row_count - i) - (POS_Y_DISTANCE * ratio / 2);
    
    return Point(x, y);
}

void CharaEvolutionListLayer::showCharaSprite()
{
    //キャラの初期表示
    for (int i = 0; i < _refine_index_list.size(); i++)
    {
        _chara[_refine_index_list[i]]->setVisible(true);
        _chara[_refine_index_list[i]]->setPosition(_position_index_list[i]);
        _chara[_refine_index_list[i]]->setZOrder(Z_FadeIn_Chara);
    }
}

void CharaEvolutionListLayer::showSortPopup(Ref* sender)
{
    //並び替えポップアップの表示
    auto popup_layer = CharaListSortPopupLayer::create(_chara_type, "CharaEvolutionList");
    addChild(popup_layer, 999999);
}

void CharaEvolutionListLayer::showCharaCount()
{
    //所持キャラ数の背景
    auto num_base = Sprite::create("init/CharaMenu/num_base.png");
    num_base->setScale(ratio);
    num_base->setAnchorPoint(Point(1.0, 0.5));
    num_base->setPosition(convertFromDisplaySize(Point(640, 122), 0.5, 0));
    addChild(num_base, Z_Num_Base, T_Num_Base);
    
    //キャラ数
    string text = "所持数 " + toString<int>(_chara_count) + " / " + SaveData::loadString("max_chara_count");
    auto num_label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 24);
    num_label->setPosition(Point(num_base->getContentSize() / 2));
    num_base->addChild(num_label, Z_Num_Label, T_Num_Label);
}

void CharaEvolutionListLayer::checkDeckUsed()
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
        }
    }
}

void CharaEvolutionListLayer::checkFavoriteChara()
{
    //お気に入りチェック
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    HmsVector<string> favorite_list = split(favorite_character_list, "/");
    
    for (int i = 0; i < _chara.size(); i++)
    {
        auto found = find(favorite_list.begin(), favorite_list.end(), _chara[i]->getCharaListId());
        
        if (found != favorite_list.end())
        {
            //お気に入りスプライトの表示
            _chara[i]->setFavoriteSprite();
        }
        else {
            //見つからなかった場合、取り除く
            auto favorite_sp = _chara[i]->getChildByTag(CharaSprite::T_Favorite);
            if (favorite_sp) favorite_sp->removeFromParent();
        }
    }
}

void CharaEvolutionListLayer::checkEnableEvolution()
{
    //進化先キャラIDがあり、デッキ使用中でもなく、保護中でもない場合に限り、そのキャラID毎の枚数を集計
    map<int, int> chara_count;
    for(int i = 0; i<_chara.size(); i++) {
        int chara_id = fromString<int>(_chara.at(i)->getCharaId());
        //HMSLOG("chara_id=%d", chara_id);
        if(_chara[i]->getNextCharaId()!="0" && _chara[i]->getDeckUsedFlg()==false && _chara[i]->getChildByTag(CharaSprite::T_Favorite)==nullptr) {
            chara_count[chara_id]++;
        }
    }
    
    for(auto itr = chara_count.begin(); itr != chara_count.end(); ++itr) {
        int key = itr->first;
        int value = itr->second;
        HMSLOG("key=%d, value=%d", key, value);
    }
    
    for(int i = 0; i<_chara.size(); i++) {
        int chara_id = fromString<int>(_chara.at(i)->getCharaId());
        int count = chara_count[chara_id];
        string text = "";
        if(_chara[i]->getNextCharaId()=="0") text = " 進化 \n未開放";
        else if(_chara[i]->getDeckUsedFlg()==true) text = "使用中";
        else if(_chara[i]->getChildByTag(CharaSprite::T_Favorite)!=nullptr) text = "保護中";
        else if(count<3) text = "枚数不足";
        else _chara[i]->setEvolutionFlg(true);
            
        if(text!="") {
            _chara[i]->setColor(Color3B(150, 150, 150));
            
            //使用中文字とアクション
            auto chara_node = _chara[i]->getChildByTag(CharaSprite::T_Node);
            
            auto used_label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 24);
            used_label->setTextColor(Color4B(255, 255, 140, 255));
            if(_chara[i]->getNextCharaId()=="0") used_label->setTextColor(Color4B(255, 140, 140, 255));
            used_label->setPosition(Point(chara_node->getContentSize() / 2));
            chara_node->addChild(used_label);
            
            auto fadeOut = FadeOut::create(0.8f);
            auto fadeIn  = FadeIn::create(0.4f);
            auto repeat  = RepeatForever::create(Sequence::create(DelayTime::create(1.5f), fadeOut, DelayTime::create(0.5f), fadeIn, nullptr));
            used_label->runAction(repeat);
        }
    }

    HMSLOG("checkEnableEvolution");
}

void CharaEvolutionListLayer::createHelpButton()
{
    //ヘルプボタンの作成
    auto help_btn_off = Sprite::create("init/Home/Quest/bt_help.png");
    auto help_btn_on  = Sprite::create("init/Home/Quest/bt_help.png");
    help_btn_on->setColor(Color3B(180, 180, 180));
    
    auto help_btn = MenuItemSprite::create(help_btn_off, help_btn_on, [this](Ref* sender) {
        
        auto normal_scene = (NormalScene *)this->getParent();
        
        string url = HOST;
        url += "user/index.php?&p=page__chara_evolution&user_id=" + SaveData::loadString("user_master_id");
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "キャラ進化ヘルプ");
        normal_scene->addChild(WebViewLoadLayer, 9);
    });
    help_btn->setScale(ratio);
    auto menu = Menu::create(help_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(570, 789), 0.5, 1));
    this->addChild(menu, Z_Help_Menu, T_Help_Menu);
}

void CharaEvolutionListLayer::refreshPage(bool* chara_type)
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
}

void CharaEvolutionListLayer::actionSort()
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
            _chara[_refine_index_list[i]]->setVisible(true);
            
            auto move = MoveTo::create(0.3f, _position_index_list[i]);
            _chara[_refine_index_list[i]]->runAction(move);
        }
    });
    
    this->runAction(Sequence::create(fade_func, DelayTime::create(0.2f), move_func, nullptr));
}

void CharaEvolutionListLayer::showCharaDetail(float dt)
{
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(_touch_chara->getCharaId());
    this->addChild(detail_layer, Z_Detail_Layer, T_Detail_Layer);
    
    //閉じられたときによばれる関数の指定
    auto close_func = [this](){
        checkFavoriteChara();
    };
    detail_layer->setCloseFunc(close_func);
    
    _touch_flg = false;
    _touch_chara = nullptr;
}

bool CharaEvolutionListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan CharaEvolutionListLayer!");
    
    //タッチデータの初期化
    _touch_chara = nullptr;
    _touch_flg = true;
    
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
    if (_touch_chara != nullptr) this->scheduleOnce(schedule_selector(CharaEvolutionListLayer::showCharaDetail), 0.3f);
    
    return true;
}
void CharaEvolutionListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void CharaEvolutionListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //スケジューラの停止
    this->unscheduleAllCallbacks();
    
    //キャラ詳細の表示
    //if (_touch_chara != nullptr && this->getChildByTag(T_Detail_Layer) == nullptr) showCharaDetail(0);
    
    //キャラタッチ判定
    if (_touch_flg && _touch_chara != nullptr)
    {
        if (_touch_chara->getEvolutionFlg()==true)
        {
            HMSLOG("ok. chara_id=%s,name=%s,next_chara_id=%s", _touch_chara->getCharaId().c_str(), _touch_chara->getName().c_str(), _touch_chara->getNextCharaId().c_str());
            SaveData::saveString("evolution_chara_id", _touch_chara->getCharaId());
            
            //音の設定
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
            
            //画面遷移
            auto normal_scene = (NormalScene *)this->getParent();
            normal_scene->changePage("CharaEvolutionConfirm");
        }
    }
}

void CharaEvolutionListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void CharaEvolutionListLayer::scrollViewDidScroll(ScrollView *view)
{
    //スケジューラの停止
    this->unscheduleAllCallbacks();
    
    //データの初期化
    _touch_chara = nullptr;
}

