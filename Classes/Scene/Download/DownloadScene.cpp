#include "DownloadScene.h"

Scene* DownloadScene::scene()
{
    Scene *scene = Scene::create();
    DownloadScene *layer = DownloadScene::create();
    scene->addChild(layer);
    return scene;
    
}

bool DownloadScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    return true;
}


//セットアップ
void DownloadScene::setup()
{
    
    touchON() ;
    
    //ストレージのパスを取得する
    save_path = DownloadScene::getDownloadSavePathAndMkdir();
    
    HMSLOG("save_path=%s(Downloadscene::init)", save_path.c_str());
    
    download_size[0] = 0;
    download_size[1] = 0;
    
    //画像一覧のテキストをサーバから取得するリクエスト
    imageFileListRequest();
}

//別スレッドから呼び出される関数
//サーバーとローカルのファイルの差異を確認する
static void *checkDownloadList(void *data) {
    //void *からint配列に変換
    int *download_size = (int *)data;
    
    //画像の一覧をimage_list.txtから取得
    string save_path_use = DownloadScene::getDownloadSavePathAndMkdir();
    string image_list_filename = save_path_use + "image_list.txt";
    string tsv;
    ifstream ifs(image_list_filename.c_str());
    ifs >> tsv;
    
    HMSLOG("tsv=%s", tsv.c_str());
    HmsVector<string> str_array = split(tsv, "@@");
    //画像の一覧をHmsListへ変換
    HmsList image_list;
    image_list.setDelimiter(",", "@");
    image_list.import_tsv(str_array.at(0));
    //image_list.print();
    
    string download_host = str_array.at(3);
    
    //ローカルとサーバでダウンロードの画像サイズに差異が無いかチェック
    //差異があるものをダウンロードリストに加えるとともに、必要なダウンロードサイズを足していく
    for(int i=0; i<image_list.size(); i++) {
        string filename = image_list.getValue(i, "filename");
        string folder = image_list.getValue(i, "folder");
        string image_filename = save_path_use + folder + filename;
        unsigned long image_size = DownloadScene::getFileSize(image_filename.c_str());
        unsigned long size = fromString<unsigned long>(image_list[i]["size"]);
        //HMSLOG("folder=%s,filename=%s", folder.c_str(), filename.c_str());
        if(image_size!=size){
            HMSLOG("####no_mach_filename:%s local:%ld sever:%ld",filename.c_str(), image_size, size);
            download_size[1] += size;
        }
    }
    
    download_size[2] = 1;
    
    return nullptr;
}


//別スレッドから呼び出される関数
//画像一覧のうち端末にないものを保存する
static void *getImageFiles(void *data)
{
    //void *からint配列に変換
    int *download_size = (int *)data;
    
    //画像の一覧をimage_list.txtから取得
    string save_path_use = DownloadScene::getDownloadSavePathAndMkdir();
    string image_list_filename = save_path_use + "image_list.txt";
    string tsv;
    ifstream ifs(image_list_filename.c_str());
    ifs >> tsv;
    
    CCLOG("tsv=%s", tsv.c_str());
    HmsVector<string> str_array = split(tsv, "@@");
    //画像の一覧をHmsListへ変換
    HmsList image_list;
    
    image_list.setDelimiter(",", "@");
    image_list.import_tsv(str_array.at(0));
    //image_list.print();
    
    string download_host = str_array.at(3);
    
    //画像をダウンロード
    for(int i=0; i<image_list.size(); i++) {
        string filename = image_list[i]["filename"];
        string folder = image_list[i]["folder"];
        
        unsigned long size = fromString<unsigned long>(image_list[i]["size"]);
        
        //HMSLOG("filename=%s, size=%lu", filename.c_str(), size);
        string image_filename = save_path_use + folder + filename;
        //HMSLOG("image_filename=%s", image_filename.c_str());
        
        unsigned long image_size = DownloadScene::getFileSize(image_filename.c_str());
        
        if(size!=image_size) {
            //HMSLOG("download start");
            
            //まずはファイルのある場所のディレクトリが存在するかを確認し、なければディレクトリを作成する
            HmsVector<string> folder_str = split(folder, "/");
            string folder_path = save_path_use;
            for(int j=0;j<folder_str.size()-1;j++){
                folder_path += folder_str.at(j);
                if(access(folder_path.c_str(), 0)!=0){
                    mkdir(folder_path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                    CCLOG("####mkdir(%s)---(getImageFiles)", folder_path.c_str());
                    //iphoneであれば作成したディレクトリをiCloudのバックアップ対象から外す設定をする
                    if(IF_IOS){
                        AppHomeManager::iCloudTakeOff(folder_path);
                    }
                }
                folder_path += "/";
            }
            
            FILE *fp = fopen(image_filename.c_str(), "wb");
            string url = download_host;
            url += folder +filename;
            CCLOG("download start. url=%s", url.c_str());
            CURL *curl;
            curl = curl_easy_init();//ハンドラの初期化
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); //接続するサーバのURLを指定
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);//データ読み込み時(バッファの書き込み時)に呼ばれる関数を指定
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);//WRITEFUNCTIONで呼ばれる関数に渡す、引数を設定
            curl_easy_perform(curl);//実行
            curl_easy_cleanup(curl);
            fclose(fp);
            
            //ダウンロードの終わったサイズ分更新
            download_size[0] += fromString<int>(image_list[i]["size"]);
        } else {
            CCLOG("download passed");
        }
    }
    
    //ダウンロードが終わっている場合一応終わりましたということで変数にダウンロード完了後のファイルサイズを設定する
    download_size[0] = download_size[1];
    
    return nullptr ;
}


