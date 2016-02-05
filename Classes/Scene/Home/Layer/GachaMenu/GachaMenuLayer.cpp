#include "GachaMenuLayer.h"

bool GachaMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("GachaMenuLayer::init()");
    
    return true;
}
void GachaMenuLayer::getSize()
{
    ratio = getVisibleRatio();
    visibleSize = Director::getInstance()->getVisibleSize();
    header_height = 140.0f;
    footer_height = 108.0f;
    main_screen_size = (visibleSize.height  / ratio - header_height - footer_height);
    main_screen_w = visibleSize.width / ratio;
    main_screen_size_split_10 = main_screen_size / 10;
    main_screen_size_split_5 = main_screen_size / 5;
    deck_center_point = convertFromDisplaySize(Point(320, 427), 0.5, 0.5) ;
    
    initEventListener();//イベントリスナー設定
}

void GachaMenuLayer::getData()
{
    user_master_id = SaveData::loadString("user_master_id");        //ユーザーデフォルトからユーザーIDを持ってくる
    jewel_count = fromString<int>(SaveData::loadString("jewel"));   //ユーザーデフォルトから宝玉の情報を持ってくる
    gold_count = fromString<int>(SaveData::loadString("gold"));     //ユーザーデフォルトから資金の情報を持ってくる
    gacha_tab = SaveData::loadInt("gacha_tab"); //ユーザーデフォルトからガチャタブの情報を持ってくる
}

void GachaMenuLayer::setupImage()
{
    //プレミアムガチャ背景
    auto back_premium = Sprite::create("init/Home/Gacha/revision/bk_gacha_premium.png");
    back_premium->setScale(ratio);
    back_premium->setTag(PREMIUM_GACHA_TAB_IMG_TAG);
    back_premium->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,750), 0.5, 0.5));
    back_premium->setAnchorPoint(Point(0.5,1.0));
    this->addChild(back_premium,5);
    
    //スペシャルガチャ背景
    auto back_special = Sprite::create("init/Home/Gacha/revision/bk_gacha_special.png");
    back_special->setScale(ratio);
    back_special->setTag(PLUS_GACHA_TAB_IMG_TAG);
    back_special->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,750), 0.5, 0.5));
    back_special->setAnchorPoint(Point(0.5,1.0));
    this->addChild(back_special,5);

    //無料ガチャ背景
    auto back_free = Sprite::create("init/Home/Gacha/revision/bk_gacha_free.png");
    back_free->setScale(ratio);
    back_free->setTag(FREE_GACHA_TAB_IMG_TAG);
    back_free->setOpacity(0);
    back_free->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,750), 0.5, 0.5));
    back_free->setAnchorPoint(Point(0.5,1.0));
    this->addChild(back_free,5);
    
    //ガチャバナー
    auto single_banner = Sprite::create(premium_fix_banner_file_path);
    single_banner->setScale(ratio);
    single_banner->setTag(GACHA_BANNER_TAG);
    single_banner->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,324), 0.5, 0.5));
    single_banner->setAnchorPoint(Point(0.5,0.5));
    this->addChild(single_banner,GACHA_BANNER_Z);
    if(this->gacha_tab==2) single_banner->setVisible(false);
    
    //プライス(現在未使用)
    price_label = HMSFontUtil::createWithTTFBySystemFontArgument("１回１宝玉！！", FONT_NAME_SYSTEM, 28);
    price_label->setScale(ratio);
    price_label->setTag(GACHA_BANNER_TAG);
    price_label->setPosition(convertFromDisplaySize(Point(145,205), 0.5, 0.5));
    price_label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    //enableOutline(Color4B(0, 0, 0, 255), 1);
//    this->addChild(price_label, 18);
}

