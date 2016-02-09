#include "Utility.h"
#include "NormalScene.h"

//アプリ終了
void Utility::appliClose()
{
    Director::getInstance()->end();
}

//BGM音量設定
void Utility::setBGMVolume(int volume)
{
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.01*volume);
}
//効果音音量設定
void Utility::setEffectVolume(int volume)
{
    SimpleAudioEngine::getInstance()->setEffectsVolume(0.01*volume);
}


bool Utility::containsPoint(Node *node, const Vec2& visible_point)
{
    //絶対拡大率取得
    float absolute_scale = getCascadeAbsoluteScale(node) ;
    Point convertedSize = node->getContentSize() * absolute_scale ;
    
    Point convertedPos = node->convertToWorldSpace(Point::ZERO);
    Rect rect = Rect(convertedPos.x, convertedPos.y, convertedSize.x, convertedSize.y) ;
        
    if (rect.containsPoint(visible_point)){
        return true ;
    }else{
        return false;
    }
}

//絶対拡大率を取得
float Utility::getCascadeAbsoluteScale(Node *node){
    Node *parent = node->getParent() ;
    
    if(parent != nullptr){
        return node->getScale() * getCascadeAbsoluteScale(parent) ;
    }else{
        return node->getScale();
    }
    
}

//横幅(max_width)に合うFontSizeを計算する(横幅が十分に広ければfont_sizeと同じ数値を返す)
float Utility::calcFlexibleFontSize(string text, float font_size, float max_width){
    
    //文字を改行文字で分解して最大文字数を取得する
    HmsVector<string> vec_text = split(text.c_str(), "\n");
    
    int text_length = 0;
    for (int i = 0; i < vec_text.size(); i++)
    {
        int tmp = calcStrLength(vec_text[i]);
        if (text_length < tmp) text_length = tmp;
    }
    
    //横幅にサイズを合わせたときのテキストサイズ
    int width_match_text_point = max_width / text_length ;
//    int width_match_text_point = max_width / calcStrLength(text) ;
    
    //横幅が十分に広ければ引数と同じ値を返す
    if(width_match_text_point > font_size){
        return font_size;
    }else{
        return width_match_text_point;
    }
}

//文字列の置換を行う
string Utility::replaceStr(string target_string, string search_string, string replace_string)
{
    std::string::size_type  Pos( target_string.find( search_string ) );
    
    while( Pos != std::string::npos )
    {
        target_string.replace( Pos, search_string.length(), replace_string );
        Pos = target_string.find( search_string, Pos + replace_string.length() );
    }
    
    return target_string;
}

void Utility::_setCascadeOpacityEnabled(Node* parent, bool enabled)
{
    //子ノード全てに透過を適用させる処理
    parent->setCascadeOpacityEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeOpacityEnabled(parent->getChildren().at(i), enabled);
    }
    
}

void Utility::_setCascadeColorEnabled(Node* parent, bool enabled)
{
    //子ノード全てに透過を適用させる処理
    parent->setCascadeColorEnabled(enabled); //子ノードにも適用させる処理
    
    for (int i=0; i<parent->getChildrenCount(); i++)
    {
        _setCascadeColorEnabled(parent->getChildren().at(i), enabled);
    }
    
}

std::string createJsonString(string json_name, HmsVector<string> &vec_data)
{
    //Json文字列を作成する関数
    std::string jsonString = "{" + json_name + ":{";
    string cr = "\n";
    
    for (int i = 0; i < vec_data.size(); i++)
    {
        jsonString += to_string(i) + ":" + vec_data[i];
        if (i != vec_data.size() - 1) jsonString += ",";

    }
    jsonString += "}}";
    HMSLOG("jsonString = %s", jsonString.c_str());

    return jsonString;
}

