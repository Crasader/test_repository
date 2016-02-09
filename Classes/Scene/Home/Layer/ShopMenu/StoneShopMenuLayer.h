#ifndef __War__StoneShopMenuLayer__
#define __War__StoneShopMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "InAppBillingManager.h"
#include "StoneShopBuyConfirmLayer.h"
#include "cocos-ext.h"
#include "JewelSprite.h"
#include "WebViewLoadLayer.h"

#include "PopupChangePageLayer.h"

//#define ITEM_NUM 8
#define TOKUTEI_HELP_URL HOST"user/?&p=page__help_tokutei"	//特定商取引法についてヘルプで開くURL

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class StoneShopMenuLayer : public cocos2d::Layer, public ScrollViewDelegate, public InAppBillingDelegate
{
private:
    enum Order{
        ORDER_SCROLLVIEW=10,
        ORDER_RECEIVE_LAYER=20,
    };
    
    
    int _cell_size;	//１つのセルサイズ
    int _purchase;	//購入する宝玉数
	int _price;		//購入する金額
    
    int _display_item_count; //アイテム表示件数
    Json _buyable_shop_item_list;
    
    void initShopItemArrayByOsType(HmsVector<string> &shopItemNameArray, HmsVector<string> &shopItemPriceArray, HmsVector<string> &shopItemNameIdArray);
    
    
public:
    float ratio;
    virtual bool init();
	//EventListenerTouchOneByOne *listener;
    CREATE_FUNC(StoneShopMenuLayer);
    
    //課金用delegate
    void setDelegate(string item_name, string price, string itemId, int userId);
    virtual void onSuccess(string itemId);
    virtual void onCancel();
    
    void createScrollView();	//スクロールビューの作成
    Layer* createList();		//リストの作成
	void setupImage();			//画像の設定
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //購入アイテムの表示フロー  購入権限チェック(pay_check.php) ->  購入可能アイテム取得(get_shop_item_list.php)  -> リスト生成処理
    void startListDisplayFlow();
    
    //購入前通信
    void payCheckRequest();
    void payCheckCallback(HttpClient *sender, HttpResponse *response);
    void getShopItemListRequest();
    void getShopItemListCallback(HttpClient *sender, HttpResponse *response);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__StoneShopMenuLayer__) */
