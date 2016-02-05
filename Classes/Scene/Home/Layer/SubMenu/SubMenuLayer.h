#ifndef __War__SubMenuLayer__
#define __War__SubMenuLayer__

#include "cocos2d.h"
#include "NormalScene.h"

#define QUESTION_URL HOST"user/?&p=page__inquiry"  //お問い合わせを開く用URL

USING_NS_CC;

class SubMenuLayer : public Layer
{
private:
    
    enum Tag {
        T_Inquiry = 1,
        T_Help,
		T_Ranking,
        T_Profile,
        T_Homepage,
#if(IF_ANDROID)
        T_Serial,
#endif
        T_Title,
        T_Other,
    };
    
    EventListenerTouchOneByOne *listener;
    HmsVector<Sprite *> _button_list;
    
public:
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(SubMenuLayer);
    
    void initEventListener(); //イベントリスナーの設定
    void createButton(); //ボタンの作成
    
    void changePage(int tag_no);
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};
#endif /* defined(__War__SubMenuLayer__) */
