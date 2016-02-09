#ifndef __war__BazaarExhibitConfirmLayer__
#define __war__BazaarExhibitConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "BazaarExhibitHistoryLayer.h"
#include "CharaSprite.h"

USING_NS_CC;

class BazaarExhibitConfirmLayer: public Layer, public ui::EditBoxDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    
    CC_SYNTHESIZE(string, _chara_list_id, CharaListId); //キャラリストID
    CC_SYNTHESIZE(string, _chara_id, CharaId); //キャラID
    
    //通信で返ってきたデータを格納する変数
    json11::Json _history_data;
    
    BazaarExhibitHistoryLayer* _history_layer; //取引履歴用のレイヤー
    
    string exchange_ticket_count;
    string under_ticket_count;
    bool open_flg;
public:
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Chara_Sprite,
        T_ButtonBg,
        T_Button,
        T_HistoryMenu,
        T_HistoryMenuItem,
        T_EditBox,
        T_Hisitory_BG,
        T_Comment,
        T_Popup_Layer,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Shadow = 1,
        Z_Chara_Sprite,
        Z_ButtonBg,
        Z_Button,
        Z_HistoryMenu,
        Z_HistoryMenuItem,
        Z_EditBox,
        Z_History_BG,
        Z_Comment,
        Z_Popup_Layer,
    };
    
    
    BazaarExhibitConfirmLayer(); //コンストラクタ
    
    static BazaarExhibitConfirmLayer* create();
    virtual bool init();
    
    float ratio;
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createCharaData(); //キャラのデータを生成
    void getHistoryData(); //通信して取引履歴を取得
    void createHistoryLayer(HttpClient *sender, HttpResponse *response); //取引履歴表示レイヤー
    void historyLayerOpenClose(); //取引履歴レイヤーを出したり、消したりする
    
    int getShortageTicketCount();
    /* EditBox処理 */
    virtual void editBoxEditingDidBegin(ui::EditBox *editBox);
    virtual void editBoxEditingDidEnd(ui::EditBox *editBox);
    virtual void editBoxTextChanged(ui::EditBox *editBox, const std::string& text);
    virtual void editBoxReturn(ui::EditBox *editBox);
    
    //出品実行
    void exhibitStart();
    void exhibitStartCallBack(HttpClient *sender, HttpResponse *response);
    
    void onEnter() ; //初期実行関数
};

#endif /* defined(__war__BazaarExhibitConfirmLayer__) */
