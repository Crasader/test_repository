#include "TournamentEventDeckChoiceLayer.h"

TournamentEventDeckChoiceLayer::TournamentEventDeckChoiceLayer()
:_deck_no(0)
{
}

TournamentEventDeckChoiceLayer* TournamentEventDeckChoiceLayer::create()
{
    TournamentEventDeckChoiceLayer *sprite = new TournamentEventDeckChoiceLayer();
    
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TournamentEventDeckChoiceLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    HMSLOG("TournamentEventDeckChoiceLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの設定（通常のデッキ番号を初期番号として設定しておく）
    _deck_no = fromString<int>(SaveData::loadString("party_no"));
    
    return true;
}

void TournamentEventDeckChoiceLayer::onEnter()
{
    Layer::onEnter();
    
    //データの設定
    setData();
    
    //イベントリスナーの設定
    initEventListener();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("トーナメントデッキ", "TournamentList", this);
    
    //条件判定通信処理
    checkEventConditionhttpRequest();
}

void TournamentEventDeckChoiceLayer::setData()
{
    //デッキ情報の取得と設定
    string json_text = SaveData::loadString("chara_data_list");
    string err;
    _chara_data_list = Json::parse(json_text, err);
    
    string json_text2 = SaveData::loadString("user_party_list");
    string err2;
    _user_party_list = Json::parse(json_text2, err2);
    
}

void TournamentEventDeckChoiceLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TournamentEventDeckChoiceLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TournamentEventDeckChoiceLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TournamentEventDeckChoiceLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TournamentEventDeckChoiceLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TournamentEventDeckChoiceLayer::checkEventConditionhttpRequest()
{
    //選択したトーナメントIDを取得
    string tournament_event_list_id = SaveData::loadString("tournament_event_list_id");
    
    //トーナメント条件判定通信処理
    string url = HOST;
    url += "event/tournament_event/check_deck_chara_condition.php";
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&tournament_event_list_id=" + tournament_event_list_id;
    auto selector = httpresponse_selector(TournamentEventDeckChoiceLayer::checkEventConditionCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void TournamentEventDeckChoiceLayer::checkEventConditionCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    HMSLOG("result = %s", result.c_str());
    
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    _condition_match_list = json["condition_match_list"].object_items();
    
    //キャラマップデータの作成
    createMapData();
    
    //デッキ画像の作成
    createDeckSprite();
    
    //デッキの表示
    showDeckSprite();
    
    //矢印の作成
    createArrowSprite() ;
    
    actionArrowSprite() ;
    
    //ボタンの作成
    createButton();
    
    //条件判定
    alertTournamentConditionNotMatchWithChara();
    
    //警告文の作成
    createAlertText();
    
    //警告文の表示、非表示
    changeAlertLabel();
}

void TournamentEventDeckChoiceLayer::showTournamentTerm()
{
    //受付終了時刻の表示
    auto normal_scene = (NormalScene *)this->getParent();
    auto node = normal_scene->getChildByTag(NormalScene::T_Tournament_Time_Sprite);
    
    if (!node) {
        auto sp = TournamentTermSprite::create();
        sp->setScale(ratio);
        sp->setAnchorPoint(Point(0.5, 0.5));
        sp->setPosition(convertFromDisplaySize(Point(552 + _contentSize.width , 787), 0.5, 1));
        normal_scene->addChild(sp, NormalScene::Z_Tournament_Time_Sprite, NormalScene::T_Tournament_Time_Sprite);
        
        auto move = MoveTo::create(MOVING_TIME, convertFromDisplaySize(Point(552, 789), 0.5, 1));
        sp->runAction(Sequence::create(DelayTime::create(MOVING_TIME), move, nullptr));
    }
}

void TournamentEventDeckChoiceLayer::createMapData()
{
    //キャラデータのMapリストの作成
    for (int i = 0; i < countJsonData(_chara_data_list); i++)
    {
        string iStr = toString<int>(i);
        _chara_list[_chara_data_list[iStr]["chara_list_id"].string_value()] = _chara_data_list[iStr].object_items();
    }
}

void TournamentEventDeckChoiceLayer::createDeckSprite()
{
    //デッキ画像の作成
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        //キャラJsonDataの作成
        createCharaJsonData(i);
        
        _deck_bg.push_back(DeckFormationSprite::create(_vec_chara_data, i));
        _deck_bg[i]->setPosition(convertFromDisplaySize(Point(320, 510), 0.5, 0.5));
        _deck_bg[i]->setScale(0.82 * ratio);
        _deck_bg[i]->setVisible(false); //見えない状態で設定
        this->addChild(_deck_bg[i], Z_Deck_Bg, T_Deck_Bg);
    }
}

void TournamentEventDeckChoiceLayer::createCharaJsonData(int i)
{
    //キャラJsonDataの作成
    
    //初期化
    _vec_chara_data.clear();
    
    string iStr = toString<int>(i);
    for (int j = 0; j < MAX_DECK_CHARA_COUNT; j++)
    {
        _vec_chara_data.push_back(_chara_list[_user_party_list[iStr]["chara_list_id" + toString<int>(j + 1)].string_value()]);
    }
    
    //vecの21番目(配列だと20番目)に総大将データを入れる
    _vec_chara_data.push_back(_chara_list[_user_party_list[iStr]["leader_chara_list_id"].string_value()]);
}

void TournamentEventDeckChoiceLayer::showDeckSprite()
{
    //現在設定中のデッキの表示
    _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->setPosition(convertFromDisplaySize(Point(320, 510), 0.5, 0.5));
    _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->setVisible(true);
}

void TournamentEventDeckChoiceLayer::createArrowSprite()
{
    //左矢印
    _left_arrow = Sprite::create("init/CharaMenu/arrow_left.png");
    _left_arrow->setScale(ratio);
    _left_arrow->setAnchorPoint(Point(0.37, 0.5)); //画像の中心位置を起点とする
    _left_arrow->setPosition(convertFromDisplaySize(Point(30, 510), 0.5, 0.5));
    this->addChild(_left_arrow, Z_Left_Arrow, T_Left_Arrow);
    
    //右矢印
    _right_arrow = Sprite::create("init/CharaMenu/arrow_right.png");
    _right_arrow->setScale(ratio);
    _right_arrow->setPosition(convertFromDisplaySize(Point(610, 510), 0.5, 0.5));
    _right_arrow->setAnchorPoint(Point(0.37, 0.5)); //画像の中心位置を起点とする
    this->addChild(_right_arrow, Z_Right_Arrow, T_Right_Arrow);
}

void TournamentEventDeckChoiceLayer::actionArrowSprite()
{
    MoveBy *left_arrow_move = MoveBy::create(0.6f, Point(-20,0)) ;
    ActionEase *left_arrow_move_ease_in = EaseExponentialIn::create(left_arrow_move);
    ActionEase *left_arrow_move_ease_in_back = left_arrow_move_ease_in->reverse();
    MoveTo *left_arrow_move_default = MoveTo::create(0.01, _left_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *left_arrow_forever_action = RepeatForever::create(Sequence::create(left_arrow_move_ease_in, left_arrow_move_ease_in_back, left_arrow_move_default, nullptr)) ;
    
    MoveBy *right_arrow_move = MoveBy::create(0.6f, Point(+20,0)) ;
    ActionEase *right_arrow_move_ease_in = EaseExponentialIn::create(right_arrow_move);
    ActionEase *right_arrow_move_ease_in_back = right_arrow_move_ease_in->reverse();
    MoveTo *right_arrow_move_default = MoveTo::create(0.01, _right_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *right_arrow_forever_action = RepeatForever::create(Sequence::create(right_arrow_move_ease_in, right_arrow_move_ease_in_back, right_arrow_move_default, nullptr)) ;
    
    _left_arrow->runAction(left_arrow_forever_action) ;
    _right_arrow->runAction(right_arrow_forever_action) ;
}

void TournamentEventDeckChoiceLayer::createButton()
{
    //デッキ参加ボタンの作成
    auto join_off = Sprite::create("init/Home/Event/TournamentEvent/bt_join_deck.png");
    auto join_on  = Sprite::create("init/Home/Event/TournamentEvent/bt_join_deck.png");
    join_on->setColor(Color3B(180, 180, 180));
    
    auto join_btn = MenuItemSprite::create(join_off, join_on, [this](Ref* sender) {

        //選択中のデッキ番号を、トーナメント使用デッキ番号として保持しておく
        SaveData::saveString("tournament_event_deck_no", toString<int>(_deck_no));
        
        //最終確認ページへの遷移
        auto normal_scene = (NormalScene *)this->getParent();
        normal_scene->changePage("TournamentEventConfirmEntry");
    });
    join_btn->setScale(ratio);
    join_btn->setTag(T_Join_MenuItem);
    
    auto menu = Menu::create(join_btn, nullptr);
    menu->setPosition(convertFromDisplaySize(Point(320, 220), 0.5, 0.5));
    this->addChild(menu, Z_Join_Menu, T_Join_Menu);
}

void TournamentEventDeckChoiceLayer::alertTournamentConditionNotMatchWithChara()
{
    //条件にマッチしないキャラを赤く点滅させる
    for (int i = 0; i < MAX_DECK_COUNT; i++) {
        
        _vec_alert_flg.push_back(false);
        
        for (int j = 0; j <= MAX_DECK_CHARA_COUNT; j++) {
            
            string iStr = toString<int>(i);
            string jStr = toString<int>(j);
            
            if (_condition_match_list[iStr][jStr].bool_value() == false) {
                
                _vec_alert_flg.set(true, i);
                
                auto chara = _deck_bg[i]->getChildByTag(DeckFormationSprite::Tag::T_Card + j);
                if (chara) {
                    
                    //キャラにバッテン画像の配置
                    auto ban_sp = Sprite::create("download/Chara/icon/icon15.png");
                    ban_sp->setPosition(Point(chara->getContentSize() / 2));
                    chara->addChild(ban_sp, DeckFormationSprite::Z_Ban_Sprite);
                    
                    //キャラを点滅させる
                    auto tint1 = TintTo::create(0.5f, 255, 130, 130);
                    auto tint2 = TintTo::create(0.5f, 255, 255, 255);
                    chara->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.5f), tint1, tint2, nullptr)));
                }
            }
        }
    }
}

