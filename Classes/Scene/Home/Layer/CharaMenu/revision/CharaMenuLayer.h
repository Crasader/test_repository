#ifndef __war__CharaMenuLayer__
#define __war__CharaMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class CharaMenuLayer : public Layer
{
private:
    enum Tag {
        T_Formation = 1,
        T_Sell,
        T_CharaList,
        T_Exchange,
        T_Evolution,
        T_Basser,
    };
    
    EventListenerTouchOneByOne *listener;
    HmsVector<Sprite *> _button_list;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(CharaMenuLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createButton(); //ボタンの作成
    void changePage(int tag_no); //ページ遷移
    
    void showExchangeTicketSprite(); //交換チケットの所持枚数
    void showBookButton(); //図鑑ボタンの表示
    
    //キャラデータ通信処理
    void getCharaDataRequest();
    void charaDataCallback(HttpClient *sender, HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__CharaMenuLayer__) */
