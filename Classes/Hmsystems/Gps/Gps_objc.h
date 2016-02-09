#import "GpsManager.h"
#import <CoreLocation/CoreLocation.h>

@interface Gps : NSObject <CLLocationManagerDelegate>
{
    GpsDelegate *m_delegate;
    CLLocationManager *_locationManager;
}

@property(readwrite) GpsDelegate *m_delegate;
@property(readwrite) CLLocationManager *_locationManager;

+ (id) sharedGps;
- (void) addDelegate: (GpsDelegate *) delegate;
- (void) requestLocation;

@end
