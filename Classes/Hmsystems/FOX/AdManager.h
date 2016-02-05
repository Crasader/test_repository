//
//  AdManager.h
//  ForceOperationX v2.15.2g
//
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol CheckVersionDelegate
- (void)didLoadVersion:(id)sender;
@end

@interface AppAdForceManager : NSObject

- (void)sendConversionWithStartPage:(NSString*)url;
- (void)sendConversionWithStartPage:(NSString*)url buid:(NSString*)buid;
- (void)setUrlScheme:(NSURL*)url;
- (void)setUrlSchemeWithOptions:(NSDictionary *)launchOptions;
- (void)setOptout:(BOOL)optout;
- (void)setDebugMode:(BOOL)debug;
- (BOOL)getBundleVersionStatus;
- (void)checkVersionWithDelegate:(id)delegate;
- (void)cacheDefaultUserAgent;

+ (AppAdForceManager*)sharedManager;

@end
