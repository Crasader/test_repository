//
//  CARMediaCommon.h
//
//  Created by CAリワード.
//  Copyright (c) 2013 CA Reward. All rights reserved.
//

#import "CARMediaCommon.h"

@interface CARMediaCommon (Common)

+ (void) setGaid:(NSString *)gaid;
+ (void) setTrxid:(NSString *)trxid;
+ (void) setScheme:(NSString *)scheme;
+ (void) setEncodedScheme:(NSString *)scheme;
+ (void) setWebView:(UIWebView *)webView;
+ (NSString *) getGaid;
+ (NSString *) getTrxid;
+ (NSString *) getScheme;
+ (NSString *) getEncodedScheme;
+ (UIWebView *) getWebView;
+ (BOOL) isOK:(NSString *)response;
+ (NSString *) request:(NSString *)url;
+ (void) setUserNickName:(NSString *)_userNickName;
+ (NSString *) getUserNickName;
+ (void) setUserKey:(NSString *)_userKey;
+ (NSString *) getUserKey;
+ (void) setMediaId:(NSString *)_mediaId;
+ (NSString *) getMediaId;
+ (void) setMediaAccessToken:(NSString *)_mediaAccessToken;
+ (NSString *) getMediaAccessToken;

@end
