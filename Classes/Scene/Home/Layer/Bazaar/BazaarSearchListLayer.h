#ifndef __War__BazaarSearchListLayer__
#define __War__BazaarSearchListLayer__

#include "cocos2d.h"
#include "json11.hpp"
#include "HttpConnect.h"

USING_NS_CC;
using namespace std;
using namespace json11;

class BazaarListLayer;

class BazaarSearchListLayer : public Layer
{
private:
    BazaarListLayer* bazaarList;
    int ticketCount;
    Json bazaarSeachList;
    
    CC_SYNTHESIZE(string, sortName, SortName); //名前
    CC_SYNTHESIZE(int, sortCharaType, SortCharaType); //タイプ
    CC_SYNTHESIZE(int, sortRarelity, SortRarelity); //レア
    CC_SYNTHESIZE(int, sortCost, SortCost); //コスト
public:
    //タグ
    enum Tag{
        T_ScrollView = 1,
        T_ScrollLayer,
        T_Exhibit,
        T_Search_Condition,
        T_Search_list,
        T_Search_list_Comment,
        T_Update,
        T_Exchange_Ticket_BG,
        T_Exchange_Ticket_Label,
        T_Title,
        T_Other,
        T_PopupLayer
    };
    
    //ZOrder
    enum ZOrder{
        Z_ScrollView = 1,
        Z_ScrollLayer,
        Z_Button,
        Z_SearchList,
        Z_Search_List_Comment,
        Z_Update,
        Z_Exchange_Ticket_BG,
        Z_Exchange_Ticket_Label,
        Z_PopupLayer
    };
    float ratio;
    BazaarSearchListLayer();
    virtual bool init();
    CREATE_FUNC(BazaarSearchListLayer);
    
    void GetBazaarData();
    void GetBazaarDataCallBack(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void showExchangeTicketSprite(); //所持チケット枚数
    void createButton(); //ボタンの作成
    void createSearchPopup(); //条件検索用のポップアップ
    void exchangeCharaDataRequest(string exchange_chara_id);
    void exchangeCharaDataCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    string exchangeCharaResultMessage(string result);
    void refreshList();
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__BazaarSearchListLayer__) */
