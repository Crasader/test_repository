#include "BazaarExhibitHistoryLayer.h"

BazaarExhibitHistoryLayer::BazaarExhibitHistoryLayer()
{
}

BazaarExhibitHistoryLayer* BazaarExhibitHistoryLayer::create(json11::Json history_data)
{
    BazaarExhibitHistoryLayer *history = new BazaarExhibitHistoryLayer();
    if (history && history->init(history_data))
    {
        history->autorelease();
        return history;
    }
    CC_SAFE_DELETE(history);
    return nullptr;
}

bool BazaarExhibitHistoryLayer::init(json11::Json history_data)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("BazaarExhibitHistoryLayer::init()");
    
    ratio = getVisibleRatio();
    
    //データの設定
    this->_history_data = history_data;
    
    return true;
}

void BazaarExhibitHistoryLayer::onEnter()
{
    Layer::onEnter();
    
	//タップイベントの登録
	initEventListener();
    
    //画面の生成
    createBg();
    
    //リストの生成
    createList();
}

void BazaarExhibitHistoryLayer::createBg()
{
    //背景の生成
    auto bg = Sprite::create("init/Home/Popup/popup_500.png");
    bg->setScale(0.937*ratio, 0.996*ratio);
    bg->setPosition(convertFromDisplaySize(Point (320, 490), 0.5, 0.5));
    this->addChild(bg, Z_BG, T_BG);
    
    //コメント
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("このキャラの取引成立履歴", FONT_NAME_SYSTEM, 29);
    label->setScale(ratio);
    label->setAnchorPoint(Point(0.5, 0));
    label->setPosition(convertFromDisplaySize(Point(320, 666), 0.5, 0.5));
	label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    addChild(label, Z_Message, T_Message);
}

void BazaarExhibitHistoryLayer::createList()
{
    auto ListLayer = Layer::create();
    this->addChild(ListLayer, Z_List, T_List);
    
    int loop_count = countJsonData(_history_data);
    
    //取引履歴があったら
    if (loop_count > 0){
        createListTrue(loop_count);
    }
    //一件もなかったら
    else {
        createListFalse();
    }
}

void BazaarExhibitHistoryLayer::createListTrue(int loop_count)
{
    auto ListLayer = (Layer*)this->getChildByTag(T_List);
    float list_base_space = 96;
    for (int i=0;i<loop_count;i++){
        //ベース画像
        auto list_base = Sprite::create("init/Home/Bazaar/bk_exhibit_list.png");
        list_base->setScale(ratio);
        list_base->setPosition(convertFromDisplaySize(Point(320, 600-(list_base_space*i)), 0.5, 0.5));
        ListLayer->addChild(list_base, Z_List_base);
        
        //テキスト
        string exchange_ticket = _history_data[toString(i)]["exchange_ticket"].string_value();
        string exchange_date = _history_data[toString(i)]["insert_date"].string_value();
        
        auto date_label = HMSFontUtil::createWithTTFBySystemFontArgument(exchange_date, FONT_NAME_SYSTEM, 22);
        date_label->setAnchorPoint(Point(0, 0.5));
        date_label->setScale(ratio);
        date_label->setPosition(convertFromDisplaySize(Point(230, 618-(list_base_space*i)), 0.5, 0.5));
        date_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        date_label->setColor(Color3B(234, 245, 67));
        ListLayer->addChild(date_label, Z_List_Message);
        
        auto exchange_ticket_label = HMSFontUtil::createWithTTFBySystemFontArgument(exchange_ticket+"枚", FONT_NAME_SYSTEM, 22);
        exchange_ticket_label->setAnchorPoint(Point(0, 0.5));
        exchange_ticket_label->setScale(ratio);
        exchange_ticket_label->setPosition(convertFromDisplaySize(Point(230, 580-(list_base_space*i)), 0.5, 0.5));
        exchange_ticket_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        exchange_ticket_label->setColor(Color3B(234, 245, 67));
        ListLayer->addChild(exchange_ticket_label, Z_List_Message);
    }
}

void BazaarExhibitHistoryLayer::createListFalse()
{
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument("取引履歴はありません。", FONT_NAME_SYSTEM, 29);
    label->setScale(ratio);
    label->setAnchorPoint(Point(0.5, 0.5));
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    this->addChild(label, Z_List_Message);
}

void BazaarExhibitHistoryLayer::inPopupAction()
{
    //ポップアップを表示するアクション
    auto scale = ScaleTo::create(0.1f, 1.0);
    this->runAction(Sequence::create(DelayTime::create(0.1f), scale, nullptr));
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_on.mp3");
}

void BazaarExhibitHistoryLayer::outPopupAction()
{
    //ポップアップを隠すアクション
    auto scale = ScaleTo::create(0.1f, 0);
    this->runAction(scale);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_off.mp3");
}


void BazaarExhibitHistoryLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BazaarExhibitHistoryLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BazaarExhibitHistoryLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BazaarExhibitHistoryLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(BazaarExhibitHistoryLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool BazaarExhibitHistoryLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    HMSLOG("onTouchBegan");
    return true;
}
void BazaarExhibitHistoryLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarExhibitHistoryLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void BazaarExhibitHistoryLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}