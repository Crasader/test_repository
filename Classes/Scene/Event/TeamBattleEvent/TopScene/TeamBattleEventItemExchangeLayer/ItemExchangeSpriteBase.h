#ifndef __war__ItemExchangeSpriteBase__
#define __war__ItemExchangeSpriteBase__

#include "cocos2d.h"
#include "HMSFontUtil.h"
#include "HmsGeometricFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"
#include "Utility.h"
#include "CharaSprite.h"

USING_NS_CC;
using namespace std;
using namespace json11;


//何かを集めて何かと交換するSprite(キャラ交換画面のようなもの)のベースクラス
//これを継承すればいいようにしたいけどゴチャってきたから無理かもしれない
//継承先変更部分
//:交換ボタンを押したときのアクション
//:交換に使うアイテムの情報
//:jsonから取得したデータの保存
class ItemExchangeSpriteBase : public Sprite
{
protected:
    EventListenerTouchOneByOne *listener;
    
    float ratio;
    
    Node* _node;        //親レイヤーのポインタを保持しておく
    Json _json_data;    //jsonデータ
    Json _chara_data;   //キャラデータ
    
    string item_type;               //報酬のタイプ(chara:キャラ gold:資金 exchange_ticket:交換チケット)
    string related_primary_id;      //報酬物のID
    string exchange_item_sprite_path; //交換アイテムの画像パス
    string exchange_item_name;      //交換アイテムの名前
    int exchange_item;              //交換アイテムの所持数
    bool is_exchange_count_remain;  //交換アイテムの残り交換回数制限あるかフラグ(N:制限あり Y:制限なし)
    int exchange_count_remain;      //交換アイテムの残り交換回数
    bool is_chara;                  //報酬がキャラかどうかフラグ
    
    CC_SYNTHESIZE(string, _primary_id, PrimaryId); //プライマリーID
    CC_SYNTHESIZE(string, _name, Name); //報酬名
    
    CC_SYNTHESIZE(string, _start_date, StartDate);  //交換掲載開始時間
    CC_SYNTHESIZE(string, _end_date, EndDate);      //交換掲載終了時間

    CC_SYNTHESIZE(string, _need_exchange_item, NeedExchangeItem); //必要交換アイテム枚数
    CC_SYNTHESIZE(bool, _exchange_flg, ExchangeFlg); //既に交換済みかチェック
    CC_SYNTHESIZE(bool, _show_date_flg, ShowDateFlg);//日付の表示フラグ
    
    bool _moving_flg; //動かしたかフラグ
    bool _shortage_exchange_ticket_flg; //必要交換アイテム枚数を満たしているかフラグ
    
    MenuItem* exchange_btn;
    MenuItem* detail_btn;
    Sprite* item_sprite;
    
    //タグ
    enum Tag{
        T_Chara_Sprite = 1,
        T_Name,
        T_Icon_Sprite,
        T_Chara_Detail_MenuItem,
        T_Chara_Detail_Menu,
        T_Exchange_MenuItem,
        T_Exchange_Menu,
        T_Exchange_Ticket,
        T_End_Date_Label,
        T_Exchange_Count_Remain,
        T_Node,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Chara_Sprite = 1,
        Z_Name,
        Z_Icon_Sprite,
        Z_Chara_Detail_MenuItem,
        Z_Chara_Detail_Menu,
        Z_Exchange_MenuItem,
        Z_Exchange_Menu,
        Z_Exchange_Ticket,
        Z_End_Date_Label,
        Z_Node,
    };
public:
    
    
    virtual bool init(json11::Json json, Node* normal_scene);
    
    void initEventListener();           //イベントリスナーの設定
    void setupData(json11::Json json);  //データの設定
    void generateItemSprite();          //交換報酬画像の生成
    void createNameLabel();             //キャラ名の作成
    
    void showNeedExchangeItemLabel();   //必要交換アイテム枚数
    void createButton();                //キャラ詳細と交換ボタンの作成
    void showExchangeEndDate();         //交換終了期間の文字列
    void showExchangeCountRemain();     //残り交換回数の表示
    int exchangeCountRemainDecrement();//残り交換回数を１減らす
    string createExchageEndDateText();  //掲載終了時間の文字列の作成
    
    void createDisableCascadeNode();    //ノードの作成
    
    void setExchangedItem();            //交換済みの状態にする
    void setUnselectedExchangeButton(); //交換アイテムが足りないときに、ボタンを押せなくする処理
    string getRewardItemPath(string item_type); //報酬アイテムの画像パス
    
    //継承先で実装する関数
    virtual void tapExchangeButtonAction(); //交換ボタンタップ時のアクション
    virtual void setExchangeItemData();     //交換に使用するアイテムのデータセット
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){};
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__ItemExchangeSpriteBase__) */
