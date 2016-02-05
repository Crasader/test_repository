//
//  FoxNotifyPlugin.h
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CocosDenshion.h" //エラーが出たら http://192.168.100.31/news/81を参照して対処して下さい
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= 30000
#import <AVFoundation/AVFoundation.h>
#else
#import "CDXMacOSXSupport.h"
#endif

@interface UIApplication(SupressWarnings)

- (BOOL)application:(UIApplication *)application foxOpenURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation;

@end
