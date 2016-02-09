#ifndef __war__CharaListLayer__
#define __war__CharaListLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "CharaListSortPopupLayer.h"
#include "CardDetailLayer.h"
#include "ScrollBarLayer.h"

#define SORT_BTN_SIZE 60

USING_NS_CC;

class CharaListLayer : public Layer , public ScrollViewDelegate
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
        T_Normal_Btn,
        T_Rare_Btn,
        T_Super_Btn,
        T_Ultra_Btn,
        T_Ultra_Plus_Btn,
        T_Platinum_Btn,
        T_Rarity_Menu,
        T_Offense_Btn,
        T_Defense_Btn,
        T_Stratege_Btn,
        T_General_Btn,
        T_Type_Menu,
        T_Switch_MenuItem,
        T_Switch_Menu,
        T_FadeOut_Chara,
        T_FadeIn_Chara,
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
        Z_Normal_Btn,
        Z_Rare_Btn,
        Z_Super_Btn,
        Z_Ultra_Btn,
        Z_Ultra_Plus_Btn,
        Z_Platinum_Btn,
        Z_Rarity_Menu,
        Z_Offense_Btn,
        Z_Defense_Btn,
        Z_Stratege_Btn,
        Z_General_Btn,
        Z_Type_Menu,
        Z_Switch_MenuItem,
        Z_Switch_Menu,
        Z_FadeOut_Chara,
        Z_FadeIn_Chara,
        Z_Detail_Layer,

    };
    
    enum ButtonStatus{
        All_Selected = 1,
        All_Unselected,
    };

    EventListenerTouchOneByOne *listener;
    
    HmsVector<CharaSprite *> _chara; //キャラ画像
    HmsVector<int> _refine_index_list; //絞り込み該当インデックスリスト
    HmsVector<Point> _position_index_list; //キャラのポジションインテックスリスト
    Json _chara_data_list; //キャラデータリスト
    
    int _chara_count; //所持キャラ数
    int _row_count; //行数
    int _sort_status; //ソートの状態
    bool _chara_rarity[6] = {false}; //レア度絞り込みパラメータ
    bool _chara_type[4] = {false}; //絞り込みキャラタイプ
    int _view_count_default;
    
    CharaSprite* _touch_chara; //タッチしたキャラ
    
public:
    CharaListLayer(); //コンストラクタ
    
    float ratio;
    
    static CharaListLayer* create(bool *chara_type, bool *chara_rarity);
    virtual bool init(bool *chara_type, bool *chara_rarity);
//    CREATE_FUNC(CharaListLayer);
    
    void initData(bool* chara_type, bool *chara_rarity); //データの初期化
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
    void showCharaSpriteInterval(float f); //キャラ画像の生成
    int viewCount;
    void showSortPopup(Ref* sender); //並び替えポップアップレイヤーの表示
    
    void createRarityButton(); //レア度絞り込みボタンの作成
    void createTypeButton(); //キャラタイプ絞り込みボタンの作成
    void createSwitchButton();
    void selectRarity(Ref* sender); //レア度ボタン選択コールバック
    void selectType(Ref* sender); //ボタン選択コールバック
    
    bool changeBoolean(bool status);
    
    int countCharaRaritySelected(); //キャラレアリティの選択状態の数を返す
    void initCharaRarity(ButtonStatus status); //キャラのレアリティ状態を全て初期化
    
    int countCharaTypeSelected(); //キャラタイプの選択状態の数を返す
    void initCharaType(ButtonStatus status); //キャラのタイプ状態を全て初期化
    
    void refreshSortPage(bool* chara_type); //絞り込み、ソートをしたときのページリフレッシュ
    
    void setRarityButtonAndTypeButtonData(); //レア度とキャラタイプによるボタンデータの設定
    void setRarityButtonStatus(); //レア度ボタン状態を設定
    void setTypeButtonStatus(); //キャラタイプボタン状態を設定
    void setEnableMenuButton(bool boolean);
    
    void createScrollView(); //スクロールビューの作成
    void setScrollViewData(); //スクロールビューのデータの設定（表示範囲や開始位置など）
    Layer* createScrollList(); //スクロールビュー内に表示するデータの設定
    
    Point calcPositionChara(int i, int j); //キャラのポジションの計算
    void showCharaSprite(); //キャラの表示
    
    void refreshPage(bool* chara_type); //キャラの並び順などを変更する
    void actionSort(); //並び替えアクション
    
    void showCharaDetail(float dt); //キャラ詳細レイヤーの表示
    void checkFavoriteChara(); //キャラお気に入りチェック
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    ScrollBarLayer *bar;
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__CharaListLayer__) */
