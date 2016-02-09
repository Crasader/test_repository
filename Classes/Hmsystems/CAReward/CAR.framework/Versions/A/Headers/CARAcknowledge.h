//
//  CARAcknowledge.h
//
//  Created by CAリワード on 11/03/07.
//  Copyright 2011 CA Reward, Inc. All rights reserved.
//
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@interface CARAcknowledge : NSObject <UIAlertViewDelegate> {
 @private
    BOOL        debug_;
	NSInteger	numberOfRequest_;
    NSString    *publicAppKey_;	
    NSString    *cid_;
    NSString    *pid_;
    NSString    *cpi_;
    NSString    *appVersion_;
    NSString    *cipheredString_;
    NSString    *query_;
	NSString	*afid_;
	NSString	*mid_;
	NSString	*userid_;
    NSString    *title_;
    NSString    *msg_;
    NSString    *url_;
    BOOL        urlScheme_;
    NSString    *uiidString_;
    NSString    *installDateString_;
    NSString    *launchDateString_;
    BOOL        finished_;
    NSString    *i4sa_;
    NSString    *ext_track_;

    NSInteger   snumber_;
    NSString    *stime_;
    NSString    *auid_;

    NSString    *uid_;
    
    NSMutableDictionary *products_;
    
    BOOL        launchedAnalytics_;
}

@property (nonatomic, assign) BOOL debug;
@property (nonatomic, assign) NSInteger numberOfRequest;
@property (nonatomic, copy) NSString *redirectURLString;
@property (nonatomic, copy) NSString *i4sa;
@property (nonatomic, copy) NSString *ext_track;
@property (nonatomic, copy) NSString *appVersion;

// プロパティ
//@property BOOL isOnlyFirst;
@property (copy, readwrite) NSString *publicAppKey;
@property (copy, readwrite) NSString *cid;
@property (copy, readwrite) NSString *pid;
@property (copy, readwrite) NSString *cpi;
@property (copy, readwrite) NSString *query;
@property (nonatomic, retain) NSString *afid;
@property (nonatomic, retain) NSString *mid;
@property (nonatomic, retain) NSString *userid;
@property (nonatomic, retain) NSString *title;
@property (nonatomic, retain) NSString *msg;
@property (nonatomic, retain) NSString *url;
@property (nonatomic, assign) BOOL urlScheme;
@property (nonatomic, retain) NSString *uiidString;
@property (nonatomic, retain) NSString *installDateString;
@property (nonatomic, retain) NSString *launchDateString;
@property (nonatomic, assign) BOOL finished;
@property (nonatomic, assign) NSInteger snumber;
@property (nonatomic, retain) NSString *stime;
@property (nonatomic, retain) NSString *auid;
@property (nonatomic, retain) NSString *uid;
@property (nonatomic, retain) NSString *cipheredString;
@property (nonatomic, retain) NSMutableDictionary *products;
@property (nonatomic, assign) BOOL launchedAnalytics;

- (void) getUID;
- (NSString *)ciphered;

- (void)notifyAppLaunchToCAR:(NSString *)publicAppKey 
                         cid:(NSString *)cid 
                         pid:(NSString *)pid 
                         cpi:(NSString *)cpi 
                       query:(NSString *)query 
                   urlScheme:(BOOL)urlScheme;

- (void)notifyAppLaunch:(NSString *)publicAppKey 
                         cid:(NSString *)cid 
                         pid:(NSString *)pid 
                         cpi:(NSString *)cpi;

@end
