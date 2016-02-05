#ifndef __HMS_LIST_H__
#define __HMS_LIST_H__

#include "cocos2d.h"
#include "HmsVector.h"

using namespace std;
USING_NS_CC;

class HmsList {
protected:
    //内部のvector
    HmsVector<map<string, string> > vec;
    //カラムの順序を保存
    vector<string> vec_column;
    //各列の区切り文字
    string delimiter_columns;
    //各行の区切り文字
    string delimiter_rows;
public:
    //コンストラクタ
    HmsList();
    
    //デストラクタ
    ~HmsList() {}
    
    //区切り文字を設定する
    void setDelimiter(string str_for_columns, string str_for_rows);
    
    //TSVをhashのリストとして取り込む
    void import_tsv(string tsv_text);
    
    //hashのリストをTSV形式にて出力する
    string export_tsv(void);
    
    //リストのサイズを取得する
    int size();
    
    //リストの特定のデータを取得する
    string getValue(int index, string key);
    
    //[]演算子（配列参照演算子）の対応
    map<string, string> operator[](int index);
    
    //リストの中身をログに出力する
    void print();
};

#endif // __HMS_LIST_H__