int TournamentEventDeckChoiceLayer::calcArrayNo(Pos position, int deck_no)
{
    //デッキ番号から配列上での番号を取得する（配列番号とデッキ番号は1ずれているので注意）
    int array_no = deck_no - 1;
    switch (position)
    {
        case Pos::Left:
        {
            //左側のデッキ番号の取得
            array_no  = deck_no - 2;
            if (array_no < 0) array_no = MAX_DECK_COUNT - 1 ; //左が0より下の場合、デッキNo最大を左に表示
        }break;
        case Pos::Right:
        {
            //右側のデッキ番号の取得
            array_no = deck_no;
            if (array_no >= MAX_DECK_COUNT) array_no = 0; //右が最大より上の場合、デッキNo最小を右に表示
        }break;
        default: break;
            
    }
    return array_no;
}

void TournamentEventDeckChoiceLayer::showTouchDeckSprite()
{
    //左画像
    _deck_bg[calcArrayNo(Pos::Left, _deck_no)]->setVisible(true);
    _deck_bg[calcArrayNo(Pos::Left, _deck_no)]->setPosition(convertFromDisplaySize(Point(-320, 510), 0.5, 0.5));
    
    //右画像
    _deck_bg[calcArrayNo(Pos::Right, _deck_no)]->setVisible(true);
    _deck_bg[calcArrayNo(Pos::Right, _deck_no)]->setPosition(convertFromDisplaySize(Point(960, 510), 0.5, 0.5));
}