void GachaMenuLayer::setupButton()
{
    //ページタイトルの作成
    if(gacha_tab == 1){
        HeaderLayer::setupTitleBar("無料ガチャ", "RevisionGachaMenu", this);
    }else if(gacha_tab == 2) {
        HeaderLayer::setupTitleBar("プレミアムガチャ", "RevisionGachaMenu", this);
    }else if(gacha_tab == 3) {
        HeaderLayer::setupTitleBar("スペシャルガチャ", "RevisionGachaMenu", this);
    }
    
	//スペシャルガチャの開催フラグ
	bool special_gacha_flg = SaveData::loadBool("special_gacha_flg");
	//スペシャルガチャのガチャID
	int special_gacha_id = SaveData::loadInt("special_gacha_id");
	if(special_gacha_id == 0) special_gacha_id = 3;//スペシャルガチャが開催していない時、デフォルトでgacha_id=3を入れる。
    
    //出現割合ボタン
    gachaRateBtn();

//    //ガチャタブボタン(課金)
//    auto premium_sprite = MenuItemImage::create("download/Gacha/new_gacha_img/gacha_tab_pay_gray.png","download/Gacha/new_gacha_img/gacha_tab_pay_gray.png", CC_CALLBACK_1(GachaMenuLayer::changeGachaTab_sender, this));
//    premium_sprite->setScale(ratio);
//    premium_sprite->setAnchorPoint(Point(0.5, 1.0));
//    premium_sprite->setTag(PREMIUM_GACHA_TAB_TAG);
//    auto premium_menu_bt = Menu::create(premium_sprite, nullptr);
//    premium_menu_bt->setPosition(convertFromDisplaySize(Point(320,747), 0.5, 0.5));
//    this->addChild(premium_menu_bt, 4);
//    
//    //ガチャタブボタン(スペシャルガチャ)
//    auto plus_sprite = MenuItemImage::create("download/Gacha/new_gacha_img/gacha_tab_plus_gray.png","download/Gacha/new_gacha_img/gacha_tab_plus_gray.png", CC_CALLBACK_1(GachaMenuLayer::changeGachaTab_sender, this));
//    plus_sprite->setScale(ratio);
//    plus_sprite->setAnchorPoint(Point(0.5, 1.0));
//    plus_sprite->setTag(PLUS_GACHA_TAB_TAG);
//    auto plus_menu_bt = Menu::create(plus_sprite, nullptr);
//    plus_menu_bt->setPosition(convertFromDisplaySize(Point(108,747), 0.5, 0.5));
//    this->addChild(plus_menu_bt, 4);
    
//    if(special_gacha_flg==false) {
//        
//    } else {
    //開催中という表示
//        Sprite *img = Sprite::create("download/Gacha/gacha_insession.png");
//        img->setScale(ratio);
//        img->setPosition(convertFromDisplaySize(Point(113, 742), 0.5, 0.5));
//        this->addChild(img, 10);
//    }
    
//    //ガチャタブボタン(無料)
//    auto free_sprite = MenuItemImage::create("download/Gacha/new_gacha_img/gacha_tab_free_gray.png","download/Gacha/new_gacha_img/gacha_tab_free_gray.png", CC_CALLBACK_1(GachaMenuLayer::changeGachaTab_sender, this));
//    free_sprite->setScale(ratio);
//    free_sprite->setAnchorPoint(Point(0.5, 1.0));
//    free_sprite->setTag(FREE_GACHA_TAB_TAG);
//    auto free_menu_bt = Menu::create(free_sprite, nullptr);
//    free_menu_bt->setPosition(convertFromDisplaySize(Point(532,747), 0.5, 0.5));
//    this->addChild(free_menu_bt, 4);
    
    //-------------- 無料ガチャボタン生成 ----------------
     //ガチャ確認画面表示ボタン(1連 無料)
    auto confirmation_single2 = MenuItemImage::create("init/Home/Gacha/revision/bt_gacha_free1.png","init/Home/Gacha/revision/bt_gacha_free1.png", CC_CALLBACK_1(GachaMenuLayer::popupConfirmation, this));
    confirmation_single2->setScale(ratio);
    confirmation_single2->setAnchorPoint(Point(0.5, 0.5));
    confirmation_single2->setTag(FREE_POPUP_TAG);
    auto confirmation_single_menu2 = Menu::create(confirmation_single2, nullptr);
    confirmation_single_menu2->setPosition(convertFromDisplaySize(Point(187, 200), 0.5, 0.5));
    confirmation_single_menu2->setAnchorPoint(Point(0.5,1.0));
    this->addChild(confirmation_single_menu2, BUTTON_POPUP_Z, FREE_POPUP_TAG);
    
    //ガチャ確認画面表示ボタン(10連 無料)
    auto confirmation_ten2 = MenuItemImage::create("init/Home/Gacha/revision/bt_gacha_free10.png", "init/Home/Gacha/revision/bt_gacha_free10.png", CC_CALLBACK_1(GachaMenuLayer::popupConfirmation, this));
    confirmation_ten2->setScale(ratio);
    confirmation_ten2->setAnchorPoint(Point(0.5, 0.5));
    confirmation_ten2->setTag(FREE_TENCONTINUOUS_POPUP_TAG);
    auto confirmation_ten_menu2 = Menu::create(confirmation_ten2, nullptr);
    confirmation_ten_menu2->setPosition(convertFromDisplaySize(Point(453, 200), 0.5, 0.5));
    confirmation_ten_menu2->setAnchorPoint(Point(0.5,1.0));
    this->addChild(confirmation_ten_menu2, BUTTON_POPUP_Z, FREE_TENCONTINUOUS_POPUP_TAG);
    
    //-------------- プレミアムガチャボタン生成 ----------------
    //ガチャ確認画面表示ボタン(1連 課金)
    auto confirmation_single = MenuItemImage::create("init/Home/Gacha/revision/bt_gacha_premium.png","init/Home/Gacha/revision/bt_gacha_premium.png", CC_CALLBACK_1(GachaMenuLayer::popupConfirmation, this));
    confirmation_single->setScale(ratio);
    confirmation_single->setAnchorPoint(Point(0.5, 0.5));
    confirmation_single->setTag(SINGLE_POPUP_TAG);
    auto confirmation_single_menu = Menu::create(confirmation_single, nullptr);
    confirmation_single_menu->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 324), 0.5, 0.5));
    confirmation_single_menu->setAnchorPoint(Point(0.5,1.0));
    this->addChild(confirmation_single_menu, BUTTON_POPUP_Z, SINGLE_POPUP_TAG);
    
    //ガチャ確認画面表示ボタン(10連 課金)
    auto confirmation_ten = MenuItemImage::create("init/Home/Gacha/revision/bt_gacha_premium10.png", "init/Home/Gacha/revision/bt_gacha_premium10.png", CC_CALLBACK_1(GachaMenuLayer::popupConfirmation, this));
    confirmation_ten->setScale(ratio);
    confirmation_ten->setAnchorPoint(Point(0.5, 0.5));
    confirmation_ten->setTag(TENCONTINUOUS_POPUP_TAG);
    auto confirmation_ten_menu = Menu::create(confirmation_ten, nullptr);
    confirmation_ten_menu->setPosition(convertFromDisplaySize(Point(187, 200), 0.5, 0.5));
    confirmation_ten_menu->setAnchorPoint(Point(0.5,1.0));
    this->addChild(confirmation_ten_menu, BUTTON_POPUP_Z, TENCONTINUOUS_POPUP_TAG);
    
    //ガチャ確認画面表示ボタン(30連 課金)
    auto confirmation_30 = MenuItemImage::create("init/Home/Gacha/revision/bt_gacha_premium30.png", "init/Home/Gacha/revision/bt_gacha_premium30.png", CC_CALLBACK_1(GachaMenuLayer::popupConfirmation, this));
    confirmation_30->setScale(ratio);
    confirmation_30->setAnchorPoint(Point(0.5, 0.5));
    confirmation_30->setTag(CONTINUOUS30_POPUP_TAG);
    auto confirmation_30_menu = Menu::create(confirmation_30, nullptr);
    confirmation_30_menu->setPosition(convertFromDisplaySize(Point(453, 200), 0.5, 0.5));
    confirmation_30_menu->setAnchorPoint(Point(0.5,1.0));
    this->addChild(confirmation_30_menu, BUTTON_POPUP_Z, CONTINUOUS30_POPUP_TAG);
    
    
    //-------------- スペシャルガチャボタン生成 ----------------
    //ガチャ確認画面表示ボタン(30連 スペシャルガチャ)
    auto plus_30 = MenuItemImage::create("download/Gacha/new_gacha_img/" + to_string(special_gacha_id) + "/bt_gacha_revision.png", "download/Gacha/new_gacha_img/" + to_string(special_gacha_id) + "/bt_gacha_revision.png", CC_CALLBACK_1(GachaMenuLayer::popupConfirmation, this));
    plus_30->setScale(ratio);
    plus_30->setAnchorPoint(Point(0.5, 0.5));
    plus_30->setTag(PLUS_30_POPUP_TAG);
    auto plus_30_menu = Menu::create(plus_30, nullptr);
    plus_30_menu->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2,185), 0.5, 0.5));
    plus_30_menu->setAnchorPoint(Point(0.5,1.0));
    this->addChild(plus_30_menu, BUTTON_POPUP_Z, PLUS_30_POPUP_TAG);
    
