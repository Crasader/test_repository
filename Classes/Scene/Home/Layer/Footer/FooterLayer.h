#ifndef __War__FooterLayer__
#define __War__FooterLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "HmsVector.h"

#define HOME_BUTTON_TAG 1
#define QUEST_BUTTON_TAG 2
#define CHARA_BUTTON_TAG 3
#define GACHA_BUTTON_TAG 4
#define SHOP_BUTTON_TAG 5
#define FRIEND_BUTTON_TAG 6

USING_NS_CC;

class FooterLayer : public cocos2d::Layer
{
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(FooterLayer);
	
	bool new_img_gacha;
	bool new_img_quest;
	
    void changePage(Ref *sender); //チェンジページ処理
    
    HmsVector<Sprite *> normal_button; //ボタン背景画像
    HmsVector<Sprite *> selected_button; //押したときボタン画像
    HmsVector<MenuItemSprite *> menuItem; //メニューアイテム
    
    void createButtonSprite(); //ボタンの作成
    void showButton(); //ボタンの表示
    void recreateButtonSprite(int tag_id); //ボタンの再構成
    void onEnter(); //実行関数
	void addNewImg(int number); //NEW表示
	void removeNewImg(int number); //NEW非表示
};

#endif /* defined(__War__FooterLayer__) */
