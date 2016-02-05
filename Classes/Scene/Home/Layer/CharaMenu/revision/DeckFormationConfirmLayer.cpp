#include "DeckFormationConfirmLayer.h"

DeckFormationConfirmLayer::DeckFormationConfirmLayer()
:_touch_chara_index(-1)
{
}

DeckFormationConfirmLayer* DeckFormationConfirmLayer::create(HmsVector<CharaSprite *> &deck_chara_list)
{
    DeckFormationConfirmLayer *sprite = new DeckFormationConfirmLayer();
    
    if (sprite && sprite->init(deck_chara_list))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool DeckFormationConfirmLayer::init(HmsVector<CharaSprite *> &deck_chara_list)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("CharacterListLayer::init()");
    
    ratio = getVisibleRatio();
    
    //セルリストの初期化
    _deck_chara_list = deck_chara_list;
    
    return true;
}

void DeckFormationConfirmLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの設定
    initEventListener();
    
    //背景の作成
    createBg();
    
    //タイトルの作成
    createTitle();
    
    //はい、いいえボタンの作成
    createButton();
    
    //デッキキャラ画像の作成
    createDeckChara();
    
    //ポジションインデックスの作成
    createPositionIndex();
    
    //デッキキャラの表示
    showDeckChara();
    
    //表示アクション
    Utility::openPopupAction(this);
}

void DeckFormationConfirmLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(DeckFormationConfirmLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(DeckFormationConfirmLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(DeckFormationConfirmLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(DeckFormationConfirmLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void DeckFormationConfirmLayer::createBg()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, 1);
    
    //背景
    _bg = Sprite::create("init/Home/Popup/popup_700.png");
    _bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    _bg->setScale(0);
    this->addChild(_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
    
}

void DeckFormationConfirmLayer::createTitle()
{
    //タイトルの作成
    auto title = Sprite::create("init/Home/Popup/popup_index.png");
    title->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 70));
    _bg->addChild(title);
    
    //タイトル文字
    auto titleLabel = HMSFontUtil::createWithTTFBySystemFontArgument("デッキ確認", FONT_NAME_SYSTEM,33);
    titleLabel->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    titleLabel->enableOutline(Color4B(0, 0, 0, 255), 1);
    titleLabel->setPosition(Point(_bg->getContentSize().width / 2 ,_bg->getContentSize().height - 70));
    _bg->addChild(titleLabel, Z_Title, T_Title);
}

void DeckFormationConfirmLayer::createButton()
{
    //はいボタン
    auto yes_off = Sprite::create("init/Home/Common/bt_yes.png");
    auto yes_on  = Sprite::create("init/Home/Common/bt_yes.png");
    yes_on->setColor(Color3B(180, 180, 180));
    
    auto yes_btn = MenuItemSprite::create(yes_off, yes_on, [this](Ref* sender){
        //通信処理
        sellCharaDataRequest();
    } );
    
    
    //いいえボタン
    auto no_off = Sprite::create("init/Home/Common/bt_no.png");
    auto no_on  = Sprite::create("init/Home/Common/bt_no.png");
    no_on->setColor(Color3B(180, 180, 180));
    
    auto no_btn = MenuItemSprite::create(no_off, no_on, [this](Ref* sender){
        
        //変更したデータ（並び替えたデータ）を反映させる
        auto layer = (DeckFormationLayer *)this->getParent();
        layer->setDataReturnConfirmLayer(_deck_chara_list);
        
        //閉じる処理
        Utility::closePopupAction(this);
    });
    
    auto menu = Menu::create(yes_btn, no_btn, nullptr);
    menu->alignItemsHorizontallyWithPadding(20);
    menu->setPosition(Point(_bg->getContentSize().width / 2, 60));
    _bg->addChild(menu);
    
    //変更確認メッセージ
    auto confirm_label = HMSFontUtil::createWithTTFBySystemFontArgument("デッキ内容を変更しても宜しいですか？", FONT_NAME_SYSTEM, 28);
    confirm_label->setPosition(Point(_bg->getContentSize().width / 2, 120));
    _bg->addChild(confirm_label);
}

void DeckFormationConfirmLayer::createDeckChara()
{
    //キャラ画像の作成
    for (int i = 0; i < _deck_chara_list.size(); i++)
    {
        if (_deck_chara_list[i] != nullptr) _chara.push_back(CharaSprite::create(createCharaJsonData(i), false));
    }
}

Json DeckFormationConfirmLayer::createCharaJsonData(int i)
{
    //キャラのJsonデータを作成 (C+11からの初期化子リストでコンテナの初期化を使用している)
    Json json = Json::object {
        {"chara_list_id", _deck_chara_list[i]->getCharaListId()},
        {"chara_id", _deck_chara_list[i]->getCharaId()},
        {"chara_no", _deck_chara_list[i]->getCharaNo()},
        {"second_name", _deck_chara_list[i]->getSecondName()},
        {"name", _deck_chara_list[i]->getName()},
        {"name_kana", _deck_chara_list[i]->getNameKana()},
        {"rare", _deck_chara_list[i]->getRare()},
        {"chara_type", toString<int>((int)_deck_chara_list[i]->getCharaType())},
        {"left_joint", toString<int>((int)_deck_chara_list[i]->getLeftJoint())},
        {"right_joint", toString<int>((int)_deck_chara_list[i]->getRightJoint())},
        {"hp", _deck_chara_list[i]->getHp()},
        {"power", _deck_chara_list[i]->getPower()},
        {"need_cost", _deck_chara_list[i]->getNeedCost()},
        {"add_cost", _deck_chara_list[i]->getAddCost()},
        {"turn_count", _deck_chara_list[i]->getTurnCount()},
        {"active_skill_id", _deck_chara_list[i]->getActiveSkillId()},
        {"passive_skill_id", _deck_chara_list[i]->getPassiveSkillId()},
        {"chara_comment", _deck_chara_list[i]->getCharaComment()},
        {"add_timing", _deck_chara_list[i]->getAddTiming()},
        {"insert_date", _deck_chara_list[i]->getInsertDate()},
    };
    
    return json;
}

void DeckFormationConfirmLayer::createPositionIndex()
{
    for (int i = 0; i < (_chara.size() / ROW_1_CHARA_COUNT) + 1; i++)
    {
        for (int j = 0; j < ROW_1_CHARA_COUNT; j++)
        {
            _position_index.push_back(calcPositionChara(i, j));
        }
    }
}

Point DeckFormationConfirmLayer::calcPositionChara(int i, int j)
{
    int x, y;
    
    x = POS_X_DECK_FIRST + (POS_X_DECK_DISTANCE * j);
    y = POS_Y_DECK_FIRST - (POS_Y_DECK_DISTANCE * i);
    
    return Point(x, y);
}

void DeckFormationConfirmLayer::showDeckChara()
{
    //デッキキャラの表示
    
    //背景スクエア
    _chara_bg = Sprite::create("init/Home/Quest/popup_battle_deck_square2.png");
    _chara_bg->setAnchorPoint(Point(0.5, 1.0));
    _chara_bg->setPosition(Point(_bg->getContentSize().width / 2, _bg->getContentSize().height - 120));
    _bg->addChild(_chara_bg, Z_Chara_Bg, T_Chara_Bg);
    
    //フレンドマーク
    auto friend_sp = Sprite::create("init/CharaMenu/revision/friend.png");
    friend_sp->setPosition(Point(_position_index[0].x, _position_index[0].y + 40));
    _chara_bg->addChild(friend_sp, Z_Friend, T_Friend);
    
    
    //キャラの表示
    for (int i = 0; i < _chara.size(); i++)
    {
        _chara[i]->setScale(0.8f); //枠が小さい為、少し小さく表示する
        _chara[i]->setPosition(_position_index[i]);
        _chara_bg->addChild(_chara[i], Z_Chara, T_Chara);
    }
}

string DeckFormationConfirmLayer::createRequestCharaDeckList()
{
    //リクエストするときのキャラリストの作成
    string deck_list;
    
    for (int i = 0; i < _deck_chara_list.size(); i++) {
        deck_list += _deck_chara_list[i]->getCharaListId();
        if (i != _deck_chara_list.size() - 1) deck_list += ",";
    }
    
    return deck_list;
}

void DeckFormationConfirmLayer::sellCharaDataRequest()
{
    //デッキ変更通信処理
    string url = HOST;
    url += "chara/chara_revision/set_deck_chara_list.php";
    
    string body = "user_master_id=" + SaveData::loadString("user_master_id") + "&deck_chara_list=" + createRequestCharaDeckList() + "&deck_no=" + SaveData::loadString("party_no");
    auto selector = httpresponse_selector(DeckFormationConfirmLayer::sellCharaDataCallback);
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void DeckFormationConfirmLayer::sellCharaDataCallback(HttpClient *sender, HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    if(result=="not connect"||result=="response NG") return;
    
    string err;
    Json json = Json::parse(result, err);
    
    if (json.is_object())
    {
        //初心の手引きクリアーフラグ3 デッキ編成をする
        if (SaveData::loadBool("beginner_guide_clear_flg_3") == false) {
            SaveData::saveBool("beginner_guide_clear_flg_3", true);
            SaveData::saveBool("beginner_layer_open_flg", true);
        }
        
        //パーティーデータのリスト保存
        SaveData::saveString("user_party_list", json["user_party_list"].dump());
        
        //デッキ一覧ページへ遷移
        auto normal_scene = (NormalScene *)this->getParent()->getParent();
        normal_scene->changePage("DeckFormationTop");
        
        //デッキ変更成功音
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/combo5.mp3");
    }
    else
    {
        //デッキ編成失敗
        Utility::hidePopupAction(this);
        auto func = [this](){
            Utility::openPopupAction(this);
        };
        string message = "デッキ編成に失敗しました。\n通信環境を確認の上、時間を置き\n再度お試し下さい。";
        auto popup_layer = PopupOkButtonLayer::create(func, message, false);
        this->addChild(popup_layer, 999999);
    }
}

void DeckFormationConfirmLayer::setTouchMode(int touch_index)
{
    //キャラをタッチ中の状態にする
    _chara[touch_index]->setScale(1.2f);
    _chara[touch_index]->setOpacity(200);
    _chara[touch_index]->setGlobalZOrder(1);
}

void DeckFormationConfirmLayer::setUnTouchMode(int touch_index)
{
    //タッチ中を解除する
    _chara[touch_index]->setScale(0.8f);
    _chara[touch_index]->setOpacity(255);
    _chara[touch_index]->setGlobalZOrder(0);
}

void DeckFormationConfirmLayer::changeCharaPosition(int touch_index)
{
    //タッチ先のキャラと場所を変更する
    _chara[_touch_chara_index]->setPosition(_position_index[touch_index]);

    //タッチ先のキャラをタッチキャラの場所に移動する
    auto move = MoveTo::create(0.2f, Point(_position_index[_touch_chara_index]));
    _chara[touch_index]->runAction(move);
    
    //インデックスの入れ替え
    CharaSprite* tmp = _chara[_touch_chara_index];
    _chara.set(_chara[touch_index], _touch_chara_index);
    _chara.set(tmp, touch_index);
}

void DeckFormationConfirmLayer::changeDeckCharaList(int touch_index)
{
    //デッキキャラリストの内容を入れ替える
    CharaSprite* tmp = _deck_chara_list[_touch_chara_index];
    _deck_chara_list.set(_deck_chara_list[touch_index], _touch_chara_index);
    _deck_chara_list.set(tmp, touch_index);
    
    for (int i = 0; i < _deck_chara_list.size(); i++)
    {
        if (_deck_chara_list[i] != nullptr) CCLOG("deck_chara = %s", _deck_chara_list[i]->getCharaListId().c_str());
    }
}

bool DeckFormationConfirmLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan DeckFormationConfirmLayer!");
    
    //初期化
    _touch_chara_index = -1;
    
    //デッキスプライトのどのキャラをタッチしたかチェック
    for (int i = 0; i < _chara.size(); i++)
    {
        float scale = _chara[i]->getScale() * ratio;
        Point convertedPos = _chara[i]->convertToWorldSpace(Point::ZERO);
        Rect rect_chara = Rect(convertedPos.x, convertedPos.y, _chara[i]->getContentSize().width * scale, _chara[i]->getContentSize().height * scale);
        
        if (rect_chara.containsPoint(touch->getLocation()))
        {
            //キャラをタッチ中の状態にする
            setTouchMode(i);
            
            //若干上に表示
            _chara[i]->setPosition(Point(_chara[i]->getPosition().x, _chara[i]->getPosition().y + 30));
            
            //タッチしたキャラインデックスを保持
            _touch_chara_index = i;
            
            //キャラタップ音
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/button9.mp3");
            
            return true;
        }
    }

    return true;
}

void DeckFormationConfirmLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_touch_chara_index != -1)
    {
        _chara[_touch_chara_index]->setPosition(Point(_chara[_touch_chara_index]->getPosition() + touch->getDelta() / ratio)); //ratioがかかったものの上に乗っているのでratioで割らなければいけない、注意！
    }
}

void DeckFormationConfirmLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //タッチエンドポイントにキャラが居るかチェック
    if (_touch_chara_index != -1)
    {
        //タッチ状態を解除
        setUnTouchMode(_touch_chara_index);
        
        for (int i = 0; i < _chara.size(); i++)
        {
            float scale = _chara[i]->getScale() * ratio;
            Point convertedPos = _chara[i]->convertToWorldSpace(Point::ZERO);
            Rect rect_chara = Rect(convertedPos.x, convertedPos.y, _chara[i]->getContentSize().width * scale, _chara[i]->getContentSize().height * scale);
            
            if (rect_chara.containsPoint(touch->getLocation()) && _touch_chara_index != i)
            {
                //いた場合入れ替える
                
                //画像の場所の入れ替え
                changeCharaPosition(i);
                
                //デッキキャラリストの内容を入れ替える
                changeDeckCharaList(i);
                
                //交換音
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/page.mp3");
                
                return;
            }
        }
        
        //いなかった場合元の場所に戻す
        _chara[_touch_chara_index]->setPosition(Point(_position_index[_touch_chara_index]));
        
    }
}

void DeckFormationConfirmLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