void Utility::initDungeonData()
{
    //使用フレンドカードの情報を更新
    SaveData::saveString("helper_user_sprite_data", "");
    
    //ダンジョンデータの初期化
    SaveData::saveString("enemy_user_master_id", "0");
    SaveData::saveString("choice_friend_id", "0");
    SaveData::saveString("npc_battle_event_rank_list_id", "0");
    SaveData::saveString("quest_stage_id", "0");
    SaveData::saveString("ranking_event_stage_id", "0");
    SaveData::saveString("boss_event_stage_log_id", "0");
    SaveData::saveBool("battle_restart_flg", false);
    SaveData::saveInt("battle_list_id", 0);
    SaveData::saveString("battle_mode", "solo");
    SaveData::saveString("event_rank_level", "0");
    SaveData::saveString("online_battle_type", "");
    SaveData::saveString("result_log_id", "0");
    SaveData::saveInt("OnlineBattleSurrenderFlg", 0);
    SaveData::saveBool("battle_restart_flg", false);
    SaveData::saveString("ranking_event_stage_id", "0");
    SaveData::saveString("boss_event_battle_log_id", "0");
    SaveData::saveString("challenge_event_stage_id", "0");
    SaveData::saveString("floor_count", "0");
    
//    SaveData::saveString("party_no", "1");
    
    //クエストデータの初期化
    SaveData::saveString("quest_stage_data", "");
    SaveData::saveString("quest_explain_data", "");
    
    //イベントデータの初期化
    SaveData::saveString("event_id", "");
    SaveData::saveString("event_rank_data", "");
    SaveData::saveString("event_explain_data", "");
    
    //国士無双イベントのデータの初期化
    SaveData::saveString("ranking_event_id", "");
    SaveData::saveString("ranking_event_event_rank_data", "");
    SaveData::saveString("ranking_event_explain_data", "");
    
    //ボスイベントのデータの初期化
    SaveData::saveString("boss_event_id", "");
    SaveData::saveString("boss_event_event_rank_data", "");
    SaveData::saveString("boss_event_explain_data", "");
    
    //試練イベントのデータの初期化
    SaveData::saveString("challenge_event_id", "");
    SaveData::saveString("challenge_event_event_rank_data", "");
    SaveData::saveString("challenge_event_explain_data", "");
    
    //トーナメントイベントのデータの初期化
    SaveData::saveString("tournament_event_list_id", "0");
    SaveData::saveString("tournament_event_deck_no", "0");
    SaveData::saveString("tournament_event_entry", "0");
    SaveData::saveBool("tournament_replay_flg", false);
    NormalScene::_tournament_next_battle_time = 0;
    
    //Resultシーンで使用するフレンド登録のデータ初期化
    SaveData::saveBool("follow_flg", false);
    SaveData::saveString("choice_friend_id", "0");
    SaveData::saveString("choice_friend_level","");
    SaveData::saveString("choice_friend_rank_name","");
    SaveData::saveString("choice_friend_user_name","");
    SaveData::saveString("choice_friend_comment","");
    SaveData::saveString("choice_friend_card_image_id","") ;
    
    //再開処理の初期化
    SaveData::saveString("event_battle_result_log_id", "");
    SaveData::saveString("quest_battle_result_log_id", "");
    SaveData::saveString("ranking_event_stage_log_id", "");
    SaveData::saveString("challenge_event_stage_log_id", "");
    SaveData::saveInt("ranking_event_power", 0);//連戦時の保存士気
    SaveData::saveInt("ranking_event_hp", 0);//連戦時の保存HP
}

void Utility::setOnlineBattleData(int battle_list_id, string enemy_user_master_id, bool restart_flg)
{
    //オンラインバトルデータの設定
    SaveData::saveString("battle_mode", "vs");
    SaveData::saveInt("battle_list_id", battle_list_id);
    SaveData::saveString("enemy_user_master_id", enemy_user_master_id);
    SaveData::saveBool("battle_restart_flg", restart_flg);
    
    HMSLOG("battle_list_id = %d, enemy_user_master_id = %s, restart_flg = %s", battle_list_id, enemy_user_master_id.c_str(), (restart_flg ? "true" : "false"));
    
}

void Utility::createPosisionIndex(HmsVector<Point> &vec_position_index, int row_count, int column_count, int first_x, int first_y, int distance_x, int distance_y)
{
    //ポジションインデックスを作成する
    for (int i = 0; i < row_count; i++)
    {
        for (int j = 0; j < column_count; j++)
        {
            float x, y;
            x = first_x + (distance_x * j);
            y = first_y - (i * distance_y);
            
            vec_position_index.push_back(Point(x, y));
        }
    }
}

void Utility::openPopupAction(cocos2d::Node *node)
{
    //ポップアップを表示するアクション
    auto ratio = getVisibleRatio();
    
    auto children = node->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == Utility::T_Popup_Action)
        {
            auto scale = ScaleTo::create(0.1f, 1.0 * ratio);
            child->runAction(Sequence::create(DelayTime::create(0.1f), scale, nullptr));
        }
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_on.mp3");
}

