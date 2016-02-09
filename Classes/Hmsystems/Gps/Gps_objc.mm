#import "Gps_objc.h"

@implementation Gps

@synthesize m_delegate;
@synthesize _locationManager;

static Gps* _instance = nil;

+ (id)sharedGps
{
    if (!_instance) {
        _instance = [[Gps alloc] init];
    }
    return _instance;
}

- (void) addDelegate: (GpsDelegate *) delegate
{
    m_delegate = delegate;
}

- (void) requestLocation
{
    NSLog(@"ios gps requestLocation()");
    _locationManager = [[CLLocationManager alloc] init];
    
    //位置情報が使えない時
    if ([CLLocationManager locationServicesEnabled] == NO) {
        if(m_delegate)
        {
            m_delegate->onSuccess("");
        }
    }
    
    // 位置取得の要請
    NSString *currentVersion = [[UIDevice currentDevice] systemVersion];
    if([currentVersion compare:@"8.0" options:NSNumericSearch] == NSOrderedAscending){
        
    } else {
        [_locationManager requestWhenInUseAuthorization];
    }
    
    _locationManager.distanceFilter = kCLDistanceFilterNone;
    _locationManager.desiredAccuracy = kCLLocationAccuracyBest;
    _locationManager.delegate = self;
    [_locationManager startUpdatingLocation];
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation *location = [locations lastObject];
    [_locationManager stopUpdatingLocation];
    [self showLocation:location];
}
- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error
{
    NSLog(@"didFailWithError");
    if(m_delegate)
    {
        m_delegate->onSuccess("");
    }
}
- (void)locationManager:(CLLocationManager *)manager didFinishDeferredUpdatesWithError:(NSError *)error
{
    NSLog(@"didFinishDeferredUpdatesWithError");
    if(m_delegate)
    {
        m_delegate->onSuccess("");
    }
}

- (void)showLocation:(CLLocation *)location
{
    //NSString *latitudeStr = [NSString stringWithFormat:@"%f", location.coordinate.latitude];
    //NSString *longitudeStr = [NSString stringWithFormat:@"%f", location.coordinate.longitude];
    //NSString *altitudeStr = [NSString stringWithFormat:@"%f", location.altitude];
    NSString *returnStr = [NSString stringWithFormat:@"%f_%f_%f", location.coordinate.latitude, location.coordinate.longitude, location.altitude];
    
    if(m_delegate)
    {
        m_delegate->onSuccess([returnStr UTF8String]);
    }
}

@end
