#ifndef __HMS_UTILITY_FUNCTION_H__
#define __HMS_UTILITY_FUNCTION_H__

#include "cocos2d.h"
#include "HmsVector.h"
#include "json11.hpp"

USING_NS_CC;
using namespace std;

//************************
//文字列関連
//************************

//文字列を特定の文字列で分割し、HmsVectorに格納して返す
HmsVector<string> split(const string str, const string delim);
vector<string> splitString(const string str, const string delim);

//UTF-8の文字列を一文字ずつ配列に格納
HmsVector<string> splitByUtf8(string str);

//文字列(UTF-8の日本語)の長さを返す
int calcStrLength(string str);

//文字列(UTF-8の日本語)の分解
string calcStrSub(string str, int num);

//文字列中から特定の文字列を検索して別の文字列に変換する。
string strReplace (std::string& str, const std::string& from, const std::string& to);

//************************
//座標関連
//************************
//2点間の距離を計算する
float calculateDistance(Point from, Point to);

//2点間の距離を計算する（2:1のhexマップの場合）
float calculateDistanceAtHexMap(Point from, Point to);

//************************
//日付関連
//************************

//1970年1月1日から現在時刻までの経過秒数を取得（秒単位）
time_t getNowTime();
//1970年1月1日から現在時刻までの経過秒数を取得（ミリ秒単位）
double getNowTimeInMilli();
//現在の曜日を数字で取得（0日曜〜6土曜）
int getNowWeek();
//time_t型の日付を文字列に変換する
string timeToString(time_t now_time);

//************************
//json関連
//************************

int countJsonData(json11::Json json);

//************************
//ネットワーク関連
//************************
string URLEncode(string input);

#endif // __HMS_UTILITY_FUNCTION_H__
