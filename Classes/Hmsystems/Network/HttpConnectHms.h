
#ifndef __war__HttpConnectHms__
#define __war__HttpConnectHms__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "HttpClient.h"
#include "LoadingLayer.h"
#include "RetransmissionLayer.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::network;

class HttpConnectHms : public Ref
{
public:
    
  
    
public:
    
    //static string getHttpConnectId();
    static string addVersionAndOS(string str, string delim);
    
    //送信
    //static void sendHttpRequest(Layer *obj, string url, SEL_HttpResponse selector);
    //static void sendHttpRequest(string url, std::function<void(HttpClient *, HttpResponse *)> func, string body = "");
    static void sendHttpRequest(string url, std::function<void(HttpClient *, HttpResponse *)> func, string body = "");
    //static void sendHttpRequestLoading(Layer *obj, string url, std::function<void(HttpClient *, HttpResponse *)> func, string body = "");
    
    //受信
    static string getHttpResponse(Layer *layer, HttpClient *cliant, HttpResponse *response);
    
    
    //タイムアウト
    //void timeOutCheck(float dt);
    
    //再送信画面呼出
    //static void HttpRetransmissionLayerCall(Scene *scene, string text);
    
};


#endif /* defined(__war__HttpConnectHms__) */
