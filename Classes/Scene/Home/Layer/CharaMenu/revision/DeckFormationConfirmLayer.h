#ifndef __war__DeckFormationConfirmLayer__
#define __war__DeckFormationConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "SaveData.h"

#define POS_X_DECK_FIRST 63
#define POS_Y_DECK_FIRST 340
#define POS_X_DECK_DISTANCE 97
#define POS_Y_DECK_DISTANCE 96

USING_NS_CC;

class DeckFormationConfirmLayer : public Layer
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
    HmsVector<CharaSprite *> _deck_chara_list; //デッキキャラリスト
    HmsVector<CharaSprite *> _chara; //キャラ画像
    
    HmsVector<Point > _position_index; //ポジションインデックス
    
    Sprite* _chara_bg; //背景スクエア
    int _touch_chara_index; //タッチしたキャラのインデックス
    
public:
    DeckFormationConfirmLayer(); //コンストラクタ
    
    float ratio;
    static DeckFormationConfirmLayer* create(HmsVector<CharaSprite *> &deck_chara_list);
    virtual bool init(HmsVector<CharaSprite *> &deck_chara_list);
    //    CREATE_FUNC(DeckFormationConfirmLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createBg(); //背景の作成
    void createTitle(); //タイトルの作成
    void createButton(); //はい、いいえボタンの作成
    void createDeckChara(); //デッキキャラの作成
    Json createCharaJsonData(int i); //キャラのJsonデータの作成
    void createPositionIndex(); //ポジションインデックスの作成
    Point calcPositionChara(int i, int j); //デッキキャラのポジションの計算
    void showDeckChara(); //デッキキャラの表示
    
    string createRequestCharaDeckList(); //リクエストで送るキャラリストの作成
    
    void changeCharaPosition(int touch_index); //キャラの場所入れ替え
    void changeDeckCharaList(int touch_index); //デッキキャラリストの内容を入れ替える
    
    void setTouchMode(int touch_index); //タッチ中の状態にする
    void setUnTouchMode(int touch_index); //タッチ中の状態から戻す
    
    //キャラ売却通信処理
    void sellCharaDataRequest();
    void sellCharaDataCallback(HttpClient *sender, HttpResponse *response);
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__DeckFormationConfirmLayer__) */