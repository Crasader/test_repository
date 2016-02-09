#include "TeamCreateTeamLayer.h"
#include "HmsGeometricFunction.h"
#include "HmsUtilityFunction.h"
#include "Utility.h"
#include "NormalScene.h"
#include "popupConfirmLayer.h"

USING_NS_CC;

bool TeamCreateTeamLayer::init()
{
    if(!Layer::init()) {
        return false;
    }
    HMSLOG("TeamCreateTeamLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void TeamCreateTeamLayer::onEnter()
{
    Layer::onEnter();
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("チーム結成", "TeamNoBelong", this);
    
    setup();
    
    //タップイベントの設定
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TeamCreateTeamLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(TeamCreateTeamLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(TeamCreateTeamLayer::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(TeamCreateTeamLayer::onTouchCancelled, this);
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void TeamCreateTeamLayer::setup()
{
    //枠
    Sprite *frame = Sprite::create("init/Team/no_belong/create/popup_614_400.png");
    frame->setPosition(convertFromDisplaySize(Point(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2), 0.5, 0.5));
    frame->setScale(ratio);
    this->addChild(frame, 0, FRAME);
    
    //文字列の追加
    createLabels(frame) ;
    
    //入力欄
    ui::EditBox* editBox;
    editBox = ui::EditBox::create(Size(508, 70), ui::Scale9Sprite::create("init/Team/no_belong/create/entry_column.png"));
    editBox->setPlaceHolder("チーム名を入力") ;
    editBox->setPlaceholderFontColor(Color3B::GRAY) ;
    editBox->setPosition(Point(frame->getContentSize().width/2,68));
    editBox->setFontColor(Color3B::WHITE);
    editBox->setMaxLength(8);
    editBox->setFont(FONT_NAME_SYSTEM, 36);
    editBox->setText("");
    editBox->setInputMode(ui::EditBox::InputMode::ANY);
    editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    frame->addChild(editBox, 1, EDIT_BOX);
    
    //結成
    Sprite *create = Sprite::create("init/Team/no_belong/create/bt_create_name.png");
    create->setPosition(convertFromDisplaySize(Point(320, 230), 0.5, 0));
    create->setScale(getVisibleRatio());
    this->addChild(create, 1, CREATE_BUTTON);
    
}

void TeamCreateTeamLayer::createLabels(Node *parent_sprite)
{
    //チーム名を入力してください
    Label *team_detail_label_line1 = HMSFontUtil::createWithTTFBySystemFontArgument("チーム名を入力してください", FONT_NAME_SYSTEM, 35);
    team_detail_label_line1->setHorizontalAlignment(TextHAlignment::CENTER) ;
    team_detail_label_line1->setPosition(Point(parent_sprite->getContentSize().width/2, 333)) ;
    parent_sprite->addChild(team_detail_label_line1) ;
    
    //10文字以内で入力して下さい。
    Label *team_detail_label_line2 = HMSFontUtil::createWithTTFBySystemFontArgument("8文字以内で入力して下さい。", FONT_NAME_SYSTEM, 25);
    team_detail_label_line2->setHorizontalAlignment(TextHAlignment::CENTER) ;
    team_detail_label_line2->setPosition(Point(parent_sprite->getContentSize().width/2, 265)) ;
    parent_sprite->addChild(team_detail_label_line2) ;
    
    //チーム名はギルド結成後も変更可能です。
    Label *team_detail_label_line3 = HMSFontUtil::createWithTTFBySystemFontArgument("チーム名はチーム結成後も変更可能です。", FONT_NAME_SYSTEM, 25);
    team_detail_label_line3->setHorizontalAlignment(TextHAlignment::CENTER) ;
    team_detail_label_line3->setPosition(Point(parent_sprite->getContentSize().width/2, 232)) ;
    parent_sprite->addChild(team_detail_label_line3) ;
    
    //※「／」は入力不可です
    Label *team_detail_label_line4 = HMSFontUtil::createWithTTFBySystemFontArgument("※「／」は入力不可です", FONT_NAME_SYSTEM, 25);
    team_detail_label_line4->setTextColor(Color4B::RED) ;
    team_detail_label_line4->setHorizontalAlignment(TextHAlignment::CENTER) ;
    team_detail_label_line4->setPosition(Point(parent_sprite->getContentSize().width/2, 193)) ;
    parent_sprite->addChild(team_detail_label_line4) ;
    
}

bool TeamCreateTeamLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return true;
}
void TeamCreateTeamLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
}
void TeamCreateTeamLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    //結成
    Sprite *create = (Sprite*)this->getChildByTag(CREATE_BUTTON);
    Rect create_rect = create->boundingBox();
    if(create_rect.containsPoint(touch->getLocation())) {
        ui::EditBox *editBox = (ui::EditBox *)this->getChildByTag(FRAME)->getChildByTag(EDIT_BOX);
        _input_team_name = editBox->getText();
        
        //入力禁止文字チェック用
        int forbid_index = (int)_input_team_name.find(ILLEGAL_CHAR);
        
        //未入力チェック
        if(_input_team_name=="") {
            string text = "チーム名が未入力です";
            popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
            this->addChild(comment, 9999);
        }
        //入力禁止文字チェック
        else if(forbid_index > -1) {
            string text = "「";
            text += ILLEGAL_CHAR;
            text += "」は入力禁止文字です";
            popupNoChangePageLayer *comment = popupNoChangePageLayer::create(text);
            this->addChild(comment, 9999);
        }
        else {
            string text = "チーム名:";
            text += _input_team_name;
            text += "\n";
            text += "以上の内容でよろしいですか?";
            popupConfirmLayer *confirm = popupConfirmLayer::create(text);
            auto ok_func = [this](){
                string url = HOST;
                url += "team/team_create.php";
                url += "?user_master_id=" + SaveData::loadString("user_master_id");
                
                //チーム名
                url += "&team_name=";
                url +=  URLEncode(_input_team_name);
                
                //HMSLOG("url=%s", url.c_str());
                auto selector = httpresponse_selector(TeamCreateTeamLayer::httpCallback);
                HttpConnect::sendHttpRequest(this, url, selector);
            };
            auto ng_func = [](){}; //いいえを押したとき何もしない
            confirm->setFunction(ok_func, ng_func);
            
            this->addChild(confirm, 9999);
        }
        
        return;
    }
    
}

void TeamCreateTeamLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void TeamCreateTeamLayer::httpCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
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