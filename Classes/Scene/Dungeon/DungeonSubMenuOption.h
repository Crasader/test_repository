#ifndef __war__DungeonSubMenuOption__
#define __war__DungeonSubMenuOption__

#include "cocos2d.h"
#include "DungeonScene.h"
#include "NormalScene.h"
#include "SaveData.h"

#define SOUND_SLIDER 11      //BGM音量スライダー
#define EFFECT_SLIDER 12     //効果音量スライダー
#define EFFECT_ON 13         //エフェクトオンボタン
#define EFFECT_OFF 14        //エフェクトオフボタン
#define SOUND_DEFAULT 15     //音量デフォルトボタン
#define SOUND_SET 16         //音量適用ボタン
#define EFFECT_BUTTON_TAG 20 //エフェクトボタン

USING_NS_CC;

class DungeonSubMenuOptionLayer : public cocos2d::Layer
{
private:
    EventListenerTouchOneByOne *listener;
    
    bool first_flg=true; //オプション表示初期フラグ
    bool _effectFlg; //エフェクトフラグ
    float _bgm_volume; //BGM音量
    float _effect_volume; //エフェクト音量
    float _get_bgm_volume; //変更後のBGM音量
    float _get_effect_volume; //変更後のエフェクト音量
protected:
    enum btnMenuZOrder {
        Z_button = 1,
        Z_label,
    };
public:
    float ratio;
    
    int halfWidth;
    int halfHeight;

    Sprite* bg;
    MenuItemSprite* _effect_btn_on;
    MenuItemSprite* _effect_btn_off;
    
    Size visibleSize;
    
    virtual bool init();
    CREATE_FUNC(DungeonSubMenuOptionLayer);
    
    void createEventListener(); //イベントリスナーの作成
    void createBg(); //背景画像の作成
    void createLabel(); //ラベルの作成
    void createSlider(); //スライダーの作成
    void createButton(); //ボタンの作成
    
    void changeButton(); //ボタンの切り替え
    
    void popupAction();
    void deleteAction();
    
    void saveSetting(Ref* sender);
    void loadSetting(Ref* sender);
    void changeBgmVolume(Ref* sender, Control::EventType controlEvent);
    void changeSeVolume(Ref* sender, Control::EventType controlEvent);
    void changeEffect(Ref* sender);
    
    //タップ処理
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    //初期実行関数
    void onEnter();
};

#endif /* defined(__war__DungeonSubMenuOption__) */
