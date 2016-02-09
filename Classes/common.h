#ifndef __COMMON_H__
#define __COMMON_H__

//------------------------------- App設定 ----------------------------------
//本番URL
//#define HOST "http://3bw.jp/"
//開発URL
#define HOST "http://192.168.100.62/war_php_1_1/"

//お問い合わせ先メールアドレス
#define MAIL_ADDRESS "仮メールアドレス@support.co.jp"

//パッケージ名
#define ANDROID_PACKAGE_NAME "jp.pkga.war"

//アプリバージョン
#define APPLICATION_VIRSION_FOR_IOS "2.9"
#define APPLICATION_VIRSION_FOR_ANDROID "2.9"

//　0:Resourceのダウンロード機能を無効にする 1:ダウンロードを行う ※本番では1
#define DOWNLOAD_RESOURCE_FLG 1

// 楽天App市場フラグ 0:Google Play 1:楽天App市場 ※java側の変更も必要
#define RAKUTEN_FLG 0

#define SERVICE_HOST "3bw.jp" //本番のHOST
//--------------------------------------------------------------------------



//------------------------------ フォント設定 --------------------------------
#define FONT_NAME "fonts/DShirkg8.ttf"

#if (CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
    #define FONT_NAME_SYSTEM "HiraginoKaku-W8-90msp-RKSJ-H"
    #define IF_IOS 1
    #define IF_ANDROID 0
#else
    #define FONT_NAME_SYSTEM "fonts/DShirkg8.ttf"
    #define IF_IOS 0
    #define IF_ANDROID 1
#endif
//--------------------------------------------------------------------------



//-------------------------------- 画面サイズ -------------------------------
#define HEADER_SIZE 140
#define TITLE_BAR_SIZE 80
#define FOOTER_SIZE 108
#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 960
//--------------------------------------------------------------------------



//------------------------------ 区切り文字 ---------------------------------
//項目
#define SPLIT_CHARACTER_HEAD "//"
//タブ
#define SPLIT_CHARACTER_TAB "/t"
//行
#define SPLIT_CHARACTER_LINE "/n"
//入力禁止文字
#define ILLEGAL_CHAR "/"
//--------------------------------------------------------------------------

//共通
#define PI 3.1415926535897932384626433832795

//ダウンロードstep最大値
#define DOWNLOAD_STEP_MAX 3

//名称
#define JEWEL_NAME "オーブ"
#define MONEY_NAME "資金"

//通信タイムアウト時間(0.01秒)
#define TIME_OUT_CHECK_COUNT 1000

#endif // __COMMON_H__
