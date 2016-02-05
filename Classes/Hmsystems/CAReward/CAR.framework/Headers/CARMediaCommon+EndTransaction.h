//
//  CARMediaCommon.h
//
//  Created by CAリワード.
//  Copyright (c) 2013 CA Reward. All rights reserved.
//

#import "CARMediaCommon.h"

@interface CARMediaCommon (PointTransaction)
// コミット・ロールバック処理
+ (BOOL) endTransaction:(int)status;

@end
