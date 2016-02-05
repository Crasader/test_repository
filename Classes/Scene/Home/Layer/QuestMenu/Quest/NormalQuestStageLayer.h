#ifndef __war__NormalQuestStageLayer__
#define __war__NormalQuestStageLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "QuestStageSprite.h"
#include "QuestExplainSprite.h"

USING_NS_CC;

class NormalQuestStageLayer : public Layer, public ScrollViewDelegate
{
private:
    EventListenerTouchOneByOne *listener;
    
    enum Power{
        GO = 1,
        LESS_MAX_POWER,
        LESS_POWER,
    };
    
    enum Tag{
        T_SUM_BG = 12,
        T_HELP_BUTTON = 13,
    };
    
public:
    NormalQuestStageLayer();
    ~NormalQuestStageLayer();
    
    Json _quest_explain_data;
    Json _quest_stage_data;
    
    float ratio;
    int cell_size;
    string name_or_reward = "name";
    
    QuestExplainSprite* _explain_sp; //クエスト説明画像
    HmsVector<QuestStageSprite *> _stage_sp;
    HmsVector<Json > _quest_stage_level_data;
    
    bool touchButtonFlg; //ボタンフラグ
    
    static NormalQuestStageLayer* create();
    virtual bool init();
//    CREATE_FUNC(NormalQuestStageLayer);
    
    void createQuestExplainSprite(); //クエストの説明画像
    
    void initEventListener(); //イベントリスナーの登録
    void setQuestExplainData(); //説明画像データの設定
    void setStageData(); //ステージデータの設定
    void createQuestStageSprite(); //クエストリストの作成
    void createStageLavelData(); //ステージレベルのリスト作成
    void createSwitchButton();//表示切り替えボタンの作成
    void createRewardCountSprite(); //報酬数の数の作成
    void createStarHighlight(); //スターを「星を5つ集めよう」という表示
    
    
    void createScrollView(); //スクロールビューの作成
    void initSprite(Json json); //スプライトの作成
    Layer* createList(); //リストの作成
    
    void changePage(int tag); //ページの遷移
    
    void saveQuestEnemySpriteData(QuestStageSprite* stage_sp); //最終確認の敵のデータ
    void saveQuestPointData(QuestStageSprite* stage_sp); //ステージデータの保存
    
    Power checkPower(int need_power);
    
    void _setCascadeOpacityEnabled(Node* parent, bool enabled); //再起的に透過率を全てに適用させる関数
    void _setCascadeColorEnabled(Node* parent, bool enabled); //再起的に色合いを全てに適用させる関数
    
    void soundNG(); //NG音の再生

    //スクロール実装必須メソッド
    void scrollViewDidScroll(ScrollView *view);
    void scrollViewDidZoom(ScrollView *view){};
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void update(float dt);
    void onEnter(); //初期実行関数
    
    void createHelpButton() ;
};

#endif /* defined(__war__NormalQuestStageLayer__) */
