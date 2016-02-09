#ifndef __War__SkillConfirmLayer__
#define __War__SkillConfirmLayer__

#include "cocos2d.h"
#include "Chara.h"
#define SCL_CHARA_SELECT_TAG 1
#define SCL_HEADER_TAG 2
#define SCL_HEADER_P_TAG 10
#define SCL_FOOTER_TAG 3
#define SCL_FOOTER_YES_TAG 4
#define SCL_FOOTER_NO_TAG 5
#define SCL_SKILL_NAME_TAG 6
#define SCL_SKILL_DETAIL_TAG 7
#define SCL_SKILL_CONFIRM_TAG 8
#define SCL_SKILL_COST_TAG 9

USING_NS_CC;
using namespace std;

class SkillConfirmLayer : public Layer
{
private:
    EventListenerTouchOneByOne *listener;
    float ratio;
	Chara *used_chara;//スキル使用キャラ
    MenuItemSprite* yes_btn = nullptr;
    MenuItemImage *hatudouItem = nullptr;
    MenuItemImage *cancelItem = nullptr;
    
    bool selectTargetFlg = false;
    Sprite* shadow = nullptr;
    Sprite* target_mark = nullptr;
    
    vector<Chara *> target_charas;//選択スキル対象キャラ達
    vector<pair<int, int>> target_places;//選択スキル対象スペース
    
    //Chara *targeted_chara = nullptr;//選んでいるキャラ
    int targeted_index = 0;
    
    ClippingNode *clipper = nullptr;
public:
    virtual bool init();
    CREATE_FUNC(SkillConfirmLayer);
    
    void makeSelectUI(Point mark_position);
    void showDetail(Chara *used_chara);
    void selectTarget();
    void selectPlace();
    void skillPrepare();
    void skill();
    void close();
    Sprite* header_bg_img_passive = nullptr;
	Sprite* header_bg_img;
	Sprite* footer_bg_img=nullptr;
	Sprite* chara_select;
	Label* skill_name;
	Label* skill_detail;
	HmsVector<Sprite *> skill_cost_sprite;
	bool visible_flg;
    void end();
    
    //タップ処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif /* defined(__War__SkillConfirmLayer__) */
