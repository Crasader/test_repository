#ifndef __DOWNLOAD_SCENE_H__
#define __DOWNLOAD_SCENE_H__

#include "cocos2d.h"
#include "common.h"
#include "HmsList.h"
#include "SaveData.h"
#include "HttpConnect.h"
#include "curl/curl.h"

#include "TitleScene.h"
#include "HmsList.h"
#include "HmsUtilityFunction.h"
#include "HmsTemplateFunction.h"
#include "HmsGeometricFunction.h"
#include "AppHomeManager.h"
#include <iostream>
#include <fstream>
#include "sys/stat.h"
#include "sys/types.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "NormalScene.h"
#include "Utility.h"


//tag_idを列挙
#define DOWNLOAD_COMMENT_TAG 12

USING_NS_CC;
using namespace std;

class DownloadScene : public Layer
{
public:
    //ダウンロードが完了した時の関数
    CC_SYNTHESIZE(function<void()>, onCompleteDownload, OnCompleteDownload);
    
    //端末内でのダウンロード画像の保存先を取得する
    static string getDownloadSavePathAndMkdir();
    
    //ファイルサイズを取得する
    static unsigned long getFileSize(const char *fileName);
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* scene();
    void setup();
    
    // implement the "static node()" method manually
    CREATE_FUNC(DownloadScene);


private:
    string download_host;
    int download_size[3];//ダウンロードサイズ、1番目が現在値、2番目が最大値、3番目はファイルチェック状態0開始1チェック中2終了
    string image_list_filename;//画像ファイル一覧テキストの保存ファイル名
    string save_path;//画像ファイルの保存先
    HmsList image_list; //取得した画像情報一覧
    int percentage; //ダウンロード完了した％
    string downloat_type;//ダウンロードのタイプ
    int value;
    int comment_count;
    //タップのリスナー
    EventListenerTouchOneByOne *touchListener;
    
    
    //その他
    void commentSet();//ダウンロードファイルのチェック中にコメントを表示する
    void commentDelete();//ダウンロードファイルのチェック中のコメントを削除する
    void deleteDownloadscene(float dt);//自分自身を削除
    void updateCheck(float dt);//ファイルチェック状況を表示
    void startDownload();
    void updateProgress(float dt);//ダウンロード状況を表示
    void imageFileListRequest(); //画像一覧のテキストをサーバから取得するリクエスト
    void imageFileListCallback(HttpClient *sender, HttpResponse *data);//画像一覧のテキストをサーバから取得する
    void saveImageFileList(string response);//画像一覧を保存する
    void userDefaultUpdate();//step、またはdungeon_idを更新する
    //static void *getImageFiles(void *data);//画像一覧のうち端末にないものを保存する
    static string getSDCardDirname();//AndroidのSDカードのパスを取得する
    //タップON OFF切り替え
    void touchON() ;
    void touchOFF() ;
    
};

#endif // __DOWNLOAD_SCENE_H__
