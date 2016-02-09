#include "ComebackCampaignLayer.hpp"

#define ITEM_SIZE 400 * 0.7 //400×400の画像を0.7倍して使用している

ComebackCampaignLayer::ComebackCampaignLayer()
:_current_index(0)
,_max_present_count(0)
{
}

ComebackCampaignLayer* ComebackCampaignLayer::create(Json json)
{
    ComebackCampaignLayer *node = new ComebackCampaignLayer();
    if (node && node->init(json))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool ComebackCampaignLayer::init(Json json)
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ComebackCampaignLayer::init()");
    
    ratio = getVisibleRatio();
    
    //イベントリスナーの初期化
    initEventListener();
    
    _comeback_present_data_list = json;
    _max_present_count = countJsonData(_comeback_present_data_list);
    
    return true;
}

void ComebackCampaignLayer::initEventListener()
{
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(ComebackCampaignLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(ComebackCampaignLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(ComebackCampaignLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(ComebackCampaignLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void ComebackCampaignLayer::onEnter()
{
    Layer::onEnter();
    
    //影の作成
    createShadow();
    
    //ポップアップ画像の作成
    createPopupImage();
    
    //タイトルの作成
    createTitleLabel();
    
    //ボタンの作成
    createButton();
    
    //プレゼント画像とプレゼント文字列
    createPresentSpriteAndLabel();
    
    //お知らせ文
    createInfoLabel();
    
    if (_max_present_count > 1) {
        
        //矢印の作成
        createArrowSprite();
        
        //選択プレゼント番号
        createSelectedPresentNumberLabel();
        
        //表示しているのが何個目か知らせる点
        createDot();
    }
    
    //ポップアップアクション
    Utility::openPopupAction(this);
}

void ComebackCampaignLayer::createShadow()
{
    //影
    auto shadow = Sprite::create("init/Home/Common/back_shadow50.png");
    shadow->setScale(ratio);
    shadow->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(shadow, Z_Shadow, T_Shadow);
}

void ComebackCampaignLayer::createTitleLabel()
{
    //タイトルラベルの作成
    string text = "カムバックキャンペーン";
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 48, 560));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_popup_bg->getContentSize().width / 2, 630));
    _popup_bg->addChild(label, Z_Title, T_Title);
}

void ComebackCampaignLayer::createPopupImage()
{
    //ポップアップ背景の作成
    _popup_bg = Sprite::create("init/Home/Popup/popup_700.png");
    _popup_bg->setScale(0);
    _popup_bg->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(_popup_bg, Utility::Z_Popup_Action, Utility::T_Popup_Action);
}

void ComebackCampaignLayer::createButton()
{
    //OKボタンの作成
    auto ok_off = Sprite::create("init/Home/Common/bt_ok.png");
    auto ok_on  = Sprite::create("init/Home/Common/bt_ok.png");
    ok_on->setColor(Color3B(180, 180, 180));
    
    MenuItem* ok_btn = MenuItemSprite::create(ok_off, ok_on, [this](Ref* sender) {
        
         //ポップアップ閉じる
         Utility::closePopupAction(this);
        
        auto mypage_layer = (MypageLayer *)this->getParent()->getChildByTag(MAIN_LAYER_TAG);
        auto normal_scene = (NormalScene *)mypage_layer->getParent() ;
        
        this->removeFromParent();
        if(mypage_layer && normal_scene->getMainName()=="")mypage_layer->showNextPopupStatus();
     });
    ok_btn->setAnchorPoint(Point(0.5, 0));
    
    auto menu = Menu::create(ok_btn, nullptr);
    menu->setPosition(Point(_popup_bg->getContentSize().width / 2, 40));
    _popup_bg->addChild(menu, Z_Menu, T_Menu);
}

void ComebackCampaignLayer::createPresentSpriteAndLabel()
{
    //マスク部分を作成
    auto clipper = ClippingRectangleNode::create();
    clipper->setClippingRegion(Rect(0, 0, ITEM_SIZE, ITEM_SIZE));
    clipper->setPosition(Point(_popup_bg->getContentSize().width / 2 - ITEM_SIZE / 2, _popup_bg->getContentSize().height /2 + 30 - ITEM_SIZE / 2));
    _popup_bg->addChild(clipper, Z_Clipper, T_Clipper);
    
    //画像を載せるノードを生成
    _node = Node::create();
    clipper->addChild(_node);
    
    for (int i = 0; i < _max_present_count; i++) {
        
        string iStr = toString<int>(i);
        
        //カムバックキャンペーンプレゼントの表示
        string image_path, label_text;
        getImagePathForItemType(image_path, label_text, _comeback_present_data_list[iStr]);
        
        //画像の作成
        auto item_sp = Sprite::create(image_path);
        item_sp->setAnchorPoint(Point(0, 0));
        item_sp->setPosition(Point((ITEM_SIZE * i), 0));
        item_sp->setScale(0.7);
        _node->addChild(item_sp);
        
        //ラベルテキストの格納
        _vec_label_text.push_back(label_text);
    }
    
    //ラベルの作成
    string text = _vec_label_text[0];
    auto present_label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(text, 29, 560));
    present_label->setHorizontalAlignment(TextHAlignment::CENTER);
    present_label->setAnchorPoint(Point(0.5, 0));
    present_label->setPosition(Point(_popup_bg->getContentSize().width / 2, 134));
    _popup_bg->addChild(present_label, Z_Present_Label, T_Present_Label);
}

