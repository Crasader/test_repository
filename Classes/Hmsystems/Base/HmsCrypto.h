#ifndef __HMS_CRYPTO_H__
#define __HMS_CRYPTO_H__ 

#include "cocos2d.h"
#include "common.h"
#include "HmsConsoleUtil.h"
#include "HmsUtilityFunction.h"
#include "HmsVector.h"
#include "CCCrypto.h"
#include "base/base64.h"

using namespace std;
USING_NS_CC;

class HmsCrypto{
public:
    
//sha1
static string sha1(string text);

//チェックサムを作成する
static string md5encode(string text) ;
    
//チェックサムを作成する (未使用)
static string makeCheckSum(string text);
static string makeCheckSum(string url_query, string key);
    
private :
static char* bin2hex(unsigned char* bin, int binLength) ;
};

#endif // __HMS_CRYPTO_H__ 