//    //説明画像
//    Sprite *img;
//    Point p;
    Layer *layer_free = Layer::create();
    this->addChild(layer_free, BUTTON_POPUP_Z, FREE_GACHA_LAYER_TAG);
    
    Layer *layer_premium = Layer::create();
    this->addChild(layer_premium, BUTTON_POPUP_Z, PREMIUM_GACHA_LAYER_TAG);
    
    Layer *layer_plus = Layer::create();
    this->addChild(layer_plus, BUTTON_POPUP_Z, PLUS_GACHA_LAYER_TAG);
//
//    if(special_gacha_flg==false) {
//        img->setVisible(false);
//    }
    
    //固定バナー
    string filename = "download/Gacha/new_gacha_img/" + to_string(special_gacha_id) + "/gacha_detail.png";
    int y = 309;
    if(special_gacha_flg==false) {
        filename = "download/Gacha/banner_gacha_waiting.png";
        y = 395;
    }
    Sprite *img = Sprite::create(filename.c_str());
    img->setScale(ratio);
    img->setPosition(convertFromDisplaySize(Point(320, y), 0.5, 0.5));
    layer_plus->addChild(img);
}

void GachaMenuLayer::gachaRateBtn()
{
    //出現確率ボタン
    auto gachaRateBtn_off = Sprite::create("init/Home/Gacha/bt_probability3.png");
    auto gachaRateBtn_on = Sprite::create("init/Home/Gacha/bt_probability3.png");
    gachaRateBtn_on->setColor(Color3B(180, 180, 180));

    auto gachaRateBtn = MenuItemSprite::create(gachaRateBtn_off, gachaRateBtn_on, [&](Ref* sender)
    {
        auto normal_scene = (NormalScene *)this->getParent();
        
        string url = HOST;
        if(gacha_tab == 1){
            //無料ガチャページ
            url += "user/index.php?p=page__gacha_rate_free&user_id=" + SaveData::loadString("user_master_id");
        }else if(gacha_tab == 2) {
            //プレミアムページ
            url += "user/index.php?p=page__gacha_rate&user_id=" + SaveData::loadString("user_master_id");
        }else if(gacha_tab == 3) {
            //スペシャルガチャのページ
            url += "user/index.php?p=page__gacha_rate_plus&user_id=" + SaveData::loadString("user_master_id");
        }
        
        auto WebViewLoadLayer = WebViewLoadLayer::create(url, "キャラクター出現確率");
        normal_scene->addChild(WebViewLoadLayer, 10);

    });
    gachaRateBtn->setScale(ratio);

    Menu* gachaRateMenu = Menu::create(gachaRateBtn, nullptr);
    gachaRateMenu->setPosition(convertFromDisplaySize(Point(547,788), 0.5, 1));
    this->addChild(gachaRateMenu);
}


//タップ可能か   タップしてから離してリストの位置が定位置にいくまでの間false
bool GachaMenuLayer::getEnableTap(){
    return _enable_tap;
}

void GachaMenuLayer::setEnableTap(bool b){
    _enable_tap = b;
}

