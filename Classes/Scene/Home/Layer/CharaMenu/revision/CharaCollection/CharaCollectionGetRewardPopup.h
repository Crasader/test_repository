#ifndef __war__CharaCollectionGetRewardPopup__
#define __war__CharaCollectionGetRewardPopup__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class CharaCollectionGetRewardPopup : public Layer
{
private:
    enum Tag{
        T_Popup = 1,
    };
    
    enum Order{
        O_shadow= 1,
        O_confetti,
        O_light,
        O_booty,
        O_chara,
        O_reward_title,
        O_balloon,
        O_balloon_label,
        O_booty_label,
    };
    
    EventListenerTouchOneByOne *listener;
    
    string rewardItemType;
    string rewardItemId;
    string comment;
    string itemCount;
    string charaName;
    bool actionFlg;
    
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
    Sprite* booty ;
    Sprite* light;
    Sprite* chara;
    Sprite* rewardTitle;
    Sprite* confetti01;
    Sprite* confetti02;
    Sprite* naviBalloon;
    Label* bootyLabel;
    
    void createSprite() ; //以下のスプライトの生成
    void createBg(); //背景画像の作成
    void createBooty() ; //報酬画像の作成
    void createBootyTitle(); //報酬の文字
    void createLight() ; //後ろの光の生成
    void createChara() ; //キャラの生成fan
    void createRewardTitle() ; //「〜達成」の文字
    void createNaviBaloon() ; //吹き出しの作成
    void createConfetti() ; //紙吹雪の作成
    void moveConfetti(float dt) ; //紙吹雪をループさせて動かす
    
    void createRunAction() ; //runActionの生成
    
public:
    float ratio;
    
    static CharaCollectionGetRewardPopup* create(string rewardItemType, string rewardItemId, string comment, string count, string charaName);
    virtual bool init(string rewardItemType, string rewardItemId, string comment, string count, string charaName);
    
    void initEventListener(); //イベントリスナーの登録
    
    void setIdForBattleRestart(int battle_list_id);
    
    //閉じたときに呼ぶ関数を持つレイヤー
    Layer* parent;
    //閉じたときに呼ぶ関数
    SEL_SCHEDULE selForClose;
    
    //閉じたときに呼ばれる関数の登録
    void setSchedulerForClose(Layer *parent, SEL_SCHEDULE sel);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__BeginnerGuideClearAnimLayer__) */

