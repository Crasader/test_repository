#ifndef __War__BazaarTopLayer__
#define __War__BazaarTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class BazaarTopLayer : public Layer
{
private:
    
    enum Tag {
        T_Inquiry = 1,
        T_BG,
        T_Exhibit,
		T_MyExhibit,
        T_Search,
        T_Help,
        T_Title,
        T_Other,
    };
    
    enum Zorder {
        Z_BG = 1,
        Z_Button,
        Z_Text,
        Z_Text2,
        Z_SearchList
    };
    
    int new_flg;
    string exchange_day_limit_flg;
    string can_exchange_count;
    string can_exhibit_count;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(BazaarTopLayer);
    
    void createBG(); //背景の作成
    void createButton(); //ボタンの作成
    void getExhibitData(); //出品情報取得
    void getExhibitDataCallBack(HttpClient *sender, HttpResponse *response); //出品情報取得コールバック
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__War__BazaarTopLayer__) */
