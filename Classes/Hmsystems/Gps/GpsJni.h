#include <jni.h>

#ifndef __GPS_JNI_H__
#define __GPS_JNI_H__


class GpsJni {
        
 public :
  static void requestLocationJni();

};

extern "C" {    
  JNIEXPORT void JNICALL Java_jp_pkga_war_gps_Gps_onSuccess(JNIEnv* , jclass, jstring) ;
}

#endif // __GPS_JNI_H__
