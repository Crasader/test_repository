//
//  CARNotifyAppLaunch.h
//
//  Created by CA-Reward on 11/08/29.
//  Copyright 2011 CA-Reward,Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CARNotifyAppLaunch : NSObject

- (id)initWithAppKey:(NSString*)appKey;
- (NSString*)getUID;
- (void)notifyAppLaunch:(NSString*)cId mId:(NSString*)mId pId:(NSString*)pId userId:(NSString*)userId apiKey:(NSString*)apiKey;

- (void)setDebug:(BOOL)debug;

@end
