//  NativeBridge.h
//
//  Created by takataka5845 on 2014/02/24.
//
//

#ifndef __AppActivity__NativeBridge__
#define __AppActivity__NativeBridge__

class NativeBridge {
	public :
	static bool openURL(const char *url);
	static bool checkSerialRewardAndroid(const char *user_master_id, const char *password, bool is_pro);
};

#endif /* defined(__AppActivity__NativeBridge__) */
