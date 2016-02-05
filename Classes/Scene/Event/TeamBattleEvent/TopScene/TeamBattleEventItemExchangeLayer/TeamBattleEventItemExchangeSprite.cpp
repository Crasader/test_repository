#include "TeamBattleEventItemExchangeLayer.h"
#include "TeamBattleEventItemExchangeSprite.h"
#include "popupConfirmLayer.h"

TeamBattleEventItemExchangeSprite* TeamBattleEventItemExchangeSprite::create(json11::Json json, Node* node)
{
    TeamBattleEventItemExchangeSprite *sprite = new TeamBattleEventItemExchangeSprite();
    if (sprite && sprite->init(json, node))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

//交換アイテム画像のパス
void TeamBattleEventItemExchangeSprite::setExchangeItemData()
{
    //交換に使用するアイテムのデータをセット
    exchange_item_sprite_path = "init/CharaMenu/revision/chara_exchange/icon_ticket.png";
    exchange_item_name = "旗";
    exchange_item = fromString<int>(SaveData::loadString("team_battle_event_item"));
}

//交換ボタンタップ時のアクション
void TeamBattleEventItemExchangeSprite::tapExchangeButtonAction()
{
    //TODO この先のレイアウトはポップアップによったりするかもしれないのでデザインきてから対応する。
    //とりあえずは適当に通信だけ出来るようにする。
    //交換するかどうかの確認ポップアップ
    auto ok_func = [this](){
        //通信する
        auto parent = (TeamBattleEventItemExchangeLayer*)_node;
        parent->exchangeRequest(getPrimaryId());
    };
    auto ng_func = [](){}; //何もしない
    
    string message = exchange_item_name+_need_exchange_item+"個で交換します。\nよろしいですか？";
    auto popup_layer = popupConfirmLayer::create(message);
    popup_layer->setFunction(ok_func, ng_func);
    _node->addChild(popup_layer, ZORDER_ALERT_LAYER, ALERT_LAYER_TAG);
}