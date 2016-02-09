//
//  SkillErrorLayer.h
//  war
//
//  Created by HMS on 2015/01/20.
//
//

#ifndef __war__SkillErrorLayer__
#define __war__SkillErrorLayer__

#include "cocos2d.h"
#include "Chara.h"

class SkillErrorLayer : public cocos2d::Layer
{
protected:
	cocos2d::EventListenerTouchOneByOne* listener;
	cocos2d::Sprite* frame;
	float ratio;
	float play_back_time;//再生時間
public:
	CREATE_FUNC(SkillErrorLayer);	//create関数
	virtual bool init();			//init関数
	void setData(int error_type, float time);
	
	void popupAction();
	void deleteAction();
};

#endif /* defined(__war__SkillErrorLayer__) */
