#ifndef __war__ModalLayer__
#define __war__ModalLayer__

#include "cocos2d.h"
#include "HmsGeometricFunction.h"
#include "ui/CocosGUI.h"
#include "json11.hpp"
#include "PopupStayLayer.h"
#include "HttpConnect.h"
#include "common.h"
#include "SaveData.h"
#include "NativeBridge.h"
#include "HmsUtilityFunction.h"

//モーダルレイヤの内容
#define MODAL_CONTENTS_INFO  1      //お知らせ
#define MODAL_CONTENTS_STONE 2		//石(課金)の購入
#define MODAL_CONTENTS_POWER 3		//スタミナ回復
#define MODAL_CONTENTS_CHARA 4		//カード(キャラ)枠増加
#define MODAL_CONTENTS_FRIEND 5		//友達招待
#define MODAL_CONTENTS_GACHARATE 6	//WebView_GachaRateテスト
#define MODAL_CONTENTS_SERIAL 7     //WebViewシリアル入力
#define MODAL_CONTENTS_TERMS 8		//利用規約 ※WebView
#define MODAL_CONTENTS_EVENT_POWER 9		//イベント行動力回復
#define MODAL_CONTENTS_BOSS_EVENT_POWER 10 //ボスイベント行動力回復

USING_NS_CC;
using namespace network;
using namespace json11;

class ModalLayer : public cocos2d::Layer
{
protected:
	float ratio;
	EventListenerTouchOneByOne* listener;
	Sprite* _frame;
	experimental::ui::WebView* pWebView;
    
    void popupAction(); //フレームを出現させる
    void deleteAction(); //フレームを削除させる
    void closeAction(); //閉じて見えなくするだけ
    int _contents; //表示するコンテンツ番号
    
    MenuItemSprite* pCanselBtn; //いいえボタン
    MenuItemSprite* pOkBtn; //はいボタン
    Label* _message; //表示メッセージ
    
public:
    
    int _schedulerTime ;
    Layer *_parent;
    SEL_SCHEDULE _sel;
    
    static ModalLayer* create(int contents);
   /*
    第二引数にコールバックを呼ぶクラス
    第三引数に関数
    第四引数にコールバックの呼び出しタイミングを代入
    0:指定無し 1:HTTP通信成功時 2以降:今後適当に追加 
    */
    static ModalLayer* create(int contents, Layer *parent, SEL_SCHEDULE sel, int schedulerTime);
    
	virtual bool init(int contents, Layer *parent, SEL_SCHEDULE sel, int schedulerTime); //init関数
    
    void setScheduler(Layer *parent, SEL_SCHEDULE sel, int schedulerTime);
    
    void createEventListener(); //イベントリスナーの作成
    void createShadowLayer(); //影レイヤーの作成
    void createFrameSetUp(); //フレーム構築
    void createCloseButton(); //閉じるボタンの作成
    void createContentsData(); //ポップアップの中身の作成
    void createButton(); //はい、いいえのボタン作成
    
	
	//---------------タッチイベント関数--------------//
    bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	void onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	void onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	void onTouchChancelled(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	
	//---------------http通信関数--------------//
	void modalHttpRequest();													//リクエスト
	void modalHttpResponse(network::HttpClient *sender, network::HttpResponse *response);	//レスポンス
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__ModalLayer__) */
