//
//  UserCutInLayer.h
//  war
//
//  Created by HMS on 2015/01/09.
//
//

#ifndef __war__UserCutInLayer__
#define __war__UserCutInLayer__

#include "cocos2d.h"
#include "Chara.h"
#include "NormalScene.h"

class UserCutInLayer : public cocos2d::Layer
{
protected:
	cocos2d::EventListenerTouchOneByOne* listener;
	cocos2d::Sprite* frame;
	float ratio;
	float play_back_time;//再生時間
public:
	CREATE_FUNC(UserCutInLayer);	//create関数
	virtual bool init();			//init関数
	void setData(int chara_id, string user_name, int total_damage, float time);
};

#endif /* defined(__war__UserCutInLayer__) */
