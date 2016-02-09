#include "EvolutionResultLayer.h"

bool EvolutionResultLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("EvolutionResultLayer::init()");
    initEventListener();

    return true;
}

void EvolutionResultLayer::setData(int gacha_type,int card_count,int hit_card[],int chara_list_id[],int rare_card[],string card_name_list[],int chara_type[]){
    
    setGachaType(gacha_type);
    setCardCount(card_count);
    for(int i=0;i<_card_count;i++){
        _hit_card[i]=hit_card[i];
        _chara_list_id[i]=chara_list_id[i];
        _rare_card[i]=rare_card[i];
        _card_name[i]=card_name_list[i];
        _chara_type[i]=chara_type[i];
    }
}

void EvolutionResultLayer::startAnimation(){
    auto normal_scene = (NormalScene *)this->getParent();
    if (!normal_scene) {
        return;
    }
    auto animationLayer = EvolutionResultAnimationLayer::create();//リザルトアニメーションの生成
    animationLayer->setTag(6);
    animationLayer->setData(_gachatype,_card_count,_hit_card,_rare_card,_chara_type,_chara_list_id);
    animationLayer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
    SEL_SCHEDULE s = schedule_selector(EvolutionResultLayer::afterAnimation);
    animationLayer->setScheduler(this, s);
    normal_scene->addChild(animationLayer,10);
    
    //タップ禁止レイヤー生成
    AllTapDisableLayer *allTapDisaleLayer = AllTapDisableLayer::create();
    normal_scene->addChild(allTapDisaleLayer,8,ALL_TAP_DISABLE_LAYER_TAG);
}

void EvolutionResultLayer::getSize(){
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    header_height = 140.0f;
    footer_height = 108.0f;
    main_screen_size = (visibleSize.height  / ratio - header_height - footer_height);
    main_screen_w = visibleSize.width / ratio;
    main_screen_size_split_10 = main_screen_size / 10;
    main_screen_size_split_5 = main_screen_size / 5;
}

void EvolutionResultLayer::createBg(){
    auto bg = Sprite::create("init/Home/Gacha/back2.png");
    bg->setScale(1.0f*ratio,1.2f*ratio);
    bg->setPosition(convertFromDisplaySize(Point(320, visibleSize.height / 2 /ratio), 0.5, 0.5));
    this->addChild(bg, 1);
}

void EvolutionResultLayer::onEnter(){
    Layer::onEnter();

    //ページタイトルの作成
    HeaderLayer::setupTitleBar("進化結果", "EvolutionList", this);
    
    //リザルトアニメーションの開始
    startAnimation();
    
    //サイズの取得
    getSize();
    
    //背景画像の作成
    createBg();
    
}

void EvolutionResultLayer::afterAnimation(float dt){
    //フレンドスプライトの作成
    initCardDetailCell();
    
    //スクロールビューの作成
    createScrollView();
}

void EvolutionResultLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(EvolutionResultLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(EvolutionResultLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(EvolutionResultLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(EvolutionResultLayer::onTouchCancelled, this);
    listener->setSwallowTouches(true) ;
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
bool EvolutionResultLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_chara_detail_cell.size() > 0)
    {
        //ボタンタッチフラグtrue
        touchButtonFlg = true;
    }
    
    return true;
}
void EvolutionResultLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void EvolutionResultLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto tag = 0;
    
    
    if (touchButtonFlg == true) {
        
        //タッチフラグを戻す
        touchButtonFlg = false;
        
        auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
        
        for (auto child : children)
        {
            //childをspriteにキャスト
            auto sp = (Sprite *)child;
            
            //spriteの絶対座標の値を取得
            auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
            
            
            //セルの当たり判定
            Rect cell_rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
            
            //セルのどこかをタップしたら次のページへ遷移
            if (cell_rect.containsPoint(touch->getLocation()))
            {
                showDetail(tag) ;
                HMSLOG("tag = %d", sp->getTag());
            }
            tag++;
        }
    }
}

void EvolutionResultLayer::showDetail(int tag)
{
    auto detail_layer = CardDetailLayer::create(toString<int>(_hit_card[tag]));
    this->addChild(detail_layer, 999999) ;
}

void EvolutionResultLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void EvolutionResultLayer::scrollViewDidScroll(ScrollView *view)
{
    //スクロールしたらボタンタッチをfalseにする
    touchButtonFlg = false;
}

void EvolutionResultLayer::initCardDetailCell()
{
    //ガチャでひけた詳細情報のセルの配列を初期化
    _chara_detail_cell.clear();
    
    int id = TimeMeasure::start() ;
    //詳細情報を初期化
    for(int i=0;i<_card_count;i++){
        string num = toString<int>(i);
        _chara_detail_cell.push_back(CharaDetailCellSprite::create(_chara_list_id[i]));
    }
    TimeMeasure::endAndPrint(id) ;
}

Layer* EvolutionResultLayer::createList()
{
    //初期セルサイズとスクロールビューの中のレイヤー設定
    _cell_size = 138 * ratio;
    auto layer = Layer::create();
    
    int num = 0;
    for (int i = (int)_chara_detail_cell.size()-1; 0 <= i; i--)
    {
        _chara_detail_cell[num]->setScale(ratio);
        _chara_detail_cell[num]->setTag(num);
        _chara_detail_cell[num]->setPosition(Point(320 * ratio, (_cell_size / 2) + (_cell_size * i)));
        layer->addChild(_chara_detail_cell[num], 1);
        num++;
    }
    
    return layer;
}

void EvolutionResultLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio)- (FOOTER_SIZE * ratio)));
    
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
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _chara_detail_cell.size()));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _chara_detail_cell.size()) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)))));
//    pScrollView->setContentOffset(Point(0, 0 - ((_cell_size * _chara_detail_cell.size()) - _contentSize.height)));
}