void TournamentEventDeckChoiceLayer::moveTouchDeckSprite(float delta)
{
    //デッキ画像を移動させる
    for (int i = 0; i < _deck_bg.size(); i++)
    {
        _deck_bg[i]->setPosition(Point(_deck_bg[i]->getPosition().x + delta, _deck_bg[i]->getPosition().y));
    }
}

void TournamentEventDeckChoiceLayer::showCharaDetail(float dt)
{
    //キャラ詳細の表示
    auto detail_layer = CardDetailLayer::create(_touch_chara->getCharaId());
    this->addChild(detail_layer, Z_Detail_Layer, T_Detail_Layer);
}

void TournamentEventDeckChoiceLayer::setNowDeckSprite(int origin_deck_no, int previous_deck_no)
{
    //引数で与えられたデッキ番号に内容と位置を更新する
    
    //データの更新
    setData();
    
    //コピー先のデッキスプライトの内容を更新
    createCharaJsonData(previous_deck_no - 1);
    _deck_bg[previous_deck_no - 1]->refreshDeckSprite(_vec_chara_data);
    
    //引数以外のデッキスプライトを非表示にする
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        if (i != previous_deck_no - 1) _deck_bg[i]->setVisible(false);
        else _deck_bg[i]->setVisible(true);
    }
    
    //位置の設定
    auto move = MoveTo::create(0.1f, convertFromDisplaySize(Point(320, 510), 0.5, 0.5));
    auto ease = EaseOut::create(move, 2);
    _deck_bg[previous_deck_no - 1]->runAction(ease);
    
    //矢印の表示
    _left_arrow->setVisible(true);
    _right_arrow->setVisible(true);
    
    //選択中デッキNoの更新
    _deck_no = previous_deck_no;
    SaveData::saveString("party_no", toString<int>(_deck_no));
    CCLOG("previous_deck_no = %s", SaveData::loadString("party_no").c_str());
}

void TournamentEventDeckChoiceLayer::createAlertText()
{
    //警告文の表示
    string alert_text = "※このトーナメントでは使用出来ない\nカードが含まれています。";
    auto alert_label = HMSFontUtil::createWithTTFBySystemFontArgument(alert_text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(alert_text, 34, 460));
    alert_label->setScale(ratio);
    alert_label->setPosition(convertFromDisplaySize(Point(320, 140), 0.5, 0.5));
    alert_label->enableOutline(Color4B(0, 0, 0, 255), 3);
    alert_label->setVisible(false);
    this->addChild(alert_label, Z_Alert_Label, T_Alert_Label);
}

