#ifndef __war__DeckChoiceBaseLayer__
#define __war__DeckChoiceBaseLayer__

#include "cocos2d.h"
#include "CharaSprite.h"
#include "DeckFormationSprite.h"
#include "DeckSprite.h"

USING_NS_CC;

//デッキ選択ベースレイヤー
//デッキ選択する画面(出撃系以外)はこれを子供に持てば良い
class DeckChoiceBaseLayer : public Layer
{
private:
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<Sprite *> _deck_bg; //デッキリスト
    Json _chara_data_list;
    Json _user_party_list;
    float deck_position_y; //デッキ郡のY座標
    float deck_scale; //デッキ一覧のサイズ(標準1.0f)
    float is_half; //デッキ一覧を半分サイズの表示にするか
    
    map <string, Json> _chara_list; //所持キャラ全てのデータをマップにしたもの（キーはchara_list_id）
    HmsVector<Json > _vec_chara_data; //デッキスプライトに送るキャラデータ
    CharaSprite* _touch_chara; //タッチしたキャラ
public:
    
    //タグ
    enum Tag{
        T_Deck_Bg = 1,
        T_Right_Arrow,
        T_Left_Arrow,
        T_Join_MenuItem,
        T_Join_Menu,
        T_Alert_Label,
        T_Detail_Layer,
        T_OkButton_Layer,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_Deck_Bg = 1,
        Z_Right_Arrow,
        Z_Left_Arrow,
        Z_Join_MenuItem,
        Z_Join_Menu,
        Z_Alert_Label,
        Z_Detail_Layer,
        Z_OkButton_Layer,
    };
    
    enum Pos{
        Left = 1,
        Middle,
        Right,
    };
    
    Sprite* _left_arrow; //矢印左画像
    Sprite* _right_arrow; //矢印右画像
    
    float ratio;
    
    static DeckChoiceBaseLayer* create(float _deck_position_y, float _deck_scale, bool _is_half);
    virtual bool init(float _deck_position_y, float _deck_scale, bool _is_half);
    //    CREATE_FUNC(DeckChoiceBaseLayer);
    
    CC_SYNTHESIZE(int, _deck_no, DeckNo); //現在のデッキ番号
    
    void setData(); //データの設定
    void createMapData(); //キャラマップデータの作成
    void createCharaJsonData(int i);
    
    void createDeckSprite(); //デッキ画像の作成
    void showDeckSprite(); //デッキ画像の表示
    void createArrowSprite();//矢印の作成
    void actionArrowSprite();//矢印のアニメーション
    
    void initEventListener(); //イベントリスナーの設定
    
    void showTouchDeckSprite(); //タップされた時左右に画像を表示させる関数
    void moveTouchDeckSprite(float delta); //タップ時に画像を移動させる関数
    
    int calcArrayNo(Pos position, int deck_no); //デッキ番号から配列上での番号を計算する
    void showCharaDetail(float dt); //キャラ詳細の表示
    void halfDeckScroll(Touch *touch); //半分表示のデッキの中身をスクロールした下半分が見えるようにする
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__DeckChoiceBaseLayer__) */
