#ifndef __war__BeginnerGuideClearAnimLayer__
#define __war__BeginnerGuideClearAnimLayer__

#include "cocos2d.h"
#include "NormalScene.h"

USING_NS_CC;

class BeginnerGuideClearAnimLayer : public cocos2d::Layer
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
    
    string _reward_item_type;
    string _reward_related_id;
    string _comment;
    string _count;
    string _chara_name;
    bool _all_complate_anim_flg ;
    bool _action_flg; 
    
    void popupAction(); //ポップアップ表示アクション
    void deleteAction(); //ポップアップ削除アクション
    
    Sprite *_booty ;
    Sprite *_light;
    Sprite *_chara;
    Sprite *_reward_title;
    Sprite *_confetti_1;
    Sprite *_confetti_2;
    Sprite *_navi_balloon;
    Label *_booty_label;
    
    void createSprite() ; //以下のスプライトの生成
    void createBg(); //背景画像の作成
    void createBooty() ; //報酬画像の作成
    void createBootyTitle(); //報酬の文字
    void createLight() ; //後ろの光の生成
    void createChara() ; //キャラの生成
    void createRewardTitle() ; //「〜達成」の文字
    void createNaviBaloon() ; //吹き出しの作成
    void createConfetti() ; //紙吹雪の作成
    void moveConfetti(float dt) ; //紙吹雪をループさせて動かす
    
    void createRunAction() ; //runActionの生成
    
public:
    float ratio;
    
    static BeginnerGuideClearAnimLayer* create(string reward_item_type, string reward_related_id, string comment, string count, string chara_name, bool all_complate_anim_flg);
    virtual bool init(string reward_item_type, string reward_related_id, string comment, string count, string chara_name, bool all_complate_anim_flg);
    
    void initEventListener(); //イベントリスナーの登録
    
    void setIdForBattleRestart(int battle_list_id);
    
    //閉じたときに呼ぶ関数を持つレイヤー
    Layer *_parent;
    //閉じたときに呼ぶ関数
    SEL_SCHEDULE _sel_for_close;
    
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

