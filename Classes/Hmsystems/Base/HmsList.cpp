#include "HmsList.h"
#include "HmsUtilityFunction.h"

//コンストラクタ
HmsList::HmsList()
{
    delimiter_columns = "/t";
    delimiter_rows = "/n";
}

//区切り文字を設定する
void HmsList::setDelimiter(string str_for_columns, string str_for_rows)
{
    delimiter_columns = str_for_columns;
    delimiter_rows = str_for_rows;
}

//TSVをhashのリストとして取り込む
void HmsList::import_tsv(string tsv_text)
{
    HmsVector<string> row_list = split(tsv_text, delimiter_rows);
    int count_rows = row_list.size();
    HmsVector<string> names_column;
    for(int i=0; i<count_rows; i++) {
        string row = row_list.at(i);
        if(row=="") continue;
        HmsVector<string> column_list = split(row, delimiter_columns);
        int count_columns = column_list.size();
        map<string, string> hash;
        for(int j=0; j<count_columns; j++) {
            string column = column_list.at(j);
            if(i==0) {//最初の行はからカラム名の一覧
                names_column.insert(column);
                vec_column.push_back(column);
            } else {//2行目以降はデータ
                hash.insert(map<string, string>::value_type(names_column.at(j), column));
                //HMSLOG("insert hash [%s]->[%s]", names_column.at(j).c_str(), column.c_str());
            }
        }
        if(i>0) {
            vec.insert(hash);
        }
    }
}

//hashのリストをTSV形式にて出力する
string HmsList::export_tsv(void)
{
    string tsv_text = "";
    int count = vec.size();
    if(count==0) return tsv_text;
    
    string delim = "";
    for(int i=0; i<vec_column.size(); i++) {
        string column_name = vec_column.at(i);
        tsv_text += delim + column_name;
        delim = delimiter_columns;
    }
    tsv_text += delimiter_rows;
    
    for(int i=0; i<count; i++) {
        map<string, string> hash = vec.at(i);
        delim = "";
        for(int j=0; j<vec_column.size(); j++) {
            string column_name = vec_column.at(j);
            map<string, string>::iterator it = hash.find(column_name);
            if(it!=hash.end()) {
                tsv_text += delim + it->second;
                delim = delimiter_columns;
            }
        }
        tsv_text += delimiter_rows;
    }
    return tsv_text;
}

//リストのサイズを取得する
int HmsList::size()
{
    return vec.size();
}

//リストの特定のデータを取得する
string HmsList::getValue(int index, string key)
{
    string value = "";
    if(index<0||index>=size()) return value;
    
    map<string, string> hash = vec.at(index);
    map<string, string>::iterator it = hash.find(key);
    if(it!=hash.end()) {
        value = it->second;
    }
    return value;
}

//[]演算子（配列参照演算子）の対応
map<string, string> HmsList::operator[](int index)
{
    if(index<0||index>=size()) {
        map<string, string> hash;
        hash.insert(map<string, string>::value_type("", ""));
        return hash;
    } else {
        return vec.at(index);
    }
}

//リストの中身をログに出力する
void HmsList::print()
{
    int count = vec.size();
    for(int i=0; i<count; i++) {
        map<string, string> hash = vec.at(i);
        HMSLOG("[%d] =>", i);
        
        for(int j=0; j<vec_column.size(); j++) {
            string column_name = vec_column.at(j);
            map<string, string>::iterator it = hash.find(column_name);
            if(it!=hash.end()) {
                string value = it->second;
                HMSLOG("    [%s] => [%s]", column_name.c_str(), value.c_str());
            }
        }
    }
}
