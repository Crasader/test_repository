#ifndef __War__TutorialNaviLayer__
#define __War__TutorialNaviLayer__

#include "cocos2d.h"
#include "TutorialNaviCharaSprite.h"
#include "HmsList.h"

#define MESSEGE_BAR_TAG 1

USING_NS_CC;

#define IMG_OK_BT "init/Home/Common/bt_ok.png"                  //OKボタン画像

class TutorialNaviLayer : public Layer
{
public:
    enum TutorialNaviLayerTag{
        TAG_NAVI_CHARA,
    };
    enum class CommentIndex{
        COMMENT_1 = 1,
        COMMENT_2,
        COMMENT_3,
        COMMENT_4,
        COMMENT_5,
        COMMENT_6,
        COMMENT_7,
        COMMENT_8,
        COMMENT_9,
        COMMENT_10,
        COMMENT_11,
        COMMENT_12,
        COMMENT_13,
        COMMENT_14,
        COMMENT_15,
        COMMENT_END = 999,
    };
    float ratio;
    
    CommentIndex _index;
    
    //tip表示済みフラグ
    bool visited_tips;
    
    virtual bool init();
    CREATE_FUNC(TutorialNaviLayer);

    //基本
    void setup(Layer *parent, CommentIndex index) ;
    void setScheduler(Layer *parent, SEL_SCHEDULE sel);
    
    
    //ナビキャラを出現させるアクションの取得
    ActionInterval *getDownAction();
    
    //ナビキャラを上に移動する時のアクション
    ActionInterval *getUpAction();
    
    void naviCharaTalkStart(string message);
    void close();
    
    void callback();
    
    //UserDefault操作
    static void initCompleteData(); //初期化
    static bool checkCommentComplete(CommentIndex index); //完了済みか調べる
    static void setCommentComplete(CommentIndex index, bool finish_flg);
    static bool checkCommentAllComplete();   //全て完了しているか調べる
    
private:
    EventListenerTouchOneByOne *listener;
    TutorialNaviCharaSprite *navi_chara_sprite ;
    Point navi_chara_up_position ;
    Point navi_chara_down_position ;
    
    
    Layer *parent;
    SEL_SCHEDULE sel;
    
    //タップを不可能にするリスナーの追加
    void initListener() ;
    bool onTouchBegan(Touch *touch, Event *event) ;
    
    string createMessageByCommentIndex(CommentIndex index) ;
    
    //リスナーの削除
    void removeListener() ;
    
    void onEnter(); //初期実行関数
    
    void initalize();
    
};

#endif /* defined(__War__TutorialNaviLayer__) */
