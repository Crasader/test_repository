//
//  CARView.h
//  CARewardSDK
//
//  Created by A13058 on 2014/06/12.
//
//

#import <UIKit/UIKit.h>

@interface CARView : UIView

@property (retain) NSString* fromId;
@property (retain) NSArray* segments;

- (void) execute;

@end
