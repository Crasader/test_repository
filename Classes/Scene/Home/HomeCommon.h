#include "common.h"

#ifndef __HOME_COMMON_H__
#define __HOME_COMMON_H__

////////////// NormalScene系 //////////////
#define MAIN_LAYER_TAG 1
#define TITLE_LAYER_TAG 2
#define HEADER_LAYER_TAG 3
#define FOOTER_LAYER_TAG 4
#define INFO_LAYER_TAG 5
#define RANKING_LAYER_TAG 6

#define ZORDER_MAIN_LAYER 1
#define ZORDER_TITLE_LAYER 2
#define ZORDER_HEADER_LAYER 3
#define ZORDER_FOOTER_LAYER 4
#define ZORDER_INFO_LAYER 5
#define ZORDER_RANKING_LAYER 6

#define NEXT_LAYER_TAG 7
#define ALL_TAP_DISABLE_LAYER_TAG 8

#define MOVING_TIME 0.2f
#define FADE_TIME 0.1f

#define BACK_BUTTON_TAG 50
#define ZORDER_TITLE_BAR 2
#define ZORDER_BACK_BUTTON 3
#define ZORDER_TITLE_LABEL 3

#define ZORDER_BUTTON 1

#define DEFAULT_FONT_SIZE 29
#define FONT_SIZE_LIST 38

#define CHALLENGER_BUTTON_TAG 10

#define CHARA_NEW_TIME 3600
////////////////////////////////////////////


///////// TitleScene Tutorial系 /////////////
#define TOUCH_SCREAN_TAG 2
#define ZORDER_TOUCH_SCREAN 2

#define BLINKING_TIME 2.0f

#define ZORDER_DOWNLOAD_LAYER 1000
////////////////////////////////////////////


///////////////  体力回復系  ///////////////
//行動力回復時間（1分 * 180秒 = 180)
#define POWER_HEAL_TIME 180

//ランキングイベント行動力回復時間（3時間 * 60分 * 60秒 = 10800)
#define EVENT_POWER_HEAL_TIME 10800 

//ボスイベント行動力回復時間（3時間 * 60分 * 60秒 = 10800)
#define BOSS_EVENT_POWER_HEAL_TIME 10800
////////////////////////////////////////////


/////////////  ポップアップ画面系  /////////////
#define ALERT_LAYER_TAG 1000
#define ZORDER_ALERT_LAYER 1000

#define POPUP_ACTION_TAG 2000

#define POPUP_LAYER_TAG 3000
#define ZORDER_POPUP_LAYER 3000
////////////////////////////////////////////


///////////////  ロード画面系  ///////////////
#define LOADING_LAYER_TAG 10000
#define ZORDER_LOADING_LAYER 10000

#define RETRANS_LAYER_TAG 20000
#define ZORDER_RETRANS_LAYER 20000
////////////////////////////////////////////

///////////////  スクロール画面系  ///////////////
#define SCROLLVIEW_TAG 1
#define SCROLLLAYER_TAG 2
///////////////////////////////////////////////

///////////////  キャラ画面系  ///////////////
#define ROW_1_CHARA_COUNT 5 //一行に並ぶキャラの数
#define POS_X_FIRST 80
#define POS_X_DISTANCE 120
#define POS_Y_DISTANCE 120

#define MAX_SELL_COUNT 20 //一度に売却できる数
#define SELL_BG_HEIGHT 120

#define MAX_DECK_COUNT 10  //デッキの最大数
#define MAX_DECK_CHARA_COUNT 20 //デッキ所持枚数
//#define MAX_CHARA_HAVE_NUMBER 3 //デッキ内で所持できる最大枚数
/////////////////////////////////////////////

/////////////// 重複しているdefineをまとめた ///////////
#define BACK_TAG -1
#define BACK_LIST_TAG -1
#define LOCK_BUTTUN_TAG 11
#define LOCK_IMAGE_TAG 12
#define SORT_LAYER_TAG 20
#define NEW_GET_TIME 6000.0f
////////////////////////////////////////////////////

///////////////  キャラ売却価格  ///////////////
//PHPのdefineから取得するようにしたい！
#define SELL_PRICE_NONE 0
#define SELL_PRICE_NORMAL 100
#define SELL_PRICE_RARE 200
#define SELL_PRICE_SUPER_RARE 300
#define SELL_PRICE_ULTRA_RARE 400
#define SELL_PRICE_LEGEND_RARE 500
///////////////////////////////////////////////

/////////////// 課金アイテム系 start ////////////////

//課金アイテムのproductの情報はAndroid, ios共にDBから取得するように変更しました。

/////////////// 課金アイテム系 end ////////////////

#endif // __HOME_COMMON_H__
