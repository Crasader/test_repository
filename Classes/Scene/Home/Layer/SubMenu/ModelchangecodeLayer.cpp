#include "ModelchangecodeLayer.h"

bool ModelchangecodeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    HMSLOG("ModelchangecodeLayer::init()");
    
    ratio = getVisibleRatio();
    
    return true;
}

void  ModelchangecodeLayer::onEnter()
{
    Layer::onEnter();
    
    setupImage();
}

void ModelchangecodeLayer::setupImage(){
    
    //ページタイトルの作成
    HeaderLayer::setupTitleBar("機種変更コード発行", "SubMenu", this);
    
    //機種変更コードを発行し、コードをaddChildする
    modelchangecodeCheckRequest();
}

//機種変更コード発行通信処理
void ModelchangecodeLayer::modelchangecodeCheckRequest(){
    
    //auto http_connect = HttpConnect::create();
    string url = HOST;
    url += HTTP_Modelchangecode_URL;
    auto selector = httpresponse_selector(ModelchangecodeLayer::modelchangecodeCallback);
    
    string body = "user_master_id=";
    body += user_master_id;//ユーザーマスターID(string型)
    HttpConnect::sendHttpRequest(this, url, selector, body);
}
//機種変更コード発行通信レスポンス
void ModelchangecodeLayer::modelchangecodeCallback(HttpClient *sender, HttpResponse *response){
    string result = HttpConnect::getHttpResponse(sender, response);
    //通信エラー以外
    if(result=="not connect"||result=="response NG") return;
    
    HMSLOG("result = %s", result.c_str());
    
    string err;
    Json json = Json::parse(result, err);
    
    string str_code = "";
    if(json["err"].string_value() == "1"){
        str_code = "あなたの機種変更コードは \n";
        str_code += json["code"].string_value();
        str_code += "\n です。\nこのコードを新しい携帯の\n「機種変更コード入力」で入力する事で\n機種変更が完了致します。";
    }else{
        str_code = json["next_time"].string_value();
    }
    auto label = HMSFontUtil::createWithTTFBySystemFontArgument(str_code, FONT_NAME, 30);
    label->setScale(ratio);
    label->setTag(MODELCHANGECODE_LABEL_TAG);
    label->setPosition(convertFromDisplaySize(Point(320, 480), 0.5, 0.5));
    this->addChild(label, 10,MODELCHANGECODE_LABEL_TAG);
}


void ModelchangecodeLayer::changePage(Ref* sender){
    
    auto normal_scene = (NormalScene *)this->getParent();
    normal_scene->changePage("SubMenu");
    HMSLOG("touch back!");
}