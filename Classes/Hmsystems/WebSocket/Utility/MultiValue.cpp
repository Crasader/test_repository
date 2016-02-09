#include "MultiValue.h"
#include "PrimitiveParser.h"

using namespace std;

const string MultiValue::kValueDelimiterString = "<%VAL%>";
const string MultiValue::kDelimiterString = "<%MVAL%>";

MultiValue* MultiValue::create(){
    MultiValue *node = new MultiValue();
    
    if (node && node->init())
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool MultiValue::init(){
    return true;
}

MultiValue* MultiValue::create(vector<ValueCommand> value){
    MultiValue *node = new MultiValue();
    
    if (node && node->init(value))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool MultiValue::init(vector<ValueCommand> value){
    values = value;
    return true;
}

void MultiValue::push(int value){
    ValueCommand val;
    val.type = "int";
    val.value = PrimitiveParser::convertToCommand(value);
    values.push_back(val);
}

void MultiValue::push(uint value){
    ValueCommand val;
    val.type = "uint";
    val.value = PrimitiveParser::convertToCommand(value);
    values.push_back(val);
}

void MultiValue::push(long value){
    ValueCommand val;
    val.type = "long";
    val.value = PrimitiveParser::convertToCommand(value);
    values.push_back(val);
}

void MultiValue::push(float value){
    ValueCommand val;
    val.type = "float";
    val.value = PrimitiveParser::convertToCommand(value);
    values.push_back(val);
}

void MultiValue::push(bool value){
    ValueCommand val;
    val.type = "bool";
    val.value = PrimitiveParser::convertToCommand(value);
    values.push_back(val);
}

void MultiValue::push(Vec2 value){
    ValueCommand val;
    val.type = "Vec2";
    val.value = PrimitiveParser::convertToCommand(value);
    values.push_back(val);
}

void MultiValue::push(string value){
    ValueCommand val;
    val.type = "string";
    val.value = encodeString(value);
    values.push_back(val);
}

MultiValue::ValueCommand MultiValue::pop(){
    if(values.size() == 0){
        ValueCommand val;
        val.type = "invalid";
        val.value = "";
        return val;
    }
    ValueCommand val = values[0];
    values.erase(values.begin());
    return val;
}

string MultiValue::replaceString(std::string value, std::string from, std::string to){
    string::size_type pos = value.find(from);
    while (pos != string::npos) {
        value.replace(pos, from.size(), to);
        pos = value.find(from, pos + to.size());
    }
    return value;
}

string MultiValue::encodeString(string value){
    string result = replaceString(value, kValueDelimiterString, "");
    result = replaceString(result, kDelimiterString, "");
    return result;
}

vector<MultiValue::ValueCommand> MultiValue::getValues() const{
    return values;
}



