#include "TeamListLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "Utility.h"
#include "NormalScene.h"
#include "HMSFontUtil.h"

#define TAG_TAB1 11
#define TAG_TAB2 12
#define TAG_LIST1 1000
#define TAG_LIST2 2000

bool TeamListLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("TeamListLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamListLayer::onEnter()
{
    Layer::onEnter();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamListLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamListLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamListLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamListLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //データ処理
    _use_name = SaveData::loadString("team_list_mode");
    
    
    if(_use_name=="search") {
        HeaderLayer::setupTitleBar("検索結果", "TeamSearchTeam", this);
        
        string team_search_result = SaveData::loadString("team_search_result");
        string err;
        Json json = Json::parse(team_search_result, err);
        _json_team_list = json["team_list"].object_items();
        SaveData::saveString("team_search_result", "");
    } else if(_use_name=="app") {
        //ページタイトルの作成
        HeaderLayer::setupTitleBar("申請中チーム一覧", "TeamNoBelong", this);
        
        string team_search_result = SaveData::loadString("team_app_result");
        string err;
        Json json = Json::parse(team_search_result, err);
        _json_team_list = json["team_list"].object_items();
        SaveData::saveString("team_app_result", "");
    } else if(_use_name=="soli") {
        //ページタイトルの作成
        HeaderLayer::setupTitleBar("勧誘中チーム一覧", "TeamNoBelong", this);
        
        string team_search_result = SaveData::loadString("team_soli_result");
        string err;
        Json json = Json::parse(team_search_result, err);
        _json_team_list = json["team_list"].object_items();
        SaveData::saveString("team_soli_result", "");
    }
    _cell_count = countJsonData(_json_team_list);
    _cell_size = 130 * ratio;
    
    if(_cell_count>0) {
        createScrollView();
    } else {
        auto popup = popupChangePageLayer::create("該当のチームはありません。", "TeamNoBelong");
        addChild(popup, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
    }
    
//    updateTab();
}

//void TeamListLayer::updateTab()
//{
//    ////タブ////
//    string tab_app_file = "init/Team/no_belong/team/bt_app_";
//    string tab_soli_file = "init/Team/no_belong/team/bt_soli_";
//    //志願中
//    if(_use_name=="app") {
//        tab_app_file += "on";
//        tab_soli_file += "off";
//    }
//    //勧誘中
//    else if(_use_name=="soli") {
//        tab_app_file += "off";
//        tab_soli_file += "on";
//    }
//    tab_app_file += ".png";
//    tab_soli_file += ".png";
//    //志願中、勧誘中以外はタブ無し
//    if(_use_name=="app" || _use_name=="soli") {
//        Sprite *tab_app = Sprite::create(tab_app_file.c_str());
//        Sprite *tab_soli = Sprite::create(tab_soli_file.c_str());
//        tab_app->setPosition(convertFromDisplaySize(Point(165, 860), 0.5, 1));
//        tab_soli->setPosition(convertFromDisplaySize(Point(475, 860), 0.5, 1));
//        tab_app->setScale(ratio);
//        tab_soli->setScale(ratio);
//        this->addChild(tab_app, 1, TAG_TAB1);
//        this->addChild(tab_soli, 1, TAG_TAB2);
//    }
//    
////    //検索結果時はタイトルバー
////    if(_use_name=="search") {
////        Sprite *bar = Sprite::create("init/Team/no_belong/team_search/search_result_bar.png");
////        bar->setPosition(convertFromDisplaySize(Point(320, 760), 0.5, 1));
////        bar->setScale(ratio);
////        this->addChild(bar, 1);
////    }
//}

void TeamListLayer::createScrollView()
{
    auto pScrollView = ScrollView::create();
    pScrollView->setAnchorPoint(Point::ZERO);
    pScrollView->setPosition(convertFromDisplaySize(Point(0, FOOTER_SIZE), 0.5, 0));
    pScrollView->setViewSize(Size(640 * ratio, _contentSize.height - (HEADER_SIZE * ratio) - (TITLE_BAR_SIZE * ratio) - (FOOTER_SIZE * ratio)));
    
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
    pScrollView->setContentSize(Size(_contentSize.width, _cell_size * _cell_count));
    
    //スクロール開始位置の設定（コンテンツサイズの高さから画面縦を引いた負の値）
    int offset_y = 0 - ((_cell_size * _cell_count) - (_contentSize.height - (HEADER_SIZE * ratio + TITLE_BAR_SIZE * ratio + FOOTER_SIZE * ratio)));
    pScrollView->setContentOffset(Point(0, offset_y));
}

Layer *TeamListLayer::createList()
{
    auto layer = Layer::create();
    
    for(int i=0; i<_cell_count; i++) {
        string iStr = toString(i);
        //枠
        Sprite *frame = Sprite::create("init/Team/management/search/popup_team_s.png");
        frame->setScale(ratio);
        frame->setPosition(Point(320 * ratio, (_cell_size * i) + 65 * ratio));
        layer->addChild(frame, -1);
        
        //名前
        auto team_name = HMSFontUtil::createWithTTFBySystemFontArgument(_json_team_list[iStr]["name"].string_value(), FONT_NAME_SYSTEM, Utility::calcFlexibleFontSize(_json_team_list[iStr]["name"].string_value(), 24, 180));
        team_name->setPosition(Point(38, frame->getContentSize().height/2));
        team_name->setAnchorPoint(Point(0, 0.5));
        frame->addChild(team_name);
        
        //ギルドタイプ//
        string team_type_str = "" ;
        string team_type = _json_team_list[iStr]["team_type"].string_value();
        if(team_type == "a") team_type_str = "初心者歓迎" ;
        else if(team_type == "b") team_type_str = "中級者限定" ;
        else if(team_type == "c") team_type_str = "上級者求む" ;
        else team_type_str = "----" ;
        
        //プレイタイプ//
        string play_type_str = "" ;
        string play_type = _json_team_list[iStr]["play_type"].string_value();
        if(play_type == "a") play_type_str = "まったり" ;
        else if(play_type == "b") play_type_str = "ほどほど" ;
        else if(play_type == "c") play_type_str = "がっつり" ;
        else play_type_str = "----" ;
        
        Label *team_type_label = HMSFontUtil::createWithTTFBySystemFontArgument(team_type_str, FONT_NAME_SYSTEM, 23);
        team_type_label->setPosition(Point(420, frame->getContentSize().height/2 + 20));
        team_type_label->setHorizontalAlignment(TextHAlignment::LEFT) ;
        team_type_label->setVerticalAlignment(TextVAlignment::TOP);
        team_type_label->setAnchorPoint(Point(0.5, 0.5));
        frame->addChild(team_type_label);
        
        Label *play_type_label = HMSFontUtil::createWithTTFBySystemFontArgument(play_type_str, FONT_NAME_SYSTEM, 23);
        play_type_label->setPosition(Point(420, frame->getContentSize().height/2 - 27));
        play_type_label->setHorizontalAlignment(TextHAlignment::LEFT) ;
        play_type_label->setVerticalAlignment(TextVAlignment::TOP);
        play_type_label->setAnchorPoint(Point(0.5, 0.5));
        frame->addChild(play_type_label);
        
        //リーダーキャラ
        if(_json_team_list[iStr]["team_leader_chara_id"].string_value()!="") {
            string leader_chara_file = "download/Home/chara_thum/";
            leader_chara_file += _json_team_list[iStr]["team_leader_chara_id"].string_value();
            leader_chara_file += ".png";
            Sprite *thum = Sprite::create(leader_chara_file.c_str());
            thum->setAnchorPoint(Point(0, 1));
            thum->setPosition(Point(2, 168));
            frame->addChild(thum);
        }
        
//        //Lv値
//        auto lv_value = HMSFontUtil::createWithTTFBySystemFontArgument("Lv" + _json_team_list[iStr]["level"].string_value(), FONT_NAME_SYSTEM, 24);
//        lv_value->setAnchorPoint(Point(0, 0));
//        lv_value->setPosition(Point(325, 168));
//        frame->addChild(lv_value);
        
        //メンバー数値
        string member_num_value_str = _json_team_list[iStr]["member_count"].string_value();
        member_num_value_str += "/";
        member_num_value_str += _json_team_list[iStr]["max_member_count"].string_value();
        auto member_num_value = HMSFontUtil::createWithTTFBySystemFontArgument(member_num_value_str, FONT_NAME_SYSTEM, 22);
        member_num_value->setPosition(Point(562, frame->getContentSize().height/2 - 23));
        member_num_value->setHorizontalAlignment(TextHAlignment::LEFT);
        member_num_value->setAnchorPoint(Point(0.5, 0.5)) ;
        frame->addChild(member_num_value);
        
        //チームコメント
//        string team_comment_str = _json_team_list[iStr]["comment"].string_value();
//        auto team_comment = HMSFontUtil::createWithTTFBySystemFontArgument(team_comment_str, FONT_NAME_SYSTEM, 22);
//        team_comment->setHorizontalAlignment(TextHAlignment::LEFT);
//        team_comment->setVerticalAlignment(TextVAlignment::TOP);
//        team_comment->setPosition(Point(30, 140));
//        team_comment->setAnchorPoint(Point(0, 1));
//        team_comment->setDimensions(560, 130);
//        frame->addChild(team_comment);
        
        ////////↓あとで消すこと↓////////
        //検索結果//
        if(_use_name=="search") {
            //申請
            Sprite *app = Sprite::create("init/Team/no_belong/team_search/bt_apply.png");
            app->setScale(ratio);
            app->setPosition(Point(290 * ratio, (_cell_size * i) + 50 * ratio));
            layer->addChild(app, 1, TAG_LIST1+i);
        }
        //志願中//
        else if(_use_name=="app") {
            //取消
            Sprite *cancel = Sprite::create("init/Team/no_belong/team_search/bt_cancel.png");
            cancel->setScale(ratio);
            cancel->setPosition(Point(290 * ratio, (_cell_size * i) + 50 * ratio));
            layer->addChild(cancel, 1, TAG_LIST1+i);
        }
        //勧誘中//
        else if(_use_name=="soli") {
            //承認
            Sprite *approval = Sprite::create("init/Team/no_belong/team_search/bt_approval.png");
            approval->setScale(ratio);
            approval->setPosition(Point(200 * ratio, (_cell_size * i) + 50 * ratio));
            layer->addChild(approval, 1, TAG_LIST1+i);
            //拒否
            Sprite *refusal = Sprite::create("init/Team/no_belong/team_search/bt_refusal.png");
            refusal->setScale(ratio);
            refusal->setPosition(Point(440 * ratio, (_cell_size * i) + 50 * ratio));
            layer->addChild(refusal, 1, TAG_LIST2+i);
        }
    }
    
    return layer;
}

void TeamListLayer::scrollViewDidScroll(ScrollView *view)
{
    _tap_flg = false;
}


bool TeamListLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _tap_flg = true;
    return true;
}
void TeamListLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}
void TeamListLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(_tap_flg==true) {
        _tap_flg = false;
        
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
                if (rect.containsPoint(touch->getLocation()) && tag_id>0) {
                    if(_use_name=="search") {
                        int index = tag_id - TAG_LIST1;
                        //SEL_CallFuncND selector = callfuncND_selector(NormalScene::httpTeamDemandCompleted);
                        
                        string text = "[" + _json_team_list[toString(index)]["name"].string_value() + "]";
                        text += "\n";
                        text += "に加入申請をしてもよろしいですか?";
                        popupConfirmLayer *confirm = popupConfirmLayer::create(text);
                        auto ok_func = [this, index](){
                            string url = HOST;
                            url += "team/team_petition_member.php";
                            url += "?user_master_id=" + SaveData::loadString("user_master_id");
                            url += "&team_id=" + _json_team_list[toString(index)]["team_id"].string_value();
                            url += "&type=a";
                            
                            HMSLOG("url=%s", url.c_str());
                            auto selector = httpresponse_selector(TeamListLayer::applyCallback);
                            HttpConnect::sendHttpRequest(this, url, selector);
                        };
                        auto ng_func = [](){}; //いいえを押したとき何もしない
                        confirm->setFunction(ok_func, ng_func);
                        
                        this->addChild(confirm, 9999);
                    } else if(_use_name=="app") {
                        int index = tag_id - TAG_LIST1;
 
                        string text = "[" + _json_team_list[toString(index)]["name"].string_value() + "]";
                        text += "\n";
                        text += "への加入申請を取り消してもよろしいですか?";
                        popupConfirmLayer *confirm = popupConfirmLayer::create(text);
                        auto ok_func = [this, index](){
                            string url = HOST;
                            url += "team/team_petition_member.php";
                            url += "?user_master_id=" + SaveData::loadString("user_master_id");
                            url += "&team_id=" + _json_team_list[toString(index)]["team_id"].string_value();
                            url += "&type=d";
                            
                            HMSLOG("url=%s", url.c_str());
                            auto selector = httpresponse_selector(TeamListLayer::cancelCallback);
                            HttpConnect::sendHttpRequest(this, url, selector);
                        };
                        auto ng_func = [](){}; //いいえを押したとき何もしない
                        confirm->setFunction(ok_func, ng_func);
                        
                        this->addChild(confirm, 9999);
                    } else if(_use_name=="soli") {
                        if(tag_id<TAG_LIST2) {
                            int index = tag_id - TAG_LIST1;
                            
                            string text = "[" + _json_team_list[toString(index)]["name"].string_value() + "]";
                            text += "\n";
                            text += "に入ります。よろしいですか?";
                            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
                            auto ok_func = [this, index](){
                                string url = HOST;
                                url += "team/team_petition_member.php";
                                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                                url += "&team_id=" + _json_team_list[toString(index)]["team_id"].string_value();
                                url += "&type=ia";
                                
                                HMSLOG("url=%s", url.c_str());
                                auto selector = httpresponse_selector(TeamListLayer::soliYesCallback);
                                HttpConnect::sendHttpRequest(this, url, selector);
                            };
                            auto ng_func = [](){}; //いいえを押したとき何もしない
                            confirm->setFunction(ok_func, ng_func);
                            
                            this->addChild(confirm, 9999);
                        } else {
                            int index = tag_id - TAG_LIST2;
                            
                            string text = "[" + _json_team_list[toString(index)]["name"].string_value() + "]";
                            text += "\n";
                            text += "からの勧誘を拒否してもよろしいですか?";
                            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
                            auto ok_func = [this, index](){
                                string url = HOST;
                                url += "team/team_petition_member.php";
                                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                                url += "&team_id=" + _json_team_list[toString(index)]["team_id"].string_value();
                                url += "&type=ir";
                                
                                HMSLOG("url=%s", url.c_str());
                                auto selector = httpresponse_selector(TeamListLayer::soliNoCallback);
                                HttpConnect::sendHttpRequest(this, url, selector);
                            };
                            auto ng_func = [](){}; //いいえを押したとき何もしない
                            confirm->setFunction(ok_func, ng_func);
                            
                            this->addChild(confirm, 9999);
                        }
                    }
                }
            }
        }
    
    }
}
void TeamListLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void TeamListLayer::applyCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamNoBelong");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamListLayer::cancelCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamNoBelong");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamListLayer::soliYesCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamTop");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }
}

void TeamListLayer::soliNoCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    string err;
    Json json = Json::parse(result, err);
    
    if(json["result"].string_value()=="OK") {//成功
        popupChangePageLayer *layer = popupChangePageLayer::create(json["message"].string_value(), "TeamNoBelong");
        this->addChild(layer, 9999);
    } else {
        //メッセージ表示
        popupNoChangePageLayer *layer = popupNoChangePageLayer::create(json["message"].string_value());
        this->addChild(layer, 9999);
    }}