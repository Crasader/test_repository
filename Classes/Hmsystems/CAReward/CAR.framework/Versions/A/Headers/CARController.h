//
//  CARController.h
//
//  Created by CAリワード on 12/03/21.
//  Copyright (c) 2012 CA Reward. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/Storekit.h>

// ポイント管理が通知するコミットステータス
typedef enum _POINT_COMMIT_STATUS_ENUM{
    STATUS_OK,
    STATUS_NG,
    STATUS_MAX
}POINT_COMMIT_STATUS_ENUM;

@protocol IUserPointBehaviour <NSObject>

// インセンティブ⇔ポイント交換 コミット処理結果をコールバック通知
// STATUS_OK = ポイント交換完了
// STATUS_NG = 〃エラー
@required
-(void)onSpendResult:(POINT_COMMIT_STATUS_ENUM)commitStatus;

// インセンティブ⇔ポイント交換 の処理前にコールバックする
// STATUS_OK = 処理を継続
// STATUS_NG = 処理を中断する
@required
-(POINT_COMMIT_STATUS_ENUM)onPrepareSpend:(NSString*)scheme;

@end

@protocol IUserPointTransactionBehaviour <NSObject>
@required
-(void)onPrepareSpend:(NSString*)scheme;
-(void)onSpendResult:(POINT_COMMIT_STATUS_ENUM)commitStatus;
@end

@interface CARController : NSObject

+ (void)init;

+ (void)setDebug:(BOOL)debug;
+ (void)setNumberOfRequest:(NSInteger)numberOfRequest;
+ (void)setRedirectUrlString:(NSString*)redirectURLString;
+ (void)setI4sa:(NSString*)i4sa;
+ (void)setExtTrack:(NSString*)ext_track;
+ (void)setUid:(NSString*)uid;
+ (void)setAppVersion:(NSString*)appVer;
+ (void)setLaunchedAnalytics:(BOOL)launchedAnalytics;

+ (void)notifyAppLaunchToCAR:(NSString *)publicAppKey 
                         cid:(NSString *)cid 
                         pid:(NSString *)pid 
                         cpi:(NSString *)cpi 
                       query:(NSString *)query 
                   urlScheme:(BOOL)urlScheme;
+ (void)notifyAppLaunch:(NSString *)publicAppKey 
                    cid:(NSString *)cid 
                    pid:(NSString *)pid 
                    cpi:(NSString *)cpi;

+ (NSString*)getUID:(NSString*)publicAppKey;

+ (void)notifyAnalyticsStart;
+ (void)notifyAnalyticsEnd;
+ (void)notifyAnalyticsEvent:(NSInteger)kind_id
                    is_async:(BOOL)is_async;

+ (void)addProducts:(NSArray *)products;
+ (void)notifyCompletePayment:(SKPaymentTransaction *)transaction;

+ (void)notifyCompleteContBill:(NSString *)cvid
                     cvn:(NSString *)cvn
                  cvext1:(NSString *)cvext1
                  cvext2:(NSString *)cvext2
                cvamount:(NSString *)cvamount;
+ (void)notifyCancelContBill:(NSString *)cvid 
                         cvn:(NSString *)cvn
                      cvext1:(NSString *)cvext1
                      cvext2:(NSString *)cvext2;


// -----------------------
// for user point manage
+ (void)startUserPointManager:(NSString *)publicAppKey
                      mediaId:(NSString *)mediaId
             mediaAccessToken:(NSString *)mediaAccessToken
                      userKey:(NSString *)userKey
                 userNickName:(NSString *)userNickName
                     callback:(id<IUserPointBehaviour>)callback
                      isDebug:(BOOL)isDebug;

+ (void)commitUserPointManager;
+ (void)rollbackUserPointManager;
+ (void)addPoint:(NSString *)scheme;

+ (void) close;

+ (NSString*)getAutoRegisterdUserPointManagerUserKey:(NSString*)publicAppKey;
+ (void)setIUserPointManager:(id<IUserPointBehaviour>)callback;
+ (id<IUserPointBehaviour>)getIUserPointManager;

+ (void)setOnetimeKey:(NSString*)onetimeKey;
+ (NSString*)getOnetimeKey;

@end
