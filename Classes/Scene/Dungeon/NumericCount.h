
#ifndef __war__NumericCount__
#define __war__NumericCount__

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class NumericCount : public Layer
{
private:
    int now_value;//現在表示している数値
    int target_value;//カウンター用目標数値
    int counter;
    
public:
    float ratio;
    int digits = 2; //桁数
    int adjust = -6; //数字の間隔。プラスの数値で離れる、マイナスで近づく
    string png_path = "init/Dungeon/design/battle_num_g";
    vector<Sprite *> sprite_vec;//桁ごとの画像
    
    void setValue(int value ,string effect_type);//数値の変更
    void setCountingValue(int value);//数値を少しづつカウントしながら変更
    
    virtual bool init();
    CREATE_FUNC(NumericCount);
    
    void update(float dt);
};
#endif /* defined(__war__NumericCount__) */