int GachaMenuLayer::calcLeftDeckIndex(int center_index){
    int leftDeckIndex = (center_index - 1) % DETAIL_ARRAY_SIZE ;
    if(leftDeckIndex<0)leftDeckIndex = leftDeckIndex + DETAIL_ARRAY_SIZE ;
    return leftDeckIndex ;
}

int GachaMenuLayer::calcRightDeckIndex(int center_index){
    int rightDeckIndex = (center_index + 1) % DETAIL_ARRAY_SIZE ;
    return rightDeckIndex ;
}

ClippingNode *GachaMenuLayer::createMascByTargetSprite(Node *masc_target_node)
{
    //与えられたNodeのサイズの大きさのマスクを生成する
    Size deck_size = masc_target_node->getContentSize() ;
    
    //マスクの形状を取得する
    DrawNode *shape = this->getMascShapeByTargetSprite(masc_target_node);
    
    ClippingNode *clipper = ClippingNode::create();
    clipper->setAnchorPoint(Point(0.5, 1.0));
    //clipper->setPosition(convertFromDisplaySize(_deckSelectCenterPosition, 0.5, 1));
    clipper->setPosition(deck_center_point);
    
    clipper->setStencil(shape);
    clipper->setScale(ratio);
    
    return clipper ;
}

DrawNode* GachaMenuLayer::getMascShapeByTargetSprite(Node *masc_target_node)
{
    float deck_width = masc_target_node->getContentSize().width ;
    float deck_height = masc_target_node->getContentSize().height;
    
    DrawNode *shape = DrawNode::create();
    static Vec2 square[4];
    square[0] = Vec2(-deck_width/2, -deck_height/2);
    square[1] = Vec2(deck_width/2, -deck_height/2);
    square[2] = Vec2(deck_width/2, deck_height/2);
    square[3] = Vec2(-deck_width/2, deck_height/2);
    
    static Color4F green(0, 1, 0, 1);
    shape->drawPolygon(square, 4, green, 0, green);
    return shape;
}

void GachaMenuLayer::initBanner(){

    //スワイプ式のガチャバナーを生成(無料)
    HmsVector<Sprite*> contents_free ;
    
    for(int i=0 ; i<free_slide_banner_file_paths.size() ; i++){
        Sprite *sp_free_banner = Sprite::create(free_slide_banner_file_paths[i]);//1スプライト
        contents_free.push_back(sp_free_banner);
    }
    
    contents_slide_sprite_free = BannerContentsSlideSprite::create(contents_free, BannerContentsSlideSprite::ScrollDirection::HORIZONTAL);
    contents_slide_sprite_free->setAnchorPoint(Point(0.5, 0.5));
    contents_slide_sprite_free->setPosition(convertFromDisplaySize(Point(320, 545), 0.5, 0.5));
    contents_slide_sprite_free->setScale(ratio);
    
    this->addChild(contents_slide_sprite_free, GACHA_LAYER_Z) ;
    contents_slide_sprite_free->setVisible(false) ; //最初は無料ガチャは表示しない
    
    
    //スワイプ式のガチャバナーを生成(有料)
    HmsVector<Sprite*> contents_premium ;
    for(int i=0 ; i<premium_slide_banner_file_paths.size() ; i++){
        Sprite *sp_premium_banner = Sprite::create(premium_slide_banner_file_paths[i]);//1スプライト
        contents_premium.push_back(sp_premium_banner);
    }
    
    contents_slide_sprite_premium = BannerContentsSlideSprite::create(contents_premium, BannerContentsSlideSprite::ScrollDirection::HORIZONTAL) ;
    contents_slide_sprite_premium->setAnchorPoint(Point(0.5, 0.5));
    contents_slide_sprite_premium->setPosition(deck_center_point.x, deck_center_point.y+100*ratio);
    contents_slide_sprite_premium->setScale(ratio);
    
    this->addChild(contents_slide_sprite_premium, GACHA_LAYER_Z) ;
    
    //スワイプ式のガチャバナーを生成(有料UR+)
    HmsVector<Sprite*> contents_plus ;
    for(int i=0 ; i<plus_slide_banner_file_paths.size() ; i++){
        Sprite *sp_plus_banner = Sprite::create(plus_slide_banner_file_paths[i]);//1スプライト
        contents_plus.push_back(sp_plus_banner);
    }
    
    contents_slide_sprite_plus = BannerContentsSlideSprite::create(contents_plus, BannerContentsSlideSprite::ScrollDirection::HORIZONTAL) ;
    contents_slide_sprite_plus->setAnchorPoint(Point(0.5, 0.5));
    contents_slide_sprite_plus->setPosition(deck_center_point.x, deck_center_point.y+105*ratio);
    contents_slide_sprite_plus->setScale(ratio);
    
    this->addChild(contents_slide_sprite_plus, GACHA_LAYER_Z) ;
    
}