void Utility::closePopupAction(cocos2d::Node *node, function<void()> _func)
{
    //ポップアップを閉じるアクション
    auto children = node->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == Utility::T_Popup_Action)
        {
            auto scale = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    auto func= CallFunc::create([node, _func](){
        
        //もしfunctionが与えられていたら実行してから閉じる
        if (_func != nullptr) _func();
        
        node->removeFromParent();
    });
    
    node->runAction(Sequence::create(DelayTime::create(0.2f), func, nullptr));
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_off.mp3");
}

void Utility::hidePopupAction(Node* node)
{
    //ポップアップを隠すアクション（消さない！）
    auto children = node->getChildren();
    
    for (auto child : children)
    {
        if (child->getTag() == Utility::T_Popup_Action)
        {
            auto scale = ScaleTo::create(0.1f, 0);
            child->runAction(scale);
        }
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("download/SE/popup_off.mp3");
}

void Utility::setFavorite(string chara_list_id)
{
    //お気に入り追加処理
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    if(favorite_character_list != "") favorite_character_list += "/";
    favorite_character_list += chara_list_id;
    
    SaveData::saveString("favorite_character_list", favorite_character_list);
    HMSLOG("favorite_character_list = %s", SaveData::loadString("favorite_character_list").c_str());
}

void Utility::removeFavorite(string chara_list_id)
{
    //お気に入り解除処理
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    
    //分解
    HmsVector<string> vec_favorite_character_list = split(favorite_character_list, "/");
    
    //新しいお気に入りリストの作成
    string new_list = "";
    for(int i = 0; i < vec_favorite_character_list.size(); i++) {
        
        if(vec_favorite_character_list.at(i) != chara_list_id)
        {
            if(new_list != "") new_list += "/";
            new_list += vec_favorite_character_list.at(i);
        }
    }
    
    //保存
    SaveData::saveString("favorite_character_list", new_list);
    HMSLOG("favorite_character_list = %s", SaveData::loadString("favorite_character_list").c_str());
}

bool Utility::checkFavorite(string chara_list_id)
{
    //お気に入りチェック
    bool check_flg = false;
    
    string favorite_character_list = SaveData::loadString("favorite_character_list");
    HmsVector<string> favorite_list = split(favorite_character_list, "/");
    
    auto found = find(favorite_list.begin(), favorite_list.end(), chara_list_id);
    if (found != favorite_list.end()) check_flg = true;
    
    return check_flg;
}

int Utility::_test_state = 0 ;
bool Utility::isTest(){
    if(_test_state == 0) {
        string host = HOST ;
        string service_host = SERVICE_HOST ;
        std::string::size_type pos = host.find(service_host);
        if(pos==std::string::npos){
            _test_state = 1; //開発と判定
        }else{
            _test_state = 2; //本番と判定
        }
    }
    
    if(_test_state == 1) return true ;
    
    return false;
}

time_t Utility::getNowTimeStampCheckServerTime()
{
    //サーバー時間との差を考慮した現在時刻のタイムスタンプを取得
    time_t now_time;
    time(&now_time);
    time_t now_timestamp = now_time - (time_t)SaveData::loadInt("server_diff_time");
    
    return now_timestamp;
}

string Utility::dbtimeFormat(string datetime, string format)
{
    struct tm tm;
    strptime(datetime.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1; //サマータイムフラグを無効（※この処理が無いと１時間ずれる）
    
    char str[100] ;
    strftime(str, sizeof(str), format.c_str(), &tm) ;
    return str ;
}


/*
引数に指定したパスを省略して使用できるようにする。
 param path: {プラットフォーム毎に異なるパス}
 (例)
      使用前:
      Sprite *sprite = Sprite::create({プラットフォーム毎に異なるパス}/download/Test/test.png);
      使用後:
      Sprite *sprite = Sprite::create(download/Test/test.png);
*/
void Utility::setSearchPath(string path)
{
    vector<string> searchPaths = FileUtils::getInstance()->getSearchPaths();
    vector<string>::iterator iter = searchPaths.begin();
    searchPaths.insert(iter, path);
    FileUtils::getInstance()->setSearchPaths(searchPaths);
}

string Utility::getDownloadSavePath()
{
    return FileUtils::getInstance()->getWritablePath() + "Resources/";
}


//void Utility::downloadReosurceIfNeeded(const std::string& filename)
//{
//    if(FileUtils::isFileExist(filename) == false){
//        //タイトルへ
//        auto scene = TitleScene::createScene();
//        auto transition = TransitionFade::create(3.0f, scene);
//        Director::getInstance()->replaceScene(transition);
//    }
//}