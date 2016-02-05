#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "json11.hpp"

//************************
//文字列関連
//************************

//文字列を特定の文字列で分割し、HmsVectorに格納して返す
HmsVector<string> split(const string str, const string delim)
{
    HmsVector<string> res;
    size_t current = 0;
    size_t found;
    size_t delimlen = delim.size();
    while((found=str.find(delim, current)) != string::npos) {
        string s = string(str, current, found - current);
        res.insert(s);
        current = found + delimlen;
    }
    string s = string(str, current, str.size() - current);
    res.insert(s);
    return res;
}
//vector版
vector<string> splitString(const string str, const string delim)
{
    vector<string> res;
    size_t current = 0;
    size_t found;
    size_t delimlen = delim.size();
    while((found=str.find(delim, current)) != string::npos) {
        string s = string(str, current, found - current);
        res.push_back(s);
        current = found + delimlen;
    }
    string s = string(str, current, str.size() - current);
    res.push_back(s);
    return res;
}

//UTF-8の文字列を一文字ずつ配列に格納
HmsVector<string> splitByUtf8(string str)
{
    HmsVector<string> str_vec;
    
    int pos;
    unsigned char lead;
    int char_size;
    
    for (pos = 0; pos < str.size(); pos += char_size) {
        
        lead = str[pos];
        
        if (lead < 0x80) {
            char_size = 1;
        } else if (lead < 0xE0) {
            char_size = 2;
        } else if (lead < 0xF0) {
            char_size = 3;
        } else {
            char_size = 4;
        }
        
        str_vec.push_back(str.substr(pos, char_size));
    }
    
    return str_vec;
}

//UTF-8の文字列の長さを返す
int calcStrLength(string str)
{
    int pos;
    unsigned char lead;
    int char_size;
    
    int count = 0;
    for (pos = 0; pos < str.size(); pos += char_size) {
        
        lead = str[pos];
        
        if (lead < 0x80) {
            char_size = 1;
        } else if (lead < 0xE0) {
            char_size = 2;
        } else if (lead < 0xF0) {
            char_size = 3;
        } else {
            char_size = 4;
        }
        
        count++;
    }
    
    return count;
}

//UTF-8の文字列の分解
string calcStrSub(string str, int num)
{
    string str2;
    int pos;
    unsigned char lead;
    int char_size;
    
    int number = 0;
    for (pos = 0; pos < str.size(); pos += char_size) {
        
        lead = str[pos];
        
        if (lead < 0x80) {
            char_size = 1;
        } else if (lead < 0xE0) {
            char_size = 2;
        } else if (lead < 0xF0) {
            char_size = 3;
        } else {
            char_size = 4;
        }
        
        str2 += str.substr(pos, char_size);
        
        number++;
        if (number >= num) break;
    }
    
    return str2;
}

//文字列中から特定の文字列を検索して別の文字列に変換する。
/*
* @param str  : 置換対象の文字列。上書かれます。
* @param from : 検索文字列
* @param to   : 置換後の文字列
*/
string strReplace (std::string& str, const std::string& from, const std::string& to) {
	std::string::size_type pos = 0;
	while(pos = str.find(from, pos), pos != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
	return str;
}

//************************
//座標関連
//************************

//2点間の距離を計算する
float calculateDistance(Point from, Point to)
{
    float diff_x = to.x - from.x;
    float diff_y = to.y - from.y;
    float distance = sqrtf(diff_x*diff_x + diff_y*diff_y);
    return distance;
}

//2点間の距離を計算する（2:1のhexマップの場合）
float calculateDistanceAtHexMap(Point from, Point to)
{
    float diff_x = to.x - from.x;
    float diff_y = (to.y - from.y)*2;
    float distance = sqrtf(diff_x*diff_x + diff_y*diff_y);
    return distance;
}

//************************
//日付関連
//************************

//1970年1月1日から現在時刻までの経過秒数を取得（秒単位）
time_t getNowTime()
{
    time_t now_time;
    time(&now_time);
    return now_time;
}

//1970年1月1日から現在時刻までの経過秒数を取得（ミリ秒単位）
double getNowTimeInMilli()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    double time = now.tv_sec + now.tv_usec/1000.0/1000.0;
    return time;

}

//現在の曜日を数字で取得（0日曜〜6土曜）
int getNowWeek()
{
    time_t now_time;
    time(&now_time);
    struct tm *timeinfo;
    timeinfo = localtime(&now_time);
    return timeinfo->tm_wday;
}

//time_t型の日付を文字列に変換する
string timeToString(time_t now_time)
{
    time(&now_time);
    char buffer[80];
    struct tm *timeinfo;
    timeinfo = localtime(&now_time);
    strftime(buffer,80,"%Y%m%d %T",timeinfo);
    string str = string(buffer);
    return str;
}

//************************
//json関連
//************************

int countJsonData(json11::Json json)
{
    //jsonの数をチェック
    int data_count = 0;
    while (1)
    {
        string num = toString<int>(data_count);
        
        if (!json[num].is_null())
        {
            data_count++;
        }
        else
        {
            break;
        }
    }
    return data_count;
}

//************************
//ネットワーク関連
//************************
string URLEncode(string input)
{
    const unsigned char chars[] = "0123456789ABCDEF";
    
    unsigned char *output = new unsigned char[input.size()*3 + 1 ];
    unsigned char *in = (unsigned char*)input.c_str();
    unsigned char* out = output;
    
    for( ; *in!='\0' ; in++ ){
        if ((*in >= 'A' && *in <= 'Z') || (*in >= 'a' && *in <= 'z') || (*in >= '0' && *in <= '9') ||
            *in == '=' || *in == '-' || *in == '_' || *in == '.' || *in == '~') {
            *out++ = (char)*in;
        } else {
            *out++ = '%';
            *out++ = chars[*in >> 4];
            *out++ = chars[*in & 0x0f];
        }
    }
    *out = '\0';
    
    std::string sout = (char*)output;
    delete output;
    
    return sout;
}
