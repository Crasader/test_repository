#ifndef __war__DeckCopyConfirmLayer__
#define __war__DeckCopyConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "SaveData.h"

#define POS_X_DECK_FIRST 63
#define POS_Y_DECK_FIRST 340
#define POS_X_DECK_DISTANCE 97
#define POS_Y_DECK_DISTANCE 96

USING_NS_CC;

class DeckCopyConfirmLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    //タグ
    enum Tag{
        T_Shadow = 1,
        T_Title,
        T_Close_Menu,
        T_Chara_Bg,
        T_Chara,
        T_Friend,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Shadow = 1,
        Z_Title,
        Z_Close_Menu,
        Z_Chara_Bg,
        Z_Chara,
        Z_Friend,
    };
    
    Sprite* _bg; //背景画像
    int _origin_deck_no;   //コピー元のデッキNo
    int _previous_deck_no; //選択したコピー先デッキNo
    
    HmsVector<CharaSprite *> _chara; //キャラ画像
    HmsVector<Point > _position_index; //ポジションインデックス
    
    map <string, Json> _chara_list; //所持キャラ全てのデータをマップにしたもの（キーはchara_list_id）
    
    Json _chara_data_list;
    Json _user_party_list;
    
    Sprite* _chara_bg; //背景スクエア
    int _touch_chara_index; //タッチしたキャラのインデックス
    
public:
    DeckCopyConfirmLayer(); //コンストラクタ
    
    float ratio;
    static DeckCopyConfirmLayer* create(int previous_deck_no);
    virtual bool init(int previous_deck_no);
    //    CREATE_FUNC(DeckCopyConfirmLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void setData(); //データの設定
    void createMapData(); //キャラマップデータの作成
    void createBg(); //背景の作成
    void createTitle(); //タイトルの作成
    void createButton(); //はい、いいえボタンの作成
    void createDeckChara(); //デッキキャラの作成
    Json createCharaJsonData(int i); //キャラのJsonデータの作成
    void createPositionIndex(); //ポジションインデックスの作成
    Point calcPositionChara(int i, int j); //デッキキャラのポジションの計算
    void showDeckChara(); //デッキキャラの表示
    
    //デッキコピー通信処理
    void copyDeckDataRequest();
    void copyDeckDataCallback(HttpClient *sender, HttpResponse *response);
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__DeckCopyConfirmLayer__) */
