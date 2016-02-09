#ifndef __war__MypageInfoLayer__
#define __war__MypageInfoLayer__

#include "cocos2d.h"
#include "TitleScene.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace extension;
using namespace json11;

class MypageInfoLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    void createBg(); //背景の作成
    void showImage(); //画像の表示
    string _image_path; //表示画像のパス
    
    bool _touchable; //タッチ可能フラグ
public:
    float ratio;
    
    MypageInfoLayer(); //コンストラクタ
    static MypageInfoLayer* create(string image_path);
    virtual bool init(string image_path);
    
    void initEventListener(); //イベントリスナーの初期化
    
    void popupAction();
    void deleteAction();
    
    //タッチイベント処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    void onEnter(); //初期実行関数
};


#endif /* defined(__war__MypageInfoLayer__) */
