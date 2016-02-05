#ifndef __war__MultiValue__
#define __war__MultiValue__

#include "cocos2d.h"

USING_NS_CC;

class MultiValue : public Node{
 public:
    struct ValueCommand{
     public:
        std::string type;
        std::string value;
    };
    
    static const std::string kDelimiterString;
    static const std::string kValueDelimiterString;
    
    static MultiValue* create();
    static MultiValue* create(std::vector<ValueCommand> value);
    virtual bool init();
    virtual bool init(std::vector<ValueCommand> value);
    
    void push(int value);
    void push(uint value);
    void push(long value);
    void push(float value);
    void push(bool value);
    void push(Vec2 value);
    void push(std::string value);
    ValueCommand pop();
    
    std::vector<ValueCommand> getValues() const;
private:
    std::string encodeString(std::string value);
    
    std::vector<ValueCommand> values;
    std::string replaceString(std::string value, std::string from, std::string to);
};

#endif /* defined(__war__MultiValue__) */
