//
//  SkillCutInLayer.h
//  war
//
//  Created by HMS on 2015/01/09.
//
//

#ifndef __war__SkillCutInLayer__
#define __war__SkillCutInLayer__

#include "cocos2d.h"
#include "Chara.h"
#include "NormalScene.h"

class SkillCutInLayer : public cocos2d::Layer
{
protected:
	cocos2d::EventListenerTouchOneByOne* listener;
	cocos2d::Sprite* frame;
	float ratio;
	float play_back_time;//再生時間
public:
	CREATE_FUNC(SkillCutInLayer);	//create関数
	virtual bool init();			//init関数
	void setData(int chara_id, string skill_name, float time);
};

#endif /* defined(__war__SkillCutInLayer__) */
