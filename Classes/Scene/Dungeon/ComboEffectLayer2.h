//
//  ComboEffectLayer2.h
//  war
//
//  Created by HMS on 2015/02/10.
//
//

#ifndef __war__ComboEffectLayer2__
#define __war__ComboEffectLayer2__

#include "cocos2d.h"
#include "Chara.h"
#include "SuperAnimNodeV2.h"

class ComboEffectLayer2 : public cocos2d::Layer, public SuperAnim::SuperAnimNodeListener
{
protected:
	cocos2d::EventListenerTouchOneByOne* listener;
	cocos2d::Sprite* frame;
	float ratio;
	float play_back_time;//再生時間
public:
	CREATE_FUNC(ComboEffectLayer2);	//create関数
	virtual bool init();			//init関数
	void setData(Chara* chara_l, Chara* chara_r, float time);
};

#endif /* defined(__war__ComboEffectLayer2__) */
