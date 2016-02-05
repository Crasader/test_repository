#include "TeamMemberListLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "Utility.h"
#include "NormalScene.h"
#include "TeamManagementBaseLayer.h"
#include "TeamMemberDetailLayer.h"
#include "TeamMemberSearchConditionPopupLayer.h"
#include "TeamMemberSearchSortPopupLayer.h"
#include "HMSFontUtil.h"
#include "popupConfirmLayer.h"
#include "TeamMemberSprite.h"

#define TAG_BACK 11
#define TAG_TEAM 12
#define TAG_MEMBER 13
#define TAG_APP 14
#define TAG_SOLI 15

bool TeamMemberListLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamMemberListLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamMemberListLayer::onEnter()
{
    Layer::onEnter();
    
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamMemberListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamMemberListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamMemberListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamMemberListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //データ取得
    getData();
}

void TeamMemberListLayer::getData()
{
     _use_name = SaveData::loadString("team_member_list_mode");
    
    ////ページタイトルの作成 ///
    if(_use_name=="search") {
        //検索結果は検索へ
        HeaderLayer::setupTitleBar("勧誘", "TeamManagement", this, HeaderLayer::TitleBarType::WIDTH_324);
    } else if(_use_name=="leader_change") {
        //リーダー引き継ぎはチーム管理へ
        HeaderLayer::setupTitleBar("メンバー", "TeamManagement", this);
    } else {
        //その他は未所属トップへ
        HeaderLayer::setupTitleBar("メンバー", "TeamTop", this);
    }
    
    
    if(_use_name!="search") {
        string url = HOST;
        url += "team/team_get_member.php";
        url += "?user_master_id=" + SaveData::loadString("user_master_id");
        url += "&team_id=" + toString(SaveData::loadInt("team_id"));
        url += "&type=" + _use_name;
        
        HMSLOG("url=%s", url.c_str());
        auto selector = httpresponse_selector(TeamMemberListLayer::getDataCallback);
        HttpConnect::sendHttpRequest(this, url, selector);
    } else {
        //検索のPHPを呼ぶ(ページ入った最初は新着順)
        sortName = (SearchTag)SaveData::loadInt("sort_team_member_status");
//        sortName = SearchTag::NewList;
        searchId = "";
        searchUserName = "";
        
        //検索用ボタン作成
        searchButtonCreate();
        
        //検索する関数
        searchInvitationList();
    }
}

void TeamMemberListLayer::getDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    //データ処理
    string err;
    Json json = Json::parse(result, err);
    _json_team_member_list = json["team_member_list"].object_items();
    _count_p = json["count_p"].int_value();
    _count_i = json["count_i"].int_value();

    _cell_count = countJsonData(_json_team_member_list);
    _cell_size = 130 * ratio;
    
    if(_cell_count>0) {
        createScrollView();
    } else {
        string text = "該当ユーザはいません。";
        popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
        this->addChild(comment, 9999);
    }
    
    updateTab();
}

void TeamMemberListLayer::searchInvitationList()
{
    string url = HOST;
    url += "team/team_user_search.php";
    url += "?user_master_id=" + SaveData::loadString("user_master_id");
    
    //ユーザID
    url += "&user_id=";
    url += this->searchId;
    
    //ユーザ名
    url += "&user_name=";
    url += URLEncode(this->searchUserName);
    
    //ソート順
    url += "&sort_name=";
    url += getSearchSortName(this->sortName);
    
    SaveData::saveString("team_invitation_friend_id", searchId);
    SaveData::saveString("team_invitation_user_name", searchUserName);
    SaveData::saveInt("sort_team_member_status", this->sortName);
    
    //HMSLOG("url=%s", url.c_str());
    auto selector = httpresponse_selector(TeamMemberListLayer::searchInvitationListCallback);
    HttpConnect::sendHttpRequest(this, url, selector);
}

void TeamMemberListLayer::searchInvitationListCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    //データ処理
    string err;
    Json json = Json::parse(result, err);
    
    _json_team_member_list = json["team_member_list"].object_items();
    
    _cell_count = countJsonData(_json_team_member_list);
    _cell_size = 130 * ratio;
    
    createScrollView();
    
    updateTab();
    
    Label* title_label = (Label*)this->getChildByTag(T_Search_text);
    title_label->setString(getSearchSortNameDisplay(this->sortName));
}

