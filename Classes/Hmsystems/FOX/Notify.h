//
//  Notify.h
//  ForceOperationX v2.15.1g
//
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NSURLConnection *connection;

@interface Notify : NSObject {

}

- (void)manageDevToken:(NSData *)devToken;
- (BOOL)sendOpenedStatus:(NSDictionary *)userInfo application:(UIApplication *)app;
- (BOOL)sendOpenedStatus:(NSDictionary *)launchOptions;
- (BOOL)openUrlScheme: (NSString *)url;

+ (Notify*)sharedManager;

@end
