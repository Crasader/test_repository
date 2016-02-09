
#ifndef __war__HMSFontUtil__
#define __war__HMSFontUtil__

#include "cocos2d.h"
#include "common.h"

USING_NS_CC;
using namespace std;

class HMSFontUtil{
public:
    
    //Label::createWithSystemFontと同じ引数でLabel::createWithTTFと同じ状態のLabelを生成する
    static Label* createWithTTFBySystemFontArgument(string str, string font_name, float text_point) ;
    
};


#endif /* defined(__war__HMSFontUtil__) */
