//
//  CARMediaCommon.h
//
//  Created by CAリワード.
//  Copyright (c) 2013 CA Reward. All rights reserved.
//

#import "CARMediaViewController.h"

@interface CARMediaViewController (Common)

+ (UIViewController *) getRootViewController;

+ (void) setAppKey:(NSString *)_appKey;
+ (NSString *) getAppKey;
+ (void) setApiKey:(NSString *)_apiKey;
+ (NSString *) getApiKey;
+ (void) setFromId:(NSString *)_fromId;
+ (NSString *) getFromId;
+ (void) setLoadingMsg:(NSString *)_loadingMsg;
+ (NSString *) getLoadingMsg;
+ (void) setMediaOwnerId:(NSString *)_mediaOwnerId;
+ (NSString *) getMediaOwnerId;
+ (void) setUrl:(NSString *)_url;
+ (NSString *) getUrl;
+ (void) setTitle:(NSString *)_title;
+ (NSString *) getTitle;
+ (void) setBackButton:(NSString *)_backButton;
+ (NSString *) getBackButton;
+ (void) setButtonGravity:(NSString *)_buttonGravity;
+ (NSString *) getButtonGravity;
+ (void) setDeltaString:(NSString *)_deltaString;
+ (NSString *) getDeltaString;
+ (void) setBkColor:(NSString *)_bkColor;
+ (NSString *) getBkColor;
+ (void) setUsePointManage:(BOOL)_usePointManage;
+ (BOOL) getUsePointManage;


@end