void ComebackCampaignLayer::getImagePathForItemType(string& image_path, string& label_text, Json json)
{
    //画像のパスと、表示テキストの取得
    string item_type          = json["item_type"].string_value();
    string related_primary_id = json["related_primary_id"].string_value();
    string chara_name         = json["chara_name"].string_value();
    string count              = json["count"].string_value();
    
    if (item_type == "chara") {
        image_path = "download/Chara/all400/" + related_primary_id + ".png";
        label_text = chara_name + " × " + count + " を受け取りました";
    }
    else if (item_type == "gold") {
        image_path = "init/Present/item2_400.png";
        label_text = "資金 × " + count + " を受け取りました";
    }
    else if (item_type == "jewel") {
        image_path = "init/Present/item_400.png";
        label_text = "宝玉 × " + count + " を受け取りました";
    }
}

void ComebackCampaignLayer::createInfoLabel()
{
    auto info_label = HMSFontUtil::createWithTTFBySystemFontArgument("※受け取ったアイテムはプレゼントボックスに送られます", FONT_NAME_SYSTEM, 18);
    info_label->setHorizontalAlignment(TextHAlignment::CENTER);
    info_label->setAnchorPoint(Point(0.5, 0));
    info_label->setPosition(Point(_popup_bg->getContentSize().width / 2, 25));
    _popup_bg->addChild(info_label, Z_Info_Label, T_Info_Label);
}

void ComebackCampaignLayer::createArrowSprite()
{
    //左矢印
    _left_arrow = Sprite::create("init/CharaMenu/arrow_left.png");
    _left_arrow->setAnchorPoint(Point(0.37, 0.5));
    _left_arrow->setPosition(Point(70, _popup_bg->getContentSize().height /2 + 30));
    _popup_bg->addChild(_left_arrow, Z_Left_Arrow, T_Left_Arrow);
    
    //右矢印
    _right_arrow = Sprite::create("init/CharaMenu/arrow_right.png");
    _right_arrow->setAnchorPoint(Point(0.37, 0.5));
    _right_arrow->setPosition(Point(_popup_bg->getContentSize().width - 70, _popup_bg->getContentSize().height /2 + 30));
    _popup_bg->addChild(_right_arrow, Z_Right_Arrow, T_Right_Arrow);
    
    checkAndShowArrowSprite(_current_index);
}

void ComebackCampaignLayer::createSelectedPresentNumberLabel()
{
    //選択プレゼント番号ラベル
    string text = toString<int>(_current_index + 1) + " / " + toString<int>(_max_present_count);
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(text, FONT_NAME_SYSTEM, 38);
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setPosition(Point(_popup_bg->getContentSize().width / 2, 550));
    _popup_bg->addChild(label, Z_Selected_Present_Number, T_Selected_Present_Number);
    
}

