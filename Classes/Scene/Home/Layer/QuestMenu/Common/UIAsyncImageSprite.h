#ifndef __war__UIAsyncImageSprite__
#define __war__UIAsyncImageSprite__

#include "cocos2d.h"
#include "NormalScene.h"

using namespace std;
using namespace cocos2d;

class UIAsyncImageSprite : public Sprite
{
public:
    float ratio;
    UIAsyncImageSprite();
    ~UIAsyncImageSprite();
    static UIAsyncImageSprite* create(const char* url, Vec2 imageSize = Vec2(0, 0));
    UIAsyncImageSprite* init(const char* url, Vec2 imageSize = Vec2(0, 0));
    void setTag(int tag);
    
protected:
    Vec2 _imageSize;
    
    void httpRequest(const char* url) ;
    void onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) ;
};
#endif /* defined(__war__UIAsyncImageSprite__) */
