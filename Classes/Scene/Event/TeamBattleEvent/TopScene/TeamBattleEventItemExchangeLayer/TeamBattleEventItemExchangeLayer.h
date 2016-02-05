#ifndef __War__TeamBattleEventItemExchangeLayer__
#define __War__TeamBattleEventItemExchangeLayer__

#include "cocos2d.h"
#include "ScrollSprite.h"
#include "TeamBattleEventItemExchangeSprite.h"

USING_NS_CC;

class TeamBattleEventItemExchangeLayer : public Layer, ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    
    HmsVector<TeamBattleEventItemExchangeSprite *> _exchange_item_sp; //交換アイテムの画像
    json11::Json _exchange_item_data_list; //交換アイテムの情報リスト
    
    HmsVector<Point > _vec_position_index; //ポジションインデックスリスト
    
    int _cell_size; //セルの大きさ
    int _event_item; //所持イベントアイテム数
    
    enum Tag {
        T_ScrollView = 1,
        T_ScrollLayer,
        T_Inquiry,
        T_Event_Item_Bg,
        T_Event_Item_Label,
        T_Popup_Layer = 100,
    };
    
    enum Zorder {
        Z_ScrollView = 1,
        Z_ScrollLayer,
        Z_Button,
        Z_Popup_Layer = 100,
    };
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(TeamBattleEventItemExchangeLayer);
    
    void onEnter(); //初期実行関数
    void initEventListener(); //イベントリスナーの設定
    
    void createEventItem(); //所持イベントアイテム数表示
    void getExchangeItemInfoRequest(); //通信処理
    void exchangeItemInfoCallback(HttpClient *sender, HttpResponse *response);
    
    void createPositionIndex(); //ポジションインデックスの作成
    void sortExchangeItem(); //交換済みアイテムを下に持っていくソート処理
    void checkExchangedItemAndShortage(); //交換済みのアイテムか、又は所持イベントアイテムが足りているかチェックする
    void actionUpdatePage();
    
    void createScrollView();
    void setScrollViewData();
    Layer* createScrollList();
    
    void exchangeRequest(string reward_exchange_id);
    void exchangeRequestCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
};
#endif /* defined(__War__TeamBattleEventItemExchangeLayer__) */
