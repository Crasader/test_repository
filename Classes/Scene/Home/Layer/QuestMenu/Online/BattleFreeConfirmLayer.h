#ifndef __War__BattleFreeConfirmLayer__
#define __War__BattleFreeConfirmLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "UserInfoCellSprite.h"
#include "HmsList.h"
#include "HomeCommon.h"

#define MESSEGE_BAR_TAG 1

USING_NS_CC;

class BattleFreeConfirmLayer : public Layer
{
private:
    int battle_list_id;
    bool cancel_flg;
    MenuItemImage* cancel_btn;
public:
    Json _tips_json ;
    
    float ratio;
	EventListenerTouchOneByOne* listener;
	Sprite *_tips_frame;
    Sprite *_left_chara;
    Sprite *_right_chara;
	Label *_tips_title;
	Label *_tips_comment;
    
	Label *_next;
	DrawNode *_next_shape;
    
    enum Tag{
       T_ANIMATION,
       T_MESSAGE,
    };
    
    virtual bool init();
    CREATE_FUNC(BattleFreeConfirmLayer);
    
    void setup();
    void showTipsAction(); 
    void createTips();
    void waitBattle();
    DrawNode* getTriangleShape(const Point& point);
    void waitBattleCallback(HttpClient *sender, HttpResponse *response);
    void cancelWaitBattle();
    void cancelWaitBattleCallback(HttpClient *sender, HttpResponse *response);
	void showLoadingEffect(Node* parent, string filename, Vec2 position, float scale);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif /* defined(__War__BattleFreeConfirmLayer__) */