void TeamMemberListLayer::searchButtonCreate()
{
    //並べ替え
    auto sort_btn_on = Sprite::create("init/Team/management/search/bt_display_change.png");
    auto sort_btn_off = Sprite::create("init/Team/management/search/bt_display_change.png");
    sort_btn_on->setColor(Color3B(180, 180, 180));
    auto sort_btn = MenuItemSprite::create(sort_btn_off, sort_btn_on, [this](Ref* sender){
        TeamMemberSearchSortPopupLayer* popupLayer = TeamMemberSearchSortPopupLayer::create(this);
        this->addChild(popupLayer, O_PopupLayer, T_PopupLayer);
    });
    sort_btn->setScale(ratio);
    auto sort_menu = Menu::create(sort_btn, nullptr);
    sort_menu->setPosition(convertFromDisplaySize(Point(426, 789), 0.5, 1));
    this->addChild(sort_menu);
    
    //表示順テキスト
    auto title_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 19);
    title_label->setScale(ratio);
    title_label->setPosition(convertFromDisplaySize(Point(426 ,779), 0.5, 1));
    this->addChild(title_label, 1, T_Search_text);
    
    //条件検索
    auto condition_btn_on = Sprite::create("init/Team/management/search/bt_condition_search.png");
    auto condition_btn_off = Sprite::create("init/Team/management/search/bt_condition_search.png");
    condition_btn_on->setColor(Color3B(180, 180, 180));
    auto condition_btn = MenuItemSprite::create(condition_btn_off, condition_btn_on, [this](Ref* sender){
        TeamMemberSearchConditionPopupLayer* popupLayer = TeamMemberSearchConditionPopupLayer::create(this);
        this->addChild(popupLayer, O_PopupLayer, T_PopupLayer);
    });
    condition_btn->setScale(ratio);
    auto condition_menu = Menu::create(condition_btn, nullptr);
    condition_menu->setPosition(convertFromDisplaySize(Point(570, 789), 0.5, 1));
    this->addChild(condition_menu);
}

string TeamMemberListLayer::getSearchSortName(int sort_tag)
{
    if (sort_tag == SearchTag::NewList)
        return "new";
    else if (sort_tag == SearchTag::LoginList)
        return "login";
    else if (sort_tag == SearchTag::LevelList)
        return "level";
    else if (sort_tag == SearchTag::RankList)
        return "rank";
    
    return "new";
}

string TeamMemberListLayer::getSearchSortNameDisplay(int sort_tag)
{
    if (sort_tag == SearchTag::NewList)
        return "入会日時順";
    else if (sort_tag == SearchTag::LoginList)
        return "最終ログイン";
    else if (sort_tag == SearchTag::LevelList)
        return "Lv順";
    else if (sort_tag == SearchTag::RankList)
        return "Rank順";
    
    return "入会日時順";
}

void TeamMemberListLayer::refreshList()
{
    removeChildByTag(SCROLLVIEW_TAG);
//    bazaarCharaSpriteList.clear();
//    charaSpritePositionList.clear();
    searchInvitationList();
}

