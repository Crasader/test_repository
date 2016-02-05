#ifndef __war__DeckFormationTopLayer__
#define __war__DeckFormationTopLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "CharaListSortPopupLayer.h"
#include "CharaSellConfirmLayer.h"
#include "DeckFormationSprite.h"
#include "DeckCopyLayer.h"
#include "DeckNamePopupLayer.h"

USING_NS_CC;

class DeckFormationTopLayer : public Layer
{
private:
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<DeckFormationSprite *> _deck_bg; //背景画像
    
    Json _chara_data_list;
    Json _user_party_list;
    
    map <string, Json> _chara_list; //所持キャラ全てのデータをマップにしたもの（キーはchara_list_id）
    HmsVector<Json > _vec_chara_data; //デッキスプライトに送るキャラデータ
    CharaSprite* _touch_chara; //タッチしたキャラ
    
public:
    
    //タグ
    enum Tag{
        T_General_MenuItem = 1,
        T_Formation_MenuItem,
        T_Menu,
        T_Deck_Bg,
        T_Right_Arrow,
        T_Left_Arrow,
        T_Tournament_Entry_Layer,
        T_Detail_Layer,
        T_Copy_Layer,
        T_Copy_Confirm_Layer,
        T_OkButton_Layer,
        T_Deck_Name_Layer,
        T_Deck_Name_Complete_Layer,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_General_MenuItem = 1,
        Z_Formation_MenuItem,
        Z_Menu,
        Z_Deck_Bg,
        Z_Right_Arrow,
        Z_Left_Arrow,
        Z_Tournament_Entry_Layer,
        Z_Detail_Layer,
        Z_Copy_Layer,
        Z_Copy_Confirm_Layer,
        Z_OkButton_Layer,
        Z_Deck_Name_Layer,
        Z_Deck_Name_Complete_Layer,
    };
    
    enum Pos{
        Left = 1,
        Middle,
        Right,
    };
    
    Sprite* _left_arrow; //矢印左画像
    Sprite* _right_arrow; //矢印右画像
    
    DeckFormationTopLayer(); //コンストラクタ
    float ratio;
    
    static DeckFormationTopLayer* create();
    virtual bool init();
    //    CREATE_FUNC(DeckFormationTopLayer);
    
    CC_SYNTHESIZE(int, _deck_no, DeckNo); //現在のデッキ番号
    
    void setData(); //データの設定
    void createDeckCopyButton(); //デッキコピーボタンの作成
    void createMapData(); //キャラマップデータの作成
    void createCharaJsonData(int i);
    
    void createDeckSprite(); //デッキ画像の作成
    void showDeckSprite(); //デッキ画像の表示
    void createArrowSprite();//矢印の作成
    void actionArrowSprite();//矢印のアニメーション
    void createButton(); //総大将編成、デッキ編成のボタン作成
    
    void initEventListener(); //イベントリスナーの設定
    
    void showTouchDeckSprite(); //タップされた時左右に画像を表示させる関数
    void moveTouchDeckSprite(float delta); //タップ時に画像を移動させる関数
    
    int calcArrayNo(Pos position, int deck_no); //デッキ番号から配列上での番号を計算する
    void showCharaDetail(float dt); //キャラ詳細の表示
    
    void setNowDeckSprite(int origin_deck_no, int previous_deck_no); //現在の中心あるデッキスプライトを更新する
    void setNewDeckName(int deck_no); //デッキ名を新しいデッキ名に変更する
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

    void createTournamentButton();

    void onEnter(); //初期実行関数
};

#endif /* defined(__war__DeckFormationTopLayer__) */
