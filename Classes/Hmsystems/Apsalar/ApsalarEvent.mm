#include "ApsalarEvent.h"
#include "Apsalar.h"

void ApsalarEvent::sendEvent(std::string event_name)
{
    [Apsalar event:[NSString stringWithUTF8String:event_name.c_str()]];
}
