#ifndef TournamentTermSprite_hpp
#define TournamentTermSprite_hpp

#include "cocos2d.h"
#include "HttpConnect.h"
#include "HMSFontUtil.h"
#include "HmsConsoleUtil.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
#include "Utility.h"
#include "json11.hpp"
#include "TournamentEventTopSprite.h"

USING_NS_CC;

class TournamentTermSprite : public Sprite
{
public:
    
    enum Tag{
        T_Term_Label = 1,
        T_Time,
    };
    
    enum ZOrder{
        Z_Term_Label = 1,
        Z_Time,
    };
    
    time_t _wait_start_date_timestamp;
    time_t _remain_time;
    
    TournamentTermSprite(); //コンストラクタ
    
    static TournamentTermSprite* create();
    virtual bool init();
    float ratio;
    
    void calcTime(); //時間の計算
    void createLabel();
    string createRemainTimeStr(); 
    
    void closeAction(); //閉じるアクション
    
    void updateRemainTime(float dt); //時間の更新処理
    void onEnter(); //初期実行関数
};

#endif /* TournamentTermSprite_hpp */
