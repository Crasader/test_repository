#ifndef __HMS_TEMPLATE_FUNCTION_H__
#define __HMS_TEMPLATE_FUNCTION_H__

#include "cocos2d.h"
#include "HmsVector.h"
#include <cxxabi.h>

using namespace std;
USING_NS_CC;

//引数で与えられた変数を文字列に変換する
template<class T> string toString(T t)
{
    string s;
    stringstream ss;
    ss.precision(100);
    
    ss << t;
    return ss.str();
}

//引数で与えられた文字列を、任意の型に変換する
template<class T> T fromString(string s)
{
    T t;
    istringstream ss(s);
    ss >> t;
    return t;
}

//引数で与えられたオブジェクトのクラス名を取得する
//template<class T> string getClassName(T t)
//{
//    int status;
//    const type_info & id_p = typeid(t);
//    char *str = abi::__cxa_demangle(id_p.name(), 0, 0, &status);
//    string class_name = string(str);
//    return class_name;
//}

#endif // __HMS_TEMPLATE_FUNCTION_H__
