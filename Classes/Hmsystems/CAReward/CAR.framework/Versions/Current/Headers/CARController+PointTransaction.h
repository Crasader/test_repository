//
//  CARController+PointTransaction.h
//  CARewardSDK
//
//  Created by A13058 on 2014/04/21.
//
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "CARController.h"

@interface CARController (PointTransaction)

//
+ (void)setIUserPointTransactionManager:(id<IUserPointTransactionBehaviour>)callback;
+ (id<IUserPointTransactionBehaviour>)getIUserPointTransactionManager;

+ (void)startUserPointTransactionManager:(NSString *)publicAppKey
								 mediaId:(NSString *)mediaId
						mediaAccessToken:(NSString *)mediaAccessToken
								 userKey:(NSString *)userKey
							userNickName:(NSString *)userNickName
								callback:(id<IUserPointTransactionBehaviour>)callback
								 isDebug:(BOOL)isDebug;

+ (void)proceedUserPointTransaction;

@end
