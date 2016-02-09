#ifndef __war__PrimitiveParser__
#define __war__PrimitiveParser__

#include "cocos2d.h"
#include "MultiValue.h"

USING_NS_CC;

class PrimitiveParser{
public:
    static const std::string kDelimiter;
    
    static int asInt(std::string value);
    static uint asUint(std::string value);
    static long asLong(std::string value);
    static float asFloat(std::string value);
    static bool asBool(std::string value);
    static Vec2 asVec2(std::string value);
    static MultiValue::ValueCommand asValueCommand(std::string value);
    static MultiValue* asMultiValue(std::string value);
    
    static std::string convertToCommand(int value);
    static std::string convertToCommand(uint value);
    static std::string convertToCommand(long value);
    static std::string convertToCommand(float value);
    static std::string convertToCommand(bool value);
    static std::string convertToCommand(Vec2 value);
    static std::string convertToCommand(MultiValue::ValueCommand value);
    static std::string convertToCommand(MultiValue* value);
private:
    static std::vector<std::string> split(const std::string &str,
                                          const std::string &delim);
};

#endif /* defined(__war__PrimitiveParser__) */
