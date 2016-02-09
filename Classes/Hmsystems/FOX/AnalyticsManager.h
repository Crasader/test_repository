//
//  AnalyticsManager.h
//  ForceOperationX Analytics v2.15.1g
//
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@interface ForceAnalyticsManager : NSObject

/*
 * セッション開始・終了
 */
+ (void)sendStartSession;
+ (void)sendEndSession;

/*
 * イベントトラッキング
 *    アプリ内の特定のイベントの実行回数や利用されたアイテム数などを集計できます。
 *    実行例：
 *        [ForceAnalyticsManager sendEvent:@"チュートリアル完了" action:nil label:nil value:1];
 *
 *        [ForceAnalyticsManager sendEvent:@"進行イベント" action:@"街到着" label:@"最初の街" value:1];
 */
+ (void)sendEvent:(NSString*)eventName action:(NSString*)action label:(NSString*)label value:(NSUInteger)value;

/*
 * アプリ内課金購入イベント
 */
+ (void)sendEvent:(NSString*)eventName action:(NSString*)action label:(NSString*)label transaction:(SKPaymentTransaction*)transaction product:(SKProduct*)product;

/*
 * 課金イベント
 */
+ (void)sendEvent:(NSString*)eventName
           action:(NSString*)action
            label:(NSString*)label
          orderID:(NSString*)orderID
              sku:(NSString*)sku
         itemName:(NSString*)itemName
            price:(double)price
         quantity:(NSUInteger)quantity
         currency:(NSString*)currency;

/*
 * 会員情報送信
 *     アプリ側の会員IDでトラッキング・集計したい場合に実行してください。
 *     複数回実行した場合には、最後に送信された情報で上書きされます。
 *     実行例：
 *        [ForceAnalyticsManager sendUserInfo:@"user0123"];
 *
 *     会員IDに加えて任意のユーザー属性を5つまで追加できます。
 *     例えば、attr1にゲーム内職業、attr2にゲーム内レベルなどを設定することで、
 *     それぞれの属性でトラッキング・集計ができます。
 *     実行例：
 *         [ForceAnalyticsManager sendUserInfo:@"user0123" attr1:@"魔法使い" attr2:@"25" attr3:nil attr4:nil attr5:nil];
 */
+ (void)sendUserInfo:(NSString*)userid;
+ (void)sendUserInfo:(NSString*)userid attr1:(NSString*)attr1 attr2:(NSString*)attr2 attr3:(NSString*)attr3 attr4:(NSString*)attr4 attr5:(NSString*)attr5;

@end
