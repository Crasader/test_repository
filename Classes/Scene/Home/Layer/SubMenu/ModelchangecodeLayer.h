#ifndef __war__ModelchangecodeLayer__
#define __war__ModelchangecodeLayer__

#include "cocos2d.h"
#include "NormalScene.h"
#include "SubMenuLayer.h"
#include "ModelchangeLayer.h"
#include "QuestionLayer.h"
#include "HelpLayer.h"
#include "common.h"
#include "HmsGeometricFunction.h"
#include "XTLayer.h"
#include "cocos-ext.h"
#include "HttpConnect.h"
#include "HmsTemplateFunction.h"
#include "SaveData.h"
#include "network/HttpClient.h"
#include "ModelchangecodeLayer.h"
#include "PopupStayLayer.h"

#define MESSEGE_BAR_TAG 1

#define ZORDER_BAR 1

#define MODELCHANGECODE_LABEL_TAG 10 //機種変更コードラベルのタグ
#define MODELCHANGESPRITE_TAG 20 //機種変更コード表示スプライトのタグ

#define IMG_OK_BT "init/Home/Common/bt_ok.png"  //OKボタン画像
#define HTTP_Modelchangecode_URL "user/new_code.php" //URL

USING_NS_CC;

class ModelchangecodeLayer : public Layer
{
public:
    float ratio;
    
    string user_master_id = SaveData::loadString("user_master_id");
    
    virtual bool init();
    CREATE_FUNC(ModelchangecodeLayer);
    
    //画像の設定
    void setupImage();
    
    //閉じるボタン
    void removeSprite();
    
    void changePage(Ref* sender);
    
    void modelchangecodeCheckRequest();
    
    //通信レスポンス
    void modelchangecodeCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    void onEnter();
};

#endif /* defined(__war__ModelchangecodeLayer__) */
