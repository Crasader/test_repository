#include "ApsalarEvent.h"
#include "ApsalarJni.h"

void ApsalarEvent::sendEvent(std::string event_name)
{
  sendEventJni(event_name.c_str());
}
