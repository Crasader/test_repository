#include "HmsGeometricFunction.h"
#include "NumericCount.h"

bool NumericCount::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    this->setCascadeOpacityEnabled(true);
    this->setCascadeColorEnabled(true);
    ignoreAnchorPointForPosition(false);
    setAnchorPoint(Vec2(0.5,0.5));
    setContentSize(Size(0,0));
    ratio = getVisibleRatio();
    
    return true;
}

void NumericCount::setValue(int value, string effect_type){
    int before_value = now_value;//今までの数値
    
    now_value = value;
    if(value < 0) now_value = 0; //0未満の場合は0に
    if((int)powf(10, digits)-1 < value) now_value = (int)powf(10, digits)-1;//桁数オーバーの場合は999などの最大の値に
    
    string str = to_string(now_value);
    removeAllChildren();
    sprite_vec.clear();
    
    for(int i=0; i<str.length(); i++) {
        string s = str.substr(i, 1);//一文字づつに分解
        sprite_vec.push_back(Sprite::create(png_path + s + ".png"));
        sprite_vec.back()->setAnchorPoint(Vec2(0.5,0.5));
        sprite_vec.back()->setScale(ratio);
        float numeric_adjust = sprite_vec.back()->getContentSize().width*ratio + adjust*ratio;//数値の間隔
        sprite_vec.back()->setPosition(Point(-numeric_adjust/2 * (str.length()-1) + numeric_adjust * i, 0));//数値が中央にきれいに並ぶように
        addChild(sprite_vec.back(),1);
    }
    
    if(effect_type == "big" && before_value!=now_value){
        auto scale = ScaleTo::create(0.12f, 1.3f);
        auto ease = EaseBounceOut::create(scale);
        auto scale2 = ScaleTo::create(0.12f, 1.0f);
        auto seq = Sequence::create(ease,scale2, NULL);
        runAction(seq);
    }
}
void NumericCount::update(float dt)
{
    float time = 0.3f;
    int some = time/0.0333;
    
    if(counter > some){
        counter = 0;
        this->unschedule(schedule_selector(NumericCount::update));
        setValue(target_value, "");
    }else{
        if(now_value != (int)((float)(target_value - now_value)/(float)some*(float)counter + now_value)){
            now_value = (int)((float)(target_value - now_value)/(float)some*(float)counter + now_value);
            setValue(target_value, "");
        }
        
        counter++;
    }
}