void GachaMenuLayer::initArrowSprite(){
    
    Size detail_size = getNowTabSlideBanner()->getContentSize() ;
    
    float helf_detail_size_x = detail_size.width / 2 ;
    float arrow_left_point = deck_center_point.x - helf_detail_size_x*ratio + 14*ratio ; //画像の余白分足す(14px)
    float arrow_right_point = deck_center_point.x + helf_detail_size_x*ratio;
    float arrow_y = deck_center_point.y  ;
    
    //左の矢印
    Sprite *left_arrow = Sprite::create("init/CharaMenu/arrow_left.png");
    left_arrow->setScale(ratio);
    left_arrow->setAnchorPoint(Point(1, 0.5)) ;
    left_arrow->setPosition(Point(arrow_left_point , arrow_y));
    this->addChild(left_arrow, GACHA_LAYER_Z, TAG_LEFT_ARROW) ;
    
    //右の矢印
    Sprite *right_arrow = Sprite::create("init/CharaMenu/arrow_right.png");
    right_arrow->setScale(ratio);
    right_arrow->setAnchorPoint(Point(0, 0.5)) ;
    right_arrow->setPosition(Point(arrow_right_point, arrow_y));
    this->addChild(right_arrow, GACHA_LAYER_Z, TAG_RIGHT_ARROW) ;
}

