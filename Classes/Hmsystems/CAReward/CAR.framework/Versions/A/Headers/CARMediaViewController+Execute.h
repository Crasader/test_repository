//
//  CARMediaCommon.h
//
//  Created by CAリワード.
//  Copyright (c) 2013 CA Reward. All rights reserved.
//

#import "CARMediaViewController.h"

@interface CARMediaViewController (Execute)

+ (void) execute;
+ (CARMediaViewController *) getCARMediaViewController;
+ (void) execute:(UIViewController *)root;
+ (void) execute:(UIViewController *)root mediaViewController:(CARMediaViewController *)vc;

@end
