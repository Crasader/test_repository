#ifndef __War__DeckPackSelectLayer__
#define __War__DeckPackSelectLayer__

#include "cocos2d.h"
#include "common.h"
#include "TutorialScene.h"

#define STARTER_DECK_ARRAY_SIZE 5

USING_NS_CC;
using namespace std;

class DeckPackSelectLayer : public cocos2d::Layer
{
private:
    
    EventListenerTouchOneByOne *listener;
    float _touchFlg;
    bool start_se_flg=true;
	
    enum DeckTag{
        TAG_DECK_0 = 0,
        TAG_DECK_1,
        TAG_DECK_2,
        TAG_CLIP,
        TAG_LEFT_ARROW,
        TAG_RIGHT_ARROW,
    };
    
    enum DeckOrder{
        ORDER_DECK_0 = 1,
        ORDER_DECK_1 = 1,
        ORDER_DECK_2 = 1,
    };
    
    Point _deckSelectCenterPosition ; //選択するデッキの背景スプライト
    
    float _descriptionCenterPosition_x_list[5] ; //説明画像中心点のx座標の配列(デッキ毎で座標が異なる)
    float _descriptionCenterPosition_y ; //説明画像中心点のy座標
    
    Point _decknameCenterPosition; //デッキ名画像中心点
    Point _decknameHiddenCenterPosition; //デッキ名の画像が隠れる 中心位置
    
    //選択するデッキ
    Sprite *deck_select_bk_list[5]; //背景のスプライト
    Sprite *deck_select_chara_list[5]; //キャラのスプライト
    
    //デッキの説明
    Sprite *description_sprite_list[5];
    
    //デッキの名前
    Sprite *deck_name_sprite_list[5];
    
    ClippingNode* createMascByTargetSprite(Node *masc_target_node) ;//マスクのスプライトを生成する
    DrawNode* getMascShapeByTargetSprite(Node *masc_target_node) ; //マスクのスプライトのかたちを生成
    
    void initHeaderSprite(); //上部のヘッダー
    void initDeckNameSprite(); //デッキ名の初期化
    void initDeckSelectSprite(); //選択式のスターターデッキ関係のスプライトの作成
    void initArrowSprite(); //矢印のスプライトの生成
    void initDescriptionSprite(); //説明画像
    void initSelectButtonWindow(); //選択ボタン周りの初期化
    
    //ページを切り替え
    void changePage(Ref *ref);
    
    void createEventLitener(); //イベントリスナーの作成
    void finalizationSprite(); //スプライトの削除　メモリ管理上必要な後処理
    
    //選択肢の自動移動
    void autoSwipeLeft();
    void autoSwipeRight();
    void autoSwipeCenter();
    
    //選択肢を触っているか判定
    bool checkContainDeckPosition(Point touch_point) ;
    
    //タップ可能か ※タッチし始めからスワイプが終了するまでタップ不可能にする
    bool getEnableTap() ;
    void setEnableTap(bool b) ;
    bool _enable_tap;
    
    //中心のカードに応じて選択肢を所定の位置に変更する
    void updatePositionForSelectDeck(float dt) ;
    
    //スワイプ関係の変数
    int _centerDeckIndex ; //今中心にあるデッキ
    
    int calcLeftDeckIndex(int center_index) ; //現在の中心のカードから見て左側のカードのインデックスを取得する
    int calcRightDeckIndex(int center_index) ; //現在の中心のカードから見て右側のカードのインデックスを取得する
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    float ratio;
    Sprite* _touchSprite;
    MessageSprite* message_bg;
    
    void onEnter(); //初期実行関数
    virtual bool init();
    
    //説明画像のポップアップと削除のAction
    void popupDescriptionAction(float dt);
    void deletePopupDescriptionAction() ;
    
    //デッキ名の出現と削除のAction
    void downDeckNameAction(float dt);
    void upDeckNameAction();
    
    //矢印をリピートで動かすAction
    void repeatArrowsAction() ;
    
    //矢印の表示と非表示を切り替える
    void showArrows(float dt) ;
    void hideArrows() ;
    
    //タップした位置が含まれているかどうかタグで判定
    bool checkContainTouchPointByTag(Point touch_point, int tag) ;
        
public:
    CREATE_FUNC(DeckPackSelectLayer);
};

#endif /* defined(__War__DeckPackSelectLayer__) */