void GachaMenuLayer::repeatArrowsAction(){
    
    Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
    Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
    
    left_arrow->stopAllActions();
    right_arrow->stopAllActions();
    
    MoveBy *left_arrow_move = MoveBy::create(0.6f, Point(-20,0)) ;
    ActionEase *left_arrow_move_ease_in = EaseExponentialIn::create(left_arrow_move);
    ActionEase *left_arrow_move_ease_in_back = left_arrow_move_ease_in->reverse();
    MoveTo *left_arrow_move_default = MoveTo::create(0.01, left_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *left_arrow_forever_action = RepeatForever::create(Sequence::create(left_arrow_move_ease_in, left_arrow_move_ease_in_back, left_arrow_move_default, nullptr)) ;
    
    MoveBy *right_arrow_move = MoveBy::create(0.6f, Point(+20,0)) ;
    ActionEase *right_arrow_move_ease_in = EaseExponentialIn::create(right_arrow_move);
    ActionEase *right_arrow_move_ease_in_back = right_arrow_move_ease_in->reverse();
    MoveTo *right_arrow_move_default = MoveTo::create(0.01, right_arrow->getPosition()) ; //相対位置で移動させ続けると少しずつずれていくのでこれを最後に挟んで位置を補正する
    RepeatForever *right_arrow_forever_action = RepeatForever::create(Sequence::create(right_arrow_move_ease_in, right_arrow_move_ease_in_back, right_arrow_move_default, nullptr)) ;
    
    left_arrow->runAction(left_arrow_forever_action) ;
    right_arrow->runAction(right_arrow_forever_action) ;
}

void GachaMenuLayer::showArrows(float dt){
    Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
    Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
    
    left_arrow->setVisible(true) ;
    right_arrow->setVisible(true) ;
}

void GachaMenuLayer::hideArrows(){
    Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
    Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
    
    left_arrow->setVisible(false) ;
    right_arrow->setVisible(false) ;
}


void GachaMenuLayer::popupConfirmation(Ref* sender)
{
    MenuItem* menuItem = (MenuItem *)sender;
    int tag = menuItem->getTag();
    
    HMSLOG("tag=%d", tag);
    auto normal_scene = (NormalScene *)this->getParent();
    auto GachaConfirmationLayer = GachaConfirmationLayer::create(tag);//GachaConfirmationLayer呼び出し
    GachaConfirmationLayer->setScale(0*ratio);
    GachaConfirmationLayer->setPosition(convertFromDisplaySize(Point(0, 0), 0, 0));
    normal_scene->addChild(GachaConfirmationLayer,ZORDER_SHADOW+1);
}



void GachaMenuLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GachaMenuLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GachaMenuLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GachaMenuLayer::onTouchEnded, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GachaMenuLayer::autoSwipeLeft()
{
    getNowTabSlideBanner()->autoSwipePrev() ;
}

void GachaMenuLayer::autoSwipeRight()
{
    getNowTabSlideBanner()->autoSwipeNext() ;
}

void GachaMenuLayer::autoSwipeCenter()
{
    getNowTabSlideBanner()->autoSwipeCenter() ;
}

void GachaMenuLayer::changeGachaTab_sender(Ref* sender)
{
    MenuItem* menuItem = (MenuItem *)sender;
    int tag_no = menuItem->getTag();
    
    changeGachaTab(tag_no);
}

void GachaMenuLayer::changeGachaTab(int tab_no)
{
    Sprite* gacha_tab1 = (Sprite *)this->getChildByTag(PREMIUM_GACHA_TAB_IMG_TAG);   //プレミアムガチャタブ
    Sprite* gacha_tab2 = (Sprite *)this->getChildByTag(FREE_GACHA_TAB_IMG_TAG);      //フリーガチャタブ
    Sprite* gacha_tab3 = (Sprite *)this->getChildByTag(PLUS_GACHA_TAB_IMG_TAG);      //スペシャルガチャタブ

    MenuItem* single_popup_bt = (MenuItem *)this->getChildByTag(SINGLE_POPUP_TAG);   //最終確認画面ポップボタン
    MenuItem* free_popup_bt = (MenuItem *)this->getChildByTag(FREE_POPUP_TAG);
    MenuItem* tencontinuous_popup_bt = (MenuItem *)this->getChildByTag(TENCONTINUOUS_POPUP_TAG);
    MenuItem* free_tencontinuous_popup_bt = (MenuItem *)this->getChildByTag(FREE_TENCONTINUOUS_POPUP_TAG);
    MenuItem* continuous30_popup_bt = (MenuItem *)this->getChildByTag(CONTINUOUS30_POPUP_TAG);
    MenuItem* plus_1_popup_bt = (MenuItem *)this->getChildByTag(PLUS_1_POPUP_TAG);
    MenuItem* plus_30_popup_bt = (MenuItem *)this->getChildByTag(PLUS_30_POPUP_TAG);
    
    Layer *layer_free = (Layer *)this->getChildByTag(FREE_GACHA_LAYER_TAG);
    Layer *layer_premium = (Layer *)this->getChildByTag(PREMIUM_GACHA_LAYER_TAG);
    Layer *layer_plus = (Layer *)this->getChildByTag(PLUS_GACHA_LAYER_TAG);
	
	Size detail_size = getNowTabSlideBanner()->getContentSize() ;
	float helf_detail_size_x = detail_size.width / 2 ;
	float arrow_left_point = deck_center_point.x - helf_detail_size_x*ratio + 14*ratio ; //画像の余白分足す(14px)
	float arrow_right_point = deck_center_point.x + helf_detail_size_x*ratio;
	
    string fix_banner_file_path ;
    if(tab_no==PREMIUM_GACHA_TAB_TAG){//課金ガチャタブをタッチした場合
        
        if(gacha_tab!=2)CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/taiko_2.mp3",false);
        gacha_tab=2;//表示ガチャタブの切り替え
        //タブ画像の切り替え
        gacha_tab1->setOpacity(255);
        gacha_tab2->setOpacity(0);
        gacha_tab3->setOpacity(0);
        //最終確認画面ポップボタンの切り替え
        single_popup_bt->setVisible(true);
        free_popup_bt->setVisible(false);
        tencontinuous_popup_bt->setVisible(true);
        free_tencontinuous_popup_bt->setVisible(false);
        continuous30_popup_bt->setVisible(true);
        //plus_1_popup_bt->setVisible(false);
        plus_30_popup_bt->setVisible(false);
        
        float arrow_y = deck_center_point.y;
        Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
        Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
        left_arrow->setPosition(Point(arrow_left_point,arrow_y+100*ratio));
        right_arrow->setPosition(Point(arrow_right_point,arrow_y+100*ratio));
        repeatArrowsAction();
        
        contents_slide_sprite_premium->setVisible(true) ;
        contents_slide_sprite_free->setVisible(false) ;
        contents_slide_sprite_plus->setVisible(false) ;
        fix_banner_file_path = premium_fix_banner_file_path ;
        
        layer_free->setVisible(false);
        layer_premium->setVisible(true);
        layer_plus->setVisible(false);
        
    }else if(tab_no==FREE_GACHA_TAB_TAG){//無料ガチャタブをタッチした場合
        if(gacha_tab!=1)CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/taiko_2.mp3",false);
        gacha_tab=1;//表示ガチャタブの切り替え
        //タブ画像の切り替え
        gacha_tab1->setOpacity(0);
        gacha_tab2->setOpacity(255);
        gacha_tab3->setOpacity(0);
        //最終確認画面ポップボタンの切り替え
        single_popup_bt->setVisible(false);
        free_popup_bt->setVisible(true);
        tencontinuous_popup_bt->setVisible(false);
        free_tencontinuous_popup_bt->setVisible(true);
        continuous30_popup_bt->setVisible(false);
        //plus_1_popup_bt->setVisible(false);
        plus_30_popup_bt->setVisible(false);
        
        float arrow_y = deck_center_point.y+118*ratio;
        Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
        Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
		left_arrow->setPosition(Point(arrow_left_point,arrow_y));
		right_arrow->setPosition(Point(arrow_right_point,arrow_y));
        repeatArrowsAction();

        contents_slide_sprite_premium->setVisible(false) ;
        contents_slide_sprite_free->setVisible(true) ;
        contents_slide_sprite_plus->setVisible(false) ;
        
        fix_banner_file_path = free_fix_banner_file_path;
        
        layer_free->setVisible(true);
        layer_premium->setVisible(false);
        layer_plus->setVisible(false);
    }else if(tab_no==PLUS_GACHA_TAB_TAG){//課金UR+ガチャタブをタッチした場合
        if(gacha_tab!=3)CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/taiko_2.mp3",false);
        gacha_tab=3;//表示ガチャタブの切り替え
        //タブ画像の切り替え
        gacha_tab1->setOpacity(0);
        gacha_tab2->setOpacity(0);
        gacha_tab3->setOpacity(255);
        //最終確認画面ポップボタンの切り替え
        single_popup_bt->setVisible(false);
        free_popup_bt->setVisible(false);
        tencontinuous_popup_bt->setVisible(false);
        free_tencontinuous_popup_bt->setVisible(false);
        continuous30_popup_bt->setVisible(false);
        //plus_1_popup_bt->setVisible(true);
        plus_30_popup_bt->setVisible(true);
        
        float arrow_y = deck_center_point.y;
        Sprite *left_arrow = (Sprite *)this->getChildByTag(TAG_LEFT_ARROW) ;
        Sprite *right_arrow = (Sprite *)this->getChildByTag(TAG_RIGHT_ARROW) ;
		left_arrow->setPosition(Point(arrow_left_point,arrow_y+100*ratio));
		right_arrow->setPosition(Point(arrow_right_point,arrow_y+100*ratio));
        repeatArrowsAction();
        
        contents_slide_sprite_premium->setVisible(false) ;
        contents_slide_sprite_free->setVisible(false) ;
        contents_slide_sprite_plus->setVisible(true) ;

        fix_banner_file_path = plus_fix_banner_file_path;
        
        layer_free->setVisible(false);
        layer_premium->setVisible(false);
        layer_plus->setVisible(true);
        
        if(SaveData::loadBool("special_gacha_flg")==false) {
            plus_30_popup_bt->setVisible(false);
            
            left_arrow->setVisible(false);
            right_arrow->setVisible(false);

            contents_slide_sprite_plus->setVisible(false);
        }
    }
    
    if(getNowTabSlideBanner()->getBannerSize() == 1) {
        //バナーが一つのとき、矢印を表示しない
        hideArrows() ;
    }else{
        //バナーが複数のとき、矢印を表示しない
        showArrows(0) ;
    }
    
    //画像の切り替え
    Sprite* banner = (Sprite *)this->getChildByTag(GACHA_BANNER_TAG);
    banner->setTexture(fix_banner_file_path);//バナー(上)
    if(this->gacha_tab==1) banner->setVisible(true);
    else banner->setVisible(false);
    

    //ガチャタブ情報の保存
    SaveData::saveInt("gacha_tab",gacha_tab);
}

void GachaMenuLayer::showGachaTab(int tab_no)
{
    
    if(tab_no==2)
    {
        changeGachaTab(PREMIUM_GACHA_TAB_TAG);
    }else if(tab_no==1)
    {
        changeGachaTab(FREE_GACHA_TAB_TAG);
    }else if(tab_no==3)
    {
        changeGachaTab(PLUS_GACHA_TAB_TAG);
    }else //何もガチャタブ情報がない場合プレミアガチャを表示
    {
        changeGachaTab(PREMIUM_GACHA_TAB_TAG);
    }
    
}


bool GachaMenuLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(containDetail(touch->getLocation())){
        //スライド式バナーがタップされた場合autoSwipeScheduleを解除
        this->unschedule(schedule_selector(GachaMenuLayer::autoSwipeUpdate));
    }
    
    if(checkContainTouchPointByTag(touch->getLocation(), TAG_RIGHT_ARROW)){
        //タップした位置が 右の矢印だった場合
//        hideArrows() ; //矢印を見えなくする
//        setEnableTap(false) ; //タップ不可能にする
        
        autoSwipeLeft();
        return false ;
    }else if(checkContainTouchPointByTag(touch->getLocation(), TAG_LEFT_ARROW)){
        //タップした位置が 左の矢印だった場合
//        hideArrows() ; //矢印を見えなくする
//        setEnableTap(false) ; //タップ不可能にする
        
        autoSwipeRight();
        return false ;
    }
    
    HMSLOG("onTouchBegan");
    if(containDetail(touch->getLocation())){
        return true; //デッキの高さに含まれるとき他のonTouch~に処理を伝搬させる
    }else{
        return false;
    }
}

