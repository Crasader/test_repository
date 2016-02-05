#ifndef __War__UserNameInsertLayer__
#define __War__UserNameInsertLayer__

#include "cocos2d.h"
#include "TutorialScene.h"
#include "EditBoxUtil.h"
#include "extensions/cocos-ext.h"


USING_NS_CC;
USING_NS_CC_EXT;

class UserNameInsertLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    Sprite* popup_bg; //名前入力用の背景画像
    int max_input_length ; //入力可能な文字数
    bool touchFlg;
    
public:
    
    float ratio;
    MessageSprite* message_bg; //文字の背景画像
    
    virtual bool init();
    CREATE_FUNC(UserNameInsertLayer);
    
    void createEventLitener(); //イベントリスナーの作成
    
    void createMessage(); //メッセージ画像の作成
    void createBg(); //画像の表示
    void createButton(); //ボタンの作成
    void createEditBox(); //入力フォームの作成
	
	//NGワードチェック用コールバック処理
	void nameCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    //    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__War__UserNameInsertLayer__) */
