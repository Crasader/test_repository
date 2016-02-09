#ifndef __war__GachaResultAnimationLayer__
#define __war__GachaResultAnimationLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "XTLayer.h"
#include "cocos-ext.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "GachaMenuLayer.h"
#include "network/HttpClient.h"
#include "Effect.h"
#include "SuperAnimNodeV2.h"
#include "CharaDetailSpriteForResultAnimation.h"


#define CARD_OPEN_NO_TAG 10

class GachaResultAnimationLayer : public cocos2d::Layer, public SuperAnim::SuperAnimNodeListener
{
private:
    enum BattleFreeLayerTag{
        TAG_RERITY = 120,
        TAG_EFFECT,
    };
    
    EventListenerTouchOneByOne *listener;
    CC_SYNTHESIZE(int,_gachatype, GachaType);
    CC_SYNTHESIZE(int,_card_count, CardCount);
    
    //演出中出したり消したりする画面を覆う白と黒のスプライト
    Sprite *_white_sprite ;
    Sprite *_black_sprite;
    
    void createColorSprite();
    void startWhiteFadeOut() ; //アニメーション終了後の最初の白い画面のフェードアウト
    
    void touchIntervalCount(float dt) ;
    bool _touch_enable;
public:
    
    float ratio;
    Size visibleSize;
    float header_height;//ヘッダーの高さ
    float footer_height;//フッターの高さ
    float main_screen_size;//ヘッダーフッター以外の高さ
    float main_screen_w;//画面の横幅
    float main_screen_size_split_10;
    float main_screen_size_split_5;
    int roop_count=0;//ループ回数
    bool touch_end=false;
    
    int _rare_card[30];//カードのレア度
    int _chara_list_id[30];//ユーザーでフォルトでのID
    int _hit_card[30];//当たったカードのID
    int _chara_type[30];//当たったカードのタイプ
    string _card_name[30];//カード名
    bool se_bool; //SEの判定
    
    //カードのスプライトをaddするNode(中心を軸に回転させるにはSpriteを大きさを持たないNodeに依存させる必要がある)
    Sprite *_card_back;//裏のカード
    Sprite *_card_open;//表カード
    Sprite *_chara_detail_sprite;//表カード
	
	Sprite *_card_index_bg;//ガチャ回数表示用
	Label *_info_label;//ガチャ回数の文言ラベル
	bool info_flg;//情報表示可否判定用
    bool _box_gacha_flg = false;

    virtual bool init();
    CREATE_FUNC(GachaResultAnimationLayer);
    void initEventListener(); //イベントリスナーの設定
    
    void setBoxGachaFlg(bool box_gacha_flg);
    
    void setData(int gacha_type,int card_count,int hit_card[],int rare_card[],int chara_type[], int chara_list_id[]);//生成された際にセット
    
    void getSize();//サイズの取得
    
    void createBg();//背景画像の作成
    
    void setCardByIndex(int index);//カードの設置
	
	void setGachaInfo();//ガチャ情報設置※スキップボタンも設置
    
    void setCharaDetailByIndex(int index) ;
    
    void createResultAnimation();//リザルトアニメーションの再生
    
    int _now_card_index ;
    void updateForResultFlow(float dt) ;
    void resultActionForceEnd(int card_index) ;//現在のアニメーションを強制終了
    void resultActionByCardIndex(int card_index);//結果のアニメーション (戻り値で一連のアクションの終了までの時間を返す)
    
    bool _now_animation ;
    
    void effect();//エフェクト処理
    
    void deleteRarityAction();
    void popupRarity(int card_index);//レアリティ表示
    
    void animationEnd();//アニメーションの終了処理
    bool _animation_end ;
    
    void setScheduler(Layer *parent, SEL_SCHEDULE sel) ; //アニメーション終了時呼ばれるメソッドを登録;
    Layer *_parent;
    SEL_SCHEDULE _sel;
    
    void allTapDisaleRemove();//タッチ禁止レイヤーを削除

    //初期実行関数
    void onEnter();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};
#endif /* defined(__war__GachaResultAnimationLayer__) */
