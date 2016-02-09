#ifndef __war__TournamentEventDeckChoiceLayer__
#define __war__TournamentEventDeckChoiceLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "CharaListSortPopupLayer.h"
#include "CharaSellConfirmLayer.h"
#include "DeckFormationSprite.h"
#include "DeckCopyLayer.h"
#include "DeckNamePopupLayer.h"
#include "TournamentTermSprite.hpp"

USING_NS_CC;

class TournamentEventDeckChoiceLayer : public Layer
{
private:
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<DeckFormationSprite *> _deck_bg; //背景画像
    HmsVector<bool > _vec_alert_flg; //参加条件アラートフラグ
    
    Json _chara_data_list;
    Json _user_party_list;
    
    map <string, Json> _chara_list; //所持キャラ全てのデータをマップにしたもの（キーはchara_list_id）
    HmsVector<Json > _vec_chara_data; //デッキスプライトに送るキャラデータ
    CharaSprite* _touch_chara; //タッチしたキャラ
    
    Json _condition_match_list; //条件マッチリスト
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
        T_Tournament_Time_Sprite,
        T_Tournament_Time_Label,
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
        Z_Tournament_Time_Sprite,
        Z_Tournament_Time_Label,
    };
    
    enum Pos{
        Left = 1,
        Middle,
        Right,
    };
    
    Sprite* _left_arrow; //矢印左画像
    Sprite* _right_arrow; //矢印右画像
    
    TournamentEventDeckChoiceLayer(); //コンストラクタ
    float ratio;
    
    static TournamentEventDeckChoiceLayer* create();
    virtual bool init();
    //    CREATE_FUNC(TournamentEventDeckChoiceLayer);
    
    CC_SYNTHESIZE(int, _deck_no, DeckNo); //現在のデッキ番号
    
    void setData(); //データの設定
    void showTournamentTerm(); //受付終了時刻の表示
    void createMapData(); //キャラマップデータの作成
    void createCharaJsonData(int i);
    
    void createDeckSprite(); //デッキ画像の作成
    void showDeckSprite(); //デッキ画像の表示
    void createArrowSprite();//矢印の作成
    void actionArrowSprite();//矢印のアニメーション
    void createButton(); //参加するボタンの設定
    
    void alertTournamentConditionNotMatchWithChara(); //トーナメント条件にマッチしないキャラを赤く点滅させる
    void checkCondition(); //トーナメント条件に該当するかチェック
    
    void initEventListener(); //イベントリスナーの設定
    
    void showTouchDeckSprite(); //タップされた時左右に画像を表示させる関数
    void moveTouchDeckSprite(float delta); //タップ時に画像を移動させる関数
    
    int calcArrayNo(Pos position, int deck_no); //デッキ番号から配列上での番号を計算する
    void showCharaDetail(float dt); //キャラ詳細の表示
    
    void setNowDeckSprite(int origin_deck_no, int previous_deck_no); //現在の中心あるデッキスプライトを更新する
    void createAlertText(); //警告テキストの作成
    void changeAlertLabel(); //警告テキストの表示、非表示
    
    //通信処理
    void checkEventConditionhttpRequest() ;
    void checkEventConditionCallback(HttpClient *sender, HttpResponse *response) ;
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__TournamentEventDeckChoiceLayer__) */