void TournamentEventDeckChoiceLayer::changeAlertLabel()
{
    //警告文字の表示、非表示
    auto alert_label = (Label *)this->getChildByTag(T_Alert_Label);
    auto join_btn = (MenuItem *)this->getChildByTag(T_Join_Menu)->getChildByTag(T_Join_MenuItem);
    
    if (_vec_alert_flg[calcArrayNo(Pos::Middle, _deck_no)]) {
        alert_label->setVisible(true);
        join_btn->setEnabled(false);
        join_btn->selected();
    }
    else {
        alert_label->setVisible(false);
        join_btn->setEnabled(true);
        join_btn->unselected();
    }
}

bool TournamentEventDeckChoiceLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan TournamentEventDeckChoiceLayer!");
    
    //タッチデータの初期化
    _touch_chara = nullptr;
    
    for (int i = 0; i <= MAX_DECK_CHARA_COUNT; i++)
    {
        auto chara = (CharaSprite *)_deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->getChildByTag(DeckFormationSprite::T_Card + i);
        if (chara)
        {
            float scale = chara->getScale() * ratio;
            Point convertedPos = chara->convertToWorldSpace(Point::ZERO);
            Rect rect = Rect(convertedPos.x, convertedPos.y, chara->getContentSize().width * scale, chara->getContentSize().height * scale);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                _touch_chara = chara;
                
                break;
            }
        }
    }
    
    //左右に画像を表示させる
    showTouchDeckSprite();
    
    //矢印の非表示
    _left_arrow->setVisible(false) ;
    _right_arrow->setVisible(false) ;
    
    return true;
}
void TournamentEventDeckChoiceLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //デッキ画像の移動
    moveTouchDeckSprite(touch->getDelta().x);
}
void TournamentEventDeckChoiceLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //動きが20px以下チェック
    if(abs(touch->getStartLocation().x - touch->getPreviousLocation().x) < 20 * ratio) {
        
        //キャラをタッチしていた場合キャラ詳細の表示
        if (_touch_chara != nullptr) showCharaDetail(0);
    }
    
    listener->setEnabled(false); //アクション中はタッチ不可にする
    
    if(touch->getStartLocation().x - touch->getLocation().x < - _contentSize.width / 3 || Utility::containsPoint(getChildByTag(T_Left_Arrow), touch->getLocation()))
    {
        //右にスワイプ（左が出てくる）
        _deck_no--;
        if (_deck_no < 1) _deck_no = MAX_DECK_COUNT;
        
        //音の設定
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_deck_move.mp3");
    }
    else if (touch->getStartLocation().x - touch->getLocation().x > _contentSize.width / 3 || Utility::containsPoint(getChildByTag(T_Right_Arrow), touch->getLocation()))
    {
        //左にスワイプ（右が出てくる）
        _deck_no++;
        if (_deck_no > MAX_DECK_COUNT) _deck_no = 1;
        
        //音の設定
        CocosDenshion::SimpleAudioEngine::SimpleAudioEngine::getInstance()->playEffect("download/SE/se_deck_move.mp3");
    }
    
    //中心点からずれていれば中心に戻す
    float move_x_distance = _contentSize.width / 2 - _deck_bg[calcArrayNo(Pos::Middle, _deck_no)]->getPosition().x;
    for (int i = 0; i < MAX_DECK_COUNT; i++)
    {
        auto move = MoveBy::create(0.3f, Point(move_x_distance, 0));
        auto ease = EaseOut::create(move, 2);
        _deck_bg[i]->runAction(ease);
    }
    
    auto hide_func = CallFunc::create([this]()
                                      {
                                          //選択中以外のデッキを見えなくする
                                          for (int i = 0; i < MAX_DECK_COUNT; i++)
                                          {
                                              if (i == calcArrayNo(Pos::Middle, _deck_no)) continue;
                                              _deck_bg[i]->setVisible(false);
                                          }
                                          
                                          //警告文の表示、非表示切り替え
                                          changeAlertLabel();
                                          
                                          listener->setEnabled(true); //タッチ可能にする
                                        
                                          _left_arrow->setVisible(true) ;
                                          _right_arrow->setVisible(true) ;
                                          
                                      });
    
    this->runAction(Sequence::create(DelayTime::create(0.3f), hide_func, nullptr));
    
    //選択中デッキNoの更新
    SaveData::saveString("party_no", toString<int>(_deck_no));
    CCLOG("deck_no = %s", SaveData::loadString("party_no").c_str());
}

void TournamentEventDeckChoiceLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
