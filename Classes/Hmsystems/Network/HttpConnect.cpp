#include "HttpConnect.h"
//#include "LoadingLayer.h"
//#include "RetransmissionLayer.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
//#include "GameEndLayer.h"
//#include "Utility.h"
#include "TitleScene.h"

USING_NS_CC;

#define LOADING_LAYER_TAG_ID 901

bool HttpConnect::_connect_flg = false;

//再リクエスト用保持領域
static bool retry_flg = false;
static bool retry_transmitting = false;
static Layer *retry_obj = nullptr;
static string retry_url = "";
static string retry_tag = "";
static SEL_HttpResponse retry_selector = nullptr;
static string retry_body = "";

HttpConnect::HttpConnect()
{
    HttpClient *client = HttpClient::getInstance();
    client->setTimeoutForConnect(TIME_OUT_CHECK_COUNT*100);
    client->setTimeoutForRead(TIME_OUT_CHECK_COUNT*100);
}

HttpConnect::~HttpConnect()
{
    
}

string HttpConnect::getHttpConnectId()
{
    UserDefault *userDefault = UserDefault::getInstance();
    int http_connect_id = userDefault->getIntegerForKey("http_connect_id", 0);
    http_connect_id++;
    userDefault->setIntegerForKey("http_connect_id", http_connect_id);
    string tag = toString(http_connect_id);
    
    return tag;
}

string HttpConnect::addVersionAndOS(string str, string delim)
{
    //バージョン及びosの付加
    str += delim + "application_virsion=";
    if(IF_IOS==1) {
        str += APPLICATION_VIRSION_FOR_IOS;
        str += "&os=ios";
    } else {
        str += APPLICATION_VIRSION_FOR_ANDROID;
        str += "&os=android";
    }
    
    //ストア情報の付加
    if(IF_ANDROID && RAKUTEN_FLG){
        //楽天のときのみパラメータを付加する
        str += "&is_rakuten=1";
    }
    
    return str;
}

//送信
void HttpConnect::sendHttpRequest(Layer *obj, string url, SEL_HttpResponse selector)
{
    //ローディング画面呼出
    if(LoadingFlgCheck(url)){
        HttpConnect::LoadingLayerCall(obj);
    }
    
    //パラメータ付与
    string delim = "&";
    if (url.find("?")==string::npos) {
        delim = "?";
    }
    url = addVersionAndOS(url, delim);
    
    //通信中フラグON
    retry_flg = true;
    retry_transmitting = true;
    string tag = getHttpConnectId();
    retry_obj = obj;
    retry_url = url;
    retry_tag = tag;
    retry_selector = selector;
    retry_body = "";
    
    HttpRequest *request= new HttpRequest();
    request->setUrl(url.c_str());
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(obj, selector);
    request->setTag(tag.c_str());
    HttpClient::getInstance()->send(request);
    HMSLOG("http send request GET. tag_id=%s, url=%s", tag.c_str(), url.c_str());
    request->release();
}

//送信
void HttpConnect::sendHttpRequest(Layer *obj, string url, SEL_HttpResponse selector, string body)
{
    //ローディング画面呼出
    if(LoadingFlgCheck(url)){
        HttpConnect::LoadingLayerCall(obj);
    }
    
    //パラメータ付与
    body = addVersionAndOS(body, "&");
    
    //パラメータ保持
    retry_flg = true;
    retry_transmitting = true;
    string tag = getHttpConnectId();
    retry_obj = obj;
    retry_url = url;
    retry_tag = tag;
    retry_selector = selector;
    retry_body = body;
    
    HttpRequest *request= new HttpRequest();
    request->setUrl(url.c_str());
    request->setRequestType(HttpRequest::Type::POST);
    request->setRequestData(body.c_str(), body.length());
    request->setResponseCallback(obj, selector);
    request->setTag(tag.c_str());
    HttpClient::getInstance()->send(request);
    HMSLOG("http send request POST. tag=%s, url=%s, body=%s", tag.c_str(), url.c_str(), body.c_str());
    request->release();
}

//受信
string HttpConnect::getHttpResponse(HttpClient *sender, HttpResponse *response)
{
    //ローディング画面終了
    //HMSLOG("###retry_url:%s",retry_httpConnect->url.c_str());
    if(LoadingFlgCheck(retry_url)){
        HttpConnect::LoadingLayerClose(retry_obj);
    }
    
    //受信情報
    HttpRequest *request= response->getHttpRequest();
    if(!response->isSucceed()) {
        //HMSLOG("error buffer:%s", response->getErrorBuffer());
        ////通信エラー
        //再送信画面呼出
        HttpConnect::HttpRetransmissionLayerCall((Scene*)retry_obj, "通信エラーが発生しました。\n通信状態の良い場所で再送信してください。");
        return "not connect";
    }
    std::vector<char> *buffer = response->getResponseData();
    std::string str = std::string("");
    for(int i=0; i<buffer->size(); i++) {
        str.append(1, (*buffer)[i]);
    }
    
    //////////チェック//////////
    //HMSLOG("retry_url:%s", retry_httpConnect->url.c_str());
    //受信待ちの状態か
    if(retry_flg==true) {
        //端末の通信ID、受信待ちのリクエストの通信ID、受信した通信IDが全て同一&受信待ちのリクエストの通信状態がON
        UserDefault *userDefault = UserDefault::getInstance();
        int http_connect_id = userDefault->getIntegerForKey("http_connect_id", 0);
        if(toString(http_connect_id) == request->getTag() && retry_tag == request->getTag() && retry_transmitting == true)
        {
            ////////OK////////
            //リクエスト破棄
            retry_flg = false;
            
            //メンテナンス状態の表示 バージョンエラーの表示
            if(str=="maintenance" || str=="application_virsion_NG") {
                auto base_scene = Director::getInstance()->getRunningScene();
                auto now_scene = base_scene->getChildren().at(1);
                
                //HMSLOG("now scene %s",now_scene->getName().c_str());
                if (now_scene->getName() != "TitleScene") {
                    //タイトルシーン以外だった場合、タイトルシーンへ遷移
                    Director::getInstance()->replaceScene(TitleScene::createScene());
                }
            }
            
            return str;
        }
        else {
            ////////NG////////
            return "response NG";
        }
    }
    else {
        ////////NG////////
        return "response NG";
    }
}