bool GachaMenuLayer::checkContainTouchPointByTag(Point touch_point, int tag){
    Node *target_sprite = getChildByTag(tag) ;
    
    Rect targetBox = target_sprite->getBoundingBox();
    
    //touchPointがtargetBoxの中に含まれているか判定
    if (targetBox.containsPoint(touch_point)) return true;
    else return false ;
}

void GachaMenuLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    getNowTabSlideBanner()->moveDelta(touch->getDelta()/ratio) ;
}

BannerContentsSlideSprite *GachaMenuLayer::getNowTabSlideBanner(){
    if(gacha_tab==1){
        return contents_slide_sprite_free;
    }else if(gacha_tab==2){
        return contents_slide_sprite_premium;
    }else if(gacha_tab==3){
        return contents_slide_sprite_plus;
    }
}

void GachaMenuLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //最初のタップからの移動距離を求める
    float diff_x = touch->getLocation().x - touch->getStartLocation().x ;
    
    //画面サイズの横幅
    float visibleWidth = Director::getInstance()->getVisibleSize().width;
    
    if(diff_x >= visibleWidth/3){
        // 画面の 1/3 以上移動していたら、右にスワイプされたものと見なす
        autoSwipeRight() ;
    }else if(diff_x <= -visibleWidth/3){
        // 画面の 1/3 以上移動していたら、左にスワイプされたものと見なす
        autoSwipeLeft() ;
    }else{
        //他の条件に当てはまらない場合、画像を中心位置に戻す
        autoSwipeCenter() ;
    }
    
}

bool GachaMenuLayer::containDetail(Point point){
    Sprite *sprite = getNowTabSlideBanner();
    
    float detail_center_y = deck_center_point.y ;
    float detail_height = sprite->getContentSize().height * ratio ;
    
    float detail_top_y = detail_center_y +  detail_height / 2 ;
    float detail_bottom_y = detail_center_y - detail_height / 2 ;
    
    if(point.y < detail_top_y && point.y > detail_bottom_y){
        return true ;
    }else{
        return false ;
    }
}
void GachaMenuLayer::maintenanceRequest()
{
	//メンテ・課金可能確認
	string user_master_id = SaveData::loadString("user_master_id");
	string url = HOST;
	url += "user/pay_check.php";
	auto selector = httpresponse_selector(GachaMenuLayer::maintenanceCallback);
	string body = "user_master_id="+user_master_id;
	HttpConnect::sendHttpRequest(this, url, selector, body);
}

