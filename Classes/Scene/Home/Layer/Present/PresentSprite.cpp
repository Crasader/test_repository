#include "PresentSprite.h"
#include "PresentMenuLayer.h"

//初期値設定
PresentSprite::PresentSprite()
:_primaryId (0)
,_message()
,_item_type()
,_related_primary_id()
,_present_count(0)
,_chara_name()
,_insert_date()
{
}

PresentSprite* PresentSprite::create(){
    
    PresentSprite *sprite = new PresentSprite();
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool PresentSprite::init()
{
    if ( !Sprite::initWithFile("init/Present/popup_present2.png") )
    {
        return false;
    }
    
    ratio = getVisibleRatio();
    
    return true;
}

void PresentSprite::setupData(json11::Json json)
{
    setPrimaryId(fromString<int>(json["navi_present_log_id"].string_value()));
    setMessage(json["message"].string_value());
    setItemType(json["item_type"].string_value());
    setRelatedPrimaryId(json["related_primary_id"].string_value());
    setPresentCount(fromString<int>(json["count"].string_value()));
    setCharaName(json["chara_name"].string_value());
    setInsertDate(json["insert_date"].string_value());
}

void PresentSprite::parseInsertDate()
{
    //insert_dateの分解処理
    string insert_date = getInsertDate();
    
    insert_date.replace(10, 9, "");
    insert_date.replace(7, 1, ".");
    insert_date.replace(4, 1, ".");
    
//    insert_date.replace(19, 1, "秒");
//    insert_date.replace(16, 1, "分");
//    insert_date.replace(13, 1, "時");
//    insert_date.replace(10, 1, "日 ");
//    insert_date.replace(7, 1, "月");
//    insert_date.replace(4, 1, "年");
    
    //書き換え
    setInsertDate(insert_date);
}

void PresentSprite::createPresentSprite()
{
    //プレゼント画像の作成(デフォルトの画像を設定)
    present_sp = Sprite::create("download/Chara/parts100/card_bk1_reverse.png");
    addChild(present_sp, 2);
}

void PresentSprite::createPresentName()
{
    //プレゼントの名前の作成
    present_name_label = HMSFontUtil::createWithTTFBySystemFontArgument("", FONT_NAME_SYSTEM, 28);
    this->addChild(present_name_label, 2);
}

void PresentSprite::showPresentName()
{
    //プレゼントの名前の表示設定
    present_name_label->setDimensions(300, 50);
    present_name_label->setHorizontalAlignment(TextHAlignment::LEFT);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    present_name_label->setAnchorPoint(Point(0, 0));
    present_name_label->setPosition(Point(126, 63));
}

void PresentSprite::showPresentSprite()
{
    //画像のサイズを86×86に統一する処理
    Size imgSize = present_sp->getContentSize();
    present_sp->setScaleX(86 / imgSize.width);
    present_sp->setScaleY(86 / imgSize.height);
    
    present_sp->setPosition(Point(70, this->getContentSize().height /2));
}

void PresentSprite::changeData()
{
    //画像の切り替え
    if (_item_type == "chara")
    {
        string card_pass = "download/Chara/all400/" + _related_primary_id + ".png";
        present_sp->setTexture(card_pass);
        present_name_label->setString(_chara_name + " × " + toString<int>(_present_count));
    }
    else if (_item_type == "gold")
    {
        present_sp->setTexture("init/Present/item2.png");
        present_name_label->setString("資金 × " + toString<int>(_present_count));
    }
    else if (_item_type == "jewel")
    {
        present_sp->setTexture("init/Present/item.png");
        present_name_label->setString("宝玉 × " + toString<int>(_present_count));
    }
    else if (_item_type == "exchange_ticket")
    {
        present_sp->setTexture("init/Present/item7.png");
        present_name_label->setString("交換チケット × " + toString<int>(_present_count));
    }
    else if (_item_type == "box_gacha_ticket")
    {
        present_sp->setTexture("init/Present/item8.png");
        present_name_label->setString("BOXｶﾞﾁｬﾁｹｯﾄ × " + toString<int>(_present_count));
    }
}

void PresentSprite::showData()
{
    //プレゼント画像の作成
    createPresentSprite();
    
    //プレゼント名の作成
    createPresentName();
    
    //item_typeにより画像の切り替え
    changeData();
    
    //プレゼント画像の表示
    showPresentSprite();
    
    //プレゼントの名前の表示
    showPresentName();
    
    //プレゼントメッセージ
    auto message_label = HMSFontUtil::createWithTTFBySystemFontArgument(_message.c_str(), FONT_NAME_SYSTEM, 23);
    message_label->setDimensions(350, 55);
    message_label->setHorizontalAlignment(TextHAlignment::LEFT);
//enableOutline(Color4B(0, 0, 0, 255), 1);
    message_label->setAnchorPoint(Point(0, 0));
    message_label->setPosition(Point(126, 14));
    this->addChild(message_label, 2);
    
    //プレゼント取得日時
    auto date_label = HMSFontUtil::createWithTTFBySystemFontArgument(_insert_date.c_str(), FONT_NAME_SYSTEM, 22);
    date_label->setAnchorPoint(Point(1.0, 1.0));
    date_label->setPosition(Point(this->getContentSize().width - 25, this->getContentSize().height - 13));
//enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(date_label, 2);
    
    //受け取るボタン
    auto receive_off = Sprite::create("init/Present/bt_receive.png");
    auto receive_on = Sprite::create("init/Present/bt_receive.png");
    receive_on->setColor(Color3B(180, 180, 180));
                         
    auto menuItem = MenuItemSprite::create(receive_off, receive_on, [this](Ref* sender)
    {
        HMSLOG("primaryId = %d", _primaryId);
        auto present_menu_layer = (PresentMenuLayer *)this->getParent()->getParent()->getParent();
        present_menu_layer->presentReceiveRequest(toString<int>(_primaryId));
    });
    menuItem->setAnchorPoint(Point(1.0, 0));
    auto menu = Menu::create(menuItem, nullptr);
    menu->setPosition(Point(628, 0));
    this->addChild(menu, 2);
}

void PresentSprite::onEnter()
{
    Sprite::onEnter();
    
}