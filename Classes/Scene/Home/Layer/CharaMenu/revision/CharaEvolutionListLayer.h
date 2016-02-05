#ifndef __war__CharaEvolutionListLayer__
#define __war__CharaEvolutionListLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "CharaListSortPopupLayer.h"
#include "CardDetailLayer.h"

USING_NS_CC;

class CharaEvolutionListLayer : public Layer , public ScrollViewDelegate
{
private:
    
    //タグ
    enum Tag{
        T_ScrollView = 1,
        T_ScrollLayer,
        T_SortOn_Text,
        T_SortOff_Text,
        T_SortOn_Sp,
        T_SortOff_Sp,
        T_SortBtn_MenuItem,
        T_SortBtn_Menu,
        T_Num_Base,
        T_Num_Label,
        T_FadeOut_Chara,
        T_FadeIn_Chara,
        T_Help_MenuItem,
        T_Help_Menu,
        T_Detail_Layer,
    };
    
    //Zオーダー
    enum ZOrder{
        Z_ScrollView = 1,
        Z_ScrollLayer,
        Z_SortOn_Text,
        Z_SortOff_Text,
        Z_SortOn_Sp,
        Z_SortOff_Sp,
        Z_SortBtn_MenuItem,
        Z_SortBtn_Menu,
        Z_Num_Base,
        Z_Num_Label,
        Z_FadeOut_Chara,
        Z_FadeIn_Chara,
        Z_Help_MenuItem,
        Z_Help_Menu,
        Z_Detail_Layer,
    };
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<CharaSprite *> _chara; //キャラ画像
    HmsVector<int> _refine_index_list; //絞り込み該当インデックスリスト
    HmsVector<Point> _position_index_list; //キャラのポジションインテックスリスト
    Json _chara_data_list; //キャラデータリスト
    Json _user_party_list; //デッキ使用リスト
    
    int _chara_count; //所持キャラ数
    int _row_count; //行数
    int _sort_status; //ソートの状態
    bool _chara_type[4] = {false}; //絞り込みキャラタイプ
    
    CharaSprite* _touch_chara; //タッチしたキャラ
    bool _touch_flg;
    
public:
    CharaEvolutionListLayer(); //コンストラクタ
    
    float ratio;
    
    static CharaEvolutionListLayer* create(bool *chara_type);
    virtual bool init(bool *chara_type);
//    CREATE_FUNC(CharaEvolutionListLayer);
    
    void initData(bool* chara_type); //データの初期化
    void initSortData(bool* chara_type); //ソート時のデータの初期化
    
    void initEventListener(); //イベントリスナーの設定
    void setData(); //キャラ情報の設定
    void sortCharaData(); //キャラデータのソート
//    HmsVector<CharaSprite *> sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param);
    long getSortVal(CharaSprite *chara, string sort_param);
    
    void createRefineIndexList(); //絞り込みインデックスの作成
    void createPositionIndexList(); //表示位置のインデックスリストの作成
    
    void createSortButton(); //並び替えボタンの生成
    string createSortText(); //ソート文字列の作成
    void showCharaCount(); //所持キャラ数の表示
    void createCharaSprite(); //キャラ画像の生成
    void showSortPopup(Ref* sender); //並び替えポップアップレイヤーの表示
    
    void createScrollView(); //スクロールビューの作成
    void setScrollViewData(); //スクロールビューのデータの設定（表示範囲や開始位置など）
    Layer* createScrollList(); //スクロールビュー内に表示するデータの設定
    
    Point calcPositionChara(int i, int j); //キャラのポジションの計算
    void showCharaSprite(); //キャラの表示
    
    void refreshPage(bool* chara_type); //キャラの並び順などを変更する
    void actionSort(); //並び替えアクション
    
    void showCharaDetail(float dt); //キャラ詳細レイヤーの表示
    void checkDeckUsed(); //デッキ使用中チェック
    void checkFavoriteChara(); //キャラお気に入りチェック
    void checkEnableEvolution();//進化可能かどうかをチェック
    void createHelpButton();//ヘルプボタン作成
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__CharaEvolutionListLayer__) */
