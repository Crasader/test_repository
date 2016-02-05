#ifndef __War__TutorialNaviCharaSprite__
#define __War__TutorialNaviCharaSprite__

#include "cocos2d.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "HmsConsoleUtil.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;

class TutorialNaviCharaSprite : public cocos2d::Sprite
{
private:
    Label *talk_label ;
    Label *createMessageLabel(string message); //メッセージの表示
    
    //メッセージの中心位置
    float _message_center_position_y ;
    
    enum Tag{
        TAG_STATE_CHANGE_ACTION,
    };
    
    enum class TalkState{
        STATE_TALK_WAIT=0, //トーク待機状態 この状態以外ではコメントを受け付けない
        STATE_TALK_NOW, //トーク中 この状態でforceAllTalk()を実行されるか、喋りきるとSTATE_TALK_ENDに遷移する
        STATE_TALK_END, //トーク終了
    };
    
    TalkState talk_state ;
    
    //コメントが終了した時の三角形
    DrawNode *getTriangleShape() ;
    
    //コメント終了後の三角形を動かす
    void showTriangle() ;
    
public:
    
    bool isTalkWait() ; //トーク待機
    bool isTalkEnd() ; //トークが終了
    bool isTalkNow() ; //トークが途中
    
    //トーク待機状態にする
    void talkWait() ;
    
    //メッセージに\t(タブ文字)を含めるとそこで文字の表示を若干止める予定
    void talkStart(string message) ;
    
    static TutorialNaviCharaSprite* create();
    virtual bool init();
    
    //しゃべるアクションを開始
    void *runTalkAction(Label *label, const char message_str[]); 
    void forceAllTalk(); //コメントを強制的にすべて表示する
};

#endif /* defined(__War__TutorialNaviCharaSprite__) */
