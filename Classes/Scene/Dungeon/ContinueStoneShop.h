
#ifndef __war__ContinueStoneShop__
#define __war__ContinueStoneShop__

#include "cocos2d.h"
#include "NormalScene.h"
#include "InAppBillingManager.h"
#include "cocos-ext.h"
#include "WebViewLoadLayer.h"

#include "PopupChangePageLayer.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class ContinueStoneShop : public cocos2d::Layer, public InAppBillingDelegate
{
private:
    int _cell_size;	//１つのセルサイズ
    int _purchase;	//購入する宝玉数
    int _price;		//購入する金額
    
    int _display_item_count; //アイテム表示件数
    Json _buyable_shop_item_list;
    
    enum Order{
        ORDER_SCROLL_VIEW=10,
    };
    
    
public:
    float ratio;
    virtual bool init();
    //EventListenerTouchOneByOne *listener;
    CREATE_FUNC(ContinueStoneShop);
    
    //課金用delegate
    void setDelegate(string item_name, string price, string itemId, int userId);
    virtual void onSuccess(string itemId);
    virtual void onCancel();
    
    
    //購入前通信
    void payCheckRequest();
    void payCheckCallback(HttpClient *sender, HttpResponse *response);
    void getShopItemListCallback(HttpClient *sender, HttpResponse *response);
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__war__ContinueStoneShop__) */
