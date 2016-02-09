#include "HmsCrypto.h"

string HmsCrypto::sha1(string text){
    SHA1 sha1;
    const char* c = text.c_str() ;
    sha1.addBytes((unsigned char*)c, strlen(c));
    unsigned char buffer[cocos2d::extension::CCCrypto::SHA1_BUFFER_LENGTH];
    sha1.getDigest(buffer, cocos2d::extension::CCCrypto::SHA1_BUFFER_LENGTH);
    return bin2hex(buffer, cocos2d::extension::CCCrypto::SHA1_BUFFER_LENGTH) ;
}

//md5エンコードをしてからbase64エンコードをした文字列を返す
string HmsCrypto::md5encode(string text){
    string key = "Adk5kQxP"; //←これ適当につけた、phpと合わせること
    string contents = key + text;
    
    int len = 100;
    len = cocos2d::extension::CCCrypto::MD5_BUFFER_LENGTH;
    unsigned char buffer[len];
    
    cocos2d::extension::CCCrypto::MD5((void *)contents.c_str(), contents.length(), buffer);
    //CCCrypto::sha1((unsigned char *)text, length_text, (unsigned char *)key, length_key, buffer, len);
    
    HMSLOG("buffer=%s", buffer);
    for(int i=0; i<len; i++) {
        HMSLOG("c[%d]=0x%x", i, buffer[i]);
    }
    
    unsigned char *decode1;
    cocos2d::base64Encode((unsigned char*)buffer, (unsigned int)len, (char **)&decode1);
    HMSLOG("decode1=%s", decode1);
    
    return string((char*)decode1) ;
}

//これ未使用↓
//
//urlのパラメータ部分を渡す
//(例)index.php?user_master_id=123&password=afohals&quest_id=1334
//のようなurlのとき
//makeCheckSum("user_master_id=123&password=afohals&quest_id=1334") ;
//ないし
//makeCheckSum("?user_master_id=123&password=afohals&quest_id=1334") ;
//という形式で呼び出してください
string HmsCrypto::makeCheckSum(string url_query){
    //?が含まれる場合削除する
    if (url_query.find("?")==0){
        url_query = url_query.substr(1) ;
    }
    
    HMSLOG("url_query :%s", url_query.c_str()) ;
    
    string key = "Adk5kQxP"; //←これ適当につけた、phpと合わせること
    string text = url_query ;
    string contents = key + text;
    
    int len = 100;
    len = cocos2d::extension::CCCrypto::MD5_BUFFER_LENGTH;
    unsigned char buffer[len];
    
    cocos2d::extension::CCCrypto::MD5((void *)contents.c_str(), contents.length(), buffer);
    
    HMSLOG("buffer=%s", buffer);
    for(int i=0; i<len; i++) {
        HMSLOG("c[%d]=0x%x", i, buffer[i]);
    }
    
    unsigned char *decode1;
    cocos2d::base64Encode((unsigned char*)buffer, (unsigned int)len, (char **)&decode1);
    HMSLOG("decode1=%s", decode1);
    
    std::string s_decode1((char*)decode1) ;
    
    HmsVector<string> url_array = split(url_query, "&");
    
    int count = url_array.size();
    string chk_params;
    bool first_roop = true;
    for(int i=0; i<count; i++) {
        if(!first_roop)chk_params+="," ;
        string param_name_and_value = url_array.at(i); //user_master_id=21345　の部分
        chk_params += param_name_and_value.substr(0, param_name_and_value.find("=")) ;
        first_roop = false ;
    }
    
    string result = "&chk_sum="+s_decode1+"&chk_param="+chk_params ;
    HMSLOG("test: %s", result.c_str()) ;
    
    return result ;
}

string HmsCrypto::makeCheckSum(string text, string key){
    HMSLOG("text:%s", text.c_str()) ;
    
    string contents = key + text;
    
    int len = 100;
    len = cocos2d::extension::CCCrypto::MD5_BUFFER_LENGTH;
    unsigned char buffer[len];
    
    cocos2d::extension::CCCrypto::MD5((void *)contents.c_str(), contents.length(), buffer);
    
    HMSLOG("buffer=%s", buffer);
    for(int i=0; i<len; i++) {
        HMSLOG("c[%d]=0x%x", i, buffer[i]);
    }
    
    unsigned char *decode1;
    cocos2d::base64Encode((unsigned char*)buffer, (unsigned int)len, (char **)&decode1);
    HMSLOG("decode1=%s", decode1);
    
    std::string s_decode1((char*)decode1) ;
    
    return s_decode1;
}

char* HmsCrypto::bin2hex(unsigned char* bin, int binLength)
{
    static const char* hextable = "0123456789abcdef";
    
    int hexLength = binLength * 2 + 1;
    char* hex = new char[hexLength];
    memset(hex, 0, sizeof(char) * hexLength);
    
    int ci = 0;
    for (int i = 0; i < binLength; ++i)
    {
        unsigned char c = bin[i];
        hex[ci++] = hextable[(c >> 4) & 0x0f];
        hex[ci++] = hextable[c & 0x0f];
    }
    
    return hex;
}
