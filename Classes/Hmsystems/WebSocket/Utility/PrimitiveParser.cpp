#include "PrimitiveParser.h"

#include "HmsConsoleUtil.h"
using namespace std;

const string PrimitiveParser::kDelimiter = ",";

int PrimitiveParser::asInt(string value){
    return stoi(value);
}

uint PrimitiveParser::asUint(string value){
    return stoi(value);

}

long PrimitiveParser::asLong(string value){
    return stol(value);

}

float PrimitiveParser::asFloat(string value){
    return stof(value);
}

bool PrimitiveParser::asBool(string value){
    return value == "1" ? true : false;
}

Vec2 PrimitiveParser::asVec2(string value){
    vector<string> splitValue = split(value, kDelimiter);
    //4S基準の位置で送られてくるので自身の画面サイズ基準に変換
    float myDisplaySizeY = Director::getInstance()->getVisibleSize().height;
    float y = (myDisplaySizeY / 960) * asFloat(splitValue[1]);
    return Vec2(asFloat(splitValue[0]), y);
}
MultiValue::ValueCommand PrimitiveParser::asValueCommand(string value){
    vector<std::string> splitValue = split(value, MultiValue::kValueDelimiterString);
    MultiValue::ValueCommand val;
    val.type = splitValue[0];
    val.value = splitValue[1];
    
    return val;
}

MultiValue* PrimitiveParser::asMultiValue(std::string value){
    vector<MultiValue::ValueCommand> values;
    vector<string> val = split(value, MultiValue::kDelimiterString);
    for_each(val.begin(), val.end(), [&values](string s){
        values.push_back(asValueCommand(s));
    });
    MultiValue* multiValue = MultiValue::create(values);
    
    return multiValue;
}

string PrimitiveParser::convertToCommand(int value){
    return to_string(value);
}
string PrimitiveParser::convertToCommand(uint value){
    return to_string(value);
}
string PrimitiveParser::convertToCommand(long value){
    return to_string(value);
}
string PrimitiveParser::convertToCommand(float value){
    return to_string(value);
}
string PrimitiveParser::convertToCommand(bool value){
    return value == true ? "1" : "0";
}
string PrimitiveParser::convertToCommand(Vec2 value){
    //y座標を4Sの画面サイズ基準に変換
    float myDisplaySizeY = Director::getInstance()->getVisibleSize().height;
    float y = (960 / myDisplaySizeY) * value.y;
    return to_string(value.x) + kDelimiter + to_string(y);
}

string PrimitiveParser::convertToCommand(MultiValue::ValueCommand value){
    return value.type + MultiValue::kValueDelimiterString + value.value;
}

string PrimitiveParser::convertToCommand(MultiValue *value){
    if(value == nullptr) return "error Value is null";
    string result;
    vector<MultiValue::ValueCommand> values = value->getValues();
    int index = 0;
    for_each(values.begin(), values.end(),
             [&values, &index, &result](MultiValue::ValueCommand val){
                 result += convertToCommand(val);
                 if(index != values.size()-1){
                     result += MultiValue::kDelimiterString;
                 }
                 index++;
    });
    return result;
}

vector<string> PrimitiveParser::split(const string &str, const string &delim){
    vector<string> res;
    size_t current = 0, found, delimlen = delim.size();
    while((found = str.find(delim, current)) != string::npos){
        res.push_back(string(str, current, found - current));
        current = found + delimlen;
    }
    res.push_back(string(str, current, str.size() - current));
    return res;
}

//vector<string> PrimitiveParser::split(const string& s, const string& delimiter){
//    vector<string> result;
//    string::size_type pos = 0;
//    while (pos != string::npos) {
//        string::size_type p = s.find(delimiter, pos);
//        if(p == string::npos){
//            result.push_back(s.substr(pos));
//            break;
//        }else{
//            result.push_back(s.substr(pos, p - pos));
//        }
//        
//        pos = p = + delimiter.size();
//    }
//    return result;
//}