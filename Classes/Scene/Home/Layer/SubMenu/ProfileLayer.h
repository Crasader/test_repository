#ifndef __war__ProfileLayer__
#define __war__ProfileLayer__

#include <stdio.h>

#include "cocos2d.h"
#include "NormalScene.h"
#include "SubMenuLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "ModalLayer.h"
#include "NativeBridge.h"
#include "NicknameLayer.h"
#include "CommentLayer.h"

#define MESSEGE_BAR_TAG 1

#define ZORDER_BAR 1

#define PROFILE_NAME_URL "user/profile.php" //プロフィールURL
#define P_BG_TAG 19

USING_NS_CC;

class ProfileLayer : public Layer
{
protected:
    enum btnMenuZOrder {
        Z_button = 1,
        Z_label,
    };
public:
    float ratio;
    Size bgsize;

    string user_master_id = SaveData::loadString("user_master_id");
    
    virtual bool init();
    CREATE_FUNC(ProfileLayer);
    
    //画像の設定
    void setupImage();
    
    void ProfilePage(Ref* sender);
    
    void changePage(Ref* sender);
    
    void profileRequest();
    
    void nameCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    void onEnter(); //初期実行関数
};

#endif /* defined(__war__ProfileLayer__) */
