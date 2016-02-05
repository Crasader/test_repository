//
//  Cocos2dxFox.h
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#ifndef Cocos2dxFox_h
#define Cocos2dxFox_h
#define CC_LTV_PARAM_SKU      "_sku"
#define CC_LTV_PARAM_PRICE    "_price"
#define CC_LTV_PARAM_CURRENCY "_currency"
#define CC_LTV_PARAM_OUT      "_out"

class FoxPlugin
{
public:

    // インストール計測

    /*
     * 成果通知(ブラウザの起動あり)
     * @param url 起動後に表示するURL
     */
    static void sendConversion(const char* url);

    /*
     * 成果通信(ブラウザの起動あり)
     * @param url 起動後に表示するURL
     * @param buid 広告主端末ID
     */
    static void sendConversion(const char* url, const char* buid);


    // LTV計測

    /*
     * アプリ内成果処理
     * @param cvPoint 成果地点ID
     */
    static void sendLtv(int cvPoint);

    /*
     * アプリ内成果処理
     * @param cvPoint 成果地点ID
     * @param buid 広告主端末ID
     */
    static void sendLtv(int cvPoint, const char* buid);

    /*
     * 任意パラメータ追加処理
     * @param name パラメータ名
     * @param value 値
     */
    static void addParameter(const char* name, const char* value);


    // アクセス解析

    /* アプリ起動計測 */
    static void sendStartSession();

    /*
     * イベントトラッキング
     * @param eventName イベント名
     * @param action アクション名
     * @param label ラベル
     * @param value 値
     */
    static void sendEvent(const char* eventName, const char* action, const char* label, int value);

    /*
     * 購入イベントトラッキング
     * @param eventName イベント名
     * @param action アクション名
     * @param label ラベル
     * @param orderId 注文番号
     * @param sku 商品コード
     * @param itemName 商品名
     * @param price 商品単価
     * @param quantity 商品購入数
     * @param currency 通貨コード
     */
    static void sendEvent(const char* eventName, const char* action, const char* label, const char* orderID, const char* sku, const char* itemName, double price, int quantity, const char* currency);

    /*
     * 任意の属性によるトラッキング
     * @param userId ユーザーID
     * @param userAttr1 属性1
     * @param userAttr2 属性2
     * @param userAttr3 属性3
     * @param userAttr4 属性4
     * @param userAttr5 属性5
     */
    static void sendUserInfo(const char* userId, const char* userAttr1, const char* userAttr2, const char* userAttr3, const char* userAttr4, const char* userAttr5);


    // OS別処理

    /* iOS */
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

        /* PUSH通知(APNs)の設定と送信 */
        static void notifyManager();
    
    /* Android */
    #elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

        /*
         * PUSH通知(GCM)の設定と送信
         * @param senderId SenderID
         */
        static void notifyManager(const char* senderId);

        /*
         * v2.10.4g以前(2.8u以降〜v2.10.4g)のSDKからアップデートする際にコールする
         * 全ての処理の先頭で必ずコールしてください。
         */
        static void updateFrom2_10_4g();
    #endif   
};

#endif
