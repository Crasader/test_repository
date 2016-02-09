#include "common.h"
#include "NumberSprite.h"
#include "OutlineSprite.h"
#include "HmsGeometricFunction.h"

map<string, vector<Texture2D *>> NumberSprite::number_sprite;


Texture2D* NumberSprite::getSprite(int num, string kind, float size){
    if(num>=10 || num<0) CC_ASSERT("エラー");
    
    TTFConfig ttfConfig(FONT_NAME,size);
    
    
    if(number_sprite.find(kind) == number_sprite.end()){//まだkindがない時
        vector<Texture2D *> temp;
        for(int i=0; i<10; i++){
            
            auto *spr = OutlineSprite::createWithLabelOneSprite(Label::createWithTTF(ttfConfig,to_string(i)),2.5f*getVisibleRatio());
            spr->retain();//これがないと・・・
            temp.push_back(spr);
        }
        number_sprite.insert(std::make_pair(kind,temp));
        
        
    }
    return number_sprite[kind].at(num);
    
}