//画像一覧のテキストをサーバから取得するリクエスト
void DownloadScene::imageFileListRequest()
{
    //HTTP connection
    string url = HOST;
    //url += "download/dl_list.php?";
    url += "download/dl_list.php?";
    
    HMSLOG("url=%s", url.c_str());
    
    //imageFileListCallBack関数をコールバックに指定する
    auto selector = httpresponse_selector(DownloadScene::imageFileListCallback);
    //auto http_connect = HttpConnect::create() ;
    HttpConnect::sendHttpRequest(this, url, selector);
}

//画像一覧のテキストをサーバから取得するコールバック
void DownloadScene::imageFileListCallback(HttpClient *sender, HttpResponse *data)
{
    //結果を取得
    string response= HttpConnect::getHttpResponse(sender, data);
    //通信エラー以外
    if(response=="not connect"||response=="response NG") return;
    
    HMSLOG("response:%s", response.c_str());
    
    //画像一覧テキストを保存
    saveImageFileList(response) ;
    
    //「データチェック中です」のポップアップ表示
    commentSet();
    download_size[2] = 0;
    this->schedule(schedule_selector(DownloadScene::updateCheck), 1.0f);
    pthread_t s_thread;
    pthread_create(&s_thread, NULL, checkDownloadList, download_size);
    pthread_detach(s_thread);
}


void DownloadScene::updateCheck(float dt)
{
    if(download_size[2]==1) {
        download_size[2] = 2;
        startDownload();
    } else {
        ////ポップアップ枠////
        Sprite *sprite = (Sprite *)this->getChildByTag(DOWNLOAD_COMMENT_TAG);
        
        //コメント
        comment_count++;
        comment_count %= 5;
        
        string text = "データチェック中";
        for(int i=0; i<comment_count; i++) {
            text += ".";
        }
        Label *label = HMSFontUtil::createWithTTFBySystemFontArgument(text.c_str(), FONT_NAME_SYSTEM, 24);
        label->setPosition(Point(190, 75));
        label->setAnchorPoint(Point(0, 0.5));
//enableOutline(Color4B::BLACK,3);
   			label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
        sprite->removeAllChildren();
        sprite->addChild(label);
    }
}

void DownloadScene::startDownload()
{
    this->unschedule(schedule_selector(DownloadScene::updateCheck));
    
    //ここで「ファイルチェック中」のテキストを消す
    commentDelete();
    
    if(download_size[1]>0){//ダウンロードサイズが0より大きい場合、ダウンロードを開始する
        this->schedule(schedule_selector(DownloadScene::updateProgress), 1.0f);
        pthread_t s_thread;
        pthread_create(&s_thread, NULL, getImageFiles, download_size);
        pthread_detach(s_thread);
    } else{//ダウンロードサイズが0の場合、タイトルシーンのタップをOKにする0
//        TitleScene *title_scene = (TitleScene*)this->getParent();
//        title_scene->touchStart();
//        removeFromParentAndCleanup(true);
        deleteDownloadscene(0);
    }
}

//画像一覧テキストを保存
void DownloadScene::saveImageFileList(string response)
{
    //通信エラー以外
    if(response != "not connect" && response != "response NG") {
        HmsVector<string> str_array = split(response, "@@");

        //画像一覧と合計ファイルサイズを保存
        image_list.setDelimiter(",", "@");
        image_list.import_tsv(str_array.at(0));
        //image_list.print();
        
        download_host = str_array.at(3);

        HMSLOG("image_list size=%d", image_list.size());
        HMSLOG("download_host=%s", download_host.c_str());
        
        //画像の一覧をimage_list.txtに保存
        image_list_filename = save_path + "image_list.txt";
        HMSLOG("image_list_filename=%s", image_list_filename.c_str());
        
        FILE *fp = fopen(image_list_filename.c_str(), "w");
        if(fp==NULL) {
            HMSLOG("file cannot open");
        } else {
            fputs(response.c_str(), fp);
            fclose(fp);
        }
        
    }
}