void GachaMenuLayer::maintenanceCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
	//データ取得コールバック
	string result = HttpConnect::getHttpResponse(sender, response);
	
	//通信エラー以外
	if (result != "not connect" && result != "response NG"){
		gachaRequest();
	}
}

void GachaMenuLayer::gachaRequest()
{
    string user_master_id = SaveData::loadString("user_master_id");
	string url = HOST;
    url += "user/gacha_data.php";
    auto selector = httpresponse_selector(GachaMenuLayer::gachaCallback);
    string body = "user_master_id="+user_master_id;
    body += "&resource_version=" + SaveData::loadString("resource_version") ;
    HttpConnect::sendHttpRequest(this, url, selector, body);
}

void GachaMenuLayer::gachaCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //データ取得コールバック
    string result = HttpConnect::getHttpResponse(sender, response);
    
    //通信エラー以外
    if (result != "not connect" && result != "response NG"){
        
        string err;
        Json json = Json::parse(result, err);
        
        //ダウンロードする必要なデータがあった時タイトルに戻す処理
        if(json["needs_download"].bool_value()){
            auto normal_scene = (NormalScene *)this->getParent();
            auto func = [](){
                //タイトルへ
                auto scene = TitleScene::createScene();
                auto transition = TransitionFade::create(3.0f, scene);
                Director::getInstance()->replaceScene(transition);
            };
            //ボップアップ表示
            string message = json["message"].string_value() ;
            auto popup_layer = PopupOkButtonLayer::create(func, message, false);
            normal_scene->addChild(popup_layer, 999999);
            return ;
        }
    
		
		//ガチャバナーデータ保存
		SaveData::saveString("gacha_banner_list", json["gacha_banner_list"].dump());
        
        bool special_gacha_flg = json["special_gacha_flg"].bool_value();
		int special_gacha_time_management_id = json["special_gacha_time_management_id"].int_value();
		int special_gacha_id = json["special_gacha_id"].int_value();
        int special_gacha_jewel_count = json["special_gacha_jewel_count"].int_value();
        int special_gacha_chara_count = json["special_gacha_chara_count"].int_value();
		
		SaveData::saveBool("special_gacha_flg", special_gacha_flg);									//開催フラグ
		SaveData::saveInt("special_gacha_time_management_id", special_gacha_time_management_id);	//主キー
		SaveData::saveInt("special_gacha_id", special_gacha_id);									//スペシャルガチャID
        SaveData::saveInt("special_gacha_jewel_count", special_gacha_jewel_count);					//スペシャルガチャ1回引くのに必要な宝玉数
        SaveData::saveInt("special_gacha_chara_count", special_gacha_chara_count);					//スペシャルガチャ1回引いた際に受け取るキャラ数
        
        //バナーのパスを設定
        setupBannerPath() ;
        
        //サイズの取得
        getSize();
        
        //必要なデータの取得
        getData();
        
        //画像、ラベルの設置
        setupImage();
        
        //ボタンの設置
        setupButton();
        
        //スワイプ出来るバナーを生成
        initBanner() ;
        
        //バナーを一定周期でスライドするスケジュール
        this->schedule(schedule_selector(GachaMenuLayer::autoSwipeUpdate), 2.4f);
        
        //矢印を生成
        initArrowSprite() ;
        
        //矢印の繰り返しのアクション
        repeatArrowsAction() ;
        
        //スペシャルガチャが開催中の場合、スペシャルガチャを最初に表示
//        if (special_gacha_flg) gacha_tab = 3;
        
        //表示設定されているタブを表示する
        showGachaTab(gacha_tab);
    }
}

void GachaMenuLayer::setupBannerPath()
{
    //イベント情報から画像パスの取得
    string err;
    Json banner_list = Json::parse(SaveData::loadString("gacha_banner_list"), err);
    
    //固定のバナーのパスを変数に格納
    premium_fix_banner_file_path = banner_list["premium"]["fix"]["img_path"].string_value() ;
    free_fix_banner_file_path = banner_list["free"]["fix"]["img_path"].string_value() ;
    plus_fix_banner_file_path = banner_list["premium_plus"]["fix"]["img_path"].string_value() ;
    
    int premium_count = fromString<int>(banner_list["premium"]["slide"]["count"].string_value()) ;
    for(int i=0 ; i<premium_count ; i++){
        string iStr = toString<int>(i) ;
        premium_slide_banner_file_paths.push_back(banner_list["premium"]["slide"][iStr]["img_path"].string_value());
    }
    
    int free_count = fromString<int>(banner_list["free"]["slide"]["count"].string_value()) ;
    for(int i=0 ; i<free_count ; i++){
        string iStr = toString<int>(i) ;
        free_slide_banner_file_paths.push_back(banner_list["free"]["slide"][iStr]["img_path"].string_value());
    }
	
    int plus_count = fromString<int>(banner_list["premium_plus"]["slide"]["count"].string_value()) ;
	if(plus_count<=0) plus_slide_banner_file_paths.push_back("download/Gacha/banner_gacha_pay62.png");
    for(int i=0 ; i<plus_count ; i++){
        string iStr = toString<int>(i) ;
        plus_slide_banner_file_paths.push_back(banner_list["premium_plus"]["slide"][iStr]["img_path"].string_value());
    }
}

void GachaMenuLayer::autoSwipeUpdate(float dt){
    autoSwipeLeft() ;
}

void GachaMenuLayer::onEnter()
{
    Layer::onEnter();
    
    //通信してメンテ状態を確認する
    gachaRequest();
}