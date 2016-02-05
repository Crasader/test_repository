#ifndef __war__DeckFormationLayer__
#define __war__DeckFormationLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "CharaListSortPopupLayer.h"
#include "CharaSellConfirmLayer.h"
#include "DeckFormationSprite.h"
#include "DeckFormationConfirmLayer.h"
#include "DeckRecommendLayer.h"
#include "CardDetailLayer.h"
#include "ScrollBarLayer.h"

#define DECK_BG_SIZE 190
#define SORT_BTN_SIZE 60
#define PAGE_BAR_SIZE 90

USING_NS_CC;

class DeckFormationLayer : public Layer , public ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    
    HmsVector<Sprite *> _deck_chara; //デッキのキャラ
    HmsVector<Point > _deck_pos_index_list; //デッキのポジションインデックス
    
    HmsVector<CharaSprite *> _chara;
    HmsVector<Point > _chara_pos_index_list; //キャラ一覧のポジションインデックス
    
    HmsVector<int> _refine_index_list; //絞り込みインデックスリスト
    
    HmsVector<CharaSprite *> _deck_chara_list; //選択されたデッキキャラリスト
    
    Json _chara_data_list;
    Json _user_party_list;
    
    int _row_count; //表示するキャラリストの行数
    
    Sprite* _deck_bg; //デッキ背景
    Sprite* _page_bar; //ページ数の背景
    
    bool _chara_rarity[6] = {false}; //レア度絞り込みパラメータ
    bool _chara_type[4] = {false}; //キャラタイプ絞り込みパラメータ
    
    SpriteBatchNode* _deck_empty; //デッキ空画像
    
    int _touch_chara_index; //タッチしたキャラのインデックス
    bool _deck_touch_flg; //デッキをタッチしたかフラグ
    
    bool scroll_flg = false;
    
    int _view_count_default;
public:
    
    //タグ
    enum Tag{
        T_Title_Bar = 1,
        T_Back_Btn,
        T_Title_Label,
        T_Recommend_MenuItem,
        T_Recommend_Menu,
        T_Bg,
        T_Deck_Bg,
        T_SortOff_Text,
        T_SortOn_Text,
        T_SortBtn_MenuItem,
        T_SortBtn_Menu,
        T_Total_Label,
        T_Offense_Label,
        T_Defense_Label,
        T_Strategist_Label,
        T_Empty,
        T_Deck_Empty_Chara,
        T_Deck_Chara,
        T_Deck_Friend_Sprite,
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
        T_Type_Menu,
        T_Switch_MenuItem,
        T_Switch_Menu,
        T_Page_Bar,
        T_Reset_MenuItem,
        T_Ok_MenuItem,
        T_Menu,
        T_ScrollView,
        T_ScrollLayer,
        T_Circle,
        T_Friend,
        T_Used_Label,
        T_FadeOut_Chara,
        T_FadeIn_Chara,
        T_Recommend_Layer,
        T_Confirm_Layer,
    };
    
    //ZOrder
    enum ZOrder{
        Z_Title_Bar = 1,
        Z_Back_Btn,
        Z_Title_Label,
        Z_Recommend_MenuItem,
        Z_Recommend_Menu,
        Z_Bg,
        Z_Deck_Bg,
        Z_SortOff_Text,
        Z_SortOn_Text,
        Z_SortBtn_MenuItem,
        Z_SortBtn_Menu,
        Z_Total_Label,
        Z_Offense_Label,
        Z_Defense_Label,
        Z_Strategist_Label,
        Z_Empty,
        Z_Deck_Empty_Chara,
        Z_Deck_Chara,
        Z_Deck_Friend_Sprite,
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
        Z_Type_Menu,
        Z_Switch_MenuItem,
        Z_Switch_Menu,
        Z_Page_Bar,
        Z_Reset_MenuItem,
        Z_Ok_MenuItem,
        Z_Menu,
        Z_ScrollView,
        Z_ScrollLayer,
        Z_Circle,
        Z_Friend,
        Z_Used_Label,
        Z_FadeOut_Chara,
        Z_FadeIn_Chara,
        Z_Recommend_Layer,
        Z_Confirm_Layer,
    };

    enum DeckSetStatus{
        Initialize = 0,
        Add,
        Remove,
    };
    
    enum ButtonStatus{
        All_Selected = 1,
        All_Unselected,
    };
    
    CC_SYNTHESIZE(int, _deck_no, DeckNo); //現在選択中のデッキ番号
    CC_SYNTHESIZE(int, _total_number, TotalNumber); //選択合計カード枚数
    CC_SYNTHESIZE(int, _offense, Offense); //選択カード枚数
    CC_SYNTHESIZE(int, _defense, Defense);
    CC_SYNTHESIZE(int, _stratege, Stratege);
    
    int _sort_status; //ソート状態
    DeckSetStatus _deck_status; //デッキ変更警告時に出す判定ステータス
    
    DeckFormationLayer(); //コンストラクタ
    float ratio;
    
    static DeckFormationLayer* create(bool *chara_rarity, bool *chara_type);
    virtual bool init(bool *chara_rarity, bool *chara_type);
