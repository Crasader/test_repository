
#include "HttpConnectHms.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "TitleScene.h"
#include "PopupOk.h"

USING_NS_CC;

string HttpConnectHms::addVersionAndOS(string str, string delim)
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
void HttpConnectHms::sendHttpRequest(string url, std::function<void(HttpClient *, HttpResponse *)> func, string body)//一度のみのリクエスト送信の場合（ローディングなし）
{
    HMSLOG("HttpConnectHms....%s",url.c_str());
    
    //パラメータ付与
    body = addVersionAndOS(body, "&");
    
    HttpRequest *request= new HttpRequest();
    request->setUrl(url.c_str());
    request->setRequestType(HttpRequest::Type::POST);
    request->setRequestData(body.c_str(), body.length());
    request->setResponseCallback(func);
    auto client = HttpClient::getInstance();
    client->setTimeoutForConnect(5);
    client->setTimeoutForRead(8);//接続失敗までの時間
    client->send(request);
    
    request->release();
}

//受信
string HttpConnectHms::getHttpResponse(Layer *layer, HttpClient *cliant, HttpResponse *response)
{
    //受信情報
    vector<char> vec_char = *response->getResponseData();
    string result = string(vec_char.begin(),vec_char.end());
    
    //メンテナンス状態の表示 バージョンエラーの表示
    if(result=="maintenance" || result=="application_virsion_NG") {
        auto base_scene = Director::getInstance()->getRunningScene();
        auto now_scene = base_scene->getChildren().at(1);
        
        if (now_scene->getName() != "TitleScene") {//タイトルシーン以外だった場合、タイトルシーンへ遷移
            string message = "";
            if(result == "maintenance") message = "メンテナンス中です。タイトルに戻ります。";
            if(result == "application_virsion_NG") message = "アプリを最新のバージョンに更新してください。";
            
            auto popup = PopupOk::create();
            popup->setPopup(message,[&](){//ポップアップ出して再送信
                Director::getInstance()->replaceScene(TitleScene::createScene());
            });
            layer->addChild(popup,1000000);
            
        }
    }
    
    return result;
}
