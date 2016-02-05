//
//  CARMediaViewController.h
//
//  Created by CA-Reward on 11/08/29.
//  Copyright 2011 CA-Reward,Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>


@interface CARMediaViewController : UIViewController <UIWebViewDelegate> {
@private
	UINavigationBar* navBar_;
	UINavigationItem* navItem_;
	UIBarButtonItem* btnLeft_;
	UIWebView* webView_;
	UIActivityIndicatorView* activityIndicator_;
    MPMoviePlayerViewController* movie_;
	CGRect navBarRect_;
	NSString* itemTitle_;
	NSString* btnTitle_;
	UIColor*  navBarColor_;
    
    NSString* uiidString_;
    NSString* installDateTime_;
    NSString* publicAppKey_;
	NSString* userId_;
	NSString* mId_;
	NSString* cId_;
    NSString* param_;
	NSString* afId_;
	NSString* apiKey_;
    NSInteger page_;
    NSString* fromId_;

    BOOL browser_;
    BOOL parseSkip_;

    BOOL debug_;
    BOOL usePointManager_;
}

@property (nonatomic, copy) NSString* publicAppKey;
@property (nonatomic, copy) NSString* apiKey;
@property (nonatomic, copy) NSString* userId;
@property (nonatomic, copy) NSString* mId;
@property (nonatomic, copy) NSString* cId;
@property (nonatomic, copy) NSString* param;
@property (nonatomic, copy) NSString* fromId;

@property (nonatomic, assign) BOOL debug;
@property (nonatomic, assign) BOOL usePointManager;

@property (nonatomic) int delta;
@property (nonatomic) NSInteger orientation;

- (id)initWithTitle:(NSString*)itemTitle buttonTitle:(NSString*)btnTitle tintColor:(UIColor*)navBarColor;
- (void)loadURLString:(NSString *)urlString;
+ (CARMediaViewController *) getInsntance;

@end
