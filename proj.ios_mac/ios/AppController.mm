/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#import "AppController.h"
#import "platform/ios/CCEAGLView-ios.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "RootViewController.h"
#import "InAppBillingManager.h"
#import "Apsalar.h"
#import "SaveData.h"
#include "Cocos2dxFox.h"
#import <Fabric/Fabric.h>
#import <Crashlytics/Crashlytics.h>

@implementation AppController

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	
    [Fabric with:@[[Crashlytics class]]];
    [[Crashlytics sharedInstance] setObjectValue:[NSString stringWithUTF8String:SaveData::loadString("user_master_id").c_str()] forKey:@"user_master_id"];
    [[Crashlytics sharedInstance] setObjectValue:[NSString stringWithUTF8String:SaveData::loadString("user_name").c_str()] forKey:@"名前"];
    [[Crashlytics sharedInstance] setObjectValue:[NSString stringWithUTF8String:SaveData::loadString("level").c_str()] forKey:@"レベル"];
    [[Crashlytics sharedInstance] setObjectValue:[NSString stringWithUTF8String:SaveData::loadString("rank_name").c_str()] forKey:@"ランク"];
    [[Crashlytics sharedInstance] setObjectValue:[NSString stringWithUTF8String:SaveData::loadString("comment").c_str()] forKey:@"コメント"];
    
    //スプラッシュ時間設定
    sleep(1);
    
    //画面が自動スリープしないように制御する
    application.idleTimerDisabled = YES;
	
	// バッジを消す（０件に設定）
	[UIApplication sharedApplication].applicationIconBadgeNumber = 0;
    
    // 初回起動時にのみ成果発生ページを表示します。
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    
    // 初回起動かどうかをチェックします。(i-mobile対応。初回起動の場合、成果通知を送信する。)
    bool isFirstLoaded = [defaults boolForKey:@"isFirstLoaded"];
    if (isFirstLoaded == false) {
        //		isFirstLoaded = true;
        //		[defaults setBool:isFirstLoaded forKey:@"isFirstLoaded"];
        //		NSString *thankPageURL = @"http://spdeliver.i-mobile.co.jp/app/ad_conv.ashx?sid=9481&url=call3bw%3A%2F%2F";
        //		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:thankPageURL]];
    }
    
    //iOS用課金処理の初期化処理
    InAppBillingManager::initInAppBillingForIOS() ;
    
    //プッシュ通知部1 デバイストークンの発行をリクエスト
    // バッジ、サウンド、アラートをリモート通知対象として登録する
    // (画面にはプッシュ通知許可して良いかの確認画面が出る)
    NSString *currentVersion = [[UIDevice currentDevice] systemVersion];
    if([currentVersion compare:@"8.0" options:NSNumericSearch] == NSOrderedAscending){
        // i0S7以前の処理
        [application registerForRemoteNotificationTypes: (UIRemoteNotificationTypeBadge|
                                                          UIRemoteNotificationTypeSound|
                                                          UIRemoteNotificationTypeAlert)];
        
    } else {
        // iOS8の場合の処理
        UIUserNotificationType types = UIUserNotificationTypeBadge | UIUserNotificationTypeSound | UIUserNotificationTypeAlert;
        
        UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:types categories:nil];
        
        [[UIApplication sharedApplication] registerUserNotificationSettings:settings];
        
    }
    
    //FOXSDK
    FoxPlugin::sendConversion("default");
    FoxPlugin::notifyManager();
    
    //FOX アクセス解析
    FoxPlugin::sendStartSession();
    
    cocos2d::Application *app = cocos2d::Application::getInstance();
    app->initGLContextAttrs();
    cocos2d::GLViewImpl::convertAttrs();
    
    // Override point for customization after application launch.
    
    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    
    // Init the CCEAGLView
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [window bounds]
                                         pixelFormat: (NSString*)cocos2d::GLViewImpl::_pixelFormat
                                         depthFormat: cocos2d::GLViewImpl::_depthFormat
                                  preserveBackbuffer: NO
                                          sharegroup: nil
                                       multiSampling: NO
                                     numberOfSamples: 0 ];
    
    //マルチタップに必要な処理
    [eaglView setMultipleTouchEnabled:YES];
    
    
    // Use RootViewController manage CCEAGLView
    _viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    _viewController.wantsFullScreenLayout = YES;
    _viewController.view = eaglView;
    
    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: _viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:_viewController];
    }
    
    [window makeKeyAndVisible];
    
    [[UIApplication sharedApplication] setStatusBarHidden:true];
    
    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    cocos2d::GLView *glview = cocos2d::GLViewImpl::createWithEAGLView(eaglView);
    cocos2d::Director::getInstance()->setOpenGLView(glview);
    
    app->run();
	
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
	/*
	 Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	 Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	 */
	//We don't need to call this method any more. It will interupt user defined game pause&resume logic
	/* cocos2d::Director::getInstance()->pause(); */
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	/*
	 Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	 */
	//We don't need to call this method any more. It will interupt user defined game pause&resume logic
	/* cocos2d::Director::getInstance()->resume(); */
    [Apsalar startSession:@"HMS" withKey:@"zR2ryHwI"];
    [Apsalar event:@"wakeup"];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
	/*
	 Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	 If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
	 */
	cocos2d::Application::getInstance()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
	/*
	 Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
	 */
	cocos2d::Application::getInstance()->applicationWillEnterForeground();
    
    //FOX アクセス解析
    FoxPlugin::sendStartSession();
}

