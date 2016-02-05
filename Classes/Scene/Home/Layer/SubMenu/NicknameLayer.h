#ifndef __war__NicknameLayer__
#define __war__NicknameLayer__

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
#define NICK_NAME_URL "user/nickname.php" //ニックネーム変更用URL

USING_NS_CC;

class NicknameLayer : public Layer
{
public:
    float ratio;
    
    string user_master_id = SaveData::loadString("user_master_id");
    string _get_name;
    
    virtual bool init();
    CREATE_FUNC(NicknameLayer);
    
    //画像の設定
    void setupImage();
    void changePage(Ref* sender);
    void nicknameRequest(Ref* sender);
    
    void profileCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    void nameCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    void onEnter(); //初期実行関数
    
};

#endif /* defined(__war__NicknameLayer__) */