//ローディング画面呼び出すかフラグ
bool HttpConnect::LoadingFlgCheck(string url)
{
    bool load_flg = true;
    
    //PHPを判別するためurlを「？」で区切る
    HmsVector<string> url_array = split(url, "?");
    //HMSLOG("###url[0]:%s",url_array.at(0).c_str());
    
    //ローディング画面を呼び出さないPHPか確認(呼び出したくないPHPを||で列挙)
    //if(url_array.at(0) == HOST"dungeon/get_dungeon_list.php" ||
    //   url_array.at(0) == HOST"battle/get_live_data_detail.php"){
    if(url_array.at(0) == HOST"battle/regist_arena.php" ||
       url_array.at(0) == HOST"battle/illegal.php" ||
       url_array.at(0) == HOST"battle/waitting_battle.php" ||
       url_array.at(0) == HOST"team/team_comment_refresh_revision.php" ||
       url_array.at(0) == HOST"team/team_comment_contribute.php" ||
       url_array.at(0).find("/map/", 0) != string::npos ||
       url_array.at(0) == HOST"event/tournament_event/waiting_tournament_battle.php"){
        load_flg = false;
    }
    
    return load_flg;
}

//ローディング画面呼出
void HttpConnect::LoadingLayerCall(Layer *layer)
{
    _connect_flg = true;
    
    auto scene = (Scene *)layer->getParent();
    if (scene) {
        auto loading_layer = LoadingLayer::create();
        loading_layer->setGlobalZOrder(2);
        scene->addChild(loading_layer, 999999, LOADING_LAYER_TAG_ID);
        
        //タイムアウトチェック開始
        loading_layer->schedule(schedule_selector(HttpConnect::timeOutCheck), 0.01, TIME_OUT_CHECK_COUNT, 0);
    }
}

//ローディング画面終了
void HttpConnect::LoadingLayerClose(Layer *layer)
{
    _connect_flg = false;
    
    auto scene = (Scene *)layer->getParent();
    auto loading_layer = (LoadingLayer *)scene->getChildByTag(LOADING_LAYER_TAG_ID);
    
    if(loading_layer && scene) {
        loading_layer->unschedule(schedule_selector(HttpConnect::timeOutCheck));
        scene->removeChildByTag(LOADING_LAYER_TAG_ID);
    }
    
    //タイムアウトチェック終了
    //this->unschedule(schedule_selector(HttpConnect::timeOutCheck));
    //layer->unschedule(schedule_selector(HttpConnect::timeOutCheck));
}

//タイムアウトチェック
void HttpConnect::timeOutCheck(float dt)
{
    time_out_check_count++;
    HMSLOG("time_out_check_count:%d", time_out_check_count);
    
    //タイムアウト&受信待ちの状態か
    if(time_out_check_count == TIME_OUT_CHECK_COUNT && retry_flg==true) {
        
        Scene *scene = (Scene*)retry_obj;
        
        //受信待ちのリクエストの通信状態をOFF
        retry_transmitting = false;
        
        //ローディング画面終了
        //HttpConnect::LoadingLayerClose(scene);
        scene->removeChildByTag(LOADING_LAYER_TAG_ID);
        
        //再送信画面呼出
        HttpConnect::HttpRetransmissionLayerCall(scene, "タイムアウトしました。再送信してください。");
    }
    
}

//再送信画面呼出
void HttpConnect::HttpRetransmissionLayerCall(Scene *scene, string text)
{
    RetransmissionLayer *layer = RetransmissionLayer::create(text);
    scene->addChild(layer, 999999, LOADING_LAYER_TAG_ID);
}

//再送信
void HttpConnect::HttpRetransmission()
{
    HMSLOG("retransmit url:%s", retry_url.c_str());
    HMSLOG("retransmit tag:%s", retry_tag.c_str());
    HMSLOG("retransmit body:%s", retry_body.c_str());
    if(retry_body=="") {
        HttpConnect::sendHttpRequest(retry_obj, retry_url, retry_selector);
    } else {
        HttpConnect::sendHttpRequest(retry_obj, retry_url, retry_selector, retry_body);
    }
}
