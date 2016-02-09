//
//  Params+Unity.h
//  CARewardSDK
//
//  Created by A13058 on 2014/05/26.
//
//

#import "CARParams.h"

@interface CARParams (Unity)

+ (bool) setGameObject:(NSString *)gameObject;
+ (bool) hasGameObject;
+ (NSString *) getGameObject;

@end
