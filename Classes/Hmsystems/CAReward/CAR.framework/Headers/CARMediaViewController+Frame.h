//
//  CARMediaCommon.h
//
//  Created by CAリワード.
//  Copyright (c) 2013 CA Reward. All rights reserved.
//

#import "CARMediaViewController.h"

@interface CARMediaViewController (Frame)

- (CGRect) getApplicationFrame;
- (CGRect) getNavigationFrame;
- (CGRect) getWebViewFrame;
+ (void) setPrefersStatusBarHidden:(bool)isPrefersStatusBarHidden;

@end