- (void)applicationWillTerminate:(UIApplication *)application {
	/*
	 Called when the application is about to terminate.
	 See also applicationDidEnterBackground:.
	 */
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
	/*
	 Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
	 */
}


- (void)dealloc {
	[window release];
	[super dealloc];
}

//プッシュ通知部2 iOS8.0以降に必要な関数 デバイストークンの発行をリクエスト後この関数がコールバックされる。この関数内でregisterForRemoteNotificationsを呼び出すと成功か失敗がコールバックメソッドで通知される。(iOS8.0以前はリクエスト後自動でこの成功か失敗の通知が行われていた)
- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings
{
	[application registerForRemoteNotifications];
}

//プッシュ通知部3 デバイストークン発行失敗
- (void)application:(UIApplication*)app didFailToRegisterForRemoteNotificationsWithError:(NSError*)err{
	NSLog(@"Errorinregistration.Error:%@",err);
}

//プッシュ通知部3 デバイストークン発行成功
- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
	// デバイストークンの両端の「<>」を取り除く
	NSString *deviceTokenString = [[deviceToken description] stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"<>"]];
	// デバイストークン中の半角スペースを除去する
	deviceTokenString = [deviceTokenString stringByReplacingOccurrencesOfString:@" " withString:@""];
	NSLog(@"device_token: %@",deviceTokenString);
	
	//デバイストークンをサーバに送信
    NSString *url_base = @"http://3bw.jp/push/regist.php?os=ios&id=";
    NSString *url_str = [NSString stringWithFormat:@"%@%@", url_base, deviceTokenString];
	NSURL* url = [NSURL URLWithString: url_str];
	NSMutableURLRequest *req = [NSMutableURLRequest requestWithURL:url];
	[req setHTTPMethod:@"GET"];
	NSURLResponse *resp;    // 結果を格納するオブジェクト
	NSError *err;           // エラーを格納するオブジェクト
	NSData *result = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
	
}

// URLスキーム
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    //ユーザ情報を取得
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    NSString *user_master_id = [ud stringForKey:@"user_master_id"];
    NSString *password = [ud stringForKey:@"password"];
    NSLog(@"user_master_id: %@", user_master_id);
    NSLog(@"password: %@", password);
    NSString *host = [url host];
    NSString *query =[[url query] stringByRemovingPercentEncoding];

    if(user_master_id!=nil && password!=nil) {
        //サーバ送信
//        NSString *url_base = @"http://3bw.jp/user/url_scheme.php?user_master_id=";
        NSString *url_base = @"http://192.168.100.62/war_php_1_1/user/url_scheme.php?user_master_id=";
        NSString *url_str = [NSString stringWithFormat:@"%@%@&password=%@&type=%@&query=%@", url_base, user_master_id, password, host, query];
        NSURL* url_php = [NSURL URLWithString: url_str];
        NSMutableURLRequest *req = [NSMutableURLRequest requestWithURL:url_php];
        [req setHTTPMethod:@"GET"];
        NSURLResponse *resp;    // 結果を格納するオブジェクト
        NSError *err;           // エラーを格納するオブジェクト
        NSData *result = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
        return YES;
    }
    return NO;
}


@end