void TeamMemberListLayer::updateTab()
{
    
    //検索結果、リーダー引き継ぎはタブ無し
    if(_use_name!="search" && _use_name!="leader_change") {
        float tab_padding_y = 62 ;
        
        //メンバータブ
        removeChildByTag(TAG_MEMBER, true);
        Sprite *member_base ;
        if(_use_name=="member") member_base = Sprite::create("init/Team/management/base/label_member.png");
        else member_base = Sprite::create("init/Team/management/base/label_member_gray.png");
        member_base->setAnchorPoint(Point(0.5, 0)) ;
        member_base->setPosition(convertFromDisplaySize(Point(120, 0), 0.5, 0) + Point(0, _contentSize.height-HEADER_SIZE*ratio-TITLE_BAR_SIZE*ratio-tab_padding_y*ratio));
        member_base->setScale(ratio);
        this->addChild(member_base, O_TAB_BACK, TAG_MEMBER);
        
        //志願者タブ
        removeChildByTag(TAG_APP, true);
        Sprite *app_base ;
        if(_use_name=="app") app_base = Sprite::create("init/Team/management/base/label_app.png");
        else app_base = Sprite::create("init/Team/management/base/label_app_gray.png");
        app_base->setAnchorPoint(Point(0.5, 0)) ;
        app_base->setPosition(convertFromDisplaySize(Point(317, 0), 0.5, 0) + Point(0, _contentSize.height-HEADER_SIZE*ratio-TITLE_BAR_SIZE*ratio-tab_padding_y*ratio));
        app_base->setScale(ratio);
        this->addChild(app_base, O_TAB_BACK, TAG_APP);
        
        //志願者の数を表示
        if(_count_p>0) {
            auto bg_red = Sprite::create("init/Home/Mypage/bt_mypage_red_circle.png");
            bg_red->setPosition(Point(app_base->getContentSize().width-20, app_base->getContentSize().height-20));
            app_base->addChild(bg_red, 2);
            
            if (_count_p > 99) {
                _count_p = 99;
            }
            auto num_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(_count_p), FONT_NAME_SYSTEM, 24);
            num_label->setPosition(bg_red->getContentSize()/2);
            bg_red->addChild(num_label);
        }
        
        //勧誘中タブ
        removeChildByTag(TAG_SOLI, true);
        Sprite *soli_base ;
        if(_use_name=="soli") soli_base = Sprite::create("init/Team/management/base/label_soli.png");
        else soli_base = Sprite::create("init/Team/management/base/label_soli_gray.png");
        soli_base->setAnchorPoint(Point(0.5, 0)) ;
        soli_base->setPosition(convertFromDisplaySize(Point(521, 0), 0.5, 0) + Point(0, _contentSize.height-HEADER_SIZE*ratio-TITLE_BAR_SIZE*ratio-tab_padding_y*ratio));
        soli_base->setScale(ratio);
        this->addChild(soli_base, O_TAB_BACK, TAG_SOLI);
        
        //勧誘中の数を表示
        if(_count_i>0) {
            auto bg_red = Sprite::create("init/Home/Mypage/bt_mypage_red_circle.png");
            bg_red->setPosition(Point(soli_base->getContentSize().width-20, soli_base->getContentSize().height-20));
            soli_base->addChild(bg_red, 2);
            
            if (_count_i > 99) {
                _count_i = 99;
            }
            auto num_label = HMSFontUtil::createWithTTFBySystemFontArgument(toString<int>(_count_i), FONT_NAME_SYSTEM, 24);
            num_label->setPosition(Point(bg_red->getContentSize() / 2));
            bg_red->addChild(num_label);
        }
        
        
        string back_image  ;
        if(_use_name=="member") back_image = "init/Team/management/base/label_member_bk.png" ;
        else if(_use_name=="app") back_image = "init/Team/management/base/label_app_bk.png" ;
        else if(_use_name=="soli") back_image = "init/Team/management/base/label_soli_bk.png" ;
        
        //タブ背景
        removeChildByTag(123) ;
        Sprite *bk = Sprite::create(back_image) ;
        bk->setScale(ratio) ;
        bk->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, 0), 0.5, 0) + Point(0, _contentSize.height-HEADER_SIZE*ratio-TITLE_BAR_SIZE*ratio-tab_padding_y*ratio));
        bk->setAnchorPoint(Point(0.5, 1)) ;
        this->addChild(bk, O_BACK, 123) ;
        
        if(_use_name=="member") member_base->setZOrder(O_TAB_FRONT) ;
        else if(_use_name=="app") app_base->setZOrder(O_TAB_FRONT);
        else if(_use_name=="soli") soli_base->setZOrder(O_TAB_FRONT);
        
    }
    else if(_use_name=="search") {
        //検索結果
    }
}

void TeamMemberListLayer::createScrollView()
{
    float tab_padding_y ;
    if(_use_name=="leader_change" || _use_name=="search") {
        tab_padding_y = 0 ;
    }else{
        tab_padding_y = 80 ;
    }
    
    
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio) - tab_padding_y * ratio));
    
    //縦方向にスクロール設定
    pScrollView->setDirection(ScrollView::Direction::VERTICAL);
    
    //スクロールのバウンスを止める
    pScrollView->setBounceable(true);
    pScrollView->setDelegate(this);
    this->addChild(pScrollView, O_SCROLLVIEW, SCROLLVIEW_TAG);
    
    //PresentMenuのレイヤーを戻り値に指定して使う
    auto layer = createList();
    layer->setTag(SCROLLLAYER_TAG);
    pScrollView->setContainer(layer);
    
    //スクロールさせる範囲の設定
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _cell_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    int offset_y = 0 - ((_cell_size * _cell_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio + tab_padding_y * ratio)));
    pScrollView->setContentOffset(Point(0, offset_y));
}

