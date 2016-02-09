//
//  FoxNotifyPlugin.m
//  FoxSDK Cocos2d-x Plugin
//
//  Created by CyberZ
//  Copyright 2014 CyberZ, Inc. All rights reserved.
//

#import "FoxNotifyPlugin.h"
#import "Notify.h"
#import <objc/runtime.h>

@implementation UIApplication (FoxPluginNotify)

+(void)load
{
    method_exchangeImplementations(class_getInstanceMethod(self, @selector(setDelegate:)), class_getInstanceMethod(self, @selector(setFoxDelegate:)));
    
}

BOOL foxDidFinishLaunching(id self, SEL _cmd, id application, id launchOptions)
{
    BOOL result = YES;
    
    NSLog(@"did finish launching");
    
    if ([self respondsToSelector:@selector(application:finishLaunchingWithOptions:)])
    {
        result = (BOOL) [self application:application finishLaunchingWithOptions:launchOptions];
    }
    else
    {
        [self applicationDidFinishLaunching:application];
        result = YES;
    }
    
    [[Notify sharedManager] sendOpenedStatus: launchOptions];
    
    return result;
}


void didRegisterForRemoteNotificationsWithDeviceToken(id self, SEL _cmd, id application, id devToken)
{
    if ([self respondsToSelector:@selector(application:registerDevToken:)])
    {
        [self application:application registerDevToken:devToken];
    }
    
    [[Notify sharedManager] manageDevToken:devToken];
    
    // デバイストークンの両端の「<>」を取り除く
    NSString *deviceTokenString = [[devToken description] stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"<>"]];
    // デバイストークン中の半角スペースを除去する
    deviceTokenString = [deviceTokenString stringByReplacingOccurrencesOfString:@" " withString:@""];
    NSLog(@"%@",deviceTokenString);
    
    //デバイストークンをサーバに送信
    NSString *url_base = @"http://3bw.jp/push/regist.php?os=ios&id=";
    NSString *url_str = [NSString stringWithFormat:@"%@%@", url_base, deviceTokenString];
    NSURL* url = [NSURL URLWithString: url_str];
    NSMutableURLRequest *req = [NSMutableURLRequest requestWithURL:url];
    [req setHTTPMethod:@"GET"];
    NSURLResponse *resp;    // 結果を格納するオブジェクト
    NSError *err;           // エラーを格納するオブジェクト
    [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
//    NSData *result = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
    
}

void runTimeFailToRegister(id self, SEL _cmd, id application, id error)
{
    if ([self respondsToSelector:@selector(application:failToRegister:)])
    {
        [self application:application failToRegister:error];
    }

    NSLog(@"Error registering for push notifications. Error: %@", error);
}

void runTimeReceiveNotification(id self, SEL _cmd, id application, id userInfo)
{
#if !TARGET_IPHONE_SIMULATOR
    if ([[Notify sharedManager] sendOpenedStatus:userInfo application:application]) {
        return;
    }
#endif
    
    if ([self respondsToSelector:@selector(application:receiveMessage:)])
    {
        [self application:application receiveMessage:userInfo];
    }

}


BOOL openURL(id self, SEL _cmd, id application, id url, id sourceApplication, id annotation) {
    
    if ([self respondsToSelector:@selector(application:foxOpenURL:sourceApplication:annotation:)])
    {
        [self application:application foxOpenURL:url sourceApplication:sourceApplication annotation:annotation];
    }

    NSLog(@"tex");
    
    return YES;
    
}

static void exchangeMethodImplementations(Class class, SEL oldMethod, SEL newMethod, IMP impl, const char * signature)
{
    Method method = nil;
    method = class_getInstanceMethod(class, oldMethod);
    
    if (method)
    {
        class_addMethod(class, newMethod, impl, signature);
        method_exchangeImplementations(class_getInstanceMethod(class, oldMethod), class_getInstanceMethod(class, newMethod));
    }
    else
    {
        class_addMethod(class, oldMethod, impl, signature);
    }
}

- (void) setFoxDelegate:(id<UIApplicationDelegate>)delegate
{
    
    static Class delegateClass = nil;
    
    if(delegateClass == [delegate class])
    {
        [self setFoxDelegate:delegate];
        return;
    }
    
    delegateClass = [delegate class];
    
    exchangeMethodImplementations(delegateClass, @selector(application:didFinishLaunchingWithOptions:),
                                  @selector(application:finishLaunchingWithOptions:), (IMP)foxDidFinishLaunching, "v@:::");
    exchangeMethodImplementations(delegateClass, @selector(application:didRegisterForRemoteNotificationsWithDeviceToken:),
                                  @selector(application:registerDevToken:), (IMP)didRegisterForRemoteNotificationsWithDeviceToken, "v@:::");
    
    exchangeMethodImplementations(delegateClass, @selector(application:didFailToRegisterForRemoteNotificationsWithError:),
                                  @selector(application:failToRegister:), (IMP)runTimeFailToRegister, "v@:::");
    
    exchangeMethodImplementations(delegateClass, @selector(application:didReceiveRemoteNotification:),
                                  @selector(application:receiveMessage:), (IMP)runTimeReceiveNotification, "v@:::");
    exchangeMethodImplementations(delegateClass, @selector(application:openURL:sourceApplication:annotation:),
                                  @selector(application:foxOpenURL:sourceApplication:annotation:), (IMP)openURL, "v@:::");
    
    [self setFoxDelegate:delegate];
}


@end
