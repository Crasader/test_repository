#ifndef __War__BannerSprite__
#define __War__BannerSprite__

#include "cocos2d.h"
#include "common.h"
#include "json11.hpp"
#include <time.h>
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HMSFontUtil.h"
#include "SaveData.h"


USING_NS_CC;
using namespace json11;
using namespace std;

class BannerSprite : public cocos2d::Sprite
{
private:
    
public:
    enum ExtensionType{
        EXCEPTION,
        PNG,
        PLIST
    };
    
    BannerSprite() ;
    static BannerSprite* create(string image_path);
    virtual bool init(string image_path);
    bool initSwitch(string file_name); //拡張子によって初期化方法を切り替える 初期化成功時にtrueを返す
    void onEnter(); //初期実行関数
    
    
    // //// バナー毎の固有の機能の　関数・変数を設定 start -----
    
    //----- 繊維先を指定 --------
    CC_SYNTHESIZE(string, _change_page_name, ChangePageName);
    
    //----- 相互リンクバナー 相互リンクのurlをセット・取得  -------
    CC_SYNTHESIZE(string, _mutual_link_url, MutualLinkUrl);
    
    //------ 残り時間を表示する -------
    void addDisplayEndDate(string end_date) ;
    CC_SYNTHESIZE(bool, _event_end, EventEnd); //イベントが終了したことを判定
    
    void updateTimeDisplay(float dt); //時間を更新する
    Label *_end_date_label ;
    Label *_prefix_time_text ;
    Label *_suffix_time_text ;
    Sprite *_end_date_back ;
    time_t _server_diff_time ;
    time_t _end_time;
    
    //---------- バナー毎の固有の機能の　関数・変数を設定 end -----
    
    
    void echoLog() ;
private:
    
    ExtensionType _extension_type;
    
    SpriteFrame* getSpriteFrame(string file_name) ; //スプライトフレームを取得
    RepeatForever *_repeat_forever ;
    ExtensionType getExtensionTypeByFilename(string file_name) ;
    
};

#endif /* defined(__War__BannerSprite__) */