Layer *TeamMemberListLayer::createList()
{
    auto layer = Layer::create();
    
    for(int i=0; i<_cell_count; i++) {
        TeamMemberSprite *sp = TeamMemberSprite::create(_json_team_member_list, _cell_count - 1 -i);
        string num = toString<int>((int)_cell_count - 1 -i);
        
        sp->setScale(ratio);
        sp->setPosition(Point(320 * ratio, (_cell_size * i) + (_cell_size / 2)));
        layer->addChild(sp, 1, _cell_count - 1 -i);
    }
    
    return layer;
}

void TeamMemberListLayer::scrollViewDidScroll(ScrollView *view)
{
    _tap_flg = false;
}


bool TeamMemberListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _tap_flg = true;
    return true;
}
void TeamMemberListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamMemberListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_tap_flg==true) {
        _tap_flg = false;
        
        NormalScene *normal_scene = (NormalScene*)this->getParent();
        
        Sprite *img;
        Rect rect;
        img = (Sprite*)this->getChildByTag(TAG_MEMBER);
        if(img!=nullptr) {
            rect = img->boundingBox();
            if(rect.containsPoint(touch->getLocation())) {
                HMSLOG("tab member");
                SaveData::saveString("team_member_list_mode", "member");
                removeAllChildren();
                getData();
                return;
            }
        }
        
        img = (Sprite*)this->getChildByTag(TAG_APP);
        if(img!=nullptr) {
            rect = img->boundingBox();
            if(rect.containsPoint(touch->getLocation())) {
                HMSLOG("tab app");
                SaveData::saveString("team_member_list_mode", "app");
                removeAllChildren();
                getData();
                return;
            }
        }
        
        img = (Sprite*)this->getChildByTag(TAG_SOLI);
        if(img!=nullptr) {
            rect = img->boundingBox();
            if(rect.containsPoint(touch->getLocation())) {
                HMSLOG("tab soli");
                SaveData::saveString("team_member_list_mode", "soli");
                removeAllChildren();
                getData();
                return;
            }
        }
        
//        //戻る
//        Sprite *back = (Sprite*)this->getChildByTag(TAG_BACK);
//        Rect back_rect = back->boundingBox();
//        if(back_rect.containsPoint(touch->getLocation())) {
//            //検索結果は検索へ
//            if(_use_name=="search") {
//                normal_scene->changePage("TeamInvitation");
//            }
//            //リーダー引き継ぎはチーム管理へ
//            if(_use_name=="leader_change") {
//                normal_scene->changePage("TeamManagement");
//            }
//            //その他は未所属トップへ
//            else {
//                normal_scene->changePage("TeamTop");
//            }
//            return;
//        }
        
        if(this->getChildByTag(SCROLLVIEW_TAG)!=nullptr) {
            auto children = this->getChildByTag(SCROLLVIEW_TAG)->getChildByTag(SCROLLLAYER_TAG)->getChildren();
            
            for (auto child : children)
            {
                //childをspriteにキャスト
                auto sp = (Sprite *)child;
                
                //spriteの絶対座標の値を取得
                auto spWorldPoint = sp->convertToWorldSpace(Point::ZERO);
                
                //当たり判定作成
                Rect rect = Rect(spWorldPoint.x, spWorldPoint.y, sp->getContentSize().width * ratio, sp->getContentSize().height * ratio);
                
                int tag_id = sp->getTag();
                Point p = touch->getLocation();
                if (rect.containsPoint(touch->getLocation())) {
                    HMSLOG("tap");
                    //string user_master_id = _json_team_member_list[toString(tag_id)]["user_master_id"].string_value();
                    //SaveData::saveString("team_member_detail_user_master_id", user_master_id);
                    SaveData::saveString("team_member_detail_data", _json_team_member_list[toString(tag_id)].dump());
                    normal_scene->changePage("TeamMemberDetail");
                    
                    return;
                }
            }
        }
    }
}
void TeamMemberListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
