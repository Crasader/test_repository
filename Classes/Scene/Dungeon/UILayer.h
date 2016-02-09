#ifndef __war__UILayer__
#define __war__UILayer__

#include "cocos2d.h"
#include "HmsVector.h"
#include "Chara.h"
#include "NumericCount.h"

#define SUBMENU_LAYER_TAG 500
#define REST_TURN_IMG_TAG 511
#define REST_TURN_LABEL_TAG 512
#define SUPPORT_IMG_TAG 513
#define SUPPORT_BK_IMG_TAG 514
#define SUPPORT_LABEL1_TAG 515
#define SUPPORT_LABEL2_TAG 516

#define SUPPORT_Z 10

USING_NS_CC;

class UILayer : public Layer
{
private:
    float ratio;
    int dot_count = 0;
    
    EventListenerTouchOneByOne *listener;
    Sprite *attack_img;
    Sprite *menu_img;
    
    
    Sprite *back_ui;
    Sprite *back_ui2;
    
    Sprite *hpGuageFullSprite[2];
    Sprite *hpGuageSprite[2];
    Sprite *hpGuageDangerSprite[2];
    Sprite *hpGuageRedSprite[2];//減るときのアニメーション用
    Sprite *attack_order[2];//先攻後攻
    
    Label *name_label[2];
    Sprite *hp_label[2];
    Sprite *hp_labelL[2];
    
    NumericCount *numeric_power[2];
    
    
    Sprite *rectSprite[2];
    
    bool attackButtonClicked=false;
    bool menuButtonClicked=false;
    
    //ボスイベバトル用
    Label *situation_label;
public:
    Sprite *stamp_img;
    Sprite *timeGuageSprite[2];
    Label *rest_time_label;
    
    bool stampButtonClicked=false;
    Point power_point[2];
    Point power_point1fig[2];
    Point power_point2fig[2];
    
    vector<pair <int,Sprite *>> passiveBufferIcon[2];//使用キャラプライマリidと画像
    vector<pair <int,Sprite *>> wholeBufferIcon[2];//使用キャラプライマリidと画像
    
    virtual bool init();
    CREATE_FUNC(UILayer);
    
   
    void changeAttackButton(string state);
    bool attack_button_compel_false = false;//アタックボタンを強制的に押せなくする。チュートリアル用
    
    void initialize();
    void setTimer(int percentage_list[], bool check_flg);
    void updatePower(int now_power, int turn_index);
    void updatePlayerInfo(int hp, int hp_max, int index);
    void timeAlert(bool flg, bool index);
    bool alert_flg[2];
    
    void insertPassiveIcon(int index, int primary, int skill_id);
    void removePassiveIcon(int primary);
    void insertWholeBufferIcon(int index,Chara *used_chara, int icon_id=0);
    void removeWholeBufferIcon(int primary);
    void resetIconPosition();
    Vec2 getIconPosition(Sprite *icon_sprite);
    void updateBufferIcon();
    void resetShowAllIcon();
    void showStamp(int stamp_no ,int index);
    void updateBossEventRestTime(int boss_event_rest_time);
    
    void update(float dt);
    float stamp_cant_time = 0.0f;
    
    //タップ処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
};
#endif /* defined(__war__UILayer__) */
