#ifndef __war__CommentLayer__
#define __war__CommentLayer__

#include <stdio.h>

#include "cocos2d.h"
#include "NormalScene.h"
#include "SubMenuLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "ModalLayer.h"
#include "NativeBridge.h"
#include "ProfilePopup.h"
#include "EditBoxUtil.h"

#define COMMENT_TAG 16 //コメント編集
#define COMMENT_URL "user/comment.php" //コメント編集用URL

USING_NS_CC;

class CommentLayer : public Layer
{

public:
    string valid_input_text ;
    float ratio;
    
    virtual bool init();
    CREATE_FUNC(CommentLayer);
    
    //画像の設定
    void setupImage();
    void changePage(Ref* sender);
    void commentRequest(Ref* sender);
    
    void nameCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void profileCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__CommentLayer__) */