//ダウンロード状況を表示
void DownloadScene::updateProgress(float dt)
{
    float ratio = getVisibleRatio();
    
    int sprite_tag_id = 10001;
    if(this->getChildByTag(sprite_tag_id)==NULL) {
        string filename = "init/Home/Popup/popup_200.png";
        Sprite *sprite = Sprite::create(filename.c_str());
        sprite->setScale(ratio);
        sprite->setScaleY(ratio*1.2);
        sprite->setPosition(convertFrom640x960(Point(320, 460)));
        this->addChild(sprite, sprite_tag_id, sprite_tag_id);
    }
    
    int bar_back_tag_id = 10003;
    if(this->getChildByTag(bar_back_tag_id)==NULL){
        string filename = "init/Download/Design/progress_bar.png";
        Sprite *sprite_back = Sprite::create(filename.c_str());
        sprite_back->setScale(ratio);
        sprite_back->setPosition(convertFrom640x960(Point(320, 480)));
        this->addChild(sprite_back, bar_back_tag_id, bar_back_tag_id);
    }
    
    
    int text_tag_id1 = 10002;
    string text1;
    float f_percentage =(float)download_size[0]*100.0f/(float)download_size[1];
    percentage = (float)download_size[0]*100.0f/(float)download_size[1];
    if(download_size[0]>=download_size[1]) {
        f_percentage = 100 ;
        percentage = 100;
    }
    HMSLOG("####now=%d,max=%d,percent:%d", download_size[0], download_size[1], percentage);
    
    text1 = "データダウンロード中です: " + StringUtils::format("%.2f", f_percentage) + "%" ;
    Label *label1= HMSFontUtil::createWithTTFBySystemFontArgument(text1.c_str(), FONT_NAME_SYSTEM, 24*ratio);
    label1->setVerticalAlignment(TextVAlignment::CENTER) ;
    label1->setPosition(convertFrom640x960(Point(320,520)));
//enableOutline(Color4B(0, 0, 0, 255), 3);
  	label1->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    if(this->getChildByTag(text_tag_id1)!=NULL) this->removeChildByTag(text_tag_id1);
    this->addChild(label1, text_tag_id1, text_tag_id1);
    
    
    //string text_comment = "※通信環境の良いところで通信を行ってください。\n通信状況により時間が掛かることがあります。";
    //LabelTTF *comment = LabelTTF::create("※通信環境の良いところで通信を行ってください。\n目安時間  Wi-Fi/LTE:10分 3G:50分", FONT_NAME_SYSTEM, 22*ratio);
    Label *comment = HMSFontUtil::createWithTTFBySystemFontArgument("※通信環境の良いところで通信を行ってください。", FONT_NAME_SYSTEM, 18*ratio);
    comment->setPosition(convertFrom640x960(Point(70,460)));
    comment->setAnchorPoint(Point(0, 1));
//enableOutline(Color4B(0, 0, 0, 255), 3);
    comment->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    comment->setColor(Color3B(255,255,102));
    if(this->getChildByTag(10006)!=NULL) this->removeChildByTag(10006);
    this->addChild(comment, 10006, 10006);
    
    string data_str = "";
    if(IF_ANDROID==1) data_str += "※端末のデータ容量に空きがない場合ダウンロードが\n　強制終了する場合があります。\n　空き容量は「設定 → ストレージ」から確認できます。";
    else data_str += "※端末のデータ容量に空きがない場合ダウンロードが\n　強制終了する場合があります。\n　空き容量は「設定 → 一般 → 使用状況」から確認できます。";
    Label *data_ttf = HMSFontUtil::createWithTTFBySystemFontArgument(data_str.c_str(), FONT_NAME_SYSTEM, 18*ratio);
    data_ttf->setPosition(convertFrom640x960(Point(70, 440)));
    data_ttf->setAnchorPoint(Point(0, 1));
    data_ttf->setHorizontalAlignment(TextHAlignment(0));
//enableOutline(Color4B(0, 0, 0, 255), 3);
		data_ttf->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    data_ttf->setColor(Color3B(255, 255, 102));
    if(this->getChildByTag(10007)!=NULL) this->removeChildByTag(10007);
    this->addChild(data_ttf, 10007, 10007);
    
    //ダウンロード状態を示すバーを更新
    Layer *get_load_layer = (Layer *)this->getChildByTag(bar_back_tag_id);
    if(get_load_layer==NULL) return;
    
    float bar_percentage = (float)percentage / 100;
    string filename;
    Sprite *sprite = (Sprite *)get_load_layer->getChildByTag(10004);
    if(sprite==NULL) {
        filename = "init/Download/Design/progress_empty_bar.png";
        sprite = Sprite::create(filename.c_str());
        sprite->setAnchorPoint(Point(0, 0.5));
        sprite->setPosition(convertFrom640x960(Point(70, 480)));
        this->addChild(sprite, 10004, 10004);
    }
    sprite->setScaleX(ratio*bar_percentage);
    
    if(percentage>=100) {
        this->unschedule(schedule_selector(DownloadScene::updateProgress));
        this->scheduleOnce(schedule_selector(DownloadScene::deleteDownloadscene),2.0f);
    }
}