//    CREATE_FUNC(DeckFormationLayer);
    
    void initData(bool *chara_rarity, bool *chara_type); //データの初期化
    void initSortData(bool *chara_type); //ソート時のデータの初期化
    void setData(); //データの設定
    void initEventListener(); //イベントリスナーの設定
    void createTitleBar(); //タイトルバーの作成
    void createSortButton(); //並び替えボタンの生成
    string createSortText(); //ソート文字列の作成
    
    void createDeckBg(); //選択デッキの背景の作成
    void createDeckBgIcon(); //アイコンの作成
    void createDeckParamLabel(); //デッキのパラメータラベルの作成
    void createDeckPosIndex(); //デッキポジションインデックスの作成
    void createDeckCharaBg(); //デッキのキャラ枠の作成
    
    void createRarityButton(); //レア度絞り込みボタンの作成
    void createTypeButton(); //キャラタイプ絞り込みボタンの作成
    void createSwitchButton();
    void selectRarity(Ref* sender); //レア度ボタン選択コールバック
    void selectType(Ref* sender); //ボタン選択コールバック
    
    void createCharaSprite(); //キャラ一覧の画像の作成
    void sortCharaData(); //キャラデータのソート
    float calcMaxCharaIdLength(); //キャラIDの最大長さを調べる
    
//    HmsVector<CharaSprite *> sortCharacter(HmsVector<CharaSprite *> chara_list, string sort_param);
    long getSortVal(CharaSprite *chara, string sort_param);
    
    void createRefineIndexList(); //絞り込みインデックスの作成
    void createCharaPosIndex(); //キャラ一覧のポジションインデックスの作成
    Point calcPositionChara(int i, int j); //キャラポジションの計算
    void showCharaSprite(); //キャラ画像の表示
    
    void createScrollView(); //スクロールビューの作成
    void setScrollViewData(); //スクロールビューのデータの設定（表示範囲や開始位置など）
    Layer* createScrollList(); //スクロールビュー内に表示するデータの設定
    
    void createPageBar(); //画面下部のページバー
    
    void setRarityButtonAndTypeButtonData(); //レア度とキャラタイプによるボタンデータの設定
    void setRarityButtonStatus(); //レア度ボタン状態を設定
    void setTypeButtonStatus(); //キャラタイプボタン状態を設定
    
    void setDeckData(); //最初に現在設定中のデッキデータを設定しておく
    
    void createDeckCharaList(CharaSprite* chara); //デッキキャラリストの作成
    void setSelectedCharaToDeckSprite(int element_number, string chara_id); //デッキ選択画像に選択したキャラを設定する
    void removeSelectedCharaToDeckSprite(int element_number); //デッキ選択画面の画像を取り除く
    void updateSelectedNumber(CharaSprite::CharaType type, DeckSetStatus status); //選択した合計キャラ数とタイプを更新する
    
    void createNumberCircle(int value, CharaSprite* chara); //選択ナンバー画像をキャラにつける
    void removeNumberCircle(int value, CharaSprite* chara); //選択ナンバーを取り除く
    void turnColorSelect(); //全ての選択色を反転させる
    
    int checkDeckListNullCount(); //デッキリストのnullptr数を調べる
    
    void initCharaSelect(); //キャラ選択状態を初期化する
    void showSortPopup(Ref* sender); //並び替えポップアップレイヤーの表示
    
    void refreshSortPage(bool* chara_type); //絞り込み、ソートをしたときのページリフレッシュ
    void actionSort(); //並び替えアクション
    
    void setDataReturnConfirmLayer(HmsVector<CharaSprite *> &deck_chara_list); //確認画面から戻ってきたときの、並び替えをしたデータの設定
    void setRecommendData(int offense_number, int defense_number, int strategist_number); //おすすめ選択データの設定
    
    bool changeBoolean(bool status);
    
    int countCharaRaritySelected(); //キャラレアリティの選択状態の数を返す
    void initCharaRarity(ButtonStatus status); //キャラのレアリティ状態を全て初期化
    
    int countCharaTypeSelected(); //キャラタイプの選択状態の数を返す
    void initCharaType(ButtonStatus status); //キャラのタイプ状態を全て初期化
    
    int checkCharaHaveNumber(CharaSprite* chara); //デッキ選択枚数チェック
    void setUsedCharaSprite(CharaSprite* chara); //キャラが3枚選択された場合、そのキャラを選択できない状態にする
    void removeUsedCharaSprite(CharaSprite* chara); //選択解除して2枚選択された場合になった場合、そのキャラの選択を解除する
    void showCharaDetail(float dt); //キャラ詳細の表示
    
    void setTouchMode(int touch_index); //タッチ中の状態にする
    void setUnTouchMode(int touch_index); //タッチ中の状態から戻す
    
    void changeCharaPosition(int touch_index); //画像の入れ替え
    void changeDeckCharaList(int touch_index); //キャラリストデータの入れ替え
    void refreshNumbering(); //番号を採番しなおす
    
    void setEnableMenuButton(bool boolean); //メニューボタンのタッチ判定の変更
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    void scrollEnd(float dt);
    
    void onEnter(); //初期実行関数
    
    void showCharaSpriteInterval(float f); //キャラ画像の生成
    int viewCount;
    ScrollBarLayer *bar;
};

#endif /* defined(__war__DeckFormationLayer__) */
