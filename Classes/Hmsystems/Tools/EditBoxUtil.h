
#ifndef __war__EditBoxUtil__
#define __war__EditBoxUtil__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "common.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class EditBoxUtil{
public:
    
    //指定最大文字数を超過している場合falseを返す
    // text: 指定文字列
    // max_length : 最大文字数
    static bool isOverUtfLength(string text, int max_length) ;
    
    // 文字列の先頭から指定した文字列の分で削除する
    static string overSubstring(string text, int max_length) ;
    
    //無効な文字列を含むか判定
    static bool containInvalidChar(string text) ;
    
    //無効charの一覧を文字列で取得 delimiterを指定してそれらを区切る文字を指定
    static string getInvalidCharsString(string delimiter = " ") ;
private:
    
    
    template<class T> static bool inArray(T target , T array[]) ;
    static int utfLength(const char *cStr) ;
    static int charUTFByte(unsigned char cChar); //charのバイト数を調べる 
};


#endif /* defined(__war__EditBoxUtil__) */
