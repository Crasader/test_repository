#include "GetGachaLayer.h"

GetGachaLayer::GetGachaLayer()
:_card_num(0)
,_action_flg(true)
{
}

bool GetGachaLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("GetGachaLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void GetGachaLayer::createEventLitener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GetGachaLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GetGachaLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GetGachaLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GetGachaLayer::setupGachaResult()
{
    //引いたガチャデータの読み込み
    string result = SaveData::loadString("tutorial_gacha");
    HMSLOG("tutorial_gacha = %s", result.c_str());
    
    string err;
    _gacha_result = Json::parse(result, err);
}

void GetGachaLayer::createCards()
{
    //裏面のカードを10枚作成
    for (int i = 0; i < 10; i++)
    {
        _card.push_back(Sprite::create("download/Chara/parts100/card_bk1_reverse.png"));
        _card[i]->setScale(ratio);
    }
}

void GetGachaLayer::initCards()
{
    //カードの配置
    int num = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int y = 0; y < 5; y++)
        {
            _card[num]->setPosition(convertFromDisplaySize(Point(CARD_1_POS_X + (CARD_DISTANCE_X * y), CARD_1_POS_Y - (CARD_DISTANCE_Y * i)), 0.5, 0.5));
            addChild(_card[num], 1);
            num++;
        }
    }
}

void GetGachaLayer::createMessage()
{
    //文字の背景画像
    message_bg = MessageSprite::create("10連ガチャの結果です。", TextHAlignment::LEFT);
    message_bg->setScale(ratio);
    message_bg->setPosition(convertFromDisplaySize(Point(320, 750), 0.5, 0.5));
    addChild(message_bg, 1, MESSAGE_SPRITE_TAG);
}

void GetGachaLayer::showGetCard(float dt)
{
    //カードを1枚ずつ表示させる
    string card_pass = "download/Chara/thumb100/"+_gacha_result[toString<int>(_card_num)].string_value()+".png";
    _card[_card_num]->setTexture(card_pass.c_str());
    _card_num++;
}

void GetGachaLayer::showGetCardAll()
{
    //カードを全て表示させる
    for (int i = 0; i < 10; i++)
    {
        string card_pass = "download/Chara/thumb100/"+_gacha_result[toString<int>(i)].string_value()+".png";
        _card[i]->setTexture(card_pass.c_str());
    }
}

void GetGachaLayer::showTouchStart()
{
    //タッチ画像の表示
    auto touch_sp = TouchScreenSprite::create();
    touch_sp->setScale(ratio);
    touch_sp->setPosition(convertFromDisplaySize(Point(320, 80), 0.5, 0));
    this->addChild(touch_sp, ZORDER_TOUCH_SCREAN, TOUCH_SCREAN_TAG);
}


void GetGachaLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    createEventLitener();
    
    //ガチャ結果の読み込み
    setupGachaResult();
    
    //初期カードの作成
    createCards();
    
    //カードの配置
    initCards();
    
    //メッセージの作成
    createMessage();
    
    //カードの表示
    this->schedule(schedule_selector(GetGachaLayer::showGetCard), 1.0f, 9, 1.5f);
}

bool GetGachaLayer::onTouchBegan(Touch *touch, Event *event)
{
    HMSLOG("GetGachaLayer! touchBegan");
    return true;
}

void GetGachaLayer::onTouchMoved(Touch *touch, Event *event)
{
    
}

void GetGachaLayer::onTouchEnded(Touch *touch, Event *event)
{
    if (_action_flg)
    {
        //カードを全て表示
        showGetCardAll();
        
        //タッチスタート表示
        showTouchStart();
        
        _action_flg = false;
    }
    else
    {
        //メッセージ画像を見えなくさせる
        message_bg->setVisible(false);
        
        //チュートリアル終了ポップアップ
        auto popup_layer = PopupTutorialEndLayer::create();
        this->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
}
