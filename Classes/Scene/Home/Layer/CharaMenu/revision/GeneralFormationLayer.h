#ifndef __war__GeneralFormationLayer__
#define __war__GeneralFormationLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "CharaSprite.h"
#include "CharaListSortPopupLayer.h"
#include "CharaSellConfirmLayer.h"
#include "GeneralFormationConfirmLayer.h"
#include "ScrollBarLayer.h"

USING_NS_CC;

class GeneralFormationLayer : public Layer , public ScrollViewDelegate
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
        T_Sell_Bg,
        T_Circle,
        T_Sell_Price_Label,
        T_Select_Label,
        T_Chara_Count_Label,
        T_FadeOut_Chara,
        T_FadeIn_Chara,
        T_Confirm_layer,
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
        Z_Sell_Bg,
        Z_Circle,
        Z_Sell_Price_Label,
        Z_Select_Label,
        Z_Chara_Count_Label,
        Z_FadeOut_Chara,
        Z_FadeIn_Chara,
        Z_Confirm_layer,
        Z_Detail_Layer,
    };
    
    EventListenerTouchOneByOne *listener;
    
    HmsVector<CharaSprite *> _chara; //キャラ画像
    HmsVector<int> _refine_index_list; //絞り込み該当インデックスリスト
    HmsVector<Point> _position_index_list; //キャラのポジションインテックスリスト
    
    Json _chara_data_list; //キャラデータリスト
    Json _user_party_list; //デッキ使用リスト
    
    bool _chara_type[4] = {false}; //絞り込みキャラタイプ
    int _chara_count; //所持キャラ数
    int _row_count; //行数
    int _sort_status; //ソートの状態
    CharaSprite* _touch_chara; //タッチしたキャラ
    
    string _deck_no; //現在選択中のデッキNo
    
    int _view_count_default;
public:
    GeneralFormationLayer(); //コンストラクタ
    
    float ratio;
    
    static GeneralFormationLayer* create(bool *chara_type);
    virtual bool init(bool *chara_type);
    //    CREATE_FUNC(GeneralFormationLayer);
    
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
    
    void checkDeckUsed(); //デッキ使用中かチェックする
    void showCharaSprite(); //キャラの表示
    void actionSort(); //並び替えアクション
    
    Point calcPositionChara(int i, int j); //キャラのポジションの計算
    void refreshSortPage(bool* chara_type); //キャラの並び順などを変更する
    
    void showCharaDetail(float dt); //キャラ詳細レイヤーの表示
    
    //タップイベント
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    void onEnter(); //初期実行関数
    
    void showCharaSpriteInterval(float f); //キャラ画像の生成
    int viewCount;
    ScrollBarLayer *bar;
};

#endif /* defined(__war__GeneralFormationLayer__) */
