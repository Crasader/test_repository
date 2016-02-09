#import "GpsManager.h"
#import "Gps_objc.h"

void GpsManager::setDelegate(GpsDelegate* pDelegate)
{
    [[Gps sharedGps] addDelegate:pDelegate];
}

void GpsManager::requestLocation()
{
    [[Gps sharedGps] requestLocation];
}
