#ifndef __War__HttpConnect__
#define __War__HttpConnect__

#include "cocos2d.h"
#include "HomeCommon.h"
#include "HttpClient.h"
#include "LoadingLayer.h"
#include "RetransmissionLayer.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::network;

class HttpConnect : public Ref
{
public:
    
    static bool _connect_flg; //通信中のフラグ
    
    //通信中フラグ
    bool transmitting;
    //再リクエスト用パラメータ
    Layer *obj;
    string url;
    string tag;
    SEL_HttpResponse selector;
    string body;
    HttpRequest *request;
    
    //タイムアウト用カウント
    int time_out_check_count;
    
    HttpConnect();
    virtual ~HttpConnect();
    
    //static int create();
    
    //送信初期処理
    //HttpConnect(Layer *obj, string url, SEL_HttpResponse selector);
    //HttpConnect(Layer *obj, string url, SEL_HttpResponse selector, string body);
    
    static string getHttpConnectId();
    static string addVersionAndOS(string str, string delim);
    
    //送信
    static void sendHttpRequest(Layer *obj, string url, SEL_HttpResponse selector);
    static void sendHttpRequest(Layer *obj, string url, SEL_HttpResponse selector, string body);
    //受信
    static string getHttpResponse(HttpClient *sender, HttpResponse *response);
    
    //ローディング画面を出すかを返す
    static bool LoadingFlgCheck(string url);
    
    //ローディング画面呼出
    static void LoadingLayerCall(Layer *layer);
    //ローディング画面終了
    static void LoadingLayerClose(Layer *layer);
    
    //タイムアウト
    void timeOutCheck(float dt);
    
    //再送信画面呼出
    static void HttpRetransmissionLayerCall(Scene *scene, string text);
    
    //再送信
    static void HttpRetransmission();
};

#endif /* defined(__War__HttpConnect__) */
