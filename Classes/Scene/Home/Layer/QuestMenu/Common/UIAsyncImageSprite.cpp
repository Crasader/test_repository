#include "UIAsyncImageSprite.h"

UIAsyncImageSprite::UIAsyncImageSprite(){
    
}
UIAsyncImageSprite::~UIAsyncImageSprite(){
    
}
UIAsyncImageSprite* UIAsyncImageSprite::create(const char *url, Vec2 imageSize){

    UIAsyncImageSprite *node = new UIAsyncImageSprite();
    node->init(url, imageSize);
    node->autorelease();
    return node;
}

UIAsyncImageSprite* UIAsyncImageSprite::init(const char *url, Vec2 imageSize){

    Sprite::init();
    
    _imageSize = imageSize ;
        
    httpRequest(url) ;
    
    return this;
}

void UIAsyncImageSprite::setTag(int tag){
    Node::setTag(tag) ;
}

void UIAsyncImageSprite::httpRequest(const char* url){
    HttpRequest *request = new HttpRequest();
    request->setUrl(url);
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(UIAsyncImageSprite::onHttpRequestCompleted, this)); // コールバック
    HttpClient::getInstance()->send(request);
    request->release();
}

void UIAsyncImageSprite::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
//    HttpRequest *request= response->getHttpRequest();
//    HMSLOG("http request completed");
//    HMSLOG("url:%s", request->getUrl());
//    HMSLOG("tag:%s", request->getTag());
//    HMSLOG("status:%ld", response->getResponseCode());
    
    ratio = getVisibleRatio();
    
    if (!response->isSucceed()){
        HMSLOG("画像データ取得失敗 %ld %s",response->getResponseCode(),response->getErrorBuffer());
//        Image* image = new Image();
//        image->initWithImageFile("download/Event/banner.png");
//        
//        Texture2D *texture = new Texture2D();
//        texture->initWithImage(image);
//        this->initWithTexture(texture);
//        
//        if (_imageSize != Vec2(0, 0))
//        {        
//            //画像のサイズを_imageSizeの大きさに統一する処理
//            Size imgSize = this->getContentSize();
//            this->setScaleX((_imageSize.x / imgSize.width) * ratio);
//            this->setScaleY((_imageSize.y / imgSize.height) * ratio);
//        }
        return;
    }
    
    std::vector<char> *buffer = response->getResponseData();
    
    Image *image = new Image();
    image->initWithImageData(reinterpret_cast<unsigned char*>(&(buffer->front())), buffer->size());
    
    Texture2D *texture = new Texture2D();
    texture->initWithImage(image);
    this->initWithTexture(texture);
    
    if (_imageSize != Vec2(0, 0))
    {
        //画像のサイズを_imageSizeの大きさに統一する処理
        Size imgSize = this->getContentSize();
        
        this->setScaleX((_imageSize.x / imgSize.width) * ratio);
        this->setScaleY((_imageSize.y / imgSize.height) * ratio);
    }
    
    image->release();
    texture->release();
}