#include "HmsScrollLayer.h"

HmsScrollLayer::HmsScrollLayer()
:_btn_touch_flg(false)
{
}

HmsScrollLayer::~HmsScrollLayer()
{
}

HmsScrollLayer* HmsScrollLayer::create(int cell_size, HmsVector<Sprite *> &item_list, function<void(int tag)> func, int top_distance)
{
    HmsScrollLayer *pRet = new HmsScrollLayer();
    
    if (pRet && pRet->init(cell_size, item_list, func, top_distance))
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

bool HmsScrollLayer::init(int cell_size, HmsVector<Sprite *> &item_list, function<void(int tag)> func, int top_distance)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("HmsScrollLayer::init()");
    
    ratio = getVisibleRatio();
    
    //メンバ変数へ保存
    _cell_size = cell_size;
    _item_list = item_list;
    _func      = func;
    _top_distance = top_distance;
    
    return true;
}

void HmsScrollLayer::onEnter()
{
    Layer::onEnter();
    
    //イベントリスナーの登録
    createEventListener();
    
    //スクロール画面の作成
    createScrollView();
}

void HmsScrollLayer::createEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(HmsScrollLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HmsScrollLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HmsScrollLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void HmsScrollLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(_contentSize.width, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio) - _top_distance));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, 10, SCROLLVIEW_TAG);
    
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _item_list.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _item_list.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio + _top_distance)))));
}

Layer* HmsScrollLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    auto layer = Layer::create();
    
    //画像の配置
    int num = 0;
    for (int i = _item_list.size()-1; 0 <= i; i--)
    {
        _item_list[num]->setScale(ratio);
        _item_list[num]->setAnchorPoint(Point(0, 0));
        _item_list[num]->setPosition(Point((320 - _item_list[num]->getContentSize().width / 2) * ratio,  (_cell_size * i)));
//        _item_list[num]->setPosition(Point(320 * ratio, (_cell_size / 2) + (_cell_size * i)));
//        _item_list[num]->setTag(_item_list[num]->getTag());
        layer->addChild(_item_list[num], 1);
        num++;
    }
    
    return layer;
}

void HmsScrollLayer::scrollViewDidScroll(cocos2d::extension::ScrollView *view)
{
    _btn_touch_flg = false;
}

void HmsScrollLayer::scrollViewDidZoom(cocos2d::extension::ScrollView *view)
{
}

bool HmsScrollLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _btn_touch_flg = true;
    
    return true;
}
void HmsScrollLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void HmsScrollLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_btn_touch_flg == true)
    {
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //spriteの絶対座標の値を取得
            auto spWorldPoint = child->convertToWorldSpace(Point::ZERO);
            
            //当たり判定作成
            Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, child->getContentSize().width * ratio, child->getContentSize().height * ratio);
            
            if (rect.containsPoint(touch->getLocation()))
            {
                HMSLOG("touch tag = %d", child->getTag());
                
                //関数の実行
                _func(child->getTag());
            }
        }
    }
}
void HmsScrollLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void HmsScrollLayer::disableSpriteTouch(int tag)
{
    auto sp = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildByTag(tag);
    
    //Size(0, 0)でしてすることでタッチ領域を無くす（AnchorPoint(0, 0)になってしまうことに注意）
    sp->setContentSize(Size(0, 0));
}