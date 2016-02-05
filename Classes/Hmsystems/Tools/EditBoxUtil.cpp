//
//  EditBoxUtil.cpp
//  war
//
//
//

#include "EditBoxUtil.h"


bool EditBoxUtil::isOverUtfLength(string text, int max_length){
    
    //文字数をカウント
    int count = utfLength(text.c_str()) ;
    
    if(count>max_length) {
        return true;
    }else{
        return false;
    }
}

string EditBoxUtil::overSubstring(string target_str, int max_length){
    
    int count = 0;
    
    //文字列の単位のバイト数文ずらしていく
    int offset = 0 ;
    while(target_str[offset]){
        int byte = charUTFByte(target_str[offset]) ;
        offset+=byte;
        count++ ;
        
        //文字列の個数がmax_lengthに一致したときにbreakする
        if(count==max_length){
            break;
        }
    }
    
    return target_str.substr(0, offset) ;
}

//無効文字列の一覧
static char invalid_char[] = {':', '/', '\\', '?', '%', '*', '<', '>', '"', '|'};
static string invalid_string[] = {"　"};

string EditBoxUtil::getInvalidCharsString(string delimiter){
    string result_string ;
    bool first_roop = true ;
    for(int i=0; i<sizeof(invalid_char); i++) {
        if(!first_roop) result_string += delimiter ;
        
        result_string += invalid_char[i] ;
        first_roop = false;
    }
    return result_string ;
}

bool EditBoxUtil::containInvalidChar(string target_str){
    
    bool contain_invalid = false ;
    
    //文字列の単位のバイト数文ずらしていく
    int offset = 0 ;
    while(target_str[offset]){
        int byte = charUTFByte(target_str[offset]) ;
        
        if(byte>=2){
            //HMSLOG("1バイト以上の文字 byte:%d", byte) ;
            string check_target = target_str.substr(offset, byte) ;
            if(inArray<string>(check_target, invalid_string)){
                //HMSLOG("無効な文字列が含まれるためNG") ;
                contain_invalid = true;
                break;
            }
            offset+=byte;
        }else{
            if(isspace(target_str[offset])!=0){
                //HMSLOG("空白等を含むNG") ;
                contain_invalid = true;
            }else if(inArray<char>(target_str[offset], invalid_char)){
                //HMSLOG("無効な記号が含まれるNG") ;
                contain_invalid = true;
                break;
            }else{
                //HMSLOG("OK") ;
            }
            
            offset++ ;
        }
        
    }
    return contain_invalid ;
}

template<class T> bool EditBoxUtil::inArray(T target , T array[]){
    bool contain_array = false ;
    for(int i = 0 ; i<sizeof(array) ; i++){
        if(target == array[i]){
            contain_array = true;
            break;
        }
    }
    return contain_array  ;
}

int EditBoxUtil::utfLength(const char *buff)
{
    if( buff == NULL ) return 0;
    
    int count = 0;
    int pos = 0;
    int max_bytes = strlen( buff );
    
    // BOM 読み飛ばし
    if( max_bytes >= 3 )
    {
        if( static_cast<unsigned char>( buff[0] ) == 0xEF &&
           static_cast<unsigned char>( buff[1] ) == 0xBB &&
           static_cast<unsigned char>( buff[2] ) == 0xBF )
        {
            pos += 3;
        }
    }
    
    while( pos < max_bytes )
    {
        ++count; // 文字数カウント
        
        if( ( buff[pos] & 0x80 ) == 0 )
        {
            ++pos; // 1バイト文字
        }
        else
        {
            for( char tmp = buff[pos] & 0xfc; (tmp & 0x80); tmp = tmp << 1 )
            {
                ++pos; // 複数バイト文字
            }
        }
    }
    return count;
}

int EditBoxUtil::charUTFByte(unsigned char cChar)
{
    int iByte;
    
    if ((cChar >= 0x00) && (cChar <= 0x7f)) {
        iByte = 1;
    } else if ((cChar >= 0xc2) && (cChar <= 0xdf)) {
        iByte = 2;
    } else if ((cChar >= 0xe0) && (cChar <= 0xef)) {
        iByte = 3;
    } else if ((cChar >= 0xf0) && (cChar <= 0xf7)) {
        iByte = 4;
    } else if ((cChar >= 0xf8) && (cChar <= 0xfb)) {
        iByte = 5;
    } else if ((cChar >= 0xfc) && (cChar <= 0xfd)) {
        iByte = 6;
    } else {
        iByte = 0;
    }
    
    return iByte;
}