void DownloadScene::deleteDownloadscene(float dt)
{
    //ダウンロードの終了時の処理の実行
    if(onCompleteDownload){
        onCompleteDownload() ;
    }

    //自分自身を削除
    removeFromParentAndCleanup(true);
}

//端末内でのダウンロード画像の保存先を取得する
string DownloadScene::getDownloadSavePathAndMkdir()
{
    string save_path = "";

    if(IF_IOS==1) {
        //todo くにつくのgetAppHome.mmを参考にしてディレクトリを作る。iCloudのバックアップ対象とならないようにする
        save_path = Utility::getDownloadSavePath() ;
    } else if(IF_ANDROID==1) {
        //TODO:getSDCardDirnameを使うべきか後で調査
        //save_path = DownloadScene::getSDCardDirname() + "/"+ ANDROID_PACKAGE_NAME +"/Resources/";
        save_path = Utility::getDownloadSavePath() ;
        HMSLOG("save_path:%s", save_path.c_str()) ;
    }

    //Resourcesディレクトリが無ければ、ディレクトリを作成する
    if(access(save_path.c_str(), 0)!=0){
        mkdir(save_path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        HMSLOG("####mkdir(%s)---(getLocalSavePath)",save_path.c_str());
        //iphoneであれば作成したディレクトリをiCloudのバックアップ対象から外す設定をする
        if(IF_IOS){
            AppHomeManager::iCloudTakeOff(save_path);
        }
    }

    return save_path;
}

/*
//AndroidのSDカードのパスを取得する
string DownloadScene::getSDCardDirname()
{
    string path = ANDROID_PACKAGE_NAME;
    path = "/data/data/" + path + "/files/sdcard_path.txt";
    HMSLOG("save_path=%s(c++)", path.c_str());

    long nSize; //unsigned long nSizeから変更;
    char *buffer;
    buffer = (char *)FileUtils::getInstance()->getFileData(path, "r", &nSize);
    
    if(buffer[nSize]!=0) buffer[nSize] = 0;
    HMSLOG("sdcard_path=%s(c++), size=%lu", buffer, nSize);
    string base_path = buffer;
    HMSLOG("sdcard_path=%s(c++)", base_path.c_str());
    
    return base_path;
}
 */

unsigned long DownloadScene::getFileSize(const char *fileName) {
	int fd = open(fileName, O_RDONLY);
    if(fd>0) {
        struct stat statBuf;
        fstat(fd, &statBuf);
        close(fd);
        return (unsigned long) statBuf.st_size;
    } else {
        close(fd);
        return 0;
    }
}

//「データチェック中です」のポップアップ表示
void DownloadScene::commentSet()
{
    ////ポップアップ枠////
    Sprite *sprite = Sprite::create("init/Home/Popup/popup_150.png");
    sprite->setPosition(convertFrom640x960(Point(320, 480)));
    sprite->setScale(getVisibleRatio());
    this->addChild(sprite, 1, DOWNLOAD_COMMENT_TAG);
    
    //コメント
    Label *label = HMSFontUtil::createWithTTFBySystemFontArgument("データチェック中", FONT_NAME_SYSTEM, 24);
    label->setPosition(Point(190, 75));
    label->setAnchorPoint(Point(0, 0.5));
//enableOutline(Color4B::BLACK,3);
		label->enableShadow(Color4B(0, 0, 0, 255), Size(2, -2), 2);
    sprite->addChild(label);
}

//「ファイルチェック中です」のコメントを削除する
void DownloadScene::commentDelete()
{
    removeChildByTag(DOWNLOAD_COMMENT_TAG, true);
}

//タップイベントON
void DownloadScene::touchON()
{
    
    //タップイベントの設定
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [](Touch *touch,Event *event){
        return true;
    };
    
    //タップイベントの登録
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}


//タップイベントOFF
void DownloadScene::touchOFF(){
    _eventDispatcher->removeEventListener(touchListener);
}
