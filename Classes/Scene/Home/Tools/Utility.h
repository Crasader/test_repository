#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "HmsUtilityFunction.h"
#include "HMSFontUtil.h"
#include "HmsGeometricFunction.h"
#include "SaveData.h"
//#include "CCFileUtils.h"

USING_NS_CC;
using namespace std;
using namespace CocosDenshion;

class Utility : public Ref
{
public:
    
    //タグ
    enum Tag{
        T_Popup_Action = 2500,
    };
    
    //Zorder
    enum ZOrder{
        Z_Popup_Action = 2500,
    };
    
    //アプリ終了
    static void appliClose();
    
    //BGM音量設定
    static void setBGMVolume(int volume);
    
    //効果音音量設定
    static void setEffectVolume(int volume);
    
    /*
     指定PointがNodeの範囲に含まれているか調べる(第2引数のPointは、640*960のデザイン座標ではなく、端末毎で異なる実際の左下からのピクセル数を与えてください)
     */
    static bool containsPoint(Node *node, const Vec2& visible_point);
    
    //絶対拡大率を取得
    static float getCascadeAbsoluteScale(Node *node);
    
    /*
    横幅(max_width)からはみ出ないようにフォントサイズを計算する(横幅が十分に広ければfont_sizeと同じ数値を返す)
     
    例) 横幅100pxの枠に文字を4文字程度表示したい、フォントサイズ24pxが体感的にちょうど良いフォントサイズだが、
        文字列長が5以上では、はみ出てしまう。 長さが5以上の時は24pxピクセル以下の、適切なフォントサイズを計算した値を取得したい
     -> calcFlexibleFontSize("あああ", 24, 100) ; -> 三文字の場合、24pxを返す
     -> calcFlexibleFontSize("あああああ", 24, 100) ; -> 五文字の場合、20pxを返す
     */
    static float calcFlexibleFontSize(string text, float font_size, float max_width) ;
    
    //文字列の置換
    static string replaceStr(string target_string, string search_string, string replace_string);
    
    //全ての子に透過設定を適用させる処理
    static void _setCascadeOpacityEnabled(Node* parent, bool enabled);
    
    //全ての子に色設定を適用させる処理
    static void _setCascadeColorEnabled(Node* parent, bool enabled);
    
    //Jsonデータの作成
    static std::string createJsonString(string json_name, HmsVector<string> &vec_data);
    
    //ダンジョンシーンデータの初期化
    static void initDungeonData();
    
    //オンラインバトルデータの設定
    static void setOnlineBattleData(int battle_list_id, string enemy_user_master_id, bool restart_flg = false);
    
    //ポジションインデックスを作成
    static void createPosisionIndex(HmsVector<Point > &vec_position_index, int row_count, int column_count, int first_x, int first_y, int distance_x, int distance_y);
    
    //ポップアップ表示アクション
    static void openPopupAction(Node* node);
    
    //ポップアップ閉じるアクション
    static void closePopupAction(Node* node, function<void() > _func = nullptr);
    
    //ポップアップを隠すアクション（消さない！）
    static void hidePopupAction(Node* node);
    
    //お気に入り登録、解除、チェック処理
    static void setFavorite(string chara_list_id);
    static void removeFavorite(string chara_list_id);
    static bool checkFavorite(string chara_list_id);
    
    //開発か本番かのチェック
    static int _test_state ; // 0:isTest()が一度も呼ばれていない 1:テストと判定済み 2:本番と判定済み
    static bool isTest() ; //テスト時(開発のホスト時)はtrue ※毎回判定をするのが若干処理時間を取るので 二度目以降は一度判定した値を返し続けるようにする。
    
    static time_t getNowTimeStampCheckServerTime(); //サーバー時間を考慮した現在時刻を返す
    
    /*
    2014-10-09 11:22:33などの形式で与えられたDATETIMEの文字列をフォーマットする
     例えば 第一引数:"2014-10-09 11:22:33" 第二引数:"%Y年%m月%d日 %H時%M分%S秒" で "2014年10月09日 11時22分33"をもどす
     */
    static string dbtimeFormat(string datatime, string format) ;
    
    //引数に指定したパスを省略して使用できるようにする。
    static void setSearchPath(string path) ;
    
    //リソースのダウンロード先のパスを取得
    static string getDownloadSavePath() ;
    
//    //リソースが存在しなかったらタイトルシーンに戻りダウンロードする
//    void downloadReosurceIfNeeded(const std::string& filename) ;
};

#endif // __UTILITY_H__