void ComebackCampaignLayer::createDot()
{
    float center_x = _popup_bg->getContentSize().width / 2;
    float distance = 60.0f;
    
    float first_pos_x = center_x - (float)(_max_present_count - 1) / 2 * distance;
    
    for (int i = 0; i < _max_present_count; i++) {
        
        _vec_position.push_back(Point(first_pos_x + (distance * i), 196));
        
        _vec_dot.push_back(DrawNode::create());
        _popup_bg->addChild(_vec_dot[i], Z_Dot, T_Dot);
    }
    
    //最初は１つ目が選択状態
    selectedDotItemNumber(_current_index);
}

void ComebackCampaignLayer::switchDotAndArrowAndNuber(int index)
{
    //オブジェクトの切り替え
    selectedDotItemNumber(index);
    
    checkAndShowArrowSprite(index);
    
    switchPresentNumber(index);
    
    auto label = (Label *)_popup_bg->getChildByTag(T_Present_Label);
    label->setString(_vec_label_text[index]);
}

void ComebackCampaignLayer::selectedDotItemNumber(int index)
{
    //drawノードの色がsetColorで変更できなかった為、clearして再生成している
    //drawNodeはgetColor、getPositionでも取得不可能なので、_vec_positionに位置情報を追加した....
    for (int i = 0; i < _max_present_count; i++) {
        _vec_dot[i]->clear();
        _vec_dot[i]->drawDot(_vec_position[i], 14.0, Color4F::WHITE);
    }
    
    _vec_dot[index]->clear();
    _vec_dot[index]->drawDot(_vec_position[index], 14.0, Color4F::ORANGE);
}

void ComebackCampaignLayer::checkAndShowArrowSprite(int index)
{
    //一番最初と最後の場合の矢印の切り替え
    _left_arrow->setVisible(true);
    _right_arrow->setVisible(true);
    
    if (index == 0) _left_arrow->setVisible(false);
    if (index + 1 == _max_present_count) _right_arrow->setVisible(false);
}

void ComebackCampaignLayer::switchPresentNumber(int index)
{
    //表示しているプレゼント番号の更新
    string text = toString<int>(index + 1) + " / " + toString<int>(_max_present_count);
    auto label = (Label *)_popup_bg->getChildByTag(T_Selected_Present_Number);
    label->setString(text);
}

void ComebackCampaignLayer::switchSprite(ComebackCampaignLayer::SwitchPos switch_pos)
{
    //画像の切り替え（左の場合は負、右の場合は正の値に動かす）
    int index = 1;
    if (switch_pos == Left) index = -1;
    
    _eventDispatcher->setEnabled(false); //タッチ判定をアクション中無効にしている
    
    auto move = MoveBy::create(0.2f, Point( (-1 * index) * ITEM_SIZE , 0));
    
    auto func = CallFunc::create([this, index](){
        
        //オブジェクトの切り替え
        switchDotAndArrowAndNuber(_current_index + index);
        
        _current_index += index;
        
        _eventDispatcher->setEnabled(true);
    });
    
    this->runAction(Sequence::create(TargetedAction::create(_node, move), func, nullptr));
}

bool ComebackCampaignLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}

void ComebackCampaignLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}

void ComebackCampaignLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (_max_present_count > 1) {
        //画像のポイントを相対位置から絶対位置に戻してからRectを取得している　※注意：getBoundingBox()では出来ない
        Point left_arrow_point = _left_arrow->convertToWorldSpace(Point::ZERO);
        auto left_arrow_rect   = Rect(left_arrow_point.x, left_arrow_point.y, _left_arrow->getContentSize().width * ratio, _left_arrow->getContentSize().height * ratio);
        if (left_arrow_rect.containsPoint(touch->getLocation())) {
            if (0 < _current_index) switchSprite(SwitchPos::Left);
        }
        
        Point right_arrow_point = _right_arrow->convertToWorldSpace(Point::ZERO);
        auto right_arrow_rect   = Rect(right_arrow_point.x, right_arrow_point.y, _right_arrow->getContentSize().width * ratio, _right_arrow->getContentSize().height * ratio);
        if (right_arrow_rect.containsPoint(touch->getLocation())) {
            if (_current_index + 1 < _max_present_count) switchSprite(SwitchPos::Right);
        }
    }
}
void ComebackCampaignLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
}