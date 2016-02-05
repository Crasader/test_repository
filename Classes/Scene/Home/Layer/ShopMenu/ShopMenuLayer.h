#ifndef __War__ShopMenuLayer__
#define __War__ShopMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "StoneShopAgeConfirmLayer.h"
#include "StoneShopAge2ndConfirmLayer.h"

#define MESSEGE_BAR_TAG 1
#define ZORDER_BAR 1

#define MENU_BUTTON_T 200 //タグ
#define MENU_BUTTON_Z 200 //Zオーダー

//年齢別課金額の設定 ※20歳以上は制限無しなので記述なし
#define AGE_BUY_UNDER16 5000 //16歳未満
#define AGE_BUY_OVER16 10000 //16歳以上

USING_NS_CC;

class ShopMenuLayer : public Layer
{
protected:
	enum btnMenuTag {
		T_charaBox,
		T_powerRestora,
		T_crystalPurchase,
	};
	enum btnMenuZOrder {
		Z_button = 1,
		Z_label,
        ORDER_RECEIVE_LAYER=20,
	};
public:
	float ratio;
	
	virtual bool init();
	CREATE_FUNC(ShopMenuLayer);
	
	//画像の設定
	void setupButton();
	//Shop => MyHome 遷移関数
	void changePage(Ref* sender);
	//キャラ最大所持数追加コールバック関数
	void charaBoxUpGrow(Ref* sender);
	//スタミナ回復コールバック関数
	void powerRestoration(Ref* sender);
	//宝玉購入確認レイヤー呼び出しコールバック関数
	void jewelPurchaseConfirm(Ref* sender);
	
    void shopMenuRequest();
    void shopMenuCallback(HttpClient *sender, HttpResponse *response);
	
	//メニューボタンのSpriteを作成
	Sprite* menuButtonCreateSprite(string img_name, int count);
    
	//初期実行関数
	void onEnter();
};

#endif /* defined(__War__ShopMenuLayer__) */
