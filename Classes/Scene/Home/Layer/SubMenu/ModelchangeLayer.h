#ifndef __war__ModelchangeLayer__
#define __war__ModelchangeLayer__

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
#include "ModelchangeLayer.h"
#include "ModelchangecodeLayer.h"
#include "TitleScene.h"
#include "SubMenuPopup.h"

#define MESSEGE_BAR_TAG 1
#define ZORDER_BAR 1

//タグ
#define MODELCHANGECODE_LABEL_TAG 10 //機種変更コードラベルのタグ
#define MODELCHANGESPRITE_TAG 20 //機種変更コード表示スプライトのタグ
#define EDITBOX_TAG 15 //テキストボックスのタグ
//画像
#define IMG_OK_BT "init/Home/Common/bt_ok.png"  //OKボタン画像
#define IMG_TEXTBOX_BACK "init/Home/Friend/entry_column.png"  //テキストボックスの背景画像
#define IMG_MADALLAYER_POPUP "init/Home/Popup/popup_500.png" //メッセージ背景
//URL
#define HTTP_MODELCHANGE_URL "user/new_models.php" //変更処理URL
#define HTTP_MODELCHANGE_FINISH_URL "user/finish_code.php" //変更処理後URL

USING_NS_CC;

class ModelchangeLayer : public Layer
{
public:
    float ratio;
    
    string user_master_id = SaveData::loadString("user_master_id");
    
    virtual bool init();
    CREATE_FUNC(ModelchangeLayer);
    
    //画像の設定
    void setupImage();
    //閉じるボタン
    void removeSprite();
    //テキストボックスのID取得
    string getTextID();
    
    void changePage(Ref* sender);
    
    //機種変更ボタンが押されたときのコールバック
    void onModelChangeButton(Ref *sender) ;
    void modelchangeCheckRequest();
    
    void modelchangeFinishRequest(string uid);
    //変更通信レスポンス
    void modelchangeCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    //変更後処理通信レスポンス
    void modelchangeFinishCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);

    void modelchangecodeCheckRequest();
    
    void moveTitle();
    
    void modelchangecodeCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);

    //初期実行関数
    void onEnter();
};
#endif /* defined(__war__ModelchangeLayer__) */
