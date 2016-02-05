//
//  EditBoxUtil.cpp
//  war
//
//
//

#include "HMSFontUtil.h"


//第二引数は使わないでTTFフォントの生成に適している種類のパスを指定する
Label *HMSFontUtil::createWithTTFBySystemFontArgument(string text, string font_name, float text_point){
    auto label = Label::createWithSystemFont(text, font_name, text_point) ;
    label->enableOutline(Color4B(0, 0, 0, 255), 1 ) ;
    return label ;
}